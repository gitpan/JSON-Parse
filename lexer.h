/* This is a Cfunctions (version 0.27) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://cfunctions.sourceforge.net/'. */

/* This file was generated with:
`cfunctions -i -n -c lexer.c' */
#ifndef CFH_LEXER_H
#define CFH_LEXER_H

/* From `lexer.c': */

#line 118 "lexer.c"
typedef struct buffer {
    char * value;
    int characters;
    int allocated;
    int line;
    int  status;
} 

#line 126 "lexer.c"
buffer_t;

#line 209 "lexer.c"
int lexer (void * ignore , const char ** json_ptr , buffer_t * b );

#endif /* CFH_LEXER_H */
