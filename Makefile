#
# Root Makefile for mette-alpha
#

export TARGET_ARCH:=i386
#export TARGET_ARCH:=mips


ifeq ($(TARGET_ARCH), mips)
  TOOLS_SPEC:=mips-unknown-linux-uclibc
include Makefile.local
  CFLAGS_FOR_TARGET:=-DTARGET_IS_BE=1 -mdivide-traps -mno-check-zero-division -mno-abicalls -mno-shared #-save-temps
endif
ifeq ($(TARGET_ARCH), arm)
  TOOLS_SPEC:=arm-none-linux-uclibcgnueabi
include Makefile.local
  CFLAGS_FOR_TARGET:=-DTARGET_IS_BE=0
endif
ifeq ($(TARGET_ARCH), i386)
  TOOLS_PATH:=
  CFLAGS_FOR_TARGET:=-DTARGET_IS_BE=0 -g
endif

# Target build tools
export CC_FOR_TARGET:=$(TOOLS_PATH)gcc
export LD_FOR_TARGET:=$(TOOLS_PATH)ld
export AR_FOR_TARGET:=$(TOOLS_PATH)ar
export RANLIB_FOR_TARGET:=$(TOOLS_PATH)ranlib
export SIZE_FOR_TARGET:=$(TOOLS_PATH)size

# Target build options
CFLAGS_FOR_TARGET:=-Os -std=c99 -ffreestanding -Werror -I./ -DTARGET_ARCH_$(TARGET_ARCH)=1 $(CFLAGS_FOR_TARGET)
LDFLAGS_FOR_TARGET:=-g -static $(LDFLAGS_FOR_TARGET)
export CFLAGS_FOR_TARGET
export LDFLAGS_FOR_TARGET


# Host build tools
export CC_FOR_HOST:=gcc
export LD_FOR_HOST:=gcc
export AR_FOR_HOST:=ar
export RANLIB_FOR_HOST:=ranlib
export SIZE_FOR_HOST:=size

export CFLAGS_FOR_HOST:=-g -D_BSD_SOURCE=1
export LDFLAGS_FOR_HOST:=-g

export SOURCES_ROOT=$(shell pwd)/src
export BUILD_ROOT=$(shell pwd)/.objs

.PHONY: all src clean

all: src

src:
	mkdir -p $(BUILD_ROOT)
	mkdir -p $(BUILD_ROOT)/$(TARGET_ARCH)
	$(MAKE) -C src all

clean:
	rm -rf $(BUILD_ROOT)

distclean: clean
	$(MAKE) -C src distclean
