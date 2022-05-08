#pragma once

#include <stdio.h>

struct ng_css_prop
{
	char *key, *value;
	struct ng_css_prop *prev;
};

struct ng_css_pattern
{
	struct ng_css_prop *yields;
	char *pattern;
	struct ng_css_pattern *prev;
};

struct ng_css_pattern *ng_css_parse_str(char *str);
void ng_css_free_prop(struct ng_css_prop *prop);
void ng_css_free_pattern(struct ng_css_pattern *pattern);

/* These functions narrow the set of applicable patterns and apply the
 * properties if a pattern is matched. These should rarely need to be
 * called by the user. They rely on the implicit state set by
 * ng_with_{name,id,class}() */
void ng_css_begin_element();
void ng_css_end_element();

/* Debugging functions */
void ng_css_print_prop(FILE *to, struct ng_css_prop *prop);
void ng_css_print_pattern(FILE *to, struct ng_css_pattern *pattern);
