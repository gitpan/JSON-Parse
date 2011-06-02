#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unicode.h"
#include "lexer.h"
#include "json_parse.h"
#include "json_parse_grammar.tab.h"

#if 0
#define MESSAGE(format, args...) {\
    printf (format, ## args);\
    }
#else
#define MESSAGE(format, args...)
#endif

#define WHITESPACE         \
    '\n':                  \
    b->line++;             \
 case ' ':                 \
 case '\t':                \
 case '\r'

#define GRAMMAR \
    '{':        \
 case '[':      \
 case ']':      \
 case '}':      \
 case ':':      \
 case ','

#define START_NUMBER \
    '1':             \
 case '2':           \
 case '3':           \
 case '4':           \
 case '5':           \
 case '6':           \
 case '7':           \
 case '8':           \
 case '9'

#define DIGIT                                   \
    START_NUMBER:                               \
    case '0'

#define abcdef                                    \
    'a':                                          \
 case 'b':                                        \
 case 'c':                                        \
 case 'd':                                        \
 case 'e':                                        \
 case 'f'

#define ABCDEF                                       \
    'A':                                             \
 case 'B':                                           \
 case 'C':                                           \
 case 'D':                                           \
 case 'E':                                           \
 case 'F'

#define STATE(x)                                \
    x:                                          \
    c = * p;                                    \
    p++;                                        \
    switch (c) {                                \
 case 0:                                        \
 return FILEEND;                               

#define PUSH p--

#define GOT(x)                                  \
    *json_ptr = p;                              \
    return x;

#define GOT_VALUE(x)                            \
    b->value[b->characters] = '\0';                   \
    GOT(x)

#define STRING chars
#define NUMBER number
#define FILEEND eof
#define ERROR_INITIAL json_parse_lex_fail
#define ERROR_HEX json_parse_unicode_fail
#define ERROR_ESCAPE json_parse_unknown_escape_fail
#define ERROR_START_NUMBER json_parse_number_fail

/* End a switch (transition table) with an appropriate failure (x). */

#define END(x)                                          \
    default:                                            \
    b->status = ERROR_ ## x;                            \
    MESSAGE ("Failing lex stage with value %s\n",       \
            #x);                                        \
    return -1;                                          \
}

/* End a switch (transition table) without a failure. */

#define CLOSE }

#define BUFFER_SIZE 0x400

#ifdef HEADER

typedef struct buffer {
    char * value;
    int characters;
    int allocated;
    int line;
    int /* json_parse_status */ status;
} buffer_t;

#endif

static int add_value (buffer_t * b)
{
    if (b->value == 0) {
        b->allocated = BUFFER_SIZE;
        b->value = malloc (b->allocated);
        if (! b->value) {
            return 0;
        }
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
    b->value[b->characters] = ch;                \
    b->characters++;                             \
    if (b->characters >= b->allocated) {         \
        add_value (b);                           \
    }

#define ADD ADDC(c)

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
        * json_ptr += strlen (#value);                   \
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
        ADDC(utf8[i]);
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

int lexer (void * ignore, const char ** json_ptr, buffer_t * b)
{
    const char * p = * json_ptr;
    char c;
    int hex_digits;
    int hex;

    STATE(initial);
    case WHITESPACE:
        goto initial;
    case GRAMMAR:
        //MESSAGE ("%c\n", c);
        GOT (c);
    case '-':
        RESET;
        ADD;
        goto start_number;
    case DIGIT:
        RESET;
        PUSH;
        goto start_number;
    case '"':
        RESET;
        goto string;
    LITERAL ('t', true);
    LITERAL ('f', false);
    LITERAL ('n', null);
    END(INITIAL);

    STATE(string);
    case '\\':
        MESSAGE ("Escape.\n");
        goto string_escape;
    case '"':
        GOT_VALUE (STRING);
    default:
        ADD;
        goto string;
    CLOSE;

    STATE(string_escape);
    case '\\':
    case '/':
    case '"':
        ADD;
        goto string;
    case 'b':
        ADDC (8);
        goto string;
    case 'f':
        ADDC (12);
        goto string;
    case 'n':
        ADDC (10);
        goto string;
    case 'r':
        ADDC (13);
        goto string;
    case 't':
        ADDC (9);
        goto string;
    case 'u':
        MESSAGE ("Looking for Unicode.\n");
        hex_digits = 0;
        hex = 0;
        goto four_hex;
    END(ESCAPE);

    STATE(start_number);
    case DIGIT:
        ADD;
        goto number;
    END(START_NUMBER);

    STATE(number);
    case DIGIT:
    case 'e':
    case 'E':
    case '.':
        ADD;
        goto number;
    default:
        PUSH;
        GOT_VALUE (NUMBER);
    CLOSE;

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
