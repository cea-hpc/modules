.. Home page

Environment Modules
===================

**Welcome to the Environment Modules documentation portal. The Environment
Modules package provides for the dynamic modification of a user's environment
via modulefiles.**

The Modules package is a tool that simplifies shell initialization and
lets users easily modify their environment during a session using
modulefiles.

Each modulefile contains the information needed to configure the shell for
an application. Once the Modules package is initialized, the environment
can be modified on a per-module basis using the :command:`module` command
which interprets modulefiles. Typically modulefiles instruct the
:command:`module` command to alter or set shell environment variables such as
:envvar:`PATH`, :envvar:`MANPATH`, etc. modulefiles may be shared by many
users on a system and users may have their own collection to supplement or
replace the shared modulefiles.

Modules can be **loaded** and **unloaded** dynamically and atomically,
in an clean fashion. All popular shells are supported, including *bash*,
*ksh*, *zsh*, *sh*, *csh*, *tcsh*, *fish*, as well as some scripting
languages such as *tcl*, *perl*, *python*, *ruby*, *cmake* and *r*.

Modules are useful in managing different versions of applications. Modules
can also be bundled into metamodules that will load an entire suite of
different applications.

.. note:: Modules presented here are ones that modify the shell or script
   execution environment. They should not be confused with language-specific
   modules (e.g., Perl modules, Python modules or R modules) that add specific
   capabilities to scripts.

Quick examples
--------------

Here is an example of loading a module on a Linux machine under bash.
::

    $ module load gcc/6.1.1
    $ which gcc
    $ /usr/local/gcc/6.1.1/linux-x86_64/bin/gcc

Now we'll switch to a different version of the module
::

    $ module switch gcc gcc/6.3.1
    $ which gcc
    /usr/local/gcc/6.3.1/linux-x86_64/bin/gcc

And now we'll unload the module altogether
::

    $ module unload gcc
    $ which gcc
    gcc not found

Now we'll log into a different machine, using a different shell (tcsh).
::

    % module load gcc/6.3.1
    % which gcc
    /usr/local/gcc/6.3.1/linux-aarch64/bin/gcc

Note that the command line is exactly the same, but the path has
automatically configured to the correct architecture.

.. toctree::
   :hidden:
   :maxdepth: 2
   :caption: Basics

   INSTALL
   INSTALL-win
   MIGRATING
   NEWS
   FAQ
   diff_v3_v4

.. toctree::
   :hidden:
   :maxdepth: 2
   :caption: Examples

   cookbook

.. toctree::
   :hidden:
   :maxdepth: 2
   :caption: Reference

   ml
   module
   modulefile

.. toctree::
   :hidden:
   :maxdepth: 2
   :caption: Development

   CONTRIBUTING
   design


License
-------

Modules is distributed under the GNU General Public License, either version 2
or (at your option) any later version (GPL v2+).
