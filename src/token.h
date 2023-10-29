#include <stdio.h>

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
	bool signd = true; // (only applies to int types)
	
	union
	{
		char c;
		short s;
		int i;
		float f;
		double d;
		long l;
		void* p;
	}
	value;
};

Token getTok (FILE* source);
