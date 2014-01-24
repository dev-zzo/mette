/*
 * VM assembly flex scanner.
 * ref: http://dinosaur.compilertools.net/flex/manpage.html
 * ref: http://flex.sourceforge.net/manual/
 * ref: http://flex.sourceforge.net/manual/Bison-Bridge.html
 */

%{

#include "vma.h"
#include "vma_tokens.h"

#define YY_DECL int yylex(vma_context_t *state)

static void dump_yytext();

%}

%option noyywrap
%array

 /* Start conditions */
%x COMMENT
%x STRING

 /* Patterns */
DIGIT	[0-9]
HEXDIGIT [0-9a-fA-F]
OCTDIGIT [0-7]
ALNUM	[0-9a-zA-Z_]
ALPHA	[a-zA-Z_]
WSPACE	[\t\v\r ]
ID		{ALPHA}{ALNUM}*

%%

 /* Numbers: integers */
((0x|0X){HEXDIGIT}+)|(0{OCTDIGIT}+)|({DIGIT}+) {
	yylval.const_int = strtol(yytext, NULL, 0);
	return INTEGER;
}

 /* Keywords */
[.][dD][eE][fF][sS] return KW_DEFS;
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
[cC][mM][pP][.][bB] return OP_CMP_B;
[cC][mM][pP][.][aA] return OP_CMP_A;
[cC][mM][pP][.][eE][qQ] return OP_CMP_EQ;
[lL][dD][cC][.][0] return OP_LDC_0;
[lL][dD][cC][.][1] return OP_LDC_1;
[lL][dD][cC][.][2] return OP_LDC_2;
[lL][dD][cC][.][8][.][uU] return OP_LDC_8_U;
[lL][dD][cC][.][8][.][sS] return OP_LDC_8_S;
[lL][dD][cC][.][3][2] return OP_LDC_32;
[lL][eE][aA] return OP_LEA;
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
[bB][rR] return OP_BR;
[bB][rR][.][tT] return OP_BR_T;
[bB][rR][.][fF] return OP_BR_F;
[cC][aA][lL][lL] return OP_CALL;
[rR][eE][tT] return OP_RET;
[iI][cC][aA][lL][lL] return OP_ICALL;
[iI][jJ][mM][pP] return OP_IJMP;
[nN][cC][aA][lL][lL] return OP_NCALL;

 /* Identifiers */
{ID} {
	yylval.text.length = strlen(yytext);
	yylval.text.buffer = strdup(yytext);
	return IDENTIFIER;
}

 /* Strings */
"\"" {
	yylval.text.length = 0;
	yylval.text.buffer = NULL;
	BEGIN(STRING);
}
<STRING>[\\][x]{HEXDIGIT}{HEXDIGIT} {
	while (yytext[yyleng - 1] != '\\')
		yyleng--;
	yytext[yyleng - 1] = (char)strtol(&yytext[yyleng + 1], NULL, 16);
	yytext[yyleng] = '\0';
	/* Dirty, dirty hack! yymore() uses strlen, which doesn't allow for NULL chars. */
	(yy_more_offset) = yyleng;
}
<STRING>[\\]{OCTDIGIT}{OCTDIGIT}?{OCTDIGIT}? {
	while (yytext[yyleng - 1] != '\\')
		yyleng--;
	yytext[yyleng - 1] = (char)strtol(&yytext[yyleng], NULL, 8);
	yytext[yyleng] = '\0';
	/* Dirty, dirty hack! yymore() uses strlen, which doesn't allow for NULL chars. */
	(yy_more_offset) = yyleng;
}
<STRING>[\\]["rnt\\] {
	char ch;
	yyleng -= 1;
	switch (yytext[yyleng]) {
		case '"':
			ch = '"';
			break;
		case '\\':
			ch = '\\';
			break;
		case 'r':
			ch = '\r';
			break;
		case 'n':
			ch = '\n';
			break;
		case 't':
			ch = '\t';
			break;
	}
	yytext[yyleng - 1] = ch;
	yytext[yyleng] = '\0';
	yymore();
}
<STRING>"\n" {
	/* TODO: line number. */
	fprintf(stderr, "Warning: string literal spans multiple lines.\n");
	BEGIN(INITIAL);
	return STRINGT;
}
<STRING>[^"] {
	yymore();
}
<STRING>["] {
	yylval.text.length = yyleng - 1;
	yylval.text.buffer = malloc(yyleng);
	/* Skip the quote */
	memcpy(yylval.text.buffer, yytext, yyleng - 1);
	BEGIN(INITIAL);
	return STRINGT;
}

 /* Comments */
"#" {
	BEGIN(COMMENT);
}
<COMMENT>[^\n]+ /* Ignored */ {
	BEGIN(INITIAL);
}

{WSPACE}+ /* Ignored */

 /* Punctuation */
[:,()+*-/|&^.@~] return yytext[0];

"\n" { }

<<EOF>> {
	return END;
}

. {
	printf("Unrecognized character: %s\n", yytext);
	return yytext[0];
}

%%

static void dump_yytext()
{
	int i = 0;

	while (i < yyleng) {
		printf("%02X ", (unsigned char)yytext[i]);
		++i;
	}
	printf("\n");
}

void vma_lexer_set_input(FILE *input)
{
	yyrestart(input);
}

