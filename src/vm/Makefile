
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
	vm_asm \

GENERATORS:= \
	vm_gen_opcodes \

GENERATED:= \
	vm_asm.l.ops.tab \
	vm_opcodes_switch.tab \
	vm_asm.lexer.c \
	vm_asm.parser.c \
	vm_asm.tokens.h \

.PHONY: all vm_asm

all: $(GENERATORS) $(TARGETS)

mette-vm.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
	$(SIZE) $@

vm_core.o: vm_opcodes_switch.tab

vm_opcodes.h vm_opcodes_switch.tab vm_asm.l.ops.tab: vm_gen_opcodes
	./vm_gen_opcodes

vm_gen_opcodes: vm_gen_opcodes.c vm_opcodes.in vm_opcodes_traits.h
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $<

vm_asm: vm_asm.y vm_asm.l
	bison --defines=./vm_asm.tokens.h --output=vm_asm.parser.c vm_asm.y
	flex -f -o vm_asm.lexer.c vm_asm.l
	
clean:
	rm -f $(OBJECTS) *.i *.s
	rm -f $(TARGETS)
	rm -f $(GENERATORS) $(GENERATED)

