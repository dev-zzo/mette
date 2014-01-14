#
# Makefile for VM RTL code library
#

# Main target in this dir
TARGET:=mette-rtl.a

# What to build from
SOURCES:= \
	rtl_memory.c \
	rtl_strbuf.c \
	rtl_sockets.c \
	rtl_select.c \
	rtl_thunks.c \

GENERATED:= \
	rtl_thunks.c \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc

# Override CFLAGS
CFLAGS+=-I$(SOURCES_ROOT)/sysdep -I$(SOURCES_ROOT)/vm

rtl_thunks.c: $(SOURCES)
	rm -f $@
	echo "#include \"vm_thunks.h\"" >> $@
	../vm/vm_gen_thunks/vm_gen_thunks $^ >> $@
