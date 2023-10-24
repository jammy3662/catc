#include <stdio.h>

enum Tok
{
	// single printable ascii characters are their literal values
	// i.e. PLUS = '+'
	NULLTOK = -1,
	NONE = 0,
	
	LESSEQUAL = '~' + 1,
	MOREEQUAL,
	PLUSEQUAL,
	MINUSEEQUAL,
	TIMESEQUAL,
	DIVEQUAL,
	SHIFT_L,
	SHIFT_R,
	
	COMPARE,
	
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
	
	TEXT,
};

struct Token
{
	int id;
	char* text;
	
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
