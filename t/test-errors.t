use JSON::Parse qw/json_to_perl valid_json/;
use warnings;
use strict;
use Test::More tests => 4;
eval {
    json_to_perl ("noquote");
};
ok ($@ =~ /stray characters/);
#print "1: $@\n";
eval {
    json_to_perl ("{-bajunga}");
};
ok ($@ =~ /unparseable number/);
#print "2: $@\n";

eval {
    json_to_perl ("[\"\\u932P\"]");
};
ok ($@ =~ /Unicode.*decoding failed/);
#print "3: $@\n";

eval {
    json_to_perl ("[\"\\M\"]");
};
ok ($@ =~ /unknown escape sequence/);
#print "4: $@\n";
