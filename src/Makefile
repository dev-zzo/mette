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
	vm/mette-vm.a \
	rtl/mette-rtl.a \
	sysdep/mette-sysdep.a \

# Any additional subtargets here
SUBTARGETS:= \
	sysdep \
	vm \
	rtl \

GENERATED:= \
	mette.map

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc

# Override CFLAGS
CFLAGS+=-I$(SOURCES_ROOT)/sysdep/ -I$(SOURCES_ROOT)/vm/ -I$(SOURCES_ROOT)/rtl/ 
LDFLAGS+=-nostdlib -Wl,--gc-sections -Wl,-Map,mette.map

# Additional rules
rtl: vm
