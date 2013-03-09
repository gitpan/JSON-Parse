package JSON::Parse;
require Exporter;
@ISA = qw(Exporter);
@EXPORT_OK = qw/json_to_perl valid_json json_file_to_perl/;
use warnings;
use strict;
our $VERSION = '0.16';
use XSLoader;
XSLoader::load 'JSON::Parse', $VERSION;

1;

