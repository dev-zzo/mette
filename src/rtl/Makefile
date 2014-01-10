#
# Makefile for VM RTL code library
#

# Main target in this dir
TARGET:= \
	mette-rtl.a \

# What to build from
SOURCES:= \
	rtl_memory.c \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc

# Override CFLAGS
CFLAGS+=-I$(SOURCES_ROOT)/sysdep -I$(SOURCES_ROOT)/vm
