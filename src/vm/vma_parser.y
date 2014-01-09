/*
 * VM assembly bison grammar.
 * ref: http://www.gnu.org/software/bison/manual/bison.html
 */

%debug

%{

#include "vma.h"
#include <stdio.h>

%}

%lex-param {struct vma_context *ctx}
%parse-param {struct vma_context *ctx}

%{

/* Globals... */
extern int yylex(struct vma_context *ctx);

static void yyerror(struct vma_context *ctx, const char *msg);

%}

%union {
	int const_int;
	const char *text;
	struct vma_symbol *sym;
	struct vma_expr_node *expr;
	struct vma_expr_list *expr_list;
	struct vma_insn_node *insn;
	struct vma_unit *unit;
}

%token END 0 "end of file"
%token NEWLINE
%type <sym> label
%type <expr> expr
%type <expr_list> expr_list
%type <insn> insn
%type <insn> stmt
%type <unit> unit
%left '|'
%left '&'
%left '^'
%left '+' '-'
%left '*' '/'
%left NEG
%left NOT

%token <const_int> INTEGER
%token <text> IDENTIFIER

%token KW_DEFB KW_DEFH KW_DEFW
%token KW_RESB KW_RESH KW_RESW

%token OP_ADD OP_SUB OP_MULU OP_MULS OP_DIVU OP_DIVS 
%token OP_AND OP_OR OP_XOR OP_NOT OP_LSL OP_LSR OP_ASR 
%token OP_CMP_LT OP_CMP_GT OP_CMP_LE OP_CMP_GE OP_CMP_B OP_CMP_A OP_CMP_BE OP_CMP_AE OP_CMP_EQ OP_CMP_NE 
%token OP_LDC_0 OP_LDC_1 OP_LDC_2 OP_LDC_8_U OP_LDC_8_S OP_LDC_32 
%token OP_LDM_8_U OP_LDM_8_S OP_LDM_16_U OP_LDM_16_S OP_LDM_32 
%token OP_STM_8 OP_STM_16 OP_STM_32 
%token OP_LOCALS OP_LDLOC OP_STLOC 
%token OP_DUP OP_SWAP OP_POP 
%token OP_BR_S OP_BR_L OP_BR_T OP_BR_F 
%token OP_CALL OP_RET 
%token OP_ICALL OP_IJMP 
%token OP_NCALL

%printer { fprintf(yyoutput, "%08X", $$); } INTEGER;
%printer { fprintf(yyoutput, "%s", $$); } IDENTIFIER;

%%

unit
	: /* empty */
		{ $$ = ctx->unit = vma_build_unit(); }
	| unit stmt
		{ $$ = vma_append_unit($1, $2); }
;

stmt
	: insn
		{ $$ = $1; }
	| label insn
		{ $$ = $2; $1->location = $$; }
;

insn
	: KW_DEFB expr_list
		{ $$ = vma_build_insn(INSN_DEFB); $$->u.expr_list = $2; }
	| KW_DEFH expr_list
		{ $$ = vma_build_insn(INSN_DEFH); $$->u.expr_list = $2; }
	| KW_DEFW expr_list
		{ $$ = vma_build_insn(INSN_DEFW); $$->u.expr_list = $2; }
	| KW_RESB expr
		{ $$ = vma_build_insn(INSN_RESB); $$->u.expr = $2; }
	| KW_RESH expr
		{ $$ = vma_build_insn(INSN_RESH); $$->u.expr = $2; }
	| KW_RESW expr
		{ $$ = vma_build_insn(INSN_RESW); $$->u.expr = $2; }
	| OP_ADD
		{ $$ = vma_build_insn(INSN_ADD); }
	| OP_SUB
		{ $$ = vma_build_insn(INSN_SUB); }
	| OP_MULU
		{ $$ = vma_build_insn(INSN_MULU); }
	| OP_MULS
		{ $$ = vma_build_insn(INSN_MULS); }
	| OP_DIVU
		{ $$ = vma_build_insn(INSN_DIVU); }
	| OP_DIVS
		{ $$ = vma_build_insn(INSN_DIVS); }
	| OP_AND
		{ $$ = vma_build_insn(INSN_AND); }
	| OP_OR
		{ $$ = vma_build_insn(INSN_OR); }
	| OP_XOR
		{ $$ = vma_build_insn(INSN_XOR); }
	| OP_NOT
		{ $$ = vma_build_insn(INSN_NOT); }
	| OP_LSL
		{ $$ = vma_build_insn(INSN_LSL); }
	| OP_LSR
		{ $$ = vma_build_insn(INSN_LSR); }
	| OP_ASR
		{ $$ = vma_build_insn(INSN_ASR); }
	| OP_CMP_LT
		{ $$ = vma_build_insn(INSN_CMP_LT); }
	| OP_CMP_GT
		{ $$ = vma_build_insn(INSN_CMP_GT); }
	| OP_CMP_LE
		{ $$ = vma_build_insn(INSN_CMP_LE); }
	| OP_CMP_GE
		{ $$ = vma_build_insn(INSN_CMP_GE); }
	| OP_CMP_B
		{ $$ = vma_build_insn(INSN_CMP_B); }
	| OP_CMP_A
		{ $$ = vma_build_insn(INSN_CMP_A); }
	| OP_CMP_BE
		{ $$ = vma_build_insn(INSN_CMP_BE); }
	| OP_CMP_AE
		{ $$ = vma_build_insn(INSN_CMP_AE); }
	| OP_CMP_EQ
		{ $$ = vma_build_insn(INSN_CMP_EQ); }
	| OP_CMP_NE
		{ $$ = vma_build_insn(INSN_CMP_NE); }
	| OP_LDC_0
		{ $$ = vma_build_insn(INSN_LDC_0); }
	| OP_LDC_1
		{ $$ = vma_build_insn(INSN_LDC_1); }
	| OP_LDC_2
		{ $$ = vma_build_insn(INSN_LDC_2); }
	| OP_LDC_8_U expr
		{ $$ = vma_build_insn(INSN_LDC_8_U); $$->u.expr = $2; }
	| OP_LDC_8_S expr
		{ $$ = vma_build_insn(INSN_LDC_8_S); $$->u.expr = $2; }
	| OP_LDC_32 expr
		{ $$ = vma_build_insn(INSN_LDC_32); $$->u.expr = $2; }
	| OP_LDM_8_U
		{ $$ = vma_build_insn(INSN_LDM_8_U); }
	| OP_LDM_8_S
		{ $$ = vma_build_insn(INSN_LDM_8_S); }
	| OP_LDM_16_U
		{ $$ = vma_build_insn(INSN_LDM_16_U); }
	| OP_LDM_16_S
		{ $$ = vma_build_insn(INSN_LDM_16_S); }
	| OP_LDM_32
		{ $$ = vma_build_insn(INSN_LDM_32); }
	| OP_STM_8
		{ $$ = vma_build_insn(INSN_STM_8); }
	| OP_STM_16
		{ $$ = vma_build_insn(INSN_STM_16); }
	| OP_STM_32
		{ $$ = vma_build_insn(INSN_STM_32); }
	| OP_LOCALS expr
		{ $$ = vma_build_insn(INSN_LOCALS); $$->u.expr = $2; }
	| OP_LDLOC expr
		{ $$ = vma_build_insn(INSN_LDLOC); $$->u.expr = $2; }
	| OP_STLOC expr
		{ $$ = vma_build_insn(INSN_STLOC); $$->u.expr = $2; }
	| OP_DUP
		{ $$ = vma_build_insn(INSN_DUP); }
	| OP_SWAP
		{ $$ = vma_build_insn(INSN_SWAP); }
	| OP_POP
		{ $$ = vma_build_insn(INSN_POP); }
	| OP_BR_S IDENTIFIER
		{ $$ = vma_build_insn(INSN_BR_S); vma_init_symref(&$$->u.symref, $2); }
	| OP_BR_L IDENTIFIER
		{ $$ = vma_build_insn(INSN_BR_L); vma_init_symref(&$$->u.symref, $2); }
	| OP_BR_T IDENTIFIER
		{ $$ = vma_build_insn(INSN_BR_T); vma_init_symref(&$$->u.symref, $2); }
	| OP_BR_F IDENTIFIER
		{ $$ = vma_build_insn(INSN_BR_F); vma_init_symref(&$$->u.symref, $2); }
	| OP_CALL IDENTIFIER
		{ $$ = vma_build_insn(INSN_CALL); vma_init_symref(&$$->u.symref, $2); }
	| OP_RET
		{ $$ = vma_build_insn(INSN_RET); }
	| OP_ICALL
		{ $$ = vma_build_insn(INSN_ICALL); }
	| OP_IJMP
		{ $$ = vma_build_insn(INSN_IJMP); }
	| OP_NCALL IDENTIFIER
		{ $$ = vma_build_insn(INSN_NCALL); vma_init_symref(&$$->u.symref, $2); $$->u.symref.ncall = 1; } /* TBD */
;

label
	: IDENTIFIER ':'
		{ $$ = vma_define_symbol($1); }
;

expr_list
	: expr
		{ $$ = vma_append_expr_list(vma_create_expr_list(), $1); }
	| expr_list ',' expr
		{ $$ = vma_append_expr_list($1, $3); }
;

expr
	: INTEGER
		{ $$ = vma_build_constant_expr($1); }
	| '@' IDENTIFIER
		{ $$ = vma_build_symref_expr($2); }
	| expr '|' expr
		{ $$ = vma_build_parent_expr(EXPR_OR, $1, $3); }
	| expr '&' expr
		{ $$ = vma_build_parent_expr(EXPR_AND, $1, $3); }
	| expr '^' expr
		{ $$ = vma_build_parent_expr(EXPR_XOR, $1, $3); }
	| expr '+' expr
		{ $$ = vma_build_parent_expr(EXPR_ADD, $1, $3); }
	| expr '-' expr
		{ $$ = vma_build_parent_expr(EXPR_SUB, $1, $3); }
	| expr '*' expr
		{ $$ = vma_build_parent_expr(EXPR_MUL, $1, $3); }
	| expr '/' expr
		{ $$ = vma_build_parent_expr(EXPR_DIV, $1, $3); }
	| '-' expr %prec NEG
		{ $$ = vma_build_parent_expr(EXPR_NEG, $2, NULL); }
	| '~' expr %prec NOT
		{ $$ = vma_build_parent_expr(EXPR_NOT, $2, NULL); }
	| '(' expr ')'
		{ $$ = $2; }
;

%%

void yyerror(struct vma_context *ctx, const char *msg)
{
	vma_error(msg);
}

extern void vma_lexer_set_input(FILE *input);

int vma_parse_input(struct vma_context *ctx)
{
	VMA_ASSERT(ctx);
	VMA_ASSERT(ctx->input);

	yydebug = vma_debug;
	vma_lexer_set_input(ctx->input);
	return yyparse(ctx);
}
