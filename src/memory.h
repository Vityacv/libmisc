

#ifdef __cplusplus
extern "C" {
#endif

#define regcall __fastcall
#define stdcall __stdcall
#define cdeclcall __cdecl
#define objcall __thiscall

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
unsigned char *regcall searchBytes2(unsigned char *pBuff, uintptr_t pBuffSize,
                                   unsigned char *pPattStr);
unsigned char *regcall searchBytes3(unsigned char *pBuff, uintptr_t pBuffSize,
                                   unsigned char *pPattStr);
unsigned char *regcall scanBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                 char *pPattStr);
unsigned char *regcall scanBytes2(unsigned char *pBuff, uintptr_t pBuffSize,
                                 unsigned char *pPattStr);

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

#define bytes_search
#ifdef bytes_search
namespace bytes_search{
template <uintptr_t X> struct EnsureCompileTime {
        enum : uintptr_t {
            Value = X
        };
    };
#ifndef _FORCE_INLINE
#  ifdef _MSC_VER    /* Visual Studio */
#    define _FORCE_INLINE __forceinline
#  else
#    if defined (__cplusplus) || defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   /* C99 */
#      ifdef __GNUC__
#        define _FORCE_INLINE inline __attribute__((always_inline))
#      else
#        define _FORCE_INLINE inline
#      endif
#    else
#      define _FORCE_INLINE 
#    endif /* __STDC_VERSION__ */
#  endif  /* _MSC_VER */
#endif /* _FORCE_INLINE */

    template <unsigned... Pack> struct IndexList {};

    template <typename IndexList, unsigned Right> struct Append;
    template <unsigned... Left, unsigned Right> struct Append<IndexList<Left...>, Right> {
        typedef IndexList<Left..., Right> Result;
    };

    template <unsigned N> struct ConstructIndexList {
        typedef typename Append<typename ConstructIndexList<N - 1>::Result, N - 1>::Result Result;
    };
    template <> struct ConstructIndexList<0> {
        typedef IndexList<> Result;
    };
    inline const unsigned char XORKEY = static_cast<char>(EnsureCompileTime<0>::Value);
    _FORCE_INLINE constexpr unsigned char obfuscateChar(unsigned char * mask, size_t masksz,const char c1, const char c2, unsigned Index) {
      if(!Index){
        memset(mask,0,masksz);
      }
      mask[(unsigned)(Index/8)] &= ~(1 << (Index % 8));
      return (c1|c2)+XORKEY;
    }
    _FORCE_INLINE constexpr unsigned char obfuscateCharQ(unsigned char * mask,unsigned Index) {
      mask[(unsigned)(Index/8)] |= 1 << (Index % 8);
      return 0+XORKEY;
    }
    template <typename IndexList> class format;
    template <unsigned... Index> class format<IndexList<Index...> > {
    private:
        unsigned short sz=sizeof...(Index);
        unsigned char Value[sizeof...(Index)];
        unsigned char Mask[((unsigned)(sizeof...(Index)/8)+1)];
    public:
        _FORCE_INLINE constexpr format(const char* const String) : Value {String[Index*2] == '?' ? obfuscateCharQ(Mask,Index) : obfuscateChar(Mask, ((unsigned)(sizeof...(Index)/8)+1),(String[Index*2] >= 'A' ? String[Index*2]-'7' : String[Index*2]-'0')<<4,String[(Index*2)+1] >= 'A' ? String[(Index*2)+1]-'7' : String[(Index*2)+1]-'0', Index)...}{}
        _FORCE_INLINE unsigned char* formatGet() {
          return (unsigned char*)&sz;
        }
    };
  }
  #define BYTES_SEARCH_FORMAT( x ) ( bytes_search::format<bytes_search::ConstructIndexList<(unsigned)((sizeof( x ) - 1)/2)>::Result>( x ).formatGet() )
#endif
