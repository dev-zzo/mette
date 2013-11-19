#
# Root Makefile for mette-alpha
#

TARGET_ARCH:=i386
export TARGET_ARCH

# Build tools
export CC:=gcc
export HOST_CC:=gcc
export LD:=gcc
export AR:=ar
export RANLIB:=ranlib
export SIZE:=size

# Build options
CFLAGS:=-Os -std=c99
export CFLAGS
LDFLAGS:=-s
export LDFLAGS

export HOST_CFLAGS:=-O2 -std=c99

.PHONY: all src clean

all: src

src:
	$(MAKE) -C $@

clean:
	$(MAKE) -C src clean

