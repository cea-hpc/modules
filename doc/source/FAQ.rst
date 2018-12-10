Frequently Asked Questions on Environment Modules
=================================================

Module command
--------------

How does the ``module`` command work?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The command ``module`` is an alias for something like:

sh:

.. code-block:: sh

     module () 
     { 
        eval `/some/path/modulecmd sh $*`
     }

csh:

.. code-block:: csh

     eval `/some/path/modulecmd csh !*`

Where the ``modulecmd`` outputs valid shell commands to *stdout* which manipulates the shell's environment. Any text that is meant to be seen by the user **must** be sent to *stderr*. For example:

.. code-block:: tcl

     puts stderr "\n\tSome Text to Show\n"

I put the ``module`` command in a script and I run the script... it doesn't change my environment?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A child process (script) can not change the parent process environment. A ``module load`` in a script only affects the environment for the script itself. The only way you can have a script change the current environment is to *source* the script which reads it into the current process.

sh:

.. code-block:: sh

     . somescript

csh:

.. code-block:: csh

     source somescript

How do I capture the module command output?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This ties in with the very first question. Since the :ref:`module(1)` command is essentially an *eval*, the visible output to the screen must necessarily be sent to *stderr*. It becomes a matter on how to capture output from *stderr* for the various shells. The following examples just show how to spool the output from the **avail** command to a file. This also works for the various other module commands like **list**, **display**, etc. There are also various tricks for piping *stderr* to another program.

sh:

.. code-block:: sh

     module avail 2> spoolfile

csh: (overwrite existing file)

.. code-block:: csh

     module avail >&! spoolfile

How to use the module command from Makefile?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To make use of the ``module`` command from a Makefile, the shell initialization script should first be sourced within Makefile rule to define the ``module`` function in that context. Environment variable ``MODULESHOME`` may help to locate the shell initialization script in a generic way, like done in the following example: 

.. code-block:: Makefile

     module_list:
     	source $$MODULESHOME/init/bash; module list

Modulefiles
-----------

I want the modulefile to source some rc script that came with some application
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to do this... you can, but that's not what *modules* is all about. It is possible to have the :ref:`modulefile(4)` output some text to *stdout* to source some script when loading. However, you lose the advantage of this tool, because you won't be able to **unload** this environment. If you're tempted to do this... **don't**.

However, you can craft a modulefile by capturing the environment variables created or changed by the *rc* script. This is the goal of the ``createmodule.py`` and the ``createmodule.sh`` utilities provided in the *modules* distribution:

.. parsed-literal::

     \ |bindir|\ /createmodule.py somescript

You can also check out `<https://sourceforge.net/projects/env2/>`_, which can translate the shell file into a :ref:`modulefile(4)` and possibly reduce the amount of work needed to convert and maintain.

How do I specify the *default* modulefile for some modulefile directory?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Modules usually uses the the highest lexicographically sorted :ref:`modulefile(4)` under the directory, unless there is a ``.version`` file in that directory which has a format like the following where "native" is a modulefile (or a sub-directory) in that directory. It's also possible to set the default with a ``.modulerc`` file with a **module-version** command.

.. code-block:: tcl

     #%Module1.0#####################################################################
     ##
     ## version file for Perl
     ##
     set ModulesVersion	"native"

I don't want a *default* modulefile for the directory?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Follow the same prescription as setting a *default*, but give some *bogus* value, say *no_default*. The :ref:`module(1)` command will return an error message when no specific version is given.


Build Issues
------------

The configure script complains about Tclx
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
::

     ...
     checking for TclX configuration (tclxConfig.sh)... not found
     checking for TclX version... using 8.4
     checking TCLX_VERSION... 8.4
     checking TCLX_LIB_SPEC... TCLX_LIB_SPEC not found, need to use --with-tclx-lib
     checking TCLX_INCLUDE_SPEC... TCLX_INCLUDE_SPEC not found, need to use --with-tclx-inc
     ...

TclX is an optional library that can speed up some operations. You don't need TclX for modules to compile and work, so you can add the --without-tclx option when configuring and it should proceed to completion. In fact, it should have succeeded anyways and just not attempt to use TclX.

Otherwise, you can load the TclX library package for your OS and the ``configure`` script should find it. If not then if you know where the ``tclxConfig.sh`` file or the library and include files are placed then use the following options::

     --with-tclx=<dir>       directory containing TclX configuration
                             (tclxConfig.sh) [[searches]]
     --with-tclx-ver=X.Y     TclX version to use [[search]]
     --with-tclx-lib=<dir>   directory containing tclx libraries (libtclxX.Y)
                             [[none]]
     --with-tclx-inc=<dir>   directory containing tclx include files
                             (tclExtend.h,...) [[none]]


Meta Information
----------------

Why does *modules* use Tcl?
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The first versions of the *Modules* package used shell scripts to do its magic. The original authors then chose to implement the same in C to speed things up and to add features. At the time the only easily embeddable interpreter was Tcl which provided a standard language and the glue. Now that other interpreters are available they could be embedded, but haven't so far. There is also a pure Tcl version available.

How can I help?
^^^^^^^^^^^^^^^

We can use help at various levels. The best way to contribute is to send in a patch file (see the FAQ on how to generate a patch file) with whatever fixes. The patch will be reviewed and tested. If you are a regular contributer then you'll likely be invited to become a developer and to have direct source access, and the fame, power, and prestige that all entails.

How do I download the source repository?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Anonymously clone the git repository, view the list of branches, and set to a specific branch:

.. code-block:: sh

     git clone git://git.code.sf.net/p/modules/git modules-myversion
     cd modules-myversion
     git branch -a
     git checkout modules-3-X-Y
     git status

How do I generate a patch file?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you're starting from a tarball
"""""""""""""""""""""""""""""""""

Unpack the tarball and it should place the sources into a directory named modules-3.X.Y , then rename the directory to modules-3.X.Y-myversion or something like that. Make whatever changes you want, and be sure to test the changes and if you can add tests to identify the bug and the fix... that will endear yourself to the developers.

Once you have the changes in your version, then unpack the original sources from the tarball in a directory side-by-side to the directory with your version, and at that parent level run the following ``diff`` command:

.. code-block:: sh

     diff -u -r -P -N modules-3.X.Y modules-3.X.Y-myversion  > my.patch

If you're starting from the git cloned repository:
""""""""""""""""""""""""""""""""""""""""""""""""""

From within the git repositories.

.. code-block:: sh

     git diff > my.patch
