#include <ngui/ng_css.h>
#include <stdlib.h>

static char *ng_next_name_ = "",
	*ng_next_id_ = "",
	*ng_next_class_ = "";


void ng_css_free_prop(struct ng_css_prop *prop)
{
	if (!prop)
		return;

	free(prop->key);
	free(prop->value);

	if (prop->prev)
		ng_css_free_prop(prop->prev);

	free(prop);
}

void ng_css_free_pattern(struct ng_css_pattern *pattern)
{
	if (!pattern)
		return;

	ng_css_free_prop(pattern->yields);
	free(pattern->pattern);
	free(pattern);
}


void ng_with_name(char *name)
{
	ng_next_name_ = name;
}

void ng_with_id(char *id)
{
	ng_next_id_ = id;
}

void ng_with_class(char *class)
{
	ng_next_class_ = class;
}

void ng_css_begin_element()
{
	
}

void ng_css_end_element()
{
	
}

void ng_css_print_prop(FILE *to, struct ng_css_prop *prop)
{
	for (; prop; prop = prop->prev)
	{
		fprintf(to, "\t%s: %s;\n", prop->key, prop->value);
	}
}

void ng_css_print_pattern(FILE *to, struct ng_css_pattern *pattern)
{
	if (!pattern)
		return;

	fprintf(to, "%s {\n", pattern->pattern);
	ng_css_print_prop(to, pattern->yields);
	fprintf(to, "}\n");

	if (pattern->prev)
	{
		fprintf(to, "\n");
		ng_css_print_pattern(to, pattern->prev);
	}
}
