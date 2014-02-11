#
# Makefile for VM core library
#

# Main target in this dir
TARGET:=mette-vm.a

# What to build from
SOURCES:= \
	vm_core.c \
	vm_stack.c \
	vm_thunks.c \
	vm_loader.c \
	vm_debug.c \

GENERATED:= \
	vm_gen_opcodes.flag \
	vm_opcodes.switch.tab \
	vm_opcodes.codes.tab \
	vm_opcodes.names.tab \
	vm_gen_opcodes \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc

# Override CFLAGS
CFLAGS+=-I$(SOURCES_ROOT)/sysdep -I$(SOURCES_ROOT)/rtl -I$(SOURCES_ROOT)/vm_arch
