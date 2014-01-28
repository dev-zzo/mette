#
# Makefile for VM thunks library
#

# Main target in this dir
TARGET:=mette-vmt.a

# What to build from
SOURCES_RAW:= \
	vmt_rtl.c \
	vmt_sockets.c \
	vmt_select.c \
	vmt_misc.c \
	vmt_io.c \

GENERATED:= \
	vmt_thunks.c \

SOURCES:= \
	$(SOURCES_RAW) \
	$(GENERATED) \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc

# Override CFLAGS
CFLAGS+=-I$(SOURCES_ROOT)/vm -I$(SOURCES_ROOT)/rtl -I$(SOURCES_ROOT)/sysdep

vmt_thunks.c: $(SOURCES_RAW)
	rm -f $@
	echo '#include "vm_thunks.h"' >> $@
	../vm/vm_gen_thunks.pl $^ >> $@
