#
# Generic rules for making binaries
#

OBJECTS:= \
	$(OBJECTS) \
	$(patsubst %.c, %.o, $(SOURCES)) \
	$(patsubst %.S, %.o, $(ASMSOURCES)) \

#
# Making
#

.PHONY: all $(SUBTARGETS)

all: $(SUBTARGETS) $(TARGET)

$(SUBTARGETS): 
	$(MAKE) -C $@ all

ifneq ($(MAKECMDGOALS),clean)
include $(SOURCES:.c=.d)
endif

%.d: %.c
	-$(CC) -MM -MG $(CFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@;

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.S
	$(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJECTS) $(LIBS)
ifeq ($(suffix $(TARGET)), .a)
	$(AR) rcs $@ $^
	$(SIZE) $@
else
	$(LD) $(LDFLAGS) -o $@ $^
	$(SIZE) $@
endif

#
# Cleaning
#

SUBTARGETS_clean:=$(addsuffix _clean, $(SUBTARGETS))
.PHONY: clean $(SUBTARGETS_clean)

$(SUBTARGETS_clean): 
	$(MAKE) -C $(subst _clean,,$@) clean

clean: $(SUBTARGETS_clean)
	$(RM) $(TARGET) *.o *.d $(patsubst %.c, %.i, $(SOURCES)) $(patsubst %.c, %.s, $(SOURCES)) $(GENERATED)

