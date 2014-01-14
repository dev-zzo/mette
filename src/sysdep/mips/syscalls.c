#include "syscalls.h"
#include <stdint.h>
#include <stddef.h>

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

