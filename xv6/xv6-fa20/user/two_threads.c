/* process should wait for thread that finishes first */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void *)0)

#define PGSIZE (4096)
int ppid;
int global = 0;
#define assert(x)                             \
  if (x) {                                    \
  } else {                                    \
    printf(1, "%s: %d ", __FILE__, __LINE__); \
    printf(1, "assert failed (%s)\n", #x);    \
    printf(1, "TEST FAILED\n");               \
    kill(ppid);                               \
    exit();                                   \
  }

void worker(void *arg_ptr);
void worker2(void *arg_ptr);

int main(int argc, char *argv[]) {
  ppid = getpid();
  int thread1, thread2;

  assert((thread1 = thread_create(worker, 0)) > 0);
  assert((thread2 = thread_create(worker2, 0)) > 0);
  // printf(1, "ppid: %d, thread1: %d, thread2: %d\n", ppid, thread1, thread2);
  printf(1, "two_threads.c: thread1: %d, thread2: %d\n", thread1, thread2);
  assert(thread_join() == thread2);
  assert(global == 2);
  printf(1, "two_threads.c: joined thread2\n");
  
  int retJoinThread1 = thread_join();
  printf(1, "two_threads.c: retJoinThread1: %d\n", retJoinThread1);
  assert(retJoinThread1 == thread1);
  assert(global == 1);

  printf(1, "TEST PASSED\n");
  exit();
}

void worker(void *arg_ptr) {
  for (int i = 0; i < 100; i++) {
    sleep(1);
    // printf(1, "wait %d\n", i);
  }

  global = 1;
}

void worker2(void *arg_ptr) {
  global = 2;
  ;
}
