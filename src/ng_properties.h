#pragma once

struct ng_retained_prop
{
	char key[32];
	int type;

	union
	{
		int ival;
		double dval;
		char *sval;
		void *pval;
	};
};

struct ng_prop
{
	struct ng_retained_prop retained;
	struct ng_prop *previous;
};

struct ng_prop_def
{
	char key[32];
	enum ng_prop_type type;
	int flags;

	struct ng_prop_def *previous;
};

extern struct ng_prop_def *ng_recent_prop_def;
extern struct ng_prop *ng_recent_prop;

int ng_num_retained_props_();
void ng_populate_retained_(struct ng_retained_prop *props);
struct ng_prop *ng_get_prop_(char *key);
void ng_properties_init_();
