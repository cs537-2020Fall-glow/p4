# P4
## P4B: xv6 Kernel Threads
### Boilerplate
- kernel setup for new system calls
  - syscall.c, sysfunc.h, syscall.h: 
    - addedsys_clone, sys_join, sys_cond_init, sys_cond_wait, sys_cond_signal
- user setup
  - clone, join, thread_create, thread_join, lock_init, lock_acquire,
    lock_release, cond_init, cond_wait, cond_signal
  - user.h: function prototypes
  - usys.S: also add system calls here
  
###
- sysproc.c: sys_* definitions
- stackHigh: new proc struct member to track the high stack address for threads

### thread library
- thread.c: thread_create and thread_join definitions
- thread.o added to user makefile in USER_LIBS section

### other system calls
#### wait
  - ignore threads of the same process by checking if the pgdir values are equal
#### sbrk and growproc
- update sz for all threads in same process
  - by looping through proc table similar to wait() and join()
    
    
### cond
- param.h
- circlequeue.h, circlequeue.c

## P4A: Scalable Web Server