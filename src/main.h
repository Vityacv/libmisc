#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#ifdef UNICODE
#define __tgetmainargs __wgetmainargs
int __cdecl __wgetmainargs(int *_Argc, WCHAR ***_Argv, WCHAR ***_Env,
                           int _DoWildCard, STARTUPINFO *_StartInfo);
#else
#define __tgetmainargs __getmainargs
int __cdecl __getmainargs(int *_Argc, char ***_Argv, char ***_Env,
                          int _DoWildCard, STARTUPINFO *_StartInfo);
#endif

int _tmain(int, TCHAR **, TCHAR **);

BOOLEAN WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);


#ifdef __cplusplus
}
#endif
