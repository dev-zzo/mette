#ifndef __mette_vm_asm_h_included
#define __mette_vm_asm_h_included

#include <stddef.h>

struct vma_insn_node;

/*
 * Symbols
 */

struct vma_symbol {
	struct vma_symbol *next;
	const char *name;
	struct vma_insn_node *location;
};

struct vma_symref {
	union {
		const char *name;
		struct vma_symbol *sym;
	} u;
	unsigned resolved : 1; /* nonzero if this is a valid ref */
	unsigned ncall : 1; /* 1 if refers to a native routine */
};

extern struct vma_symbol *vma_lookup_symbol(const char *name);
extern struct vma_symbol *vma_define_symbol(const char *name);
extern void vma_init_symref(struct vma_symref *ref, const char *name);

/*
 * Expressions
 */

enum vma_expr_type {
	EXPR_CONSTANT,
	EXPR_SYMREF,
	EXPR_OR,
	EXPR_AND,
	EXPR_XOR,
	EXPR_ADD,
	EXPR_SUB,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_NEG,
	EXPR_NOT,
};

struct vma_expr_node {
	struct vma_expr_node *next; /* in expr_list */
	enum vma_expr_type type;
	union {
		int const_int;
		struct vma_expr_node *child[2]; /* lhs, rhs */
		struct vma_symref symref;
	} u;
};

extern struct vma_expr_node *vma_build_constant_expr(int value);
extern struct vma_expr_node *vma_build_symref_expr(const char *name);
extern struct vma_expr_node *vma_build_parent_expr
(
	enum vma_expr_type type,
	struct vma_expr_node *a,
	struct vma_expr_node *b
);

struct vma_expr_list {
	struct vma_expr_node *head;
	struct vma_expr_node *tail;
};

extern struct vma_expr_list *vma_create_expr_list();
extern struct vma_expr_list *vma_append_expr_list(struct vma_expr_list *list, struct vma_expr_node *node);

/*
 * Instructions
 */

enum vma_insn_type {
	INSN_ADD,
	INSN_SUB,
	INSN_MULU,
	INSN_MULS,
	INSN_DIVU,
	INSN_DIVS,
	INSN_AND,
	INSN_OR,
	INSN_XOR,
	INSN_NOT,
	INSN_LSL,
	INSN_LSR,
	INSN_ASR,
	INSN_CMP_LT,
	INSN_CMP_GT,
	INSN_CMP_LE,
	INSN_CMP_GE,
	INSN_CMP_B,
	INSN_CMP_A,
	INSN_CMP_BE,
	INSN_CMP_AE,
	INSN_CMP_EQ,
	INSN_CMP_NE,
	INSN_LDC_0,
	INSN_LDC_1,
	INSN_LDC_2,
	INSN_LDC_8_U,
	INSN_LDC_8_S,
	INSN_LDC_32,
	INSN_LDM_8_U,
	INSN_LDM_8_S,
	INSN_LDM_16_U,
	INSN_LDM_16_S,
	INSN_LDM_32,
	INSN_STM_8,
	INSN_STM_16,
	INSN_STM_32,
	INSN_LOCALS,
	INSN_LDLOC,
	INSN_STLOC,
	INSN_DUP,
	INSN_SWAP,
	INSN_POP,
	INSN_BR_S,
	INSN_BR_L,
	INSN_BR_T,
	INSN_BR_F,
	INSN_CALL,
	INSN_RET,
	INSN_ICALL,
	INSN_IJMP,
	INSN_NCALL,

	INSN_DEFB,
	INSN_DEFH,
	INSN_DEFW,
	INSN_RESB,
	INSN_RESH,
	INSN_RESW,
};

struct vma_insn_node {
	struct vma_insn_node *next;
	enum vma_insn_type type;
	union {
		struct vma_symref symref;
		struct vma_expr_node *expr;
		struct vma_expr_list *expr_list;
	} u;
};

extern struct vma_insn_node *vma_build_insn(enum vma_insn_type type);

/*
 * Translation unit
 */

struct vma_unit {
	struct vma_insn_node *head;
	struct vma_insn_node *tail;
};

extern struct vma_unit *vma_build_unit();
extern struct vma_unit *vma_append_unit(struct vma_unit *unit, struct vma_insn_node *node);

/*
 * Parsing
 */

struct vma_parser_state {
	struct vma_unit *unit;
};

/* 
 * Helper functions
 */

extern void *vma_malloc(size_t count);
extern void vma_free(void *ptr);

extern void vma_error(const char *format, ...);
extern void vma_abort(const char *format, ...);

extern int vma_errors;
extern int vma_debug;

#endif // __mette_vm_asm_h_included
