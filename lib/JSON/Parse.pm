package JSON::Parse;
require Exporter;
@ISA = qw(Exporter);
@EXPORT_OK = qw/json_to_perl valid_json/;
use warnings;
use strict;
our $VERSION = 0.07;
use XSLoader;
XSLoader::load 'JSON::Parse', $VERSION;

1;

__END__

=pod

=head1 NAME

JSON::Parse - Convert JSON into a Perl variable

=head1 SYNOPSIS

    use JSON::Parse 'json_to_perl';
    my $json = '["golden", "fleece"]';
    my $perl = json_to_perl ($json);
    # Same effect as $perl = ['golden', 'fleece'];

Convert JSON (JavaScript Object Notation) into Perl.

=head1 FUNCTIONS

=head2 valid_json

    use JSON::Parse 'valid_json';
    if (valid_json ($json)) {
        # do something
    }

This function returns I<1> if its argument is valid JSON and I<0> if
its argument is not valid JSON.

L</valid_json> does not create any Perl data structures, and it runs
about two or three times faster than L</json_to_perl>.

=head2 json_to_perl

    use JSON::Parse 'json_to_perl';
    my $perl = json_to_perl ('{"x":1, "y":2}');

This function converts JSON into a Perl structure. 

=head3 Return value

If the first argument does not contain a valid JSON text, the return
value is the undefined value.

If the first argument contains a valid JSON text, the return value is
either a hash reference or an array reference, depending on whether
the input JSON text is a serialized object or a serialized array.

=head3 Mapping from JSON to Perl

JSON elements are mapped to Perl as follows:

=over

=item JSON numbers

JSON numbers are mapped to Perl scalars. The JSON number is inserted
into Perl as a string. No conversion from the character string to a
numerical value is done.

=item JSON strings

JSON strings are mapped to Perl scalars as strings. 

JSON escape characters (see page five of L</RFC 4267>) are mapped to
the equivalent ASCII character before they are passed to Perl. Unicode
escape characters of the form \uXXXX (see page three of L</RFC 4267>)
are mapped to UTF-8 octets. This is done regardless of what input
encoding might be used.

If the input to L</json_to_perl> is marked as Unicode characters, the
output strings will be marked as Unicode characters. If the input is
not marked as Unicode characters, the output strings will not be
marked as Unicode characters.

=item JSON arrays

JSON arrays are mapped to Perl array references, with elements in the
same order as they appear in the JSON.

Thus

    my $p = json_to_perl ('["monday", "tuesday", "wednesday"]');

has the same result as a Perl declaration of the form

    my $p = [ 'monday', 'tuesday', 'wednesday' ];

=item JSON objects

JSON objects are mapped to Perl hashes (associative arrays). The
members of the object are mapped to pairs of key and value in the Perl
hash. The string part of each object member is mapped to the key of
the Perl hash. The value part of each member is mapped to the value of
the Perl hash.

Thus

    my $j = <<EOF;
    {"monday":["blue", "black"],
     "tuesday":["grey", "heart attack"],
     "friday":"Gotta get down on Friday"}
    EOF

    my $p = json_to_perl ($j);

has the same result as a Perl declaration of the form

    my $p = {
        monday => ['blue', 'black'],
        tuesday => ['grey', 'heart attack'],
        friday => 'Gotta get down on Friday',
    };

=item null

The JSON null literal is mapped to the undefined value.

=item true

The JSON true literal is mapped to a Perl string with the value 'true'.

=item false

The JSON false literal is mapped to the undefined value.

=back

=head1 RESTRICTIONS

This module imposes the following restrictions on its input

=over

=item JSON only

JSON::Parse will only parse JSON which meets the criteria of L</RFC
4267>.

=item No incremental parsing

JSON::Parse does not do incremental parsing. JSON::Parse will only parse
a fully-formed JSON string including opening and closing brackets.

=item UTF-8 only

JSON::Parse is only designed to parse bytes which are in the UTF-8
format. This is regardless of whether Perl thinks that the text is in
UTF-8 format (whether Perl has set its utf8 flag for the string). If
the input is in a different encoding than UTF-8-encoded Unicode, it is
suggested that the user converts the input before handing it to this
module. The Perl language already provides very extensive facilities
for transforming encodings in the L<Encode> module, so there seems
little point in this module duplicating them.

=back

=head1 BUGS

This is a preliminary version. The following deficiencies are
known. These may be resolved in a later version.

=over

=item False == null == undefined value

At the moment, both of "false" and "null" in JSON are mapped to the
undefined value. "true" is mapped to the string "true".

=item Numbers not checked

The author of this module has no idea whether JSON floating point
numbers are invariably understood by Perl (see L</JSON numbers>
above). Most integer and floating point numbers encountered should be
OK.

=item Number parsing may be slow

The number parsing is handled using the Bison grammar in
F<json_parse_grammar.y> so it might be slower than it could be. This
is a speculation which has not been tested in practice.

=item Line numbers are off by one

The line numbers in the error messages are off by one (line 1 is line
0).

=back

=head1 DIAGNOSTICS

The function L</valid_json> does not produce error messages. The
function L</json_to_perl> may produce the following fatal error
messages:

=over

=item unknown failure

=item a callback routine failed

=item out of memory

=item parser failed (this JSON is not grammatically correct)

=item lexer failed (there are stray characters in the input)

=item unimplemented feature of JSON encountered in input

=item Unicode \uXXXX decoding failed

=item input was empty

=item the text did not start with { or [ as it should have

=item met an unknown escape sequence (backslash \ + character)

=item there was an unparseable number in the input

=back

Error messages are accompanied by the line number and the byte number
of the input which caused the problem.

The above error messages are in the file F<json_parse.c> in the top
level of the distribution. The "callback routine failed" and "out of
memory" errors are unlikely to occur in normal usage.

Parsing errors are fatal, so to continue after an error occurs, put
the parsing into an C<eval> block:

    my $p;                       
    eval {                       
        $p = json_to_perl ($j);  
    };                           
    if ($@) {                    
        # handle error           
    }

=head1 SEE ALSO

=over

=item RFC 4627

JSON is specified in RFC (Request For Comments, a kind of internet
standards document) 4627. See, for example,
L<http://www.ietf.org/rfc/rfc4627.txt>.

=back

=head1 HOW IT WORKS

JSON::Parse is based on a parser written in C. The C parser makes use
of the utility Bison. The C parser is reentrant, in other words
thread-safe.

=head2 Performance

The module author's tests indicate that L</json_to_perl> extracts a
structure from a string several times faster than using Perl's C<eval
$string>.

Compared to other popular modules on CPAN, the speed of
L</json_to_perl>, according to the module author's tests is about a
hundred times faster than L<JSON::PP> and roughly comparable to
L<JSON::XS>. 


=head2 History

This module began as a project to write a JSON parser in C, based on
callbacks to user-defined routines rather than allocating memory, and
using the minimum amount of source code. Originally it was written
with the goal of fitting into 300 lines of C/Flex/Bison source code,
including comments, and without obfuscation. The original project has
been modified in order to make the parser reentrant and has grown over
the intended number of lines of code.

The module was previously known as JSON::Argo. With version 0.05, the
name was changed to JSON::Parse because, unknown to this module's
author, there was already a JSON parser called Argo written in Java.

Version 0.06 moved from using a Flex lexer to a C lexer in order to
increase the speed of the module, to be competitive with L<JSON::XS>.

=head1 EXPORTS

The module exports nothing by default. Functions L</json_to_perl> and
L</valid_json> can be exported on request.

=head1 SOURCES

One of the C files in the distribution is the output of the "bison"
program. The original input to the "bison" program may be found in the
directory "src" of the distribution. The user does not need to have
installed "bison" to build this module. The bison input is included in
the distribution to comply with the requirements of the GNU General
Public License.

=head1 AUTHOR

Ben Bullock, <bkb@cpan.org>

=head1 LICENSE

JSON::Parse can be used, copied, modified and redistributed under the
same terms as Perl itself.

=cut

