
CFLAGS+=-I./ -I../sysdep

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
	vm_opcodes_switch.tab \
	vma_lexer.ops.tab \
	vma_lexer.c \
	vma_parser.c \
	vma_tokens.h \

.PHONY: all vm_asm

all: $(GENERATORS) $(TARGETS)

mette-vm.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
	$(SIZE) $@

vm_core.o: vm_opcodes_switch.tab

vm_opcodes.h vm_opcodes_switch.tab vma_l.ops.tab: vm_gen_opcodes
	./vm_gen_opcodes

vm_gen_opcodes: vm_gen_opcodes.c vm_opcodes.in
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $<

vma: vma.y vma.l
	bison --defines=./vma_tokens.h --output=vma_parser.c vma.y
	flex -f -o vma_lexer.c vma.l
	
clean:
	rm -f $(OBJECTS) *.i *.s
	rm -f $(TARGETS)
	rm -f $(GENERATORS) $(GENERATED)

