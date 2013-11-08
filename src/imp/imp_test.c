#include "imp.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	struct imp_num_t a, b, c;
	
	printf("Allocating memory...\n");
	imp_num_init(&a, 64);
	imp_num_init(&b, 64);
	imp_num_init(&c, 128);
	
	imp_num_load1(&a, 0x12345678);
	imp_num_load1(&b, 0xFFFF1234);
	imp_num_print(&a); printf("\n");
	imp_num_print(&b); printf("\n");
	imp_num_print(&c); printf("\n");
	
	imp_add(&a, &b, &c);
	imp_num_print(&a); printf("\n");
	imp_num_print(&b); printf("\n");
	imp_num_print(&c); printf("\n");
	
	printf("Freeing memory...\n");
	imp_num_free(&c);
	imp_num_free(&b);
	imp_num_free(&a);

	printf("Done.\n");
	return 0;
}


