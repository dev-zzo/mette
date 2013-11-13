
CFLAGS+=-I./

OBJECTS:= \
	vm_core.o \
	vm_stack.o \

TARGETS:= \
	mette-vm.a

.PHONY: all

all: $(TARGETS)

mette-vm.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGETS)

