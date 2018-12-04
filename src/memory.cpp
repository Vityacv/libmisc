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
  uintptr_t funcAddress = pExportDir->AddressOfFunctions;
  uintptr_t ordAddress = pExportDir->AddressOfNameOrdinals;
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
      unsigned short NameOrdinal =
          ((unsigned short *)((uintptr_t)hMod +
                              ordAddress))[i];
      unsigned int procAddrVA =
          ((unsigned int *)((uintptr_t)hMod +
                            funcAddress))[NameOrdinal];
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

void regcall unprotectCode(uint8_t *adr, unsigned sz) {
  uint8_t *tmp;
  VirtualProtect(adr, sz, PAGE_EXECUTE_READWRITE, (PDWORD)&tmp);
}

void regcall unprotectMem(uint8_t *adr, unsigned sz) {
  uint8_t *tmp;
  VirtualProtect(adr, sz, PAGE_READWRITE, (PDWORD)&tmp);
}

uint8_t *regcall searchBytes3(uint8_t *pBuff, uintptr_t pBuffSize,
                                   uint8_t *pPattBuf) {
  size_t pPattSize=*(unsigned short*)(pPattBuf);
  pPattBuf+=2;
  uint8_t * pPattMaskBuf=pPattBuf+pPattSize,
  * pPattEnd = pPattBuf + pPattSize,
  * pBuffEnd = pBuff + pBuffSize;
    for (uint8_t *pBuffCur = pBuff; pBuffCur != pBuffEnd; pBuffCur++) {
      if(*pBuffCur == *pPattBuf)
      {
        uint8_t *bMask = pPattBuf, *pData = pBuffCur;
        uintptr_t i=0;
        while (pData != pBuffEnd) {
          ++i, ++pData, ++bMask;
          if (bMask == pPattEnd) return pBuffCur; 
          if ((pPattMaskBuf[(uintptr_t)(i/8)] & (1 << (i % 8)))==0 && *pData != *bMask) break;
        }
      }
    }
  return 0;
}


uint8_t *regcall scanBytes(uint8_t *pBuff, uintptr_t pBuffSize,
                                 uint8_t *pPattBuf) {
  int tmp;
  uint8_t *addr = searchBytes3(pBuff, pBuffSize, pPattBuf);
  //if (!addr) {
  //  DBGLOG("Pattern not found %p %p %s", pBuff, (void *)pBuffSize, pPattBuf);
  //}
  return addr;
}

uint8_t *searchSkipBytes(uint8_t *pBuff, uintptr_t pBuffSize, ...) {
  va_list vl;
  uint8_t *val;
  uint8_t *current = pBuff, *result = pBuff;
  while (1) {
    va_start(vl, 0);
    int i = 0;
    uintptr_t skip;
    do {
      uint8_t *pPatt = va_arg(vl, uint8_t *), *tmp;
      uintptr_t pPattSz = *(unsigned short *)pPatt;
      if (i)
        tmp = scanBytes(current, skip + pPattSz, pPatt);
      else
        tmp = scanBytes(result + 1, (pBuffSize + pBuff) - (result + 1), pPatt);
      skip = va_arg(vl, uintptr_t);
      if (!i) {
        if (tmp) {
          i++;
          current = tmp + pPattSz;
          result = tmp;
        } else
          return 0;
      }else current = tmp + pPattSz;
      if (!tmp)
        break;
      if (!skip && tmp)
        return result;
    } while (skip);
    va_end(vl);
  }
}


uint8_t *regcall scanBytes2(uint8_t *pBuff, uintptr_t pBuffSize,
                                 char *pPattStr) {
  int tmp;
  uint8_t *addr = searchBytes(pBuff, pBuffSize, pPattStr);
  if (!addr) {
    DBGLOG("Pattern not found %p %p %s", pBuff, (void *)pBuffSize, pPattStr);
  }
  return addr;
}

uint8_t *regcall searchBytes2(uint8_t *pBuff, uintptr_t pBuffSize,
                                   uint8_t *pPattBuf) {
  uintptr_t pPattSize=*(unsigned short*)(pPattBuf);
  uint8_t * pPatt=(pPattBuf+sizeof(unsigned short));
  uint8_t * pPattMaskBuf=pPatt+pPattSize;
  unsigned char pPattMask[512];
  for(uintptr_t i = 0;i!=pPattSize;i++){
    pPattMask[i] = pPattMaskBuf[(unsigned)(i/8)] & (1 << (i % 8));
  }
  {
    uint8_t *pBuffEnd = pBuff + pBuffSize,
                  *pPattEnd = pPattSize + pPatt - 1;
    for (uint8_t *pBuffCur = pBuff; pBuffCur != pBuffEnd; pBuffCur++) {
      uint8_t *bMask = pPatt, *pMask = pPattMask, *pData = pBuffCur;
      for (; pData != pBuffEnd; ++pMask, ++pData, ++bMask) {
        if (*pMask == 0 && *pData != *bMask) break;
        if (bMask == pPattEnd) return pBuffCur;
      }
    }
  }
  return 0;
}
// pBuff - scan buffer
// pBuffSize - buffer size
// pPattStr - pattern string, ?? - byte skip (example: 11 22 ?? 33)
uint8_t *regcall searchBytes(uint8_t *pBuff, uintptr_t pBuffSize,
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
    uint8_t *pBuffEnd = pBuff + pBuffSize,
                  *pPattEnd = pPattSize + pPatt - 1;
    for (uint8_t *pBuffCur = pBuff; pBuffCur != pBuffEnd; pBuffCur++) {
      uint8_t *bMask = pPatt, *pMask = pPattMask, *pData = pBuffCur;
      for (; pData != pBuffEnd; ++pMask, ++pData, ++bMask) {
        if (*pMask == 0 && *pData != *bMask) break;
        if (bMask == pPattEnd) return pBuffCur;
      }
    }
  }
  return 0;
}

void regcall memswap(uint8_t *src, uint8_t *dst, unsigned len) {
  for (unsigned char buf; len--;) {
    buf = *(uint8_t *)(src + len);
    *(uint8_t *)(src + len) = *(uint8_t *)(dst + len);
    *(uint8_t *)(dst + len) = buf;
  }
}

void * regcall memmem(uint8_t *haystack, size_t haystack_len,
                uint8_t *needle, size_t needle_len)
{
  uint8_t *begin = haystack;
  uint8_t *last_possible = begin + haystack_len - needle_len;
  uint8_t *tail = needle;
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
  return (uint8_t *)(d) + n;
}

 int bufprintw(wchar_t * buf, size_t sz,const wchar_t * fmt, ...){
   int val = 0;
   va_list ap;
   va_start(ap, fmt);
   val = swprintf((wchar_t *)buf, sz, fmt,ap);
   va_end(ap);
   return val;
}

 int bufprint(char * buf, size_t sz,const char * fmt, ...){
   int val = 0;/*
   va_list ap;
   va_start(ap, fmt);
   val = vsprintf((char *)buf, sz, fmt, ap);
   va_end(ap);*/
   return val;
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
