#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "circleQueue.h"

void cond_init(cond_t *cv) {
  setQueueEmpty(cv);
  initlock(&cv->queue_lock, "queuelock");
  cprintf("condinit(): pid: %d, head: %d, tail: %d, size: %d, lock name: %s\n", cv->pid[0], cv->head, cv->tail, cv->size, cv->queue_lock.name); // debug
}

void cond_wait(cond_t *cv, lock_t *call_lock) {
  if (call_lock->locked == 0) {
    panic("cond_wait(): Calling thread does not have lock!\n");
  }
  if (cv->size >= NTHREAD) {
    panic("cond_wait(): Condition variable queue is full!\n");
  }
  
  acquire(&cv->queue_lock);
  enqueue(cv, proc->pid);

  xchg(&call_lock->locked, 0); // release calling lock - same as lock_release
  sleep(proc, &cv->queue_lock);
  release(&cv->queue_lock);
  while (xchg(&call_lock->locked, 1) == 1); // spin until calling lock is reacquired
}