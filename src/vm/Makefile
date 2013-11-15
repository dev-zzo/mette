
CFLAGS+=-I./

OBJECTS:= \
	vm_core.o \
	vm_stack.o \
	vm_thunks.o \
	vm_loader.o \

TARGETS:= \
	mette-vm.a \

GENERATORS:= \
	vm_gen_opcodes \

GENERATED:= \
	vm_opcodes_traits.tab \
	vm_opcodes_switch.tab \

.PHONY: all

all: $(GENERATORS) $(TARGETS)

mette-vm.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
	$(SIZE) $@

vm_core.o: vm_opcodes_traits.tab vm_opcodes_switch.tab

vm_opcodes_traits.tab vm_opcodes_switch.tab: vm_gen_opcodes
	./vm_gen_opcodes

vm_gen_opcodes: vm_gen_opcodes.c vm_opcodes.in vm_opcodes_traits.h
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $<

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGETS)
	rm -f $(GENERATORS) $(GENERATED)

