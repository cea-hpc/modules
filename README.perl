Ron Isaacson <Ron.Isaacson@morganstanley.com>
Subject: Perl Env::Modulecmd

Here at Morgan Stanley, we make extensive use of modulefiles. We have at
least one Perl package that requires that a particular modulefile be
loaded, or it will not function. I decided to develop a Perl module to
automate this process. (Actually, the most difficult part was not
confusing everybody with the dual meaning of the term "module"! Heh...)

There is now a module on CPAN called Env::Modulecmd. You can download it
from your favorite CPAN mirror, or directly from the main CPAN site at
http://www.cpan.org/authors/id/I/IS/ISAACSON/Env-Modulecmd-1.0.tar.gz .
Documentation is included, and the module is very easy to install and
use.


If you try out this module and have any feedback (positive or negative),
please pass it along. Suggestions and bug reports are welcome.

--
Ron Isaacson
Morgan Stanley
ron.isaacson@morganstanley.com / (718) 754-2345

------------------------------------------------------------------------
This README file was added to Modules-3.1.3 if you are reading this in a
later version, be sure to look for later versions of Env-Modulecmd-x.y.tar.gz
at http://www.cpan.org/authors/id/I/IS/ISAACSON/
------------------------------------------------------------------------
perldoc for Env::Modulecmd

NAME
       Env::Modulecmd - Interface to modulecmd from Perl

SYNOPSIS
	 # import bootstraps, executed at compile-time

	   # explicit operations

	   use Env::Modulecmd { load => 'foo/1.0',
				unload => ['bar/1.0', 'baz/1.0'],
			      };

	   # implied loading

	   use Env::Modulecmd qw(quux/1.0 quuux/1.0);

	   # hybrid

	   use Env::Modulecmd ('bazola/1.0', 'ztesch/1.0',
			       { load => 'oogle/1.0',
				 unload => [qw(foogle/1.0 boogle/1.0)],
			       }
			      );

	 # implicit functions, executed at run-time

	   Env::Modulecmd::load (qw(fred/1.0 jim/1.0 sheila/barney/1.0));
	   Env::Modulecmd::unload ('corge/grault/1.0', 'flarp/1.0');
	   Env::Modulecmd::pippo ('pluto/paperino/1.0');


DESCRIPTION
       Env::Modulecmd provides an automated interface to
       modulecmd from Perl. The most straightforward use of
       Env::Modulecmd is for loading and unloading modules at
       compile time, although many other uses are provided.


       In general, Env::Modulecmd works by making a system call
       to 'modulecmd perl [cmd] [module]', under the assumption
       that modulecmd is in your PATH. If you set the environment
       variable PERL_MODULECMD, Env::Modulecmd will use that
       value in place of modulecmd. If modulecmd is not found,
       the shell will return an error and the script will die.

       If modulecmd outputs anything to standard error, it is
       assumed to have failed. In this case, its error output is
       repeated on Perl's standard error, and the script dies.
       Otherwise, modulecmd is assumed to have succeeded, and its
       output (if any) is eval'ed.

       If you attempt to load a module which has already been
       loaded, or perform some other benign operation, modulecmd
       will generate neither output nor error; this condition is
       silently ignored.

       Compile-Time Usage

       You can specify compile-time arguments to Env::Modulecmd
       on the use line, as follows:

	 use Env::Modulecmd ('bazola/1.0', 'ztesch/1.0',
			     { load => 'oogle/1.0',
			       unload => [qw(foogle/1.0 boogle/1.0)],
			     }
			    );

       Each argument is assumed to be either a scalar or a
       hashref. If it's a scalar, Env::Modulecmd assumes it's the
       name of a module you want to load. If it's a hashref, then
       each key is the name of a modulecmd operation (ie: load,
       unload) and each value is either a scalar (operate on one
       module) or an arrayref (operate on several modules).

       In the example given above, bazola/1.0 and ztesch/1.0 will
       be loaded by implicit usage. oogle/1.0 will be loaded
       explicitly, and foogle/1.0 and boogle/1.0 will be
       unloaded.

       Run-Time Usage

       Additional module operations can be performed at run-time
       by using implicit functions. For example:

	 Env::Modulecmd::load (qw(fred/1.0 jim/1.0 sheila/barney/1.0));
	 Env::Modulecmd::unload ('corge/grault/1.0', 'flarp/1.0');
	 Env::Modulecmd::pippo ('pluto/paperino/1.0');

       Each function name is passed as a command name to
       modulecmd, and each call can include one or more modules
       to be processed. The example above will generate the
       following six calls to modulecmd:

	 modulecmd perl load fred/1.0
	 modulecmd perl load jim/1.0
	 modulecmd perl load sheila/barney/1.0
	 modulecmd perl unload corge/grault/1.0
	 modulecmd perl unload flarp/1.0
	 modulecmd perl pippo pluto/paperino/1.0


SEE ALSO
       For more information about modules, see the module(1)
       manpage or http://www.modules.org.

BUGS
       If you find any bugs, or if you have any suggestions for
       improvement, please contact the author.

AUTHOR
       Ron Isaacson <Ron.Isaacson@morganstanley.com>

COPYRIGHT
       Copyright (c) 2001, Morgan Stanley Dean Witter and Co.

       This program is free software; you can redistribute it
       and/or modify it under the terms of the GNU General Public
       License as published by the Free Software Foundation;
       either version 2 of the License, or (at your option) any
       later version.

       This program is distributed in the hope that it will be
       useful, but WITHOUT ANY WARRANTY; without even the implied
       warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
       PURPOSE. See the GNU General Public License for more
       details.

       A copy of the GNU General Public License was distributed
       with this program in a file called LICENSE. For additional
       copies, write to the Free Software Foundation, Inc., 59
       Temple Place, Suite 330, Boston, MA 02111-1307, USA.

