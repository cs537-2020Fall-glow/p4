#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "x86.h"

void lock_init(lock_t *lock) {
  lock->locked = 0;
}

void lock_acquire(lock_t *lock) {
  while(xchg(&lock->locked, 1) != 0)
    ;
}

void lock_release(lock_t *lock) {
  xchg(&lock->locked, 0);
}