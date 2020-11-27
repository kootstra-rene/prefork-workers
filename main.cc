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

void reader() {
  TRACE();

  int fd = open("../largefile.bin", O_RDONLY | O_NONBLOCK | O_NDELAY);
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

int main(/*int argc, char ** argv*/) { 

  trace::init();
  {
    TRACE();

    const auto totalCPU = 2;

    for (auto i = 0; i < totalCPU; ++i) {
      pid_t workerId = fork();
      if (0 == workerId) {
        scheduler::run([]() {
          scheduler::addTask(reader);
          scheduler::addTask(reader);
          scheduler::addTask(reader);
          scheduler::addTask([]() {
            reader();
          });
        });
        return 0;
      }
    }

    for (int i = 0; i < totalCPU; ++i) {
      int   wstatus;
      pid_t workerId = wait(&wstatus);

      printf("worker[%d]: stop\n", WEXITSTATUS(wstatus));
    }
  }
  trace::term();

}
