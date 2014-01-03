
CFLAGS+=-I./ -I./vm/

OBJECTS:= \
	main.o \

TARGETS:= \
	mette \

SUBTARGETS:= \
	vm \
	

.PHONY: all clean $(SUBTARGETS) $(SUBTARGETS_clean)

all: $(SUBTARGETS) $(TARGETS)

mette: main.o vm/mette-vm.a
	$(LD) $(LDFLAGS) -Wl,--gc-sections -o $@ -Wl,-Map,mette.map $^
	$(SIZE) $@
	
$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
	$(SIZE) $@

$(SUBTARGETS):
	$(MAKE) -C $@ all

#
# Clean rules
#
SUBTARGETS_clean:=$(addsuffix _clean, $(SUBTARGETS))

clean: $(SUBTARGETS_clean)
	$(RM) -f $(OBJECTS) mette.map

$(SUBTARGETS_clean): 
	$(MAKE) -C $(subst _clean,,$@) clean

