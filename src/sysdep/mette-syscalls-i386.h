#ifndef __mette_syscalls_i386_included
#define __mette_syscalls_i386_included

// TODO: find out how to obtain _proper_ syscall numbers.
// The /usr/include/asm-generic/unistd.h contains something else.
#include <i386-linux-gnu/asm/unistd.h>
#include <linux/net.h>

// Some ideas shamelessly stolen from:
// http://www.scs.stanford.edu/histar/src/pkg/uclibc/libc/sysdeps/linux/i386/bits/syscalls.h

// http://simon.baymoo.org/universe/tools/symset/symset.txt

#define __SYSCALL_ARGS_0
#define __SYSCALL_ARGS_1(arg1) \
	__SYSCALL_ARGS_0, "i" (arg1)
#define __SYSCALL_ARGS_2(arg1, arg2) \
	__SYSCALL_ARGS_1(arg1), "c" (arg2)
#define __SYSCALL_ARGS_3(arg1, arg2, arg3) \
	__SYSCALL_ARGS_2(arg1, arg2), "d" (arg3)
#define __SYSCALL_ARGS_4(arg1, arg2, arg3, arg4) \
	__SYSCALL_ARGS_3(arg1, arg2, arg3), "S" (arg4)
#define __SYSCALL_ARGS_5(arg1, arg2, arg3, arg4, arg5) \
	__SYSCALL_ARGS_4(arg1, arg2, arg3, arg4), "D" (arg5)
#define __SYSCALL_ARGS_6(arg1, arg2, arg3, arg4, arg5, arg6) \
	__SYSCALL_ARGS_5(arg1, arg2, arg3, arg4, arg5), "m" (arg6)
	
#define LOAD_ARGS_0
#define LOAD_ARGS_1 \
	LOAD_ARGS_0 \
	"push %%ebx\n\t" \
	"movl %2, %%ebx\n\t"
#define LOAD_ARGS_2 LOAD_ARGS_1
#define LOAD_ARGS_3 LOAD_ARGS_2
#define LOAD_ARGS_4 LOAD_ARGS_3
#define LOAD_ARGS_5 LOAD_ARGS_4
#define LOAD_ARGS_6 \
	LOAD_ARGS_5 \
	"push %%ebp\n\t" \
	"movl %7, %%ebp\n\t"

#define RESTORE_ARGS_0
#define RESTORE_ARGS_1 \
	RESTORE_ARGS_0 \
	"pop %%ebx\n\t"
#define RESTORE_ARGS_2 RESTORE_ARGS_1
#define RESTORE_ARGS_3 RESTORE_ARGS_2
#define RESTORE_ARGS_4 RESTORE_ARGS_3
#define RESTORE_ARGS_5 RESTORE_ARGS_4
#define RESTORE_ARGS_6 \
	RESTORE_ARGS_5 \
	"pop %%ebp\n\t"

#define __SYSCALL(name, nr, ...) \
	(__extension__({ \
		long __rv; \
		__asm__ __volatile__ \
		( \
			LOAD_ARGS_##nr \
			"int $0x80\n\t" \
			RESTORE_ARGS_##nr \
			: "=a" (__rv) \
			: "0" (__NR_##name) __SYSCALL_ARGS_##nr(__VA_ARGS__) \
			: "memory", "cc" \
		); \
		__rv; \
	}))


#define ARCH_WANTS_SOCKETCALL 1

#define __SOCKETCALL_LOAD_0 \
	"movl %%esp, %%ecx\n\t"
#define __SOCKETCALL_LOAD_1 \
	"push %3\n\t" \
	__SOCKETCALL_LOAD_0
#define __SOCKETCALL_LOAD_2 \
	"push %4\n\t" \
	__SOCKETCALL_LOAD_1
#define __SOCKETCALL_LOAD_3() \
	"push %5\n\t" \
	__SOCKETCALL_LOAD_2
#define __SOCKETCALL_LOAD_4 \
	"push %6\n\t" \
	__SOCKETCALL_LOAD_3
#define __SOCKETCALL_LOAD_5 \
	"push %7\n\t" \
	__SOCKETCALL_LOAD_4
#define __SOCKETCALL_LOAD_6 \
	"push %8\n\t" \
	__SOCKETCALL_LOAD_5

#define __SOCKETCALL_RESTORE(nr) \
	"addl $(" #nr " * 4), %%esp\n\t"

#define __SOCKETCALL_ARGS_0
#define __SOCKETCALL_ARGS_1(arg1) \
	__SOCKETCALL_ARGS_0, "g" (arg1)
#define __SOCKETCALL_ARGS_2(arg1, arg2) \
	__SOCKETCALL_ARGS_1(arg1), "g" (arg2)
#define __SOCKETCALL_ARGS_3(arg1, arg2, arg3) \
	__SOCKETCALL_ARGS_2(arg1, arg2), "g" (arg3)
#define __SOCKETCALL_ARGS_4(arg1, arg2, arg3, arg4) \
	__SOCKETCALL_ARGS_3(arg1, arg2, arg3), "g" (arg4)
#define __SOCKETCALL_ARGS_5(arg1, arg2, arg3, arg4, arg5) \
	__SOCKETCALL_ARGS_4(arg1, arg2, arg3, arg4), "g" (arg5)
#define __SOCKETCALL_ARGS_6(arg1, arg2, arg3, arg4, arg5, arg6) \
	__SOCKETCALL_ARGS_5(arg1, arg2, arg3, arg4, arg5), "g" (arg6)

#define __SOCKETCALL(name, nr, ...) \
	(__extension__({ \
		long __rv; \
		__asm__ __volatile__ \
		( \
			"push %%ebx\n\t" \
			"movl %2, %%ebx\n\t" \
			__SOCKETCALL_LOAD_##nr() \
			"int $0x80\n\t" \
			__SOCKETCALL_RESTORE(nr) \
			"pop %%ebx\n\t" \
			: "=a" (__rv) \
			: "0" (__NR_socketcall), "i" (name) __SOCKETCALL_ARGS_##nr(__VA_ARGS__) \
			: "%ecx", "memory", "cc" \
		); \
		__rv; \
	}))

#endif // __mette_syscalls_i386_included

