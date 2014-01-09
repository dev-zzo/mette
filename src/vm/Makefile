
CFLAGS_FOR_TARGET+=-I./ -I../sysdep

OBJECTS:= \
	vm_core.o \
	vm_stack.o \
	vm_thunks.o \
	vm_thunks_libc.o \
	vm_loader.o \
	vm_debug.o \

TARGETS:= \
	mette-vm.a \
	vma \

GENERATORS:= \
	vm_gen_opcodes \

GENERATED:= \
	vm_opcodes.h \
	vm_opcodes_switch.tab \
	vma_lexer.ops.tab \
	vma_insn.ops.tab \
	vma_lexer.c \
	vma_parser.c \
	vma_tokens.h \
	vma_parser.txt \

.PHONY: all vm_asm

all: $(GENERATORS) $(TARGETS)

#
# VM
#

mette-vm.a: $(OBJECTS)
	$(AR_FOR_TARGET) rcs $@ $^
	$(RANLIB_FOR_TARGET) $@

$(OBJECTS): %.o: %.c
	$(CC_FOR_TARGET) -c $(CFLAGS_FOR_TARGET) -o $@ $<
	$(SIZE_FOR_TARGET) $@

vm_core.o: vm_opcodes_switch.tab

vm_opcodes.h vm_opcodes_switch.tab vma_l.ops.tab vma_insn.ops.tab: vm_gen_opcodes
	./vm_gen_opcodes

vm_gen_opcodes: vm_gen_opcodes.c vm_opcodes.in
	$(CC_FOR_HOST) $(CFLAGS_FOR_HOST) -o $@ $<

#
# VM assembler
#

VMA_OBJECTS:= \
	vma_parser.o \
	vma_lexer.o \
	vma_misc.o \
	vma_asm.o \
	vma_out.o \
	vma.o

vma: $(VMA_OBJECTS)
	$(LD_FOR_HOST) $(LDFLAGS_FOR_HOST) -o $@ $^

vma_parser.c: vma_parser.y
	bison --defines=./vma_tokens.h --report=all --report-file=vma_parser.txt -o $@ $<

vma_lexer.c: vma_lexer.l
	flex -f -o $@ $<

$(VMA_OBJECTS): %.o: %.c
	$(CC_FOR_HOST) -c $(CFLAGS_FOR_HOST) -o $@ $<


clean:
	rm -f $(OBJECTS) $(VMA_OBJECTS) *.i *.s
	rm -f $(TARGETS)
	rm -f $(GENERATORS) $(GENERATED)

