#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// P4B
int
sys_clone(void)
{
  void* fcn;
  void* arg;
  void* stack;
  
  if (argptr(0, (void*) &fcn, sizeof(*fcn)) < 0) {
    return -1;
  }
  if (argptr(1, (void*) &arg, sizeof(*arg)) < 0) {
    return -1;
  }
  if (argptr(2, (void*) &stack, sizeof(*stack)) < 0) {
    return -1;
  }
  
  // debug
  cprintf("in sys_clone - fcn: %p, arg: %s, stack: %d\n", fcn, arg, stack);
  
  return clone(fcn, arg, stack);
}

// **stack is used as output so that thread_join can free the stack
int
sys_join(void)
{
  void *stack;
  
  if (argptr(0, (void*) &stack, sizeof(*stack)) < 0) {
    return -1;
  }
  
  return join(&stack);
}

int
sys_cond_init(void)
{
  return -1;
}

int
sys_cond_wait(void)
{
  return -1;
}

int
sys_cond_signal(void)
{
  return -1;
}