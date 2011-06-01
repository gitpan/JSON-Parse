/* Copyright (c) 2010-2011 Ben Bullock (bkb@cpan.org). */

%{

#include <string.h>
#include <stdlib.h>
#include "unicode.h"
#include "json_parse.h"
#include "json_parse_grammar.tab.h"

#define YY_EXTRA_TYPE json_parse_object *

/* The initial size allocated for the buffer. */

#define BASE_SIZE 0x400

#define CALL(function) {                        \
        json_parse_status js=function;          \
        if (js != json_parse_ok) {              \
            return js;                          \
        }                                       \
    }

/*  ____  _        _               _           _ _     _               
   / ___|| |_ _ __(_)_ __   __ _  | |__  _   _(_) | __| | ___ _ __ ___ 
   \___ \| __| '__| | '_ \ / _` | | '_ \| | | | | |/ _` |/ _ \ '__/ __|
    ___) | |_| |  | | | | | (_| | | |_) | |_| | | | (_| |  __/ |  \__ \
   |____/ \__|_|  |_|_| |_|\__, | |_.__/ \__,_|_|_|\__,_|\___|_|  |___/
                           |___/                                        */

/* These functions build up the string to return as "yylval->chrs". */

/* Resize the buffer (double its size). */

static json_parse_status
buffer_resize (json_parse_object * jpo)
{
    int i;
    if (jpo->buffer.length == 0) {
        jpo->buffer.chrs = malloc (BASE_SIZE);
	if (! jpo->buffer.chrs)
	    return json_parse_memory_fail;
	jpo->buffer.size = BASE_SIZE;
    } else {
        size_t new_size = jpo->buffer.size * 2;
	jpo->buffer.chrs = realloc (jpo->buffer.chrs, new_size);
	if (! jpo->buffer.chrs)
	    return json_parse_memory_fail;
	jpo->buffer.size = new_size;	
    }
    for (i=jpo->buffer.length;i<jpo->buffer.size;i++)
        jpo->buffer.chrs[i] = '\0';
    return json_parse_ok;
}

/* Add one character to the buffer. */

static inline json_parse_status
buffer_add (json_parse_object * jpo, char c)
{
    size_t new_length = jpo->buffer.length + 1;
    if (new_length >= jpo->buffer.size) {
	CALL(buffer_resize (jpo));
    }
    jpo->buffer.chrs[jpo->buffer.length] = c;
    jpo->buffer.length = new_length;
    return json_parse_ok;
}

static json_parse_status
buffer_finish (YYSTYPE * json_parse_lval, json_parse_object * jpo)
{
    jpo->buffer.chrs[jpo->buffer.length]= '\0';
    jpo->buffer.length = 0;
    json_parse_lval->chrs = jpo->buffer.chrs;
    return json_parse_ok;
}

/* Add a JSON escape character to the buffer. The JSON escape
   characters begin with a backslash followed by a letter or ASCII as
   follows. See page five of RFC 4267 for definitions. */

static json_parse_status
buffer_add_escape (json_parse_object * jpo, char c)
{
    switch (c) {
    case '"':	return buffer_add (jpo, c);
    case '\\':	return buffer_add (jpo, c);
    case '/':	return buffer_add (jpo, c);
    case 'b':	return buffer_add (jpo, 8);
    case 'f':	return buffer_add (jpo, 12);
    case 'n':	return buffer_add (jpo, 10);
    case 'r':	return buffer_add (jpo, 13);
    case 't':	return buffer_add (jpo, 9);
    default:	return json_parse_unknown_escape_fail;
    }
}

/* Copy the whole of yytext into the buffer. */

static json_parse_status
buffer_copy_text (YYSTYPE * json_parse_lval, json_parse_object * jpo, const char * text)
{
    size_t length = strlen (text);
    while (length >= jpo->buffer.size) {
	CALL(buffer_resize (jpo));
    }
    strcpy (jpo->buffer.chrs, text);
    jpo->buffer.length = length;
    return buffer_finish (json_parse_lval, jpo);
}

/* Add a unicode character to the buffer from the UCS-2 of its
   bytes. */

static json_parse_status
buffer_add_unicode (json_parse_object * jpo, const char * ucs2_hex)
{
    int ucs2;
    char * end_pointer;
    unsigned char utf8 [UTF8_MAX_LENGTH];
    int utf8_bytes;
    int i;

    ucs2 = strtol (ucs2_hex, & end_pointer, 16);
    utf8_bytes = ucs2_to_utf8 (ucs2, utf8);
    //printf ("%s -> UCS-2: %X %d bytes.\n", ucs2_hex, ucs2, utf8_bytes);
    if (utf8_bytes <= 0) {
        return json_parse_unicode_fail;
    }
    for (i = 0; i < utf8_bytes; i++) {
        buffer_add (jpo, utf8[i]);
    }
    return json_parse_ok;
}

/* Free the memory associated with the buffer. */

void
buffer_free (json_parse_object * jpo)
{
    if (jpo->buffer.chrs) {
        free (jpo->buffer.chrs);
    }
}

/*
  __  __                             __              _                    
 |  \/  | __ _  ___ _ __ ___  ___   / _| ___  _ __  | | _____  _____ _ __ 
 | |\/| |/ _` |/ __| '__/ _ \/ __| | |_ / _ \| '__| | |/ _ \ \/ / _ \ '__|
 | |  | | (_| | (__| | | (_) \__ \ |  _| (_) | |    | |  __/>  <  __/ |   
 |_|  |_|\__,_|\___|_|  \___/|___/ |_|  \___/|_|    |_|\___/_/\_\___|_|   
                                                                         
*/

/* These macros are for the lexer only. */

/* Debugging messages. */

#if 0
#define MESSAGE(x, args...) {                           \
        printf ("%s:%d: status: %d: line %d: ",         \
                __FILE__, __LINE__,                     \
                yyextra->js, yylineno);                 \
        printf (x, ## args);                            \
        printf ("\n");                                  \
}
#else
#define MESSAGE(x, args...)
#endif

/* Add one character from the lex input to the buffer. */

#define ADD(n) buffer_add (yyextra, yytext[n])

/* Stop the lexer and set the error status to "failure". */

#define FAIL(failure)             \
    yyextra->js = failure;        \
    return -1;

/* Check the status and return if it is not OK. */

#define CHK                                     \
    if (js != json_parse_ok) {                  \
        FAIL(js);                               \
    }

#define JS json_parse_status js =

%}

/* Don't include "unistd.h". */

%option nounistd

/* Add extra arguments to the yylex function to talk to Bison. */

%option bison-bridge

 /* Make a reentrant (thread-safe) parser. */

%option reentrant

%option noyywrap

/* Do the line numbers. */

%option yylineno

 /* Prefix to use instead of "yy". See Flex manual, section 16.3 */

%option prefix="json_parse_lex_"

 /* See RFC 4267 p.4 */

NUMBER  -?([1-9][0-9]*|0)(\.[0-9]+)?([eE][-+]?[0-9]+)?

 /* Escapes, like \t for tab. See RFC 4267 p.5 */

ESCAPE	\\["\\/bfnrt]

/* White space. See RFC 4267 p.2 */

WSPACE  [\x09\x0A\x0D\x20]

/* JSON Unicode UCS-2 point, like \u9aaa. */

UNICODE \\u[0-9a-fA-F]{4}

/* Regular expressions for UTF-8 bytes. */

UTF8_2	[\xC2-\xDF][\x80-\xBF]
UTF8_3  [\xE1-\xEF][\x80-\xBF]{2}
UTF8_4	[\xF0-\xF4][\x80-\xBF]{3}

/* There are two states, INITIAL and STRING. */

%s STRING 

%%

<INITIAL>[\[\]{},:]	{ return yytext[0]; }
<INITIAL>\"		{ BEGIN(STRING); }
<INITIAL>{NUMBER}	{ JS buffer_copy_text (yylval, yyextra, yytext); CHK; return number; }
<INITIAL>{WSPACE}	{ }
<INITIAL>true		{ return true; }
<INITIAL>false		{ return false; }
<INITIAL>null		{ return null; }
<INITIAL>.		{ MESSAGE("unknown character");FAIL(json_parse_lex_fail) }
<<EOF>>                 { MESSAGE("eof");return eof; }
\0                      { MESSAGE("null = eof");return eof; }

<STRING>\"	  	{ JS buffer_finish (yylval, yyextra); CHK; 
                          BEGIN(INITIAL); return chars; }
<STRING>[\x20-\x7E]	{ ADD(0); }
<STRING>{ESCAPE}	{ JS buffer_add_escape (yyextra, yytext[1]); CHK }
<STRING>{UNICODE}	{ buffer_add_unicode (yyextra, yytext + strlen ("\\u")); }
<STRING>{UTF8_2}	{ ADD(0); ADD(1); }
<STRING>{UTF8_3}	{ ADD(0); ADD(1); ADD(2); }
<STRING>{UTF8_4}	{ ADD(0); ADD(1); ADD(2); ADD(3); }
<STRING>.		{ MESSAGE("unknown character in string");FAIL(json_parse_lex_fail); }

%%

/*
   Local variables:
   mode: text
   End: 
*/
