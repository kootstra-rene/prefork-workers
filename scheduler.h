#pragma once

namespace scheduler {

  void run(void (*method)());

  void addTask(void (*method)());

  void yield();

  extern long totalYields;
  extern void * volatile activeTask;
}