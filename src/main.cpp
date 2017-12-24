#include "pch.h"
#include "main.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifdef UNICODE
void wmainCRT()
#else
void mainCRT()
#endif
{
  STARTUPINFO _StartInfo;
  int _argc;
  _TCHAR ** _argv;
  _TCHAR ** _Env;
  int ret;
  if(__tgetmainargs(&_argc,&_argv,&_Env,FALSE,&_StartInfo)==0){
     ret=_tmain(_argc, _argv, 0);
    }
  free(_argv);
  free(_Env);
  exit(ret);
}

void tWinMainCRT()
{
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
  free(_Env);
  ret = _tWinMain(GetModuleHandle(0), 0, cmd, si.dwFlags&STARTF_USESHOWWINDOW ? si.wShowWindow : SW_SHOWDEFAULT);
  }
  exit(ret);
}

#ifdef __cplusplus
}
#endif
