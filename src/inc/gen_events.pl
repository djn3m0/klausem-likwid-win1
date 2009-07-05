#!/usr/bin/perl 

use strict;
use warnings;

my $key;
my $event_id;
my $umask;
my $num_events=0;

while (<>) {

    if (/(EVENT_[A-Z0-9_]*)[ ]*(0x[A-F0-9]+)/) {
        $event_id = $2;
    } elsif (/UMASK_([A-Z0-9_]*)[ ]*(0x[A-F0-9]+)/) {
        $key = $1;
        $umask = $2;
        $num_events++;

        print <<END;
,{\"$key\",
\t{$event_id,$umask}}
END
    }
}
print "};\n";

print "perfmon_hash_entry_t  arch_events[$num_events] = {\n";


