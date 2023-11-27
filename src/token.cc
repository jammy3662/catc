#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <array>
using namespace std;
//what
//\
trees
void insert (const char* str, char result, Trie* trie)
{
	Trie* tree = trie;
	Trie* parent = 0;
	
	for (; *str; ++str)
	{
		if (tree->value == -1)
		{
			// populate the first node at this level
			tree->value = *str;
		}
		
		// try to match the character at this level
		for (; *str != tree->value; tree = tree->next)
		{
			if (!tree->next)
			{
				// if not there, add it to the end
				tree->next = (Trie*) malloc (sizeof (Trie));
				*tree->next = {.value = *str, .next = 0x0, .match = 0x0, .result = 0};
			}
		}
		
		if (!tree->match)
		{
			tree->match = (Trie*) malloc (sizeof (Trie));
			*tree->match = {.value = -1, .next = 0x0, .match = 0x0, .result = 0};
		}
		parent = tree;
		tree = tree->match;
	}
	
	// null character can be left out;
	// store result in last character of key
	free (tree);
	tree = parent;
	tree->match = 0x0;
	tree->result = result;
}

char find (const char* key, Trie trie)
{
	Trie* tree = &trie;
	
	char result = 0;
	
	for (; *key; ++key)
	{
		result = tree->result;
		
		// try to match the character at this level
		for (; tree->value != *key; tree = tree->next)
		{
			// no more characters to test against
			if (!tree->next) return 0;
		}
		
		if (tree->match) tree = tree->match;
	}
	
	return result;
}

Trie prefixTree (list <Key> keys)
{
	Trie ret = {};
	ret.value = -1;
	
	for (Key key : keys)
	{
		insert (key.key, key.value, &ret);
	}
	
	return ret;
}

template <typename ... Ts>
Trie prefixTree (Ts... keys)
{
	constexpr int ct = sizeof... (keys);
	Key all [ct] = {keys...};
	
	Trie ret;
	
	for (int i = 0; i < ct; ++i)
	{
		Key key = all [i];
		insert (key.key, key.value, &ret);
	}
	
	return ret;
}

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
		return SYMBOL;
	
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
		
		switch (ret.id)
		{
		case SYMBOL:
			if (!(alphachar(*current)
			|| numchar (*current)
			|| *current == '_'))
				  goto finalize;
			break;
		
		case INT_LITERAL:
			if (!numchar (*current) && *current != '_')
			{
				switch (*current)
				{
				case '.':
					type = FLOAT_LITERAL;
					break;
				case 'f':
				case 'F':
					type = FLOAT_LITERAL;
					continue;
				default:
					goto finalize;
				}
			}
			break;
		
		case FLOAT_LITERAL:
			if (!numchar (*current) && *current != '_')
			{
				switch (*current)
				{
				case 'f':
				case 'F':
					hold = 0;
					*current = 0;
					return ret;
				default:
					break;
				}
				if (*current == 'f'
				|| *current == 'F')
				{
					hold = 0;
					*current = 0;
					return ret;
				}
				goto finalize;
			}
			break;
		
		case HEX_LITERAL:
			if (!(numchar(*current)
			|| alphachar (*current)
			|| *current == '_'))
				goto finalize;
			break;
		
		case BIN_LITERAL:
			if (*current != '0' && *current != '1' && *current != '_')
				goto finalize;
			break;
		
		case OCT_LITERAL:
			if (!(*current >= '0' && *current <= '7') && *current != '_')
				goto finalize;
			break;
		
		case STRING_LITERAL:
			if (*current == '\"' && !escape)
			{
				*(current + 1) = 0;
				return ret;
			}
			break;
		
		case CHAR_LITERAL:
			if (*current == '\'' && !escape)
			{
				*(current + 1) = 0;
				return ret;
			}
			break;
		
		case LINE_COMMENT:
			if (*current == '\n')
			{
				goto finalize;
			}
			break;
		
		case BLOCK_COMMENT:
			if (*current == '/' &&
			    *(current - 1) == '*')
			{
				*(current - 1) = 0;
				return ret;
			}
			break;
		
		case '=':
			*(current + 1) = 0;
			
			switch (*(current - 1))
			{
			case '=':
				ret.id = COMPARE;
				current++;
				goto finalize;
			case '+':
				ret.id = PLUSEQUAL;
				current++;
				goto finalize;
			case '-':
				ret.id = MINUSEQUAL;
				current++;
				goto finalize;
			case '*':
				ret.id = TIMESEQUAL;
				current++;
				goto finalize;
			case '/':
				ret.id = DIVEQUAL;
				current++;
				goto finalize;
			case '<':
				ret.id = LESSEQUAL;
				current++;
				goto finalize;
			case '>':
				ret.id = MOREEQUAL;
				current++;
				goto finalize;
			default:
				goto finalize;
			}
		
		case '+':
			if (*(current-1) == '+')
				ret.id = INCREMENT;
			goto finalize;
		
		case '-':
			if (*(current-1) == '-')
				ret.id = DECREMENT;
			goto finalize;
		
		case '&':
			if (*(current-1) == '&')
				ret.id = AND;
			goto finalize;
		
		case '|':
			if (*(current-1) == '|')
				ret.id = OR;
			goto finalize;
		
		case '<':
			if (*(current-1) == '<')
				ret.id = SHIFT_L;
			goto finalize;
		
		case '>':
			if (*(current-1) == '>')
				ret.id = SHIFT_R;
			goto finalize;
		
		default:
			goto finalize;
		}
	}
	
	finalize:
	
	if (*current > ' ' && *current <= '~')
		hold = *current;
	
	*current = 0; // null terminator
	
	// parse keywords
	if (type == SYMBOL)
	{
		if ((int)(current - ret.text) <= 8)
		{
			char* text = ret.text;
			
			if (ret.text [0] == 'u')
				ret.negative = false, text++;
			
			if (!strcmp (ret.text, "void"))
				ret.id = VOID_T;
			else
			if (!strcmp (ret.text, "var"))
				ret.id = VAR_T;
			else
			if (!strcmp (text, "char"))
				ret.id = CHAR_T;
			else
			if (!strcmp (text, "short"))
				ret.id = SHORT_T;
			else
			if (!strcmp (text, "int"))
				ret.id = INT_T;
			else
			if (!strcmp (text, "long"))
				ret.id = LONG_T;
			else
			if (!strcmp (ret.text, "float"))
				ret.id = FLOAT_T;
			else
			if (!strcmp (ret.text, "double"))
				ret.id = DOUBLE_T;
			else
			if (!strcmp (ret.text, "struct"))
				ret.id = STRUCT_T;
			else
			if (!strcmp (ret.text, "union"))
				ret.id = UNION_T;
			else
			if (!strcmp (ret.text, "enum"))
				ret.id = ENUM_T;
		}
	}
	
	return ret;
}
