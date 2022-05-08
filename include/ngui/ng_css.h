#pragma once

struct ng_css_prop
{
	char *key, *value;
	struct ng_css_prop *prev;
};

struct ng_css_pattern
{
	struct ng_css_prop *yields;
	char *pattern;
};

struct ng_css_pattern *ng_css_parse_str(char *str);
void ng_css_free_prop(struct ng_css_prop *prop);
void ng_css_free_pattern(struct ng_css_pattern *pattern);
