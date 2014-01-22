#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "vm_misc.h"
#include "vm_opcodes.in"

static char *sanitize_mnem(const char *text)
{
	static char buffer[256];
	int i;
	for (i = 0; text[i] != '\0'; ++i) {
		buffer[i] = text[i] == '.' ? '_' : text[i];
	}
	buffer[i] = '\0';
	return buffer;
}

static uint8_t make_opcode_value(int index, int argcount)
{
	return index | (argcount << 6);
}

int main()
{
	FILE *fd;
	int index;
	
	fprintf(stdout, "Opcode count: %d\n", ARRAY_SIZE(opcodes));
	if (ARRAY_SIZE(opcodes) >= 64) {
		fprintf(stdout, "Too many opcodes: max %d\n", 64);
		return 1;
	}
	
	fprintf(stdout, "Generating vm_opcodes.switch.tab.\n");
	fd = fopen("vm_opcodes.switch.tab", "w");
	fprintf(fd, "/* NOTE: AUTOGENERATED FILE. All editions will be discarded. */\n\n");
	fprintf(fd, "static const unsigned short offtab[64] = {\n");
	for (index = 0; index < ARRAY_SIZE(opcodes); ++index) {
		fprintf(fd, "\t&&op_%s - &&op_invalid,\n", sanitize_mnem(opcodes[index].mnemonic));
	}
	fprintf(fd, "};\n\n");
	fclose(fd);
	
	fprintf(stdout, "Generating vm_opcodes.codes.tab.\n");
	fd = fopen("vm_opcodes.codes.tab", "w");
	fprintf(fd, "/* NOTE: AUTOGENERATED FILE. All editions will be discarded. */\n\n");
	fprintf(fd, "static const uint8_t vm_insn_to_opcode[64] = {");
	for (index = 0; index < ARRAY_SIZE(opcodes); ++index) {
		if ((index & 0x07) == 0) {
			fputs("\n\t", fd);
		}
		fprintf(fd, "0x%02X, ", make_opcode_value(index, opcodes[index].stack_in));
	}
	fprintf(fd, "\n};\n\n");
	fclose(fd);
	
	fprintf(stdout, "Generating vm_opcodes.names.tab.\n");
	fd = fopen("vm_opcodes.names.tab", "w");
	fprintf(fd, "/* NOTE: AUTOGENERATED FILE. All editions will be discarded. */\n\n");
	fprintf(fd, "static const char *vm_insn_to_name[64] = {\n");
	for (index = 0; index < ARRAY_SIZE(opcodes); ++index) {
		fprintf(fd, "\t\"%s\",\n", opcodes[index].mnemonic);
	}
	fprintf(fd, "};\n\n");
	fclose(fd);
	
	return 0;
}


