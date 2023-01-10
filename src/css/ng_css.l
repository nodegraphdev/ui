%option noyywrap yylineno nodefault nounput noinput
%option noyyalloc noyyrealloc noyyfree
%option reentrant bison-bridge
%option prefix="ng_css"

%{
#define YY_EXIT_FAILURE ((void)yyscanner, EXIT_FAILURE)
#define YYSTYPE NG_CSSSTYPE

#include <string.h>	 
#include "ng_css_y.h"

int yyerror(const char *msg);
%}

%%

[a-zA-Z\-0-9_]* { yylval->str = strdup(yytext); return IDENT; }

[ \t\r\n\v]+ { return WS; }

. { return *yytext; }

%%

/* [-+]?[[:digit:]]+ { yylval->inum = strtol(yytext, NULL, 10); return INT; } */
/* [-+]?[[:digit:]]+\.[[:digit:]]* { yylval->dnum = strtod(yytext, NULL); return DOUBLE; } */
