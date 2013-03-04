#!/usr/bin/perl
#===================================================================================
#
#         FILE:  pmdesc3
#
#     SYNOPSIS:  Find versions and descriptions of installed perl Modules and PODs
#
#  DESCRIPTION:  See POD below.
#
#      CREATED:  15.06.2004 22:12:41 CEST
#     REVISION:  $Id: pmdesc3.pl,v 1.4 2007/08/10 16:02:32 mehner Exp $
#         TODO:  Replace UNIX sort pipe.
#                 
#===================================================================================

package pmdesc3;

require 5.6.1;

use strict;
use warnings;
use Carp;
use ExtUtils::MakeMaker;
use File::Find           qw(find);
use Getopt::Std          qw(getopts);
#use version;
our $VERSION        = qw(1.2.3);  # update POD at the end of this file

my  $MaxDescLength  = 150;        # Maximum length for the description field:
                                  # prevents slurping in big amount of faulty docs.

my  $rgx_version  = q/\Av?\d+(\.\w+)*\Z/; # regex for module versions 

#===  FUNCTION  ====================================================================
#         NAME:  usage
#===================================================================================
sub usage {
  my  $searchdirs = " "x12;
  $searchdirs .= join( "\n"." "x12, sort { length $b <=> length $a } @INC ) . "\n";
  print <<EOT;
Usage:   pmdesc3.pl [-h] [-s] [-t ddd] [-v dd] [--] [dir [dir [dir [...]]]]
Options:  -h         print this message
          -s         sort output (not under Windows)
          -t ddd     name column has width ddd (1-3 digits); default 36
          -v  dd     version column has width ddd (1-3 digits); default 10
          If no directories given, searches:
$searchdirs
EOT
  exit;
}

#===  FUNCTION  ====================================================================
#         NAME:  get_module_name
#===================================================================================
sub get_module_name {
  my ($path, $relative_to) = @_;

  local $_ = $path;
  s!\A\Q$relative_to\E/?!!;
  s! \.p(?:m|od) \z!!x;
  s!/!::!g;

  return $_;
}

#===  FUNCTION  ====================================================================
#         NAME:  get_module_description
#===================================================================================
sub get_module_description 
{
  my  $desc;
  my  ($INFILE_file_name) = @_;                 # input file name

  undef $/;                                     # undefine input record separator

  open  my $INFILE, '<', $INFILE_file_name
      or die  "$0 : failed to open  input file '$INFILE_file_name' : $!\n";

  my  $file = <$INFILE>;                        # slurp mode

  close  $INFILE
      or warn "$0 : failed to close input file '$INFILE_file_name' : $!\n";

  $file =~  s/\cM\cJ/\cJ/g;                     # remove DOS line ends 
  $file =~  m/\A=head1\s+NAME(.*?)\n=\w+/s;     # file starts with '=head1' (PODs)
  $desc = $1;

  if ( ! defined $desc  )
  {
    $file =~  m/\n=head1\s+NAME(.*?)\n=\w+/s;   # '=head1' is embedded
    $desc = $1;
  }

  if ( ! defined $desc  )
  {
    $file =~  m/\n=head1\s+DESCRIPTION(.*?)\n=\w+/s; # '=head1' is embedded
    $desc = $1;
  }

  if ( defined $desc )
  {
    $desc =~ s/B<([^>]+)>/$1/gs;                # remove bold markup
    $desc =~ s/C<([^>]+)>/'$1'/gs;              # single quotes to indicate literal
    $desc =~ s/E<lt>/</gs;                      # replace markup for <
    $desc =~ s/E<gt>/>/gs;                      # replace markup for >
    $desc =~ s/F<([^>]+)>/$1/gs;                # remove filename markup
    $desc =~ s/I<([^>]+)>/$1/gs;                # remove italic markup
    $desc =~ s/L<([^>]+)>/$1/gs;                # remove link markup
    $desc =~ s/X<([^>]+)>//gs;                  # remove index markup
    $desc =~ s/Z<>//gs;                         # remove zero-width character
    $desc =~ s/S<([^>]+)>/$1/gs;                # remove markup for nonbreaking spaces

    $desc =~ s/\A[ \t\n]*//s;                   # remove leading whitespaces
    $desc =~ s/\n\s+\n/\n\n/sg;                 # make true empty lines
    $desc =~ s/\n\n.*$//s;                      # discard all trailing paragraphs
    $desc =~ s/\A.*?\s+-+\s+//s;                # discard leading module name
    $desc =~ s/\n/ /sg;                         # join lines
    $desc =~ s/\s+/ /g;                         # squeeze whitespaces
    $desc =~ s/\s*$//g;                         # remove trailing whitespaces
    $desc =  substr $desc, 0, $MaxDescLength;   # limited length
  }
  return $desc;
}

#===  FUNCTION  ====================================================================
#         NAME:  get_module_version
#===================================================================================
sub get_module_version {
  local $_;                                     # MM->parse_version is naughty
  my $vers_code = MM->parse_version($File::Find::name) || '';
  $vers_code = undef unless $vers_code =~ /$rgx_version/;
  return $vers_code;
}

#===  FUNCTION  ====================================================================
#         NAME:  MAIN
#===================================================================================

my %visited;

$|++;

#---------------------------------------------------------------------------
#  process options and command line arguments
#---------------------------------------------------------------------------
my  %options;

getopts("hst:v:", \%options)         or $options{h}=1;

my  @args = @ARGV;
@ARGV = @INC unless @ARGV;

usage() if $options{h};                               #  option -h  :  usage

#---------------------------------------------------------------------------
#  option -t : width of the module name column
#---------------------------------------------------------------------------
usage() if $options{t} && $options{t}!~/^\d{1,3}$/;   # width 1-3 digits

$options{t} = "36" unless $options{t};

#---------------------------------------------------------------------------
#  option -v : width of the version column
#---------------------------------------------------------------------------
usage() if $options{v} && $options{v}!~/^\d{1,2}$/;   # width 1-2 digits

$options{v} = "10" unless $options{v};

#---------------------------------------------------------------------------
#  option -s  :  install an output filter to sort the module list
#---------------------------------------------------------------------------
if ($options{s}) {
    usage() if $^O eq "MSWin32";
    if ( open(ME, "-|") ) {
        $/ = "";
        while ( <ME> ) {
            chomp;
            print join("\n", sort split /\n/), "\n";
        }
        exit;
    }
}

#---------------------------------------------------------------------------
#  process 
#---------------------------------------------------------------------------
# 
# :WARNING:15.04.2005:Mn: under Windows descending into subdirs will be
# suppressed by the the preprocessing part of the following call to find
# :TODO:16.04.2005:Mn: remove code doubling
# 
if ( $^O ne "MSWin32" ) {                       # ----- UNIX, Linux, ...

    for my $inc_dir (sort { length $b <=> length $a } @ARGV) {
        find({
                wanted => sub {
                    return unless /\.p(?:m|od)\z/ && -f;

                    #---------------------------------------------------------------------
                    #  return from function if there exists a pod-file for this module
                    #---------------------------------------------------------------------
                    my $pod = $_;
                    my $pm  = $_;
                    if ( m/\.pm\z/ )
                    {
                        $pod  =~ s/\.pm\z/\.pod/; 
                        return if -f $pod;
                    }

                    my $module  = get_module_name($File::Find::name, $inc_dir);
                    my $version;
                    if ( /\.pod\z/ )
                    {
                        $pm =~ s/\.pod\z/\.pm/; 
                        #-------------------------------------------------------------------
                        #  try to find the version from the pm-file
                        #-------------------------------------------------------------------
                        if ( -f $pm )
                        {
                            local $_;
                            $version = MM->parse_version($pm) || "";
                            $version = undef unless $version =~ /$rgx_version/;
                        }
                    }
                    else
                    {
                        $version = get_module_version($_);
                    }
                    my $desc = get_module_description($_);

                    $version = defined $version ? " ($version)" : " (n/a)";
                    $desc    = defined $desc    ? " $desc"      : " <description not available>";

                    printf("%-${options{t}}s%-${options{v}}s%-s\n", $module, $version, $desc ); 

                },

                preprocess => sub {
                    my ($dev, $inode) = stat $File::Find::dir or return;
                    $visited{"$dev:$inode"}++ ? () : @_;
                },
            },
            $inc_dir);
    }
}
else {                                          # ----- MS Windows
    for my $inc_dir (sort { length $b <=> length $a } @ARGV) {
        find({
                wanted => sub {
                    return unless /\.p(?:m|od)\z/ && -f;

                    #---------------------------------------------------------------------
                    #  return from function if there exists a pod-file for this module
                    #---------------------------------------------------------------------
                    my $pod = $_;
                    my $pm  = $_;
                    if ( m/\.pm\z/ )
                    {
                        $pod  =~ s/\.pm\z/\.pod/; 
                        return if -f $pod;
                    }

                    my $module  = get_module_name($File::Find::name, $inc_dir);
                    my $version;
                    if ( /\.pod\z/ )
                    {
                        $pm =~ s/\.pod\z/\.pm/; 
                        #-------------------------------------------------------------------
                        #  try to find the version from the pm-file
                        #-------------------------------------------------------------------
                        if ( -f $pm )
                        {
                            local $_;
                            $version = MM->parse_version($pm) || "";
                            $version = undef unless $version =~ /$rgx_version/;
                        }
                    }
                    else
                    {
                        $version = get_module_version($_);
                    }
                    my $desc = get_module_description($_);

                    $version = defined $version ? " ($version)" : " (n/a)";
                    $desc    = defined $desc    ? " $desc"      : " <description not available>";

                    printf("%-${options{t}}s%-${options{v}}s%-s\n", $module, $version, $desc ); 

                },
            },
            $inc_dir);
    }
}

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#  Modul Documentation
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

=head1 NAME
  
pmdesc3 - List name, version, and description of all installed perl modules and PODs

=head1 SYNOPSIS

    pmdesc3.pl

    pmdesc3.pl ~/perllib

=head1 DESCRIPTION

  pmdesc3.pl [-h] [-s] [-t ddd] [-v dd] [--] [dir [dir [dir [...]]]]

  OPTIONS:  -h     : print help message; show search path
            -s     : sort output (not under Windows)
            -t ddd : name column has width ddd (1-3 digits); default 36
            -v  dd : version column has width dd (1-2 digits); default 10

Find name, version and description of all installed Perl modules and PODs.
If no directories given, searches @INC .
The first column of the output (see below) can be used as module name or
FAQ-name for perldoc.

Some modules are split into a pm-file and an accompanying pod-file.
The version number is always taken from the pm-file.

The description found will be cut down to a length of at most
150 characters (prevents slurping in big amount of faulty docs).


=head2 Output

The output looks like this:

   ...
IO::Socket         (1.28)  Object interface to socket communications
IO::Socket::INET   (1.27)  Object interface for AF_INET domain sockets
IO::Socket::UNIX   (1.21)  Object interface for AF_UNIX domain sockets
IO::Stty           (n/a)   <description not available>
IO::Tty            (1.02)  Low-level allocate a pseudo-Tty, import constants.
IO::Tty::Constant  (n/a)   Terminal Constants (autogenerated)
   ...

The three parts module name, version and description are separated
by at least one blank.

=head1 REQUIREMENTS

ExtUtils::MakeMaker, File::Find, Getopt::Std

=head1 BUGS AND LIMITATIONS

The command line switch -s (sort) is not available under non-UNIX operating
systems.  An additional shell sort command can be used.

There are no known bugs in this module.

Please report problems to Fritz Mehner, mehner@fh-swf.de .

=head1 AUTHORS

  Tom Christiansen, tchrist@perl.com (pmdesc)
  Aristotle, http://qs321.pair.com/~monkads/ (pmdesc2)
  Fritz Mehner, mehner@fh-swf.de (pmdesc3.pl)

=head1 NOTES

pmdesc3.pl is based on pmdesc2 (Aristotle, http://qs321.pair.com/~monkads/).
pmdesc3.pl adds extensions and bugfixes.

pmdesc2 is based on pmdesc (Perl Cookbook, 1. Ed., recipe 12.19).
pmdesc2 is at least one magnitude faster than pmdesc.

=head1 VERSION

1.2.3

=cut

