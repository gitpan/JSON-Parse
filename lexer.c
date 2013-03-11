#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unicode.h"
#include "json_parse.h"
#include "lexer.h"
#include "json_parse_grammar.tab.h"

/* The macro "MESSAGE" produces detailed messages on what the lexer is
   doing. In normal use of the JSON parser, these messages are not
   wanted. */

#ifdef _WIN32

static void message (const char * format, ...)
{
    /* Do nothing, just return. */

    return;
}

#define MESSAGE message

#else /* not _WIN32 */

#if 0

/* This is for debugging, it prints a message about everything the
   code is doing. */

#define MESSAGE(format, args...) {		\
	printf ("%s:%d: ", __FILE__, __LINE__);	\
	printf (format, ## args);		\
    }

#else /* 0 */

#define MESSAGE(format, args...)

#endif /* 0 */

#endif /* _WIN32 */

/* Match for whitespace, as defined by the JSON specification. This
   also increments the line number. */

#define WHITESPACE         \
    '\n':                  \
    b->line++;             \
 case ' ':                 \
 case '\t':                \
 case '\r'

/* JSON grammar artefacts. */

#define GRAMMAR \
    '{':        \
 case '}':      \
 case '[':      \
 case ']':      \
 case ':':      \
 case ','

/* The digits from one to nine. */

#define DIGIT19                                      \
    '1':                                             \
 case '2':                                           \
 case '3':                                           \
 case '4':                                           \
 case '5':                                           \
 case '6':                                           \
 case '7':                                           \
 case '8':                                           \
 case '9'

/* The "e" in things like 1.0e9. */

#define EXP            \
    'e':               \
 case 'E'

/* The digits from 0-9. JSON does not allow numbers of the form
   000134, with leading zeros, so it is necessary to distinguish
   digits which may be initial numbers from digits which may be parts
   of a number. */

#define DIGIT                                      \
    '0':                                           \
 case DIGIT19

/* The following two macros are for hexadecimal digits. */

#define abcdef \
    'a':       \
 case 'b':     \
 case 'c':     \
 case 'd':     \
 case 'e':     \
 case 'f'

#define ABCDEF \
    'A':       \
 case 'B':     \
 case 'C':     \
 case 'D':     \
 case 'E':     \
 case 'F'

/* This begins each block. */

#define STATE(x)                                \
    MESSAGE ("My state is now %s.\n", #x);	\
    x:                                          \
    c = * p;                                    \
    p++;                                        \
    switch (c) {                                \
 case 0:                                        \
 MESSAGE ("End of the file.\n");		\
 return FILEEND;                               

#define PUSH p--

/* Return a token to the parser. */

#define GOT(x)						\
    MESSAGE("I have got a valid token with id %d.\n",	\
	    x);						\
    *json_ptr = p;					\
    return x;

#define GOT_VALUE(x)                                  \
    b->value[b->characters] = '\0';                   \
    GOT(x)

#define STRING chars
#define FILEEND eof
#define ERROR_INITIAL json_parse_lex_fail
#define ERROR_HEX json_parse_unicode_fail
#define ERROR_ESCAPE json_parse_unknown_escape_fail
#define ERROR_START_NUMBER json_parse_number_fail

/* End a switch (transition table) with an appropriate failure (x). */

#define END(x)                                          \
    default:                                            \
    b->status = ERROR_ ## x;                            \
    /* Set the pointer to the offending byte. */	\
    *json_ptr = p - 1;					\
    MESSAGE ("Failing lex stage at character '%c',"	\
	     " line %d, with value %s\n", c,		\
	     b->line,					\
	     #x);					\
    return -1;                                          \
}

#define BAD_BOYS					\
    1:							\
 case 2:						\
 case 3:						\
 case 4:						\
 case 5:						\
 case 6:						\
 case 7:						\
 case 8:						\
 case 9:						\
 case 10:						\
 case 11:						\
 case 12:						\
 case 13:						\
 case 14:						\
 case 15:						\
 case 16:						\
 case 17:						\
 case 18:						\
 case 19:						\
 case 20:						\
 case 21:						\
 case 22:						\
 case 23:						\
 case 24:						\
 case 25:						\
 case 26:						\
 case 27:						\
 case 28:						\
 case 29:						\
 case 30:						\
 case 31

/* End a switch (transition table) without a failure. */

#define END_NO_DEFAULT }

#define BUFFER_SIZE 0x400

static int add_value (buffer_t * b)
{
    if (b->value == 0) {
        b->allocated = BUFFER_SIZE;
        b->value = malloc (b->allocated);
    }
    else {
        b->allocated *= 2;
        b->value = realloc (b->value, b->allocated);
    }
    if (! b->value) {
        return 0;
    }
    return 1;
}

#define ADDC(ch)                                 \
    MESSAGE("Adding '%c'.\n", ch);               \
    b->value[b->characters] = ch;                \
    b->characters++;                             \
    if (b->characters >= b->allocated) {         \
	int ok;					 \
        ok = add_value (b);			 \
	/* Cope with malloc failure. */		 \
	if (! ok) {				 \
	    b->status = json_parse_memory_fail;	 \
	    return -1;				 \
	}					 \
    }

#define ADD ADDC(c)

/* Reset the lexer. */

#define RESET                                   \
    if (! b->value) {                           \
        add_value (b);                          \
    }                                           \
    b->characters = 0

#define LITERAL(initial,value)                           \
    case initial:                                        \
    PUSH;                                                \
    MESSAGE ("Testing for %s\n", #value);                \
    if (strncmp (p, #value, strlen (#value)) == 0) {     \
        * json_ptr = p + strlen (#value);                \
        MESSAGE ("Got %s, ptr now %c\n",                 \
                 #value, ** json_ptr);                   \
        return value;                                    \
    }                                                    \
    else {                                               \
        MESSAGE ("Failed test for %s\n", #value);        \
        b->status = ERROR_INITIAL;                       \
        return -1;                                       \
    }

static int
buffer_add_unicode (buffer_t * b, int ucs2)
{
    unsigned char utf8 [UTF8_MAX_LENGTH];
    int utf8_bytes;
    int i;

    MESSAGE ("Adding unicode value %X\n", ucs2);
    utf8_bytes = ucs2_to_utf8 (ucs2, utf8);
    if (utf8_bytes <= 0) {
        b->status = json_parse_unicode_fail;
        return -1;
    }
    for (i = 0; i < utf8_bytes; i++) {
        ADDC (utf8[i]);
    }
    return 0;
}

#define HEX_CHECK(sub, offset)                                          \
    hex = hex * 16 + c - sub + offset;                                  \
    hex_digits++;                                                       \
    if (hex_digits >= 4) {                                              \
        buffer_add_unicode (b, hex);                                    \
        /* Convert the Unicode into UTF-8 */                            \
        goto string;                                                    \
    }                                                                   \
    goto four_hex;

/* This is the lexer. */

int lexer (void * ignore, const char ** json_ptr, json_parse_object * jpo_x)
{
    const char * p = * json_ptr;
    char c;
    int hex_digits;
    int hex;
    int num;
    int minus;
    int leading_zeros;
    buffer_t * b = & jpo_x->buffer;

    if (! b->value) {
        add_value (b);
    }

    num = 0;
    minus = 0;
    leading_zeros = 0;

    /* This transition table parses the initial state. */

    STATE(initial);

    case WHITESPACE:
        MESSAGE ("Skipping whitespace.\n");
        goto initial;

    case GRAMMAR:
        MESSAGE ("JSON grammar: '%c'\n", c);
        GOT (c);

    case '.':
        ADD;
        GOT (c);

	/* "+" does nothing and has no effect, so this lexer just
	   skips it. */

    case '+':
        goto initial;

    case EXP:
        ADD;
        GOT (e);

    case '-':
    case DIGIT:
        PUSH;
        goto number;

    case '"':
        RESET;
        goto string;

	/* Look for the three literals, "true", "false", and "null". */

    LITERAL ('t', true);
    LITERAL ('f', false);
    LITERAL ('n', null);
    END(INITIAL);

    /* This transition table parses numbers. */

    STATE(number);

    case '-':
        ADD;
        if (! minus) {
            minus = 1;
        }
        else {
            minus = 0;
        }
        goto number;

    case '0':
        if (! num) {
            leading_zeros = 1;
        }
    case DIGIT19:
        ADD;
        if (! num) {
            num = c - '0';
            if (minus) {
                num = - num;
            }
        }
        else {
            num *= 10;
            num += c - '0';
        }
        goto number;

    default:

	/* The character we have just read is not part of a number, so
	   push it back on the stack and decide what to do with all
	   the bits and pieces accumulated so far. */

        PUSH;

        jpo_x->integer = num;
        if (minus) {
            if (leading_zeros) {
                GOT (nzinteger);
            }
            else {
                GOT (ninteger);
            }
        }
        else {
            if (leading_zeros) {
                if (num) {
                    GOT (pzinteger);
                }
                else {
                    GOT (zero);
                }
            }
            else {
                GOT (pinteger);
            }
        }
    END_NO_DEFAULT

    /* This transition table parses a string, which means
       something beginning with " and ending with ". */

    STATE(string);

    case '\\':
        MESSAGE ("Escape.\n");
        goto string_escape;

    case '"':
        MESSAGE ("End of string.\n");
        GOT_VALUE (STRING);

	/* Bug: this does not check the contents of the string to make
	   sure they are definitely valid JSON string characters. */
	
    case BAD_BOYS:
	b->status = json_parse_control_character_fail;
	/* Set the pointer to the offending byte. */
	*json_ptr = p - 1;
	return -1;

    default:
        ADD;
        goto string;

    END_NO_DEFAULT;

    /* This transition table parses \ appearing in a string. For
       everything except \u, that means adding one character to the
       buffer. */

    STATE(string_escape);
    case '\\':
    case '/':
    case '"':
	/* Add the character itself. */
        ADD;
        goto string;
    case 'b':
        ADDC ('\b');
        goto string;
    case 'f':
        ADDC ('\f');
        goto string;
    case 'n':
        ADDC ('\n');
        goto string;
    case 'r':
        ADDC ('\r');
        goto string;
    case 't':
        ADDC ('\t');
        goto string;
    case 'u':
        MESSAGE ("Looking for Unicode.\n");
        hex_digits = 0;
        hex = 0;
        goto four_hex;
    END(ESCAPE);

    /* This transition table parses \u escaped Unicode characters like
       "\u3000". */

    STATE(four_hex);
    case DIGIT:
        HEX_CHECK('0', 0);
    case abcdef:
        HEX_CHECK('a', 10);
    case ABCDEF:
        HEX_CHECK('A', 10);
    END(HEX);
}

#ifdef TEST

#include "mini-file-library.h"

int main ()
{
    char * text;
    buffer_t b = {0};
    int i;
    mini_file_read_all ("wibble.json", & text);

    for (i = 0; i < 100000; i++) {
        char * t = text;
        while (1) {
            int i = lexer (& t, & b);
            switch (i) {
            case ERROR_ESCAPE:
                MESSAGE ("Error in escape.\n");
                goto end_parsing;
            case ERROR_INITIAL:
                MESSAGE ("Error in initial state, byte %d.\n", t - text);
                goto end_parsing;
            case ERROR_HEX:
                MESSAGE ("Error in hex.\n");
                goto end_parsing;
            case FILEEND:
                goto end_parsing;
            case STRING:
                //                b.value[b.characters] = '\0';
                //printf ("STRING: %s\n", b.value);
                break;
            case GRAMMAR:
                //printf ("%c\n", i);
                break;
            case NUMBER:
                //printf ("%c\n", i);
                break;
            default:
                printf ("Unknown state %d\n", i);
                ;
            }
        }
        end_parsing:
            ;
    }
    return 0;
}

#endif
