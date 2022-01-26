#include <ngui.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

struct ng_prop
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

	struct ng_prop *previous;
};

struct ng_prop_def
{
	char key[32];
	enum ng_prop_type type;

	struct ng_prop_def *previous;
};

static struct ng_prop_def *ng_recent_prop_def = NULL;
static struct ng_prop *ng_recent_prop = NULL;

void ng_register_prop(char *key, int type)
{
	struct ng_prop_def *def = calloc(1, sizeof(struct ng_prop_def));
	strncpy(def->key, key, 32);
	def->key[31] = 0; // strncpy() might not NULL terminate

	def->type = type;
	def->previous = ng_recent_prop_def;
	ng_recent_prop_def = def;
}

static int ng_property_type_(char *key)
{
	for (struct ng_prop_def *def = ng_recent_prop_def; def; def = def->previous)
	{
		if (strcmp(def->key, key) == 0)
		{
			return def->type;
		}
	}

	return NG_PROP_UNDEF;
}

static void ng_set_prop_val_type_(va_list list, struct ng_prop *prop, int type)
{
	if (type == NG_PROP_DOUBLE)
		prop->dval = va_arg(list, double);
	else if (type == NG_PROP_INT)
		prop->ival = va_arg(list, int);
	else if (type == NG_PROP_POINTER)
		prop->pval = va_arg(list, void *);
	else if (type == NG_PROP_STRING)
		prop->pval = va_arg(list, char *);
}

void ng_prop(char *key, ...)
{
	int type = ng_property_type_(key);

	if (type == NG_PROP_UNDEF)
	{
		fprintf(stderr, "ng_prop:warning: Property '%s' is not registered.", key);
		return;
	}

	va_list list;
	va_start(list, key);

	struct ng_prop *prop = calloc(1, sizeof(struct ng_prop));
	prop->type = type;
	strncpy(prop->key, key, 32);
	prop->key[31] = 0;
	prop->previous = ng_recent_prop;

	ng_set_prop_val_type_(list, prop, type);

	ng_recent_prop = prop;

	va_end(list);
}

void ng_props(char *key, ...)
{
	va_list list;
	va_start(list, key);

	while (key)
	{
		int type = ng_property_type_(key);

		if (type == NG_PROP_UNDEF)
		{
			fprintf(stderr, "ng_props:warning: Property '%s' is not registered.", key);
			va_end(list);
			return;
		}

		struct ng_prop *prop = calloc(1, sizeof(struct ng_prop));
		prop->type = type;
		strncpy(prop->key, key, 32);
		prop->key[31] = 0;
		prop->previous = ng_recent_prop;

		ng_set_prop_val_type_(list, prop, type);

		key = va_arg(list, char *);
	}

	va_end(list);
}

static struct ng_prop *ng_get_prop_(char *key)
{
	for (struct ng_prop *prop = ng_recent_prop; prop; prop = prop->previous)
	{
		if (strcmp(prop->key, key) == 0)
		{
			return prop;
		}
	}

	return NULL;
}

int ng_get_propi(char *key)
{
	struct ng_prop *prop = ng_get_prop_(key);
	if (prop)
		return prop->ival;
	else
		return 0;
}

double ng_get_propf(char *key)
{
	struct ng_prop *prop = ng_get_prop_(key);
	if (prop)
		return prop->ival;
	else
		return 0.0;
}

char *ng_get_props(char *key)
{
	struct ng_prop *prop = ng_get_prop_(key);
	if (prop)
		return prop->sval;
	else
		return "";
}

void *ng_get_propp(char *key)
{
	struct ng_prop *prop = ng_get_prop_(key);
	if (prop)
		return prop->pval;
	else
		return NULL;
}

void ng_reset_props()
{
	for (struct ng_prop *prop = ng_recent_prop; prop;)
	{
		struct ng_prop *previous = prop->previous;
		free(prop);
		prop = previous;
	}
	ng_recent_prop = NULL;
}
