
CFLAGS+=-I../

OBJECTS= \
	imp_add.o \
	imp_sub.o \

TARGETS= \
	imp-$(TARGET_ARCH).a

.PHONY: all

all: $(TARGETS)

imp-$(TARGET_ARCH).a: $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGETS)

