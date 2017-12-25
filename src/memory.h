

#ifdef __cplusplus
extern "C" {
#endif

#define regcall __fastcall
#define stdcall __stdcall
#define cdeclcall __cdeclcall


#define exc asm volatile("int3")

#ifdef DEBUG
#define DBGLOG(fmt, ...)                            \
  {                                                 \
    char str[1024];                                 \
    wchar_t buf[512];                               \
    sprintf(str, (const char *)fmt, ##__VA_ARGS__); \
    _conv2w(str, buf);                              \
    OutputDebugString(buf);                         \
  }

#else
#define DBGLOG(...) ((void)0)
#endif

void *regcall GetProcAddressByHash(HMODULE hMod, unsigned hashName);
#define GetProcAddr(hMod, name) GetProcAddressByHash(hMod, hash_ct(name))

#define GetModuleSize(x)                                               \
  (((PIMAGE_NT_HEADERS)((BYTE *)x + ((PIMAGE_DOS_HEADER)x)->e_lfanew)) \
       ->OptionalHeader.SizeOfImage)
void regcall memswap(unsigned char *src, unsigned char *dst, unsigned len);
void *regcall memcpyl(void *d, const void *s, size_t n);
void * regcall memmem(unsigned char *haystack, size_t haystack_len,
                unsigned char *needle, size_t needle_len);

void stdcall FreeLibraryThread(void *hDllHandle);

unsigned char *regcall searchBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                   char *pPattStr);
unsigned char *regcall scanBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                 char *pPattStr);

unsigned char regcall isMemoryExist(void *adr);
unsigned char regcall isModuleExist(uintptr_t mod);


int regcall floatToInt(float x);
unsigned regcall floatToUInt32(float x);
int regcall GetRandomInt(int range);

int regcall GetRandomIntRng(int lo, int hi);
float regcall GetRandomFloat(float min, float max);

#define X86_PAGE_SIZE 0x1000
void regcall unprotectCode(unsigned char *adr, unsigned sz = X86_PAGE_SIZE);

void regcall unprotectMem(unsigned char *adr, unsigned sz = X86_PAGE_SIZE);

#ifdef __cplusplus
}
#endif
