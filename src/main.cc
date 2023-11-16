#include <stdio.h>

#include "token.h"

int main (int argc, char** argv)
{
	Trie keywords = prefixTree ({"this", "that", "what", "tree", "trie", "trees"});
	
	FILE* file = fopen (argv [1], "r");
	
	Token t;
	t.id = NONE;
	
	while (t.id != NULLTOK)
	{
		t = getTok (file);
		printf ("%i %i\t\t%s\n",
			(unsigned char) t.id,
			find (t.text, keywords),
			t.text);
	}
	
	return 0;
}
