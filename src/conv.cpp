#include "pch.h"
#include "memory.h"
#include "conv.h"

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t getUTF8size(char* s) {
  return MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
}

uintptr_t getUTF16size(wchar_t* s) {
  return WideCharToMultiByte(CP_UTF8, 0, s, -1, NULL, 0, NULL, NULL);
}

wchar_t* regcall _conv2w(char* s, wchar_t* m) {
  uintptr_t sz = getUTF8size(s);
  MultiByteToWideChar(CP_UTF8, 0, s, -1, m, sz);
  return m;
}

wchar_t* regcall _conv2ws(char* s, wchar_t* m, size_t sz) {
  MultiByteToWideChar(CP_UTF8, 0, s, -1, m, sz);
  return m;
}

char* regcall _conv2a(wchar_t* s, char* m) {
  uintptr_t sz = getUTF16size(s);
  WideCharToMultiByte(CP_UTF8, 0, s, -1, m, sz, NULL, NULL);
  return m;
}

char* regcall _conv2as(wchar_t* s, char* m, size_t sz) {
  WideCharToMultiByte(CP_UTF8, 0, s, -1, m, sz, NULL, NULL);
  return m;
}

char *regcall _conv2mb(wchar_t *wb) {
  int i = 0;
  do {
    i++;
    *(char *)((char *)wb + i) = *(char *)(wb + i);
  } while (wb[i]);
  return (char *)wb;
}

wchar_t *regcall _conv2wb(char *mb) {
  int i = 0;
  do {
    i++;
  } while (mb[i]);
  do {
    i--;
    *(wchar_t *)((wchar_t *)mb + i) = *(char *)(mb + i);
  } while (i);
  return (wchar_t *)mb;
}

// run-time hash
unsigned hash_rta(char * str)
{
    unsigned hash = 0;
    while (*str)
    {
        hash = hash * 31;
        hash += *str++;
    }

    return hash;
}

unsigned hash_rtai(char * str)
{
    unsigned hash = 0;
    while (*str)
    {
        hash = hash * 31;
        hash += (*str > 'Z' ? *str++ : (*str++ | 0x20));
    }

    return hash;
}

unsigned hash_rtas(char * str,uintptr_t sz)
{
    unsigned hash = 0;
    unsigned i=0;
    while (sz>i)
    {
        hash = hash * 31;
        hash += *str++;
        i++;
    }

    return hash;
}

unsigned hash_rtasi(char * str,uintptr_t sz)
{
    unsigned hash = 0;
    unsigned i=0;
    while (sz>i)
    {
        hash = hash * 31;
        hash += (*str > 'Z' ? *str++ : (*str++ | 0x20));
        i++;
    }

    return hash;
}


unsigned hash_rtw(wchar_t * str)
{
    unsigned hash = 0;
    while (*str)
    {
        hash = hash * 31;
        hash += *str++;
    }

    return hash;
}

unsigned hash_rtwi(wchar_t * str)
{
    unsigned hash = 0;
    while (*str)
    {
        hash = hash * 31;
        hash += (*str > 'Z' ? *str++ : (*str++ | 0x20));
    }

    return hash;
}

unsigned hash_rtws(wchar_t * str,uintptr_t sz)
{
    unsigned hash = 0;
    unsigned i=0;
    while (sz>i)
    {
        hash = hash * 31;
        hash += *str++;
        i++;
    }

    return hash;
}

unsigned hash_rtwsi(wchar_t * str,uintptr_t sz)
{
    unsigned hash = 0;
    unsigned i=0;
    while (sz>i)
    {
        hash = hash * 31;
        hash += (*str > 'Z' ? *str++ : (*str++ | 0x20));
        i++;
    }

    return hash;
}
#ifdef __cplusplus
}
#endif
