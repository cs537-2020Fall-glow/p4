#include "circleQueue.h"
#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "sysfunc.h"


/**
 * Returns 1 if the set is currently empty, 0 otherwise
 */
char isEmpty(cond_t *pq) { return (pq->head == -1 || pq->tail == -1); }

/**
 * Add to the back of the queue by inserting after tail node
 */
int enqueue(cond_t *pq, int pid) {
  if (pq->size >= NTHREAD) {
    return -1;
  }
  
  if (isEmpty(pq)) {
    pq->head++;
    pq->tail++;
  } else {
    pq->tail = (pq->tail + 1) % NTHREAD;
  }
    
  pq->pid[pq->tail] = pid;
  pq->size++;
  // cprintf("enqueue() h %d t %d s %d hpid %d tpid %d\n", pq->head, pq->tail, pq->size, pq->pid[pq->head], pq->pid[pq->tail]);
  return pid;
}

/**
 * Remove from the front of the queue by removing the head node
 */
int dequeue(cond_t *pq) {
  // cprintf("dequeue() h %d t %d s %d pid %d\n", pq->head, pq->tail, pq->size, pq->pid[pq->head]);
  if (isEmpty(pq)) {
    return -1;
  }

  int pidDequeue = pq->pid[pq->head];
  pq->pid[pq->head] = 0;

  if (pq->head == pq->tail) {
    setQueueEmpty(pq);
    // cprintf("setQueueEmpty() h %d t %d s %d pid %d\n", pq->head, pq->tail, pq->size, pq->pid[pq->head]);
  } else {
    pq->head = (pq->head + 1) % NTHREAD;
  }
  
  pq->size--;
  return pidDequeue;
}

void setQueueEmpty(cond_t *pq) {
  cprintf("setQueueEmpty() h %d t %d s %d pid %d\n", pq->head, pq->tail, pq->size, pq->pid[pq->head]);
  pq->head = -1;
  pq->tail = -1;
  pq->size = 0;
  for (int i = 0; i < NTHREAD; i++) {
    pq->pid[i] = 0;
  }
  cprintf("setQueueEmpty() h %d t %d s %d pid %d\n", pq->head, pq->tail, pq->size, pq->pid[pq->head]);
}

/**
 * Returns pid of front of queue without dequeuing
 */ 
int peek(cond_t *pq) {
  if (isEmpty(pq))
    return -1;
  return pq->pid[pq->head];
}

/**
 * Move the specified pid to the head where it can be dequeued if needed
 * 
 * Returns:
 * -1 on error
 * Otherwise, returns pid of the old head that was swapped.
 */
int swapHead(cond_t *pq, int pid) {
  if (isEmpty(pq))
    return -1;
  
  // pid already at head if only item in line
  if (pq->size == 1)
    return pid;
  
  // find pid in PQ
  int currIndex = -1;
  for(int i = 0; i < NTHREAD; i++) {
    if (pq->pid[i] == pid) {
      currIndex = i;
      break;
    }
  }
  if (currIndex == -1) { // not found
    return -1; 
  }
  
  // swap
  int temp = pq->pid[pq->head];
  pq->pid[pq->head] = pid;
  pq->pid[currIndex] = temp;
  
  return temp;
}