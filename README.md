# P4A: Scalable Web Server
## linux/Makefile
- Added pthread* wrappers to cs537.c, make for clients needs -lpthread
## linux/cs537.c
- Add pthread* wrappers
## linux/cs537.h
- Add pthread* wrappers' prototypes
## linux/server.c
- Use two cv's: requestProcessed and requestAccepted to represent the buffer and
  the producer-consumer relationship
- Main (server, producer) waits for buffer to have an empty spot by waiting on 
  requestProcessed
  - Main signals worker that new connection was accepted
- Worker (client, consumer) waits for buffer to have a waiting request by 
  waiting on requestAccepted
  - Worker signals main that buffer spot was cleared
  - Worker does work outside of locking. Don't need to lock while worker handles
    request
- Buffer is stored as a regular, one dimensional array
  - Main loops through array to find unused spot
  - Worker loops through array to find waiting request
  - A waiting request in the buffer immediately vacates its spot when worker 
    assigns itself to start work

# P4B: xv6 Kernel Threads
## xv6/xv6-fa20/include/param.h
- macro NTHREAD for max number of threads
## xv6/xv6-fa20/include/syscall.h
- macros for system call numbers: sys_clone, sys_join, sys_cond_init, 
  sys_cond_wait, sys_cond_signal
## xv6/xv6-fa20/include/types.h
- define cond_t struct to represent a cv
  - circular array pid[NTHREAD] - need to track head, tail, and size
  - also keep track of the spinlock
## xv6/xv6-fa20/kernel/circleQueue.c
- implement circular array queue (code copied from P2B)
## xv6/xv6-fa20/kernel/circleQueue.h
- implement circular array queue (code copied from P2B)
## xv6/xv6-fa20/kernel/conditionVariable.c
- implement cond_init, cond_wait
- cond_signal needed access to ptable, so we put that in proc.c
## xv6/xv6-fa20/kernel/defs.h
- boilerplate for new system calls - add function prototypes:
  - cond_init, cond_wait, cond_signal
  - clone, join
## xv6/xv6-fa20/kernel/makefile.mk
- include two new output files: circleQueue.o, conditionVariable.o
## xv6/xv6-fa20/kernel/proc.c
- implement clone, join, cond_signal
- modify wait, growproc to be thread-friendly
  - wait
    - only free pgdir and kstack if last thread in address space
      - loop through ptable again for matching pgdir
    - otherwise, set pgdir and kstack to 0 to indicate finished thread
  - growproc
    - when more space allocated for the process, need to update all threads' sz
    - implemented by looping through p table, checking matching pgdir, and 
      updating sz
## xv6/xv6-fa20/kernel/proc.h
- struct proc add stackHigh to track highest address of thread's stack, separate
  from sz
## xv6/xv6-fa20/kernel/spinlock.h
- moved struct spinlock definition to types.h; we use it in cond_t struct
## xv6/xv6-fa20/kernel/syscall.c
- boilerplate to add new sytem calls - add to system call table: 
  - sys_clone, sys_join, sys_cond_init, sys_cond_wait, sys_cond_signal
## xv6/xv6-fa20/kernel/sysfunc.h
- boilerplate to add new system calls - add function prototypes:
  - sys_clone, sys_join, sys_cond_init, sys_cond_wait, sys_cond_signal
## xv6/xv6-fa20/kernel/sysproc.c
- connect kstack and ustack for new system calls - wasn't sure about pointers so
  a lot of trial and error here
## xv6/xv6-fa20/kernel/trap.c
- implement graceful exit if instruction pointer is set to 0xffffffff, by
  adding a check for 0xfff in the default case
## xv6/xv6-fa20/kernel/vm.c
- unmodified (debug statement now removed)
## xv6/xv6-fa20/user/makefile.mk
- add tests
- add new library files (thread.o and lock.o)
## xv6/xv6-fa20/user/user.h
- boilerplate for new user functions and system calls - add function prototypes:
  - sys calls: clone, join, cond_init, cond_wait, cond_signal
  - user space: thread_create, thread_join, lock_acquire, lock_release, 
    lock_init
## xv6/xv6-fa20/user/usys.S
- boilerplate for new system calls: clone, join, cond_init, cond_wait, 
  cond_signal
## xv6/xv6-fa20/user/lock.c
- implement basic spinlock in user space: 
  - lock_init, lock_acquire, lock_release
  - refer to OSTEP figure 28.3 and kernel/spinlock.c

## P4B tests (including provided ones)
xv6/xv6-fa20/user/clone_clone.c
xv6/xv6-fa20/user/cond.c
xv6/xv6-fa20/user/cond2.c
xv6/xv6-fa20/user/cond3.c
xv6/xv6-fa20/user/create.c
xv6/xv6-fa20/user/create2.c
xv6/xv6-fa20/user/custWaitingRoom.c
xv6/xv6-fa20/user/fork_clone.c
xv6/xv6-fa20/user/join.c
xv6/xv6-fa20/user/join2.c
xv6/xv6-fa20/user/recursion.c
xv6/xv6-fa20/user/recursion2.c
xv6/xv6-fa20/user/size.c
xv6/xv6-fa20/user/testCondInit.c
xv6/xv6-fa20/user/testThread1.c
xv6/xv6-fa20/user/thread.c
xv6/xv6-fa20/user/two_threads.c
xv6/xv6-fa20/user/two_threads_temp.c