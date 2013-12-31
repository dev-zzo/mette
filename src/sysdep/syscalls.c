#include "syscalls.h"

int xexit(int code)
{
	return sys_exit(code);
}

int xfork(void)
{
	return sys_fork(0);
}

int xgetpid(void)
{
	return sys_getpid();
}

int xchdir(const char *path)
{
	return sys_chdir(path);
}

int xunlink(const char *path)
{
	return sys_unlink(path);
}

int xchmod(const char *path, int mode)
{
	return sys_chmod(path, mode);
}

int xfchmod(int fd, int mode)
{
	return sys_fchmod(fd, mode);
}

int xstat(const char *path, void *buf)
{
	return sys_stat(path, buf);
}

int xfstat(int fd, void *buf)
{
	return sys_fstat(fd, buf);
}

int xrename(const char *path_old, const char *path_new)
{
	return sys_rename(path_old, path_new);
}

int xmkdir(const char *path, int mode)
{
	return sys_mkdir(path, mode);
}

int xrmdir(const char *path)
{
	return sys_rmdir(path);
}

int xopen(const char *path, int flags, int mode)
{
	return sys_open(path, flags, mode);
}

int xread(int fd, void *buf, size_t count)
{
	return sys_read(fd, buf, count);
}

int xwrite(int fd, void *buf, size_t count)
{
	return sys_write(fd, buf, count);
}

int xclose(int fd)
{
	return sys_close(fd);
}

int xlseek(int fd, int offset, int origin)
{
	return sys_lseek(fd, offset, origin);
}

int xselect(int n, fd_set *inp, fd_set *outp, fd_set *exp, struct timeval *tvp)
{
	return sys_select(n, inp, outp, exp, tvp);
}

int xmmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	struct mmap_arg_struct {
		unsigned long addr;
		unsigned long len;
		unsigned long prot;
		unsigned long flags;
		unsigned long fd;
		unsigned long offset;
	} args = { addr, length, prot, flags, fd, offset };
	return sys_old_mmap(&args);
}

int xmunmap(void *addr, size_t length)
{
	return sys_munmap(addr, length);
}

int xmprotect(void *addr, size_t length, int prot)
{
	return sys_mprotect(addr, length, prot);
}

int xtime(time_t *t)
{
	return sys_time(t);
}


