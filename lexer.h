/* This is a Cfunctions (version 0.28) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from 'http://www.lemoda.net/cfunctions/'. */

/* This file was generated with:
'cfunctions -i -n -c lexer.c' */
#ifndef CFH_LEXER_H
#define CFH_LEXER_H

/* From 'lexer.c': */

#line 130 "lexer.c"
typedef struct buffer {
    char * value;
    int characters;
    int allocated;
    int line;
    int /* json_parse_status */ status;
} buffer_t;

#line 223 "lexer.c"
int lexer (void * ignore , const char ** json_ptr , buffer_t * b );

#endif /* CFH_LEXER_H */
