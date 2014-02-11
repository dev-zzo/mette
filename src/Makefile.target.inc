#
# Generic rules for making binaries for target
#

CC:=$(CC_FOR_TARGET)
LD:=$(LD_FOR_TARGET)
AR:=$(AR_FOR_TARGET)
RANLIB:=$(RANLIB_FOR_TARGET)
SIZE:=$(SIZE_FOR_TARGET)

CFLAGS:=$(CFLAGS_FOR_TARGET)
LDFLAGS:=$(LDFLAGS_FOR_TARGET)

BUILD_DIR:=$(BUILD_ROOT)/$(shell basename `dirname \`pwd\`/x`)-$(TARGET_ARCH)
TARGET_DIR:=$(BUILD_ROOT)/$(TARGET_ARCH)

include $(SOURCES_ROOT)/Makefile.inc
