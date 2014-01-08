/*
 * VM assembly flex scanner.
 * ref: http://dinosaur.compilertools.net/flex/manpage.html
 * ref: http://flex.sourceforge.net/manual/Bison-Bridge.html
 */

%{

#include "vma.h"
#include "vma_tokens.h"

#define YY_DECL int yylex(struct vma_parser_state *state)

%}

%option noyywrap

 /* Start conditions */
%x COMMENT

 /* Patterns */
DIGIT	[0-9]
ALNUM	[0-9a-zA-Z_]
ALPHA	[a-zA-Z_]
WSPACE	[\t\v\r ]
ID		{ALPHA}{ALNUM}*

%%

 /* Numbers: integers */
((0x|0X){DIGIT}+)|(0{DIGIT}+)|({DIGIT}+) {
	yylval.const_int = strtol(yytext, NULL, 0);
	return INTEGER;
}

 /* Keywords */
[.][dD][eE][fF][bB] return KW_DEFB;
[.][dD][eE][fF][hH] return KW_DEFH;
[.][dD][eE][fF][wW] return KW_DEFW;
[.][rR][eE][sS][bB] return KW_RESB;
[.][rR][eE][sS][hH] return KW_RESH;
[.][rR][eE][sS][wW] return KW_RESW;

 /* Opcodes */
[aA][dD][dD] return OP_ADD;
[sS][uU][bB] return OP_SUB;
[mM][uU][lL][uU] return OP_MULU;
[mM][uU][lL][sS] return OP_MULS;
[dD][iI][vV][uU] return OP_DIVU;
[dD][iI][vV][sS] return OP_DIVS;
[aA][nN][dD] return OP_AND;
[oO][rR] return OP_OR;
[xX][oO][rR] return OP_XOR;
[nN][oO][tT] return OP_NOT;
[lL][sS][lL] return OP_LSL;
[lL][sS][rR] return OP_LSR;
[aA][sS][rR] return OP_ASR;
[cC][mM][pP][.][lL][tT] return OP_CMP_LT;
[cC][mM][pP][.][gG][tT] return OP_CMP_GT;
[cC][mM][pP][.][lL][eE] return OP_CMP_LE;
[cC][mM][pP][.][gG][eE] return OP_CMP_GE;
[cC][mM][pP][.][bB] return OP_CMP_B;
[cC][mM][pP][.][aA] return OP_CMP_A;
[cC][mM][pP][.][bB][eE] return OP_CMP_BE;
[cC][mM][pP][.][aA][eE] return OP_CMP_AE;
[cC][mM][pP][.][eE][qQ] return OP_CMP_EQ;
[cC][mM][pP][.][nN][eE] return OP_CMP_NE;
[lL][dD][cC][.][0] return OP_LDC_0;
[lL][dD][cC][.][1] return OP_LDC_1;
[lL][dD][cC][.][2] return OP_LDC_2;
[lL][dD][cC][.][8][.][uU] return OP_LDC_8_U;
[lL][dD][cC][.][8][.][sS] return OP_LDC_8_S;
[lL][dD][cC][.][3][2] return OP_LDC_32;
[lL][dD][mM][.][8][.][uU] return OP_LDM_8_U;
[lL][dD][mM][.][8][.][sS] return OP_LDM_8_S;
[lL][dD][mM][.][1][6][.][uU] return OP_LDM_16_U;
[lL][dD][mM][.][1][6][.][sS] return OP_LDM_16_S;
[lL][dD][mM][.][3][2] return OP_LDM_32;
[sS][tT][mM][.][8] return OP_STM_8;
[sS][tT][mM][.][1][6] return OP_STM_16;
[sS][tT][mM][.][3][2] return OP_STM_32;
[lL][oO][cC][aA][lL][sS] return OP_LOCALS;
[lL][dD][lL][oO][cC] return OP_LDLOC;
[sS][tT][lL][oO][cC] return OP_STLOC;
[dD][uU][pP] return OP_DUP;
[sS][wW][aA][pP] return OP_SWAP;
[pP][oO][pP] return OP_POP;
[bB][rR][.][sS] return OP_BR_S;
[bB][rR][.][lL] return OP_BR_L;
[bB][rR][.][tT] return OP_BR_T;
[bB][rR][.][fF] return OP_BR_F;
[cC][aA][lL][lL] return OP_CALL;
[rR][eE][tT] return OP_RET;
[iI][cC][aA][lL][lL] return OP_ICALL;
[iI][jJ][mM][pP] return OP_IJMP;
[nN][cC][aA][lL][lL] return OP_NCALL;

 /* Identifiers */
{ID} {
	yylval.text = strdup(yytext);
	return IDENTIFIER;
}

 /* Comments */
"#" {
	BEGIN(COMMENT);
}
<COMMENT>[^\n]+ /* Ignored */ {
	//printf("Comment: %s\n", yytext);
	BEGIN(INITIAL);
}

{WSPACE}+ /* Ignored */

 /* Punctuation */
[:,()+*-/|&^.@~] return yytext[0];

"\n" return NEWLINE;

<<EOF>> {
	return END;
}

. {
	printf("Unrecognized character: %s\n", yytext);
	return yytext[0];
}

%%

void vma_lexer_set_input(FILE *input)
{
	yyrestart(input);
}
