/* clone and verify that address space is shared */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

int ppid;
#define PGSIZE (4096)

volatile int global = 1;

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
   printf(1, "create main(): ppid: %d\n", ppid);
   
   int clone_pid = thread_create(worker, 0);
   printf(1, "create main(): clone_pid: %d\n", clone_pid);
   
   assert(clone_pid > 0);
   while(global != 5);
   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
   assert(global == 1);
   global = 5;
}
