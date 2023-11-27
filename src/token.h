#include <stdio.h>
#include <initializer_list>
#include <array>

#define list std::initializer_list

struct Key
{
	const char* key;
	char value;
};

struct Trie
{
	char value;
	Trie* next = 0x0,
	    * match = 0x0;
	char result;
};

Trie prefixTree (list <Key> strings);

template <typename ... Ts>
Trie prefixTree (Ts... keys);

void insert (const char* key, char value, Trie* tree);
char find (const char* key, Trie tree);

// TODO: write a trie "compress" function that
// groups sequential characters together

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
	ELLIPSIS,
	
	SYMBOL,
	
	VOID_T,
	VAR_T, // (void*)
	STRUCT_T,
	UNION_T,
	ENUM_T,
	CHAR_T, SHORT_T,
	INT_T, LONG_T,
	FLOAT_T, DOUBLE_T,
	
	IF, ELSE, WHILE, SWITCH, CASE,
	DO, BREAK, DEFAULT, CONTINUE,
	FOR, IN, RETURN,
	
	INLINE, SIZEOF, TYPEOF,
	CONST, STATIC, EXTERN,
	
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
	bool negative = true; // signed flag for int types
	
	union
	{
		char c; short s; int i; long l;
		float f; double d; void* p;
	}
	value;
};

Token getTok (FILE* source);
