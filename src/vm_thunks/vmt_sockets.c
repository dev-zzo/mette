#include "syscalls.h"
#include "rtl_strbuf.h"
#include "rtl_memory.h"
#include "vm_thunks.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#if TARGET_IS_BE == 0

static unsigned short rtl_htons(unsigned short x)
{
	return ((x & 0x00FFU) << 8) | ((x & 0xFF00U) >> 8);
}

#define rtl_ntohs(x) rtl_htons(x)

static unsigned int rtl_htonl(unsigned int x)
{
	return ((x & 0x000000FFU) << 24)
		| ((x & 0x0000FF00U) << 8)
		| ((x & 0x00FF0000U) >> 8)
		| ((x & 0xFF000000U) >> 24);
}

#define rtl_ntohl(x) rtl_htonl(x)

#else

#define rtl_ntohs(x) (x)
#define rtl_htons(x) (x)
#define rtl_ntohl(x) (x)
#define rtl_htonl(x) (x)

#endif


VM_THUNK(sockaddr_create)
{
	struct sockaddr_in *sa;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(unsigned long ip);
		VM_THUNK_ARG(unsigned short port);
	VM_THUNK_ARGS_END

	sa = (struct sockaddr_in *)rtl_alloc(sizeof(*sa));
	sa->sin_family = AF_INET;
	sa->sin_port = rtl_htons(args.port);
	sa->sin_addr.s_addr = rtl_htonl(args.ip);

	VM_THUNK_RETURN(sa);
}

/* sockaddr can be freed with rtl_free. No separate method. */

VM_THUNK(sockaddr_get_ip)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(struct sockaddr_in *sa);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_ntohl(args.sa->sin_addr.s_addr));
}

VM_THUNK(sockaddr_get_port)
{
	VM_THUNK_ARGS_START
		VM_THUNK_ARG(struct sockaddr_in *sa);
	VM_THUNK_ARGS_END

	VM_THUNK_RETURN(rtl_ntohs(args.sa->sin_port));
}

VM_THUNK(socket_create)
{
	int sockfd;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int type);
		VM_THUNK_ARG(int protocol);
	VM_THUNK_ARGS_END

	sockfd = sys_socket(AF_INET, args.type, args.protocol);

	VM_THUNK_RETURN(sockfd);
}

VM_THUNK(socket_set_blocking)
{
	unsigned long flags;
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(int blocking);
	VM_THUNK_ARGS_END

	flags = sys_fcntl(args.sockfd, F_GETFL, 0);
	if (flags < 0) {
		VM_THUNK_RETURN(-1);
	}
	flags = args.blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
	result = sys_fcntl(args.sockfd, F_SETFL, flags);

	VM_THUNK_RETURN(result);
}

VM_THUNK(socket_bind)
{
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(struct sockaddr *sa);
	VM_THUNK_ARGS_END

	result = sys_bind(args.sockfd, args.sa, sizeof(*args.sa));

	VM_THUNK_RETURN(result);
}

VM_THUNK(socket_listen)
{
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(int backlog);
	VM_THUNK_ARGS_END

	result = sys_listen(args.sockfd, args.backlog);

	VM_THUNK_RETURN(result);
}

VM_THUNK(socket_accept)
{
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(struct sockaddr *sa);
	VM_THUNK_ARGS_END

	result = sys_accept(args.sockfd, args.sa, sizeof(*args.sa));

	VM_THUNK_RETURN(result);
}

VM_THUNK(socket_connect)
{
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(struct sockaddr *sa);
	VM_THUNK_ARGS_END

	result = sys_connect(args.sockfd, args.sa, sizeof(*args.sa));

	VM_THUNK_RETURN(result);
}

VM_THUNK(socket_send)
{
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(rtl_strbuf_t *sb);
	VM_THUNK_ARGS_END

	result = sys_send(
		args.sockfd,
		rtl_strbuf_get_buffer(args.sb), rtl_strbuf_get_length(args.sb),
		0);

	VM_THUNK_RETURN(result);
}

VM_THUNK(socket_sendto)
{
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(rtl_strbuf_t *sb);
		VM_THUNK_ARG(struct sockaddr *sa);
	VM_THUNK_ARGS_END

	result = sys_sendto(
		args.sockfd,
		rtl_strbuf_get_buffer(args.sb), rtl_strbuf_get_length(args.sb),
		0,
		args.sa, sizeof(*args.sa));

	VM_THUNK_RETURN(result);
}

VM_THUNK(socket_recv)
{
	rtl_strbuf_t *sb;
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(size_t max_length);
	VM_THUNK_ARGS_END

	sb = rtl_strbuf_alloc(args.max_length);
	if (!sb) {
		sys_errno = ENOMEM;
		VM_THUNK_RETURN(0);
	}

	result = sys_recv(
		args.sockfd,
		rtl_strbuf_get_buffer(sb), rtl_strbuf_get_size(sb),
		0);

	if (result >= 0) {
		rtl_strbuf_set_length(sb, result);
		VM_THUNK_RETURN(sb);
	} else {
		rtl_strbuf_free(sb);
		VM_THUNK_RETURN(0);
	}
}

VM_THUNK(socket_recvfrom)
{
	rtl_strbuf_t *sb;
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(size_t max_length);
		VM_THUNK_ARG(struct sockaddr *sa);
	VM_THUNK_ARGS_END

	sb = rtl_strbuf_alloc(args.max_length);
	if (!sb) {
		sys_errno = ENOMEM;
		VM_THUNK_RETURN(0);
	}

	result = sys_recvfrom(
		args.sockfd,
		rtl_strbuf_get_buffer(sb), rtl_strbuf_get_size(sb),
		0,
		args.sa, sizeof(*args.sa));

	if (result >= 0) {
		rtl_strbuf_set_length(sb, result);
		VM_THUNK_RETURN(sb);
	} else {
		rtl_strbuf_free(sb);
		VM_THUNK_RETURN(0);
	}
}

VM_THUNK(socket_shutdown)
{
	int result;

	VM_THUNK_ARGS_START
		VM_THUNK_ARG(int sockfd);
		VM_THUNK_ARG(int how);
	VM_THUNK_ARGS_END

	result = sys_shutdown(args.sockfd, args.how);

	VM_THUNK_RETURN(result);
}
