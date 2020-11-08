#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"

// OSTEP pg. 306
void mythread(void *arg);

int main(int argc, char *argv[]) {
  int pid1;
  
  pid1 = thread_create(mythread, "ABC");
  printf(1, "in main: thread created pid: %d\n", pid1);
  thread_join();
  printf(1, "in main: end\n");
  
  exit();
}

void mythread(void *arg) {
  int pid = getpid();
  printf(1, "in mythread: %s\n", arg);
  printf(1, "in mythread: pid %d\n", pid);
  exit();
}