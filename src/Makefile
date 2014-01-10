#
# Makefile for root source
#

# Main target in this dir
TARGET:= \
	mette \

# What to build from
SOURCES:= \
	main.c \

# Additional libs to link
LIBS:= \
	vm/mette-vm.a

# Any additional subtargets here
SUBTARGETS:= \
	vm \
	rtl \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc

# Override CFLAGS
CFLAGS+=-I$(SOURCES_ROOT)/vm/
LDFLAGS+=-Wl,--gc-sections -Wl,-Map,mette.map

# Additional rules
