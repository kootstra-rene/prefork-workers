#pragma once

#include <time.h>
#include <string>
#include "scheduler.h"

// for chrome://tracing
namespace trace {

  void init(void);
  void term(void);
  int log(std::string&& text);

  class CompleteEvent {
  public:
    explicit CompleteEvent(const char* name) : _name(name) {
      clock_gettime(CLOCK_MONOTONIC, &_start);
      _stop = {0,0};
    }

    ~CompleteEvent() {
      clock_gettime(CLOCK_MONOTONIC, &_stop);
      double start = ((double)_start.tv_sec + 1.0e-9*_start.tv_nsec);
      double stop = ((double)_stop.tv_sec + 1.0e-9*_stop.tv_nsec);
      double delta_t = stop - start;

      sprintf(buffer, ",{\"dur\":%ld, \"name\":\"%s\",\"ph\":\"X\",\"pid\":%d,\"tid\":%ld,\"ts\":%ld}\n", long(delta_t*1.0e6), _name, scheduler::getpid(), (long)scheduler::activeTask, long(start*1.0e6));
      log(buffer);
    }

  private:
    const char * _name;
    struct timespec _start, _stop;
    static char buffer[128];
  };

  #define TRACE()   trace::CompleteEvent e##__LINE__(__PRETTY_FUNCTION__);
}