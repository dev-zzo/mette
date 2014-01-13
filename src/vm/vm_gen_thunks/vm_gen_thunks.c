#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

char buffer[2048];

#include "../vm_ncallhash.inc"

void process(FILE *fp)
{
	while (!feof(fp)) {
		fgets(buffer, 2048, fp);
		if (!strncmp("VM_THUNK", buffer, 8)) {
			char *ptr = buffer + 8;
			char *id;
			while (*ptr && *ptr != '(')
				++ptr;
			++ptr;
			while (*ptr && isspace(*ptr))
				++ptr;
			id = ptr;
			while (*ptr && (isalnum(*ptr) || *ptr == '_'))
				++ptr;
			*ptr = '\0';
			fprintf(stdout, "void __thunkproc_%s(vm_context_t *ctx);\n", id);
			fprintf(stdout, "vm_thunk_record_t __thunkrec_%s __attribute__ ((section (\".thunks\"))) = { 0x%08XU, __thunkproc_%s };\n", id, vma_hash_name(id), id);
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;

	--argc; ++argv;

	if (argc > 0) {
		while (argc > 0) {
			fp = fopen(argv[0], "rt");
			fprintf(stdout,"/* Source: %s */\n", argv[0]);
			process(fp);
			fclose(fp);
			--argc; ++argv;
		}
	} else {
		process(stdin);
	}

	return 0;
}