#include <ngui.h>
#include "ng_properties.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

struct ng_prop_def *ng_recent_prop_def = NULL;
struct ng_prop *ng_recent_prop = NULL;

void ng_register_prop(char *key, int type, int flags)
{
	struct ng_prop_def *def = calloc(1, sizeof(struct ng_prop_def));
	strncpy(def->key, key, 32);
	def->key[31] = 0; // strncpy() might not NULL terminate
	def->flags = flags;

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

int ng_num_retained_props_()
{
	int num = 0;

	for (struct ng_prop_def *def = ng_recent_prop_def; def; def = def->previous)
	{
		if (def->flags & NG_PROP_IMPLICIT_RETAIN)
			num++;
	}

	return num;
}

void ng_populate_retained_(struct ng_retained_prop *props)
{
	int i = 0;
	for (struct ng_prop_def *def = ng_recent_prop_def; def; def = def->previous)
	{
		if (def->flags & NG_PROP_IMPLICIT_RETAIN)
		{
			struct ng_prop *prop = ng_get_prop_(def->key);

			if (prop)
			{
				props[i++] = prop->retained;
			}

			// if undefined, do nothing, an empty key signifies end of list
		}
	}
}

static void ng_set_prop_val_type_(va_list list, struct ng_prop *prop, int type)
{
	if (type == NG_PROP_DOUBLE)
		prop->retained.dval = va_arg(list, double);
	else if (type == NG_PROP_INT)
		prop->retained.ival = va_arg(list, int);
	else if (type == NG_PROP_POINTER)
		prop->retained.pval = va_arg(list, void *);
	else if (type == NG_PROP_STRING)
		prop->retained.pval = va_arg(list, char *);
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
	prop->retained.type = type;
	strncpy(prop->retained.key, key, 32);
	prop->retained.key[31] = 0;
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
		prop->retained.type = type;
		strncpy(prop->retained.key, key, 32);
		prop->retained.key[31] = 0;
		prop->previous = ng_recent_prop;

		ng_set_prop_val_type_(list, prop, type);

		key = va_arg(list, char *);
	}

	va_end(list);
}

struct ng_prop *ng_get_prop_(char *key)
{
	for (struct ng_prop *prop = ng_recent_prop; prop; prop = prop->previous)
	{
		if (strcmp(prop->retained.key, key) == 0)
		{
			return prop;
		}
	}

	return NULL;
}

struct ng_retained_prop *ng_get_retained_prop_(struct ng_retained_prop *ret, char *key)
{
	for (int i = 0; i < ng_num_retained_props_(); i++)
	{
		if (ret[i].key[0] == 0)
		{
			return NULL;
		}
		else if (strcmp(key, ret[i].key) == 0)
		{
			return &ret[i];
		}
	}

	return NULL;
}

int ng_get_propi(char *key)
{
	struct ng_prop *prop = ng_get_prop_(key);
	if (prop)
		return prop->retained.ival;
	else
		return 0;
}

double ng_get_propf(char *key)
{
	struct ng_prop *prop = ng_get_prop_(key);
	if (prop)
		return prop->retained.ival;
	else
		return 0.0;
}

char *ng_get_props(char *key)
{
	struct ng_prop *prop = ng_get_prop_(key);
	if (prop)
		return prop->retained.sval;
	else
		return "";
}

void *ng_get_propp(char *key)
{
	struct ng_prop *prop = ng_get_prop_(key);
	if (prop)
		return prop->retained.pval;
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

void ng_properties_init_()
{
	// Register some default properties

	// Layout (retained)
	ng_register_prop("flex", NG_PROP_INT, NG_PROP_IMPLICIT_RETAIN);
	ng_register_prop("flex-direction", NG_PROP_STRING, 0);

	// Basic
	ng_register_prop("color", NG_PROP_STRING, 0);
}
