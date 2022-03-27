%define api.pure true
%define api.prefix {ng_css}
%define parse.error verbose

%code top {
	#include <stdio.h>
	#include <string.h>

	#ifndef YYNOMEM
	#define YYNOMEM goto yyexhaustedlab
	#endif

	#include "ng_str.h"
}

%parse-param {char **result}
%param {void *scanner}

%union {
	char *str;
	int inum;
	double dnum;
}

%code {
	#include "ng_css_l.h"
	int yyerror(void *yylval, const char *msg, const void *s);
}

%token	<str>			IDENT
%token	<inum>			INT
%token	<dnum>			DOUBLE
%token					WS

%type	<str>			selector class_or_id class id top

%destructor { free($$); } <str>

%%

top:			selector
				{
					*result = $$ = $1;
					return 0;
				}
		;

selector:		IDENT
		|		IDENT class_or_id
				{
					$$ = ng_strhcat($1, $2);
					free($2);
				}
		|		class_or_id
		;

class_or_id:	class
		|		id
		|		class id
				{
					$$ = ng_strhcat($1, $2);
					free($2);
				}
		|		id class
				{
					// class comes first in both cases
					$$ = ng_strhcat($2, $1);
					free($1);
				}
		;

class:			'.' IDENT
				{
					char *str = strndup(".", 1);
					$$ = ng_strhcat(str, $2);
				}
		;

id		:		'#' IDENT
				{
					char *str = strndup("#", 1);
					$$ = ng_strhcat(str, $2);
				}
		;

%%

int yyerror(void *yylval, const char *msg, const void *s)
{
	return fprintf(stderr, "ng_css:error:%s\n", msg);
}

void *ng_cssalloc(size_t size, void *yyscanner)
{
	return malloc(size);
}

void *ng_cssrealloc(void *ptr, size_t size, void *yyscanner)
{
	return realloc(ptr, size);
}

void ng_cssfree(void *ptr, void *yyscanner)
{
	free(ptr);
}

char *ng_css_parse_str(char *str)
{
	yyscan_t scanner;
	char *out;

	ng_csslex_init(&scanner);
	ng_css_scan_string(str, scanner);
	int e = yyparse(&out, scanner);

	if (e)
		printf("ng_css_parse_str:info: returned = %d\n", e);

	ng_csslex_destroy(scanner);

	return out;
}
