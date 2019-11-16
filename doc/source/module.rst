.. _module(1):

module
======


SYNOPSIS
--------

**module** [*switches*] [*sub-command* [*sub-command-args*]]


DESCRIPTION
-----------

**module** is a user interface to the Modules package. The Modules
package provides for the dynamic modification of the user's environment
via *modulefiles*.

Each *modulefile* contains the information needed to configure the
shell for an application. Once the Modules package is initialized, the
environment can be modified on a per-module basis using the **module**
command which interprets *modulefiles*. Typically *modulefiles* instruct
the **module** command to alter or set shell environment variables such
as **PATH**, **MANPATH**, etc. *Modulefiles* may be shared by many users
on a system and users may have their own set to supplement or replace the
shared *modulefiles*.

The *modulefiles* are added to and removed from the current environment
by the user. The environment changes contained in a *modulefile* can be
summarized through the **module** command as well. If no arguments are
given, a summary of the **module** usage and *sub-commands* are shown.

The action for the **module** command to take is described by the
*sub-command* and its associated arguments.


Package Initialization
^^^^^^^^^^^^^^^^^^^^^^

The Modules package and the **module** command are initialized when a
shell-specific initialization script is sourced into the shell. The script
creates the **module** command as either an alias or function and creates
Modules environment variables.

The **module** alias or function executes the **modulecmd.tcl** program
located in |emph libexecdir| and has the shell evaluate the command's
output. The first argument to **modulecmd.tcl** specifies the type of shell.

The initialization scripts are kept in |emph initdir|\ */<shell>* where
*<shell>* is the name of the sourcing shell. For example, a C Shell user
sources the |emph initdir|\ */csh* script. The sh, csh, tcsh, bash, ksh,
zsh and fish shells are supported by **modulecmd.tcl**. In addition,
python, perl, ruby, tcl, cmake, r and lisp "shells" are supported which
writes the environment changes to stdout as python, perl, ruby, tcl, lisp,
r or cmake code.

Initialization may also be performed by calling the **autoinit** sub-command
of the **modulecmd.tcl** program. Evaluation into the shell of the result
of this command defines the **module** alias or function.


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

Bourne Shell (sh) (and derivatives) with **autoinit** sub-command:

.. parsed-literal::

     eval "\`\ |libexecdir|\ /modulecmd.tcl sh autoinit\`"


Modulecmd startup
^^^^^^^^^^^^^^^^^

Upon invocation **modulecmd.tcl** sources a site-specific configuration
script if it exists. The location for this script is
|emph etcdir|\ */siteconfig.tcl*. An additional siteconfig script may be
specified with the *$MODULES_SITECONFIG* environment variable, if allowed by
**modulecmd.tcl** configuration, and will be loaded if it exists after
|emph etcdir|\ */siteconfig.tcl*. Siteconfig is a Tcl script that enables to
supersede any global variable or procedure definition of **modulecmd.tcl**.

Afterward, **modulecmd.tcl** sources rc files which contain global,
user and *modulefile* specific setups. These files are interpreted as
*modulefiles*. See :ref:`modulefile(4)` for detailed information.

Upon invocation of **modulecmd.tcl** module run-command files are sourced
in the following order:

1. Global RC file as specified by *$MODULERCFILE* or |emph etcdir|\ */rc*.
   If *$MODULERCFILE* points to a directory, the *modulerc* file in this
   directory is used as global RC file.

2. User specific module RC file *$HOME/.modulerc*

3. All *.modulerc* and *.version* files found during modulefile seeking.


Command line switches
^^^^^^^^^^^^^^^^^^^^^

The **module** command accepts command line switches as its first parameter.
These may be used to control output format of all information displayed and
the **module** behavior in case of locating and interpreting *modulefiles*.

All switches may be entered either in short or long notation. The following
switches are accepted:

**--help**, **-h**

 Give some helpful usage information, and terminates the command.

**--version**, **-V**

 Lists the current version of the **module** command. The command then
 terminates without further processing.

**--debug**, **-D**

 Debug mode. Causes **module** to print debugging messages about its
 progress.

**--verbose**, **-v**

 Enable verbose messages during **module** command execution.

**--silent**, **-s**

 Turn off error, warning and informational messages. **module** command output
 result is not affected by silent mode.

**--paginate**

 Pipe all message output into *less* (or if set, *$MODULES_PAGER*) if error
 output stream is a terminal. See also **MODULES_PAGER** section.

**--no-pager**

 Do not pipe message output into a pager.

**--color**\[=\ *WHEN*\]

 Colorize the output. *WHEN* defaults to *always* or can be *never* or *auto*.
 See also **MODULES_COLOR** section.

**--auto**

 On **load**, **unload** and **switch** sub-commands, enable automated module
 handling mode. See also **MODULES_AUTO_HANDLING** section.

**--no-auto**

 On **load**, **unload** and **switch** sub-commands, disable automated module
 handling mode. See also **MODULES_AUTO_HANDLING** section.

**--force**, **-f**

 On **load**, **unload** and **switch** sub-commands, by-pass any unsatisfied
 modulefile constraint corresponding to the declared **prereq** and
 **conflict**. Which means for instance that a *modulefile* will be loaded
 even if it comes in conflict with another loaded *modulefile* or that a
 *modulefile* will be unloaded even if it is required as a prereq by another
 *modulefile*.

 On **clear** sub-command, skip the confirmation dialog and proceed.

**--terse**, **-t**

 Display **avail**, **list** and **savelist** output in short format.

**--long**, **-l**

 Display **avail**, **list** and **savelist** output in long format.

**--default**, **-d**

 On **avail** sub-command, display only the default version of each module
 name. Default version is the explicitly set default version or also the
 implicit default version if **config** option *implicit_default* is enabled
 (see Locating Modulefiles section in the :ref:`modulefile(4)` man page for
 further details on implicit default version).

**--latest**, **-L**

 On **avail** sub-command, display only the highest numerically sorted
 version of each module name (see Locating Modulefiles section in the
 :ref:`modulefile(4)` man page).

**--starts-with**, **-S**

 On **avail** sub-command, return modules whose name starts with search query
 string.

**--contains**, **-C**

 On **avail** sub-command, return modules whose fully qualified name contains
 search query string.

**--indepth**

 On **avail** sub-command, include in search results the matching modulefiles
 and directories and recursively the modulefiles and directories contained in
 these matching directories.

**--no-indepth**

 On **avail** sub-command, limit search results to the matching modulefiles
 and directories found at the depth level expressed by the search query. Thus
 modulefiles contained in directories part of the result are excluded.

**--icase**, **-i**

 Match module specification arguments in a case insensitive manner.


Module Sub-Commands
^^^^^^^^^^^^^^^^^^^

**help** [modulefile...]

 Print the usage of each sub-command. If an argument is given, print the
 Module-specific help information for the *modulefile*.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**add** modulefile...

 See **load**.

**load** [--auto|--no-auto] [-f] modulefile...

 Load *modulefile* into the shell environment.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**rm** modulefile...

 See **unload**.

**unload** [--auto|--no-auto] [-f] modulefile...

 Remove *modulefile* from the shell environment.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**swap** [modulefile1] modulefile2

 See **switch**.

**switch** [--auto|--no-auto] [-f] [modulefile1] modulefile2

 Switch loaded *modulefile1* with *modulefile2*. If *modulefile1* is not
 specified, then it is assumed to be the currently loaded module with the
 same root name as *modulefile2*.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**show** modulefile...

 See **display**.

**display** modulefile...

 Display information about one or more *modulefiles*. The display sub-command
 will list the full path of the *modulefile* and the environment changes
 the *modulefile* will make if loaded. (Note: It will not display any
 environment changes found within conditional statements.)

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**list** [-t|-l]

 List loaded modules.

**avail** [-d|-L] [-t|-l] [-S|-C] [--indepth|--no-indepth] [path...]

 List all available *modulefiles* in the current **MODULEPATH**. All
 directories in the **MODULEPATH** are recursively searched for files
 containing the *modulefile* magic cookie. If an argument is given, then
 each directory in the **MODULEPATH** is searched for *modulefiles* whose
 pathname, symbolic version-name or alias match the argument. Argument
 may contain wildcard characters. Multiple versions of an application can
 be supported by creating a subdirectory for the application containing
 *modulefiles* for each version.

 Symbolic version-names and aliases found in the search are displayed in the
 result of this sub-command. Symbolic version-names are displayed next to
 the *modulefile* they are assigned to within parenthesis. Aliases are listed
 in the **MODULEPATH** section where they have been defined. To distinguish
 aliases from *modulefiles* a **@** symbol is added within parenthesis
 next to their name. Aliases defined through a global or user specific
 module RC file are listed under the **global/user modulerc** section.

 When colored output is enabled and a specific graphical rendition is defined
 for module *default* version, the **default** symbol is omitted and instead
 the defined graphical rendition is applied to the relative modulefile. When
 colored output is enabled and a specific graphical rendition is defined for
 module alias, the **@** symbol is omitted. The defined graphical rendition
 applies to the module alias name. See **MODULES_COLOR** and
 **MODULES_COLORS** sections for details on colored output.

 The parameter *path* may also refer to a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**aliases**

 List all available symbolic version-names and aliases in the current
 **MODULEPATH**.  All directories in the **MODULEPATH** are recursively
 searched in the same manner than for the **avail** sub-command. Only the
 symbolic version-names and aliases found in the search are displayed.

**use** [-a|--append] directory...

 Prepend one or more *directories* to the **MODULEPATH** environment
 variable.  The *--append* flag will append the *directory* to
 **MODULEPATH**.

 Reference counter environment variable **MODULEPATH_modshare** is
 also set to increase the number of times *directory* has been added to
 **MODULEPATH**.

**unuse** directory...

 Remove one or more *directories* from the **MODULEPATH** environment
 variable if reference counter of these *directories* is equal to 1
 or unknown.

 Reference counter of *directory* in **MODULEPATH** denotes the number of
 times *directory* has been enabled. When attempting to remove *directory*
 from **MODULEPATH**, reference counter variable **MODULEPATH_modshare**
 is checked and *directory* is removed only if its relative counter is
 equal to 1 or not defined. Elsewhere *directory* is kept and reference
 counter is decreased by 1.

**refresh**

 See **reload**.

**reload**

 Unload then load all loaded *modulefiles*.

 No unload then load is performed and an error is returned if the loaded
 *modulefiles* have unsatisfied constraint corresponding to the **prereq**
 and **conflict** they declare.

**purge**

 Unload all loaded *modulefiles*.

**clear** [-f]

 Force the Modules package to believe that no modules are currently loaded. A
 confirmation is requested if command-line switch *-f* (or *--force*) is not
 passed. Typed confirmation should equal to *yes* or *y* in order to proceed.

**source** scriptfile...

 Execute *scriptfile* into the shell environment. *scriptfile* must be written
 with *modulefile* syntax and specified with a fully qualified path. Once
 executed *scriptfile* is not marked loaded in shell environment which differ
 from **load** sub-command.

**whatis** [modulefile...]

 Display the information set up by the **module-whatis** commands inside
 the specified *modulefiles*. These specified *modulefiles* may be
 expressed using wildcard characters. If no *modulefile* is specified,
 all **module-whatis** lines will be shown.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**apropos** string

 See **search**.

**keyword** string

 See **search**.

**search** string

 Seeks through the **module-whatis** informations of all *modulefiles* for the
 specified *string*. All *module-whatis* informations matching the *string* in
 a case insensitive manner will be displayed. *string* may contain wildcard
 characters.

**test** modulefile...

 Execute and display results of the Module-specific tests for the
 *modulefile*.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**save** [collection]

 Record the currently set **MODULEPATH** directory list and the currently
 loaded *modulefiles* in a *collection* file under the user's collection
 directory *$HOME/.module*. If *collection* name is not specified, then
 it is assumed to be the *default* collection. If *collection* is a fully
 qualified path, it is saved at this location rather than under the user's
 collection directory.

 If **MODULES_COLLECTION_TARGET** is set, a suffix equivalent to the value
 of this variable will be appended to the *collection* file name.

 By default, if loaded modulefile corresponds to the explicitly defined
 default module version, the bare module name is recorded. If **config**
 option *implicit_default* is enabled, the bare module name is also recorded
 for the implicit default module version. If
 **MODULES_COLLECTION_PIN_VERSION** is set to **1**, module version is always
 recorded even if it is the default version.

 No *collection* is recorded and an error is returned if the loaded
 *modulefiles* have unsatisfied constraint corresponding to the **prereq**
 and **conflict** they declare.

**restore** [collection]

 Restore the environment state as defined in *collection*. If *collection*
 name is not specified, then it is assumed to be the *default* collection. If
 *collection* is a fully qualified path, it is restored from this location
 rather than from a file under the user's collection directory. If
 **MODULES_COLLECTION_TARGET** is set, a suffix equivalent to the value
 of this variable is appended to the *collection* file name to restore.

 When restoring a *collection*, the currently set **MODULEPATH**
 directory list and the currently loaded *modulefiles* are unused and
 unloaded then used and loaded to exactly match the **MODULEPATH** and
 loaded *modulefiles* lists saved in this *collection* file. The order
 of the paths and modulefiles set in *collection* is preserved when
 restoring. It means that currently loaded modules are unloaded to get
 the same **LOADEDMODULES** root than collection and currently used module
 paths are unused to get the same **MODULEPATH** root. Then missing module
 paths are used and missing modulefiles are loaded.

 If a module, without a default version explicitly defined, is recorded in a
 *collection* by its bare name: loading this module when restoring the
 collection will fail if **config** option *implicit_default* is disabled.

**saverm** [collection]

 Delete the *collection* file under the user's collection directory. If
 *collection* name is not specified, then it is assumed to be the *default*
 collection. If **MODULES_COLLECTION_TARGET** is set, a suffix equivalent to
 the value of this variable will be appended to the *collection* file name.

**saveshow** [collection]

 Display the content of *collection*. If *collection* name is not specified,
 then it is assumed to be the *default* collection. If *collection* is a
 fully qualified path, this location is displayed rather than a collection
 file under the user's collection directory. If **MODULES_COLLECTION_TARGET**
 is set, a suffix equivalent to the value of this variable will be appended
 to the *collection* file name.

**savelist** [-t|-l]

 List collections that are currently saved under the user's collection
 directory. If **MODULES_COLLECTION_TARGET** is set, only collections
 matching the target suffix will be displayed.

**initadd** modulefile...

 Add *modulefile* to the shell's initialization file in the user's home
 directory. The startup files checked (in order) are:

 C Shell

  *.modules*, *.cshrc*, *.csh_variables* and *.login*

 TENEX C Shell

  *.modules*, *.tcshrc*, *.cshrc*, *.csh_variables* and *.login*

 Bourne and Korn Shells

  *.modules*, *.profile*

 GNU Bourne Again Shell

  *.modules*, *.bash_profile*, *.bash_login*, *.profile* and *.bashrc*

 Z Shell

  *.modules*, *.zshrc*, *.zshenv* and *.zlogin*

 Friendly Interactive Shell

  *.modules*, *.config/fish/config.fish*

 If a **module load** line is found in any of these files, the *modulefiles*
 are appended to any existing list of *modulefiles*. The **module load**
 line must be located in at least one of the files listed above for any of
 the **init** sub-commands to work properly. If the **module load** line is
 found in multiple shell initialization files, all of the lines are changed.

**initprepend** modulefile...

 Does the same as **initadd** but prepends the given modules to the
 beginning of the list.

**initrm** modulefile...

 Remove *modulefile* from the shell's initialization files.

**initswitch** modulefile1 modulefile2

 Switch *modulefile1* with *modulefile2* in the shell's initialization files.

**initlist**

 List all of the *modulefiles* loaded from the shell's initialization file.

**initclear**

 Clear all of the *modulefiles* from the shell's initialization files.

**path** modulefile

 Print path to *modulefile*.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**paths** modulefile

 Print path of available *modulefiles* matching argument.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**append-path** [-d C|--delim C|--delim=C] [--duplicates] variable value...

 Append *value* to environment *variable*. The *variable* is a colon, or
 *delimiter*, separated list. See **append-path** in the :ref:`modulefile(4)`
 man page for further explanation.

**prepend-path** [-d C|--delim C|--delim=C] [--duplicates] variable value...

 Prepend *value* to environment *variable*. The *variable* is a colon, or
 *delimiter*, separated list. See **prepend-path** in the :ref:`modulefile(4)`
 man page for further explanation.

**remove-path** [-d C|--delim C|--delim=C] [--index] variable value...

 Remove *value* from the colon, or *delimiter*, separated list in environment
 *variable*. See **remove-path** in the :ref:`modulefile(4)` man page for
 further explanation.

**is-loaded** [modulefile...]

 Returns a true value if any of the listed *modulefiles* has been loaded or if
 any *modulefile* is loaded in case no argument is provided. Returns a false
 value elsewhere. See **is-loaded** in the :ref:`modulefile(4)` man page for
 further explanation.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**is-saved** [collection...]

 Returns a true value if any of the listed *collections* exists or if any
 *collection* exists in case no argument is provided. Returns a false value
 elsewhere. See **is-saved** in the :ref:`modulefile(4)` man page for further
 explanation.

**is-used** [directory...]

 Returns a true value if any of the listed *directories* has been enabled in
 **MODULEPATH** or if any *directory* is enabled in case no argument is
 provided. Returns a false value elsewhere. See **is-used** in the
 :ref:`modulefile(4)` man page for further explanation.

**is-avail** modulefile...

 Returns a true value if any of the listed *modulefiles* exists in enabled
 **MODULEPATH**. Returns a false value elsewhere. See **is-avail** in the
 :ref:`modulefile(4)` man page for further explanation.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**info-loaded** modulefile

 Returns the names of currently loaded modules matching passed *modulefile*.
 Returns an empty string if passed *modulefile* does not match any loaded
 modules. See **module-info loaded** in the :ref:`modulefile(4)` man page for
 further explanation.

**config** [--dump-state|name [value]|--reset name]

 Gets or sets **modulecmd.tcl** options. Reports the currently set value of
 passed option *name* or all existing options if no *name* passed. If a *name*
 and a *value* are provided, the value of option *name* is set to *value*. If
 command-line switch *--reset* is passed in addition to a *name*, overridden
 overridden value for option *name* is cleared.

 When a reported option value differs from default value a mention is added
 to indicate whether the overridden value is coming from a command-line switch
 (*cmd-line*) or from an environment variable (*env-var*). When a reported
 option value is locked and cannot be altered a (*locked*) mention is added.

 If no value is currently set for an option *name*, the mention *<undef>* is
 reported.

 When command-line switch *--dump-state* is passed, current **modulecmd.tcl**
 state and Modules-related environment variables are reported in addition to
 currently set **modulecmd.tcl** options.

 Existing option *names* are:

 * advanced_version_spec: advanced module version specification to finely
   select modulefiles (defines environment variable
   **MODULES_ADVANCED_VERSION_SPEC** when set
 * auto_handling: automated module handling mode (defines
   **MODULES_AUTO_HANDLING**)
 * avail_indepth: **avail** sub-command in depth search mode (defines
   **MODULES_AVAIL_INDEPTH**)
 * avail_report_dir_sym: display symbolic versions targeting directories on
   **avail** sub-command
 * avail_report_mfile_sym: display symbolic versions targeting modulefiles on
   **avail** sub-command
 * collection_pin_version: register exact modulefile version in collection
   (defines **MODULES_COLLECTION_PIN_VERSION**)
 * collection_target: collection target which is valid for current system
   (defines **MODULES_COLLECTION_TARGET**)
 * color: colored output mode (defines **MODULES_COLOR**)
 * colors: chosen colors to highlight output items (defines
   **MODULES_COLORS**)
 * contact: modulefile contact address (defines **MODULECONTACT**)
 * extended_default: allow partial module version specification (defines
   **MODULES_EXTENDED_DEFAULT**)
 * extra_siteconfig: additional site-specific configuration script location
   (defines **MODULES_SITECONFIG**)
 * home: location of Modules package master directory (defines
   **MODULESHOME**)
 * icase: enable case insensitive match (defines **MODULES_ICASE**)
 * ignored_dirs: directories ignored when looking for modulefiles
 * implicit_default: set an implicit default version for modules (defines
   **MODULES_IMPLICIT_DEFAULT**)
 * locked_configs: configuration options that cannot be superseded
 * pager: text viewer to paginate message output (defines **MODULES_PAGER**)
 * rcfile: global run-command file location (defines **MODULERCFILE**)
 * run_quarantine: environment variables to indirectly pass to
   **modulecmd.tcl** (defines **MODULES_RUN_QUARANTINE**)
 * silent_shell_debug: disablement of shell debugging property for the module
   command (defines **MODULES_SILENT_SHELL_DEBUG**)
 * search_match: module search match style (defines **MODULES_SEARCH_MATCH**)
 * set_shell_startup: ensure module command definition by setting shell
   startup file (defines **MODULES_SET_SHELL_STARTUP**)
 * siteconfig: primary site-specific configuration script location
 * tcl_ext_lib: Modules Tcl extension library location
 * term_background: terminal background color kind (defines
   **MODULES_TERM_BACKGROUND**)
 * unload_match_order: unload firstly loaded or lastly loaded module matching
   request (defines **MODULES_UNLOAD_MATCH_ORDER**)
 * verbosity: module command verbosity level (defines **MODULES_VERBOSITY**)
 * wa_277: workaround for Tcsh history issue (defines **MODULES_WA_277**)

The options *avail_report_dir_sym*, *avail_report_mfile_sym*, *ignored_dirs*,
*locked_configs*, *siteconfig* and *tcl_ext_lib* cannot be altered. Moreover
all options referred in *locked_configs* value are locked thus they cannot be
altered.


Modulefiles
^^^^^^^^^^^

*modulefiles* are written in the Tool Command Language (Tcl) and are
interpreted by **modulecmd.tcl**. *modulefiles* can use conditional
statements. Thus the effect a *modulefile* will have on the environment
may change depending upon the current state of the environment.

Environment variables are unset when unloading a *modulefile*. Thus, it is
possible to **load** a *modulefile* and then **unload** it without having
the environment variables return to their prior state.


Advanced module version specifiers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When the advanced module version specifiers mechanism is enabled (see
**MODULES_ADVANCED_VERSION_SPEC**), the specification of modulefile passed on
Modules sub-commands changes. After the module name a version constraint
prefixed by the ``@`` character may be added. It could be directly appended to
the module name or separated from it with a space character.

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
**MODULES_EXTENDED_DEFAULT**) to use an abbreviated notation like ``@1`` to
refer to more precise version numbers like ``1.2.3``. Range of versions on its
side natively handles abbreviated versions.

In order to be specified in a range of versions or compared to a range of
versions, the version major element should corresponds to a number. For
instance ``10a``, ``1.2.3``, ``1.foo`` are versions valid for range
comparison whereas ``default`` or ``foo.2`` versions are invalid for range
comparison.


Collections
^^^^^^^^^^^

Collections describe a sequence of **module use** then **module load**
commands that are interpreted by **modulecmd.tcl** to set the user
environment as described by this sequence. When a collection is activated,
with the **restore** sub-command, module paths and loaded modules are
unused or unloaded if they are not part or if they are not ordered the
same way as in the collection.

Collections are generated by the **save** sub-command that dumps the current
user environment state in terms of module paths and loaded modules. By
default collections are saved under the *$HOME/.module* directory.

Collections may be valid for a given target if they are suffixed. In this
case these collections can only be restored if their suffix correspond to
the current value of the **MODULES_COLLECTION_TARGET** environment variable
(see the dedicated section of this topic below).


EXIT STATUS
-----------

The **module** command exits with **0** if its execution succeed. Elsewhere
**1** is returned.


ENVIRONMENT
-----------

**LOADEDMODULES**

 A colon separated list of all loaded *modulefiles*.

**MODULECONTACT**

 Email address to contact in case any issue occurs during the interpretation
 of modulefiles.

**MODULEPATH**

 The path that the **module** command searches when looking for
 *modulefiles*. Typically, it is set to the master *modulefiles* directory,
 |emph prefix|\ */modulefiles*, by the initialization script. **MODULEPATH**
 can be set using **module use** or by the module initialization script
 to search group or personal *modulefile* directories before or after the
 master *modulefile* directory.

 Path elements registered in the **MODULEPATH** environment variable may
 contain reference to environment variables which are converted to their
 corresponding value by **module** command each time it looks at the
 **MODULEPATH** value. If an environment variable referred in a path element
 is not defined, its reference is converted to an empty string.

**MODULERCFILE**

 The location of a global run-command file containing *modulefile* specific
 setup. See `Modulecmd startup`_ section for detailed information.

**MODULESHOME**

 The location of the master Modules package file directory containing module
 command initialization scripts, the executable program **modulecmd.tcl**,
 and a directory containing a collection of master *modulefiles*.

**MODULES_ADVANCED_VERSION_SPEC**

 If set to **1**, enable advanced module version specifiers (see `Advanced
 module version specifiers`_ section). If set to **0**, disable advanced
 module version specifiers.

 Advanced module version specifiers enablement is defined in the following
 order of preference: **MODULES_ADVANCED_VERSION_SPEC** environment variable
 then the default set in **modulecmd.tcl** script configuration. Which means
 **MODULES_ADVANCED_VERSION_SPEC** overrides default configuration.

**MODULES_AUTO_HANDLING**

 If set to **1**, enable automated module handling mode. If set to **0**
 disable automated module handling mode. Other values are ignored.

 Automated module handling mode consists in additional actions triggered when
 loading or unloading a *modulefile* to satisfy the constraints it declares.
 When loading a *modulefile*, following actions are triggered:

 * Requirement Load: load of the *modulefiles* declared as a **prereq** of
   the loading *modulefile*.

 * Dependent Reload: reload of the modulefiles declaring a **prereq** onto
   loaded *modulefile* or declaring a **prereq** onto a *modulefile* part of
   this reloading batch.

 When unloading a *modulefile*, following actions are triggered:

 * Dependent Unload: unload of the modulefiles declaring a non-optional
   **prereq** onto unloaded modulefile or declaring a non-optional **prereq**
   onto a modulefile part of this unloading batch. A **prereq** modulefile is
   considered optional if the **prereq** definition order is made of multiple
   modulefiles and at least one alternative modulefile is loaded.

 * Useless Requirement Unload: unload of the **prereq** modulefiles that have
   been automatically loaded for either the unloaded modulefile, an unloaded
   dependent modulefile or a modulefile part of this useless requirement
   unloading batch. Modulefiles are added to this unloading batch only if
   they are not required by any other loaded modulefiles.

 * Dependent Reload: reload of the modulefiles declaring a **conflict** or an
   optional **prereq** onto either the unloaded modulefile, an unloaded
   dependent or an unloaded useless requirement or declaring a **prereq** onto
   a modulefile part of this reloading batch.

 In case a loaded *modulefile* has some of its declared constraints
 unsatisfied (pre-required modulefile not loaded or conflicting modulefile
 loaded for instance), this loaded *modulefile* is excluded from the automatic
 reload actions described above.

 For the specific case of the **switch** sub-command, where a modulefile is
 unloaded to then load another modulefile. Dependent modulefiles to Unload are
 merged into the Dependent modulefiles to Reload that are reloaded after the
 load of the switched-to modulefile.

 Automated module handling mode enablement is defined in the following order
 of preference: **--auto**/**--no-auto** command line switches,
 then **MODULES_AUTO_HANDLING** environment variable, then the default set in
 **modulecmd.tcl** script configuration. Which means **MODULES_AUTO_HANDLING**
 overrides default configuration and **--auto**/**--no-auto** command line
 switches override every other ways to enable or disable this mode.

**MODULES_AVAIL_INDEPTH**

 If set to **1**, enable in depth search results for **avail** sub-command. If
 set to **0** disable **avail** sub-command in depth mode. Other values are
 ignored.

 When in depth mode is enabled, modulefiles and directories contained in
 directories matching search query are also included in search results. When
 disabled these modulefiles and directories contained in matching directories
 are excluded.

 **avail** sub-command in depth mode enablement is defined in the following
 order of preference: **--indepth**/**--no-indepth** command line switches,
 then **MODULES_AVAIL_INDEPTH** environment variable, then the default set in
 **modulecmd.tcl** script configuration. Which means **MODULES_AVAIL_INDEPTH**
 overrides default configuration and **--indepth**/**--no-indepth** command
 line switches override every other ways to enable or disable this mode.

**MODULES_CMD**

 The location of the active module command script.

**MODULES_COLLECTION_PIN_VERSION**

 If set to **1**, register exact version number of modulefiles when saving a
 collection. Elsewhere modulefile version number is omitted if it corresponds
 to the explicitly set default version and also to the implicit default when
 **config** option *implicit_default* is enabled.

**MODULES_COLLECTION_TARGET**

 The collection target that determines what collections are valid thus
 reachable on the current system.

 Collection directory may sometimes be shared on multiple machines which may
 use different modules setup. For instance modules users may access with the
 same **HOME** directory multiple systems using different OS versions. When
 it happens a collection made on machine 1 may be erroneous on machine 2.

 When a target is set, only the collections made for that target are
 available to the **restore**, **savelist**, **saveshow** and **saverm**
 sub-commands. Saving collection registers the target footprint by suffixing
 the collection filename with ``.$MODULES_COLLECTION_TARGET``. Collection
 target is not involved when collection is specified as file path on the
 **saveshow**, **restore** and **save** sub-commands.

 For example, the **MODULES_COLLECTION_TARGET** variable may be set with
 results from commands like **lsb_release**, **hostname**, **dnsdomainname**,
 etc.

**MODULES_COLOR**

 Defines if output should be colored or not. Accepted values are *never*,
 *auto* and *always*.

 When color mode is set to *auto*, output is colored only if the standard
 error output channel is attached to a terminal.

 Colored output enablement is defined in the following order of preference:
 **--color** command line switch, then **MODULES_COLOR** environment variable,
 then **CLICOLOR** and **CLICOLOR_FORCE** environment variables, then the
 default set in **modulecmd.tcl** script configuration. Which means
 **MODULES_COLOR** overrides default configuration and the
 **CLICOLOR**/**CLICOLOR_FORCE** variables. **--color** command line switch
 overrides every other ways to enable or disable this mode.

 **CLICOLOR** and **CLICOLOR_FORCE** environment variables are also honored to
 define color mode. The *never* mode is set if **CLICOLOR** equals to **0**.
 If **CLICOLOR** is set to another value, it corresponds to the *auto* mode.
 The *always* mode is set if **CLICOLOR_FORCE** is set to a value different
 than **0**. Color mode set with these two variables is superseded by mode set
 with **MODULES_COLOR** environment variable.

**MODULES_COLORS**

 Specifies the colors and other attributes used to highlight various parts of
 the output. Its value is a colon-separated list of output items associated to
 a Select Graphic Rendition (SGR) code. It follows the same syntax than
 **LS_COLORS**.

 Output items are designated by keys. Items able to be colorized are:
 highlighted element (*hi*), debug information (*db*), tag separator (*se*);
 Error (*er*), warning (*wa*), module error (*me*) and info (*in*) message
 prefixes; Modulepath (*mp*), directory (*di*), module alias (*al*), module
 symbolic version (*sy*), module *default* version (*de*) and modulefile
 command (*cm*).

 See the Select Graphic Rendition (SGR) section in the documentation of the
 text terminal that is used for permitted values and their meaning as
 character attributes. These substring values are integers in decimal
 representation and can be concatenated with semicolons. Modules takes care of
 assembling the result into a complete SGR sequence (**\33[...m**). Common
 values to concatenate include 1 for bold, 4 for underline, 30 to 37 for
 foreground colors and 90 to 97 for 16-color mode foreground colors. See also
 https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters
 for a complete SGR code reference.

 No graphical rendition will be applied to an output item that could normaly
 be colored but which is not defined in the color set. Thus if
 **MODULES_COLORS** is defined empty, no output will be colored at all.

 The color set is defined for Modules in the following order of preference:
 **MODULES_COLORS** environment variable, then the default set in
 **modulecmd.tcl** script configuration. Which means **MODULES_COLORS**
 overrides default configuration.

**MODULES_EXTENDED_DEFAULT**

 If set to **1**, a specified module version is matched against starting
 portion of existing module versions, where portion is a substring separated
 from the rest of the version string by a ``.`` character. For example
 specified modules ``mod/1`` and ``mod/1.2`` will match existing  modulefile
 ``mod/1.2.3``.

 In case multiple modulefiles match specified module version and a single
 module has to be selected, explicitly set default version is returned if it
 is part of matching modulefiles. Elsewhere implicit default among matching
 modulefiles is returned if defined (see **MODULES_IMPLICIT_DEFAULT** section)

 This environment variable supersedes the value of the configuration option
 *extended_default* set in **modulecmd.tcl** script.

**MODULES_ICASE**

 When module specification are passed as argument to module sub-commands or
 modulefile Tcl commands, defines the case sensitiveness to apply to match
 them. When **MODULES_ICASE** is set to **never**, a case sensitive match is
 applied in any cases. When set to **search**, a case insensitive match is
 applied to the **avail**, **whatis** and **paths** sub-commands. When set to
 **always**, a case insensitive match is also applied to the other module
 sub-commands and modulefile Tcl commands for the module specification they
 receive as argument.

 Case sensitiveness behavior is defined in the following order of preference:
 **--icase** command line switch, which corresponds to the **always** mode,
 then **MODULES_ICASE** environment variable, then the default set in
 **modulecmd.tcl** script configuration. Which means **MODULES_ICASE**
 overrides default configuration and **--icase** command line switch overrides
 every other ways to set case sensitiveness behavior.

**MODULES_IMPLICIT_DEFAULT**

 Defines (if set to **1**) or not (if set to **0**) an implicit default
 version for modules without a default version explicitly defined (see
 Locating Modulefiles section in the :ref:`modulefile(4)` man page).

 Without either an explicit or implicit default version defined a module must
 be fully qualified (version should be specified in addition to its name) to
 get:

 * targeted by module **load**, **switch**, **display**, **help**, **test**
   and **path** sub-commands.

 * restored from a collection, unless already loaded in collection-specified
   order.

 * automatically loaded by automated module handling mechanisms (see
   **MODULES_AUTO_HANDLING** section) when declared as module requirement,
   with **prereq** or **module load** modulefile commands.

 An error is returned in the above situations if either no explicit or
 implicit default version is defined.

 This environment variable supersedes the value of the configuration option
 *implicit_default* set in **modulecmd.tcl** script. This environment variable
 is ignored if *implicit_default* has been declared locked in *locked_configs*
 configuration option.

**MODULES_LMALTNAME**

 A colon separated list of the alternative names set through
 **module-version** and **module-alias** statements corresponding to all
 loaded *modulefiles*. Each element in this list starts by the name of the
 loaded *modulefile* followed by all alternative names resolving to it. The
 loaded modulefile and its alternative names are separated by the ampersand
 character.

 This environment variable is intended for **module** command internal use to
 get knowledge of the alternative names matching loaded *modulefiles* in order
 to keep environment consistent when conflicts or pre-requirements are set
 over these alternative designations. It also helps to find a match after
 *modulefiles* being loaded when **unload**, **is-loaded** or **info-loaded**
 actions are run over these names.

**MODULES_LMCONFLICT**

 A colon separated list of the **conflict** statements defined by all loaded
 *modulefiles*. Each element in this list starts by the name of the loaded
 *modulefile* declaring the conflict followed by the name of all modulefiles
 it declares a conflict with. These loaded modulefiles and conflicting
 modulefile names are separated by the ampersand character.

 This environment variable is intended for **module** command internal
 use to get knowledge of the conflicts declared by the loaded *modulefiles*
 in order to keep environment consistent when a conflicting module is asked
 for load afterward.

**MODULES_LMNOTUASKED**

 A colon separated list of all loaded *modulefiles* that were not explicitly
 asked for load from the command-line.

 This environment variable is intended for **module** command internal
 use to distinguish the *modulefiles* that have been loaded automatically
 from modulefiles that have been asked by users.

**MODULES_LMPREREQ**

 A colon separated list of the **prereq** statements defined by all loaded
 *modulefiles*. Each element in this list starts by the name of the loaded
 *modulefile* declaring the pre-requirement followed by the name of all
 modulefiles it declares a prereq with. These loaded modulefiles and
 pre-required modulefile names are separated by the ampersand character. When
 a **prereq** statement is composed of multiple modulefiles, these modulefile
 names are separated by the pipe character.

 This environment variable is intended for **module** command internal
 use to get knowledge of the pre-requirement declared by the loaded
 *modulefiles* in order to keep environment consistent when a pre-required
 module is asked for unload afterward.

**MODULES_PAGER**

 Text viewer for use to paginate message output if error output stream is
 attached to a terminal. The value of this variable is composed of a pager
 command name or path eventually followed by command-line options.

 Paging command and options are defined for Modules in the following order of
 preference: **MODULES_PAGER** environment variable, then the default set in
 **modulecmd.tcl** script configuration. Which means **MODULES_PAGER**
 overrides default configuration.

 If **MODULES_PAGER** variable is set to an empty string or to the value
 *cat*, pager will not be launched.

**MODULES_RUNENV_<VAR>**

 Value to set to environment variable *<VAR>* for **modulecmd.tcl** run-time
 execution if *<VAR>* is referred in **MODULES_RUN_QUARANTINE**.

**MODULES_RUN_QUARANTINE**

 A space separated list of environment variable names that should be passed
 indirectly to **modulecmd.tcl** to protect its run-time environment from
 side-effect coming from their current definition.

 Each variable found in **MODULES_RUN_QUARANTINE** will have its value emptied
 or set to the value of the corresponding **MODULES_RUNENV_<VAR>** variable
 when defining **modulecmd.tcl** run-time environment.

 Original values of these environment variables set in quarantine are passed
 to **modulecmd.tcl** via **<VAR>_modquar** variables.

**MODULES_SEARCH_MATCH**

 When searching for modules with **avail** sub-command, defines the way query
 string should match against available module names. With **starts_with**
 value, returned modules are those whose name begins by search query string.
 When set to **contains**, any modules whose fully qualified name contains
 search query string are returned.

 Module search match style is defined in the following order of preference:
 **--starts-with** and **--contains** command line switches, then
 **MODULES_SEARCH_MATCH** environment variable, then the default set in
 **modulecmd.tcl** script configuration. Which means **MODULES_SEARCH_MATCH**
 overrides default configuration and **--starts-with**/**--contains** command
 line switches override every other ways to set search match style.

**MODULES_SET_SHELL_STARTUP**

 If set to **1**, defines when **module** command initializes the shell
 startup file to ensure that the **module** command is still defined in
 sub-shells. Setting shell startup file means defining the **ENV** and
 **BASH_ENV** environment variable to the Modules bourne shell initialization
 script. If set to **0**, shell startup file is not defined.

**MODULES_SILENT_SHELL_DEBUG**

 If set to **1**, disable any *xtrace* or *verbose* debugging property set on
 current shell session for the duration of either the module command or the
 module shell initialization script. Only applies to Bourne Shell (sh) and its
 derivatives.

**MODULES_SITECONFIG**

 Location of a site-specific configuration script to source into
 **modulecmd.tcl**. See also Modulecmd startup section.

 This environment variable is ignored if *extra_siteconfig* has been declared
 locked in *locked_configs* configuration option.

**MODULES_TERM_BACKGROUND**

 Inform Modules of the terminal background color to determine if the color set
 for dark background or the color set for light background should be used to
 color output in case no specific color set is defined with the
 **MODULES_COLORS** variable. Accepted values are **dark** and **light**.

**MODULES_UNLOAD_MATCH_ORDER**

 When a module unload request matches multiple loaded modules, unload firstly
 loaded module or lastly loaded module. Accepted values are **returnfirst**
 and **returnlast**.

**MODULES_USE_COMPAT_VERSION**

 If set to **1** prior to Modules package initialization, enable
 Modules compatibility version (3.2 release branch) rather main version
 at initialization scripts running time. Modules package compatibility
 version should be installed along with main version for this environment
 variable to have any effect.

**MODULES_VERBOSITY**

 Defines the verbosity level of the module command. Available verbosity levels
 from the least to the most verbose are:

 * silent: turn off error, warning and informational messages but does not
   affect module command output result.
 * concise: enable error and warning messages but disable informational
   messages.
 * normal: turn on informational messages, like a report of the additional
   module evaluations triggered by loading or unloading modules, aborted
   evaluation issues or a report of each module evaluation occurring during a
   **restore** or **source** sub-commands.
 * verbose: add additional informational messages, like a systematic report of
   the loading or unloading module evaluations.
 * debug: print debugging messages about module command execution.

 Module command verbosity is defined in the following order of preference:
 **--silent**, **--verbose** and **--debug** command line switches, then
 **MODULES_VERBOSITY** environment variable, then the default set in
 **modulecmd.tcl** script configuration. Which means **MODULES_VERBOSITY**
 overrides default configuration and **--silent**/**--verbose**/**--debug**
 command line switches overrides every other ways to set verbosity level.

**MODULES_WA_277**

 If set to **1** prior to Modules package initialization, enables workaround
 for Tcsh history issue (see https://github.com/cea-hpc/modules/issues/277).
 This issue leads to erroneous history entries under Tcsh shell. When
 workaround is enabled, an alternative *module* alias is defined which fixes
 the history mechanism issue. However the alternative definition of the
 *module* alias weakens shell evaluation of the code produced by modulefiles.
 Characters with special meaning for Tcsh shell (like *{* and *}*) may not be
 used anymore in shell alias definition elsewhere the evaluation of the code
 produced by modulefiles will return a syntax error.

**_LMFILES_**

 A colon separated list of the full pathname for all loaded *modulefiles*.

**<VAR>_modquar**

 Value of environment variable *<VAR>* passed to **modulecmd.tcl** in order
 to restore *<VAR>* to this value once started.

**<VAR>_modshare**

 Reference counter variable for path-like variable *<VAR>*. A colon
 separated list containing pairs of elements. A pair is formed by a path
 element followed its usage counter which represents the number of times
 this path has been enabled in variable *<VAR>*. A colon separates the
 two parts of the pair.


FILES
-----

|bold prefix|

 The **MODULESHOME** directory.

|bold etcdir|\ **/siteconfig.tcl**

 The site-specific configuration script of **modulecmd.tcl**. An additional
 configuration script could be defined using the **MODULES_SITECONFIG**
 environment variable.

|bold etcdir|\ **/rc**

 The system-wide modules rc file. The location of this file can be changed
 using the **MODULERCFILE** environment variable as described above.

**$HOME/.modulerc**

 The user specific modules rc file.

**$HOME/.module**

 The user specific collection directory.

|bold modulefilesdir|

 The directory for system-wide *modulefiles*. The location of the directory
 can be changed using the **MODULEPATH** environment variable as described
 above.

|bold libexecdir|\ **/modulecmd.tcl**

 The *modulefile* interpreter that gets executed upon each invocation
 of **module**.

|bold initdir|\ **/<shell>**

 The Modules package initialization file sourced into the user's environment.


SEE ALSO
--------

:ref:`modulefile(4)`

