#include <stdio.h>

#include "token.h"

int main (int argc, char** argv)
{
	FILE* file = fopen (argv [1], "r");
	
	Token t;
	t.id = NONE;
	
	while (t.id != NULLTOK)
	{
		t = getTok (file);
		printf ("%i\t\t%s\n", (unsigned char) t.id, t.text);
	}
	
	return 0;
}
