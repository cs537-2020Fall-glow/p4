#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "circleQueue.h"

void cond_init(cond_t * cv) {
  setQueueEmpty(cv);
  initlock(&cv->queue_lock, "queuelock");
  cprintf("condinit(): cv")
}

void cond_wait(cond_t *cv, lock_t *call_lock) {
  if (call_lock->locked == 0) {
    panic("cond_wait(): Calling thread does not have lock!\n");
  }
  if (cv->size >= NTHREAD) {
    panic("cond_wait(): Condition variable queue is full!\n");
  }
  
  
  enqueue(cv, proc->pid);
  
  
}

void cond_signal(cond_t *cv) {
  
}

