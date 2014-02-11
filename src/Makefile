#
# Makefile for root source
#

# Main target in this dir
TARGET:=mette-$(TARGET_ARCH)

# What to build from
SOURCES:= \
	main.c \

# Additional libs to link
LIBS:= \
	mette-vm.a \
	mette-vmt.a \
	mette-vm.a \
	mette-rtl.a \
	mette-sysdep.a \

# Any additional subtargets here
SUBTARGETS:= \
	sysdep \
	rtl \
	vm_arch \
	vm_thunks \
	vm \
	vma \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc

# Override CFLAGS
CFLAGS+=-I$(SOURCES_ROOT)/sysdep/ -I$(SOURCES_ROOT)/vm/ -I$(SOURCES_ROOT)/rtl/ 
LDFLAGS+=-nostdlib --script=$(TARGET).ldscript -Map=$(TARGET_DIR)/$(TARGET).map

# Additional rules
