%define api.pure full
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

%code requires {
	#include <ngui/ng_css.h>
}

%parse-param {struct  ng_css_pattern **result}
%param {void *scanner}

%union {
	char *str;
	char letter;
	int inum;
	double dnum;
	struct ng_css_prop *prop;
	struct ng_css_pattern *pattern
}

%code {
	#include "ng_css_l.h"
	int yyerror(void *yylval, const char *msg, const void *s);
}

%token	<str>			IDENT "identifier"
%token	<inum>			INT "integer"
%token	<dnum>			DOUBLE "double"
%token					WS "whitespace"

%type	<str>			selector class_or_id "class or id" class id pattern value nesting
%type	<prop>			props "properties" prop "property" prop_list "braced property list"
%type	<pattern>		declarations declaration top

%destructor { free($$); } <str>
%destructor { ng_css_free_prop($$); } <prop>
%destructor { ng_css_free_pattern($$); } <pattern>

%%

top:			maybe_space declarations maybe_space
				{
					*result = $$ = $2;
					YYACCEPT;
				}
		;

declarations:	{ $$ = NULL; }
		|		declarations maybe_space declaration
				{
					$3->prev = $1;
					$$ = $3;
				}
		;

declaration:	pattern maybe_space prop_list
				{
					$$ = malloc(sizeof(struct ng_css_pattern));
					$$->yields = $3;
					$$->pattern = $1;
					$$->prev = NULL;
				}
		;

prop_list:		'{' maybe_space props maybe_space '}'
				{
					$$ = $3;
				}
		;

props:			{ $$ = NULL; }
		|		prop
		|		props semi prop maybe_semi
				{
					$3->prev = $1;
					$$ = $3;
				}
		;

maybe_semi:		semi
		| 		maybe_space
		;

semi:			maybe_space ';' maybe_space
		;

prop:			IDENT maybe_space ':' maybe_space value
				{
					$$ = malloc(sizeof(struct ng_css_prop));
					$$->key = $1;
					$$->value = $5;
					$$->prev = NULL;
				}
		;

// TODO
value:			IDENT
		;

pattern:		selector
				{
					$$ = $1;
				}
		|		pattern nesting selector
				{
					char *str = ng_strhcat($1, $2);
					$$ = ng_strhcat(str, $3);
					free($3);
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

nesting:		maybe_space '>' maybe_space
				{
					$$ = " >";
				}
		|		WS
				{
					$$ = " ";
				}
		;

maybe_space:	WS
		|
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
					char *str = strdup(".");
					$$ = ng_strhcat(str, $2);
				}
		;

id		:		'#' IDENT
				{
					char *str = strdup("#");
					$$ = ng_strhcat(str, $2);
				}
		;

%%

int yyerror(void *yylval, const char *msg, const void *s)
{
	return fprintf(stderr, "\033[31mng_css:error:\033[0m %s\n", msg);
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

struct ng_css_pattern *ng_css_parse_str(char *str)
{
	yyscan_t scanner;
	struct ng_css_pattern *out;

	ng_csslex_init(&scanner);
	ng_css_scan_string(str, scanner);
	int e = yyparse(&out, scanner);

	if (e)
		printf("ng_css_parse_str:info: returned = %d\n", e);

	ng_csslex_destroy(scanner);

	return out;
}
