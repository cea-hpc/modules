.. _module(1):

module
======


SYNOPSIS
--------

**module** [*switches*] [*sub-command* [*sub-command-args*]]


DESCRIPTION
-----------

:command:`module` is a user interface to the Modules package. The Modules
package provides for the dynamic modification of the user's environment
via *modulefiles*.

Each *modulefile* contains the information needed to configure the
shell for an application. Once the Modules package is initialized, the
environment can be modified on a per-module basis using the :command:`module`
command which interprets *modulefiles*. Typically *modulefiles* instruct
the :command:`module` command to alter or set shell environment variables such
as :envvar:`PATH`, :envvar:`MANPATH`, etc. *Modulefiles* may be shared by many
users on a system and users may have their own set to supplement or replace
the shared *modulefiles*.

The *modulefiles* are added to and removed from the current environment
by the user. The environment changes contained in a *modulefile* can be
summarized through the :command:`module` command as well. If no arguments are
given, a summary of the :command:`module` usage and *sub-commands* are shown.

The action for the :command:`module` command to take is described by the
*sub-command* and its associated arguments.


Package Initialization
^^^^^^^^^^^^^^^^^^^^^^

The Modules package and the :command:`module` command are initialized when a
shell-specific initialization script is sourced into the shell. The script
creates the :command:`module` command as either an alias or function and
creates Modules environment variables.

The :command:`module` alias or function executes the :file:`modulecmd.tcl`
program located in |file libexecdir| and has the shell evaluate the command's
output. The first argument to :file:`modulecmd.tcl` specifies the type of
shell.

The initialization scripts are kept in |file initdir_shell| where
*<shell>* is the name of the sourcing shell. For example, a C Shell user
sources the |file initdir_csh| script. The sh, csh, tcsh, bash, ksh,
zsh and fish shells are supported by :file:`modulecmd.tcl`. In addition,
python, perl, ruby, tcl, cmake, r and lisp "shells" are supported which
writes the environment changes to stdout as python, perl, ruby, tcl, lisp,
r or cmake code.

Initialization may also be performed by calling the :subcmd:`autoinit`
sub-command of the :file:`modulecmd.tcl` program. Evaluation into the shell of
the result of this command defines the :command:`module` alias or function.

A :command:`ml` alias or function may also be defined at initialization time
if enabled (see :envvar:`MODULES_ML` section). :command:`ml` is a handy
frontend leveraging all :command:`module` command capabilities with less
character typed. See :ref:`ml(1)` for detailed information.


Examples of initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^

C Shell initialization (and derivatives):

.. parsed-literal::

     source \ |initdir|\ /csh
     module load modulefile modulefile ...

Bourne Shell (sh) (and derivatives):

.. parsed-literal::

     . \ |initdir|\ /sh
     module load modulefile modulefile ...

Perl:

.. parsed-literal::

     require "\ |initdir|\ /perl.pm";
     &module('load', 'modulefile', 'modulefile', '...');

Python:

.. parsed-literal::

     import os
     exec(open('\ |initdir|\ /python.py').read())
     module('load', 'modulefile', 'modulefile', '...')

Bourne Shell (sh) (and derivatives) with :subcmd:`autoinit` sub-command:

.. parsed-literal::

     eval "\`\ |libexecdir|\ /modulecmd.tcl sh autoinit\`"


Modulecmd startup
^^^^^^^^^^^^^^^^^

Upon invocation :file:`modulecmd.tcl` sources a site-specific configuration
script if it exists. The location for this script is
|file etcdir_siteconfig|. An additional siteconfig script may be
specified with the :envvar:`MODULES_SITECONFIG` environment variable, if
allowed by :file:`modulecmd.tcl` configuration, and will be loaded if it
exists after |file etcdir_siteconfig|. Siteconfig is a Tcl script that enables
to supersede any global variable or procedure definition of
:file:`modulecmd.tcl`.

Afterward, :file:`modulecmd.tcl` sources rc files which contain global,
user and *modulefile* specific setups. These files are interpreted as
*modulefiles*. See :ref:`modulefile(4)` for detailed information.

Upon invocation of :file:`modulecmd.tcl` module run-command files are sourced
in the following order:

1. Global RC file as specified by :envvar:`MODULERCFILE` variable or
   |file etcdir_rc|. If :envvar:`MODULERCFILE` points to a directory, the
   :file:`modulerc` file in this directory is used as global RC file.

2. User specific module RC file :file:`$HOME/.modulerc`

3. All :file:`.modulerc` and :file:`.version` files found during modulefile
   seeking.


Command line switches
^^^^^^^^^^^^^^^^^^^^^

The :command:`module` command accepts command line switches as its first
parameter. These may be used to control output format of all information
displayed and the :command:`module` behavior in case of locating and
interpreting *modulefiles*.

All switches may be entered either in short or long notation. The following
switches are accepted:

.. option:: --auto

 On :subcmd:`load`, :subcmd:`unload` and :subcmd:`switch` sub-commands, enable
 automated module handling mode. See also :envvar:`MODULES_AUTO_HANDLING`
 section.

 .. only:: html

    .. versionadded:: 4.2

.. option:: --color=<WHEN>

 Colorize the output. *WHEN* defaults to ``always`` or can be ``never`` or
 ``auto``. See also :envvar:`MODULES_COLOR` section.

 .. only:: html

    .. versionadded:: 4.3

.. option:: --contains, -C

 On :subcmd:`avail` sub-command, return modules whose fully qualified name
 contains search query string.

 .. only:: html

    .. versionadded:: 4.3

.. option:: --debug, -D

 Debug mode. Causes :command:`module` to print debugging messages about its
 progress.

 .. only:: html

    .. versionadded:: 4.0

.. option:: --default, -d

 On :subcmd:`avail` sub-command, display only the default version of each
 module name. Default version is the explicitly set default version or also
 the implicit default version if the configuration option ``implicit_default``
 is enabled (see :ref:`Locating Modulefiles` section in the
 :ref:`modulefile(4)` man page for further details on implicit default
 version).

 .. only:: html

    .. versionadded:: 4.0

.. option:: --force, -f

 On :subcmd:`load`, :subcmd:`unload` and :subcmd:`switch` sub-commands,
 by-pass any unsatisfied modulefile constraint corresponding to the declared
 :mfcmd:`prereq` and :mfcmd:`conflict`. Which means for instance that a
 *modulefile* will be loaded even if it comes in conflict with another loaded
 *modulefile* or that a *modulefile* will be unloaded even if it is required
 as a prereq by another *modulefile*.

 On :subcmd:`clear` sub-command, skip the confirmation dialog and proceed.

 .. only:: html

    .. versionadded:: 4.3
       :option:`--force`/:option:`-f` support was dropped on version `4.0`
       but reintroduced starting version `4.2` with a different meaning:
       instead of enabling an active dependency resolution mechanism
       :option:`--force` command line switch now enables to by-pass dependency
       consistency when loading or unloading a *modulefile*.

.. option:: --help, -h

 Give some helpful usage information, and terminates the command.

.. option:: --icase, -i

 Match module specification arguments in a case insensitive manner.

 .. only:: html

    .. versionadded:: 4.4
       :option:`--icase`/:option:`-i` support was dropped on version `4.0`
       but reintroduced starting version `4.4`. When set, it now applies to
       search query string and module specificiation on all sub-commands and
       modulefile Tcl commands.

.. option:: --indepth

 On :subcmd:`avail` sub-command, include in search results the matching
 modulefiles and directories and recursively the modulefiles and directories
 contained in these matching directories.

 .. only:: html

    .. versionadded:: 4.3

.. option:: --latest, -L

 On :subcmd:`avail` sub-command, display only the highest numerically sorted
 version of each module name (see :ref:`Locating Modulefiles` section in the
 :ref:`modulefile(4)` man page).

 .. only:: html

    .. versionadded:: 4.0

.. option:: --long, -l

 Display :subcmd:`avail`, :subcmd:`list` and :subcmd:`savelist` output in long
 format.

.. option:: --no-auto

 On :subcmd:`load`, :subcmd:`unload` and :subcmd:`switch` sub-commands,
 disable automated module handling mode. See also
 :envvar:`MODULES_AUTO_HANDLING` section.

 .. only:: html

    .. versionadded:: 4.2

.. option:: --no-indepth

 On :subcmd:`avail` sub-command, limit search results to the matching
 modulefiles and directories found at the depth level expressed by the search
 query. Thus modulefiles contained in directories part of the result are
 excluded.

 .. only:: html

    .. versionadded:: 4.3

.. option:: --no-pager

 Do not pipe message output into a pager.

 .. only:: html

    .. versionadded:: 4.1

.. option:: --paginate

 Pipe all message output into :command:`less` (or if set, to the command
 referred in :envvar:`MODULES_PAGER` variable) if error output stream is a
 terminal. See also :envvar:`MODULES_PAGER` section.

 .. only:: html

    .. versionadded:: 4.1

.. option:: --silent, -s

 Turn off error, warning and informational messages. :command:`module` command
 output result is not affected by silent mode.

 .. only:: html

    .. versionadded:: 4.3
       :option:`--silent`/:option:`-s` support was dropped on version `4.0`
       but reintroduced starting version `4.3`.

.. option:: --starts-with, -S

 On :subcmd:`avail` sub-command, return modules whose name starts with search
 query string.

 .. only:: html

    .. versionadded:: 4.3

.. option:: --terse, -t

 Display :subcmd:`avail`, :subcmd:`list` and :subcmd:`savelist` output in
 short format.

.. option:: --verbose, -v

 Enable verbose messages during :command:`module` command execution.

 .. only:: html

    .. versionadded:: 4.3
       :option:`--verbose`/:option:`-v` support was dropped on version `4.0`
       but reintroduced starting version `4.3`.

.. option:: --version, -V

 Lists the current version of the :command:`module` command. The command then
 terminates without further processing.


.. _Module Sub-Commands:

Module Sub-Commands
^^^^^^^^^^^^^^^^^^^

.. subcmd:: add modulefile...

 See :subcmd:`load`.

.. subcmd:: aliases

 List all available symbolic version-names and aliases in the current
 :envvar:`MODULEPATH`.  All directories in the :envvar:`MODULEPATH` are
 recursively searched in the same manner than for the :subcmd:`avail`
 sub-command. Only the symbolic version-names and aliases found in the search
 are displayed.

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: append-path [-d C|--delim C|--delim=C] [--duplicates] variable value...

 Append *value* to environment *variable*. The *variable* is a colon, or
 *delimiter*, separated list. See :mfcmd:`append-path` in the
 :ref:`modulefile(4)` man page for further explanation.

 .. only:: html

    .. versionadded:: 4.1

.. subcmd:: apropos string

 See :subcmd:`search`.

.. subcmd:: avail [-d|-L] [-t|-l] [-S|-C] [--indepth|--no-indepth] [path...]

 List all available *modulefiles* in the current :envvar:`MODULEPATH`. All
 directories in the :envvar:`MODULEPATH` are recursively searched for files
 containing the *modulefile* magic cookie. If an argument is given, then
 each directory in the :envvar:`MODULEPATH` is searched for *modulefiles*
 whose pathname, symbolic version-name or alias match the argument. Argument
 may contain wildcard characters. Multiple versions of an application can
 be supported by creating a subdirectory for the application containing
 *modulefiles* for each version.

 Symbolic version-names and aliases found in the search are displayed in the
 result of this sub-command. Symbolic version-names are displayed next to
 the *modulefile* they are assigned to within parenthesis. Aliases are listed
 in the :envvar:`MODULEPATH` section where they have been defined. To
 distinguish aliases from *modulefiles* a ``@`` symbol is added within
 parenthesis next to their name. Aliases defined through a global or user
 specific module RC file are listed under the **global/user modulerc**
 section.

 When colored output is enabled and a specific graphical rendition is defined
 for module *default* version, the ``default`` symbol is omitted and instead
 the defined graphical rendition is applied to the relative modulefile. When
 colored output is enabled and a specific graphical rendition is defined for
 module alias, the ``@`` symbol is omitted. The defined graphical rendition
 applies to the module alias name. See :envvar:`MODULES_COLOR` and
 :envvar:`MODULES_COLORS` sections for details on colored output.

 The parameter *path* may also refer to a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

.. subcmd:: clear [-f]

 Force the Modules package to believe that no modules are currently loaded. A
 confirmation is requested if command-line switch :option:`-f` (or
 :option:`--force`) is not passed. Typed confirmation should equal to ``yes``
 or ``y`` in order to proceed.

 .. only:: html

    .. versionadded:: 4.3
       :subcmd:`clear` support was dropped on version `4.0` but reintroduced
       starting version `4.3`.

.. subcmd:: config [--dump-state|name [value]|--reset name]

 Gets or sets :file:`modulecmd.tcl` options. Reports the currently set value
 of passed option *name* or all existing options if no *name* passed. If a
 *name* and a *value* are provided, the value of option *name* is set to
 *value*. If command-line switch ``--reset`` is passed in addition to a
 *name*, overridden value for option *name* is cleared.

 When a reported option value differs from default value a mention is added
 to indicate whether the overridden value is coming from a command-line switch
 (``cmd-line``) or from an environment variable (``env-var``). When a reported
 option value is locked and cannot be altered a (``locked``) mention is added.

 If no value is currently set for an option *name*, the mention ``<undef>`` is
 reported.

 When command-line switch ``--dump-state`` is passed, current
 :file:`modulecmd.tcl` state and Modules-related environment variables are
 reported in addition to currently set :file:`modulecmd.tcl` options.

 Existing option *names* are:

 * ``advanced_version_spec``: advanced module version specification to finely
   select modulefiles (defines environment variable
   :envvar:`MODULES_ADVANCED_VERSION_SPEC` when set
 * ``auto_handling``: automated module handling mode (defines
   :envvar:`MODULES_AUTO_HANDLING`)
 * ``avail_indepth``: :subcmd:`avail` sub-command in depth search mode
   (defines :envvar:`MODULES_AVAIL_INDEPTH`)
 * ``avail_report_dir_sym``: display symbolic versions targeting directories
   on :subcmd:`avail` sub-command
 * ``avail_report_mfile_sym``: display symbolic versions targeting modulefiles
   on :subcmd:`avail` sub-command
 * ``collection_pin_version``: register exact modulefile version in collection
   (defines :envvar:`MODULES_COLLECTION_PIN_VERSION`)
 * ``collection_target``: collection target which is valid for current system
   (defines :envvar:`MODULES_COLLECTION_TARGET`)
 * ``color``: colored output mode (defines :envvar:`MODULES_COLOR`)
 * ``colors``: chosen colors to highlight output items (defines
   :envvar:`MODULES_COLORS`)
 * ``contact``: modulefile contact address (defines :envvar:`MODULECONTACT`)
 * ``extended_default``: allow partial module version specification (defines
   :envvar:`MODULES_EXTENDED_DEFAULT`)
 * ``extra_siteconfig``: additional site-specific configuration script
   location (defines :envvar:`MODULES_SITECONFIG`)
 * ``home``: location of Modules package master directory (defines
   :envvar:`MODULESHOME`)
 * ``icase``: enable case insensitive match (defines :envvar:`MODULES_ICASE`)
 * ``ignored_dirs``: directories ignored when looking for modulefiles
 * ``implicit_default``: set an implicit default version for modules (defines
   :envvar:`MODULES_IMPLICIT_DEFAULT`)
 * ``locked_configs``: configuration options that cannot be superseded
 * ``ml``: define :command:`ml` command at initialization time (defines
   :envvar:`MODULES_ML`)
 * ``pager``: text viewer to paginate message output (defines
   :envvar:`MODULES_PAGER`)
 * ``rcfile``: global run-command file location (defines
   :envvar:`MODULERCFILE`)
 * ``run_quarantine``: environment variables to indirectly pass to
   :file:`modulecmd.tcl` (defines :envvar:`MODULES_RUN_QUARANTINE`)
 * ``silent_shell_debug``: disablement of shell debugging property for the
   module command (defines :envvar:`MODULES_SILENT_SHELL_DEBUG`)
 * ``search_match``: module search match style (defines
   :envvar:`MODULES_SEARCH_MATCH`)
 * ``set_shell_startup``: ensure module command definition by setting shell
   startup file (defines :envvar:`MODULES_SET_SHELL_STARTUP`)
 * ``siteconfig``: primary site-specific configuration script location
 * ``tcl_ext_lib``: Modules Tcl extension library location
 * ``term_background``: terminal background color kind (defines
   :envvar:`MODULES_TERM_BACKGROUND`)
 * ``unload_match_order``: unload firstly loaded or lastly loaded module
   matching request (defines :envvar:`MODULES_UNLOAD_MATCH_ORDER`)
 * ``verbosity``: module command verbosity level (defines
   :envvar:`MODULES_VERBOSITY`)
 * ``wa_277``: workaround for Tcsh history issue (defines
   :envvar:`MODULES_WA_277`)

 The options ``avail_report_dir_sym``, ``avail_report_mfile_sym``,
 ``ignored_dirs``, ``locked_configs``, ``siteconfig`` and ``tcl_ext_lib``
 cannot be altered. Moreover all options referred in ``locked_configs`` value
 are locked, thus they cannot be altered.

 .. only:: html

    .. versionadded:: 4.3

.. subcmd:: display modulefile...

 Display information about one or more *modulefiles*. The display sub-command
 will list the full path of the *modulefile* and the environment changes
 the *modulefile* will make if loaded. (Note: It will not display any
 environment changes found within conditional statements.)

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

.. subcmd:: help [modulefile...]

 Print the usage of each sub-command. If an argument is given, print the
 Module-specific help information for the *modulefile*.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

.. subcmd:: info-loaded modulefile

 Returns the names of currently loaded modules matching passed *modulefile*.
 Returns an empty string if passed *modulefile* does not match any loaded
 modules. See :mfcmd:`module-info loaded<module-info>` in the
 :ref:`modulefile(4)` man page for further explanation.

 .. only:: html

    .. versionadded:: 4.1

.. subcmd:: initadd modulefile...

 Add *modulefile* to the shell's initialization file in the user's home
 directory. The startup files checked (in order) are:

 C Shell

  :file:`.modules`, :file:`.cshrc`, :file:`.csh_variables` and :file:`.login`

 TENEX C Shell

  :file:`.modules`, :file:`.tcshrc`, :file:`.cshrc`, :file:`.csh_variables`
  and :file:`.login`

 Bourne and Korn Shells

  :file:`.modules`, :file:`.profile`

 GNU Bourne Again Shell

  :file:`.modules`, :file:`.bash_profile`, :file:`.bash_login`,
  :file:`.profile` and :file:`.bashrc`

 Z Shell

  :file:`.modules`, :file:`.zshrc`, :file:`.zshenv` and :file:`.zlogin`

 Friendly Interactive Shell

  :file:`.modules`, :file:`.config/fish/config.fish`

 If a ``module load`` line is found in any of these files, the *modulefiles*
 are appended to any existing list of *modulefiles*. The ``module load``
 line must be located in at least one of the files listed above for any of
 the :subcmd:`init<initadd>` sub-commands to work properly. If the
 ``module load`` line is found in multiple shell initialization files, all of
 the lines are changed.

.. subcmd:: initclear

 Clear all of the *modulefiles* from the shell's initialization files.

.. subcmd:: initlist

 List all of the *modulefiles* loaded from the shell's initialization file.

.. subcmd:: initprepend modulefile...

 Does the same as :subcmd:`initadd` but prepends the given modules to the
 beginning of the list.

.. subcmd:: initrm modulefile...

 Remove *modulefile* from the shell's initialization files.

.. subcmd:: initswitch modulefile1 modulefile2

 Switch *modulefile1* with *modulefile2* in the shell's initialization files.

.. subcmd:: is-avail modulefile...

 Returns a true value if any of the listed *modulefiles* exists in enabled
 :envvar:`MODULEPATH`. Returns a false value otherwise. See :mfcmd:`is-avail`
 in the :ref:`modulefile(4)` man page for further explanation.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 .. only:: html

    .. versionadded:: 4.1

.. subcmd:: is-loaded [modulefile...]

 Returns a true value if any of the listed *modulefiles* has been loaded or if
 any *modulefile* is loaded in case no argument is provided. Returns a false
 value otherwise. See :mfcmd:`is-loaded` in the :ref:`modulefile(4)` man page
 for further explanation.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 .. only:: html

    .. versionadded:: 4.1

.. subcmd:: is-saved [collection...]

 Returns a true value if any of the listed *collections* exists or if any
 *collection* exists in case no argument is provided. Returns a false value
 otherwise. See :mfcmd:`is-saved` in the :ref:`modulefile(4)` man page for
 further explanation.

 .. only:: html

    .. versionadded:: 4.1

.. subcmd:: is-used [directory...]

 Returns a true value if any of the listed *directories* has been enabled in
 :envvar:`MODULEPATH` or if any *directory* is enabled in case no argument is
 provided. Returns a false value otherwise. See :mfcmd:`is-used` in the
 :ref:`modulefile(4)` man page for further explanation.

 .. only:: html

    .. versionadded:: 4.1

.. subcmd:: keyword string

 See :subcmd:`search`.

.. subcmd:: list [-t|-l]

 List loaded modules.

.. subcmd:: load [--auto|--no-auto] [-f] modulefile...

 Load *modulefile* into the shell environment.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

.. subcmd:: path modulefile

 Print path to *modulefile*.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: paths modulefile

 Print path of available *modulefiles* matching argument.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: prepend-path [-d C|--delim C|--delim=C] [--duplicates] variable value...

 Prepend *value* to environment *variable*. The *variable* is a colon, or
 *delimiter*, separated list. See :mfcmd:`prepend-path` in the
 :ref:`modulefile(4)` man page for further explanation.

 .. only:: html

    .. versionadded:: 4.1

.. subcmd:: purge

 Unload all loaded *modulefiles*.

.. subcmd:: refresh

 See :subcmd:`reload`.

.. subcmd:: reload

 Unload then load all loaded *modulefiles*.

 No unload then load is performed and an error is returned if the loaded
 *modulefiles* have unsatisfied constraint corresponding to the
 :mfcmd:`prereq` and :mfcmd:`conflict` they declare.

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: remove-path [-d C|--delim C|--delim=C] [--index] variable value...

 Remove *value* from the colon, or *delimiter*, separated list in environment
 *variable*. See :mfcmd:`remove-path` in the :ref:`modulefile(4)` man page for
 further explanation.

 .. only:: html

    .. versionadded:: 4.1

.. subcmd:: restore [collection]

 Restore the environment state as defined in *collection*. If *collection*
 name is not specified, then it is assumed to be the *default* collection. If
 *collection* is a fully qualified path, it is restored from this location
 rather than from a file under the user's collection directory. If
 :envvar:`MODULES_COLLECTION_TARGET` is set, a suffix equivalent to the value
 of this variable is appended to the *collection* file name to restore.

 When restoring a *collection*, the currently set :envvar:`MODULEPATH`
 directory list and the currently loaded *modulefiles* are unused and
 unloaded then used and loaded to exactly match the :envvar:`MODULEPATH` and
 loaded *modulefiles* lists saved in this *collection* file. The order
 of the paths and modulefiles set in *collection* is preserved when
 restoring. It means that currently loaded modules are unloaded to get
 the same :envvar:`LOADEDMODULES` root than collection and currently used
 module paths are unused to get the same :envvar:`MODULEPATH` root. Then
 missing module paths are used and missing modulefiles are loaded.

 If a module, without a default version explicitly defined, is recorded in a
 *collection* by its bare name: loading this module when restoring the
 collection will fail if the configuration option ``implicit_default`` is
 disabled.

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: rm modulefile...

 See :subcmd:`unload`.

.. subcmd:: save [collection]

 Record the currently set :envvar:`MODULEPATH` directory list and the
 currently loaded *modulefiles* in a *collection* file under the user's
 collection directory :file:`$HOME/.module`. If *collection* name is not
 specified, then it is assumed to be the ``default`` collection. If
 *collection* is a fully qualified path, it is saved at this location rather
 than under the user's collection directory.

 If :envvar:`MODULES_COLLECTION_TARGET` is set, a suffix equivalent to the
 value of this variable will be appended to the *collection* file name.

 By default, if a loaded modulefile corresponds to the explicitly defined
 default module version, the bare module name is recorded. If the
 configuration option ``implicit_default`` is enabled, the bare module name is
 also recorded for the implicit default module version. If
 :envvar:`MODULES_COLLECTION_PIN_VERSION` is set to ``1``, module version is
 always recorded even if it is the default version.

 No *collection* is recorded and an error is returned if the loaded
 *modulefiles* have unsatisfied constraint corresponding to the
 :mfcmd:`prereq` and :mfcmd:`conflict` they declare.

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: savelist [-t|-l]

 List collections that are currently saved under the user's collection
 directory. If :envvar:`MODULES_COLLECTION_TARGET` is set, only collections
 matching the target suffix will be displayed.

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: saverm [collection]

 Delete the *collection* file under the user's collection directory. If
 *collection* name is not specified, then it is assumed to be the *default*
 collection. If :envvar:`MODULES_COLLECTION_TARGET` is set, a suffix
 equivalent to the value of this variable will be appended to the *collection*
 file name.

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: saveshow [collection]

 Display the content of *collection*. If *collection* name is not specified,
 then it is assumed to be the *default* collection. If *collection* is a
 fully qualified path, this location is displayed rather than a collection
 file under the user's collection directory. If
 :envvar:`MODULES_COLLECTION_TARGET` is set, a suffix equivalent to the value
 of this variable will be appended to the *collection* file name.

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: search string

 Seeks through the :mfcmd:`module-whatis` informations of all *modulefiles*
 for the specified *string*. All *module-whatis* informations matching the
 *string* in a case insensitive manner will be displayed. *string* may contain
 wildcard characters.

 .. only:: html

    .. versionadded:: 4.0
       Prior version `4.0` :mfcmd:`module-whatis` information search was
       performed with :subcmd:`apropos` or :subcmd:`keyword` sub-commands.

.. subcmd:: show modulefile...

 See :subcmd:`display`.

.. subcmd:: source scriptfile...

 Execute *scriptfile* into the shell environment. *scriptfile* must be written
 with *modulefile* syntax and specified with a fully qualified path. Once
 executed *scriptfile* is not marked loaded in shell environment which differ
 from :subcmd:`load` sub-command.

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: swap [modulefile1] modulefile2

 See :subcmd:`switch`.

.. subcmd:: switch [--auto|--no-auto] [-f] [modulefile1] modulefile2

 Switch loaded *modulefile1* with *modulefile2*. If *modulefile1* is not
 specified, then it is assumed to be the currently loaded module with the
 same root name as *modulefile2*.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

.. subcmd:: test modulefile...

 Execute and display results of the Module-specific tests for the
 *modulefile*.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 .. only:: html

    .. versionadded:: 4.0

.. subcmd:: unload [--auto|--no-auto] [-f] modulefile...

 Remove *modulefile* from the shell environment.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

.. subcmd:: unuse directory...

 Remove one or more *directories* from the :envvar:`MODULEPATH` environment
 variable if reference counter of these *directories* is equal to 1
 or unknown.

 Reference counter of *directory* in :envvar:`MODULEPATH` denotes the number
 of times *directory* has been enabled. When attempting to remove *directory*
 from :envvar:`MODULEPATH`, reference counter variable
 :envvar:`MODULEPATH_modshare<\<VAR\>_modshare>` is checked and *directory* is
 removed only if its relative counter is equal to 1 or not defined. Otherwise
 *directory* is kept and reference counter is decreased by 1.

.. subcmd:: use [-a|--append] directory...

 Prepend one or more *directories* to the :envvar:`MODULEPATH` environment
 variable.  The ``--append`` flag will append the *directory* to
 :envvar:`MODULEPATH`.

 Reference counter environment variable
 :envvar:`MODULEPATH_modshare<\<VAR\>_modshare>` is also set to increase the
 number of times *directory* has been added to :envvar:`MODULEPATH`.

.. subcmd:: whatis [modulefile...]

 Display the information set up by the :mfcmd:`module-whatis` commands inside
 the specified *modulefiles*. These specified *modulefiles* may be
 expressed using wildcard characters. If no *modulefile* is specified,
 all :mfcmd:`module-whatis` lines will be shown.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).


Modulefiles
^^^^^^^^^^^

*modulefiles* are written in the Tool Command Language (Tcl) and are
interpreted by :file:`modulecmd.tcl`. *modulefiles* can use conditional
statements. Thus the effect a *modulefile* will have on the environment
may change depending upon the current state of the environment.

Environment variables are unset when unloading a *modulefile*. Thus, it is
possible to :subcmd:`load` a *modulefile* and then :subcmd:`unload` it without
having the environment variables return to their prior state.


Advanced module version specifiers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When the advanced module version specifiers mechanism is enabled (see
:envvar:`MODULES_ADVANCED_VERSION_SPEC`), the specification of modulefile
passed on Modules sub-commands changes. After the module name a version
constraint prefixed by the ``@`` character may be added. It could be directly
appended to the module name or separated from it with a space character.

Constraints can be expressed to refine the selection of module version to:

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

Advanced specification of single version or list of versions may benefit from
the activation of the extended default mechanism (see
:envvar:`MODULES_EXTENDED_DEFAULT`) to use an abbreviated notation like ``@1``
to refer to more precise version numbers like ``1.2.3``. Range of versions on
its side natively handles abbreviated versions.

In order to be specified in a range of versions or compared to a range of
versions, the version major element should corresponds to a number. For
instance ``10a``, ``1.2.3``, ``1.foo`` are versions valid for range
comparison whereas ``default`` or ``foo.2`` versions are invalid for range
comparison.


Collections
^^^^^^^^^^^

Collections describe a sequence of :subcmd:`module use<use>` then
:subcmd:`module load<load>` commands that are interpreted by
:file:`modulecmd.tcl` to set the user environment as described by this
sequence. When a collection is activated, with the :subcmd:`restore`
sub-command, module paths and loaded modules are unused or unloaded if they
are not part or if they are not ordered the same way as in the collection.

Collections are generated by the :subcmd:`save` sub-command that dumps the
current user environment state in terms of module paths and loaded modules. By
default collections are saved under the :file:`$HOME/.module` directory.

Collections may be valid for a given target if they are suffixed. In this
case these collections can only be restored if their suffix correspond to
the current value of the :envvar:`MODULES_COLLECTION_TARGET` environment
variable (see the dedicated section of this topic below).


EXIT STATUS
-----------

The :command:`module` command exits with ``0`` if its execution succeed.
Otherwise ``1`` is returned.


ENVIRONMENT
-----------

.. envvar:: _LMFILES_

 A colon separated list of the full pathname for all loaded *modulefiles*.

.. envvar:: LOADEDMODULES

 A colon separated list of all loaded *modulefiles*.

.. envvar:: MODULECONTACT

 Email address to contact in case any issue occurs during the interpretation
 of modulefiles.

 .. only:: html

    .. versionadded:: 4.0

.. envvar:: MODULEPATH

 The path that the :command:`module` command searches when looking for
 *modulefiles*. Typically, it is set to the master *modulefiles* directory,
 |file modulefilesdir|, by the initialization script. :envvar:`MODULEPATH`
 can be set using :subcmd:`module use<use>` or by the module initialization
 script to search group or personal *modulefile* directories before or after
 the master *modulefile* directory.

 Path elements registered in the :envvar:`MODULEPATH` environment variable may
 contain reference to environment variables which are converted to their
 corresponding value by :command:`module` command each time it looks at the
 :envvar:`MODULEPATH` value. If an environment variable referred in a path
 element is not defined, its reference is converted to an empty string.

.. envvar:: MODULERCFILE

 The location of a global run-command file containing *modulefile* specific
 setup. See `Modulecmd startup`_ section for detailed information.

.. envvar:: MODULES_ADVANCED_VERSION_SPEC

 If set to ``1``, enable advanced module version specifiers (see `Advanced
 module version specifiers`_ section). If set to ``0``, disable advanced
 module version specifiers.

 Advanced module version specifiers enablement is defined in the following
 order of preference: :envvar:`MODULES_ADVANCED_VERSION_SPEC` environment
 variable then the default set in :file:`modulecmd.tcl` script configuration.
 Which means :envvar:`MODULES_ADVANCED_VERSION_SPEC` overrides default
 configuration.

 .. only:: html

    .. versionadded:: 4.4

.. envvar:: MODULES_AUTO_HANDLING

 If set to ``1``, enable automated module handling mode. If set to ``0``
 disable automated module handling mode. Other values are ignored.

 Automated module handling mode consists in additional actions triggered when
 loading or unloading a *modulefile* to satisfy the constraints it declares.
 When loading a *modulefile*, following actions are triggered:

 * Requirement Load: load of the *modulefiles* declared as a :mfcmd:`prereq`
   of the loading *modulefile*.

 * Dependent Reload: reload of the modulefiles declaring a :mfcmd:`prereq`
   onto loaded *modulefile* or declaring a :mfcmd:`prereq` onto a *modulefile*
   part of this reloading batch.

 When unloading a *modulefile*, following actions are triggered:

 * Dependent Unload: unload of the modulefiles declaring a non-optional
   :mfcmd:`prereq` onto unloaded modulefile or declaring a non-optional
   :mfcmd:`prereq` onto a modulefile part of this unloading batch. A
   :mfcmd:`prereq` modulefile is considered optional if the :mfcmd:`prereq`
   definition order is made of multiple modulefiles and at least one
   alternative modulefile is loaded.

 * Useless Requirement Unload: unload of the :mfcmd:`prereq` modulefiles that
   have been automatically loaded for either the unloaded modulefile, an
   unloaded dependent modulefile or a modulefile part of this useless
   requirement unloading batch. Modulefiles are added to this unloading batch
   only if they are not required by any other loaded modulefiles.

 * Dependent Reload: reload of the modulefiles declaring a :mfcmd:`conflict`
   or an optional :mfcmd:`prereq` onto either the unloaded modulefile, an
   unloaded dependent or an unloaded useless requirement or declaring a
   :mfcmd:`prereq` onto a modulefile part of this reloading batch.

 In case a loaded *modulefile* has some of its declared constraints
 unsatisfied (pre-required modulefile not loaded or conflicting modulefile
 loaded for instance), this loaded *modulefile* is excluded from the automatic
 reload actions described above.

 For the specific case of the :subcmd:`switch` sub-command, where a modulefile
 is unloaded to then load another modulefile. Dependent modulefiles to Unload
 are merged into the Dependent modulefiles to Reload that are reloaded after
 the load of the switched-to modulefile.

 Automated module handling mode enablement is defined in the following order
 of preference: :option:`--auto`/:option:`--no-auto` command line switches,
 then :envvar:`MODULES_AUTO_HANDLING` environment variable, then the default
 set in :file:`modulecmd.tcl` script configuration. Which means
 :envvar:`MODULES_AUTO_HANDLING` overrides default configuration and
 :option:`--auto`/:option:`--no-auto` command line switches override every
 other ways to enable or disable this mode.

 .. only:: html

    .. versionadded:: 4.2

.. envvar:: MODULES_AVAIL_INDEPTH

 If set to ``1``, enable in depth search results for :subcmd:`avail`
 sub-command. If set to ``0`` disable :subcmd:`avail` sub-command in depth
 mode. Other values are ignored.

 When in depth mode is enabled, modulefiles and directories contained in
 directories matching search query are also included in search results. When
 disabled these modulefiles and directories contained in matching directories
 are excluded.

 :subcmd:`avail` sub-command in depth mode enablement is defined in the
 following order of preference: :option:`--indepth`/:option:`--no-indepth`
 command line switches, then :envvar:`MODULES_AVAIL_INDEPTH` environment
 variable, then the default set in :file:`modulecmd.tcl` script configuration.
 Which means :envvar:`MODULES_AVAIL_INDEPTH` overrides default configuration
 and :option:`--indepth`/:option:`--no-indepth` command line switches override
 every other ways to enable or disable this mode.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_CMD

 The location of the active module command script.

 .. only:: html

    .. versionadded:: 4.1

.. envvar:: MODULES_COLLECTION_PIN_VERSION

 If set to ``1``, register exact version number of modulefiles when saving a
 collection. Otherwise modulefile version number is omitted if it corresponds
 to the explicitly set default version and also to the implicit default when
 the configuration option ``implicit_default`` is enabled.

 .. only:: html

    .. versionadded:: 4.1

.. envvar:: MODULES_COLLECTION_TARGET

 The collection target that determines what collections are valid thus
 reachable on the current system.

 Collection directory may sometimes be shared on multiple machines which may
 use different modules setup. For instance modules users may access with the
 same :envvar:`HOME` directory multiple systems using different OS versions.
 When it happens a collection made on machine 1 may be erroneous on machine 2.

 When a target is set, only the collections made for that target are
 available to the :subcmd:`restore`, :subcmd:`savelist`, :subcmd:`saveshow`
 and :subcmd:`saverm` sub-commands. Saving a collection registers the target
 footprint by suffixing the collection filename with
 ``.$MODULES_COLLECTION_TARGET``. The collection target is not involved when
 collection is specified as file path on the :subcmd:`saveshow`,
 :subcmd:`restore` and :subcmd:`save` sub-commands.

 For example, the :envvar:`MODULES_COLLECTION_TARGET` variable may be set with
 results from commands like :command:`lsb_release`, :command:`hostname`,
 :command:`dnsdomainname`, etc.

 .. only:: html

    .. versionadded:: 4.0

.. envvar:: MODULES_COLOR

 Defines if output should be colored or not. Accepted values are ``never``,
 ``auto`` and ``always``.

 When color mode is set to ``auto``, output is colored only if the standard
 error output channel is attached to a terminal.

 Colored output enablement is defined in the following order of preference:
 :option:`--color` command line switch, then :envvar:`MODULES_COLOR`
 environment variable, then :envvar:`CLICOLOR` and :envvar:`CLICOLOR_FORCE`
 environment variables, then the default set in :file:`modulecmd.tcl` script
 configuration. Which means :envvar:`MODULES_COLOR` overrides default
 configuration and the :envvar:`CLICOLOR`/:envvar:`CLICOLOR_FORCE` variables.
 :option:`--color` command line switch overrides every other ways to enable or
 disable this mode.

 :envvar:`CLICOLOR` and :envvar:`CLICOLOR_FORCE` environment variables are
 also honored to define color mode. The ``never`` mode is set if
 :envvar:`CLICOLOR` equals to ``0``. If :envvar:`CLICOLOR` is set to another
 value, it corresponds to the ``auto`` mode. The ``always`` mode is set if
 :envvar:`CLICOLOR_FORCE` is set to a value different than ``0``. Color mode
 set with these two variables is superseded by mode set with
 :envvar:`MODULES_COLOR` environment variable.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_COLORS

 Specifies the colors and other attributes used to highlight various parts of
 the output. Its value is a colon-separated list of output items associated to
 a Select Graphic Rendition (SGR) code. It follows the same syntax than
 :envvar:`LS_COLORS`.

 Output items are designated by keys. Items able to be colorized are:
 highlighted element (``hi``), debug information (``db``), tag separator
 (``se``); Error (``er``), warning (``wa``), module error (``me``) and info
 (``in``) message prefixes; Modulepath (``mp``), directory (``di``), module
 alias (``al``), module symbolic version (``sy``), module ``default`` version
 (``de``) and modulefile command (``cm``).

 See the Select Graphic Rendition (SGR) section in the documentation of the
 text terminal that is used for permitted values and their meaning as
 character attributes. These substring values are integers in decimal
 representation and can be concatenated with semicolons. Modules takes care of
 assembling the result into a complete SGR sequence (``\33[...m``). Common
 values to concatenate include ``1`` for bold, ``4`` for underline, ``30`` to
 ``37`` for foreground colors and ``90`` to ``97`` for 16-color mode
 foreground colors. See also https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters
 for a complete SGR code reference.

 No graphical rendition will be applied to an output item that could normaly
 be colored but which is not defined in the color set. Thus if
 :envvar:`MODULES_COLORS` is defined empty, no output will be colored at all.

 The color set is defined for Modules in the following order of preference:
 :envvar:`MODULES_COLORS` environment variable, then the default set in
 :file:`modulecmd.tcl` script configuration. Which means
 :envvar:`MODULES_COLORS` overrides default configuration.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_EXTENDED_DEFAULT

 If set to ``1``, a specified module version is matched against starting
 portion of existing module versions, where portion is a substring separated
 from the rest of the version string by a ``.`` character. For example
 specified modules ``mod/1`` and ``mod/1.2`` will match existing  modulefile
 ``mod/1.2.3``.

 In case multiple modulefiles match the specified module version and a single
 module has to be selected, the explicitly set default version is returned if
 it is part of matching modulefiles. Otherwise the implicit default among
 matching modulefiles is returned if defined (see
 :envvar:`MODULES_IMPLICIT_DEFAULT` section)

 This environment variable supersedes the value of the configuration option
 ``extended_default`` set in :file:`modulecmd.tcl` script.

 .. only:: html

    .. versionadded:: 4.4

.. envvar:: MODULES_ICASE

 When module specification are passed as argument to module sub-commands or
 modulefile Tcl commands, defines the case sensitiveness to apply to match
 them. When :envvar:`MODULES_ICASE` is set to ``never``, a case sensitive
 match is applied in any cases. When set to ``search``, a case insensitive
 match is applied to the :subcmd:`avail`, :subcmd:`whatis` and :subcmd:`paths`
 sub-commands. When set to ``always``, a case insensitive match is also
 applied to the other module sub-commands and modulefile Tcl commands for the
 module specification they receive as argument.

 Case sensitiveness behavior is defined in the following order of preference:
 :option:`--icase` command line switch, which corresponds to the ``always``
 mode, then :envvar:`MODULES_ICASE` environment variable, then the default set
 in :file:`modulecmd.tcl` script configuration. Which means
 :envvar:`MODULES_ICASE` overrides default configuration and :option:`--icase`
 command line switch overrides every other ways to set case sensitiveness
 behavior.

 .. only:: html

    .. versionadded:: 4.4

.. envvar:: MODULES_IMPLICIT_DEFAULT

 Defines (if set to ``1``) or not (if set to ``0``) an implicit default
 version for modules without a default version explicitly defined (see
 :ref:`Locating Modulefiles` section in the :ref:`modulefile(4)` man page).

 Without either an explicit or implicit default version defined a module must
 be fully qualified (version should be specified in addition to its name) to
 get:

 * targeted by module :subcmd:`load`, :subcmd:`switch`, :subcmd:`display`,
   :subcmd:`help`, :subcmd:`test` and :subcmd:`path` sub-commands.

 * restored from a collection, unless already loaded in collection-specified
   order.

 * automatically loaded by automated module handling mechanisms (see
   :envvar:`MODULES_AUTO_HANDLING` section) when declared as module
   requirement, with :mfcmd:`prereq` or :mfcmd:`module load<module>`
   modulefile commands.

 An error is returned in the above situations if either no explicit or
 implicit default version is defined.

 This environment variable supersedes the value of the configuration option
 ``implicit_default`` set in :file:`modulecmd.tcl` script. This environment
 variable is ignored if ``implicit_default`` has been declared locked in
 ``locked_configs`` configuration option.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_LMALTNAME

 A colon separated list of the alternative names set through
 :mfcmd:`module-version` and :mfcmd:`module-alias` statements corresponding to
 all loaded *modulefiles*. Each element in this list starts by the name of the
 loaded *modulefile* followed by all alternative names resolving to it. The
 loaded modulefile and its alternative names are separated by the ampersand
 character.

 This environment variable is intended for :command:`module` command internal
 use to get knowledge of the alternative names matching loaded *modulefiles*
 in order to keep environment consistent when conflicts or pre-requirements
 are set over these alternative designations. It also helps to find a match
 after *modulefiles* being loaded when :subcmd:`unload`, :subcmd:`is-loaded`
 or :subcmd:`info-loaded` actions are run over these names.

 .. only:: html

    .. versionadded:: 4.2

.. envvar:: MODULES_LMCONFLICT

 A colon separated list of the :mfcmd:`conflict` statements defined by all
 loaded *modulefiles*. Each element in this list starts by the name of the
 loaded *modulefile* declaring the conflict followed by the name of all
 modulefiles it declares a conflict with. These loaded modulefiles and
 conflicting modulefile names are separated by the ampersand character.

 This environment variable is intended for :command:`module` command internal
 use to get knowledge of the conflicts declared by the loaded *modulefiles*
 in order to keep environment consistent when a conflicting module is asked
 for load afterward.

 .. only:: html

    .. versionadded:: 4.2

.. envvar:: MODULES_LMNOTUASKED

 A colon separated list of all loaded *modulefiles* that were not explicitly
 asked for load from the command-line.

 This environment variable is intended for :command:`module` command internal
 use to distinguish the *modulefiles* that have been loaded automatically
 from modulefiles that have been asked by users.

 .. only:: html

    .. versionadded:: 4.2

.. envvar:: MODULES_LMPREREQ

 A colon separated list of the :mfcmd:`prereq` statements defined by all
 loaded *modulefiles*. Each element in this list starts by the name of the
 loaded *modulefile* declaring the pre-requirement followed by the name of all
 modulefiles it declares a :mfcmd:`prereq` with. These loaded modulefiles and
 pre-required modulefile names are separated by the ampersand character. When
 a :mfcmd:`prereq` statement is composed of multiple modulefiles, these
 modulefile names are separated by the pipe character.

 This environment variable is intended for :command:`module` command internal
 use to get knowledge of the pre-requirement declared by the loaded
 *modulefiles* in order to keep environment consistent when a pre-required
 module is asked for unload afterward.

 .. only:: html

    .. versionadded:: 4.2

.. envvar:: MODULES_ML

 If set to ``1``, define :command:`ml` command when initializing Modules (see
 `Package Initialization`_ section). If set to ``0``, :command:`ml` command is
 not defined.

 :command:`ml` command enablement is defined in the following order of
 preference: :envvar:`MODULES_ML` environment variable then the default set in
 :file:`modulecmd.tcl` script configuration. Which means :envvar:`MODULES_ML`
 overrides default configuration.

 .. only:: html

    .. versionadded:: 4.5

.. envvar:: MODULES_PAGER

 Text viewer for use to paginate message output if error output stream is
 attached to a terminal. The value of this variable is composed of a pager
 command name or path eventually followed by command-line options.

 Paging command and options are defined for Modules in the following order of
 preference: :envvar:`MODULES_PAGER` environment variable, then the default
 set in :file:`modulecmd.tcl` script configuration. Which means
 :envvar:`MODULES_PAGER` overrides default configuration.

 If :envvar:`MODULES_PAGER` variable is set to an empty string or to the value
 ``cat``, pager will not be launched.

 .. only:: html

    .. versionadded:: 4.1

.. envvar:: MODULES_RUN_QUARANTINE

 A space separated list of environment variable names that should be passed
 indirectly to :file:`modulecmd.tcl` to protect its run-time environment from
 side-effect coming from their current definition.

 Each variable found in :envvar:`MODULES_RUN_QUARANTINE` will have its value
 emptied or set to the value of the corresponding
 :envvar:`MODULES_RUNENV_\<VAR\>` variable when defining :file:`modulecmd.tcl`
 run-time environment.

 Original values of these environment variables set in quarantine are passed
 to :file:`modulecmd.tcl` via :envvar:`<VAR>_modquar` variables.

 .. only:: html

    .. versionadded:: 4.1

.. envvar:: MODULES_RUNENV_<VAR>

 Value to set to environment variable :envvar:`<VAR>` for
 :file:`modulecmd.tcl` run-time execution if :envvar:`<VAR>` is referred in
 :envvar:`MODULES_RUN_QUARANTINE`.

 .. only:: html

    .. versionadded:: 4.1

.. envvar:: MODULES_SEARCH_MATCH

 When searching for modules with :subcmd:`avail` sub-command, defines the way
 query string should match against available module names. With
 ``starts_with`` value, returned modules are those whose name begins by search
 query string. When set to ``contains``, any modules whose fully qualified
 name contains search query string are returned.

 Module search match style is defined in the following order of preference:
 :option:`--starts-with` and :option:`--contains` command line switches, then
 :envvar:`MODULES_SEARCH_MATCH` environment variable, then the default set in
 :file:`modulecmd.tcl` script configuration. Which means
 :envvar:`MODULES_SEARCH_MATCH` overrides default configuration and
 :option:`--starts-with`/:option:`--contains` command line switches override
 every other ways to set search match style.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_SET_SHELL_STARTUP

 If set to ``1``, defines when :command:`module` command initializes the shell
 startup file to ensure that the :command:`module` command is still defined in
 sub-shells. Setting shell startup file means defining the :envvar:`ENV` and
 :envvar:`BASH_ENV` environment variable to the Modules bourne shell
 initialization script. If set to ``0``, shell startup file is not defined.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_SILENT_SHELL_DEBUG

 If set to ``1``, disable any ``xtrace`` or ``verbose`` debugging property set
 on current shell session for the duration of either the module command or the
 module shell initialization script. Only applies to Bourne Shell (sh) and its
 derivatives.

 .. only:: html

    .. versionadded:: 4.1

.. envvar:: MODULES_SITECONFIG

 Location of a site-specific configuration script to source into
 :file:`modulecmd.tcl`. See also `Modulecmd startup`_ section.

 This environment variable is ignored if ``extra_siteconfig`` has been
 declared locked in ``locked_configs`` configuration option.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_TERM_BACKGROUND

 Inform Modules of the terminal background color to determine if the color set
 for dark background or the color set for light background should be used to
 color output in case no specific color set is defined with the
 :envvar:`MODULES_COLORS` variable. Accepted values are ``dark`` and
 ``light``.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_UNLOAD_MATCH_ORDER

 When a module unload request matches multiple loaded modules, unload firstly
 loaded module or lastly loaded module. Accepted values are ``returnfirst``
 and ``returnlast``.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_USE_COMPAT_VERSION

 If set to ``1`` prior to Modules package initialization, enable
 Modules compatibility version (3.2 release branch) rather main version
 at initialization scripts running time. Modules package compatibility
 version should be installed along with main version for this environment
 variable to have any effect.

 .. only:: html

    .. versionadded:: 4.0

.. envvar:: MODULES_VERBOSITY

 Defines the verbosity level of the module command. Available verbosity levels
 from the least to the most verbose are:

 * ``silent``: turn off error, warning and informational messages but does not
   affect module command output result.
 * ``concise``: enable error and warning messages but disable informational
   messages.
 * ``normal``: turn on informational messages, like a report of the additional
   module evaluations triggered by loading or unloading modules, aborted
   evaluation issues or a report of each module evaluation occurring during a
   :subcmd:`restore` or :subcmd:`source` sub-commands.
 * ``verbose``: add additional informational messages, like a systematic
   report of the loading or unloading module evaluations.
 * ``debug``: print debugging messages about module command execution.

 Module command verbosity is defined in the following order of preference:
 :option:`--silent`, :option:`--verbose` and :option:`--debug` command line
 switches, then :envvar:`MODULES_VERBOSITY` environment variable, then the
 default set in :file:`modulecmd.tcl` script configuration. Which means
 :envvar:`MODULES_VERBOSITY` overrides default configuration and
 :option:`--silent`/:option:`--verbose`/:option:`--debug` command line
 switches overrides every other ways to set verbosity level.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULES_WA_277

 If set to ``1`` prior to Modules package initialization, enables workaround
 for Tcsh history issue (see https://github.com/cea-hpc/modules/issues/277).
 This issue leads to erroneous history entries under Tcsh shell. When
 workaround is enabled, an alternative *module* alias is defined which fixes
 the history mechanism issue. However the alternative definition of the
 *module* alias weakens shell evaluation of the code produced by modulefiles.
 Characters with a special meaning for Tcsh shell (like ``{`` and ``}``) may
 not be used anymore in shell alias definition otherwise the evaluation of the
 code produced by modulefiles will return a syntax error.

 .. only:: html

    .. versionadded:: 4.3

.. envvar:: MODULESHOME

 The location of the master Modules package file directory containing module
 command initialization scripts, the executable program :file:`modulecmd.tcl`,
 and a directory containing a collection of master *modulefiles*.

.. envvar:: <VAR>_modquar

 Value of environment variable :envvar:`<VAR>` passed to :file:`modulecmd.tcl`
 in order to restore :envvar:`<VAR>` to this value once started.

 .. only:: html

    .. versionadded:: 4.1

.. envvar:: <VAR>_modshare

 Reference counter variable for path-like variable :envvar:`<VAR>`. A colon
 separated list containing pairs of elements. A pair is formed by a path
 element followed its usage counter which represents the number of times
 this path has been enabled in variable :envvar:`<VAR>`. A colon separates the
 two parts of the pair.

 .. only:: html

    .. versionadded:: 4.0


FILES
-----

|file prefix|

 The :envvar:`MODULESHOME` directory.

|file etcdir_siteconfig|

 The site-specific configuration script of :file:`modulecmd.tcl`. An
 additional configuration script could be defined using the
 :envvar:`MODULES_SITECONFIG` environment variable.

|file etcdir_rc|

 The system-wide modules rc file. The location of this file can be changed
 using the :envvar:`MODULERCFILE` environment variable as described above.

:file:`$HOME/.modulerc`

 The user specific modules rc file.

:file:`$HOME/.module`

 The user specific collection directory.

|file modulefilesdir|

 The directory for system-wide *modulefiles*. The location of the directory
 can be changed using the :envvar:`MODULEPATH` environment variable as
 described above.

|file libexecdir_modulecmd|

 The *modulefile* interpreter that gets executed upon each invocation
 of :command:`module`.

|file initdir_shell|

 The Modules package initialization file sourced into the user's environment.


SEE ALSO
--------

:ref:`ml(1)`, :ref:`modulefile(4)`

