#include "vma.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

extern int vma_parse_input(vma_context_t *ctx);

int vma_debug = 1;

static char *output_path = "./vma.out";
static char *input_path;

static void print_help(void)
{
	fprintf(stderr, "vma assembler\n\n");
	fprintf(stderr, "help is currently TBD.\n\n");
}

static struct option long_options[] = {
	{ "help", no_argument, 0, 'h' },
	{ "debug", no_argument, &vma_debug, 1 },
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
	vma_context_t context;

	vma_context_init(&context);
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

	context.input_name = strrchr(input_path, '/');
	if (!context.input_name) {
		context.input_name = input_path;
	} else {
		++context.input_name;
	}

	vma_debug_print("stage: parser");
	vma_parse_input(&context);
	vma_abort_on_errors();
	
	fclose(context.input);

	context.start_va = 0; /* Not used for now. */
	context.bss_va = 0;
	context.end_va = 0;

	vma_debug_print("stage: assembler");
	vma_assemble(&context);
	vma_abort_on_errors();

	vma_debug_print("stage: writer");
	vma_generate(&context);
	vma_abort_on_errors();

	fclose(context.output);

	return rv;
}
