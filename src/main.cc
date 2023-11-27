#include <stdio.h>

#include "token.h"

list <Key> KEYWORDS = 
{
	{"void", VOID_T}, {"var", VAR_T}, {"auto", VAR_T},
	{"struct", STRUCT_T}, {"union", UNION_T},
	{"enum", ENUM_T}, {"char", CHAR_T},
	{"byte", CHAR_T}, {"short", SHORT_T},
	{"int", INT_T}, {"long", LONG_T},
	{"float", DOUBLE_T}, {"double", DOUBLE_T},
	{"<=", LESSEQUAL}, {">=", MOREEQUAL},
	{"+=", PLUSEQUAL}, {"-=", MINUSEQUAL},
	{"*=", TIMESEQUAL}, {"/=", DIVEQUAL},
	{"<<", SHIFT_L}, {">>", SHIFT_R},
	{"==", COMPARE}, {"++", INCREMENT},
	{"--", DECREMENT}, {"&&", AND},
	{"and", AND}, {"||", OR},
	{"or", OR},  {"...", ELLIPSIS},
	{"if", IF}, {"else", ELSE},
	{"while", WHILE}, {"switch", SWITCH},
	{"case", CASE}, {"do", DO},
	{"break", BREAK}, {"default", DEFAULT},
	{"continue", CONTINUE},
	{"for", FOR}, {"in", IN},
	{"inline", INLINE}, {"const", CONST},
	{"static", STATIC}, {"extern", EXTERN},
	{"sizeof", SIZEOF}, {"typeof", TYPEOF}
};

int main (int argc, char** argv)
{
	Trie keywords = prefixTree (KEYWORDS);
	
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
