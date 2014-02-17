#include "syscalls.h"
#include <stdint.h>
#include <stddef.h>

// sockets

#define __NRS_socket      1
#define __NRS_bind        2
#define __NRS_connect     3
#define __NRS_listen      4
#define __NRS_accept      5
#define __NRS_getsockname 6
#define __NRS_getpeername 7
#define __NRS_socketpair  8
#define __NRS_send        9
#define __NRS_recv        10
#define __NRS_sendto      11
#define __NRS_recvfrom    12
#define __NRS_shutdown    13
#define __NRS_setsockopt  14
#define __NRS_getsockopt  15
#define __NRS_sendmsg     16
#define __NRS_recvmsg     17
#define __NRS_accept4     18

#define __SOCKETCALL_BODY(nr) \
	{ \
		__asm__ __volatile__ \
		( \
			"lea	2*4(%%ebp), %%ecx" "\n\t" \
			"int	$0x80" "\n\t" \
			"cmpl	$-4095, %%eax" "\n\t" \
			"jb		1f" "\n\t" \
			"neg	%%eax" "\n\t" \
			"movl	%%eax, sys_errno" "\n\t" \
			"movl	$-1, %%eax" "\n\t" \
			"1:" "\n\t" \
			: \
			: "a"(__NR_socketcall), "b" (nr) \
		); \
	}

#define __DEFINE_SOCKETCALL(name, rtype, ...) \
	rtype __attribute__((section(".text.syscalls"))) sys_##name(__VA_ARGS__) \
	__SOCKETCALL_BODY(__NRS_##name)

__DEFINE_SOCKETCALL(socket, int, int domain, int type, int proto);
__DEFINE_SOCKETCALL(bind, int, int sockfd, const struct sockaddr *addr, int addrlen);
__DEFINE_SOCKETCALL(connect, int, int sockfd, const struct sockaddr *addr, int addrlen);
__DEFINE_SOCKETCALL(listen, int, int sockfd, int backlog);
__DEFINE_SOCKETCALL(accept, int, int sockfd, const struct sockaddr *addr, int addrlen);
__DEFINE_SOCKETCALL(getsockname, int, int sockfd, const struct sockaddr *addr, int addrlen);
__DEFINE_SOCKETCALL(getpeername, int, int sockfd, const struct sockaddr *addr, int addrlen);
// socketpair skipped
__DEFINE_SOCKETCALL(send, int, int sockfd, void *buf, size_t len, int flags);
__DEFINE_SOCKETCALL(recv, int, int sockfd, void *buf, size_t len, int flags);
__DEFINE_SOCKETCALL(sendto, int, int sockfd, void *buf, size_t len, int flags, const struct sockaddr *addr, int addrlen);
__DEFINE_SOCKETCALL(recvfrom, int, int sockfd, void *buf, size_t len, int flags, struct sockaddr *addr, int *addrlen);
__DEFINE_SOCKETCALL(shutdown, int, int sockfd, int how);
__DEFINE_SOCKETCALL(setsockopt, int, int sockfd, int level, int name, const void *val, size_t len);
__DEFINE_SOCKETCALL(getsockopt, int, int sockfd, int level, int name, void *val, size_t *len);

long real_syscall()
{
	/*
	At this point, we have %ebp pointing to our frame:
	new_ebp
	ebx
	old_ebp <-- ebp
	ret_addr
	arg0
	arg1
	...

	These are loaded into:
	ebx ecx edx esi edi ebp
	*/
	__asm__ __volatile__
	(
		".globl __real_syscall" "\n\t"
	"__real_syscall:" "\n\t"
		"pushl	%ebx" "\n\t"
		"pushl	%edx" "\n\t"
		"pushl	%esi" "\n\t"
		"pushl	%edi" "\n\t"
		"pushl	%eax" "\n\t"
		"movl	(7*4)(%esp), %ebx" "\n\t"
		"movl	(8*4)(%esp), %ecx" "\n\t"
		"movl	(9*4)(%esp), %edx" "\n\t"
		"movl	(10*4)(%esp), %esi" "\n\t"
		"movl	(11*4)(%esp), %edi" "\n\t"
		"movl	(12*4)(%esp), %ebp" "\n\t"
		"movl	(%esp), %eax" "\n\t"
		"int	$0x80" "\n\t"
		"popl	%edi" "\n\t"
		"popl	%edi" "\n\t"
		"popl	%esi" "\n\t"
		"popl	%edx" "\n\t"
		"popl	%ebx" "\n\t"
		"cmpl	$-4095, %eax" "\n\t"
		"jb		noerror" "\n\t"
		"neg	%eax" "\n\t"
		"movl	%eax, sys_errno" "\n\t"
		"movl	$-1, %eax" "\n\t"

	"noerror:" "\n\t"
		"popl	%ebp" "\n\t"
		"ret" "\n\t"
		/* pop ebp is provided by the compiler within epilog. */
	);
	__builtin_unreachable();
}
