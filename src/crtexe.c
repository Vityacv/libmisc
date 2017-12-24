/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#undef CRTDLL
#ifndef _DLL
#define _DLL
#endif

#define SPECIAL_CRTEXE
#include <stdlib.h>
#include <tchar.h>

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


#ifndef __winitenv
extern wchar_t *** __MINGW_IMP_SYMBOL(__winitenv);
#define __winitenv (* __MINGW_IMP_SYMBOL(__winitenv))
#endif

#if !defined(__initenv) && !defined(__arm__)
extern char *** __MINGW_IMP_SYMBOL(__initenv);
#define __initenv (* __MINGW_IMP_SYMBOL(__initenv))
#endif

/* Hack, for bug in ld.  Will be removed soon.  */
#if defined(__GNUC__)
#define __ImageBase __MINGW_LSYMBOL(_image_base__)
#endif
/* This symbol is defined by ld.  */
extern IMAGE_DOS_HEADER __ImageBase;

extern void _fpreset (void);
#define SPACECHAR _T(' ')
#define DQUOTECHAR _T('\"')

extern int * __MINGW_IMP_SYMBOL(_fmode);
extern int * __MINGW_IMP_SYMBOL(_commode);

#undef _fmode
extern int _fmode;
extern int * __MINGW_IMP_SYMBOL(_commode);
#define _commode (* __MINGW_IMP_SYMBOL(_commode))
extern int _dowildcard;

extern _CRTIMP void __cdecl _initterm(_PVFV *, _PVFV *);

//static int __cdecl check_managed_app (void);

extern _CRTALLOC(".CRT$XIA") _PIFV __xi_a[];
extern _CRTALLOC(".CRT$XIZ") _PIFV __xi_z[];
extern _CRTALLOC(".CRT$XCA") _PVFV __xc_a[];
extern _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[];

/* TLS initialization hook.  */
extern const PIMAGE_TLS_CALLBACK __dyn_tls_init_callback;

extern _PVFV *__onexitbegin;
extern _PVFV *__onexitend;

extern int mingw_app_type;

HINSTANCE __mingw_winmain_hInstance;
_TCHAR *__mingw_winmain_lpCmdLine;
DWORD __mingw_winmain_nShowCmd = SW_SHOWDEFAULT;

static int argc;
extern void __main(void);
#ifdef WPRFLAG
static wchar_t **argv;
static wchar_t **envp;
#else
static char **argv;
static char **envp;
#endif

static int argret;
static int mainret=0;
static int managedapp;
static int has_cctor = 0;
//static _startupinfo startinfo;
extern LPTOP_LEVEL_EXCEPTION_FILTER __mingw_oldexcpt_handler;

extern void _pei386_runtime_relocator (void);
long CALLBACK _gnu_exception_handler (EXCEPTION_POINTERS * exception_data);
#ifdef WPRFLAG
static void duplicate_ppstrings (int ac, wchar_t ***av);
#else
static void duplicate_ppstrings (int ac, char ***av);
#endif

static int __cdecl pre_c_init (void);
_CRTALLOC(".CRT$XIAA") _PIFV mingw_pcinit = pre_c_init;

#ifdef __MINGW_SHOW_INVALID_PARAMETER_EXCEPTION
#define __UNUSED_PARAM_1(x) x
#else
#define __UNUSED_PARAM_1	__UNUSED_PARAM
#endif

static int __cdecl
pre_c_init (void)
{
  __onexitbegin = __onexitend = (_PVFV *) _encode_pointer ((_PVFV *)(-1));

  * __MINGW_IMP_SYMBOL(_fmode) = _fmode;
  * __MINGW_IMP_SYMBOL(_commode) = _commode;

  return 0;
}

static int __tmainCRTStartup (void);

int WinMainCRTStartup (void);

int WinMainCRTStartup (void)
{
  int ret = 255;

  ret = __tmainCRTStartup ();
  return ret;
}

int mainCRTStartup (void);

#ifdef _WIN64
int __mingw_init_ehandler (void);
#endif

int mainCRTStartup (void)
{
  int ret = 255;
  ret = __tmainCRTStartup ();
  return ret;
}

static
__declspec(noinline) int
__tmainCRTStartup (void)
{
  _TCHAR *lpszCommandLine = NULL;
  //STARTUPINFO StartupInfo;
  WINBOOL inDoubleQuote = FALSE;
  //memset (&StartupInfo, 0, sizeof (STARTUPINFO));

  //if (mingw_app_type)
  //  GetStartupInfo (&StartupInfo);
  //{
    void *lock_free = NULL;
    //void *fiberid = ((PNT_TIB)NtCurrentTeb())->StackBase;
    int nested = FALSE;


	_initterm ((_PVFV *)(void *)__xi_a, (_PVFV *)(void *) __xi_z);

	_initterm (__xc_a, __xc_z);

    if (__dyn_tls_init_callback != NULL)
      __dyn_tls_init_callback (NULL, DLL_THREAD_ATTACH, NULL);
    
    _pei386_runtime_relocator ();
    __mingw_oldexcpt_handler = SetUnhandledExceptionFilter (_gnu_exception_handler);
#ifdef _WIN64
    __mingw_init_ehandler ();
#endif

    //_fpreset ();

    __mingw_winmain_hInstance = (HINSTANCE) &__ImageBase;


    __main ();
#ifdef WPRFLAG
    __winitenv = envp;
    mainret = wmain (argc, argv, envp);
#else
#ifndef __arm__
    __initenv = envp;
#endif
    mainret = main (argc, argv, envp);
#endif

  //}
  return mainret;
}

extern int mingw_initltsdrot_force;
extern int mingw_initltsdyn_force;
extern int mingw_initltssuo_force;
extern int mingw_initcharmax;
