/* test cond_wait and cond_signal, cannot leave cond_wait without lock */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
int global = 0;
lock_t lock;
cond_t cond;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int
main(int argc, char *argv[])
{
   ppid = getpid();

   printf(1, "cv : 0x%p, lock : 0x%p\n", &cond, &lock);
   
   lock_init(&lock);
   cond_init(&cond);
   int thread_pid = thread_create(worker, 0);
   printf(1, "cond(): thread_pid %d, \n", thread_pid);
   assert(thread_pid > 0);

   sleep(20);
   lock_acquire(&lock);
   global = 2;
   printf(1, "cond(): global %d\n", global);
   cond_signal(&cond);
   sleep(50);
   global = 1;
   printf(1, "cond(): global %d\n", global);
   lock_release(&lock);

   int join_pid = thread_join();
   assert(join_pid == thread_pid);

   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
  lock_acquire(&lock);
  assert(global == 0);
  printf(1, "cond(): global %d\n", global);
  cond_wait(&cond, &lock);
  assert(global == 1);
  printf(1, "cond(): global %d\n", global);
  lock_release(&lock);
}

