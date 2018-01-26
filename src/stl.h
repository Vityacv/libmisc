
#ifndef _STL_H
//void *operator new(size_t sz);
//void operator delete(void *addr);

namespace stl {

struct wstring {
  wchar_t *data;
  int length;
  int capacity;
  wchar_t str[16];
};

struct string {
  char *data;
  int length;
  int capacity;
  char str[16];
};
}  // namespace stl
#define _STL_H
#endif