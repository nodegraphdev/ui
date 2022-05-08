#include <ngui/ng_css.h>
#include <stdlib.h>

void ng_css_free_prop(struct ng_css_prop *prop)
{
	free(prop->key);
	free(prop->value);

	if (prop->prev)
		ng_css_free_prop(prop->prev);

	free(prop);
}

void ng_css_free_pattern(struct ng_css_pattern *pattern)
{
	ng_css_free_prop(pattern->yields);
	free(pattern->pattern);
	free(pattern);
}
