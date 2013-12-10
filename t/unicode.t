use warnings;
use strict;
use Test::More;
use JSON::Parse qw/json_to_perl valid_json/;
use utf8;
binmode STDOUT, ":utf8";
my $builder = Test::More->builder;
binmode $builder->output,         ":utf8";
binmode $builder->failure_output, ":utf8";
binmode $builder->todo_output,    ":utf8";

my $m = '{"骪":"\u9aaa"}';
ok (valid_json ($m), "Valid good JSON");

my $ar = json_to_perl ($m);
ok (defined $ar, "Unicode \\uXXXX parsed");
is ($ar->{骪}, '骪', "Unicode \\uXXXX parsed correctly");
note ("keys = ", keys %$ar);

# Here the second unicode piece of the string is added to switch on
# the UTF-8 flag inside Perl and get the required invalidity. 

my $badunicode = '["\uD800", "バター"]';
ok (! valid_json ($badunicode), "$badunicode is invalid");

# This is what the documentation says will happen. However, I'm not
# sure this is correct or what the user expects to happen.

my $okunicode = '["\uD800"]';
ok (valid_json ($okunicode), "$okunicode is valid");

TODO: {
    local $TODO = '\u not implemented';
};
done_testing ();
