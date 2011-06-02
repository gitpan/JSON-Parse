/* Copyright (C) 2011 Ben Bullock <bkb@cpan.org>. */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include "lexer.h"
#include "json_parse.h"
#include "json_argo.h"

/* json_argo_t carries around any information we want to know about as
   we parse the text. */

typedef struct json_argo
{
    int verbose : 1;
    int utf8 : 1;
}
json_argo_t;

/* Debugging messages. */

#define MESSAGE(format, args...) {                      \
        if (data->verbose) {                            \
            printf ("%s:%d: ", __FILE__, __LINE__);     \
            printf (format, ##args);                    \
            printf ("\n");                              \
        }                                               \
    }

static json_parse_status
json_argo_string_create (void * vdata, const char * string,
                         json_parse_u_obj * out)
{
    SV * string_sv;
    json_argo_t * data = vdata;

    MESSAGE ("Creating a string from '%s'", string);
    string_sv = newSVpv (string, 0);
    if (data->utf8) {
        SvUTF8_on (string_sv);
    }
    * out = string_sv;
    return json_parse_ok;
}

static json_parse_status
json_argo_array_create (void * vdata, json_parse_u_obj * out)
{
    SV * array_sv;
    AV * array;
    json_argo_t * data = vdata;

    MESSAGE ("Creating an array");
    array = newAV ();
    array_sv = newRV_inc ((SV *) array);
    * out = array_sv;
    return json_parse_ok;
}

static json_parse_status
json_argo_hash_create (void * vdata, json_parse_u_obj * out)
{
    SV * hash_sv;
    HV * hash;
    json_argo_t * data = vdata;

    MESSAGE ("Creating a hash");
    hash = newHV ();
    hash_sv = newRV_inc ((SV *) hash);
    * out = hash_sv;
    return json_parse_ok;
}

#define TRUEVAL "true"

static json_parse_status
json_argo_ntf_create (void * vdata, json_type t, void ** out)
{
    json_argo_t * data = vdata;
    SV * ntf;
    MESSAGE ("NTF");
    switch (t) {
    case json_null:
    case json_false:
        ntf = & PL_sv_undef; 
        break;
    case json_true:
        ntf = newSVpv (TRUEVAL, strlen (TRUEVAL));
        break;
    default:
        croak ("Unknown type of JSON object %d", t);
    }
    * out = ntf;
    return json_parse_ok;
}

static json_parse_status
json_argo_array_push (void * vdata, void * varray, void * velement)
{
    SV * array_sv;
    json_argo_t * data = vdata;

    MESSAGE ("Pushing onto an array");
    if (! varray) {

    }
    array_sv = varray;
    if (SvROK (array_sv)) {
        SV * deref = SvRV (array_sv);
        if (SvTYPE (deref) == SVt_PVAV) {
            AV * array;
            SV * element;

            array = (AV*) deref;
            element = velement;
            av_push (array, element);
        }
        else {
            croak ("Error in add to array");
        }
    }
    else {
        croak ("Error in add to array");

    }
    return json_parse_ok;
}

static json_parse_status
json_argo_hash_add (void * vdata, void * vhash, void * vleft, void * vright)
{
    json_argo_t * data = vdata;
    SV * hash_ref = vhash;
    MESSAGE ("Adding a pair to hash");
    if (SvROK (hash_ref)) {
        HV * hash = (HV*) SvRV (hash_ref);
        if (SvTYPE (hash) == SVt_PVHV) {
            SV * left;
            SV * right;
            left = vleft;
            right = vright;
            hv_store_ent (hash, left, right, 0);
        }
        else {
            croak ("Error in add to hash");
        }
    }
    else {
        croak ("Error in add to hash");
    }
    return json_parse_ok;
}

/* Given JSON in a string, turn it in to Perl. */

SV *
json_argo_parse (json_parse_object * jpo, SV * json_sv)
{
    const char * json_bytes;
    json_parse_status status;
    int json_length;
    if (SvUTF8 (json_sv)) {
        json_argo_t * json_argo_data =  jpo->ud;
        json_argo_data->utf8 = 1;
        //Set this to 1 to get debuggering messages.
        json_argo_data->verbose = 0;
    }

    if (! SvOK (json_sv)) {
        return & PL_sv_undef;
    }
    json_bytes = SvPV (json_sv, json_length);
    status = json_parse (& json_bytes, jpo);
    if (status == json_parse_ok) {
        if (jpo->parse_result) {
            return jpo->parse_result;
        }
        else {
            return & PL_sv_undef;
        }        
    }
    else if (status == json_parse_no_input_fail) {
        return & PL_sv_undef;
    }
    else {
        croak ("Parsing failed: %s", json_parse_status_messages[status]);
        return & PL_sv_undef;
    }
}

SV *
json_argo_to_perl (SV * json_sv)
{
    json_argo_t json_argo_data = {0};
    json_parse_object jpo = {
        json_argo_string_create,
        json_argo_string_create,
        json_argo_array_create,
        json_argo_hash_create,
        json_argo_ntf_create,
        json_argo_array_push,
        json_argo_hash_add,
        & json_argo_data,
    };
    SV * r;

    //    json_argo_data.verbose = 1;
    json_parse_init (& jpo);

    r = json_argo_parse (& jpo, json_sv);
    json_parse_free (& jpo);
    return r;
}

/*             _ _     _       _   _             
   __   ____ _| (_) __| | __ _| |_(_) ___  _ __  
   \ \ / / _` | | |/ _` |/ _` | __| |/ _ \| '_ \ 
    \ V / (_| | | | (_| | (_| | |_| | (_) | | | |
     \_/ \__,_|_|_|\__,_|\__,_|\__|_|\___/|_| |_| */
                                              

/* The following callbacks just return. */

static json_parse_status
json_argo_string_create_valid (void * vdata, const char * string,
                                  json_parse_u_obj * out)
{
    return json_parse_ok;
}

static json_parse_status
json_argo_array_create_valid (void * vdata, json_parse_u_obj * out)
{
    return json_parse_ok;
}

static json_parse_status
json_argo_hash_create_valid (void * vdata, json_parse_u_obj * out)
{
    return json_parse_ok;
}

static json_parse_status
json_argo_ntf_create_valid (void * vdata, json_type t, void ** out)
{
    return json_parse_ok;
}

static json_parse_status
json_argo_array_push_valid (void * vdata, void * varray, void * velement)
{
    return json_parse_ok;
}

static json_parse_status
json_argo_hash_add_valid (void * vdata, void * vhash, void * vleft, void * vright)
{
    return json_parse_ok;
}

int
json_argo_valid_parse (json_parse_object * jpo, SV * json_sv)
{
    const char * json_bytes;
    json_parse_status status;
    int json_length;
    json_bytes = SvPV (json_sv, json_length);
    status = json_parse (& json_bytes, jpo);
    return ! status;
}

int
json_argo_valid_json (SV * json_sv)
{
    json_argo_t json_argo_data = {0};
    json_parse_object jpo = {
        json_argo_string_create_valid,
        json_argo_string_create_valid,
        json_argo_array_create_valid,
        json_argo_hash_create_valid,
        json_argo_ntf_create_valid,
        json_argo_array_push_valid,
        json_argo_hash_add_valid,
        & json_argo_data,
    };
    int r;

    json_parse_init (& jpo);

    r = json_argo_valid_parse (& jpo, json_sv);
    json_parse_free (& jpo);
    return r;
}
