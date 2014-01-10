#
# VM assembler
#

# Main target in this dir
TARGET:= \
	vma \

# What to build from
SOURCES:= \
	vma_lexer.c \
	vma_parser.c \
	vma_misc.c \
	vma_asm.c \
	vma_out.c \
	vma.c \

GENERATED:= \
	vma_tokens.h \
	vma_lexer.c \
	vma_parser.c \
	vma_parser.txt \

# Include either target or host
include $(SOURCES_ROOT)/Makefile.host.inc

# Override CFLAGS
CFLAGS+=-I$(SOURCES_ROOT)/vm/

# Additional rules

vma_parser.c: vma_parser.y
	bison --defines=./vma_tokens.h --report=all --report-file=vma_parser.txt -o $@ $<

vma_lexer.c: vma_lexer.l
	flex -f -o $@ $<

