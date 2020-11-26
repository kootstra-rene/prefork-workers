#pragma once

#include <stdio.h>

struct state {
  long  regs[8];
  long  id;
};

extern "C" void ut_switch(state *save, state *load);

namespace scheduler {
  void init();
  void yield();
  [[ noreturn ]] void yieldGuard();
  template<typename T> struct state * addTask(const T method);
}

extern struct state * volatile activeTask;

namespace scheduler {

  extern long _id;
  extern long totalYields;
  extern struct state $main;

  template<typename T> struct Link {
    Link<T> *   next;
    Link<T> *   prev;
    T *         data;

    Link(T * d, Link<T> *before = NULL) {
      data = d;

      if (before == NULL ) {
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

    void dump() {
      for (auto p = head; p != tail; p = p->next) {
        printf( "%p -> [%p ] -> %p\n", (void *)p->prev, (void *)p, (void *)p->next);
      }
    }
  };

  extern List<struct state> taskList;

  template<typename T> int run(const T method) {
    ut_switch(&$main, activeTask = addTask(method));
    return 0;
  }

  template<typename T> struct state * addTask(const T method) {
    long* sstack = new long[512];   // create a 4096 byte stack
    sstack[511] = (long)yieldGuard; // set return address to yieldGuard

    struct state * task = new struct state();
    task->regs[6] = (long)method;
    task->regs[7] = (long)&sstack[511];
    task->id = ++_id;

    taskList.add(task);

    return task;
  }

}
