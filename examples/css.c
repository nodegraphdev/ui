#include "../include/ngui/ng_css.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		char *res = ng_css_parse_str(argv[1]);
		printf("res: '%s'\n", res);
	}
	else
	{
		fprintf(stderr, "Give me an argument\n");
		return 1;
	}
}
