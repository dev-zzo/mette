
CFLAGS+=-I./

OBJECTS= \
	imp_alloc.o \
	imp_misc.o \
	imp_test.o \

.PHONY: all clean $(TARGET_ARCH)/imp-$(TARGET_ARCH).a

all: imp $(TARGET_ARCH)/imp-$(TARGET_ARCH).a

imp: $(OBJECTS) $(TARGET_ARCH)/imp-$(TARGET_ARCH).a
	$(LD) $(LDFLAGS) -o $@ $^

$(TARGET_ARCH)/imp-$(TARGET_ARCH).a:
	$(MAKE) -C $(TARGET_ARCH)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o imp
	$(MAKE) -C $(TARGET_ARCH) clean

