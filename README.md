# P4
## P4B: xv6 Kernel Threads
### Boilerplate
- kernel setup for new system calls
  - sys_clone, sys_join, sys_cond_init, sys_cond_wait, sys_cond_signal
  - changes in: syscall.c, sysfunc.h, syscall.h
- user setup
  - clone, join, thread_create, thread_join, lock_init, lock_acquire,
    lock_release, cond_init, cond_wait, cond_signal
  - prototypes in: in user.h
  - also add system calls to usys.S
  
###
- sys_* definitions in sysproc.c


### thread library
- thread_create and thread_join defined in thread.c
- thread.o added to user makefile in USER_LIBS section

### other system calls
- wait()
  - ignore threads of the same process by checking if the pgdir values are equal