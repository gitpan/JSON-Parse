#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

/* TESTRANDOM should never be defined in the code released to
   CPAN. This is tested in "xt/testrandom-invalid.t". */

//#define TESTRANDOM

#ifdef TESTRANDOM
#include <setjmp.h>
#endif /* def TESTRANDOM */

/* Experimental. */

static SV * json_diagnostics;

/* All instances of JSON literals are pointed to the following. These
   are initialized in "BOOT" in "Json3.xs". */

static SV * json_true;
static SV * json_false;
static SV * json_null;

#include "unicode.h"
#include "unicode.c"
#include "json-common.c"
#define PERLING
#include "json-perl.c"
#undef PERLING
#define TOKENING
#include "json-perl.c"
#undef TOKENING
#include "json-perl.c"
#include "json-entry-points.c"
#ifdef TESTRANDOM
#include "json-random-test.c"
#endif /* def TESTRANDOM */
#include "json-whitespace.c"

typedef json_token_t * JSON__Tokenize;

MODULE=JSON::Parse PACKAGE=JSON::Parse

PROTOTYPES: DISABLE

BOOT:
{
	json_true = get_sv ("JSON::Parse::true", GV_ADD);
	sv_setiv (json_true, 1);
	SvREADONLY_on (json_true);
	json_false = get_sv ("JSON::Parse::false", GV_ADD);
	sv_setiv (json_false, 0);
	SvREADONLY_on (json_false);
	json_null = get_sv ("JSON::Parse::null", GV_ADD);
	SvREADONLY_on (json_null);
}

SV * parse_json (json)
	SV * json;
CODE:
	RETVAL = parse (json);
OUTPUT:
	RETVAL

void assert_valid_json (json)
	SV * json;
CODE:
	validate (json);

#ifdef TESTRANDOM

int random_json ()
CODE:
	RETVAL = random_json ();
OUTPUT:
	RETVAL

#endif /* def TESTRANDOM */

MODULE=JSON::Parse PACKAGE=JSON::Tokenize

JSON::Tokenize tokenize_json (json)
	SV * json;
CODE:
	RETVAL = tokenize (json);
OUTPUT:
	RETVAL

void DESTROY (tokens)
	json_token_t * tokens;
CODE:
	tokenize_free (tokens);

MODULE=JSON::Parse PACKAGE=JSON::Whitespace

SV * strip_whitespace (tokens, json)
	JSON::Tokenize tokens;
	SV * json;
CODE:
	RETVAL = strip_whitespace (tokens, json);
OUTPUT:
	RETVAL

