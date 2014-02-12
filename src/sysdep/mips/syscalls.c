#include "syscalls.h"
#include <stdint.h>
#include <stddef.h>

// sockets

__DEFINE_SYSCALL3(socket, int, int domain, int type, int proto);
__DEFINE_SYSCALL3(bind, int, int sockfd, const struct sockaddr *addr, int addrlen);
__DEFINE_SYSCALL3(connect, int, int sockfd, const struct sockaddr *addr, int addrlen);
__DEFINE_SYSCALL2(listen, int, int sockfd, int backlog);
__DEFINE_SYSCALL3(accept, int, int sockfd, const struct sockaddr *addr, int addrlen);
__DEFINE_SYSCALL3(getsockname, int, int sockfd, const struct sockaddr *addr, int addrlen);
__DEFINE_SYSCALL3(getpeername, int, int sockfd, const struct sockaddr *addr, int addrlen);
// socketpair skipped
__DEFINE_SYSCALL4(send, int, int sockfd, void *buf, size_t len, int flags);
__DEFINE_SYSCALL4(recv, int, int sockfd, void *buf, size_t len, int flags);
__DEFINE_SYSCALL6(sendto, int, int sockfd, void *buf, size_t len, int flags, const struct sockaddr *addr, int addrlen);
__DEFINE_SYSCALL6(recvfrom, int, int sockfd, void *buf, size_t len, int flags, struct sockaddr *addr, int *addrlen);
__DEFINE_SYSCALL2(shutdown, int, int sockfd, int how);
__DEFINE_SYSCALL5(setsockopt, int, int sockfd, int level, int name, const void *val, size_t len);
__DEFINE_SYSCALL5(getsockopt, int, int sockfd, int level, int name, void *val, size_t *len);


long __attribute__((section(".text.syscalls"))) __sys_check_error()
{
	__asm__ __volatile__(
		"beqz	$a3, 1f" "\n\t" \
		"la		$v1, sys_errno" "\n\t" \
		"sw		$v0, 0($v1)" "\n\t"\
		"li		$v0, -1" "\n\t" \
		"1:" "\n\t" \
	);
}

