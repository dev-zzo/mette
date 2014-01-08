/*
 * VM assembly flex scanner.
 * ref: http://dinosaur.compilertools.net/flex/manpage.html
 */

%option noyywrap

%{

#include "vm_asm.tokens.h"

int yyline = 1;

%}

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
		yylval.integer = strtol(yytext, NULL, 0);
		return INTEGER;
	}

/* Identifiers */
{ID} {
		yylval.text = strdup(yytext);
		return IDENTIFIER;
	}

/* Keywords */
".db" return KW_DB;
".dh" return KW_DH;
".dw" return KW_DW;

/* Opcodes */
[aA][dD][dD] return OP_ADD;
[sS][uU][bB] return OP_SUB;
[lL][dD][cC][.][8][.][uU] return OP_LDC_8_U;

/* Comments */
"#" {
		BEGIN(COMMENT);
	}
<COMMENT>[^\n]+ /* Ignored */ {
		printf("Comment: %s\n", yytext);
		BEGIN(INITIAL);
	}

{WSPACE}+ /* Ignored */

/* Punctuation */
[:,()+*-/|&^.@~] return yytext[0];

"\n" return NEWLINE;

. {
		printf("Unrecognized character: %s\n", yytext);
	}
%%

int main(int argc, char *argv[])
{
	++argv, --argc;	/* skip over program name */
	if ( argc > 0 )
	   yyin = fopen( argv[0], "r" );
	else
	   yyin = stdin;

	yylex();
}

