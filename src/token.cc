#include "token.h"
#include <stdlib.h>
#include <array>
using namespace std;

char hold = 0;

void get (char* c, FILE* source = 0x0)
{
	static FILE* src = 0x0;
	if (!src) src = source;
	
	if (hold)
	{
		*c = hold;
		hold = 0;
	}
	else
		*c = fgetc (src);
}

bool alphachar (char c)
{
	return (c >= 'a' && c <= 'z' ||
	        c >= 'A' && c <= 'Z');
}

bool numchar (char c)
{
	return (c >= '0' && c <= '9');
}

char getKind (char* current, FILE* src)
{
	if (hold)
	{
		*current = hold;
		hold = 0;
	}
	else
		*current = fgetc (src);
	
	while ((unsigned char)*current <= ' ')
		// skip over whitespace chars
		*current = fgetc (src);
	
	if (*current == (char)-1) return NULLTOK;
	
	if (numchar (*current))
	{
		if (*current == '0')
		{
			hold = fgetc (src);
			
			if (hold == 'b')
				return BIN_LITERAL;
			else
			if (hold == 'o')
				return OCT_LITERAL;
			else
			if (hold == 'x')
				return HEX_LITERAL;
		}
		return INT_LITERAL;
	}
	else
	
	if (alphachar (*current) || *current == '_')
		return TEXT;
	
	else if (*current == '"') return STRING_LITERAL;
	else if (*current == '\'') return CHAR_LITERAL;
	
	else if (*current == '/')
	{
		hold = fgetc (src);
		
		// in case of a comment,
		// skip over the slashes / asterisk
		if (hold == '/')
		{
			*current = fgetc (src);
			hold = 0;
			return LINE_COMMENT;
		}
		else
		if (hold == '*')
		{
			*current = fgetc (src);
			hold = 0;
			return BLOCK_COMMENT;
		}
		
		return '/';
	}
	else
	if (*current > ' ' &&
	    *current <= '~') return *current;
	
	return NONE;
}

Token getTok (FILE* src)
{
	if (!src)
	{
		fprintf (stderr, "Pass an open file descriptor\n");
		return (Token){.id = NULLTOK};
	}
	
	Token ret = (Token) {};
	
	const int lookahead = 2;
	int bufsize = 16;
	bool escape = false;
	
	ret.text = (char*) malloc (bufsize * sizeof (char));
	
	char* current = ret.text;
	
	ret.id = getKind (current, src);

	int& type = ret.id;
	
	while (*current != -1)
	{
		// expand the buffer when needed
		int numread = (int) (current - ret.text);
		if (numread + lookahead >= bufsize)
		{
			if (bufsize > 2000)
			{
				fprintf (stderr, "Token overflow (%i)\n", bufsize);
				exit (1);
			}
			
			bufsize *= 2;
			ret.text = (char*) realloc (ret.text, bufsize * sizeof (char));
			current = ret.text + numread; // move current char pointer to new memory location
		}
		
		if (*current == '\\' && !escape)
			escape = true;
		else
			escape = false;
		
		current++;
		get (current, src);
		
		if (*current == -1)
			return (Token){.id = NULLTOK};
		
		if (type == TEXT)
		{
			if (!(
			   alphachar(*current)
			|| numchar (*current)
			|| *current == '_'))
				  break;
		}
		else
		if (type == INT_LITERAL)
		{
			if (!numchar (*current) && *current != '_')
			{
				if (*current == '.') type = FLOAT_LITERAL;
				else
				if (*current == 'f'
				|| *current == 'F')
				{
					type = FLOAT_LITERAL;
					continue;
				}
				else
					break;
			}
		}
		else
		if (type == FLOAT_LITERAL)
		{
			if (!numchar (*current) && *current != '_')
			{
				if (*current == 'f'
				|| *current == 'F')
				{
					hold = 0;
					*current = 0;
					return ret;
				}
				break;
			}
		}
		else
		if (type == HEX_LITERAL)
		{
			if (!(
			   numchar(*current)
			|| alphachar (*current)
			|| *current == '_'))
				break;
		}
		else
		if (type == BIN_LITERAL)
		{
			if (*current != '0' && *current != '1' && *current != '_')
				break;
		}
		else
		if (type == OCT_LITERAL)
		{
			if (!(*current >= '0' && *current <= '7') && *current != '_')
				break;
		}
		else
		if (type == STRING_LITERAL)
		{
			if (*current == '\"' && !escape)
			{
				*(current + 1) = 0;
				return ret;
			}
		}
		else
		if (type == CHAR_LITERAL)
		{
			if (*current == '\'' && !escape)
			{
				*(current + 1) = 0;
				return ret;
			}
		}
		else
		if (type == LINE_COMMENT)
		{
			if (*current == '\n')
			{
				break;
			}
		}
		else
		if (type == BLOCK_COMMENT)
		{
			if (*current == '/' &&
			    *(current - 1) == '*')
			{
				*(current - 1) = 0;
				return ret;
			}
		}
		else break;
	}
	
	if (*current > ' ' && *current < '~')
		hold = *current;
	
	*current = 0; // null terminator
	return ret;
}
