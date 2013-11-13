
SUBTARGETS:=imp vm

.PHONY: all $(SUBTARGETS)

all: $(SUBTARGETS)

$(SUBTARGETS):
	$(MAKE) -C $@ all

clean:
	$(MAKE) -C imp clean

