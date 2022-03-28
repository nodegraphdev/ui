#include "ng_str.h"

#include <string.h>
#include <stdlib.h>

char *ng_strhcat(char *original, char *append)
{
	char *str = realloc(original, strlen(original) + strlen(append) + 1);
	strcat(str, append);

	return str;
}

char *ng_strhcatc(char *original, char c)
{
	int len = strlen(original);
	char *str = realloc(original, strlen(original) + 2);
	str[len] = c;
	str[len + 1] = 0;

	return str;
}
