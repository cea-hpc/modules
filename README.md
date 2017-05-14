Modules-Tcl, native Tcl version of the Modules package
======================================================
[![Build Status](https://travis-ci.org/cea-hpc/modules-tcl.svg?branch=master)](https://travis-ci.org/cea-hpc/modules-tcl)
[![Coverage Status](https://codecov.io/gh/cea-hpc/modules-tcl/branch/master/graph/badge.svg)](https://codecov.io/gh/cea-hpc/modules-tcl)

The Modules package is a tool that simplify shell initialization and
lets users easily modify their environment during the session with
modulefiles. Modules-Tcl is the full Tcl rewrite of the Modules package.

Each modulefile contains the information needed to configure the shell for
an application. Once the Modules package is initialized, the environment can
be modified on a per-module basis using the module command which interprets
modulefiles. Typically modulefiles instruct the module command to alter or
set shell environment variables such as PATH, MANPATH, etc. modulefiles may
be shared by many users on a system and users may have their own collection
to supplement or replace the shared modulefiles.

Modules can be loaded and unloaded dynamically and atomically, in an clean
fashion. All popular shells are supported, including bash, ksh, zsh, sh,
csh, tcsh, fish, as well as some scripting languages such as perl and python.

Modules are useful in managing different versions of applications. Modules
can also be bundled into metamodules that will load an entire suite of
different applications.


Quick examples
--------------

Here is an example of loading a module on a Linux machine under bash.

    $ module load gcc/6.1.1
    $ which gcc
    $ /usr/local/gcc/6.1.1/linux-x86_64/bin/gcc

Now we'll switch to a different version of the module

    $ module switch gcc gcc/6.3.1
    $ which gcc
    /usr/local/gcc/6.3.1/linux-x86_64/bin/gcc

And now we'll unload the module altogether

    $ module unload gcc
    $ which gcc
    gcc not found

Now we'll log into a different machine, using a different shell (tcsh).

    % module load gcc/6.3.1
    % which gcc
    /usr/local/gcc/6.3.1/linux-aarch64/bin/gcc

Note that the command line is exactly the same, but the path has
automatically configured to the correct architecture.


Motivation for this Tcl version
-------------------------------

The Modules package was originally written in C. This version is referred as
Modules C or C version. Modules-Tcl is a complete rewrite of the Modules
package aiming at the following goal:

 1. Written in pure TCL, so that there are no porting issues
 2. Faster, smaller (implements most common features of the application)
 3. 100% compatibility with existing modulefiles
 4. Some new command line features
 5. Path variable counters, to allow shared usage of particular path elements.
    I.e. modules can append /usr/bin, which is not unloaded until all the
    modules that loaded it unload too.
 6. Support for "deep" modules.


Getting things running
----------------------

To learn how to install modules see `INSTALL.txt' for Unix system or
`INSTALL-win.txt' for Windows

To have things running efficiently you will need a lot of additional setup.
For an example take a look at `doc/example.txt' which explains how things
have been setup at the University of Minnesota computer science department.


Requirements
------------

 * Tcl >= 8.4


License
-------

Modules-Tcl is distributed under the GNU General Public License version 2
(GPL v2). Read the file `COPYING.GPLv2' for details.


Documentation
-------------

Look at `NEWS' for summarized information regarding the changes brought
by each released version. Look at `ChangeLog' for detailed information
regarding changes.

The `doc' directory contains both the paper and man pages describing the
user's and the module writer's usage. To generate the documentation files,
like the man pages (you need Perl podlators to build the documentation),
just type:

    $ ./configure
    $ make -C doc all

The following man pages are provided:

    module(1), modulefile(4)


Test suite
----------

Regression testing scripts are available in the `testsuite' directory (you
need dejagnu to run the test suite):

    $ ./configure
    $ make test

Once modules is installed after running `make install', you have the
ability to test this installation with:

    $ make testinstall


Links
-----

Web site:

    http://modules.sourceforge.net

SourceForge source respository:

    https://sourceforge.net/p/modules/modules-tcl/

SourceForge Issue tracking system:

    https://sourceforge.net/p/modules/_list/tickets

SourceForge project page:

    http://sourceforge.net/projects/modules/


Authors
-------

This project was started by Mark Lakata. Current developers/maintainers are:

 * Kent Mein <mein@cs.umn.edu>
 * Xavier Delaruelle <xavier.delaruelle@cea.fr>
