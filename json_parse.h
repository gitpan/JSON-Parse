/* This is a Cfunctions (version 0.27) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://cfunctions.sourceforge.net/'. */

/* This file was generated with:
`/home/ben/software/install/bin/cfunctions -in json_parse.c' */
#ifndef CFH_JSON_PARSE_H
#define CFH_JSON_PARSE_H

/* From `json_parse.c': */

#line 6 "json_parse.c"
typedef enum {
    json_parse_ok,
    json_parse_fail,
    json_parse_callback_fail,
    json_parse_memory_fail,
    json_parse_grammar_fail,
    json_parse_lex_fail,
    json_parse_unimplemented_fail,
    json_parse_unicode_fail,
    json_parse_no_input_fail,
    json_parse_bad_start_fail,
    json_parse_unknown_escape_fail,
    json_parse_n_statuses,
}
#line 22 "json_parse.c"
json_parse_status;
typedef enum {
    json_null,
    json_true,
    json_false
}
#line 29 "json_parse.c"
json_type;

#line 32 "json_parse.c"
typedef void * json_parse_u_obj;

#line 35 "json_parse.c"
typedef void * json_parse_u_data;

#line 38 "json_parse.c"
typedef json_parse_u_obj * json_parse_new_u_obj;

#line 42 "json_parse.c"
typedef json_parse_status (*json_parse_create_sn)
(json_parse_u_data, const char *, json_parse_new_u_obj);

#line 45 "json_parse.c"
typedef json_parse_status (*json_parse_create_ao)
(json_parse_u_data, json_parse_new_u_obj);

#line 48 "json_parse.c"
typedef json_parse_status (*json_parse_create_ntf)
(json_parse_u_data, json_type, json_parse_new_u_obj);

#line 51 "json_parse.c"
typedef json_parse_status (*json_parse_add2array)
(json_parse_u_data, json_parse_u_obj a, json_parse_u_obj e);

#line 54 "json_parse.c"
typedef json_parse_status (*json_parse_add2object)
(json_parse_u_data, json_parse_u_obj o, json_parse_u_obj l, json_parse_u_obj r);
typedef struct {
    json_parse_create_sn string_create;
    json_parse_create_sn number_create;
    json_parse_create_ao array_create;
    json_parse_create_ao object_create;
    json_parse_create_ntf ntf_create;
    json_parse_add2array array_add;
    json_parse_add2object object_add;
    
    json_parse_u_data ud;
    
    json_parse_u_obj parse_result;
    
    json_parse_status js;
    
    void * scanner;
    
    struct {
        size_t size;
        size_t length;
        char * chrs;
    } buffer;
} 

#line 80 "json_parse.c"
json_parse_object;

#line 100 "json_parse.c"
extern const char * json_parse_status_messages[];

#line 112 "json_parse.c"
void json_parse_init (json_parse_object * jpo );

#line 120 "json_parse.c"
int json_parse (json_parse_object * jpo );

#line 127 "json_parse.c"
void json_parse_free (json_parse_object * jpo );

#line 139 "json_parse.c"
int json_parse_error (json_parse_object * jpo_x , const char * message );

#endif /* CFH_JSON_PARSE_H */
