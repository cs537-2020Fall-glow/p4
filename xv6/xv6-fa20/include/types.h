#include "param.h"

#ifndef _TYPES_H_
#define _TYPES_H_

// Type definitions

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

// P4B - move struct spinlock def here for cv implementation
// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

// P4B condition variables
typedef struct {
   int pid[NTHREAD]; // store the PIDs in a queue
   int head;
   int tail;
   int size;
   struct spinlock queue_lock;
} cond_t;

// P4B simple spin lock
typedef struct __lock_t {
  uint locked;
} lock_t;


#ifndef NULL
#define NULL (0)
#endif

#endif //_TYPES_H_
