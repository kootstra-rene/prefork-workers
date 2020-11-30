#include <fcntl.h>
#include <unistd.h>
#include <string>

#include "trace.h"

namespace trace {

  static int fd = -1;

  int log(std::string&& text) {
    return ::write(fd, text.c_str(), text.size());
  }

  void init(void) {
    fd = open("log/trace.json", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (-1 == fd) return;

    log("{\"otherData\": {},\"traceEvents\":[{}\n");
  }

  void term(void) {
    log("]}\n");
  }

  char CompleteEvent::buffer[128];
}
