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
*ksh*, *zsh*, *sh*, *csh*, *tcsh*, *fish*, *cmd*, as well as some scripting
languages such as *tcl*, *perl*, *python*, *ruby*, *cmake* and *r*.

Modules are useful in managing different versions of applications. Modules
can also be bundled into meta-modules that will load an entire suite of
different applications.

.. note:: Modules presented here are ones that modify the shell or script
   execution environment. They should not be confused with language-specific
   modules (e.g., Perl modules, Python modules or R modules) that add specific
   capabilities to scripts.

Quick examples
--------------

Here is an example of loading a module on a Linux machine under bash.
::

    $ module load gcc/9.4.0
    $ which gcc
    $ /usr/local/gcc/9.4.0/linux-x86_64/bin/gcc

Now we'll switch to a different version of the module
::

    $ module switch gcc gcc/10
    $ which gcc
    /usr/local/gcc/10.3.0/linux-x86_64/bin/gcc

And now we'll unload the module altogether
::

    $ module unload gcc
    $ which gcc
    gcc not found

Now we'll log into a different machine, using a different shell (tcsh).
::

    % module load gcc/10.3
    % which gcc
    /usr/local/gcc/10.3.0/linux-aarch64/bin/gcc

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
   changes

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


Get started with Modules
------------------------

Learn how to retrieve and install Modules :ref:`on Unix<INSTALL>` or
:ref:`on Windows<INSTALL-win>`. An overlook on the new functionalities
introduced by each version is available in the :ref:`MIGRATING` guide.
:ref:`NEWS` provides the full list of changes added in each version. The
:ref:`changes` document gives an in-depth view of the modified behaviors and
new features between major versions.

Reference manual page for the :ref:`module(1)` and :ref:`ml(1)` commands and
for :ref:`modulefile(4)` script provide details on all supported options.

A :ref:`cookbook` of recipes describes how to use the various features of
Modules and how to extend the :command:`module` command to achieve specific
needs.

If you have questions, comments or development suggestions for the Modules
community, please read the :ref:`CONTRIBUTING` guide.

License
-------

Modules is distributed under the GNU General Public License, either version 2
or (at your option) any later version (GPL v2+).
