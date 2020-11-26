#pragma once

#include <cstddef>

struct state {
  long  regs[8];
  long  id;
};

extern "C" void ut_switch(state *save, state *load);

namespace scheduler {
  void init();
  void yield();
  [[ noreturn ]] void yieldGuard();
  template<typename T> struct state * addTask(const T method);
  void addTask(struct state * const task);

  extern long _id;
  extern long totalYields;
  extern struct state $main;
  extern struct state * volatile activeTask;
}

namespace scheduler {

  template<typename T> int run(const T method) {
    ut_switch(&$main, activeTask = addTask(method));
    return 0;
  }

  template<typename T> struct state * addTask(const T method) {
    long* sstack = new long[512];   // create a 4096 byte stack
    sstack[511] = (long)yieldGuard; // set return address to yieldGuard

    struct state * task = new struct state();
    task->regs[6] = (long)method;
    task->regs[7] = (long)&sstack[511];
    task->id = ++_id;

    scheduler::addTask(task);

    return task;
  }

}
