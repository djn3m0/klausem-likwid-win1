#!/usr/bin/perl

use lib 'util';
use strict;
use warnings;
use lib './perl';
use File::Copy;
use Cwd 'abs_path';
use Data::Dumper;
use Template;

my @Testcases;
my $name;
my $streams;
my $type;
my $flops;
my $bytes;
my $prolog='';
my $loop='';
my $increment;
my $isLoop=0;

my $BenchRoot = $ARGV[0];
my $OutputDirectory = $ARGV[1];
my $TemplateRoot = $ARGV[2];
#my $ROOT = abs_path('./bench');
my $DEBUG = 0;

#chdir ("$BenchRoot") or die "Cannot change in  : $!\n";
opendir (DIR, "./$BenchRoot") or die "Cannot open bench directory: $!\n";
my $tpl = Template->new({
        INCLUDE_PATH => ["$TemplateRoot"]
        });

while (defined(my $file = readdir(DIR))) {
    if ($file !~ /^\./) {
        print "SCANNING $file\n" if ($DEBUG);

        $file =~ /([A-Za-z_0-9]+)\.ptt/;
        $name = $1;

        $isLoop = 0;
        $prolog='';
        $loop='';
        open FILE, "<$BenchRoot/$file";
        while (<FILE>) {
            my $line = $_;

            if($line =~ /STREAMS[ ]+([0-9]+)/) {
                $streams = $1;
            } elsif ($line =~ /TYPE[ ]+(SINGLE|DOUBLE)/) {
                $type = $1;
            } elsif ($line =~ /FLOPS[ ]+([0-9]+)/) {
                $flops = $1;
            } elsif ($line =~ /BYTES[ ]+([0-9]+)/) {
                $bytes = $1;
            } elsif ($line =~ /LOOP[ ]+([0-9]+)/) {
                $increment = $1;
                $isLoop = 1;
            } else {
                if ($isLoop) {
                    $loop .= $line;
                } else {
                    $prolog .= $line;
                }
            }
        }
        close FILE;

        if ($streams > 5) {
            my $arg = 7;
            foreach my $stream ( 5 .. $streams ) {
                $prolog .= "mov STR$stream, ARG$arg\n";
                $arg++;
            }
        }

        my $Vars;
        $Vars->{name} = $name;
        $Vars->{prolog} = $prolog;
        $Vars->{increment} = $increment;
        $Vars->{loop} = $loop;

#print Dumper($Vars);

        $tpl->process('bench.tt', $Vars, "$OutputDirectory/$name.pas");
        push(@Testcases,{name    => $name,
                         streams => $streams,
                         type    => $type,
                         stride  => $increment,
                         flops   => $flops, 
                         bytes   => $bytes});
    }
}
#print Dumper(@Testcases);

my $Vars;
$Vars->{Testcases} = \@Testcases;
$Vars->{numKernels} = $#Testcases+1;
$Vars->{allTests} = join('\n',map {$_->{name}} @Testcases);
$tpl->process('testcases.tt', $Vars, "$OutputDirectory/testcases.h");


