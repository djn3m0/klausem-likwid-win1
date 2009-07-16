#!/usr/bin/perl
use lib 'ext/perl';
use strict;
use warnings;
use Text::Markdown qw(markdown);

my $title = $ARGV[2];
my $html_file;
my $mkd_file;
my $f;
my $m;

$html_file = $ARGV[1];
open OUTFILE, "> $html_file";
$mkd_file =  $ARGV[0];

print OUTFILE <<END;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html><head><meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
<title>$title</title>
<link href="doxygen.css" rel="stylesheet" type="text/css">
</head><body>
END

die("Cannot find file $mkd_file") unless (-r $mkd_file);
open INFILE, "< $mkd_file" or die;
$f = join('',<INFILE>);
close INFILE or die;
$m = Text::Markdown->new;
print OUTFILE $m->markdown($f);

print OUTFILE <<END;
</body>
</html>
END

close OUTFILE;


