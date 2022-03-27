#include "ng_str.h"

#include <string.h>
#include <stdlib.h>

char *ng_strhcat(char *original, char *append)
{
	char *str = realloc(original, strlen(original) + strlen(append) + 1);
	strcat(str, append);

	return str;
}
