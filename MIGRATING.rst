.. _MIGRATING:

MIGRATING
*********

This document describes the major changes occurring between versions of
Modules. It provides an overview of the new features and changed behaviors
that will be encountered when upgrading.


Migrating from v4.5 to v4.6 (not yet released)
==============================================

This new version is backward-compatible with v4.5 and primarily fixes bugs and
adds new features.

New features
------------

Version 4.6 introduces new functionalities that are described in this section.

sh-to-mod sub-command
^^^^^^^^^^^^^^^^^^^^^

The :subcmd:`sh-to-mod` sub-command is added to output as a modulefile content
the environment changes done by the evaluation of a shell script passed as
argument. :subcmd:`sh-to-mod` is especially useful for software providing a
shell script for their enablement in shell session: it can convert these
scripts into modulefiles.

Say for instance, a *foo* software has been installed and it provides a
``foo-setup.sh`` script to activate *foo* software in user environment::

    $ cat /path/to/foo-1.2/foo-setup.sh
    #!/bin/sh
    export FOOENV="$1"
    export PATH=/path/to/foo-1.2/bin:$PATH
    alias foo='foobin -q -l'

Calling ``module sh-to-mod`` on this shell script outputs the environment
changes it performs as a modulefile content::

    $ module sh-to-mod sh /path/to/foo-1.2/foo-setup.sh arg1
    #%Module
    prepend-path    PATH /path/to/foo-1.2/bin
    set-alias       foo {foobin -q -l}
    setenv          FOOENV arg1

Changes on environment variables, shell aliases, shell functions and current
working directory are tracked. The following shells are supported: *sh*,
*dash*, *csh*, *tcsh*, *bash*, *ksh*, *ksh93*, *zsh* and *fish*.

:subcmd:`sh-to-mod` acts as a full replacement for the standalone
:command:`createmodule.sh` and :command:`createmodule.py` scripts. However
those two scripts are currently still provided for compatibility purpose.

source-sh modulefile command
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :mfcmd:`source-sh` modulefile command is introduced to source environment
changes done by the evaluation of a shell script passed as argument. With
newly introduced :subcmd:`sh-to-mod` sub-command resulting environment changes
done by script are output as modulefile commands. :mfcmd:`source-sh` applies
those modulefile commands as if they were directly written in loading
modulefile.

:mfcmd:`source-sh` is useful for software providing a shell script for their
enablement. If you want to enable such software with :command:`module` yet
using shell script provided by software for this task, just write a modulefile
using :mfcmd:`source-sh` command to call the shell script.

Keeping the same example used to describe :subcmd:`sh-to-mod` sub-command:
*foo* software provides a ``foo-setup.sh`` script for its activation. Create a
modulefile ``foo/1.2`` that calls this script::

    $ cat /path/to/modulefiles/foo/1.2
    #%Module4.6
    source-sh sh /path/to/foo-1.2/foo-setup.sh arg1

Displaying this modulefile indicates the environment changes done by script::

    $ module display foo/1.2
    -------------------------------------------------------------------
    /path/to/modulefiles/foo/1.2:

    prepend-path    PATH /path/to/foo-1.2/bin
    set-alias       foo {foobin -q -l}
    setenv          FOOENV arg1
    -------------------------------------------------------------------

Loading the modulefile applies the environment changes seen above::

    $ module load -v foo/1.2
    Loading foo/1.2
    $ echo $FOOENV
    arg1
    $ alias foo
    alias foo='foobin -q -l'

Track of these changes is kept in user environment to be able to undo them
when modulefile is unloaded::

    $ module unload -v foo/1.2
    Unloading foo/1.2
    $ echo $FOOENV
    
    $ alias foo
    bash: alias: foo: not found

Changes on environment variables, shell aliases, shell functions and current
working directory are tracked. The following shells are supported: *sh*,
*dash*, *csh*, *tcsh*, *bash*, *ksh*, *ksh93*, *zsh* and *fish*.

Querying user's name and groups membership
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Two new sub-commands are introduced for the :mfcmd:`module-info` modulefile
command: ``username`` and ``usergroups``. They respectively fetch the name of
the user currently running :file:`modulecmd.tcl` or the name of all the groups
this user is member of.

These two new modulefile commands can help to adapt code to specific users or
groups. Like for instance to instantiate a modulefile for each group the user
is member of::

    $ cat /path/to/modulefiles/foo/.modulerc
    #%Module4.6
    foreach grp [module-info usergroups] {
        module-virtual foo/$grp .common
    }
    $ id -G -n
    grp1 grp2 grp3
    $ module avail
    --------------- /path/to/modulefiles ---------------
    foo/grp1  foo/grp2  foo/grp3

``username`` and ``usergroups`` sub-commands of :mfcmd:`module-info`
modulefile command are only supported on Unix platform.

Further reading
---------------

To get a complete list of the changes between Modules v4.5 and v4.6,
please read the :ref:`NEWS` document.


Migrating from v4.4 to v4.5
===========================

This new version is backward-compatible with v4.4 and primarily fixes bugs and
adds new features.

New features
------------

Version 4.5 introduces new functionalities that are described in this section.

ml command
^^^^^^^^^^

The ``ml`` command is added to Modules. ``ml`` is a frontend to the ``module``
command that reduces the number of characters to type to trigger module
actions.

With no argument provided ``ml`` is equivalent to ``module list``, ``ml foo``
corresponds to ``module load foo`` and ``ml -foo`` means ``module unload
foo``::

    $ ml foo
    $ ml
    Currently Loaded Modulefiles:
     1) foo/2
    $ ml -foo
    $ ml
    No Modulefiles Currently Loaded.

Multiple modules to either load or unload can be combined on a single command.
The unloads are first processed then the loads.

``ml`` accepts all command-line switches and sub-commands accepted by
``module`` command::

    $ ml avail -t foo
    foo/1
    foo/2

This handy interface has been originally developed by the `Lmod`_ project.
Having this command line interface also supported on Modules helps to provide
a similar user experience whatever the module implementation used.

.. _Lmod: https://github.com/TACC/Lmod

JSON format output
^^^^^^^^^^^^^^^^^^

The ``-j`` and ``--json`` command line switches are added for the ``avail``,
``list``, ``savelist``, ``whatis`` and ``search`` module sub-commands. When
set, the output result of these sub-commands is rendered in `JSON`_ format::

    $ module avail --json bar | python -mjson.tool
    {
        "/path/to/modulefiles": {
            "bar/2.3": {
                "name": "bar/2.3",
                "pathname": "/path/to/modulefiles/bar/2.3",
                "symbols": [
                    "default"
                ],
                "type": "modulefile"
            },
            "bar/3.4": {
                "name": "bar/3.4",
                "pathname": "/path/to/modulefiles/bar/3.4",
                "symbols": [],
                "type": "modulefile"
            }
        }
    }
    $ ml whatis -j foo/1.2.3 | python -mjson.tool
    {
        "/path/to/modulefiles": {
            "foo/1.2.3": {
                "name": "foo/1.2.3",
                "whatis": [
                    "The foo/1.2.3 modulefile"
                ]
            }
        }
    }

.. _JSON: https://tools.ietf.org/html/rfc8259

Improved Windows support
^^^^^^^^^^^^^^^^^^^^^^^^

A new option to the ``./configure`` script named ``--enable-windows-support``
is introduced to install additional files relative to the enablement of
Modules on the Windows platform. When set, this option installs
``module.cmd``, ``ml.cmd`` and ``envml.cmd`` scripts in ``bindir`` and
initialization script ``cmd.cmd`` in ``initdir``. With these four files the
Modules installation may be used from either a Unix or a Windows platform.

``module.cmd``, ``ml.cmd`` and ``envml.cmd`` scripts respectively provide the
``module``, ``ml`` and ``envml`` commands for Windows ``cmd`` terminal shell,
relying on ``modulecmd.tcl`` script which was already able to produce shell
code for this Windows shell. Initialization script ``cmd.cmd`` adds the
directory of ``module.cmd``, ``ml.cmd`` and ``envml.cmd`` to ``PATH``.

These Windows-specific files are relocatable: ``module.cmd``, ``ml.cmd`` and
``envml.cmd`` scripts expect to find initialization script ``cmd.cmd`` in the
``init`` directory next to them (to setup Modules-specific variables in
current environment) and ``cmd.cmd`` expects ``modulecmd.tcl`` to be found in
``libexec`` directory and the 3 commands in ``bin`` directory next to it.

Starting from this ``4.5`` release a distribution zipball is published to
install Modules on Windows. This zip archive ships an install and an uninstall
scripts (``INSTALL.bat`` and ``UNINSTALL.bat``). The zipball can be built
locally from Modules sources by running ``make dist-win``.

The :ref:`INSTALL-win` document describes how to install Modules on Windows
from the distribution zipball.

Error stack trace
^^^^^^^^^^^^^^^^^

Error messages will now embed a stack trace for unknown errors to help
localize the root cause of issues. This change applies to modulefile
evaluation::

    Loading foo/1.2
      Module ERROR: add-path cannot handle path equals to separator string
            while executing
        "append-path PATH :"
            (file "/path/to/modulefiles/foo/1.2" line 24)
        Please contact <root@localhost>

A stack trace is also returned when an unknown error occurs in
``modulecmd.tcl`` script, which facilitates issue report and analysis::

    $ module load bar
    ERROR: invalid command name "badcommand"
          while executing
      "badcommand"
          (procedure "module" line 14)
          invoked from within
      "module load bar"
          ("eval" body line 1)
          invoked from within
      "eval $execcmdlist"
      Please report this issue at https://github.com/cea-hpc/modules/issues

Automatic default and latest symbolic versions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When the implicit default mechanism and the `Advanced module version
specifiers`_ are both enabled, a ``default`` and a ``latest`` symbolic
versions are automatically defined for each module name.

This new feature gives the ability to select the highest version available for
a module, without knowing beforehand this version name::

    $ module load -v foo@latest
    Loading foo/1.10

The symbolic versions are automatically defined unless a symbolic version, an
alias or a regular module version already exists for these ``default`` or
``latest`` version names.

Further reading
---------------

To get a complete list of the changes between Modules v4.4 and v4.5,
please read the :ref:`NEWS` document.


Migrating from v4.3 to v4.4
===========================

This new version is backward-compatible with v4.3 and primarily fixes bugs and
adds new features.

.. warning:: Modules configuration option handling has been reworked
   internally to provide a unified way for all options to get initialized,
   retrieved or set. Existing site-specific configuration script should be
   reviewed to make use of the new ``getConf``, ``setConf``, ``unsetConf``
   and ``lappendConf`` procedures to manipulate configuration options.

New features
------------

Version 4.4 introduces new functionalities that are described in this section.

Specify modules in a case insensitive manner
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ability to match module name in a case insensitive manner has been added.
This feature can be enabled at different level with the following values set
to the ``icase`` configuration option:

* ``never``: a case sensitive match is applied in any cases
* ``search``: a case insensitive match is applied to the ``avail``, ``whatis``
  and ``paths`` sub-commands
* ``always``: a case insensitive match is applied to search contexts and also
  to the other module sub-commands and modulefile Tcl commands for the module
  specification they receive as argument.

It can help for instance to load a module without knowing the case used to
name its relative modulefile::

    $ module config icase always
    $ module load -v mysoftware
    Loading MySoftware/1.0

Insensitive case match activation can be controlled at configure time with
the ``--with-icase`` option, which could be passed any of the above activation
levels. This option could be superseded with the ``MODULES_ICASE`` environment
variable, which could be set through the **config** sub-command with the
``icase`` option. Command-line switch **--icase** supersedes in turns any
other icase configurations. When this command-line switch is passed, ``icase``
mode equals ``always``.

Extended default
^^^^^^^^^^^^^^^^

The extended default mechanism has been introduced to help selecting a module
when only the first numbers in its version are specified. Starting portion of
the version, part separated from the rest of the version string by a ``.``
character, could be used to refer to a more precise version number.

This mechanism is activated through the new configuration option
``extended_default``. It enables to refer to a module named ``foo/1.2.3`` as
``foo/1.2`` or ``foo/1``::

    $ module config extended_default 1
    $ module load -v foo/1
    Loading foo/1.2.3

When multiple versions match partial version specified and only one module
should be selected, the default version (whether implicitly or explicitly
defined) among matches is returned. The following example shows that
``foo/1.1.1``, the *foo* module default version, is selected when it matches
query. Elsewhere the highest version (also called the latest version or the
implicit default) among matching modules is returned::

    $ module av foo
    --------------- /path/to/modulefiles ---------------
    foo/1.1.1(default)  foo/1.2.1  foo/1.10
    foo/1.1.10          foo/1.2.3
    $ module load -v foo/1.1
    Loading foo/1.1.1
    $ module purge
    $ module load -v foo/1.2
    Loading foo/1.2.3
    $ module purge
    $ module load -v foo/1
    Loading foo/1.1.1

In case ``implicit_default`` option is disabled and no explicit default is
found among matches, an error is returned::

    $ module config implicit_default 0
    $ module load -v foo/1.2
    ERROR: No default version defined for 'foo/1.2'

When it is enabled, extended default applies everywhere a module could be
specified, which means it could be used with any module sub-command or any
modulefile Tcl command receiving a module specification as argument. It may
help for instance to declare dependencies between modules::

    $ module show bar/3
    ----------------------------------------------------------
    /path/to/modulefiles/bar/3.4:

    prereq		foo/1.2
    ----------------------------------------------------------
    $ module load --auto bar/3
    Loading bar/3.4
      Loading requirement: foo/1.2.3

Extended default activation can be controlled at configure time with the
``--enable-extended-default`` option. This option could be superseded with the
``MODULES_EXTENDED_DEFAULT`` environment variable, which could be set through
the **config** sub-command with the ``extended_default`` option.

Advanced module version specifiers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ability to specify finer constraints on module version has been added to
Modules. It enables to filter the module selection to a given version list or
range by specifying after the module name a version constraint prefixed by the
``@`` character.

This new feature leverages the `version specifier syntax`_ of the `Spack`_
package manager as this syntax covers all the needs for a fine-grained
selection of module versions. It copes very well with command-line typing, by
avoiding characters having a special meaning on shells. Moreover the users of
Spack that also are users of Modules may already be familiar with this syntax.

.. _version specifier syntax: https://spack.readthedocs.io/en/stable/basic_usage.html#version-specifier
.. _Spack: https://github.com/spack/spack

The mechanism introduced here is called *advanced module version specifier*
and it can be activated through the new configuration option
``advanced_version_spec``. Constraints can be expressed to refine the
selection of module version to:

* a single version with the ``@version`` syntax, for instance ``foo@1.2.3``
  syntax will select module ``foo/1.2.3``
* a list of versions with the ``@version1,version2,...`` syntax, for instance
  ``foo@1.2.3,1.10`` will match modules ``foo/1.2.3`` and ``foo/1.10``
* a range of versions with the ``@version1:``, ``@:version2`` and
  ``@version1:version2`` syntaxes, for instance ``foo@1.2:`` will select all
  versions of module ``foo`` greater than or equal to ``1.2``, ``foo@:1.3``
  will select all versions less than or equal to ``1.3`` and ``foo@1.2:1.3``
  matches all versions between ``1.2`` and ``1.3`` including ``1.2`` and
  ``1.3`` versions

This new feature enables for instance to list available versions of module
``foo`` higher or equal to ``1.2``::

    $ module config advanced_version_spec 1
    $ module av foo
    --------------- /path/to/modulefiles ---------------
    foo/1.1.1(default)  foo/1.2.1  foo/1.10
    foo/1.1.10          foo/1.2.3
    $ module av foo@1.2:
    --------------- /path/to/modulefiles ---------------
    foo/1.2.1  foo/1.2.3  foo/1.10

Then choose to load for instance a version higher than or equal to ``1.2`` and
less than or equal to ``1.3``. Default version is selected if it corresponds
to a version included in the range, elsewhere the highest version (also called
latest version or implicit default) is selected::

    $ module load -v foo@1.2:1.3
    Loading foo/1.2.3

In case ``implicit_default`` option is disabled and no explicit default is
found among version specifier matches, an error is returned::

    $ module config implicit_default 0
    $ module load -v foo@1.2:1.3
    ERROR: No default version defined for 'foo@1.2:1.3'

When advanced module version specifier is enabled, it applies everywhere a
module could be specified, which means it could be used with any module
sub-command or any modulefile Tcl command receiving a module specification
as argument. It may help for instance to declare smoother dependencies between
modules::

    $ module show bar@:2
    ----------------------------------------------------------
    /path/to/modulefiles/bar/2.3:

    prereq          foo@1.1.10,1.2.1
    ----------------------------------------------------------
    $ module load --auto bar@:2
    Loading bar/2.3
      Loading requirement: foo/1.2.1

Advanced specification of single version or list of versions may benefit from
the activation of the `Extended default`_ mechanism (range of versions
natively handles abbreviated versions)::

    $ module config extended_default 1
    $ module load -v foo@1.2
    Loading foo/1.2.3
    $ module unload -v foo @1.2,1.5
    Unloading foo/1.2.3

Advanced module version specifier activation can be controlled at configure
time with the ``--enable-advanced-version-spec`` option. This option could be
superseded with the ``MODULES_ADVANCED_VERSION_SPEC`` environment variable,
which could be set through the **config** sub-command with the
``advanced_version_spec`` option.

Further reading
---------------

To get a complete list of the changes between Modules v4.3 and v4.4,
please read the :ref:`NEWS` document.


Migrating from v4.2 to v4.3
===========================

This new version is backward-compatible with v4.2 and primarily fixes bugs and
adds new features.

New features
------------

Version 4.3 introduces new functionalities that are described in this section.

Modulepath rc file
^^^^^^^^^^^^^^^^^^

A ``.modulerc`` file found at the root of an enabled modulepath directory is
now evaluated when modulepath is walked through to locate modulefiles. This
modulepath rc file gives for instance the ability to define module alias whose
name does not correspond to any module directory in this modulepath. Thus this
kind of module alias would not be found unless if it is defined at the
modulepath global scope.

Further I/O operations optimization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Additional work has been performed to save a significant number of filesystem
I/O operations made to search and evaluate modulefiles.

When fully read, the content of a modulefile is now cached in memory to avoid
new I/O operations in case this modulefile should be read one more time during
the same module command evaluation.

Except for ``path``, ``paths``, ``list``, ``avail`` and ``aliases`` module
commands always fully read a modulefile whether its full content is needed or
just its header to verify its validity. This way modulefiles are only read
once on commands that first check modulefile validity then read again valid
files to get their full content.

Last but not least, Modules Tcl extension library is introduced to extend the
Tcl language in order to provide more optimized I/O commands to read a file or
a directory content than native Tcl commands do. This library is built and
enabled in ``modulecmd.tcl`` script with ``--enable-libtclenvmodules``
configure argument (it is enabled by default). As this library is written in
C, it must be compiled and ``--with-tcl`` or ``--with-tclinclude`` configure
arguments may be used to indicate where to find Tcl development files.

Modules Tcl extension library greatly reduces the number of filesystem I/O
operations by removing unneeded ``ioctl``, ``fcntl`` and ``lstat`` system
calls done (by Tcl ``open`` command) to read each modulefile. Directory
content read is also improved by fetching hidden and regular files in one
pass. Moreover ``.modulerc`` and ``.version`` read access is tested only if
these files are found in the directory.

Colored output
^^^^^^^^^^^^^^

The ability to graphically enhance some part of the produced output has been
added to improve readability. Among others, error, warning and info message
prefixes can be colored as well as modulepath, module alias and symbolic
version.

Color mode can be set to ``never``, ``auto`` or ``always``. When color mode is
set to ``auto``, output is colored only if the standard error output channel
is attached to a terminal.

Default color mode could be controlled at configure time with the
``--enable-color`` and the ``--disable-color`` option, which respectively
correspond to the ``auto`` and ``never`` color mode. This default mode could
be superseded with the ``CLICOLOR``, ``CLICOLOR_FORCE`` and ``MODULES_COLOR``
environment variables and the ``--color`` command-line switch.

Color to apply to each element can be controlled with the ``MODULES_COLORS``
environment variable or the ``--with-dark-background-colors`` and
``--with-light-background-colors`` configure options. These variable and
options take as value a colon-separated list in the same fashion ``LS_COLORS``
does. In this list, output item that should be highlighted is designated by
a key which is associated to a `Select Graphic Rendition (SGR) code`_.

.. _Select Graphic Rendition (SGR) code: https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters

The ``MODULES_TERM_BACKGROUND`` environment variable and the
``--with-terminal-background`` configure option help Modules to determine if
the color set for dark background or the color set for light background should
be used to color output in case no specific color set is defined with the
``MODULES_COLORS`` variable.

Output items able to be colorized and their relative key are: highlighted
element (``hi``), debug information (``db``), tag separator (``se``); Error
(``er``), warning (``wa``), module error (``me``) and info (``in``) message
prefixes; Modulepath (``mp``), directory (``di``), module alias (``al``),
module symbolic version (``sy``), module ``default`` version (``de``) and
modulefile command (``cm``).

For instance the default color set for a terminal with dark background is
defined to::

    hi=1:db=2:se=2:er=91:wa=93:me=95:in=94:mp=1;94:di=94:al=96:sy=95:de=4:cm=92

When colored output is enabled and a specific graphical rendition is defined
for module *default* version, the ``default`` symbol is omitted and instead
the defined graphical rendition is applied to the relative modulefile. When
colored output is enabled and a specific graphical rendition is defined for
module alias, the ``@`` symbol is omitted.

``CLICOLOR`` and ``CLICOLOR_FORCE`` environment variables are also honored to
define color mode. The ``never`` mode is set if ``CLICOLOR`` equals to ``0``.
If ``CLICOLOR`` is set to another value, it corresponds to the ``auto`` mode.
The ``always`` mode is set if ``CLICOLOR_FORCE`` is set to a value different
than ``0``. Color mode set with these two variables is superseded by mode set
with ``MODULES_COLOR`` environment variable.

Configure modulecmd with config sub-command
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The **config** sub-command has been added to ``module`` to help getting or
setting the **modulecmd.tcl** options. With no additional command-line
argument, this sub-command reports the current value of all existing options
with a mention to indicate if this value has been overridden from a
command-line switch or from an environment variable.

See the description of this sub-command in the :ref:`module(1)` man page for a
complete reference on existing configuration options.

Most of the options can be altered by passing the option name and a value to
the sub-command. Setting an option by this mean overrides its default value,
set at installation time in **modulecmd.tcl** script, by defining the
environment variable which supersedes this default.::

    $ module config auto_handling 1
    $ module config auto_handling
    Modules Release 4.3.0 (2019-07-26)
    
    - Config. name ---------.- Value (set by if default overridden) ---------------
    auto_handling             1 (env-var)

Setting options with ``module config`` could be done in the Modules
initialization RC file to change default value of options when ``module``
command is initialized.

When command-line switch ``--reset`` and an option name is passed to the
**config** sub-command, it restores default value for configuration option by
unsetting related environment variable.

With command-line switch ``--dump-state``, the **config** sub-command reports,
in addition to currently set options, the current state of **modulecmd.tcl**
script and Modules-related environment variables. Providing the output of the
``module config --dump-state`` command when submitting an issue to the Modules
project will help to analyze the situation.

Control module command verbosity
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ability to control message verbosity has been added so ``module`` command
can be configured whether it should display more or less information.
Available verbosity levels from the least to the most verbose are:

* ``silent``: turn off error, warning and informational messages but does not
  affect module command output result.
* ``concise``: enable error and warning messages but disable informational
  messages.
* ``normal``: turn on informational messages, like a report of the additional
  module evaluations triggered by loading or unloading modules, aborted
  evaluation issues or a report of each module evaluation occurring during a
  **restore** or **source** sub-commands.
* ``verbose``: add additional informational messages, like a systematic report
  of the loading or unloading module evaluations.
* ``debug``: print debugging messages about module command execution.

Default verbosity level can be controlled at configure time with the
``--with-verbosity`` option, which could be passed any of the above level
names. This default verbosity level could be superseded with the
``MODULES_VERBOSITY`` environment variable, which could be set through the
**config** sub-command with the ``verbosity`` option. Command-line switches
**--silent**, **--verbose** and **--debug** supersede in turns any other
verbosity configuration to respectively set module command silent, verbose or
in debug mode.

Other new sub-commands, command-line switches and environment variables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* The **avail** sub-command gets two new command-line switches: **--indepth**
  and **--no-indepth**. These options control whether search results should
  recursively include or not modulefiles from directories matching search
  query. Shell completion scripts have been updated to complete available
  modulefiles in the no in depth mode.

* The **MODULES_AVAIL_INDEPTH** environment variable defines if the **avail**
  sub-command should include or exclude by default the modulefiles from
  directories matching search query. Its value is superseded by the use of the
  **--indepth** and **--no-indepth** command-line switches.

* The **clear** sub-command, which was available on Modules version 3.2, has
  been reintroduced. This sub-command resets the Modules runtime information
  but does not apply further changes to the environment at all. This
  sub-command now leverages the **--force** command-line switch to skip its
  confirmation dialog.

* The **MODULES_SITECONFIG** environment variable defines an additional
  siteconfig script which is loaded if it exists after the siteconfig script
  configured at build time in ``modulecmd.tcl``. This ability is enabled by
  default and could be disabled with configure option
  ``--with-locked-configs=extra_siteconfig``.

* The **MODULES_UNLOAD_MATCH_ORDER** environment variable sets whether the
  firstly or the lastly loaded module should be selected for unload when
  multiple loaded modules match unload request. Configure option
  ``--with-unload-match-order`` defines this setting which can be superseded
  by the environment variable. By default, lastly loaded module is selected
  and it is recommended to keep this behavior when used modulefiles express
  dependencies between each other.

* The **MODULES_IMPLICIT_DEFAULT** environment variable sets whether an
  implicit default version should be defined for modules with no default
  version explicitly defined. When enabled, which is the default behavior, a
  module version is automatically selected (latest one) when the generic
  name of the module is passed. When implicit default is disabled and no
  default version is explicitly defined for a module, the name of this module
  to evaluate should be fully qualified elsewhere an error is returned.
  Configure option ``--enable-implicit-default`` defines this setting which
  can be superseded by the environment variable. This superseding mechanism
  can be disabled with configure option
  ``--with-locked-configs=implicit_default``.

* The **MODULES_SEARCH_MATCH** environment variable defines the matching style
  to perform when searching for available modules. With **starts_with** value,
  modules whose name begins by search query string are returned. When search
  match style is set to **contains**, modules returned are those whose fully
  qualified name contains search query string. Configure option
  ``--with-search-match`` defines this setting which can be superseded by the
  environment variable, which in turns can be superseded by the
  **--starts-with** and **--contains** command-line switches of **avail**
  module sub-command.

* The **MODULES_SET_SHELL_STARTUP** environment variable controls whether or
  not shell startup file should be set to ensure ``module`` command is defined
  once shell has been initialized. When enabled, the ``ENV`` and ``BASH_ENV``
  environment variables are set, when ``module`` function is defined, to the
  Modules bourne shell initialization script. Configure options
  ``--enable-set-shell-startup`` and ``--disable-set-shell-startup`` define
  this setting which can be superseded by the environment variable.

* When initializing the ``module`` command in a shell session, initialization
  configuration files stored in the defined configuration directory are taken
  into account if present instead of the configuration files stored in the
  initialization script directory. When they are stored in the configuration
  directory, these configuration files are named ``initrc`` and
  ``modulespath`` instead of respectively ``modulerc`` and ``.modulespath``.
  The location of the installation of those files can be controlled with
  configure option ``--with-initconf-in``, which accepts ``etcdir`` and
  ``initdir`` values.

* The **MODULES_WA_277** environment variable helps to define an alternative
  ``module`` alias on Tcsh shell when set to *1*. It workarounds an issue on
  Tcsh history mechanism occurring with default ``module`` command alias:
  erroneous history entries are recorded each time the ``module`` command is
  called. However the alternative definition of the module alias weakens shell
  evaluation of the code produced by modulefiles. Characters with special
  meaning for Tcsh shell (like *{* and *}*) may not be used anymore in shell
  alias definition elsewhere the evaluation of the code produced by
  modulefiles will return a syntax error.


Further reading
---------------

To get a complete list of the changes between Modules v4.2 and v4.3,
please read the :ref:`NEWS` document.


Migrating from v4.1 to v4.2
===========================

This new version is backward-compatible with v4.1 and primarily fixes bugs and
adds new features.

New features
------------

Version 4.2 introduces new functionalities that are described in this section.

.. _v42-conflict-constraints-consistency:

Modulefile conflict constraints consistency
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With the **conflict** modulefile command, a given modulefile can list the
other modulefiles it conflicts with. To load this modulefile, the modulefiles
it conflicts with cannot be loaded.

This constraint was until now satisfied when loading the modulefile declaring
the **conflict** but it vanished as soon as this modulefile was loaded. In the
following example ``a`` modulefile declares a conflict with ``b``::

    $ module load b a
    WARNING: a cannot be loaded due to a conflict.
    HINT: Might try "module unload b" first.
    $ module list
    Currently Loaded Modulefiles:
     1) b
    $ module purge
    $ module load a b
    $ module list
    Currently Loaded Modulefiles:
     1) a   2) b

Consistency of the declared **conflict** is now ensured to satisfy this
constraint even after the load of the modulefile declaring it. This is
achieved by keeping track of the conflict constraints of the loaded
modulefiles in an environment variable called ``MODULES_LMCONFLICT``::

    $ module load a b
    ERROR: WARNING: b cannot be loaded due to a conflict.
    HINT: Might try "module unload a" first.
    $ module list
    Currently Loaded Modulefiles:
     1) a

An environment variable is used to keep track of this conflict information to
proceed the same way than used to keep track of the loaded modulefiles with
the ``LOADEDMODULES`` environment variable.

In case a conflict constraint toward a modulefile is set by an already loaded
modulefile, loading the conflicting modulefile will lead to a load evaluation
attempt in order for this modulefile to get the chance to solve the constraint
violation. If at the end of the load evaluation, the conflict has not been
solved, modulefile load will be discarded.

.. warning:: On versions ``4.2.0`` and ``4.2.1``, a conflict constraint set by
   an already loaded modulefile forbade the load of the conflicting
   modulefile. This has been changed starting version ``4.2.2`` to better cope
   with behaviors of previous Modules version: an evaluation attempt of the
   conflicting modulefile is made to give it the opportunity to solve this
   conflict by using **module unload** modulefile command.

.. _v42-prereq-constraints-consistency:

Modulefile prereq constraints consistency
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With the **prereq** modulefile command, a given modulefile can list the
other modulefiles it pre-requires. To load this modulefile, the modulefiles it
pre-requires must be loaded prior its own load.

This constraint was until now satisfied when loading the modulefile declaring
the **prereq** but, as for the declared **conflict**, it vanished as soon as
this modulefile was loaded. In the following example ``c`` modulefile declares
a prereq on ``a``::

    $ module load c
    WARNING: c cannot be loaded due to missing prereq.
    HINT: the following module must be loaded first: a
    $ module list
    No Modulefiles Currently Loaded.
    $ module load a c
    $ module list
    Currently Loaded Modulefiles:
     1) a   2) c
    $ module unload a
    $ module list
    Currently Loaded Modulefiles:
     1) c

Consistency of the declared **prereq** is now ensured to satisfy this
constraint even after the load of the modulefile declaring it. This is
achieved, like for the conflict consistency, by keeping track of the prereq
constraints of the loaded modulefiles in an environment variable called
``MODULES_LMPREREQ``::

    $ module load a c
    $ module list
    Currently Loaded Modulefiles:
     1) a   2) c
    $ module unload a
    ERROR: WARNING: a cannot be unloaded due to a prereq.
    HINT: Might try "module unload c" first.
    $ module list
    Currently Loaded Modulefiles:
     1) a   2) c

.. _v42-by-passing-module-constraints:

By-passing module defined constraints
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ability to by-pass a **conflict** or a **prereq** constraint defined by
modulefiles is introduced with the ``--force`` command line switch (``-f`` for
short notation) for the **load**, **unload** and **switch** sub-commands.

With this new command line switch, a given modulefile is loaded even if it
conflicts with other loaded modulefiles or even if the modulefiles it
pre-requires are not loaded. Some example reusing the same modulefiles ``a``,
``b`` and ``c`` than above::

    $ module load b
    $ module load --force a
    WARNING: a conflicts with b
    $ module list
    Currently Loaded Modulefiles:
     1) b   2) a
    $ module purge
    $ module load --force c
    WARNING: c requires a loaded
    $ module list
    Currently Loaded Modulefiles:
     1) c

``--force`` also enables to unload a modulefile required by another loaded
modulefiles::

    $ module load a c
    $ module list
    Currently Loaded Modulefiles:
     1) a   2) c
    $ module unload --force a
    WARNING: a is required by c
    $ module list
    Currently Loaded Modulefiles:
     1) c

In a situation where some of the loaded modulefiles have unsatisfied
constraints corresponding to the **prereq** and **conflict** they declare, the
**save** and **reload** sub-commands do not perform and return an error.

.. _v42-automated-module-handling-mode:

Automated module handling mode
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An automatic management of the dependencies between modulefiles has been added
and it is called *automated module handling mode*. This new mode consists in
additional actions triggered when loading or unloading a modulefile to satisfy
the constraints it declares.

When loading a modulefile, following actions are triggered:

* Requirement Load (ReqLo): load of the modulefiles declared as a **prereq**
  of the loading modulefile.

* Dependent Reload (DepRe): reload of the modulefiles declaring a **prereq**
  onto loaded modulefile or declaring a **prereq** onto a modulefile part of
  this reloading batch.

When unloading a modulefile, following actions are triggered:

* Dependent Unload (DepUn): unload of the modulefiles declaring a non-optional
  **prereq** onto unloaded modulefile or declaring a non-optional **prereq**
  onto a modulefile part of this unloading batch. A **prereq** modulefile is
  considered optional if the **prereq** definition order is made of multiple
  modulefiles and at least one alternative modulefile is loaded.

* Useless Requirement Unload (UReqUn): unload of the **prereq** modulefiles
  that have been automatically loaded for either the unloaded modulefile, an
  unloaded dependent modulefile or a modulefile part of this useless
  requirement unloading batch. Modulefiles are added to this unloading batch
  only if they are not required by any other loaded modulefiles.
  ``MODULES_LMNOTUASKED`` environment variable helps to keep track of these
  automatically loaded modulefiles and to distinguish them from modulefiles
  asked by user.

* Dependent Reload (DepRe): reload of the modulefiles declaring a **conflict**
  or an optional **prereq** onto either the unloaded modulefile, an unloaded
  dependent or an unloaded useless requirement or declaring a **prereq** onto
  a modulefile part of this reloading batch.

In case a loaded modulefile has some of its declared constraints unsatisfied
(pre-required modulefile not loaded or conflicting modulefile loaded for
instance), this loaded modulefile is excluded from the automatic reload
actions described above.

For the specific case of the **switch** sub-command, where a modulefile is
unloaded to then load another modulefile. Dependent modulefiles to Unload are
merged into the Dependent modulefiles to Reload that are reloaded after the
load of the switched-to modulefile.

This automated module handling mode integrates concepts (like the Dependent
Reload mechanism) of the Flavours_ extension, which was designed for Modules
compatibility version. As a whole, automated module handling mode can be seen
as a generalization and as an expansion of the Flavours_ concepts.

.. _Flavours: https://sourceforge.net/projects/flavours/

This new feature can be controlled at build time with the
``--enable-auto-handling`` configure option. This default configuration can be
superseded at run-time with the ``MODULES_AUTO_HANDLING`` environment variable
or the command line switches ``--auto`` and ``--no-auto``.

By default, automated module handling mode is disabled and will stay so until
the next major release version (5.0) where it will be enabled by default. This
new feature is currently considered experimental and the set of triggered
actions will be refined over the next feature releases.

.. _v42-consistency-module-load-unload-commands:

Consistency of module load/unload commands in modulefile
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With the **module load** modulefile command, a given modulefile can
automatically load a modulefile it pre-requires. Similarly with the **module
unload** modulefile command, a given modulefile can automatically unload a
modulefile it conflicts with.

Both commands imply additional actions on the loaded environment (loading or
unloading extra modulefiles) that should cope with the constraints defined by
the loaded environment.

Additionally **module load** and **module unload** modulefile commands express
themselves constraints on loaded environment that should stay satisfied to
ensure consistency.

To ensure the consistency of **module load** modulefile command once the
modulefile defining it has been loaded, this command is assimilated to a
**prereq** command. Thus the defined constraint is recorded in the
``MODULES_LMPREREQ`` environment variable. Same approach is used for **module
unload** modulefile command which is assimilated to a **conflict** command.
Thus the defined constraint is recorded in the ``MODULES_LMCONFLICT``
environment variable.

To ensure the consistency of the loaded environment, the additional actions of
the **module load** and **module unload** modulefile commands have been
adapted in particular situations:

* When unloading modulefile, **module load** command will unload the
  modulefile it targets only if no other loaded modulefile requires it and if
  this target has not been explicitly loaded by user.

* When unloading modulefile, **module unload** command does nothing as the
  relative conflict registered at load time ensure environment consistency and
  will forbid conflicting modulefile load.

Please note that loading and unloading results may differ than from previous
Modules version now that consistency is checked:

* Modulefile targeted by a **module load** modulefile command may not be able
  to load due to a registered conflict in the currently loaded environment.
  Which in turn will break the load of the modulefile declaring the **module
  load** command.

* Modulefile targeted by a **module unload** modulefile command may not be
  able to unload due to a registered prereq in the loaded environment. Which
  in turn will break the load of the modulefile declaring the **module
  unload** command.

* If automated module handling mode is enabled, **module load** modulefile
  command is interpreted when unloading modulefile as part of the Useless
  Requirement Unload (UReqUn) mechanism not through modulefile evaluation.
  As a consequence, an error occurring when unloading the modulefile targeted
  by the **module load** command does not break the unload of the modulefile
  declaring this command. Moreover unload of the **module load** targets is
  done in the reverse loaded order, not in the **module load** command
  definition order.

.. _v42-alias-symbolic-name-consistency:

Modulefile alias and symbolic modulefile name consistency
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With the **module-alias** and **module-version** modulefile commands,
alternative names can be given to a modulefile. When these names are used to
load for instance a modulefile, they are resolved to the modulefile they
target which is then processed for the load action.

Until now, the alias and symbolic version names were correctly resolved for
the **load** and **unload** actions and also for the querying sub-commands
(like **avail** or **whatis**). However this alternative name information
vanishes once the modulefile it resolves to is loaded. As a consequence there
was no consistency over these alternative designations. In the following
example ``f`` modulefile declares a conflict on ``e`` alias which resolves to
``d`` modulefile::

    $ module load e
    $ module list
    Currently Loaded Modulefiles:
     1) d
    $ module info-loaded e
    $ module load f
    $ module list
    Currently Loaded Modulefiles:
     1) d   2) f

Consistency of the alternative names set on a modulefile with **module-alias**
and **module-version** commands is now ensured to enable modulefile commands
**prereq**, **conflict**, **is-loaded** and **module-info loaded** using these
alternative designations as argument. This consistency is achieved, like for
the conflict and prereq consistencies, by keeping track of the alternative
names of the loaded modulefiles in an environment variable called
``MODULES_LMALTNAME``::

    $ module load e
    $ module list
    Currently Loaded Modulefiles:
     1) d
    $ module info-loaded e
    d
    $ module load f
    WARNING: f cannot be loaded due to a conflict.
    HINT: Might try "module unload e" first.
    $ module list
    Currently Loaded Modulefiles:
     1) d

.. _v42-variable-change-through-modulefile-evaluation:

Environment variable change through modulefile evaluation context
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All environment variable edition commands (``setenv``, ``unsetenv``,
``append-path``, ``prepend-path`` and ``remove-path``) have been updated to:

* Reflect environment variable value change on the environment of the current
  modulefile Tcl interpreter. So using ``$env(VAR)`` will return the currently
  defined value for environment variable ``VAR``, not the one found prior
  modulefile evaluation.
* Clear environment variable content instead of unsetting it on the
  environment of the current modulefile Tcl interpreter to avoid raising
  error about accessing an undefined element in ``$env()``. Code is still
  produced to purely unset environment variable in shell environment.

Exception is made for the ``whatis`` evaluation mode: environment variables
targeted by variable edition commands are not set to the defined value in the
evaluation context during this ``whatis`` evaluation. These variables are
only initialized to an empty value if undefined. This exception is made to
save performances on this global evaluation mode.

.. _v42-versioned-magic-cookie:

Express Modules compatibility of modulefile with versioned magic cookie
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Any modulefile should start with the ``#%Module`` magic cookie and sometimes
a version number may be placed right after this string. Until now this
version number corresponded to a modulefile format version but it was never
checked.

Starting with this new Modules release, this version number reflects the
minimum version of Modules required to interpret the modulefile. If the
version number is set along the magic cookie string it is now checked and the
modulefile is interpreted only if Modules version is greater or equal to this
version number. For instance, if a modulefile begins with the ``#%Module4.3``
string, it can only be evaluated by Modules version 4.3 and above. Elsewhere
the modulefile is ignored like files without the ``#%Module`` magic cookie
set.

.. _v42-module-message-report:

Improved module message report
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Module sub-commands like ``load``, ``unload`` or ``switch``, may perform
multiple load or unload modulefile evaluations in a row. Also these kind of
evaluation modes may sometimes trigger additional load or unload evaluations,
when for instance a modulefile contains a ``module load`` command.

To improve the readability of the module messages produced relatively to
a load or an unload evaluation, these messages are now stacked under a
*Loading* or an *Unloading* message block that gathers all the messages
produced for a given modulefile evaluation::

    $ module load --no-auto foo
    Loading foo/1.2
      ERROR: foo/1.2 cannot be loaded due to missing prereq.
        HINT: the following module must be loaded first: bar/4.5

In addition, foreground ``load``, ``unload``, ``switch`` and ``restore``
actions (ie. asked on the command-line) now report a summary of the
additional load and unload evaluations that were eventually triggered in
the process::

    $ module load --auto foo
    Loading foo/1.2
      Loading requirement: bar/4.5

New modulefile commands
^^^^^^^^^^^^^^^^^^^^^^^

2 new modulefile Tcl commands have been introduced:

* **set-function**: define a shell function on sh-kind and fish shells.
* **unset-function**: unset a shell function on sh-kind and fish shells.

Further reading
---------------

To get a complete list of the changes between Modules v4.1 and v4.2,
please read the :ref:`NEWS` document.


Migrating from v4.0 to v4.1
===========================

This new version is backward-compatible with v4.0 and primarily fixes bugs and
adds new features.

New features
------------

Version 4.1 introduces a bunch of new functionalities. These major new
features are described in this section.

Virtual modules
^^^^^^^^^^^^^^^

A virtual module stands for a module name associated to a modulefile. The
modulefile is the script interpreted when loading or unloading the virtual
module which appears or can be found with its virtual name.

The **module-virtual** modulefile command is introduced to give the ability
to define these virtual modules. This new command takes a module name as
first argument and a modulefile location as second argument::

    module-virtual app/1.2.3 /path/to/virtualmod/app

With this feature it is now possible to dynamically define modulefiles
depending on the context.

Extend module command with site-specific Tcl code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``module`` command can now be extended with site-specific Tcl
code. ``modulecmd.tcl`` now looks at a **siteconfig.tcl** file in an
``etcdir`` defined at configure time (by default ``$prefix/etc``). If
it finds this Tcl script file, it is sourced within ``modulecmd.tcl`` at the
beginning of the main procedure code.

``siteconfig.tcl`` enables to supersede any global variable or procedure
definitions made in ``modulecmd.tcl`` with site-specific code. A module
sub-command can for instance be redefined to make it fit local needs
without having to touch the main ``modulecmd.tcl``.

Quarantine mechanism to protect module execution
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To protect the module command run-time environment from side effect
coming from the current environment definition a quarantine mechanism
is introduced. This mechanism, sets within module function definition
and shell initialization script, modifies the ``modulecmd.tcl`` run-time
environment to sanitize it.

The mechanism is piloted by environment variables. First of all
``MODULES_RUN_QUARANTINE``, a space-separated list of environment variable
names. Every variable found in ``MODULES_RUN_QUARANTINE`` will be set in
quarantine during the ``modulecmd.tcl`` run-time. Their value will be set
empty or set to the value of the corresponding ``MODULES_RUNENV_<VAR>``
environment variable if defined. Once ``modulecmd.tcl`` is started it
restores quarantine variables to their original values.

``MODULES_RUN_QUARANTINE`` and ``MODULES_RUNENV_<VAR>`` environment variables
can be defined at build time by using the following configure option::

    --with-quarantine-vars='VARNAME[=VALUE] ...'

Quarantine mechanism is available for all supported shells except ``csh``
and ``tcsh``.

Pager support
^^^^^^^^^^^^^

The informational messages Modules sends on the *stderr* channel may
sometimes be quite long. This is especially the case for the avail
sub-command when hundreds of modulefiles are handled. To improve the
readability of those messages, *stderr* output can now be piped into a
paging command.

This new feature can be controlled at build time with the ``--with-pager``
and ``--with-pager-opts`` configure options. Default pager command is set
to ``less`` and its relative options are by default ``-eFKRX``. Default
configuration can be supersedes at run-time with ``MODULES_PAGER`` environment
variables or command-line switches (``--no-pager``, ``--paginate``).

.. warning:: On version ``4.1.0``, the ``PAGER`` environment variable was
   taken in consideration to supersede pager configuration at run-time. Since
   version ``4.1.1``, ``PAGER`` environment variable is ignored to avoid side
   effects coming from the system general pager configuration.

Module function to return value in scripting languages
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On Tcl, Perl, Python, Ruby, CMake and R scripting shells, module function
was not returning value and until now an occurred error led to raising a
fatal exception.

To make ``module`` function more friendly to use on these scripting shells
it now returns a value. False in case of error, true if everything goes well.

As a consequence, returned value of a module sub-command can be checked. For
instance in Python::

    if module('load', 'foo'):
      # success
    else:
      # failure

New modulefile commands
^^^^^^^^^^^^^^^^^^^^^^^

4 new modulefile Tcl commands have been introduced:

* **is-saved**: returns true or false whether a collection, corresponding to
  currently set collection target, exists or not.
* **is-used**: returns true or false whether a given directory is currently
  enabled in ``MODULEPATH``.
* **is-avail**: returns true or false whether a given modulefile exists in
  currently enabled module paths.
* **module-info loaded**: returns the exact name of the modulefile currently
  loaded corresponding to the name argument.

Multiple collections, paths or modulefiles can be passed respectively to
``is-saved``, ``is-used`` and ``is-avail`` in which case true is returned if
at least one argument matches condition (acts as a OR boolean operation). No
argument may be passed to ``is-loaded``, ``is-saved`` and ``is-used``
commands to return if anything is respectively loaded, saved or used.

If no loaded modulefile matches the ``module-info loaded`` query, an empty
string is returned.

New module sub-commands
^^^^^^^^^^^^^^^^^^^^^^^

Modulefile-specific commands are sometimes wished to be used outside of a
modulefile context. Especially for the commands managing path variables
or commands querying current environment context. So the following
modulefile-specific commands have been made reachable as module sub-commands
with same arguments and properties as if called from within a modulefile:

* **append-path**
* **prepend-path**
* **remove-path**
* **is-loaded**
* **info-loaded**

The ``is-loaded`` sub-command returns a boolean value. Small Python example::

    if module('is-loaded', 'app'):
      print 'app is loaded'
    else:
      print 'app not loaded'

``info-loaded`` returns a string value and is the sub-command counterpart
of the ``module-info loaded`` modulefile command::

    $ module load app/0.8
    $ module info-loaded app
    app/0.8

Further reading
---------------

To get a complete list of the changes between Modules v4.0 and v4.1,
please read the :ref:`NEWS` document.


Migrating from v3.2 to v4.0
===========================

Major evolution occurs with this v4.0 release as the traditional *module*
command implemented in C is replaced by the native Tcl version. This full
Tcl rewrite of the Modules package was started in 2002 and has now reached
maturity to take over the binary version. This flavor change enables to
refine and push forward the *module* concept.

This document provides an outlook of what is changing when migrating from
v3.2 to v4.0 by first describing the introduced new features. Both v3.2
and v4.0 are quite similar and transition to the new major version should
be smooth. Slights differences may be noticed in a few use-cases. So the
second part of the document will help to learn about them by listing the
features that have been discontinued in this new major release or the
features where a behavior change can be noticed.

New features
------------

On its overall this major release brings a lot more robustness to the
*module* command with now more than 4000 non-regression tests crafted
to ensure correct operations over the time. This version 4.0 also comes
with fair amount of improved functionalities. The major new features are
described in this section.

Additional shells supported
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Modules v4 introduces support for **fish**, **lisp**, **tcl** and **R**
code output.

Non-zero exit code in case of error
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All module sub-commands will now return a non-zero exit code in case of error
whereas Modules v3.2 always returned zero exit code even if issue occurred.

Output redirect
^^^^^^^^^^^^^^^

Traditionally the *module* command output text that should be seen by the
user on *stderr* since shell commands are output to *stdout* to change
shell's environment. Now on *sh*, *bash*, *ksh*, *zsh* and *fish* shells,
output text is redirected to *stdout* after shell command evaluation if
shell is in interactive mode.

Filtering avail output
^^^^^^^^^^^^^^^^^^^^^^

Results obtained from the **avail** sub-command can now be filtered to only
get the default version of each module name with use of the **--default**
or **-d** command line switch. Default version is either the explicitly
set default version or the highest numerically sorted modulefile or module
alias if no default version set.

It is also possible to filter results to only get the highest numerically
sorted version of each module name with use of the **--latest** or **-L**
command line switch.

Extended support for module alias and symbolic version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Module aliases are now included in the result of the **avail**, **whatis**
and **apropos** sub-commands. They are displayed in the module path
section where they are defined or in a *global/user modulerc* section for
aliases set in user's or global ``modulerc`` file. A **@** symbol is added
in parenthesis next to their name to distinguish them from modulefiles.

Search may be performed with an alias or a symbolic version-name passed
as argument on **avail**, **whatis** and **apropos** sub-commands.

Modules v4 resolves module alias or symbolic version passed to **unload**
command to then remove the loaded modulefile pointed by the mentioned
alias or symbolic version.

A symbolic version sets on a module alias is now propagated toward the
resolution path to also apply to the relative modulefile if it still
correspond to the same module name.

Hiding modulefiles
^^^^^^^^^^^^^^^^^^

Visibility of modulefiles can be adapted by use of file mode bits or file
ownership. If a modulefile should only be used by a given subset of persons,
its mode an ownership can be tailored to provide read rights to this group of
people only. In this situation, module only reports the modulefile, during an
**avail** command for instance, if this modulefile can be read by the current
user.

These hidden modulefiles are simply ignored when walking through the
modulepath content. Access issues (permission denied) occur only when trying
to access directly a hidden modulefile or when accessing a symbol or an alias
targeting a hidden modulefile.

Improved modulefiles location
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When looking for an implicit default in a modulefile directory, aliases
are now taken into account in addition to modulefiles and directories to
determine the highest numerically sorted element.

Modules v4 resolves module alias or symbolic version when it points to a
modulefile located in another modulepath.

Access issues (permission denied) are now distinguished from find issues
(cannot locate) when trying to access directly a directory or a modulefile
as done on **load**, **display** or **whatis** commands. In addition,
on this kind of access not readable ``.modulerc`` or ``.version`` files are
ignored rather producing a missing magic cookie error.

Module collection
^^^^^^^^^^^^^^^^^

Modules v4 introduces support for module *collections*. Collections
describe a sequence of **module use** then **module load** commands that
are interpreted by Modules to set the user environment as described by this
sequence. When a collection is activated, with the **restore** sub-command,
modulepaths and loaded modules are unused or unloaded if they are not part
or if they are not ordered the same way as in the collection.

Collections are generated by the **save** sub-command that dumps the current
user environment state in terms of modulepaths and loaded modules. By default
collections are saved under the ``$HOME/.module`` directory. Collections
can be listed with **savelist** sub-command, displayed with **saveshow**
and removed with **saverm**.

Collections may be valid for a given target if they are suffixed. In this
case these collections can only be restored if their suffix correspond
to the current value of the ``MODULES_COLLECTION_TARGET`` environment
variable. Saving collection registers the target footprint by suffixing
the collection filename with ``.$MODULES_COLLECTION_TARGET``.

Path variable element counter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Modules 4 provides path element counting feature which increases a
reference counter each time a given path entry is added to a given
path-like environment variable. As consequence a path entry element is
removed from a path-like variable only if the related element counter is
equal to 1. If this counter is greater than 1, path element is kept in
variable and reference counter is decreased by 1.

This feature allows shared usage of particular path elements. For instance,
modulefiles can append ``/usr/local/bin`` to ``PATH``, which is not unloaded
until all the modulefiles that loaded it unload too.

Optimized I/O operations
^^^^^^^^^^^^^^^^^^^^^^^^

Substantial work has been done to reduce the number of I/O operations
done during global modulefile analysis commands like **avail** or
**whatis**. ``stat``, ``open``, ``read`` and ``close`` I/O operations have
been cut down to the minimum required when walking through the modulepath
directories to check if files are modulefiles or to resolve module aliases.

Interpretation of modulefiles and modulerc are handled by the minimum
required Tcl interpreters. Which means a configured Tcl interpreter is
reused as much as possible between each modulefile interpretation or
between each modulerc interpretation.

Sourcing modulefiles
^^^^^^^^^^^^^^^^^^^^

Modules 4 introduces the possibility to **source** a modulefile rather
loading it. When it is sourced, a modulefile is interpreted into the shell
environment but then it is not marked loaded in shell environment which
differ from **load** sub-command.

This functionality is used in shell initialization scripts once **module**
function is defined. There the ``etc/modulerc`` modulefile is sourced to
setup the initial state of the environment, composed of *module use*
and *module load* commands.


Removed features and substantial behavior changes
-------------------------------------------------

Following sections provide list of Modules v3.2 features that are
discontinued on Modules v4 or features with a substantial behavior change
that should be taken in consideration when migrating to v4.

Package initialization
^^^^^^^^^^^^^^^^^^^^^^

``MODULESBEGINENV`` environment snapshot functionality is not supported
anymore on Modules v4. Modules collection mechanism should be used instead to
**save** and **restore** sets of enabled modulepaths and loaded modulefiles.

Command line switches
^^^^^^^^^^^^^^^^^^^^^

Some command line switches are not supported anymore on v4.0. When still
using them, a warning message is displayed and the command is ran with these
unsupported switches ignored. Following command line switches are concerned:

* ``--force``, ``-f``
* ``--human``
* ``--verbose``, ``-v``
* ``--silent``, ``-s``
* ``--create``, ``-c``
* ``--icase``, ``-i``
* ``--userlvl`` lvl, ``-u`` lvl

Module sub-commands
^^^^^^^^^^^^^^^^^^^

During an **help** sub-command, Modules v4 does not redirect output made
on stdout in *ModulesHelp* Tcl procedure to stderr. Moreover when running
**help**, version 4 interprets all the content of the modulefile, then call
the *ModulesHelp* procedure if it exists, whereas Modules 3.2 only interprets
the *ModulesHelp* procedure and not the rest of the modulefile content.

When **load** is asked on an already loaded modulefiles, Modules v4 ignores
this new load order whereas v3.2 refreshed shell alias definitions found
in this modulefile.

When **switching** on version 4 an *old* modulefile by a *new* one,
no error is raised if *old* modulefile is not currently loaded. In this
situation v3.2 threw an error and abort switch action. Additionally on
**switch** sub-command, *new* modulefile does not keep the position held
by *old* modulefile in loaded modules list on Modules v4 as it was the
case on v3.2. Same goes for path-like environment variables: replaced
path component is appended to the end or prepended to the beginning of
the relative path-like variable, not appended or prepended relatively to
the position hold by the swapped path component.

During a **switch** command, version 4 interprets the swapped-out modulefile
in *unload* mode, so the sub-modulefiles loaded, with ``module load``
order in the swapped-out modulefile are also unloaded during the switch.

Modules 4 provides path element counting feature which increases a reference
counter each time a given path entry is added to a given environment
variable. This feature also applies to the ``MODULEPATH`` environment
variable. As consequence a modulepath entry element is removed from the
modulepath enabled list only if the related element counter is equal to 1.
When **unusing** a modulepath if its reference counter is greater than 1,
modulepath is kept enabled and reference counter is decreased by 1.

On Modules 3.2 paths composing the ``MODULEPATH`` environment variable
may contain reference to environment variable. These variable references
are resolved dynamically when ``MODULEPATH`` is looked at during module
sub-command action. This feature has been discontinued on Modules v4.

Following Modules sub-commands are not supported anymore on v4.0:

* ``clear``
* ``update``


Modules specific Tcl commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Modules v4 provides path element counting feature which increases a reference
counter each time a given path entry is added to a given environment
variable. As a consequence a path entry element is not always removed
from a path-like variable when calling to ``remove-path`` or calling to
``append-path`` or ``append-path`` at unloading time. The path element is
removed only if its related element counter is equal to 1. If this counter
is greater than 1, path element is kept in variable and reference counter
is decreased by 1.

On Modules v4, **module-info mode** returns during an **unload** sub-command
the ``unload`` value instead of ``remove`` on Modules v3.2.  However if
*mode* is tested against ``remove`` value, true will be returned. During a
**switch** sub-command on Modules v4, ``unload`` then ``load`` is returned
instead of ``switch1`` then ``switch2`` then ``switch3`` on Modules
v3.2. However if *mode* is tested against ``switch`` value, true will
be returned.

When using **set-alias**, Modules v3.2 defines a shell function when
variables are in use in alias value on Bourne shell derivatives, Modules
4 always defines a shell alias never a shell function.

Some Modules specific Tcl commands are not supported anymore on v4.0. When
still using them, a warning message is displayed and these unsupported Tcl
commands are ignored. Following Modules specific Tcl commands are concerned:

* ``module-info flags``
* ``module-info trace``
* ``module-info tracepat``
* ``module-info user``
* ``module-log``
* ``module-trace``
* ``module-user``
* ``module-verbosity``


Further reading
---------------

To get a complete list of the differences between Modules v3.2 and v4,
please read the :ref:`diff_v3_v4` document.

A significant number of issues reported for v3.2 have been closed on v4.
List of these closed issues can be found at:

https://github.com/cea-hpc/modules/milestone/1?closed=1
