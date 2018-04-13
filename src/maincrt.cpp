#include "pch.h"
#include "main.h"


#ifdef __cplusplus
extern "C" {
#endif
typedef void(__cdecl *_PVFV)();
typedef int(__cdecl *_PIFV)(void);
typedef void(__cdecl *_PVFI)(int);
extern _PVFV *__onexitbegin;
extern _PVFV *__onexitend;
extern _PIFV __xi_a[], __xi_z[];
extern _PVFV __xc_a[], __xc_z[], __xp_a[], __xp_z[], __xt_a[], __xt_z[];

typedef void (*func_ptr) (void);
extern func_ptr __CTOR_LIST__[];
extern func_ptr __DTOR_LIST__[];

extern const PIMAGE_TLS_CALLBACK __dyn_tls_init_callback;
extern void _pei386_runtime_relocator (void);

void __cdecl _initterm(_PVFV *, _PVFV *);

static int
pre_c_init (void)
{
  _PVFV *onexitbegin;

  onexitbegin = (_PVFV *) malloc (32 * sizeof (_PVFV));
  __onexitend = __onexitbegin = (_PVFV *) (onexitbegin);

  if (onexitbegin == NULL)
    return 1;
  *onexitbegin = (_PVFV) NULL;
  return 0;
}

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

inline void onattach(HMODULE hMod,void * pReserved){
  pre_c_init();
  _initterm ((_PVFV *)(void *)__xi_a, (_PVFV *)(void *) __xi_z);
  _initterm(__xc_a, __xc_z);
  if (__dyn_tls_init_callback != NULL)
  {
    __dyn_tls_init_callback (hMod, DLL_THREAD_ATTACH, pReserved);
  }
  _pei386_runtime_relocator ();
  __do_global_ctors ();
}

inline void ondetach(){
    _PVFV * onexitbegin = (_PVFV *)  (__onexitbegin);
    if (onexitbegin)
      {
        _PVFV *onexitend = (_PVFV *)  (__onexitend);
        while (--onexitend >= onexitbegin)
    if (*onexitend != NULL)
      (**onexitend) ();
        free (onexitbegin);
        __onexitbegin = __onexitend = (_PVFV *) NULL;
      }
}


#ifdef UNICODE
void wmainCRT()
#else
void mainCRT()
#endif
{
  onattach(0,0);
  STARTUPINFO _StartInfo;
  int _argc;
  _TCHAR ** _argv;
  _TCHAR ** _Env;
  int ret;
  if(__tgetmainargs(&_argc,&_argv,&_Env,FALSE,&_StartInfo)==0){
     ret=_tmain(_argc, _argv, 0);
    }
  free(_argv);
  //free(_Env);
  ondetach();
  exit(ret);
}

void tWinMainCRT()
{
  onattach(0,0);
  STARTUPINFO si;
  si.dwFlags = 0;
  STARTUPINFO _StartInfo;
  int _argc;
  _TCHAR ** _argv;
  _TCHAR ** _Env;
  int ret;
  if(__tgetmainargs(&_argc,&_argv,&_Env,FALSE,&_StartInfo)==0){
  TCHAR *cmd=GetCommandLine();
  cmd=cmd+_tcslen(_argv[0])+2;
  //if(_argc!=1)cmd=cmd+1;
  while(*cmd==_T(' '))cmd++;
  free(_argv);
  ret = _tWinMain(GetModuleHandle(0), 0, cmd, si.dwFlags&STARTF_USESHOWWINDOW ? si.wShowWindow : SW_SHOWDEFAULT);
  }
  ondetach();
  exit(ret);
}

BOOL WINAPI DllMainCRT(HINSTANCE hInst, DWORD reason, LPVOID imp)
{
  if (reason == DLL_PROCESS_ATTACH)
  {
    onattach(hInst,imp);
  }

  BOOL ret = DllMain(hInst, reason, imp);

  if (reason == DLL_PROCESS_DETACH)
  {
    ondetach();
  }

  return ret;
}

#ifdef __cplusplus
}
#endif
