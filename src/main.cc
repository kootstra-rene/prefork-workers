#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <sys/wait.h>

#include "scheduler.h"
#include "trace.h"

size_t _read(int fd, char* buffer, size_t len) { 
  size_t n;
  {
    TRACE();
    n = read(fd, buffer, len);
  }
  if (n) scheduler::yield();
  return n;  
}

void reader(const char* path) {
  TRACE();

  int fd = open(path, O_RDONLY | O_NONBLOCK | O_NDELAY);
  if (-1 == fd) return;

  const int blockSize = 64*1024;
  char *block = new char[blockSize];

  for (;;) {
    auto n = _read(fd, block, blockSize);
    if (n == 0) break;
  }

  delete [] block;
  close(fd);
}

void reader2() {
  reader("../largefile.bin");
}

void idler(int workers) { TRACE();
  while (workers > 0) {
    int   wstatus;
    pid_t workerId = waitpid(-1, &wstatus, WNOHANG);
    if (workerId == -1) break; else
    if (workerId == 0) {
      // main idle loop for processing while task run.
      struct timespec time = {};
      time.tv_sec=0;
      time.tv_nsec=1000000; // 1ms
      nanosleep(&time, nullptr);
      printf(".");
      continue;        
    }
    else {
      printf("worker[%d]: stop:%d\n", workerId, WEXITSTATUS(wstatus));
      workers--;
    }
  }
}

int main(/*int argc, char ** argv*/) { 

  trace::init();
  {
    TRACE();

    const auto totalCPU = 4;

    for (auto i = 0; i < totalCPU; ++i) {
      pid_t workerId = fork();
      if (0 == workerId) {
        scheduler::run([]() { TRACE();
          scheduler::task(reader, "../largefile.bin");
          scheduler::task(reader2);
        });
        return 0;
      }
    }

    // only main thread will end up here.
    scheduler::run([]() { TRACE();
      scheduler::task(idler, totalCPU);
    });
  }
  trace::term();
}
