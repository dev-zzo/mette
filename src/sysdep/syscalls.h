#ifndef __mette_syscalls_included
#define __mette_syscalls_included

// http://www.win.tue.nl/~aeb/linux/lk/lk-4.html
// http://syscalls.kernelgrok.com/

// ==================== BEGIN ARCH-SPECIFIC CODE ==============================
#include "syscalls-i386.h"
// =================== END ARCH-SPECIFIC CODE =================================

// processes

#define sys_exit(code) __SYSCALL(exit, 1, code)
#define sys_exit_group(code) __SYSCALL(exit_group, 1, code)
#define sys_fork(regs) __SYSCALL(fork, 1, regs)
#define sys_getpid() __SYSCALL(getpid, 0)

// signals

#define sys_sigaction(sig, act, oldact) __SYSCALL(sigaction, 3, sig, act, oldact)
#define sys_sigpending(set) __SYSCALL(sigpending, 1, set)
#define sys_kill(pid, sig) __SYSCALL(kill, 2, pid, sig)
#define sys_rt_sigaction(sig, act, oldact, sigsetsize) __SYSCALL(rt_sigaction, 4, sig, act, oldact, sigsetsize)
#define sys_rt_sigprocmask(how, set, oset, sigsetsize) __SYSCALL(rt_sigprocmask, 4, how, set, oset, sigsetsize)
#define sys_rt_sigpending(set, sigsetsize) __SYSCALL(rt_sigpending, 2, set, sigsetsize)
#define sys_rt_sigqueueinfo(pid, sig, info) __SYSCALL(rt_sigqueueinfo, 3, pid, sig, info)
#define sys_rt_sigsuspend(set, sigsetsize) __SYSCALL(rt_sigsuspend, 2, set, sigsetsize)

// file system

#define sys_chdir(path) __SYSCALL(chdir, 1, path)
#define sys_unlink(path) __SYSCALL(unlink, 1, path)
#define sys_chmod(path, mode) __SYSCALL(chmod, 2, path, mode)
#define sys_fchmod(fd, mode) __SYSCALL(fchmod, 2, fd, mode)
#define sys_stat(path, sbuf) __SYSCALL(stat, 2, path, buf)
#define sys_fstat(fd, sbuf) __SYSCALL(fstat, 2, fd, sbuf)
#define sys_access(path, mode) __SYSCALL(access, 2, path, mode)
#define sys_rename(pathold, pathnew) __SYSCALL(rename, 2, pathold, pathnew)
#define sys_mkdir(path, mode) __SYSCALL(mkdir, 2, path, mode)
#define sys_rmdir(path) __SYSCALL(rmdir, 1, path)
#define sys_truncate(path, length) __SYSCALL(truncate, 2, path, length)
#define sys_ftruncate(fd, length) __SYSCALL(ftruncate, 2, fd, length)
#define sys_open(path, flags, mode) __SYSCALL(open, 3, path, flags, mode)
#define sys_read(fd, buf, count) __SYSCALL(read, 3, fd, buf, count)
#define sys_write(fd, buf, count) __SYSCALL(write, 3, fd, buf, count)
#define sys_close(fd) __SYSCALL(close, 1, fd)
#define sys_lseek(fd, offset, origin) __SYSCALL(lseek, 2, offset, origin)
#define sys_flock(fd, cmd) __SYSCALL(flock, 2, fd, cmd)
#define sys_select(n, inp, outp, exp, tvp) __SYSCALL(select, 5, n, inp, outp, exp, tvp)
#define sys_pipe(pipefd) __SYSCALL(pipe, 1, pipefd)

// memory

#define sys_brk(addr) __SYSCALL(brk, 1, addr)
#define sys_old_mmap(args) __SYSCALL(old_mmap, 1, args)
#define sys_munmap(addr, length) __SYSCALL(munmap, 2, addr, length)
#define sys_mprotect(addr, length, prot) __SYSCALL(mprotect, 3, addr, length, prot)

// sockets

#ifdef ARCH_WANTS_SOCKETCALL
// multiplexed socket API
#define sys_socket(domain, type, proto) __SOCKETCALL(SYS_SOCKET, 3, domain, type, proto)
#define sys_bind(sockfd, addr, addrlen) __SOCKETCALL(SYS_BIND, 3, sockfd, addr, addrlen)
#define sys_connect(sockfd, addr, addrlen) __SOCKETCALL(SYS_CONNECT, 3, sockfd, addr, addrlen)
#define sys_listen(sockfd, backlog) __SOCKETCALL(SYS_LISTEN, 2, sockfd, backlog)
#define sys_accept(sockfd, addr, addrlen) __SOCKETCALL(SYS_ACCEPT, 3, sockfd, addr, addrlen)
#define sys_getsockname(sockfd, addr, addrlen) __SOCKETCALL(SYS_GETSOCKNAME, 3, sockfd, addr, addrlen)
#define sys_getpeername(sockfd, addr, addrlen) __SOCKETCALL(SYS_GETPEERNAME, 3, sockfd, addr, addrlen)
// socketpair skipped
#define sys_send(sockfd, buf, len, flags) __SOCKETCALL(SYS_SEND, 4, sockfd, buf, len, flags)
#define sys_recv(sockfd, buf, len, flags) __SOCKETCALL(SYS_RECV, 4, sockfd, buf, len, flags)
#define sys_sendto(sockfd, buf, len, flags, addr, addrlen) __SOCKETCALL(SYS_SENDTO, 6, sockfd, buf, len, flags, addr, addrlen)
#define sys_recvfrom(sockfd, buf, len, flags, addr, addrlen) __SOCKETCALL(SYS_RECVFROM, 6, sockfd, buf, len, flags, addr, addrlen)
#define sys_shutdown(sockfd, how) __SOCKETCALL(SYS_SHUTDOWN, 2, sockfd, how)
#define sys_setsockopt(sockfd, level, name, val, len) __SOCKETCALL(SYS_SETSOCKOPT, 5, sockfd, level, name, val, len)
#define sys_getsockopt(sockfd, level, name, val, len) __SOCKETCALL(SYS_GETSOCKOPT, 5, sockfd, level, name, val, len)
// *msg skipped
#else
// non-multiplexed socket API
#endif

// IPC

// misc

#define sys_time(tloc) __SYSCALL(time, 1, tloc)

#endif // __mette_syscalls_included

