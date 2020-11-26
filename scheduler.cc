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

  "movq 32(%rsi), %rbx\n"
  "movq 40(%rsi), %rbp\n"
  "movq 48(%rsi), %rdx\n"
  "movq 56(%rsi), %rcx\n"

  "movq %rcx, %rsp\n"
  "jmp *%rdx\n"
);

namespace scheduler {

  template<typename T> struct Link {
    Link<T> *   next;
    Link<T> *   prev;
    T *         data;

    Link(T * d, Link<T> *before = NULL) {
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

  };

  template<typename T> struct List {
    Link<T> * head;
    Link<T> * tail;
    Link<T> * curr;
    long      total;

    List() {
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

  long _id = 0;
  long totalYields = 0;
  struct state $main;

  List<struct state> taskList;
  struct state * volatile activeTask = NULL;

  void init() {
  } 

  void yield() {
    taskList.curr = taskList.curr->next;
    auto callerTask = activeTask;
    activeTask = taskList.curr->data;
    totalYields++;

    ut_switch(callerTask, activeTask);
  }

  [[ noreturn ]] void yieldGuard() {
    taskList.del();

    if (!taskList.total) {
      ut_switch(activeTask, &$main);
    }

    scheduler::yield();
    throw 0xdead;
  }

  void addTask(struct state * const task) {
    taskList.add(task);
  }

}