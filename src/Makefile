
SUBTARGETS:=imp vm sysdep

.PHONY: all clean $(SUBTARGETS) $(SUBTARGETS_clean)

all: $(SUBTARGETS)

$(SUBTARGETS):
	$(MAKE) -C $@ all

#
# Clean rules
#
SUBTARGETS_clean:=$(addsuffix _clean, $(SUBTARGETS))

clean: $(SUBTARGETS_clean)

$(SUBTARGETS_clean): 
	$(MAKE) -C $(subst _clean,,$@) clean

