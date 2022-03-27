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

char *ng_css_parse_str(char *str);
