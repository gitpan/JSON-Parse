use strict;
use warnings;
use ExtUtils::MakeMaker;

# Module requires Perl 5.8 or above.

if ($] < 5.008) {
    exit;
}

my $pm = 'lib/JSON/Parse.pm';

WriteMakefile (
    NAME => 'JSON::Parse',
    VERSION_FROM => $pm,
    ABSTRACT_FROM => $pm,
    OBJECT => 'Parse.o json_argo.o json_parse.o lexer.o json_parse_grammar.tab.o unicode.o',
    LICENSE => 'perl',
    AUTHOR => 'Ben Bullock <bkb@cpan.org>',
);
