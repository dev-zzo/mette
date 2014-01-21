#include "testing.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static void __test_abort()
{
	exit(1);
}

void __test_fail(const char *test_name, const char *cond)
{
	fprintf(stderr, "%s: ", test_name);
	fprintf(stderr, "Expectation '%s' failed.", cond);
	__test_abort();
}

void __test_fail_msg(const char *test_name, const char *text, ...)
{
	va_list args;

	va_start(args, text);
	fprintf(stderr, "%s: ", test_name);
	vfprintf(stderr, text, args);
	va_end(args);
	__test_abort();
}

int main(int argc, char *argv[])
{

}
