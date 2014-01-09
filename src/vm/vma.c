#include "vma.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

extern int vma_parse_input(struct vma_context *ctx);

int vma_debug = 1;

static char *output_path = "./vma.out";
static char *input_path;
static vma_vaddr_t load_va = 0x00A00000U;

static void print_help(void)
{
	fprintf(stderr, "vma assembler\n\n");
	fprintf(stderr, "help is currently TBD.\n\n");
}

static struct option long_options[] = {
	{ "help", no_argument, 0, 'h' },
	{ "debug", no_argument, &vma_debug, 1 },
	{ "load-va", required_argument, 0, 'l' },
	{ "output", required_argument, 0, 'o' },
	{ 0, 0, 0, 0 }
};

static void parse_args(int argc, char *argv[])
{
	int option_char = 0;

	while (option_char != -1) {
		int option_index = 0;

		option_char = getopt_long(argc, argv, "hl:o:", long_options, &option_index);

		switch (option_char) {
			case -1:
				/* end of recognized options */
				break;

			case 0:
				/* long option as a flag (handled by getopt_long) */
				break;

			case 'l':
				load_va = strtol(optarg, NULL, 16);
				break;

			case 'o':
				output_path = strdup(optarg);
				break;

			case 'h':
			case '?':
				print_help();
				exit(0);
				break;

			default:
				print_help();
				exit(1);
				break;
		}
	}

	while (optind < argc) {
		/* non-options (input files) */
		input_path = argv[optind];
		optind++;
	}
}

int main(int argc, char *argv[])
{
	int rv = 0;
	FILE *input = NULL;
	struct vma_context context;

	parse_args(argc, argv);

	if (!input_path) {
		vma_abort("no input file.");
	}

	vma_debug_print("input file: %s", input_path);
	context.input = fopen(input_path, "r");
	if (!context.input) {
		vma_abort("could not open input file '%s'.", input_path);
	}

	vma_debug_print("output file: %s", output_path);
	context.output = fopen(output_path, "wb");
	if (!context.output) {
		vma_abort("could not open input file '%s'.", input_path);
	}

	vma_debug_print("stage: parser");
	vma_parse_input(&context);
	vma_abort_on_errors();

	context.start_va = load_va;
	context.bss_va = 0;
	context.end_va = 0;

	vma_debug_print("stage: assembler");
	vma_assemble(&context);
	vma_abort_on_errors();

	/* TBD: write the result */
	vma_debug_print("stage: writer");
	vma_generate(&context);
	vma_abort_on_errors();

	fclose(context.output);
	fclose(context.input);

	return rv;
}
