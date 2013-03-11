use strict;
use warnings;
use ExtUtils::MakeMaker;

# Module requires Perl 5.8 or above.

if ($] < 5.008) {
    exit;
}

my $pm = 'lib/JSON/Parse.pm';
my $pod = 'lib/JSON/Parse.pod';
my $repo = 'https://github.com/benkasminbullock/JSON-Parse';

WriteMakefile (
    NAME => 'JSON::Parse',
    VERSION_FROM => $pm,
    ABSTRACT_FROM => $pod,
    OBJECT => 'Parse.o json_parse.o lexer.o json_parse_grammar.tab.o unicode.o',
    LICENSE => 'perl',
    AUTHOR => 'Ben Bullock <bkb@cpan.org>',
    META_MERGE => {
        resources => {
            "bugtracker" => "$repo/issues",
	    "repository" => $repo,
        },
    },
);
