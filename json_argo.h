/* This is a Cfunctions (version 0.28) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from 'http://www.lemoda.net/cfunctions/'. */

/* This file was generated with:
'cfunctions -i -n -c json_argo.c' */
#ifndef CFH_JSON_ARGO_H
#define CFH_JSON_ARGO_H

/* From 'json_argo.c': */

#line 160 "json_argo.c"
SV * json_argo_parse (json_parse_object * jpo , SV * json_sv );

#line 199 "json_argo.c"
SV * json_argo_to_perl (SV * json_sv );

#line 269 "json_argo.c"
int json_argo_valid_parse (json_parse_object * jpo , SV * json_sv );

#line 280 "json_argo.c"
int json_argo_valid_json (SV * json_sv );

#endif /* CFH_JSON_ARGO_H */
