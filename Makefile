#
# Root Makefile for mette-alpha
#

#TARGET_ARCH:=i386
TARGET_ARCH:=mips
export TARGET_ARCH

ifeq ($(TARGET_ARCH), mips)
TOOLS_SPEC:=mips-unknown-linux-uclibc
TOOLS_PATH:=$(HOME)/x-tools/$(TOOLS_SPEC)/bin/$(TOOLS_SPEC)-
CFLAGS_FOR_TARGET:=-DTARGET_IS_BE=1 -mdivide-traps -mno-check-zero-division # -save-temps
endif
ifeq ($(TARGET_ARCH), arm)
TOOLS_SPEC:=arm-none-linux-uclibcgnueabi
TOOLS_PATH:=$(HOME)/x-tools/$(TOOLS_SPEC)/bin/$(TOOLS_SPEC)-
CFLAGS_FOR_TARGET:=-DTARGET_IS_BE=0
endif
ifeq ($(TARGET_ARCH), i386)
TOOLS_PATH:=
CFLAGS_FOR_TARGET:=-DTARGET_IS_BE=0
endif

# Target build tools
export CC_FOR_TARGET:=$(TOOLS_PATH)gcc
export LD_FOR_TARGET:=$(TOOLS_PATH)gcc
export AR_FOR_TARGET:=$(TOOLS_PATH)ar
export RANLIB_FOR_TARGET:=$(TOOLS_PATH)ranlib
export SIZE_FOR_TARGET:=$(TOOLS_PATH)size

# Target build options
CFLAGS_FOR_TARGET:=-Os -std=c99 -ffreestanding -mno-abicalls -mno-shared -DTARGET_ARCH_$(TARGET_ARCH)=1 $(CFLAGS_FOR_TARGET)
LDFLAGS_FOR_TARGET:=-s -static $(LDFLAGS_FOR_TARGET)
export CFLAGS_FOR_TARGET
export LDFLAGS_FOR_TARGET


# Host build tools
export CC_FOR_HOST:=gcc
export LD_FOR_HOST:=gcc

export CFLAGS_FOR_HOST:=-O2 -D_BSD_SOURCE=1
export LDFLAGS_FOR_HOST:=-s


.PHONY: all src clean

all: src

src:
	$(MAKE) -C $@

clean:
	$(MAKE) -C src clean

