# This tests the production of error messages when presented with
# invalid input.

use JSON::Parse qw/json_to_perl valid_json/;
use warnings;
use strict;
use Test::More tests => 5;
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
    json_to_perl ('["\\u932P"]');
};
ok ($@ =~ /badly-formed \\u Unicode/);
note ($@);

eval {
    json_to_perl ('["\M"]');
};
ok ($@ =~ /unknown escape sequence/, "Test on bad escape sequence");
note ($@);

eval {
    json_to_perl (chr (0xFF));
};
ok ($@ =~ /0xFF/, "Test error message with unprintable character");
note ($@);

