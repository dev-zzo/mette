#
# Makefile for VM arch files
#

.PHONY: all
all: vm_opcodes.switch.tab vm_opcodes.codes.tab vm_opcodes.names.tab

vm_opcodes.switch.tab vm_opcodes.codes.tab vm_opcodes.names.tab: vm_gen_opcodes
	./vm_gen_opcodes

vm_gen_opcodes: vm_gen_opcodes.c vm_opcodes.in
	$(CC) -o $@ $<

.PHONY: clean
clean:
