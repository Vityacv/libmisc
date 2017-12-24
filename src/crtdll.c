
#include <stdlib.h>

#ifndef _CRTIMP
#ifdef CRTDLL
#define _CRTIMP __declspec(dllexport)
#else
#ifdef _DLL
#define _CRTIMP __declspec(dllimport)
#else
#define _CRTIMP
#endif
#endif
#endif

#if defined(_MSC_VER)
#define _CRTALLOC(x) __declspec(allocate(x))
#elif defined(__GNUC__)
#define _CRTALLOC(x) __attribute__ ((section (x) ))
#else
#error Your compiler is not supported.
#endif

typedef void(__cdecl *_PVFV)(void);
typedef int(__cdecl *_PIFV)(void);
typedef void(__cdecl *_PVFI)(int);
extern void __cdecl _initterm(_PVFV *,_PVFV *);
extern void __main ();
extern void _pei386_runtime_relocator (void);
extern _CRTALLOC(".CRT$XIA") _PIFV __xi_a[];
extern _CRTALLOC(".CRT$XIZ") _PIFV __xi_z[];
extern _CRTALLOC(".CRT$XCA") _PVFV __xc_a[];
extern _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[];

/* TLS initialization hook.  */
extern const PIMAGE_TLS_CALLBACK __dyn_tls_init_callback;

extern _PVFV *__onexitbegin;
extern _PVFV *__onexitend;

static int pre_c_init (void);

_CRTALLOC(".CRT$XIAA") _PIFV pcinit = pre_c_init;


typedef void (*func_ptr) (void);
extern func_ptr __CTOR_LIST__[];
extern func_ptr __DTOR_LIST__[];

void __do_global_dtors (void);
void __do_global_ctors (void);

void
__do_global_dtors (void)
{
  static func_ptr *p = __DTOR_LIST__ + 1;

  while (*p)
    {
      (*(p)) ();
      p++;
    }
}

void
__do_global_ctors (void)
{
  unsigned long nptrs = (unsigned long) (ptrdiff_t) __CTOR_LIST__[0];
  unsigned long i;

  if (nptrs == (unsigned long) -1)
    {
      for (nptrs = 0; __CTOR_LIST__[nptrs + 1] != 0; nptrs++);
    }

  for (i = nptrs; i >= 1; i--)
    {
      __CTOR_LIST__[i] ();
    }

  atexit (__do_global_dtors);
}

static int
pre_c_init (void)
{
  _PVFV *onexitbegin;

  onexitbegin = (_PVFV *) malloc (32 * sizeof (_PVFV));
  __onexitend = __onexitbegin = (_PVFV *) _encode_pointer (onexitbegin);

  if (onexitbegin == NULL)
    return 1;
  *onexitbegin = (_PVFV) NULL;
  return 0;
}

extern void __main(void);
int mainCRTStartup2 (void);
int mainCRTStartup2 (void){
#ifdef _WIN64
      __mingw_init_ehandler ();
#endif
  _initterm ((_PVFV *)(void *)__xi_a, (_PVFV *)(void *) __xi_z);

  _initterm (__xc_a, __xc_z);
  HMODULE hMod = 0;
  void * lpreserved =0;
    if (__dyn_tls_init_callback != NULL)
  {
    __dyn_tls_init_callback (hMod, DLL_THREAD_ATTACH, lpreserved);
  }
  _pei386_runtime_relocator ();
  __do_global_ctors ();
  wmain (0, 0, 0);
{
    _PVFV * onexitbegin = (_PVFV *) _decode_pointer (__onexitbegin);
    if (onexitbegin)
      {
        _PVFV *onexitend = (_PVFV *) _decode_pointer (__onexitend);
        while (--onexitend >= onexitbegin)
    if (*onexitend != NULL)
      (**onexitend) ();
        free (onexitbegin);
        __onexitbegin = __onexitend = (_PVFV *) NULL;
      }
  return 1;
}
}
