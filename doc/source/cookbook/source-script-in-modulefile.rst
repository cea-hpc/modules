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

Usage example
-------------

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
modulefile::

    $ module sh-to-mod bash example/source-script-in-modulefile/foo-1.2/foo-setup.sh arg1
    #%Module
    prepend-path    PATH example/source-script-in-modulefile/foo-1.2/bin
    set-alias       foo {foobin -q -l}
    setenv          FOOENV arg1

Output could be redirected into a ``foo/1.2`` file and make it the modulefile
to enable software foo::

    $ mkdir -p modulefiles/foo
    $ module sh-to-mod bash example/source-script-in-modulefile/foo-1.2/foo-setup.sh arg1 >modulefiles/foo/1.2
    $ module use ./modulefiles
    $ module show foo
    -------------------------------------------------------------------
    modulefiles/foo/1.2:

    prepend-path    PATH example/source-script-in-modulefile/foo-1.2/bin
    set-alias       foo {foobin -q -l}
    setenv          FOOENV arg1
    -------------------------------------------------------------------

Instead of transforming shell script in modulefile, a modulefile using
:mfcmd:`source-sh` modulefile command to evaluate shell script at modulefile
evaluation time may be written::

    $ cat <<EOF >modulefiles/foo/1.2
    #%Module
    source-sh bash example/source-script-in-modulefile/foo-1.2/foo-setup.sh arg1
    EOF

When displaying a modulefile using ``source-sh`` modulefile command,
modulefile commands resulting from ``source-sh`` evaluation are reported::

    $ module show foo/1.2
    -------------------------------------------------------------------
    modulefiles/foo/1.2:

    prepend-path    PATH example/source-script-in-modulefile/foo-1.2/bin
    set-alias       foo {foobin -q -l}
    setenv          FOOENV arg1
    -------------------------------------------------------------------

Loading this ``foo/1.2`` module will enable access to software *foo*::

    $ module load foo/1.2
    $ alias foo
    alias foo='foobin -q -l'
    $ foo
    foo, version 1.2

Unloading ``foo/1.2`` module will properly revert these environment settings::

    $ module unload foo/1.2
    $ alias foo
    bash: alias: foo: not found
    $ foobin
    bash: foobin: command not found

As conclusion, these new features enable to leverage the setup scripts that
are provided along with software to make them reachable from the ``module``
environment.
