#
# Generic rules for making binaries for host
#

CC:=$(CC_FOR_HOST)
LD:=$(LD_FOR_HOST)
AR:=$(AR_FOR_HOST)
RANLIB:=$(RANLIB_FOR_HOST)
SIZE:=$(SIZE_FOR_HOST)

CFLAGS:=$(CFLAGS_FOR_HOST)
LDFLAGS:=$(LDFLAGS_FOR_HOST)

include $(SOURCES_ROOT)/Makefile.inc
