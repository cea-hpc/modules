#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  csv2err.pl
#
#        USAGE:  ./csv2err.pl [-hH] -i CSV-FILE [ -t TEMPDIR ]
#
#  DESCRIPTION:  Generate a Vim-quickfix compatible errorfile from a CSV-file 
#                produced by Devel::NYTProf.
#
#      OPTIONS:  ---
# REQUIREMENTS:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:  Dr. Fritz Mehner (fgm), mehner@fh-swf.de
#      COMPANY:  FH Südwestfalen, Iserlohn
#      VERSION:  1.0
#      CREATED:  13.02.2009 17:04:00
#     REVISION:  $Id: csv2err.pl,v 1.1 2009/02/19 19:21:01 mehner Exp $
#===============================================================================

use strict;
use warnings;

use Getopt::Std;
use File::Basename;

our($opt_H, $opt_h, $opt_i, $opt_t);
getopts('hHi:t:');                      	      # process command line arguments

if ( defined $opt_h ) {                         # process option -h
print <<EOF;
usage: $0 [-hH] -i CSV-FILE -o ERR-FILE
  -h       this message
  -H       hot spots only ( time, calls, and time/call are zero)
  -i       input file (CSV)
  -t       temp dir to keep the extracted source file and the quickfix file (optional)
EOF
  exit 0;
}

if ( ! defined $opt_i ) {
  die "\nusage: $0 [-hH] -i CSV-FILE -o ERR-FILE\n";
}

my	$tmpdir	= '';
if ( defined $opt_t ) {
	$tmpdir	= $opt_t.'/';
}

my  $csv_file_name = $opt_i;                    # input file name

open  my $csv, '<', $csv_file_name
  or die  "$0 : failed to open  input file '$csv_file_name' : $!\n";

my  $rawline;
my  $cookedline;
my  @linepart;
my  $sourcelinenumber = 0;
my  $sourceline;
my  $src_file_name		= $csv_file_name;
my  $err_file_name;

$src_file_name 	=~ s/-(block|line|sub)\.csv$//;
$src_file_name	= basename($src_file_name);    # remove path

$err_file_name	= $src_file_name;
$err_file_name	.= '.err';

open  my $err, '>', $tmpdir.$err_file_name
  or die  "$0 : failed to open  output file '$tmpdir.$err_file_name' : $!\n";

open  my $src, '>', $tmpdir.$src_file_name
	or die  "$0 : failed to open  output file '$tmpdir.$src_file_name' : $!\n";


#---------------------------------------------------------------------------
# filter lines
#  input format: <time>,<calls>,<time/call>,<source line> 
# output format: <filename>:<line>:<time>:<calls>:<time/call>: <source line>
#---------------------------------------------------------------------------
while ( $rawline = <$csv> ) {
  if ( $rawline =~ /^#/ ) {                     # comments produced by the profiler
    print {$err} $rawline;
  } else {                                      # timed source lines
    $sourcelinenumber++;
    @linepart   = split ( /,/, $rawline );
    $sourceline	= join( ',', @linepart[3..$#linepart] );
    $cookedline = $src_file_name.':'.$sourcelinenumber.':';
    $cookedline .= join( ':', @linepart[0..2] ).': ';
    $cookedline .= $sourceline;
    if ( defined $opt_H ) {                     # drop lines which were never called?
      if ( ($linepart[0]+$linepart[1]+$linepart[2] != 0 ) ) {
        print {$err} $cookedline;
      }
    } else {
      print {$err} $cookedline;
    }
      print {$src} $sourceline;
  }
}

close  $csv
  or warn "$0 : failed to close input file '$csv_file_name' : $!\n";

close  $err
  or warn "$0 : failed to close output file '$err_file_name' : $!\n";

close  $src
	or warn "$0 : failed to close output file '$src_file_name' : $!\n";

