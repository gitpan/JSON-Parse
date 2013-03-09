/* This is a Cfunctions (version 0.28) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from 'http://www.lemoda.net/cfunctions/'. */

/* This file was generated with:
'/home/ben/software/install/bin/cfunctions -in json_parse.c' */
#ifndef CFH_JSON_PARSE_H
#define CFH_JSON_PARSE_H

/* From 'json_parse.c': */

#line 6 "json_parse.c"
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
typedef void * json_parse_u_obj;
typedef void * json_parse_u_data;
typedef json_parse_u_obj * json_parse_new_u_obj;
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

#line 109 "json_parse.c"
extern const char * json_parse_status_messages[];

#line 121 "json_parse.c"
void json_parse_init (json_parse_object * jpo );

#line 127 "json_parse.c"
int json_parse (const char ** json , json_parse_object * jpo );

#line 138 "json_parse.c"
void json_parse_free (json_parse_object * jpo );

#line 149 "json_parse.c"
int json_parse_error (const char ** json_ptr , json_parse_object * jpo_x , const char * message );

#endif /* CFH_JSON_PARSE_H */
