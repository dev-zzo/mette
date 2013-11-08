#
# Root Makefile for mette-alpha
#

TARGET_ARCH:=i386
export TARGET_ARCH

# Build tools
export CC:=gcc
export LD:=gcc
export AR:=ar
export RANLIB:=ranlib

# Build options
CFLAGS:=-Os 
export CFLAGS
LDFLAGS:=-s
export LDFLAGS

.PHONY: all src clean

all: src

src:
	$(MAKE) -C $@

clean:
	$(MAKE) -C src clean

