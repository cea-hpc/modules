.. _source-script-in-modulefile:

Source shell script in modulefile
=================================

When working with large software suite providing a shell script for their
enablement in user environment, it is usually desired to also provide access
to these software through ``module``. However these software enablement may be
complex and it may be wise to keep using the shell script provided by software
editor rather crafting a modulefile from scratch.

This recipe describes how to make modulefiles for such software by using the
enablement shell script provided with them.

Implementation
--------------

Modules version 4.6 introduces a new sub-command named :subcmd:`sh-to-mod` and
a new modulefile command named :mfcmd:`source-sh`. The ``sh-to-mod``
outputs as a modulefile content the environment changes done by the evaluation
of a shell script passed as argument. On the other hand, the ``source-sh``
modulefile command sources environment changes done by the evaluation of a
shell script passed as argument.

Both new features relies on the same mechanism that starts a designated shell
to:

* get current environment state (environment variables, shell aliases, shell
  functions and current working directory)
* source designated shell script with defined arguments
* get resulting environment state

Once done, environment prior and after script source are compared to determine
the corresponding environment changes and translate those changes into
modulefile commands (:mfcmd:`setenv`, :mfcmd:`prepend-path`,
:mfcmd:`set-alias`, :mfcmd:`set-function`, ...).

:subcmd:`sh-to-mod` outputs these resulting modulefile commands. This output
can be redirected into a file to create a modulefile. :mfcmd:`source-sh` on
the other hand sources the resulting modulefile commands to evaluate them as
if they were written in the modulefile calling ``source-sh``.

``sh-to-mod`` and ``source-sh`` support the following shells: *sh*, *dash*,
*csh*, *tcsh*, *bash*, *ksh*, *ksh93*, *zsh* and *fish*.

**Compatible with Modules v4.6+**

Basic usage example
-------------------

For this recipe, a dummy software named *foo* is used as example. *foo* is
installed in version 1.2 in ``example/source-script-in-modulefile/foo-1.2``
directory and it provides a ``foo-setup.sh`` script to activate itself in
user environment:

.. literalinclude:: ../../example/source-script-in-modulefile/foo-1.2/foo-setup.sh
   :language: bash
   :caption: foo-setup.sh

First line of ``foo-setup.sh`` script helps to identify which shell needs to
be used to evaluate it: ``bash``.

:subcmd:`sh-to-mod` may be used to get this script translated as a
modulefile:

.. parsed-literal::

    :ps:`$` module sh-to-mod bash example/source-script-in-modulefile/foo-1.2/foo-setup.sh arg1
    #%Module
    :sgrcm:`prepend-path`    PATH example/source-script-in-modulefile/foo-1.2/bin
    :sgrcm:`set-alias`       foo {foobin -q -l}
    :sgrcm:`setenv`          FOOENV arg1

Output could be redirected into a ``foo/1.2`` file and make it the modulefile
to enable software foo:

.. parsed-literal::

    :ps:`$` mkdir -p modulefiles/foo
    :ps:`$` module sh-to-mod bash example/source-script-in-modulefile/foo-1.2/foo-setup.sh arg1 >modulefiles/foo/1.2
    :ps:`$` module use ./modulefiles
    :ps:`$` module show foo
    -------------------------------------------------------------------
    :sgrhi:`modulefiles/foo/1.2`:

    :sgrcm:`prepend-path`    PATH example/source-script-in-modulefile/foo-1.2/bin
    :sgrcm:`set-alias`       foo {foobin -q -l}
    :sgrcm:`setenv`          FOOENV arg1
    -------------------------------------------------------------------

Instead of transforming shell script in modulefile, a modulefile using
:mfcmd:`source-sh` modulefile command to evaluate shell script at modulefile
evaluation time may be written:

.. literalinclude:: ../../example/source-script-in-modulefile/modulefiles/foo/1.2
   :language: Tcl
   :caption: modulefiles/foo/1.2

When displaying a modulefile using ``source-sh`` modulefile command,
modulefile commands resulting from ``source-sh`` evaluation are reported:

.. parsed-literal::

    :ps:`$` module show foo/1.2
    -------------------------------------------------------------------
    :sgrhi:`.../modulefiles/foo/1.2`:

    :sgrcm:`prepend-path`    PATH example/source-script-in-modulefile/foo-1.2/bin
    :sgrcm:`set-alias`       foo {foobin -q -l}
    :sgrcm:`setenv`          FOOENV arg1
    -------------------------------------------------------------------

Loading this ``foo/1.2`` module will enable access to software *foo*:

.. parsed-literal::

    :ps:`$` module load foo/1.2
    :ps:`$` alias foo
    alias foo='foobin -q -l'
    :ps:`$` foo
    foo, version 1.2

Unloading ``foo/1.2`` module will properly revert these environment settings:

.. parsed-literal::

    :ps:`$` module unload foo/1.2
    :ps:`$` alias foo
    bash: alias: foo: not found
    :ps:`$` foobin
    bash: foobin: command not found

As conclusion, these new features enable to leverage the setup scripts that
are provided along with software to make them reachable from the ``module``
environment.

Usage with shell-specific scripts
---------------------------------

When the initialization script provided by software only defines environment
variables, this script could be used to setup the user environment through the
use of ``source-sh`` in a modulefile whatever the shell ran by user, as the
``module`` command will accurately translate script changes into the language
of the running shell.

For instance the ``foo/1.2`` module, that uses the ``source-sh`` modulefile
command over the ``foo-setup.sh`` bash script, could also be used when running
the ``tcsh`` or ``fish`` shell:

.. parsed-literal::

    :ps:`$` echo $version
    tcsh 6.22.03 (Astron) 2020-11-18 (x86_64-unknown-linux) ...
    :ps:`$` module show foo/1.2
    -------------------------------------------------------------------
    :sgrhi:`.../modulefiles/foo/1.2`:

    :sgrcm:`prepend-path`    PATH example/source-script-in-modulefile/foo-1.2/bin
    :sgrcm:`set-alias`       foo {foobin -q -l}
    :sgrcm:`setenv`          FOOENV arg1
    -------------------------------------------------------------------
    :ps:`$` module load foo/1.2
    :ps:`$` foo
    foo, version 1.2

Software may sometimes provide a specific script for each shell they support
as they do not perform their initialization the same way on every shell. Quite
often a shell function is defined for *sh* shells whereas an alias is setup
for *csh* shells (as such shells do not support shell function).

Dummy software *bar* is used to demonstrate this situation. *bar* is installed
in version 2.1 in ``example/source-script-in-modulefile/bar-2.1`` directory
and it provides a ``bar-setup.sh`` and a ``bar-setup.csh`` scripts to activate
itself in user environment, depending on the shell kind used.

.. literalinclude:: ../../example/source-script-in-modulefile/bar-2.1/bar-setup.sh
   :language: bash
   :caption: bar-setup.sh

.. literalinclude:: ../../example/source-script-in-modulefile/bar-2.1/bar-setup.csh
   :language: csh
   :caption: bar-setup.csh

To accurately initialize environment for *bar* software, the ``bar`` module
needs to call the ``.sh`` script if user is currently running a shell from the
sh family, or to call the ``.csh`` script if user runs a csh-kind shell.

.. literalinclude:: ../../example/source-script-in-modulefile/modulefiles/bar/2.1
   :language: Tcl
   :caption: modulefiles/bar/2.1

This way the ``bar`` shell function is initialized when loading module from a user
environment running a *sh* shell:

.. parsed-literal::

    :ps:`$` echo $BASH_VERSION
    5.1.0(1)-release
    :ps:`$` module use example/source-script-in-modulefile/modulefiles
    :ps:`$` module show bar
    -------------------------------------------------------------------
    :sgrhi:`.../modulefiles/bar/2.1`:

    :sgrcm:`prepend-path`    PATH example/source-script-in-modulefile/bar-2.1/bin
    :sgrcm:`set-function`    bar {
        barbin -q -l}
    -------------------------------------------------------------------
    :ps:`$` module load bar
    :ps:`$` type bar
    bar is a function
    bar () 
    { 
        barbin -q -l
    }
    :ps:`$` bar
    bar, version 2.1

Whereas the ``bar`` shell alias is setup on *csh* shell environment:

.. parsed-literal::

    :ps:`$` echo $version
    tcsh 6.22.03 (Astron) 2020-11-18 (x86_64-unknown-linux) ...
    :ps:`$` module use example/source-script-in-modulefile/modulefiles
    :ps:`$` module show bar
    -------------------------------------------------------------------
    :sgrhi:`.../modulefiles/bar/2.1`:

    :sgrcm:`prepend-path`    PATH example/source-script-in-modulefile/bar-2.1/bin
    :sgrcm:`set-alias`       bar {barbin -q -l}
    -------------------------------------------------------------------
    :ps:`$` module load bar
    :ps:`$` alias bar
    barbin -q -l
    :ps:`$` bar
    bar, version 2.1

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
