package JSON::Parse;
require Exporter;
@ISA = qw(Exporter);
@EXPORT_OK = qw/json_to_perl valid_json json_file_to_perl/;
use warnings;
use strict;
our $VERSION = '0.17';
use XSLoader;
XSLoader::load 'JSON::Parse', $VERSION;
use Carp;

sub json_file_to_perl
{
    my ($file_name) = @_;
    my $json = '';
    open my $in, "<:encoding(utf8)", $file_name or croak $!;
    while (<$in>) {
	$json .= $_;
    }
    close $in or croak $!;
    return json_to_perl ($json);
}

1;

