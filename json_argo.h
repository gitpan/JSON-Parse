/* This is a Cfunctions (version 0.27) generated header file.
   Cfunctions is a free program for extracting headers from C files.
   Get Cfunctions from `http://cfunctions.sourceforge.net/'. */

/* This file was generated with:
`cfunctions -i -n -c json_argo.c' */
#ifndef CFH_JSON_ARGO_H
#define CFH_JSON_ARGO_H

/* From `json_argo.c': */

#line 161 "json_argo.c"
SV * json_argo_parse (json_parse_object * jpo , SV * json_sv );

#line 196 "json_argo.c"
SV * json_argo_to_perl (SV * json_sv );

#line 266 "json_argo.c"
int json_argo_valid_parse (json_parse_object * jpo , SV * json_sv );

#line 277 "json_argo.c"
int json_argo_valid_json (SV * json_sv );

#endif /* CFH_JSON_ARGO_H */
