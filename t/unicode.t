use warnings;
use strict;
use Test::More;
use JSON::Parse qw/json_to_perl valid_json parse_json/;
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
ok (! valid_json ($okunicode), "$okunicode is valid");

my $surpair = '["\uD834\uDD1E"]';
my $spo;
eval {
    $spo = parse_json ($surpair);
};
ok (! $@, "parsed surrogate pairs");
is (ord ($spo->[0]), 0x1D11E, "g-clef surrogate pair");

no utf8;
# 蟹
my $kani = '["\u87f9", "蟹", "\u87f9猿"]';
my $p = parse_json ($kani);
ok (utf8::is_utf8 ($p->[0]), "kani upgraded regardless");
ok (! utf8::is_utf8 ($p->[1]), "input string not upgraded, even though it's UTF-8");
ok (utf8::is_utf8 ($p->[2]), "upgrade this too");
is (length ($p->[2]), 2, "length is two by magic");

# There is a small danger that the user could put non-UTF-8 bytes and
# then get a non-UTF-8 string upgraded into Perl "utf8", because we
# don't validate all the bytes in the input string (should do this).

TODO: {
    local $TODO = '\u surrogate pairs not implemented';
};
done_testing ();
