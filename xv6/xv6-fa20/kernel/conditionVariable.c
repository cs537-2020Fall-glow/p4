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
  
  cprintf("cond_wait: proc->pid: %d\n", proc->pid); // debug
  cprintf("cond_wait(): pid: %d, head: %d, tail: %d, size: %d, lock name: %s\n", cv->pid[0], cv->head, cv->tail, cv->size, cv->queue_lock.name); // debug
  
  acquire(&cv->queue_lock);
  cprintf("cond_wait(): after acquire\n");
  enqueue(cv, proc->pid);
  cprintf("cond_wait(): after enqueue\n");
  cprintf("cond_wait(): pid: %d, head: %d, tail: %d, size: %d, lock name: %s\n", cv->pid[0], cv->head, cv->tail, cv->size, cv->queue_lock.name); // debug
  // release(&cv->queue_lock);
  sleep(proc, &cv->queue_lock);
  cprintf("cond_wait(): after sleep\n");
  xchg(&call_lock->locked, 0); // this is a duplicate of lock_release
  cprintf("cond_wait(): after release calling lock\n");
}