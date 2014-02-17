#
# Generic rules for making binaries
#

#
# Making
#

.PHONY: all $(SUBTARGETS)

all: prebuild $(SUBTARGETS) $(TARGET) $(LIBTARGET)

prebuild:
	mkdir -p $(BUILD_DIR)

$(SUBTARGETS): 
	$(MAKE) -C $@ all

ifneq ($(MAKECMDGOALS),distclean)
include $(patsubst %.c, $(BUILD_DIR)/%.d, $(SOURCES))
endif

$(BUILD_DIR)/%.d: %.c
	-$(CC) -MM -MG $(CFLAGS) $< | sed 's,\($*\)\.o[ :]*,$(BUILD_DIR)/\1.o $@ : ,g' > $@;

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.S
	$(CC) -c $(CFLAGS) -o $@ $<

OBJECTS:= \
	$(OBJECTS) \
	$(SOURCES:.c=.o) \
	$(ASMSOURCES:.S=.o)

REAL_OBJECTS:=$(patsubst %, $(BUILD_DIR)/%, $(OBJECTS))
REAL_LIBS:=$(patsubst %, $(TARGET_DIR)/%, $(LIBS))
REAL_TARGET:=$(TARGET_DIR)/$(TARGET)
REAL_LIBTARGET:=$(TARGET_DIR)/$(LIBTARGET)

.PHONY: $(TARGET) $(LIBTARGET)

$(TARGET): $(REAL_TARGET)

$(REAL_TARGET): $(REAL_OBJECTS) $(REAL_LIBS)
	$(LD) $(LDFLAGS) -o $@ $^ $(REAL_LIBS)
	$(SIZE) $@
	objdump -h $@

$(LIBTARGET): $(REAL_LIBTARGET)

$(REAL_LIBTARGET): $(REAL_OBJECTS)
	$(AR) rcs $@ $^
	$(SIZE) $@

#
# Cleaning
#

.PHONY: clean distclean

clean:

distclean:
	$(RM) $(GENERATED)
	for target in $(SUBTARGETS) ; do $(MAKE) -C $$target $@ ; done

