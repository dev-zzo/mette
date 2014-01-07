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
WSPACE	[\t\v ]
ID		{ALPHA}{ALNUM}*

%%

((0x|0X){DIGIT}+)|(0{DIGIT}+)|({DIGIT}+) {
		yylval.integer = strtol(yytext, NULL, 0);
		return INTEGER;
	}

{ID} {
		yylval.text = yytext;
		return IDENTIFIER;
	}

".db" return KW_DB;
".dh" return KW_DH;
".dw" return KW_DW;

[aA][dD][dD] return OP_ADD;
[sS][uU][bB] return OP_SUB;

"#" { /* Comment */
		BEGIN(COMMENT);
	}
<COMMENT>[^\n]+ /* Ignored */ {
		printf("Comment: %s\n", yytext);
		BEGIN(INITIAL);
	}

{WSPACE}+ /* Ignored */

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

