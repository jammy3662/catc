#include <stdio.h>
#include <initializer_list>

struct Trie
{
	char value;
	Trie* next,
	    * match;
	char result;
};

Trie prefixTree (std::initializer_list
	<const char*> strings);

void insert (const char* str, char result, Trie* tree);

char find (const char* key, Trie tree);

enum Tok
{
	// single printable ascii characters are their literal values
	// i.e. PLUS = '+'
	NULLTOK = -1,
	NONE = 0,
	
	LESSEQUAL = (char)('~' + 2),
	MOREEQUAL,
	PLUSEQUAL,
	MINUSEQUAL,
	TIMESEQUAL,
	DIVEQUAL,
	SHIFT_L,
	SHIFT_R,
	COMPARE,
	INCREMENT,
	DECREMENT,
	AND,
	OR,
	
	TEXT,
	
	VOID_T,
	VAR_T, // (void*)
	STRUCT_T,
	UNION_T,
	ENUM_T,
	CHAR_T, SHORT_T,
	INT_T, LONG_T,
	FLOAT_T, DOUBLE_T,
	
	INT_LITERAL,
	HEX_LITERAL,
	BIN_LITERAL,
	OCT_LITERAL,

	FLOAT_LITERAL,

	BOOL_LITERAL,
	CHAR_LITERAL,
	STRING_LITERAL,
	
	LINE_COMMENT,
	BLOCK_COMMENT,
};

struct Token
{
	int id;
	char* text;
	bool sined = true; // (only applies to int types)
	
	union
	{
		char c; short s; int i; long l;
		float f; double d; void* p;
	}
	value;
};

Token getTok (FILE* source);
