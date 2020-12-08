#include <cstddef>
#include <unistd.h>

#include "scheduler.h"

asm(
  ".text\n"
  ".globl ut_switch\n"
  "ut_switch:\n"

  "movq 0(%rsp), %rdx\n"
  "leaq 8(%rsp), %rcx\n"

  "movq %r12,  0(%rdi)\n"
  "movq %r13,  8(%rdi)\n"
  "movq %r14, 16(%rdi)\n"
  "movq %r15, 24(%rdi)\n"

  "movq  0(%rsi), %r12\n"
  "movq  8(%rsi), %r13\n"
  "movq 16(%rsi), %r14\n"
  "movq 24(%rsi), %r15\n"

  "movq %rbx, 32(%rdi)\n"
  "movq %rbp, 40(%rdi)\n"
  "movq %rdx, 48(%rdi)\n"
  "movq %rcx, 56(%rdi)\n"
  "movq %rdi, 64(%rdi)\n"

  "movq 32(%rsi), %rbx\n"
  "movq 40(%rsi), %rbp\n"
  "movq 48(%rsi), %rdx\n"
  "movq 56(%rsi), %rcx\n"
  "movq 64(%rsi), %rdi\n"

  "movq %rcx, %rsp\n"
  "jmp *%rdx\n"
);

extern "C" void ut_switch(void *save, void *load);

namespace scheduler {

  struct state {
    long  regs[9];
    long* stack;
  };

  template<typename T> struct Link {
    Link<T> *   next;
    Link<T> *   prev;
    T *         data;

    explicit Link(T * d, Link<T> *before = NULL) {
      data = d;

      if (before == NULL) {
        next = prev = this;
      }
      else {
        next = before->next;
        prev = before;

        before->next->prev = this;
        before->next = this;
      }
    }

    ~Link() {
      delete data;
    }
  };

  template<typename T> struct List {
    Link<T> * head;
    Link<T> * tail;
    Link<T> * curr;
    long      total;

    explicit List() {
      head = tail = NULL;
    }

    void add(T* task) {
      if (head == NULL) {
        Link<T> * link = new Link<T>(task);
        head = tail = curr = link;
      }
      else {
        Link<T> * link = new Link<T>(task, tail);
        tail = link;
      }
      ++total;
    }

    void del() {
      Link<T> * elem = curr;

      if (curr == head && curr == tail) {
        head = tail = NULL;
      }
      else
      if (curr == head) {
        head = elem->next;
      }
      else
      if (curr == tail) {
        tail = elem->prev;
      }

      elem->prev->next = elem->next;
      elem->next->prev = elem->prev;
      curr = elem->next;
      delete elem;
      --total;
    }

  };

  int32_t _pid = -1;
  long totalYields = 0;
  struct state $main;

  List<struct state> taskList;
  void * volatile activeTask = NULL;

  void yield() {
    taskList.curr = taskList.curr->next;
    auto callerTask = activeTask;
    activeTask = taskList.curr->data;
    totalYields++;

    ut_switch(callerTask, activeTask);
  }

  [[ noreturn ]] void yieldGuard() {
    taskList.del();
    // destroy the stack, should be fine because there is no preemption.
    delete [] ((struct state *)activeTask)->stack;

    if (!taskList.total) {
      ut_switch(activeTask, &$main);
    }

    scheduler::yield();
    throw 0xdead;
  }

  void run(void (*boot_strap)(void)) {
    boot_strap();

    activeTask = taskList.head->data;
    ut_switch(&$main, activeTask);
    activeTask = NULL;
  }

  void addTask(void (*method)(), void* self) {
    long* sstack = new long[512];   // create a 4096 byte stack
    sstack[511] = (long)yieldGuard; // set return address to yieldGuard

    struct state * task = new struct state();
    task->regs[6] = (long)method;
    task->regs[7] = (long)&sstack[511];
    task->regs[8] = (long)self;
    task->stack = sstack;

    taskList.add(task);
  }

  int32_t getpid() {
    if (-1 == _pid) {
      _pid = ::getpid();
    }
    return _pid;
  }
}
