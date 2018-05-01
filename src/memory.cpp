#include "pch.h"
#include <tlhelp32.h>
#include <imagehlp.h>
#include <psapi.h>
#include "memory.h"
#include "conv.h"

template <typename T>
inline void *regcall GetProcAddressByHashT(T pOptHdr, HMODULE hMod,
                                           unsigned hashName) {
  uintptr_t ExportDirVA = pOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
                              .VirtualAddress,
            ExportDirSz =
                pOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
  IMAGE_EXPORT_DIRECTORY *pExportDir =
      (IMAGE_EXPORT_DIRECTORY *)((uintptr_t)hMod + ExportDirVA);
  unsigned int *NameVA =
      (unsigned int *)((uintptr_t)hMod + pExportDir->AddressOfNames);
  intptr_t i = pExportDir->NumberOfNames;
  do {
    i--;
    if(hashName<=0xFFFF) {
      i=hashName;
      i-=pExportDir->Base;
    }
    else if (hash_rta((char *)((uintptr_t)hMod + NameVA[i])) == hashName) {
    } else {
      if (i < 0) break;
      continue;
    }
    {
      unsigned int procAddrVA =
          ((unsigned int *)((uintptr_t)hMod +
                            pExportDir->AddressOfFunctions))[i];
      if (procAddrVA > ExportDirVA && ExportDirVA + ExportDirSz) {
        {
          return (void *)GetProcAddress(hMod,
                                        (char *)((uintptr_t)hMod + NameVA[i]));
        }
      } else {
        return (void *)((uintptr_t)hMod + procAddrVA);
      }
    }
  } while (i > 0);
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

void *regcall GetProcAddressByHash(HMODULE hMod, unsigned hashName) {
  IMAGE_NT_HEADERS *pNtHdr =
      (IMAGE_NT_HEADERS *)((uintptr_t)hMod +
                           (uintptr_t)(((IMAGE_DOS_HEADER *)hMod)->e_lfanew));
  IMAGE_FILE_HEADER *pFileHdr =
      (IMAGE_FILE_HEADER *)((uintptr_t)pNtHdr +
                            FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader));
  IMAGE_OPTIONAL_HEADER *pOptHdr =
      (IMAGE_OPTIONAL_HEADER *)((uintptr_t)pNtHdr +
                                offsetof(IMAGE_NT_HEADERS, OptionalHeader));
  switch (pFileHdr->Machine) {
    case IMAGE_FILE_MACHINE_I386: {
      return GetProcAddressByHashT((IMAGE_OPTIONAL_HEADER32 *)pOptHdr, hMod,
                                   hashName);
    }
    case IMAGE_FILE_MACHINE_AMD64: {
      return GetProcAddressByHashT((IMAGE_OPTIONAL_HEADER64 *)pOptHdr, hMod,
                                   hashName);
    }
  }
  return 0;
}

unsigned char regcall isMemoryExist(void *adr) {
  MEMORY_BASIC_INFORMATION minfo;
  VirtualQuery(adr, &minfo, sizeof(MEMORY_BASIC_INFORMATION));
  return minfo.State != MEM_FREE;
}

unsigned char regcall isModuleExist(uintptr_t mod) {
  TCHAR str[1024];
  return GetModuleFileName((HMODULE)mod, str, 511) != 0;
}

void regcall unprotectCode(unsigned char *adr, unsigned sz) {
  unsigned char *tmp;
  VirtualProtect(adr, sz, PAGE_EXECUTE_READWRITE, (PDWORD)&tmp);
}

void regcall unprotectMem(unsigned char *adr, unsigned sz) {
  unsigned char *tmp;
  VirtualProtect(adr, sz, PAGE_READWRITE, (PDWORD)&tmp);
}

unsigned char *regcall scanBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                 char *pPattStr) {
  int tmp;
  unsigned char *addr = searchBytes(pBuff, pBuffSize, pPattStr);
  if (!addr) {
    DBGLOG("Pattern not found %p %p %s", pBuff, (void *)pBuffSize, pPattStr);
  }
  return addr;
}

// pBuff - scan buffer
// pBuffSize - buffer size
// pPattStr - pattern string, ?? - byte skip (example: 11 22 ?? 33)
unsigned char *regcall searchBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                   char *pPattStr) {
  uintptr_t pPattSize;
  unsigned char pPatt[512], pPattMask[512];
  {
    uintptr_t pCur1 = 0, pCur2 = 0;
    unsigned short uSym = 0;
    do {
      char cSym = pPattStr[pCur1];
      if (pCur1 % 2 == 0 && pCur1 != 0) {
        pPatt[pCur2] = (uSym >> 8) << 4 | uSym;
        uSym = 0;
        pCur2++;
      }
      uSym = uSym << 8;
      if (cSym == '?') {
        pPattMask[pCur2] = 1;
        pCur1 += 2;
        continue;
      } else {
        pPattMask[pCur2] = 0;
        cSym >= 'A' ? cSym -= '7' : cSym -= '0';
      }
      uSym = uSym | cSym;
      pCur1++;
    } while (pPattStr[pCur1 - 1]);
    pPattSize = (pCur1 - 1) / 2;
  }
  {
    unsigned char *pBuffEnd = pBuff + pBuffSize,
                  *pPattEnd = pPattSize + pPatt - 1;
    for (unsigned char *pBuffCur = pBuff; pBuffCur != pBuffEnd; pBuffCur++) {
      unsigned char *bMask = pPatt, *pMask = pPattMask, *pData = pBuffCur;
      for (; pData != pBuffEnd; ++pMask, ++pData, ++bMask) {
        if (*pMask == 0 && *pData != *bMask) break;
        if (bMask == pPattEnd) return pBuffCur;
      }
    }
  }
  return 0;
}

void regcall memswap(unsigned char *src, unsigned char *dst, unsigned len) {
  for (unsigned char buf; len--;) {
    buf = *(unsigned char *)(src + len);
    *(unsigned char *)(src + len) = *(unsigned char *)(dst + len);
    *(unsigned char *)(dst + len) = buf;
  }
}

void * regcall memmem(unsigned char *haystack, size_t haystack_len,
                unsigned char *needle, size_t needle_len)
{
  unsigned char *begin = haystack;
  unsigned char *last_possible = begin + haystack_len - needle_len;
  unsigned char *tail = needle;
  char point;

  /*
   * The first occurrence of the empty string is deemed to occur at
   * the beginning of the string.
   */
  if (needle_len == 0)
    return (void *)begin;

  /*
   * Sanity check, otherwise the loop might search through the whole
   * memory.
   */
  if (haystack_len < needle_len)
    return NULL;

  point = *tail++;
  for (; begin <= last_possible; begin++) {
    if (*begin == point && !memcmp(begin + 1, tail, needle_len - 1))
      return (void *)begin;
  }

  return NULL;
}

void stdcall FreeLibraryThread(void *hDllHandle) {
  FreeLibraryAndExitThread((HMODULE)hDllHandle, 0);
}

void *regcall memcpyl(void *d, const void *s, size_t n) {
  const char *sc, *se;
  char *dc;

  dc = (char *)d;
  sc = (const char *)s;
  se = sc + n;

  if (se != sc) {
    do {
      *dc++ = *sc++;
    } while (sc < se);
  }
  return (unsigned char *)(d) + n;
}

int regcall floatToInt(float x) { return int(x); }

unsigned regcall floatToUInt32(float x) { return unsigned(x); }

int regcall GetRandomInt(int range) {
  if (range == -1)  // check for divide-by-zero case
  {
    return ((rand() % 2) - 1);
  }

  return (rand() % (range + 1));
}

int regcall GetRandomIntRng(int lo, int hi) {
  if ((hi - lo + 1) == 0)  // check for divide-by-zero case
  {
    if (rand() & 1)
      return (lo);
    else
      return (hi);
  }

  return ((rand() % (hi - lo + 1)) + lo);
}

float regcall GetRandomFloat(float min, float max) {
  float randNum = (float)rand() / RAND_MAX;
  float num = min + (max - min) * randNum;
  return num;
}

#ifdef __cplusplus
}
#endif
