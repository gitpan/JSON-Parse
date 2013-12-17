typedef enum {
    json_error_invalid,
    json_error_unexpected_character,
    json_error_unexpected_end_of_input,
    json_error_leading_zero,
    json_error_second_half_of_surrogate_pair_missing,
    json_error_not_surrogate_pair,
    json_error_empty_input,
    json_error_overflow
}
json_error_t;

const char * json_errors[json_error_overflow] = {
    "Invalid",
    "Unexpected character '%c'",
    "Unexpected end of input",
    "Leading zero",
    "Second half of surrogate pair missing",
    "Not surrogate pair",
    "Empty input",
};
enum expectation {
    xwhitespace,
    xcomma,
    xvalue_separator,
    xobject_end,
    xarray_end,
    xhexadecimal_character,
    xstring_start,
    xunicode_escape,
    xdigit,
    xdot,
    xminus,
    xplus,
    xarrayobjectstart,
    xstringchar,
    xliteral,
    xescape,
    n_expectations
};
#define XWHITESPACE (1<<xwhitespace)
#define XCOMMA (1<<xcomma)
#define XVALUE_SEPARATOR (1<<xvalue_separator)
#define XOBJECT_END (1<<xobject_end)
#define XARRAY_END (1<<xarray_end)
#define XHEXADECIMAL_CHARACTER (1<<xhexadecimal_character)
#define XSTRING_START (1<<xstring_start)
#define XUNICODE_ESCAPE (1<<xunicode_escape)
#define XDIGIT (1<<xdigit)
#define XDOT (1<<xdot)
#define XMINUS (1<<xminus)
#define XPLUS (1<<xplus)
#define XARRAYOBJECTSTART (1<<xarrayobjectstart)
#define XSTRINGCHAR (1<<xstringchar)
#define XLITERAL (1<<xliteral)
#define XESCAPE (1<<xescape)
char * input_expectation[n_expectations] = {
"whitespace: '\\n', '\\r', '\\t', ' '"
"comma: ','"
"value separator: ':'"
"end of object: '}'"
"end of array: ']'"
"hexadecimal character: '0-9', 'a-f', 'A-F'"
"start of string: '\"'"
"unicode escape \\uXXXX"
"digit: '0-9'"
"dot: '.'"
"minus: '-'"
"plus: '+'"
"start of an array, or object,: '{', '['"
"an ASCII or UTF-8 character excluding '\"'"
"literal: 'true', 'false', or 'null'"
"escape: '\\', '/', '\"', 'b', 'f', 'n', 'r', 't', 'u'"
};
