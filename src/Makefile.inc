#
# Generic rules for making binaries
#

#
# Making
#

.PHONY: all $(SUBTARGETS)

all: prebuild $(SUBTARGETS) $(TARGET)

$(SUBTARGETS): 
	$(MAKE) -C $@ all

ifneq ($(MAKECMDGOALS),distclean)
include $(SOURCES:.c=.d)
endif

%.d: %.c
	-$(CC) -MM -MG $(CFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@;

$(BUILD_DIR)/%.o: %.c
	mkdir -p `dirname $@`
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.S
	mkdir -p `dirname $@`
	$(CC) -c $(CFLAGS) -o $@ $<

prebuild:

OBJECTS:= \
	$(OBJECTS) \
	$(SOURCES:.c=.o) \
	$(ASMSOURCES:.S=.o)

REAL_OBJECTS:=$(patsubst %, $(BUILD_DIR)/%, $(OBJECTS))
REAL_LIBS:=$(patsubst %, $(TARGET_DIR)/%, $(LIBS))
REAL_TARGET:=$(TARGET_DIR)/$(TARGET)

.PHONY: $(TARGET)

$(TARGET): $(REAL_TARGET)

$(REAL_TARGET): $(REAL_OBJECTS) $(REAL_LIBS)
ifeq ($(suffix $(TARGET)), .a)
	$(AR) rcs $@ $^
	$(SIZE) $@
else
	$(LD) $(LDFLAGS) -o $@ $^ $(REAL_LIBS)
ifeq ("$(LD)", "$(LD_FOR_TARGET)")
	$(SIZE) $@
	objdump -h $@
endif
endif

#
# Cleaning
#

.PHONY: clean distclean

clean:

distclean:
	$(RM) $(TARGET) *.d $(SOURCES:.c=.i) $(SOURCES:.c=.s) $(GENERATED)
	for target in $(SUBTARGETS) ; do $(MAKE) -C $$target $@ ; done

