#pragma once

#include <cstddef>

struct state {
  long  regs[8];
  long  id;
};

namespace scheduler {
  void run(void (*boot_strap)(void));
  void yield();
  [[ noreturn ]] void yieldGuard();
  void addTask(struct state * const task);

  extern long _id;
  extern long totalYields;
  extern struct state $main;
  extern struct state * volatile activeTask;
}

namespace scheduler {

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
