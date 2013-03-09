/* Copyright (c) 2010-2013 Ben Bullock (bkb@cpan.org). */

#include <stdio.h>
#include <stdlib.h>

#ifdef HEADER

/* This is a string holder type. */

typedef struct buffer {
    /* The string itself. */
    char * value;
    /* The number of valid characters in the string. */
    int characters;
    /* The number of characters allocated for the string. */
    int allocated;
    /* The line number this string originates from. */
    int line;
    int /* json_parse_status */ status;
}
buffer_t;

typedef enum {
    json_parse_ok,
    json_parse_memory_fail,
    json_parse_grammar_fail,
    json_parse_lex_fail,
    json_parse_unicode_fail,
    json_parse_no_input_fail,
    json_parse_bad_start_fail,
    json_parse_unknown_escape_fail,
    json_parse_n_statuses,
} 
json_parse_status;

typedef enum {
    json_null,
    json_true,
    json_false
} 
json_type;

/* User object */
typedef void * json_parse_u_obj;

/* User data */
typedef void * json_parse_u_data;

/* Place for user to return a newly-created object */
typedef json_parse_u_obj * json_parse_new_u_obj;

/* Function types */
typedef json_parse_status 
(*json_parse_create_sn)
(json_parse_u_data, const char *, json_parse_new_u_obj);
typedef json_parse_status 
(*json_parse_create_n)
(json_parse_u_data, int, json_parse_new_u_obj);
typedef json_parse_status 
(*json_parse_create_ao)
(json_parse_u_data, json_parse_new_u_obj);
typedef json_parse_status
(*json_parse_create_ntf)
(json_parse_u_data, json_type, json_parse_new_u_obj);
typedef json_parse_status
(*json_parse_add2array)
(json_parse_u_data, json_parse_u_obj a, json_parse_u_obj e);
typedef json_parse_status
(*json_parse_add2object)
(json_parse_u_data, json_parse_u_obj o, json_parse_u_obj l, json_parse_u_obj r);

typedef struct {
    json_parse_create_sn string_create;
    json_parse_create_sn number_create;
    json_parse_create_n integer_create;
    json_parse_create_ao array_create;
    json_parse_create_ao object_create;
    json_parse_create_ntf ntf_create;
    json_parse_add2array array_add;
    json_parse_add2object object_add;
    /* The data to be passed in to the above routines. */
    json_parse_u_data ud;
    /* The end-result of the parsing. */
    json_parse_u_obj parse_result;
    /* Holder for the flex scanner. */
    void * scanner;
    /* Buffer for reading strings. */
    buffer_t buffer;
    /* integer number holder. */
    int integer;
}
json_parse_object;

#endif

#include "json_parse.h"
#include "lexer.h"
#include "json_parse_grammar.tab.h"

const char * json_parse_status_messages[json_parse_n_statuses] = {
    "OK",
    "Out of memory",
    "The JSON is not grammatically correct",
    "There are stray characters in the JSON",
    "There is a badly-formed \\u Unicode escape in the JSON",
    "The JSON string is empty",
    "The JSON did not start with '{' or '['",
    "There is an unknown escape sequence in the JSON",
};

/* This declares the parsing function in
   "json_parse_grammar.tab.c". */

int json_parse_parse (const char ** json_ptr, json_parse_object * jpo);

/* With the reentrant parser, it is necessary to initialize the
   buffers which are in jpo->scanner. This also sets the value of
   yyextra to jpo. */

void json_parse_init (json_parse_object * jpo)
{
}

/* This is the main entry point of the routine. */

int json_parse (const char ** json, json_parse_object * jpo)
{
    int parser_status;
    /* Make sure there is no error message lingering. */
    jpo->buffer.status = json_parse_ok;
    /* Set the initial line number to one. */
    jpo->buffer.line = 1;
    parser_status = json_parse_parse (json, jpo);
    return parser_status;
}

void json_parse_free (json_parse_object * jpo)
{
    if (jpo->buffer.value) {
        free (jpo->buffer.value);
    }
}

/* This is the error handler required by yacc/bison. What it does is
   to correctly set the error status in the user's object. The client
   of this parser then decides what to do about the error. */

int json_parse_error (const char ** json_ptr, json_parse_object * jpo_x, const char * message)
{
    /*
    printf ("parse error %d.\n", jpo_x->buffer.status);
    */
    if (jpo_x->buffer.status == json_parse_ok)
	jpo_x->buffer.status = json_parse_grammar_fail;
    return 0;
}
