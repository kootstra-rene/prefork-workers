#pragma once

#include <cstdint>

#include "traits.h"

#define A0 typename traits<T>::A0
#define A1 typename traits<T>::A1
#define A2 typename traits<T>::A2
#define A3 typename traits<T>::A3
#define RT typename traits<T>::RT
#define SG typename traits<T>::SG

namespace scheduler {

  void run(void (*method)());

  void addTask(void (*method)(), void* self = nullptr);

  void yield();

  extern long totalYields;
  extern void * volatile activeTask;

  int32_t getpid();

  template<typename T> struct invoker {

    explicit invoker(T func, A0 _a, A1 _b, A2 _c, A3 _d): m(func), a(_a), b(_b), c(_c), d(_d) {
    }

    SG m;
    A0 a;
    A1 b;
    A2 c;
    A3 d;

    void invoke() {
      invoker<T>& i = *this;
      switch ((int)traits<T>::Arguments) {
        case 4: unsafe_cast<RT(*)(A0,A1,A2,A3)>(i.m)(i.a, i.b, i.c, i.d);  break;
        case 3: unsafe_cast<RT(*)(A0,A1,A2)>(i.m)(i.a, i.b, i.c);  break;
        case 2: unsafe_cast<RT(*)(A0,A1)>(i.m)(i.a, i.b);  break;
        case 1: unsafe_cast<RT(*)(A0)>(i.m)(i.a);  break;
        case 0: unsafe_cast<RT(*)()>(i.m)();  break;
      }

      delete &i;
    }
  };

  template <typename T> void task(T func, A0 a = _(), A1 b = _(), A2 c = _(), A3 d = _()) {
    auto i = new invoker<T>(func, a, b, c, d);

    addTask(unsafe_cast<void(*)(void)>(&invoker<T>::invoke), i);
  }
}

#undef SG
#undef RT
#undef A3
#undef A2
#undef A1
#undef A0
