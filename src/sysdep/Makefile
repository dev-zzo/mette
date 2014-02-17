#
# Makefile for VM system dependent library
#

LIBTARGET:=mette-sysdep.a

SOURCES:= \
	syscalls.c \
	syscalls-$(TARGET_ARCH).c \
	xstring.c \

ASMSOURCES:= \
	startup-$(TARGET_ARCH).S \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.target.inc
