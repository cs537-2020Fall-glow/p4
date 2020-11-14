#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"

void mythread(void *arg);

int main(int argc, char *argv[]) {
  int pid1;
  
  pid1 = thread_create(mythread, NULL);
  printf(1, "in main: thread created pid: %d\n", pid1);
  thread_join();
  printf(1, "in main: end\n");
  
  exit();
}

void mythread(void *arg) {
  int pid = getpid();
  printf(1, "in mythread: %s\n", arg);
  printf(1, "mythread %d called with null arg\n", pid);
  exit();
}