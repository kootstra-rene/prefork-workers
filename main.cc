#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include "scheduler.h"
#include "trace.h"

size_t _read(int fd, char* buffer, size_t len) { 
  size_t n;
  {
    trace::CompleteEvent e(__PRETTY_FUNCTION__);
    n = read(fd, buffer, len);
  }
  if (n) scheduler::yield();
  return n;  
}

void reader() {
  trace::CompleteEvent e(__PRETTY_FUNCTION__);

  int fd = open("../largefile.bin", O_RDONLY | O_NONBLOCK | O_NDELAY);
  if (-1 == fd) return;

  const int blockSize = 64*1024;
  char *block = new char[blockSize];

  int32_t hash = 7;
  for (;;) {
    auto n = _read(fd, block, blockSize);
    if (n == 0) break;
    for (size_t i = 0; i < n; i++) {
      hash = hash * 31 + block[i];
    }
  }

  delete [] block;
  fprintf(stdout, "%d\n", hash);
}

int main(/*int argc, char ** argv*/) { 
struct timespec tstart={0,0}, tend={0,0};
clock_gettime(CLOCK_MONOTONIC, &tstart);

  trace::init();
  {
    trace::CompleteEvent e(__PRETTY_FUNCTION__);

    scheduler::run([]() {
      scheduler::addTask(reader);
      scheduler::addTask(reader);
      scheduler::addTask(reader);
      scheduler::addTask([]() {
        reader();
      });
    });

  }
  trace::term();

clock_gettime(CLOCK_MONOTONIC, &tend);
double delta_t = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);

  auto count = scheduler::totalYields;
  fprintf(stdout, "totalYields: #%ld\n", count);
  auto timePerYield = delta_t * (1e09 / count);
  fprintf(stdout, "%fs (%fns)\n", delta_t, timePerYield);
}
