#
# Root Makefile for mette-alpha
#

#TARGET_ARCH:=i386
TARGET_ARCH:=mips
export TARGET_ARCH

ifeq ($(TARGET_ARCH), mips)
TOOLS_PREFIX:=/home/user/x-tools/mips-unknown-linux-uclibc/bin/mips-unknown-linux-uclibc-
CFLAGS_FOR_TARGET:=-DTARGET_IS_BE=1 -mdivide-traps -mno-check-zero-division
endif
ifeq ($(TARGET_ARCH), i386)
endif

# Build tools
export HOST_CC:=gcc
export CC:=$(TOOLS_PREFIX)gcc
export LD:=$(TOOLS_PREFIX)gcc
export AR:=$(TOOLS_PREFIX)ar
export RANLIB:=$(TOOLS_PREFIX)ranlib
export SIZE:=$(TOOLS_PREFIX)size

# Build options
CFLAGS:=-Os -std=c99 -ffreestanding -mno-abicalls -mno-shared -DTARGET_ARCH_$(TARGET_ARCH)=1 $(CFLAGS_FOR_TARGET)
LDFLAGS:=-s -static $(LDFALGS_FOR_TARGET)
export CFLAGS
export LDFLAGS

export HOST_CFLAGS:=-O2 -std=c99

.PHONY: all src clean

all: src

src:
	$(MAKE) -C $@

clean:
	$(MAKE) -C src clean

