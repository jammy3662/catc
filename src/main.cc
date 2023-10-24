#include <stdio.h>

#include "token.h"

int main (int argc, char** argv)
{
	FILE* file = fopen (argv [1], "r");
	
	Token t;
	while ((t = getTok (file)).id != NULLTOK)
	{
		printf ("%i\t\t%s\n", t.id, t.text);
	}
	
	return 0;
}
