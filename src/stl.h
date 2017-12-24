
#ifndef _STL_H
//void *operator new(size_t sz);
//void operator delete(void *addr);

namespace stl {

struct wstring {
  wchar_t *data;
  int length;
  int capacity;
};

struct string {
  wchar_t *data;
  int length;
  int capacity;
};
}  // namespace stl
#define _STL_H
#endif