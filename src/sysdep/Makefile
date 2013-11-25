
CFLAGS+=-I./

OBJECTS:= \
	xmalloc.o \
	xstring.o \

TARGETS:= \
	mette-sysdep.a \

.PHONY: all

all: $(TARGETS)

mette-sysdep.a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
	$(SIZE) $@

clean:
	rm -f $(OBJECTS) $(TARGETS)

