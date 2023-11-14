# <img src="https://raw.githubusercontent.com/cea-hpc/modules/main/doc/img/modules_red.svg" height="180" alt="Modules"/>

[![Linux Build Status](https://github.com/cea-hpc/modules/workflows/linux-tests/badge.svg)](https://github.com/cea-hpc/modules/actions?query=workflow:linux-tests)
[![Windows Build Status](https://github.com/cea-hpc/modules/workflows/windows-tests/badge.svg)](https://github.com/cea-hpc/modules/actions?query=workflow:windows-tests)
[![FreeBSD/OS X/Linux Build Status](https://api.cirrus-ci.com/github/cea-hpc/modules.svg)](https://cirrus-ci.com/github/cea-hpc/modules)
[![Coverage Status](https://codecov.io/gh/cea-hpc/modules/branch/main/graph/badge.svg)](https://codecov.io/gh/cea-hpc/modules)
[![Documentation Status](https://readthedocs.org/projects/modules/badge/?version=latest)](https://modules.readthedocs.io/en/latest/?badge=latest)
[![Packaging status](https://repology.org/badge/tiny-repos/environment-modules.svg)](https://repology.org/metapackage/environment-modules/versions)
[![Twitter](https://img.shields.io/twitter/url/https/twitter.com/EnvModules.svg?style=social&label=Follow)](https://twitter.com/EnvModules)

Modules, provides dynamic modification of a user's environment
==============================================================

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
csh, tcsh, fish, cmd, as well as some scripting languages such as tcl, perl,
python, ruby, cmake and r.

Modules are useful in managing different versions of applications. Modules
can also be bundled into meta-modules that will load an entire suite of
different applications.


Quick examples
--------------

Here is an example of loading a module on a Linux machine under bash.

    $ module load gcc/9.4.0
    $ which gcc
    $ /usr/local/gcc/9.4.0/linux-x86_64/bin/gcc

Now we'll switch to a different version of the module

    $ module switch gcc gcc/10
    $ which gcc
    /usr/local/gcc/10.3.0/linux-x86_64/bin/gcc

And now we'll unload the module altogether

    $ module unload gcc
    $ which gcc
    gcc not found

Now we'll log into a different machine, using a different shell (tcsh).

    % module load gcc/10.3
    % which gcc
    /usr/local/gcc/10.3.0/linux-aarch64/bin/gcc

Note that the command line is exactly the same, but the path has
automatically configured to the correct architecture.


Getting things running
----------------------

The simplest way to build and install Modules on a Unix system is:

    $ ./configure
    $ make
    $ make install

To learn the details on how to install modules see [`INSTALL.txt`][1] for Unix
system or [`INSTALL-win.txt`][2] for Windows.


Requirements
------------

 * Tcl >= 8.5


License
-------

Modules is distributed under the GNU General Public License, either version 2
or (at your option) any later version (GPL v2+). Read the file `COPYING.GPLv2`
for details.


Documentation
-------------

See [`MIGRATING`][3] to get an overlook of the new functionalities introduced
by each released versions. [`NEWS`][4] provides the full list of changes added
in each version. The [`Changes`][5] document gives an in-depth view of the
modified behaviors and new features between major versions. You may also look
at the `ChangeLog` for the technical development details.

The `doc` directory contains both the paper and man pages describing the
user's and the module writer's usage. To generate the documentation files,
like the man pages (you need Sphinx >= 1.0 to build the documentation), just
type:

    $ ./configure
    $ make -C doc all

The following man pages are provided:

    module(1), ml(1), modulefile(5)


Test suite
----------

Regression testing scripts are available in the `testsuite` directory (you
need DejaGnu to run the test suite):

    $ ./configure
    $ make test

Once modules is installed after running `make install`, you have the
ability to test this installation with:

    $ make testinstall


Links
-----

Web site:

> http://modules.sourceforge.net

Online documentation:

> https://modules.readthedocs.io

GitHub source repository:

> https://github.com/cea-hpc/modules

GitHub Issue tracking system:

> https://github.com/cea-hpc/modules/issues

SourceForge project page:

> http://sourceforge.net/projects/modules/


Code of conduct
---------------

Please note that Modules has a [Code of conduct][6]. By participating in the
Modules community, you agree to abide by its rules.


Authors
-------

Current core developer and maintainer is:

 * Xavier Delaruelle <xavier.delaruelle AT cea.fr>

The following people have notably contributed to Modules and Modules would
not be what it is without their contributions:

 * R.K. Owen
 * Kent Mein
 * Mark Lakata
 * Harlan Stenn
 * Leo Butler
 * Robert Minsk
 * Jens Hamisch
 * Peter W. Osel
 * John L. Furlani

[1]: https://modules.readthedocs.io/en/stable/INSTALL.html
[2]: https://modules.readthedocs.io/en/stable/INSTALL-win.html
[3]: https://modules.readthedocs.io/en/stable/MIGRATING.html
[4]: https://modules.readthedocs.io/en/stable/NEWS.html
[5]: https://modules.readthedocs.io/en/stable/changes.html
[6]: CODE_OF_CONDUCT.md
