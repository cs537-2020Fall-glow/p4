#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

#define PGSIZE		4096		// bytes mapped by a page

// allocate one page for a new thread's stack
int thread_create(void (*start_routine)(void*), void *arg) {
  void *newStack;
  int pid;
  
  // printf(1, "thread_create 1\n"); // debug
  
  newStack = malloc(PGSIZE);
  if (newStack == NULL) {
    return -1;
  }
  
  // printf(1, "thread_create 2\n"); // debug
  // printf(1, "in thread_create - start_routine: %p, arg: %s\n", start_routine, arg);
  
  pid = clone(start_routine, arg, newStack);
  
  return pid;
}

// waits for a child thread that shares the address space with the calling proc
// frees the stack if thread done
int thread_join() {
  void *stack;
  int pid;
  
  pid = join(&stack);
  if (pid != -1) {
    free(stack);
  }
  
  return pid;
}