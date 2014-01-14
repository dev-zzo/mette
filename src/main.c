#include "vm.h"
#include "syscalls.h"

const char string[] = "Hello world!\n\n"; // 14

int main(int argc, char *argv[])
{
	//vm_load_exec(argv[1]);
	sys_write(1, string, 14);
	sys_write(1, argv[0], 8);
	sys_write(1, argv[1], 8);
	sys_exit(24);
	return 0;
}
