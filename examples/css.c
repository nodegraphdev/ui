#include "../include/ngui/ng_css.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	char *res = ng_css_parse_str("div#hi.hello");
	printf("res: '%s'\n", res);
}
