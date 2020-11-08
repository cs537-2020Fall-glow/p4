/* check that address space size is updated in threads */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
int global = 0;
unsigned int size = 0;
lock_t lock, lock2;
int num_threads = 8;


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

   int arg = 101;
   void *arg_ptr = &arg;

   lock_init(&lock);
   lock_init(&lock2);
   lock_acquire(&lock);
   lock_acquire(&lock2);

   int i;
   for (i = 0; i < num_threads; i++) {
      int thread_pid = thread_create(worker, arg_ptr);
      assert(thread_pid > 0);
   }

   size = (unsigned int)sbrk(0);
   printf(1, "size1 = %d\n", size);

   while (global < num_threads) {
      lock_release(&lock);
      sleep(100);
      lock_acquire(&lock);
   }

   global = 0;
   sbrk(10000);
   size = (unsigned int)sbrk(0);
   printf(1, "size2 = %d\n", size);
   lock_release(&lock);

   while (global < num_threads) {
      lock_release(&lock2);
      sleep(100);
      lock_acquire(&lock2);
   }
   lock_release(&lock2);


   for (i = 0; i < num_threads; i++) {
      int join_pid = thread_join();
      assert(join_pid > 0);
   }

   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
   lock_acquire(&lock);
   uint temp1 = (unsigned int)sbrk(0);
   printf(1,"worker(): temp size1 = %d size = %d\n", temp1, size);
   assert(temp1 == size);
   //assert((unsigned int)sbrk(0) == size);
   global++;
   lock_release(&lock);

   lock_acquire(&lock2);
   uint temp2 = (unsigned int)sbrk(0);
   printf(1,"worker(): temp size2 = %d size = %d\n", temp2, size);
   assert(temp2 == size);
   //assert((unsigned int)sbrk(0) == size);
   global++;
   lock_release(&lock2);

   exit();
}

