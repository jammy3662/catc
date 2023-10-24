#include "token.h"
#include <stdlib.h>

/*
I could put some tex here
if i wanted to *and it (should work)
*/

Token getTok (FILE* src)
{
	if (!src)
	{
		fprintf (stderr, "Pass an open file descriptor\n");
		return (Token){.id = NULLTOK};
	}
	
	Token ret;
	ret.id = NONE;
	ret.value = {};
	
	int bufsize = 16;
	int lookahead = 2;
	bool escape = false;
	
	ret.text = (char*) calloc (bufsize, sizeof (char));
	char* current = ret.text;
	
	static char hold = 0;
	
	#define get(CHAR) if (hold) {CHAR = hold; hold = 0;} else CHAR = fgetc (src)
	#define unget(CHAR) hold = CHAR
	
	int& type = ret.id;
	bool finished = false;
	
	get(*current);
	
	while (*current <= ' ')
	{
		if (*current == -1)
			return (Token){.id = NULLTOK};
		// skip over whitespace chars
		get(*current);
	}
	
	if (*current == -1)
		return (Token){.id = NULLTOK};
	
	if (*current >= '0' && *current <= '9')
	{
		if (*current == '0')
		{
			char next;
			get(next);
			
			if (next == 'b')
				type = BIN_LITERAL;
			else
			if (next == 'o')
				type = OCT_LITERAL;
			else
			if (next == 'x')
				type = HEX_LITERAL;
			else
			{
				type = INT_LITERAL;
				unget(next);
			}
		}
		else type = INT_LITERAL;
	}
	else if ((*current >= 'a' && *current <= 'z') ||
	         (*current >= 'A' && *current <= 'Z') ||
			  *current == '_')
		type = TEXT;
	
	else if (*current == '"') type = STRING_LITERAL;
	else if (*current == '\'') type = CHAR_LITERAL;
	
	else if (*current == '/')
	{
		char next;
		get(next);
		
		// in case of a comment,
		// skip over the slashes / asterisk
		if (next == '/')
		{
			type = LINE_COMMENT;
			get (*current);
		}
		else
		if (next == '*')
		{
			type = BLOCK_COMMENT;
			get (*current);
		}
		else
		{
			// didn't find a comment,
			// put the lookahead char back
			type = '/';
			unget(next);
			return ret;
		}
	}
	else
	if (*current > ' ' && *current <= '~')
	{
		type = *current;
		return ret;
	}
	else
	{
		type = NONE;
		return ret;
	}
	
	while (!finished)
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
		get(*current);
		
		if (*current == -1)
			return (Token){.id = NULLTOK};
		
		if (type == TEXT)
		{
			if (!
			    ((*current >= 'a' && *current <= 'z') ||
				 (*current >= 'A' && *current <= 'Z') ||
				 (*current >= '0' && *current <= '9') ||
				  *current == '_'))
				break;
		}
		
		else
		if (type == INT_LITERAL)
		{
			if (!(*current >= '0' && *current <= '9') && *current != '_')
			{
				if (*current == '.') type = FLOAT_LITERAL;
				else
				if (*current == 'f' ||
				    *current == 'F')
				{
					type = FLOAT_LITERAL;
					*current = 0;
					return ret;
				}
				else
					break;
			}
		}
		else
		if (type == FLOAT_LITERAL)
		{
			if (!(*current >= '0' && *current <= '9') && *current != '_')
			{
				if (*current == 'f' ||
				    *current == 'F')
				{
					*current = 0;
					return ret;
				}
				
				else
					break;
			}
		}
		else
		if (type == HEX_LITERAL)
		{
			if (!
			     ((*current >= '0' && *current <= '9') ||
			     (*current >= 'a' && *current <= 'f') ||
				 (*current >= 'A' && *current <= 'F') ||
				  *current == '_'))
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
				*current = 0;
				return ret;
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
	}
	
	if (*current > ' ')
		unget(*current);
	
	*current = 0; // null terminator
	return ret;
}
