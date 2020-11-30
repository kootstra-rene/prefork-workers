#pragma once

#include <cstdint>

namespace scheduler {

  void run(void (*method)());

  void addTask(void (*method)());

  void yield();

  extern long totalYields;
  extern void * volatile activeTask;

  int32_t getpid();
}
