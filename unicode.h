/* This is a Cfunctions (version 0.27) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://cfunctions.sourceforge.net/'. */

/* This file was generated with:
`cfunctions -g unicode -n unicode.c unicode-character-class.c' */
#ifndef CFH_UNICODE_H
#define CFH_UNICODE_H

/* From `unicode.c': */

#line 6 "unicode.c"
#define UTF8_MAX_LENGTH 4

#line 17 "unicode.c"
int utf8_to_ucs2 (const unsigned char * input , const unsigned char ** end_ptr );

#line 54 "unicode.c"
int ucs2_to_utf8 (int ucs2 , unsigned char * utf8 );

#line 79 "unicode.c"
int unicode_chars_to_bytes (const unsigned char * utf8 , int n_chars );

#line 98 "unicode.c"
int unicode_count_chars (const unsigned char * utf8 );

#line 122 "unicode.c"

#ifdef TEST
void print_bytes (const unsigned char * bytes );

#line 132 "unicode.c"
void test_ucs2_to_utf8 (const unsigned char * input );

#endif /* def TEST */
/* From `unicode-character-class.c': */

#line 10 "unicode-character-class.c"
int ucs2_is_kana (int ucs );

#line 20 "unicode-character-class.c"
int utf8_is_kana_chars (const unsigned char * utf8 , int len );

#endif /* CFH_UNICODE_H */
