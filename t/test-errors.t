# This tests the production of error messages when presented with
# invalid input.

use JSON::Parse qw/json_to_perl valid_json/;
use warnings;
use strict;
use Test::More tests => 4;
eval {
    json_to_perl ("noquote");
};
ok ($@ =~ /stray characters/);
note ($@);

eval {
    json_to_perl ("{\"golf\":-01bajunga}");
};
ok ($@ =~ /not grammatically correct/);
note ($@);

eval {
    json_to_perl ("[\"\\u932P\"]");
};
ok ($@ =~ /Unicode.*decoding failed/);
note ($@);

eval {
    json_to_perl ("[\"\\M\"]");
};
ok ($@ =~ /unknown escape sequence/);
note ($@);

