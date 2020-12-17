.. _diff_v3_v4:

Differences between versions 3.2 and 4
======================================

This document lists functionality differences between Modules version 3.2 and Modules version 4. Modules version 3.2 is also referred in this document as *compatibility version* whereas Modules version 4 is also referred as *new main version*. Modules version 4 is based on what was previously called Modules-Tcl. The goal of this document is to reference the features of the compatibility version that are missing or behave differently on the new main version and the features that can only be found on this new version.

First part the of the document covers the features that are missing or that behave differently on Modules 4 than on compatibility version. The second part of the document covers the features that are specific to the Modules 4 version thus missing on compatibility version. Comparison takes as a basis version ``3.2.10`` of compatibility version against Modules version ``4.0``. Any change made past these versions will explicitly mention the release number starting from the difference appears or disappears.

Regarding missing features, this document only lists their name or the command line argument related to them. Please refer to the :ref:`module(1)` and the :ref:`modulefile(4)` man pages of the compatibility or main version to learn the details about these features.

Last but not least, the numerous bugs or limitations spotted on Modules ``3.2`` across the years have been fixed in Modules ``4.0``. The list of reported issues that have been solved are available on `the project code forge <https://github.com/cea-hpc/modules/milestone/1?closed=1>`_.

Features missing or with different behavior than compatibility version
----------------------------------------------------------------------

This section describes the features of the compatibility version that are not supported or that behave differently on Modules 4.


Package Initialization
^^^^^^^^^^^^^^^^^^^^^^

:envvar:`MODULESBEGINENV` environment snapshot functionality is not supported on version 4. Modules collection mechanism should be preferred to save and restore sets of enabled modulepaths and loaded modulefiles. Modules 4 also introduces a system configuration file *init/modulerc*, located in Modules installation directory. Starting version ``4.3``, this file could also be *etc/initrc*. This *modulerc*/*initrc* file is sourced by Modules shell initialization scripts and helps to setup the initial environment right after initializing the :command:`module` command.


Command line switches
^^^^^^^^^^^^^^^^^^^^^

**--human**

**--create**, **-c**

**--userlvl** lvl, **-u** lvl
 
 These command line switches are not supported on Modules 4. When these options are passed on the command-line, it produces an *Unsupported option* warning and command is ran with the unsupported switches ignored.
 
**--ter**

**--lon**

**--sil**

**--verb**
 
 These intermediate-form command line switches are not supported on Modules 4. Short or long switch name should be used instead.
 
:option:`-h`
 
 This command line switch is short version of :option:`--help` switch on Modules 4 whereas it is short version of **--human** switch on compatibility version.

:option:`--force`, :option:`-f`
 
 This command line switch was not supported starting Modules version ``4.0`` but reintroduced starting version ``4.2`` with a different meaning: instead of enabling an active dependency resolution mechanism ``--force`` command line switch now enables to by-pass dependency consistency when loading or unloading a *modulefile*.

:option:`--verbose`, :option:`-v`

:option:`--silent`, :option:`-s`

 These command line switches were not supported starting Modules version ``4.0`` but reintroduced starting version ``4.3``. However, reintroduced :option:`--silent` switch does not redirect stderr channel to ``/dev/null`` if stderr is found not to be a tty.

:option:`--icase`, :option:`-i`

 This command line switch was not supported starting Modules version ``4.0`` but reintroduced starting version ``4.4``. When :option:`--icase` switch is now set it applies to search query string and module specificiation on all sub-commands and modulefile Tcl commands.


Module Sub-Commands
^^^^^^^^^^^^^^^^^^^
On compatibility version, paths composing the :envvar:`MODULEPATH` environment variable may contain reference to environment variable. These variable references are resolved dynamically when :envvar:`MODULEPATH` is looked at during module sub-command action like :subcmd:`avail`. This feature was missing on Modules ``4.0`` but it has been re-introduced on Modules ``4.1``.

:subcmd:`update`

 This module sub-commands is not supported on Modules 4.

:subcmd:`clear`

 This command line switch was not supported starting Modules version ``4.0`` but reintroduced starting version ``4.3``. It now takes into account the :option:`--force` command-line switch to skip confirmation dialog.

:subcmd:`refresh`
 
 On compatibility version, only the shell aliases defined by the currently loaded modulefiles are set again on a :subcmd:`refresh` command. Whereas on new main version this command is an alias on the :subcmd:`reload` command that :subcmd:`unload` then :subcmd:`load` all the currently loaded modulefiles.

:subcmd:`help`
 
 Compatibility version redirects output made on stdout in ``ModulesHelp`` Tcl procedure to stderr.
 
 During an :subcmd:`help` sub-command, only the ``ModulesHelp`` Tcl procedure of a modulefile is interpreted on compatibility version. Version 4 interprets all the content of the modulefile, then call the ``ModulesHelp`` procedure if it exists.
 
 On version 4, ``ModulesHelp`` subroutine is not ran if an error occurred during the interpretation of the *modulefile* main body.

:subcmd:`display`
 
 On version 4, ``ModulesDisplay`` subroutine is not ran if an error occurred during the interpretation of the *modulefile* main body.

:subcmd:`avail`
 
 On compatibility version, the same Tcl interpreter is used for the interpretation of all *.modulerc* or *.version* files during an :subcmd:`avail` command but the state of this interpreter is not reset between each interpretation. So some variable and procedure definitions may spread from one interpretation to another on this compatibility version. Modules 4 reuses the same interpreter for all *.modulerc* or *.version* interpretation but it is cleaned between each interpretation to protect from definition spread.
 
 In case of :option:`--terse` or :option:`--long` mode, all enabled modulepaths will be displayed whether they hold result to display or not. Modules 4 outputs only the modulepaths where matching results are found. Modulepaths with no result to report are discarded from output.

:subcmd:`apropos`
 
 On Modules 4, *string* passed as argument is always searched in a case insensitive manner.

:subcmd:`load`
 
 On Modules 4, the value of an environment variable is set even if the new value is the same as the current value of this variable in environment.
 
 When an already loaded *modulefiles* is asked for load again, compatibility version will refresh the shell alias definition this modulefile hold if any, whereas Modules 4 will ignore the new load order.
 
 In case of *modulefile* loading another modulefile, if sub-modulefile load fails calling modulefile will still be loaded on compatibility version whereas Modules 4 will also abort calling modulefile load.

 Starting with version ``4.1``, content sent to the ``stdout`` channel during a *modulefile* interpretation is spooled to effectively transmit this content to stdout after rendering the environment changes made by this modulefile.

:subcmd:`unload`
 
 On Modules 4, the value of an environment variable is set even if the new value is the same as the current value of this variable in environment.
 
 Compatibility version enables to load a *modulefile* by passing on the command-line the name of a module alias or symbolic version pointing to this modulefile. However this module alias or symbolic version name cannot be used to unload the modulefile once loaded. Modules 4 enables to pass a module alias or symbolic version name to unload a loaded modulefile referred by this name.

 On versions ``4.0`` and ``4.1``, unloading an unexistent modulefile generates an *Unable to locate modulefile* error. Starting with version ``4.2``, unloading a module only looks at loaded module list and does not trigger a modulefile search. So starting version ``4.2`` the same behavior than Modules compatibility version is obtained.

 Starting with version ``4.1``, content sent to the ``stdout`` channel during a *modulefile* interpretation is spooled to effectively transmit this content to stdout after rendering the environment changes made by this modulefile.

 When the specified module to unload matches multiple loaded modules, Modules 4 unloads lastly loaded module whereas compatibility version unloads firstly loaded module. A configuration option ``unload_match_order`` has been introduced in version ``4.3`` and it enables to restore the behavior of compatibility version when it is set to ``returnfirst``.

:subcmd:`switch`
 
 When switching on version 4 an *old* modulefile by a *new* one, no error is raised if *old* modulefile is not currently loaded. In this situation compatibility version throws an error and abort switch action.
 
 When switching on Modules 4 an *old* modulefile by a *new* one, this *new* modulefile does not keep the position that the *old* modulefile had in the :envvar:`LOADEDMODULES` list as done on compatibility version but it is appended to the end of the list. Same goes for PATH-like environment variables: replaced PATH component is appended to the end or prepended to the beginning of the relative PATH-like variable, not appended or prepended relatively to the position hold by the swapped PATH component.
 
 When a modulefile loads another modulefile with a *module load* order, this sub-module is not unloaded when the top modulefile is swapped-out during a :subcmd:`switch` command on compatibility version. Version 4 interprets the swapped-out modulefile in unload mode, so the *module load* order is interpreted as *module unload* order and sub-module is unloaded.
 
:subcmd:`use`
 
 When the modulepath to enable is passed as a relative path, compatibility version will set it using passed relative name whereas Modules 4 will determine the corresponding absolute path and will register it rather passed relative name.
 
:subcmd:`unuse`
 
 Modules 4 provides path element counting feature which increases a reference counter each time a given path entry is added to a given environment variable. This feature also applies to the :envvar:`MODULEPATH` environment variable. As consequence a modulepath entry element is removed from the modulepath enabled list only if the related element counter is equal to 1. When unusing a modulepath if its reference counter is greater than 1, modulepath is kept enabled and reference counter is decreased by 1.

:subcmd:`whatis`

 On Modules 4, environment variable edition commands (*setenv*, *unsetenv*, *append-path*, *prepend-path* and *remove-path*) do no set variable to the defined value on the modulefile evaluation context during a :subcmd:`whatis` evaluation. Instead environment variables are initialized with an empty value if undefined, to avoid raising error when attempting access to an undefined element during the modulefile evaluation.

:subcmd:`initadd`

:subcmd:`initprepend`

:subcmd:`initswitch`
 
 On version 4 no message is displayed to give details on how list of modulefiles to load has been altered in initialization file.
 
:subcmd:`initrm`
 
 No message is displayed on Modules 4 to inform of the modulefiles that have been removed from the loading list in initialization file.
 
 Empty ``module load`` line is left on version 4 when last modulefile from a line is asked to be removed. On compatibility version ``module load null`` line is set in this case.

:subcmd:`initclear`
 
 Empty ``module load`` lines are left on version 4 whereas ``module load null`` lines are set on compatibility version.
 

Modules Specific Tcl Commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On version 4 the characters that have a special meaning when rendered in shells are escaped when used in value to set environment variables, shell alias or shell functions. Thus it is possible to set environment elements with value containing these special characters (like backticks). On version 3 these special characters were evaluated by shell when recording the environment changes implied by modulefile evaluation.

:mfcmd:`append-path`

:mfcmd:`prepend-path`
 
 Modules 4 produces an error when adding a bare colon character *:* as a path element to a path-like variable, as this colon cannot be distinguished from the colon used for path separator.
 
 Modules 4 supports adding or removing empty path element to a path-like variable, whereas compatibility version looses track of this path element when the path-like variable is modified afterward. Empty path element enables to set a leading colon character *:*, which has a specific meaning on some regular environment variable like :envvar:`MANPATH` or :envvar:`LD_LIBRARY_PATH`.

 When adding a path element to the :envvar:`MANPATH` environment variable, Modules 4 is treating this variable like any other whereas a special treatment was applied on compatibility version: a default MANPATH value, set at configure time, was appended in case :envvar:`MANPATH` variable was unset.

:mfcmd:`remove-path`
 
 Modules 4 provides path element counting feature which increases a reference counter each time a given path entry is added to a given environment variable. As consequence a path entry element is removed from a path-like variable only if the related element counter is equal to 1. If this counter is greater than 1, path element is kept in variable and reference counter is decreased by 1.

 When unloading a modulefile, :mfcmd:`remove-path` command is not applied to environment variable on Modules 4, whereas on compatibility version it is processed the exact same way than when loading modulefile.
 
:mfcmd:`exit`
 
 On Modules 4 code passed to the :mfcmd:`exit` Modules specific Tcl command will not be thrown to be the :command:`module` return value.
 
:mfcmd:`module-alias`

:mfcmd:`module-version`
 
 In case the specified aliased module or the symbolic version introduces a resolution loop with already defined aliases or symbolic versions, this new alias or symbolic version is not registered and an error message is raised. On compatibility version, alias or symbolic version introducing loop are registered as the modulefile resolution is not computed at registration time.
 
:mfcmd:`module-info`
 
 **module-info flags**
 
 **module-info trace**
 
 **module-info tracepat**
 
 **module-info user**
  
  These :mfcmd:`module-info` options are related to compatibility version-specific features so they are available on Modules 4 but with a dummy implementation that always returns false or an empty value.
  
 **module-info mode**
  
  During an :subcmd:`unload` sub-command, ``unload`` is returned instead of ``remove``. However if *mode* is tested against ``remove`` value, true will be returned.
  
  During a :subcmd:`switch` sub-command, ``unload`` then ``load`` is returned instead of ``switch1`` then ``switch2`` then ``switch3``. However if *mode* is tested against ``switch`` value, true will be returned.
  
 **module-info name**

  If the module name passed to the command-line has been specified as a full path name, the **module-info name** used in modulefile returns this file base name on compatibility version whereas it returns on Modules 4+ the full path name as it is identified by this name once loaded.

 **module-info version**
  
  Declared aliases or symbolic versions are not registered anymore if they introduce a resolution loop. As a result **module-info version** does not return an ``*undef*`` string value as it does not face resolution loop situation anymore.
  
 **module-info symbols**
  
  Declared aliases or symbolic versions are not registered anymore if they introduce a resolution loop. As a consequence symbolic versions introducing loop situation are not part anymore of the **module-info symbols** returned result as they are not registered.
  
  A symbolic version sets on a module alias will be propagated toward the resolution path to also apply to the relative *modulefile* if it still correspond to the same module name.
  
:mfcmd:`module-log`

:mfcmd:`module-trace`

:mfcmd:`module-user`

:mfcmd:`module-verbosity`
 
 These Modules specific Tcl commands are related to compatibility version-specific features so they are available on Modules 4 but with a dummy implementation that always displays a warning message saying the command is not implemented.
 
:mfcmd:`module-whatis`
 
 When multiple words are passed as argument to :mfcmd:`module-whatis` but they are not enclosed in double-quotes or curly braces they will be displayed as a single line on Modules 4 whereas compatibility version displays them as one line per word.
 
:mfcmd:`set-alias`
 
 Whereas compatibility version sets a shell function when variables are in use in alias value on Bourne shell derivatives, Modules 4 always defines a shell alias never a shell function.


Locating Modulefiles
^^^^^^^^^^^^^^^^^^^^

On version 4, when a module alias is set and overrides name of an existing directory, this alias is taken into account to locate the default version of this module name and the *modulefiles* locating in the directory are ignored.

When looking for an implicit default in a *modulefile* directory, aliases are taken into account in addition to *modulefiles* and directories to determine the highest numerically sorted element.

Modules 4 will resolve module alias or symbolic version passed to :subcmd:`unload` command to then remove the loaded modulefile pointed by the mentioned alias or symbolic version.

Modules 4 resolves module alias or symbolic version pointing to a *modulefile* located in another modulepath.

When locating *modulefiles* on Modules 4, if a *.modulerc*, a *.version*, a directory or a *modulefile* cannot be read during the search it is simply ignored with no error message produced. Visibility of *modulefiles* can thus be adapted to the rights the user has been granted. Exception is made when trying to directly access a directory or a *modulefile*. In this case, the access issue is returned as an error message. Access issue is also returned when a direct access is made to a module alias or a symbolic version targeting an unreadable *modulefile*.


Features specific to the new main version
-----------------------------------------

This section describes the features of Modules version 4 that are not supported on the compatibility version. Please refer to the above section for features supported by both versions but behaving differently.


Package Initialization
^^^^^^^^^^^^^^^^^^^^^^

Compatibility version does not support *fish*, *lisp*, *tcl* and *R* as code output.

On version 4 and for *sh*, *bash*, *ksh*, *zsh* and *fish* shells, text output, like listing from the :subcmd:`avail` command, is redirected from *stderr* to *stdout* after shell command evaluation if shell is in interactive mode. Starting version ``4.1``, this content redirection occurs if shell session is attached to a terminal.

Starting version ``4.5``, a new alias or function called :command:`ml` may be defined at initialization time, to provide a handy frontend to the :command:`module` command.

Modulecmd startup
^^^^^^^^^^^^^^^^^

Starting with version ``4.1``, :file:`modulecmd.tcl` sources upon invocation a site-specific configuration script named :file:`siteconfig.tcl`. This Tcl script enables to supersede any global variable or procedure definition of modulecmd.tcl.


Command line switches
^^^^^^^^^^^^^^^^^^^^^

:option:`--debug`, :option:`-D`

:option:`--default`, :option:`-d`

:option:`--latest`, :option:`-L`
 
 These command line switches are not supported on compatibility version.
 
:option:`--paginate`

:option:`--no-pager`

 These command line switches appeared on version ``4.1`` and are not supported on compatibility version.

:option:`--auto`

:option:`--no-auto`

 These command line switches appeared on version ``4.2`` and are not supported on compatibility version.

:option:`--indepth`

:option:`--no-indepth`

:option:`--color`

:option:`--starts-with`, :option:`-S`

:option:`--contains`, :option:`-C`

 These command line switches appeared on version ``4.3`` and are not supported on compatibility version.

:option:`--json`, :option:`-j`

 This command line switch appeared on version ``4.5`` and is not supported on compatibility version.

:option:`--trace`, :option:`-T`

:option:`--all`, :option:`-a`

:option:`-DD`

 These command line switches appeared on version ``4.6`` and are not supported on compatibility version.

:option:`-vv`

 This command line switche appeared on version ``4.7`` and is not supported on compatibility version.


Module Sub-Commands
^^^^^^^^^^^^^^^^^^^

All module sub-commands will return a non-zero exit code in case of error whereas on compatibility version issues that occurred do not lead to an exit of the :command:`module` command with a non-zero code.

Starting with version ``4.1``, :command:`module` function for all scripting languages, like Perl or Python, always returns a value. In case of error, a *false* boolean value is returned instead of raising a fatal exception. For module sub-commands returning a text value, the module function will actually return this value. In all other cases a *true* boolean value is returned.


:subcmd:`reload`

:subcmd:`source`

:subcmd:`search`

:subcmd:`save`

:subcmd:`restore`

:subcmd:`saverm`

:subcmd:`saveshow`

:subcmd:`savelist`

:subcmd:`path`

:subcmd:`paths`

:subcmd:`autoinit`

:subcmd:`aliases`

:subcmd:`test`
 
 These module sub-commands are not supported on compatibility version.

:subcmd:`append-path`

:subcmd:`prepend-path`

:subcmd:`remove-path`

:subcmd:`is-loaded`

:subcmd:`is-saved`

:subcmd:`is-used`

:subcmd:`is-avail`

:subcmd:`info-loaded`

 These module sub-commands appeared on version ``4.1`` and are not supported on compatibility version.

:subcmd:`config`

 This module sub-command appeared on version ``4.3`` and is not supported on compatibility version.
 
:subcmd:`sh-to-mod`

 This module sub-command appeared on version ``4.6`` and is not supported on compatibility version.
 
:subcmd:`avail`

:subcmd:`whatis`

:subcmd:`apropos`
 
 Non-critical errors are not displayed on these sub-commands. Only valid results are returned.
 
 Module aliases are included in the result of these sub-commands. They are displayed in the module path section where they are defined or in a *global/user modulerc* section for aliases set in user's or global modulerc file. A ``@`` symbol is added in parenthesis next to their name to distinguish them from *modulefiles*.
 
 Search may be performed with an alias or a symbolic version-name passed as argument.
 
 Arguments to these :file:`avail`, :file:`whatis` and :file:`apropos` commands may use wildcard characters to express glob patterns.


Collections
^^^^^^^^^^^

Modules collections are not supported on compatibility version.


Environment
^^^^^^^^^^^

:envvar:`MODULECONTACT`

:envvar:`MODULES_COLLECTION_TARGET`

:envvar:`MODULES_USE_COMPAT_VERSION`

:envvar:`<VAR>_modshare`

 These environment variables are not supported on compatibility version.

:envvar:`MODULES_CMD`

:envvar:`MODULES_COLLECTION_PIN_VERSION`

:envvar:`MODULES_PAGER`

:envvar:`MODULES_RUNENV_\<VAR\>`

:envvar:`MODULES_RUN_QUARANTINE`

:envvar:`MODULES_SILENT_SHELL_DEBUG`

:envvar:`<VAR>_modquar`

 These environment variables appeared on version ``4.1`` and are not supported on compatibility version.

:envvar:`MODULES_AUTO_HANDLING`

:envvar:`MODULES_LMALTNAME`

:envvar:`MODULES_LMCONFLICT`

:envvar:`MODULES_LMNOTUASKED`

:envvar:`MODULES_LMPREREQ`

 These environment variables appeared on version ``4.2`` and are not supported on compatibility version.

:envvar:`MODULES_AVAIL_INDEPTH`

:envvar:`MODULES_COLOR`

:envvar:`MODULES_COLORS`

:envvar:`MODULES_IMPLICIT_DEFAULT`

:envvar:`MODULES_SEARCH_MATCH`

:envvar:`MODULES_SET_SHELL_STARTUP`

:envvar:`MODULES_SITECONFIG`

:envvar:`MODULES_TERM_BACKGROUND`

:envvar:`MODULES_UNLOAD_MATCH_ORDER`

:envvar:`MODULES_VERBOSITY`

:envvar:`MODULES_WA_277`

 These environment variables appeared on version ``4.3`` and are not supported on compatibility version.

:envvar:`MODULES_ADVANCED_VERSION_SPEC`

:envvar:`MODULES_EXTENDED_DEFAULT`

:envvar:`MODULES_ICASE`

 These environment variables appeared on version ``4.4`` and are not supported on compatibility version.

:envvar:`MODULES_ML`

 This environment variable appeared on version ``4.5`` and is not supported on compatibility version.

:envvar:`MODULES_LMSOURCESH`

:envvar:`MODULES_NEARLY_FORBIDDEN_DAYS`

 These environment variables appeared on version ``4.6`` and are not supported on compatibility version.

:envvar:`MODULES_SHELLS_WITH_KSH_FPATH`

:envvar:`MODULES_IMPLICIT_REQUIREMENT`

:envvar:`MODULES_LMTAG`

:envvar:`MODULES_TAG_ABBREV`

:envvar:`MODULES_TAG_COLOR_NAME`

 These environment variables appeared on version ``4.7`` and are not supported on compatibility version.


Modules Specific Tcl Commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:mfcmd:`conflict`

:mfcmd:`prereq`

 Starting with version ``4.2``, these Modules-specific Tcl commands support being called with a symbolic modulefile or a modulefile alias passed as argument.

:mfcmd:`module`
 
 In case of ``module load`` command specifying multiple *modulefiles*, when mode is set to ``unload`` these *modulefiles* will be unloaded in the reverse order to ensure correct handling of prerequisites.

 Starting with version ``4.7``, the ``load``, ``unload`` and ``switch`` sub-commands support the ``--not-req`` option to avoid recording a prereq or a conflict requirement toward specified modules.

:mfcmd:`module-info`
 
 **module-info command**
  
  This :mfcmd:`module-info` option is not supported on compatibility version.

 **module-info loaded**
  
  This :mfcmd:`module-info` option appeared on version ``4.1`` and is not supported on compatibility version.

 **module-info usergroups**

 **module-info username**

  These :mfcmd:`module-info` options appeared on version ``4.6`` and are not supported on compatibility version.

 **module-info tags**

  This :mfcmd:`module-info` options appeared on version ``4.7`` and is not supported on compatibility version.

:mfcmd:`append-path`

 Starting with version ``4.1``, :mfcmd:`append-path` handles being called with multiple *value* arguments and option ``--duplicates`` is added.

:mfcmd:`prepend-path`

 Starting with version ``4.1``, :mfcmd:`prepend-path` handles being called with multiple *value* arguments and option ``--duplicates`` is added.

:mfcmd:`remove-path`

 Starting with version ``4.1``, :mfcmd:`remove-path` handles being called with multiple *value* arguments and option ``--index`` is added.

:mfcmd:`is-loaded`

 Starting with version ``4.1``, :mfcmd:`is-loaded` supports being called with no argument passed. In this case, it returns *true* if any modulefile is currently loaded, *false* otherwise.

 Starting with version ``4.2``, :mfcmd:`is-loaded` supports being called with a symbolic modulefile or a modulefile alias passed as argument.

 This Modules-specific Tcl command was not enabled for *modulerc* evaluation starting Modules version ``4.0`` but it has been reintroduced starting version ``4.2.1``.

:mfcmd:`is-saved`

:mfcmd:`is-used`

:mfcmd:`is-avail`

:mfcmd:`module-virtual`

 These Modules-specific Tcl commands appeared on version ``4.1`` and are not supported on compatibility version.

:mfcmd:`set-function`

:mfcmd:`unset-function`

 These Modules-specific Tcl commands appeared on version ``4.2`` and are not supported on compatibility version.

:mfcmd:`source-sh`

:mfcmd:`module-hide`

:mfcmd:`module-forbid`

 These Modules-specific Tcl commands appeared on version ``4.6`` and are not supported on compatibility version.

:mfcmd:`versioncmp`

:mfcmd:`module-tag`

 These Modules-specific Tcl commands appeared on version ``4.7`` and are not supported on compatibility version.


Modules Variables
^^^^^^^^^^^^^^^^^

:mfvar:`ModuleTool`

:mfvar:`ModuleToolVersion`

 These Modules-specific Tcl variables appeared on version ``4.7`` and are not supported on compatibility version.
