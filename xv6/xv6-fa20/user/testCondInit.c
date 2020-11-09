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
  //  printf(1, "cv->head : %d, cv->tail: %d, cv->size: %d, cv->queue_lock->name: %s\n", cond.head, cond.tail, cond.size, cond.queue_lock.name);
   int thread_pid = thread_create(worker, 0);
   assert(thread_pid > 0);
   exit();
}

void
worker(void *arg_ptr) {
  printf(1, "worker(): \n");
}

