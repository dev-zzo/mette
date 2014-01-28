/*
 * VM assembly bison grammar.
 * ref: http://www.gnu.org/software/bison/manual/bison.html
 */

%debug

%{

#include "vma.h"
#include <stdio.h>

%}

%lex-param {vma_context_t *ctx}
%parse-param {vma_context_t *ctx}

%{

/* Globals... */
extern int yylex(vma_context_t *ctx);

static void yyerror(vma_context_t *ctx, const char *msg);

static void append_insn(vma_context_t *ctx, vma_insn_t *insn);

%}

%union {
	int const_int;
	struct {
		unsigned length;
		char *buffer;
	} text;
	vma_symbol_t *sym;
	vma_expr_t *expr;
	vma_expr_list_t *expr_list;
	vma_insn_t *insn;
	struct vma_unit *unit;
}

%token END 0 "end of file"
%token NEWLINE 1 "newline"
%type <sym> label
%type <expr> expr
%type <expr_list> expr_list
%type <insn> labeled_insn
%type <insn> addressable_insn
%type <insn> datadef_insn
%type <insn> datares_insn
%type <insn> machine_insn
%left '|'
%left '&'
%left '^'
%left '+' '-'
%left '*' '/'
%left NEG
%left NOT

%token <const_int> INTEGER
%token <text> IDENTIFIER
%token <text> STRINGT

%token KW_DEFS KW_DEFB KW_DEFH KW_DEFW
%token KW_RESB KW_RESH KW_RESW
%token KW_CONST

%token OP_ADD OP_SUB OP_MULU OP_MULS OP_DIVU OP_DIVS 
%token OP_AND OP_OR OP_XOR OP_NOT OP_LSL OP_LSR OP_ASR 
%token OP_CMP_LT OP_CMP_GT OP_CMP_B OP_CMP_A OP_CMP_EQ
%token OP_LDC_0 OP_LDC_1 OP_LDC_2 OP_LDC_8_U OP_LDC_8_S OP_LDC_32 OP_LEA 
%token OP_LDM_8_U OP_LDM_8_S OP_LDM_16_U OP_LDM_16_S OP_LDM_32 
%token OP_STM_8 OP_STM_16 OP_STM_32 
%token OP_LOCALS OP_LDLOC OP_STLOC 
%token OP_DUP OP_SWAP OP_POP 
%token OP_BR OP_BR_L OP_BR_T OP_BR_F 
%token OP_CALL OP_RET 
%token OP_ICALL OP_IJMP 
%token OP_NCALL

%printer { fprintf(yyoutput, "%08X", $$); } INTEGER;
%printer { fprintf(yyoutput, "%s", $$.buffer); } IDENTIFIER;

%%

unit
	: /* empty */
		{ ctx->insns_tail = ctx->insns_head = NULL; }
	| unit NEWLINE
		/* Eat newlines */
	| unit listed_stmt
		/* No action. */
	| unit dropped_stmt
		/* No action. */
	| unit error NEWLINE
		{
			vma_error("line %d: syntax error.", @2.first_line);
			yyerrok;
		}
;

listed_stmt
	: labeled_insn
		{ $1->line = @1.first_line; append_insn(ctx, $1); }
	| addressable_insn
		{ $1->line = @1.first_line; append_insn(ctx, $1); }
;

dropped_stmt
	: constdef_insn
		{ }
;

labeled_insn
	: label addressable_insn
		{ $$ = $2; $1->u.location = $2; }
;

addressable_insn
	: machine_insn
		{ $$ = $1; }
	| datadef_insn
		{ $$ = $1; }
	| datares_insn
		{ $$ = $1; }
;

constdef_insn
	: IDENTIFIER KW_CONST expr
		{ vma_symbol_t *c = vma_symtab_define(&ctx->constants, $1.buffer, 1); c->u.value = $3; }
;

datadef_insn
	: KW_DEFB expr_list
		{ $$ = vma_insn_build(INSN_DEFB); $$->u.expr_list = $2; }
	| KW_DEFH expr_list
		{ $$ = vma_insn_build(INSN_DEFH); $$->u.expr_list = $2; }
	| KW_DEFW expr_list
		{ $$ = vma_insn_build(INSN_DEFW); $$->u.expr_list = $2; }
	| KW_DEFS STRINGT
		{ $$ = vma_insn_build(INSN_DEFS); $$->u.text.length = $2.length; $$->u.text.buffer = $2.buffer; }
;

datares_insn
	: KW_RESB expr
		{ $$ = vma_insn_build(INSN_RESB); $$->u.expr = $2; }
	| KW_RESH expr
		{ $$ = vma_insn_build(INSN_RESH); $$->u.expr = $2; }
	| KW_RESW expr
		{ $$ = vma_insn_build(INSN_RESW); $$->u.expr = $2; }
;

machine_insn
	: OP_ADD
		{ $$ = vma_insn_build(INSN_ADD); }
	| OP_SUB
		{ $$ = vma_insn_build(INSN_SUB); }
	| OP_MULU
		{ $$ = vma_insn_build(INSN_MULU); }
	| OP_MULS
		{ $$ = vma_insn_build(INSN_MULS); }
	| OP_DIVU
		{ $$ = vma_insn_build(INSN_DIVU); }
	| OP_DIVS
		{ $$ = vma_insn_build(INSN_DIVS); }
	| OP_AND
		{ $$ = vma_insn_build(INSN_AND); }
	| OP_OR
		{ $$ = vma_insn_build(INSN_OR); }
	| OP_XOR
		{ $$ = vma_insn_build(INSN_XOR); }
	| OP_NOT
		{ $$ = vma_insn_build(INSN_NOT); }
	| OP_LSL
		{ $$ = vma_insn_build(INSN_LSL); }
	| OP_LSR
		{ $$ = vma_insn_build(INSN_LSR); }
	| OP_ASR
		{ $$ = vma_insn_build(INSN_ASR); }
	| OP_CMP_LT
		{ $$ = vma_insn_build(INSN_CMP_LT); }
	| OP_CMP_GT
		{ $$ = vma_insn_build(INSN_CMP_GT); }
	| OP_CMP_B
		{ $$ = vma_insn_build(INSN_CMP_B); }
	| OP_CMP_A
		{ $$ = vma_insn_build(INSN_CMP_A); }
	| OP_CMP_EQ
		{ $$ = vma_insn_build(INSN_CMP_EQ); }
	| OP_LDC_0
		{ $$ = vma_insn_build(INSN_LDC_0); }
	| OP_LDC_1
		{ $$ = vma_insn_build(INSN_LDC_1); }
	| OP_LDC_2
		{ $$ = vma_insn_build(INSN_LDC_2); }
	| OP_LDC_8_U expr
		{ $$ = vma_insn_build(INSN_LDC_8_U); $$->u.expr = $2; }
	| OP_LDC_8_S expr
		{ $$ = vma_insn_build(INSN_LDC_8_S); $$->u.expr = $2; }
	| OP_LDC_32 expr
		{ $$ = vma_insn_build(INSN_LDC_32); $$->u.expr = $2; }
	| OP_LEA IDENTIFIER
		{ $$ = vma_insn_build(INSN_LEA); vma_symref_init(&$$->u.symref, $2.buffer); }
	| OP_LDM_8_U
		{ $$ = vma_insn_build(INSN_LDM_8_U); }
	| OP_LDM_8_S
		{ $$ = vma_insn_build(INSN_LDM_8_S); }
	| OP_LDM_16_U
		{ $$ = vma_insn_build(INSN_LDM_16_U); }
	| OP_LDM_16_S
		{ $$ = vma_insn_build(INSN_LDM_16_S); }
	| OP_LDM_32
		{ $$ = vma_insn_build(INSN_LDM_32); }
	| OP_STM_8
		{ $$ = vma_insn_build(INSN_STM_8); }
	| OP_STM_16
		{ $$ = vma_insn_build(INSN_STM_16); }
	| OP_STM_32
		{ $$ = vma_insn_build(INSN_STM_32); }
	| OP_LOCALS expr
		{ $$ = vma_insn_build(INSN_LOCALS); $$->u.expr = $2; }
	| OP_LDLOC expr
		{ $$ = vma_insn_build(INSN_LDLOC); $$->u.expr = $2; }
	| OP_STLOC expr
		{ $$ = vma_insn_build(INSN_STLOC); $$->u.expr = $2; }
	| OP_DUP
		{ $$ = vma_insn_build(INSN_DUP); }
	| OP_SWAP
		{ $$ = vma_insn_build(INSN_SWAP); }
	| OP_POP
		{ $$ = vma_insn_build(INSN_POP); }
	| OP_BR IDENTIFIER
		{ $$ = vma_insn_build(INSN_BR); vma_symref_init(&$$->u.symref, $2.buffer); }
	| OP_BR_T IDENTIFIER
		{ $$ = vma_insn_build(INSN_BR_T); vma_symref_init(&$$->u.symref, $2.buffer); }
	| OP_BR_F IDENTIFIER
		{ $$ = vma_insn_build(INSN_BR_F); vma_symref_init(&$$->u.symref, $2.buffer); }
	| OP_CALL IDENTIFIER
		{ $$ = vma_insn_build(INSN_CALL); vma_symref_init(&$$->u.symref, $2.buffer); }
	| OP_RET
		{ $$ = vma_insn_build(INSN_RET); }
	| OP_ICALL
		{ $$ = vma_insn_build(INSN_ICALL); }
	| OP_IJMP
		{ $$ = vma_insn_build(INSN_IJMP); }
	| OP_NCALL IDENTIFIER
		{
			$$ = vma_insn_build(INSN_NCALL);
			$$->u.symref.u.sym = vma_symtab_define(&ctx->ncalls, $2.buffer, 0);
		}
;

label
	: IDENTIFIER ':' newline.opt
		{ $$ = vma_symtab_define(&ctx->labels, $1.buffer, 1); }
;

expr_list
	: expr
		{ $$ = vma_expr_list_append(vma_expr_list_create(), $1); }
	| expr_list ',' expr
		{ $$ = vma_expr_list_append($1, $3); }
;

expr
	: INTEGER
		{ $$ = vma_expr_build_literal($1); }
	| IDENTIFIER
		{ $$ = vma_expr_build_symref($1.buffer); }
	| expr '|' expr
		{ $$ = vma_expr_build_parent(EXPR_OR, $1, $3); }
	| expr '&' expr
		{ $$ = vma_expr_build_parent(EXPR_AND, $1, $3); }
	| expr '^' expr
		{ $$ = vma_expr_build_parent(EXPR_XOR, $1, $3); }
	| expr '+' expr
		{ $$ = vma_expr_build_parent(EXPR_ADD, $1, $3); }
	| expr '-' expr
		{ $$ = vma_expr_build_parent(EXPR_SUB, $1, $3); }
	| expr '*' expr
		{ $$ = vma_expr_build_parent(EXPR_MUL, $1, $3); }
	| expr '/' expr
		{ $$ = vma_expr_build_parent(EXPR_DIV, $1, $3); }
	| '-' expr %prec NEG
		{ $$ = vma_expr_build_parent(EXPR_NEG, $2, NULL); }
	| '~' expr %prec NOT
		{ $$ = vma_expr_build_parent(EXPR_NOT, $2, NULL); }
	| '(' expr ')'
		{ $$ = $2; }
;

newline.opt
	: /* empty */
	| newline.opt NEWLINE
;

%%

static void yyerror(vma_context_t *ctx, const char *msg)
{
	/* Nothing. */
}

extern void vma_lexer_set_input(FILE *input);

int vma_parse_input(vma_context_t *ctx)
{
	VMA_ASSERT(ctx);
	VMA_ASSERT(ctx->input);

	//yydebug = vma_debug;
	vma_lexer_set_input(ctx->input);
	return yyparse(ctx);
}

static void append_insn(vma_context_t *ctx, vma_insn_t *insn)
{
	if (ctx->insns_tail) {
		ctx->insns_tail->next = insn;
	} else {
		ctx->insns_head = insn;
	}
	ctx->insns_tail = insn;
	vma_debug_print("appending insn %p", ctx->insns_tail);
}
