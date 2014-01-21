#include "vm.h"
#include "syscalls.h"
#include "rtl_print.h"

const char string[] = "Hello world!\n\n"; // 14

int main(int argc, char *argv[])
{
	//vm_load_exec(argv[1]);
	rtl_print_fd(1, "No operands test!\n");
	rtl_print_fd(1, "Conversion: 08x (123456): '%08x'\n", 0x123456);
	rtl_print_fd(1, "Conversion: z (123456): '%z'\n", "abcdabcd");
	sys_exit(24);
	return 0;
}
