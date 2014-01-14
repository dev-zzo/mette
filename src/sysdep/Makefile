#
# Makefile for VM system dependent library
#

TARGET:= \
	mette-sysdep.a \

SOURCES:= \
	syscalls.c \
	$(TARGET_ARCH)/syscalls.c \
	xstring.c \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc
