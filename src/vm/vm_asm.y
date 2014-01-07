/*
 * VM assembly bison grammar.
 */

%union {
	uint32_t integer;
	const char *text;
}

%token <integer> INTEGER
%token <text> IDENTIFIER
%token NEWLINE
%token EOF
%token DOT
%token KW_DB
%token KW_DH
%token KW_DW
%token OP_ADD
%token OP_SUB

%%

input:
	  /* empty */
	| input line
;

line:
	  NEWLINE
	| stmt NEWLINE
	| stmt EOF
;

stmt:
	  insn
	| label insn
;

insn:
	  kw_insn
	| op_insn
;

kw_insn:
	  KW_DB expr_list
	| KW_DH expr_list
	| KW_DW expr_list
;

op_insn:
	  OP_ADD
	| OP_SUB
;

label: IDENTIFIER ':'
;

expr_list: expr
	| expr_list ',' expr
;

expr:
	  INTEGER
	| '(' expr ')'
;

%%


