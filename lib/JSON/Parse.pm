=encoding UTF-8

=head1 NAME

Json3 - abstract here.

=head1 SYNOPSIS

    use Json3;

=head1 DESCRIPTION

=head1 FUNCTIONS

=cut
package JSON::Parse;
require Exporter;
@ISA = qw(Exporter);
@EXPORT_OK = qw/parse_json/;
%EXPORT_TAGS = (
    all => \@EXPORT_OK,
);
use warnings;
use strict;
use Carp;
our $VERSION = '0.19_02';
require XSLoader;
XSLoader::load (__PACKAGE__, $VERSION);

our $true = 1;
our $false = 0;
our $null;

1;
