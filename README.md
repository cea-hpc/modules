Modules, provides dynamic modification of a user's environment
==============================================================
[![Linux/OS X Build Status](https://travis-ci.org/cea-hpc/modules.svg?branch=master)](https://travis-ci.org/cea-hpc/modules)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/cea-hpc/modules?svg=true&branch=master)](https://ci.appveyor.com/project/xdelaruelle/modules-a6nha/branch/master)
[![FreeBSD Build Status](https://api.cirrus-ci.com/github/cea-hpc/modules.svg)](https://cirrus-ci.com/github/cea-hpc/modules)
[![Coverage Status](https://codecov.io/gh/cea-hpc/modules/branch/master/graph/badge.svg)](https://codecov.io/gh/cea-hpc/modules)
[![Documentation Status](https://readthedocs.org/projects/modules/badge/?version=latest)](https://modules.readthedocs.io/en/latest/?badge=latest)
[![Packaging status](https://repology.org/badge/tiny-repos/environment-modules.svg)](https://repology.org/metapackage/environment-modules/versions)

The Modules package is a tool that simplify shell initialization and
lets users easily modify their environment during the session with
modulefiles.

Each modulefile contains the information needed to configure the shell for
an application. Once the Modules package is initialized, the environment can
be modified on a per-module basis using the module command which interprets
modulefiles. Typically modulefiles instruct the module command to alter or
set shell environment variables such as PATH, MANPATH, etc. modulefiles may
be shared by many users on a system and users may have their own collection
to supplement or replace the shared modulefiles.

Modules can be loaded and unloaded dynamically and atomically, in an clean
fashion. All popular shells are supported, including bash, ksh, zsh, sh,
csh, tcsh, fish, as well as some scripting languages such as tcl, perl,
python, ruby, cmake and r.

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


Getting things running
----------------------

The simplest way to build and install Modules on a Unix system is:

    $ ./configure
    $ make
    $ make install

To learn the details on how to install modules see `INSTALL.txt` for Unix
system or `INSTALL-win.txt` for Windows.


Requirements
------------

 * Tcl >= 8.4


License
-------

Modules is distributed under the GNU General Public License version 2 (GPL
v2). Read the file `COPYING.GPLv2` for details.


Documentation
-------------

Look at `NEWS` for summarized information regarding the changes brought
by each released version. Look at `ChangeLog` for detailed information
regarding changes.

The `doc` directory contains both the paper and man pages describing the
user's and the module writer's usage. To generate the documentation files,
like the man pages (you need Sphinx >= 1.0 to build the documentation), just
type:

    $ ./configure
    $ make -C doc all

The following man pages are provided:

    module(1), modulefile(4)


Test suite
----------

Regression testing scripts are available in the `testsuite` directory (you
need dejagnu to run the test suite):

    $ ./configure
    $ make test

Once modules is installed after running `make install`, you have the
ability to test this installation with:

    $ make testinstall


Transition from v3.2
--------------------

Starting from v4.0, the Modules project provides the `module` command based
on the native Tcl implementation as main version instead of the traditional
C version. The full Tcl rewrite of the Modules package, previously called
Modules-Tcl, was started in 2002 and has now reached maturity to take over
the binary version in order to push forward the *module* concept.

All new releases are in fact *double-releases* as they also ship the latest
stable version of the traditional C flavor of modulecmd. This *compatibility*
version is labeled 3.2.11 as it is based on 3.2.10 with addition of widely
used patches. The compatibility version builds and installs by default
along with the new main version (v4 or later). Same initialization scripts
are shared between compatibility and new main versions and a `switchml`
shell function enables to swap from one module flavor to another.

If you are moving from Modules 3.2 to 4.0 or later, please look at the
MIGRATING document. It provides an outlook of what has changed between the 2
versions. Both v3.2 and >=v4.0 are quite similar and transition to the new
major version should be smooth. Slights differences may however be noticed
in a few use-cases and the migration guide provides details about them.


Links
-----

Web site:

    http://modules.sourceforge.net

Online documentation:

    https://modules.readthedocs.io

GitHub source respository:

    https://github.com/cea-hpc/modules

GitHub Issue tracking system:

    https://github.com/cea-hpc/modules/issues

SourceForge project page:

    http://sourceforge.net/projects/modules/


Authors
-------

Current core developers and maintainers are:

 * Xavier Delaruelle <xavier.delaruelle@cea.fr>
 * R.K. Owen <rk@owen.sj.ca.us>
 * Kent Mein <mein@cs.umn.edu>

The following people have notably contributed to Modules and Modules would
not be what it is without their contributions:

 * Mark Lakata
 * Harlan Stenn
 * Leo Butler
 * Robert Minsk
 * Jens Hamisch
 * Peter W. Osel
 * John L. Furlani
