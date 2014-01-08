
CFLAGS_FOR_TARGET+=-I./ -I./vm/

OBJECTS:= \
	main.o \

TARGETS:= \
	mette \

SUBTARGETS:= \
	vm \
	

.PHONY: all clean $(SUBTARGETS) $(SUBTARGETS_clean)

all: $(SUBTARGETS) $(TARGETS)

mette: main.o vm/mette-vm.a
	$(LD_FOR_TARGET) $(LDFLAGS_FOR_TARGET) -Wl,--gc-sections -o $@ -Wl,-Map,mette.map $^
	$(SIZE_FOR_TARGET) $@
	
$(OBJECTS): %.o: %.c
	$(CC_FOR_TARGET) -c $(CFLAGS_FOR_TARGET) -o $@ $<
	$(SIZE_FOR_TARGET) $@

$(SUBTARGETS):
	$(MAKE) -C $@ all

#
# Clean rules
#
SUBTARGETS_clean:=$(addsuffix _clean, $(SUBTARGETS))

clean: $(SUBTARGETS_clean)
	$(RM) -f $(OBJECTS) *.i *.s mette.map

$(SUBTARGETS_clean): 
	$(MAKE) -C $(subst _clean,,$@) clean

