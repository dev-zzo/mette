#include "syscalls.h"
#include <stddef.h>
#include <sys/types.h>

int sys_errno;

// processes

__DEFINE_SYSCALL1(exit, void, int code);
__DEFINE_SYSCALL1(exit_group, void, int code);
__DEFINE_SYSCALL1(fork, int, void *regs);
__DEFINE_SYSCALL0(getpid, int);

// signals

//#define sys_sigaction(sig, act, oldact) __SYSCALL(sigaction, 3, sig, act, oldact)
//#define sys_sigpending(set) __SYSCALL(sigpending, 1, set)
//#define sys_kill(pid, sig) __SYSCALL(kill, 2, pid, sig)
//#define sys_rt_sigaction(sig, act, oldact, sigsetsize) __SYSCALL(rt_sigaction, 4, sig, act, oldact, sigsetsize)
//#define sys_rt_sigprocmask(how, set, oset, sigsetsize) __SYSCALL(rt_sigprocmask, 4, how, set, oset, sigsetsize)
//#define sys_rt_sigpending(set, sigsetsize) __SYSCALL(rt_sigpending, 2, set, sigsetsize)
//#define sys_rt_sigqueueinfo(pid, sig, info) __SYSCALL(rt_sigqueueinfo, 3, pid, sig, info)
//#define sys_rt_sigsuspend(set, sigsetsize) __SYSCALL(rt_sigsuspend, 2, set, sigsetsize)

// file system

//#define sys_chdir(path) __SYSCALL(chdir, 1, path)
__DEFINE_SYSCALL1(unlink, int, const char *path);
__DEFINE_SYSCALL2(chmod, int, const char *path, unsigned long mode);
//#define sys_fchmod(fd, mode) __SYSCALL(fchmod, 2, fd, mode)
//#define sys_stat(path, sbuf) __SYSCALL(stat, 2, path, buf)
//#define sys_fstat(fd, sbuf) __SYSCALL(fstat, 2, fd, sbuf)
//#define sys_access(path, mode) __SYSCALL(access, 2, path, mode)
//#define sys_rename(pathold, pathnew) __SYSCALL(rename, 2, pathold, pathnew)
__DEFINE_SYSCALL2(mkdir, int, const char *path, unsigned long mode);
__DEFINE_SYSCALL1(rmdir, int, const char *path);
//#define sys_truncate(path, length) __SYSCALL(truncate, 2, path, length)
//#define sys_ftruncate(fd, length) __SYSCALL(ftruncate, 2, fd, length)
__DEFINE_SYSCALL3(open, int, const char *path, int flags, int mode);
__DEFINE_SYSCALL3(read, int, int fd, void *buf, size_t count);
__DEFINE_SYSCALL3(write, int, int fd, const void *buf, size_t count);
__DEFINE_SYSCALL1(close, int, int fd);
__DEFINE_SYSCALL3(lseek, int, int fd, off_t offset, int origin);
__DEFINE_SYSCALL2(flock, int, int fd, unsigned int cmd);
__DEFINE_SYSCALL3(ioctl, int, int fd, unsigned int cmd, unsigned long arg);
__DEFINE_SYSCALL3(fcntl, int, int fd, unsigned int cmd, unsigned long arg);
__DEFINE_SYSCALL5(_newselect, int, int n, fd_set *inp, fd_set *outp, fd_set *exp, struct timeval *tvp);
//#define sys_pipe(pipefd) __SYSCALL(pipe, 1, pipefd)

// memory

__DEFINE_SYSCALL1(brk, void *, void *addr);
__DEFINE_SYSCALL6(mmap, void *, void *addr, size_t length, int prot, int flags, int fd, off_t offset);
//#define sys_old_mmap(args) __SYSCALL(old_mmap, 1, args)
__DEFINE_SYSCALL2(munmap, int, void *addr, size_t length);
//#define sys_mprotect(addr, length, prot) __SYSCALL(mprotect, 3, addr, length, prot)

// sockets: platform-specific

// IPC

// misc

__DEFINE_SYSCALL1(time, time_t, time_t *tloc);
