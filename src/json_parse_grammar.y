/* Copyright (c) 2010-2011 Ben Bullock (bkb@cpan.org). */

/* JSON parser */

%{

#include <stdlib.h>

/* The option below gives this the prefix "json_parse_" but the lexer
   has the prefix "json_parse_lex_". The following define sends the
   compiler to the correct yylex function. */

#define json_parse_lex json_parse_lex_lex

#include "json_parse.h"
#include "json_parse_grammar.tab.h"
#include "json_parse_lexer.h"

#define CALL(f) json_parse_status js; js = (*jpo_x->f)
#define CALL2(f) js = (*jpo_x->f)

/* Check the return value from a call to a */

#define CHK if (js != json_parse_ok) {          \
        jpo_x->js = js;                         \
        return 1;                               \
    }

/* UD is the user data part of "json_parse_object". */

#define UD jpo_x->ud

/* Debugging stuff. */

#if 0
#define MESSAGE(x, args...) {                                   \
        printf ("%s:%d: ", __FILE__, __LINE__ );                \
        printf ("status: %d ", json_parse_global_jpo->js);      \
        printf (x, ## args);                                    \
        printf ("\n");                                          \
    }
#else
#define MESSAGE(x, args...)
#endif

#define FAIL(status) {                                                  \
        MESSAGE("%s", #status);                                         \
        /* Check that there is not already an error message */          \
        if (jpo_x->js == json_parse_ok) {                               \
            jpo_x->js = json_parse_ ## status ## _fail;                 \
        }                                                               \
        return jpo_x->js;                                               \
    }

/* The reentrant lexer needs to use allocated memory to hold its
information. The "scanner" member of "jpo_x" is that information. */

#define scanner jpo_x->scanner

%}

/* Reentrant parser. */

%pure-parser

/* Arguments passed in to this function. */

%parse-param {json_parse_object * jpo_x}

/* Arguments passed out to the lexer. */

%lex-param {void * scanner}

/* This holds one "word" in the grammar. */

%union {
    json_parse_u_obj	  uo;
    json_parse_u_obj 	  uo_pair[2];
    const char *  chrs;
}

%name-prefix "json_parse_"

%token <chrs> chars
%token <chrs> number
%token true
%token false
%token null
%token eof
%type <uo> json
%type <uo> object
%type <uo> array
%type <uo> _pairs
%type <uo> _value
%type <uo> string
%type <uo> _list
%type <uo_pair> _pair

%%

json:	object eof              { MESSAGE ("json=object");
                                  jpo_x->parse_result = $$;
                                  return jpo_x->js; }
	| array eof  		{ MESSAGE ("json=array");
                                  jpo_x->parse_result = $$;
                                  return jpo_x->js; }
          /* Error handlers */
        | eof                   { FAIL (no_input); }
        | chars                 { FAIL (bad_start); }
        | error                 { FAIL (grammar); }

object: '{' _pairs '}'		{ $$ = $2; }

_pairs:	/* empty */		{ CALL(object_create)(UD, & $$); CHK }
	| _pair	 		{ CALL(object_create)(UD, & $$); CHK
	  			  CALL2(object_add)(UD, $$, $1[0], $1[1]); CHK }
	| _pairs ',' _pair	{ CALL(object_add)(UD, $1, $3[0], $3[1]); CHK 
                                  $$ = $1; }

_pair:	string ':' _value	{ $$[0] = $1; $$[1] = $3; }

string: chars                   { CALL(string_create)(UD, $1, & $$); CHK }

array:	'[' _list ']'		{ $$ = $2; }

_list:	/* empty */		{ CALL(array_create)(UD, & $$); CHK }
	| _value		{ CALL(array_create)(UD, & $$); CHK 
	  			  CALL2(array_add)(UD, $$, $1); CHK }
	| _list ',' _value	{ CALL(array_add)(UD, $1, $3); CHK; $$ = $1; }

_value:	chars	    		{ CALL(string_create)(UD, $1, & $$); CHK }
	| number	    	{ CALL(number_create)(UD, $1, & $$); CHK }
	| object
	| array
	| true			{ CALL(ntf_create)(UD, json_true, & $$); CHK }
	| false			{ CALL(ntf_create)(UD, json_false, & $$); CHK }
	| null			{ CALL(ntf_create)(UD, json_null, & $$); CHK }

%%

/*
   Local variables:
   mode: text
   End:
*/

