#!/usr/bin/perl 

use strict;
use warnings;

my $key;
my $eventId;
my $limit;
my $umask;
my $num_events=0;
my @events = ();

while (<>) {

    if (/(EVENT_[A-Z0-9_]*)[ ]+(0x[A-F0-9]+)[ ]+([A-Z0-9|]+)/) {
        $eventId = $2;
        $limit = $3;
    } elsif (/UMASK_([A-Z0-9_]*)[ ]*(0x[A-F0-9]+)/) {
        $key = $1;
        $umask = $2;
        push(@events,{name=>$key, limit=>$limit, eventId=>$eventId, mask=>$umask});
        $num_events++;
    }
}

open RESULTFILE,">out.h";
print RESULTFILE "#define NUM_ARCH_EVENTS_CORE2 $num_events\n\n";
print RESULTFILE "static PerfmonEvent  arch_events[NUM_ARCH_EVENTS_CORE2] = {\n";

foreach my $event (@events) {

        print RESULTFILE <<END;
,{\"$event->{name}\",
  \"$event->{limit}\", 
   $event->{eventId},$event->{mask}}
END
}

print  RESULTFILE "};\n";
close RESULTFILE;

