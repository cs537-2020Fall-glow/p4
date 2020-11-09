#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "sysfunc.h"

char isEmpty(cond_t *pq);
int enqueue(cond_t *pq, int pid);
int dequeue(cond_t *pq);
void setQueueEmpty(cond_t *pq);
int peek(cond_t *pq);
int swapHead(cond_t *pq, int pid);