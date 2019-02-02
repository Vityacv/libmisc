

#define MK_FNV32_OFFSET_BASIS 0x811c9dc5;
#define MK_FNV32_PRIME 16777619;

#ifdef __cplusplus

constexpr static unsigned hash_ct(const char *str) {
  unsigned hash = 0;
  while (*str) {
    hash = hash * 31;
    hash += *str++;
  }

  return hash;
}

constexpr static unsigned hash_ct(wchar_t *str) {
  unsigned hash = 0;
  while (*str) {
    hash = hash * 31;
    hash += *str++;
  }

  return hash;
}

#define obfuscate_strings
#ifdef obfuscate_strings
namespace str_obfuscate {
template <uintptr_t X> struct EnsureCompileTime {
  enum : uintptr_t { Value = X };
};

#ifndef _FORCE_INLINE
#ifdef _MSC_VER /* Visual Studio */
#define _FORCE_INLINE __forceinline
#else
#if defined(__cplusplus) ||                                                    \
    defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L /* C99 */
#ifdef __GNUC__
#define _FORCE_INLINE inline __attribute__((always_inline))
#else
#define _FORCE_INLINE inline
#endif
#else
#define _FORCE_INLINE
#endif /* __STDC_VERSION__ */
#endif /* _MSC_VER */
#endif /* _FORCE_INLINE */

// Use Compile-Time as seed
#define Seed                                                                   \
  ((__TIME__[7] - '0') * 1 + (__TIME__[6] - '0') * 10 +                        \
   (__TIME__[4] - '0') * 60 + (__TIME__[3] - '0') * 600 +                      \
   (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000) +                 \
      (__LINE__ * 100000)

constexpr unsigned LinearCongruentGenerator(unsigned Rounds) {
  return 1013904223 + 1664525 * ((Rounds > 0)
                                     ? LinearCongruentGenerator(Rounds - 1)
                                     : Seed & 0xFFFFFFFF);
}
#define Random() EnsureCompileTime<LinearCongruentGenerator(10)>::Value
#define RandomNumber(Min, Max) (Min + (Random() % (Max - Min + 1)))

template <unsigned... Pack> struct IndexList {};

template <typename IndexList, unsigned Right> struct Append;
template <unsigned... Left, unsigned Right>
struct Append<IndexList<Left...>, Right> {
  typedef IndexList<Left..., Right> Result;
};

template <unsigned N> struct ConstructIndexList {
  typedef
      typename Append<typename ConstructIndexList<N - 1>::Result, N - 1>::Result
          Result;
};
template <> struct ConstructIndexList<0> { typedef IndexList<> Result; };

inline const unsigned char XORKEY = static_cast<char>(RandomNumber(0, 0xFF));
_FORCE_INLINE constexpr char obfuscateChar(const char Character,
                                           unsigned Index) {
  return Character + (XORKEY + Index);
}
_FORCE_INLINE constexpr char returnChar(const char Character, unsigned Index) {
  return Character;
}
template <typename IndexList> class obfuscate;
template <unsigned... Index> class obfuscate<IndexList<Index...>> {
private:
  char Value[sizeof...(Index)];
  char Value2[sizeof...(Index)];
  wchar_t end;

public:
  _FORCE_INLINE constexpr obfuscate(const char *const String)
      : Value{obfuscateChar(String[Index], Index)...} {}

  _FORCE_INLINE char *deobfuscate() {
    deobfuscateFunc(Value, sizeof...(Index), XORKEY);
    return Value;
  }
  _FORCE_INLINE wchar_t *deobfuscateW() {
    deobfuscateWFunc(Value, sizeof...(Index), XORKEY);
    return (wchar_t *)Value;
  }
  static void regcall deobfuscateFunc(char *Value, uintptr_t i,
                                      unsigned char key) {
    Value[i] = 0;
    do {
      i--;
      Value[i] = (unsigned char)(*(unsigned char *)(Value + i) - (key + i));
    } while (i);
  }
  static void regcall deobfuscateWFunc(char *Value, uintptr_t i,
                                       unsigned char key) {
    *(wchar_t *)((wchar_t *)Value + i) = 0;
    do {
      i--;
      *(wchar_t *)((wchar_t *)Value + i) =
          (unsigned char)(*(unsigned char *)(Value + i) - (key + i));
    } while (i);
  }
  char *get() { return Value; }
};
} // namespace str_obfuscate
#undef _T
#define _C(x)                                                                  \
  (str_obfuscate::obfuscate<                                                   \
       str_obfuscate::ConstructIndexList<sizeof(x) - 1>::Result>(x)            \
       .deobfuscate())
#ifdef _UNICODE
#define _T(x)                                                                  \
  (str_obfuscate::obfuscate<                                                   \
       str_obfuscate::ConstructIndexList<sizeof(x) - 1>::Result>(x)            \
       .deobfuscateW())
#else
#define _T(x) _C(x)
#endif
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _T
#if defined(UNICODE)
#define _T(x) (wchar_t *)L##x
#else
#define _T(x) x
#endif
#endif

#ifdef __cplusplus
}
#endif
// not recommended for use
#define c2a(x)                                                                 \
  _conv2a((wchar_t *)x,                                                        \
          (char *)alloca(getUTF16size((wchar_t *)x) + sizeof(char)))
#define c2w(x)                                                                 \
  _conv2w(x, (wchar_t *)alloca(sizeof(wchar_t) * getUTF8size(x) +              \
                               sizeof(wchar_t)))

#ifdef __cplusplus
extern "C" {
#endif
char *regcall _conv2mb(wchar_t *wb); //convert widebyte to multibyte
wchar_t *regcall _conv2wb(char *mb); //convert multibyte to widebyte
wchar_t *regcall _conv2w(char *s, wchar_t *m); // convert string to wide string
wchar_t *regcall _conv2ws(char *s, wchar_t *m,
                          size_t sz);       // convert chars to wide chars
char *regcall _conv2a(wchar_t *s, char *m); // convert wide string to string
char *regcall _conv2as(wchar_t *s, char *m,
                       size_t sz); // convert wide chars to chars
uintptr_t regcall getUTF8size(char *s);
uintptr_t regcall getUTF16size(wchar_t *s);
unsigned regcall hash_rta(char *str);  // hash string
unsigned regcall hash_rtai(char *str); // hash string case-insensitive
unsigned regcall hash_rtas(char *str, uintptr_t sz); // hash chars
unsigned regcall hash_rtasi(char *str,
                            uintptr_t sz); // hash chars case-insensitive
unsigned regcall hash_rtw(wchar_t *str);   // hash wide string
unsigned regcall hash_rtwi(wchar_t *str);  // hash wide string case-insensitive
unsigned regcall hash_rtws(wchar_t *str, uintptr_t sz); // hash wide chars
unsigned regcall hash_rtwsi(wchar_t *str,
                            uintptr_t sz); // hash wide chars case-insensitive
#ifdef __cplusplus
}
#endif
