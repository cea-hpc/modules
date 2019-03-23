.. _NEWS:

Release notes
=============

This file describes changes in recent versions of Modules. It primarily
documents those changes that are of interest to users and admins.

Modules 4.2.3 (2019-03-23)
--------------------------

* Add all the module dependency-related internal information to those saved
  prior a modulefile evaluation in order to correctly restore internal state
  in case modulefile evaluation fails.
* Init: in shell initialization scripts, initialize ``MANPATH`` if not set
  with a value that preserves ``manpath`` system configuration even after
  addition of paths to this variable by modulefiles. (fix issue#224)
* Enable to define an entire path entry to the ``MODULEPATH`` variable which
  corresponds to a variable reference only. (fix issue#223)
* Cookbook: add the *modulefiles-in-git* recipe. (contribution from Scott
  Johnson)
* When ``module switch`` commands are found in modulefiles, track switched-off
  modulefile as a conflict and switched-to modulefile as a requirement to
  apply same behaviors than for ``module load`` and ``module unload`` commands
  in modulefiles. If ``module switch`` has only one argument, do not define a
  conflict toward switched-off modulefile. *CAUTION: it is not recommended to
  use `module switch` command in modulefiles*. (fix issue#229)
* When unloading a module, revert ``module switch`` commands found in
  modulefile: switched-on module is converted to a ``module unload``, like for
  ``module load`` command. Nothing is done for switched-off module, like for
  ``module unload`` command. (fix issue#226)
* For default element in a modulefile directory which is a module alias that
  points to a modulefile, when this modulefile is loaded, it receives as
  alternative names the eventual module aliases set on the distant directory
  holding the alias pointing to it. (fix issue#231)
* When unloading a module that contains ``module load`` or ``module switch``
  commands in its modulefile, select for unload the automatically loaded
  requirement module which has been loaded prior its dependent. (fix
  issue#232)
* Doc: describe Emacs settings useful for adhering to coding conventions in
  CONTRIBUTING guide. (fix issue #233 with contribution from Ben Bowers)
* When looking for a loaded or loading dependency requirement, select among
  the eventual multiple candidates the closest match to the dependent module.
* During the unload of a module, if the unload of one of its dependent (by the
  *Dependent Unload* mechanism) fails, abort the whole unload process.
  Exception made if the force mode is enabled. In this case failing module
  stays loaded and the *Dependent Unload* mechanism continues with next module
  to unload.
* During the unload of a module, if the unload of one of its useless
  requirements (by the *Useless Requirement Unload* mechanism) fails, keep the
  requirements of this failing module loaded. Such error is reported as a
  warning and it does not stop the whole unload process. (fix issue#240)
* During the load or the unload of a module, if the unload of one of its
  dependent (by the *Dependent Reload* mechanism) fails, abort the whole
  unload or load process. Exception made if the force mode is enabled. In this
  case failing module stays loaded and *Dependent Reload* mechanism continues
  with next module to unload. This failing module is removed from the
  *Dependent Reload* list, so it will not take part of the load phrase of the
  mechanism. (fix issue#239)
* During the load or the unload of a module, if the load of one of its
  dependent (by the *Dependent Reload* mechanism) fails, abort the whole
  unload or load process. Exception made if the force mode is enabled. In this
  case failing module stays loaded and *Dependent Reload* mechanism continues
  with next module to load. When the mechanism is applied during a ``switch``
  command, force mode is enabled by default on the load phase. (fix issue#241)
* When reloading all loaded modules with the ``reload`` sub-command, if one
  reloading module fails to unload or load, abort the whole reload process to
  preserve environment sanity. (fix issue#237)
* During the unload of a module when the automated module handling mode is
  disabled and this module declares its requirements with the ``module load``
  modulefile command. If the unload of one of its useless requirements (by the
  *Useless Requirement Unload* mechanism) fails, whole unload process is not
  aborted and continue with next module to unload. (fix issue#238)
* Contrib: add ``mtreview`` utility script that analyzes test suite log file
  to compare actual and expected output of failed test. ``mt`` does not output
  the full test suite logs anymore but only the information produced by
  ``mtreview`` on failed tests.
* Install: exclude Continuous Integration configurations from dist tarballs.


Modules 4.2.2 (2019-02-17)
--------------------------

* Correct the *Dependent Unload* mechanism when it triggers the unload of 2
  modules making together a requirement from another module. This module is
  now also added to the dependent modules to unload.
* Doc: add a cookbook section in the documentation and port there the 3
  pre-existing recipes: *inhibit-report-info*, *top-priority-values* and
  *unload-firstly-loaded*.
* Doc: add a CONTRIBUTING guide.
* Doc: fix a typo on the Python initialization example in module man page.
* Doc: add a FAQ entry to describe the use of module from Makefile. (with
  contribution from Robert McLay)
* Trim any white-space, newline or ``;`` characters at the beginning or end of
  the function body passed to set-function modulefile command.
* Init: add recognition of the ``--auto``, ``--no-auto`` and ``--force``
  command-line switches in fish shell completion script.
* Init: add recognition of the ``--auto``, ``--no-auto``, ``--force``,
  ``--paginate`` and ``--no-pager`` command-line switches in zsh shell
  completion script.
* When the load of a modulefile is asked but a conflict is registered against
  this modulefile by an already loaded module, the load evaluation is now
  performed and the conflict is checked after this evaluation. If the conflict
  is still there, this evaluation (and the evaluation of its requirements) is
  rolled back. (fix issue#216)
* Init: fix ``_module_not_yet_loaded`` alias in tcsh completion script to
  handle situation when ``noclobber`` variable is set. Also ensure actual
  ``rm`` command is called and not an alias. (fix issue#219)
* Fix warning message when the load of a modulefile is forced over a reflexive
  conflict (message was reported twice).
* When looking at the dependency of a loaded module, only consider requirement
  loaded before dependent module (holding a prior position in the loaded
  module list) as valid. Those loaded after dependent module are considered as
  an unmet dependency thus they are not taking part in the *Dependent Unload*,
  the *Useless Requirement Unload* and the *Dependent Reload* mechanisms.


Modules 4.2.1 (2018-11-11)
--------------------------

* Cookbook: add the *inhibit-report-info* recipe.
* Cookbook: port *unload-firstly-loaded* and *top-priority-values* recipes to
  v4.2.
* Init: fix listing of loaded modules for *fish* and *tcsh* shell completions.
* Init: fix saved collection listing when no collection found for *bash*,
  *zsh*, *tcsh* and *fish* shell completions.
* Adapt ``system`` modulefile Tcl command to execute the command passed as
  argument through shell, like it is performed on compatibility version. (fix
  issue#205) 
* Correctly filter modulefile search memory cache entries when using a full
  search result to search later on a specific modulefile.
* Prefix debug messages by information on the current modulefile or modulerc
  interpreter if any.
* Init: fix listing of loaded modules on unload and switch sub-commands for
  *bash* shell completion.
* Refrain ``module unload`` modulefile command from unloading a module
  required by another loading module.
* Enable ``is-loaded`` modulefile Tcl command in modulerc interpretation
  context, like done on compatibility version. (fix issue#207)
* Check a required module is not already loading before attempting to load it.
  Helps to handle cyclic dependencies.
* Compute loaded modules requirement dependency relations without cycle and
  consider the module closing the cycle in a constraint violation state to
  avoid reloading loops on the *Dependent Reload* mechanism.
* Safely unset dependency reference when computing dependency relations as
  some dependencies expressed may target same module.
* Ensure a loaded module matching multiple entries of a same *or* ``prereq``
  will just be considered as one module matching this requirement.
* Init: quote prompt in *csh* and *tcsh* script with ``:q`` rather double
  quotes to accommodate prompts with embedded newlines. (fix issue#209 with
  contribution from Satya Mishra)
* Init: skip shell environment alteration if ``autoinit`` command fails. (fix
  issue#208)
* Reword path-like variable element counter reference handling to simply
  ignore the counter values not coherent with the content of related
  path-like variable. (fix issue#206)


Modules 4.2.0 (2018-10-18)
--------------------------

* Add ``chdir`` and ``puts`` environment settings to the per-modulefile
  evaluation saved context. So previous values of these settings are restored
  in case of evaluation failure.
* Fix save and restore of ``x-resource`` environment settings on the
  per-modulefile evaluation context.
* Use the correct warning procedure to report the full reference counter
  inconsistency message (so this message is fully inhibited during global
  ``whatis`` evaluations).
* Make ``append-path``, ``prepend-path``, ``remove-path`` and ``unsetenv``
  commands alter ``env`` Tcl global array during ``display``, ``help``,
  ``test`` or ``whatis`` evaluation modes. Thus an invalid argument passed to
  these commands will now raise error on these modes. (see
  :ref:`v42-variable-change-through-modulefile-evaluation` section in
  MIGRATING document)
* On ``whatis`` mode, ``append-path``, ``prepend-path``, ``remove-path``,
  ``setenv`` and ``unsetenv`` commands initialize variables if undefined but
  do not set them to their accurate value for performance concern.
* Clear value instead of unsetting it during an unload mode evaluation of
  ``setenv`` or ``*-path`` commands to avoid breaking later reference to the
  variable in modulefile.
* Make ``getenv`` command returns value on ``help``, ``test`` or ``whatis``
  evaluation modes. (fix issue#188)
* Add an argument to the ``getenv`` command to return the value of this
  argument if the queried variable is undefined.
* Use a different modulefile interpreter for each evaluation mode.
* Adapt the procedure called for each modulefile command depending on the
  evaluation mode to adapt behavior of these commands to the module command
  currently running.
* Report calling name and arguments for modulefile commands on ``display``
  mode. For the commands evaluated during this mode, trigger this report at
  the end of the evaluation.
* Inhibit ``chdir``, ``conflict``, ``module``, ``module-log``,
  ``module-trace``, ``module-user``, ``module-verbosity``, ``prereq``,
  ``set-alias``, ``system``, ``unset-alias``, ``x-resource`` commands on
  ``help``, ``test`` and ``whatis`` evaluation modes.
* Ignore ``chdir``, ``module``, ``module-trace``, ``module-verbosity``,
  ``module-user`` and ``module-log`` commands found during modulerc
  evaluation.
* Correctly restore an empty string value on sub-interpreter global variables
  when sanitizing this interpreter between two modulefile/modulerc
  evaluations.
* Cache in memory results of a modulefile search to reuse it in case of rerun
  instead of re-walking the filesystem.
* Evaluate global rc files once module sub-command is known and registered,
  so it can be queried during their evaluation.
* Rename ``_moduleraw`` shell function in ``_module_raw`` to use a common
  ``_module_`` prefix for all module-related internal shell functions.
* Install: add ``--enable-append-binpath`` and ``--enable-append-binpath``
  configure options to append rather prepend the bin or man directory when
  adding them to the relative environment variable.
* Doc: clarify documentation for module usage on scripting language like Perl
  or Python to mention that arguments to the ``module`` function should be
  passed as list and not as a single string.
* When interpreting a ``setenv`` modulefile order during an unload evaluation,
  variable is still set to be unset in generated shell code but it is set to
  the value defined on the ``setenv`` order in the interpreter context instead
  of being cleared.
* Register the conflicts defined by loaded modules in the environment
  (variable ``MODULES_LMCONFLICT``) and ensure they keep satisfied. (see
  :ref:`v42-conflict-constraints-consistency` section in MIGRATING document)
* Register the prereqs defined by loaded modules in the environment (variable
  ``MODULES_LMPREREQ``) and ensure they keep satisfied. (see
  :ref:`v42-prereq-constraints-consistency` section in MIGRATING document)
* Introduce the automated module handling mode, which consists in additional
  actions triggered when loading or unloading a modulefile to satisfy the
  dependency constraints it declares. Those actions are when loading a
  modulefile: the *Requirement Load* and the *Dependent Reload*. When
  unloading a modulefile, *Dependent Unload*, *Useless Requirement Unload* and
  *Dependent Reload* actions are triggered. (see
  :ref:`v42-automated-module-handling-mode` section in MIGRATING document)
* Track the loaded modules that have been automatically loaded (with
  environment variable ``MODULES_LMNOTUASKED``) to distinguish them from
  modules that have been explicitly asked by user. This information helps to
  determine what module becomes a useless requirement once all its dependent
  modules are unloaded.
* Track in saved collections the loaded modules that have been automatically
  loaded by add of a ``--notuasked`` argument to ``module load`` collection
  lines. So this information is restored in loaded environment when collection
  is restored. This ``--notuasked`` argument is ignored outside of a
  collection restore context.
* Consider modules loaded from a ``module source`` file as explicitly asked by
  user.
* Install: add ``--enable-auto-handling`` configure option to enable or
  disable the automatic modulefile handling mechanism.
* Process list of loaded modules or modules to load one by one during the
  ``restore``, ``purge`` and ``reload`` sub-commands whatever the auto
  handling mode is.
* Add the ability to control whether the auto_handling mode should be enabled
  or disabled with an environment variable called ``MODULES_AUTO_HANDLING`` or
  from the command-line with ``--auto`` and ``--no-auto`` switches. These
  command-line switches are ignored when called from modulefile.
* Init: add pager-related command-line options in shell completion scripts.
* Doc: describe ``MODULES_LMCONFLICT``, ``MODULES_LMPREREQ`` and
  ``MODULES_LMNOTUASKED`` in module.1 man page.
* Add ``-f`` and ``--force`` command-line switches to by-pass dependency
  consistency during ``load``, ``unload`` or ``switch`` sub-commands. (see
  :ref:`v42-by-passing-module-constraints` section in MIGRATING document)
* Disallow collection ``save`` or loaded modules ``reload`` if some loaded
  modules have some of their dependency constraints unsatisfied.
* The *Dependent Reload* action of a ``load``, ``unload`` and ``switch``
  sub-commands excludes modules that have unsatisfied constraints and includes
  modules whose constraints are satisfied again (when sub-command process
  solves a conflict for instance).
* Doc: describe ``--force``, ``--auto`` and ``--no-auto`` command-line
  switches and ``MODULES_AUTO_HANDLING`` variable in module.1 man page.
* Ignore directories ``.SYNC`` (DesignSync) and ``.sos`` (SOS) when walking
  through modulepath directory content. (contribution from Colin Marquardt)
* Install: look for ``make`` rather ``gmake`` on MSYS2.
* Fix ``exec()`` usage in Python module function definition to retrieve the
  correct return status on Python3.
* Cookbook: add the *top-priority-values* and *unload-firstly-loaded* recipes.
* Install: add ``gcc`` to the build requirements in RPM specfile.
* Silent any prereq violation warning message when processing *Dependent
  Reload* mechanism or ``purge`` sub-command.
* Doc: mention ``createmodule.sh`` and ``createmodule.py`` scripts in FAQ.
  (fix issue#189)
* Register all alternative names of loaded modules in environment with
  ``MODULES_LMALTNAME`` variable. These names correspond to the symbolic
  versions and aliases resolving to the loaded modules. Helps to consistenly
  solve ``conflict`` or ``prereq`` constraints set over these alternative
  names. (fix issue#143 / see
  :ref:`v42-consistency-module-load-unload-commands` section in MIGRATING
  document)
* Doc: describe ``MODULES_LMALTNAME`` in module.1 man page.
* Install: add ``--with-bin-search-path`` configure option to get in control
  of the path list used to search the tools required to build and configure
  Modules. (fix issue#164)
* Install: add ``--enable-silent-shell-debug-support`` configure option to add
  the ability to control whether or not code to support silent shell debug
  should be added to the module function and sh-kind initialization scripts.
  (fix issue#166)
* Install: add ``--enable-quarantine-support`` configure option to add the
  ability to control whether or not code to support quarantine mechanism
  should be added to the module function and initialization scripts.
  (fix issue#167)
* Check version set in modulefile magic cookie. If modulefile sets a version
  number greater than ``modulecmd.tcl`` script version, this modulefile is not
  evaluated like when no magic cookie is set at all. (fix issue#171 / see
  :ref:`v42-versioned-magic-cookie` section in MIGRATING document)
* Fix uninitialized variable in procedure producing list of element output.
  (fix issue#195)
* Ensure the consistency of ``module load`` modulefile command once the
  modulefile defining it has been loaded by assimilating this command to a
  ``prereq`` command. Thus the defined constraint is recorded in the
  ``MODULES_LMPREREQ`` environment variable. Same approach is used for
  ``module unload`` modulefile command which is assimilated to a ``conflict``
  command. Thus the defined constraint is recorded in the
  ``MODULES_LMCONFLICT`` environment variable. (see
  :ref:`v42-alias-symbolic-name-consistency` section in MIGRATING document)
* Only look at loaded modules when unloading so unloading an nonexistent
  modulefile does not produce an error anymore. (fix issue#199)
* Report error raised from modulefile evaluation as ``ERROR`` rather
  ``WARNING``, like when a conflict constraint is hit. Moreover this kind of
  evaluation error is now silenced on global evaluation like when proceding
  ``avail`` or ``search`` sub-commands.
* Record messages to report them by block on when processing a ``load`` or an
  ``unload`` modulefile evaluation to improve readability on these evaluating
  modes that may cascade additional actions. (see
  :ref:`v42-module-message-report` section in MIGRATING document)
* Foreground ``load``, ``unload``, ``switch`` and ``restore`` actions (ie.
  asked on the command-line) now report a summary of the additional load and
  unload evaluations that were eventually triggered in the process.
* Support ``del`` and ``remove`` aliases for ``unload`` sub-command like on
  compatibility version. (fix issue#200 with contribution from Wenzler)
* Correctly transmit the arguments along with the command to execute on
  ``system`` modulefile command. (fix issue#201)
* Contrib: add ``mt`` utility script which helps to run just specific part of
  the test suite.
* Introduce ``set-function`` and ``unset-function`` modulefile commands to
  define shell function on sh-kind and fish shells. (fix issue#193 with
  contribution from Ben Bowers)


Modules 4.1.4 (2018-08-20)
--------------------------

* Doc: fix typo on ``getenv`` command description in modulefile(4) man page
  and clarify this command should be preferred over ``::env`` variable to
  query environment variable value in modulefile.
* Init: fix ``bash`` and ``zsh`` completion scripts to enable Extended Regular
  Expression (ERE) on ``sed`` command with ``-E`` argument (rather ``-r``) for
  compatibility with OS X's and BSDs' sed. (fix issue#178)
* Handle default version sets on an hidden modulefile (were not found
  previously). (fix issue#177)
* Init: fix ``ksh`` initialization script for ksh88 compatibility. (fix
  issue#159)
* Install: use ``sed`` command rather ``grep`` and ``cut`` in ``configure``
  and ``Makefile`` scripts. (fix issue#175 with contribution from Michael
  Sternberg)
* Fix typo, tab indentation and pipe opening mode on ``createmodule.py``
  utility script. (contribution from Jan Synacek)
* Check ``ModulesVersion`` value set from ``.version`` rc file to ensure this
  value refers to a version name in current directory. Report error if a
  nested value is detected and ignore this value. (fix issue#176)


Modules 4.1.3 (2018-06-18)
--------------------------

* Make ``setenv`` command alter ``env`` Tcl global array during ``help``,
  ``test`` or ``whatis`` evaluation modes. (fix issue#160)
* Doc: describe MANPATH variable special treatment on compatibility version
  in diff_v3_v4 document.
* Initialize and export _moduleraw SH shell function if ``stderr`` is attached
  to a terminal. Was previously checking ``stdout``. (fix issue#169)
* For ``csh`` shells, quote code generated by modulecmd.tcl to pass it to the
  ``eval`` shell command.
* Escape special characters when producing code to define shell aliases (fix
  issue#165)
* Correct modulefile lookup when a modulefile directory is overwritten by a
  module alias definition but it contains an empty sub-directory. (fix
  issue#170)
* Doc: describe ``getenv`` command in modulefile(4) man page.
* Improve SH shell detection in profile.sh initialization script to use shell
  variable on ``bash`` or ``zsh`` to determine current shell name. (fix
  issue#173)


Modules 4.1.2 (2018-03-31)
--------------------------

* Add an example global rc file in ``contrib/etc`` directory that ensures
  ``MODULEPATH`` is always defined.
* Check ``HOME`` environment variable is defined on ``savelist`` and
  ``is-saved`` commands or raise error if not.
* Fix saving of deep module default version in collection when version pinning
  is disabled: if ``foo/bar/version`` is default version for ``foo``,
  collection will retain just ``foo`` (was retaining ``foo/bar``).
* Enable to save and restore collections containing full path modulefiles
  eventually with no modulepath defined.
* Run ``puts`` command not related to ``stderr`` or ``stdout`` channels in
  calling modulefile context to correctly get access to the targeted file
  channel. (fix issue#157)
* Quote ``autoinit`` result for eval interpretation on SH-kind shells to avoid
  parameter expansion to randomly occur on generated code depending on file
  or directory names of current working directory. (fix RH bug#1549664)
* Ignore empty elements found in ``MODULEPATH``, ``LOADEDMODULES`` or
  ``_LMFILES_`` to ensure all elements in these variables are non-empty
  strings.
* Raise error if loaded environment is in an inconsistent state when calling
  commands requiring correlation of information from the ``LOADEDMODULES`` and
  the ``_LMFILES_`` environment variables. Error raised on ``load``,
  ``unload``, ``switch``, ``reload``, ``purge``, ``list``, ``save`` and
  ``restore`` commands.  May affect ``info-loaded`` or ``is-loaded`` commands
  if module passed as argument to these command is specified as a full path
  modulefile.
* Fix ``list`` command to process loaded modules information before performing
  any content output.
* Install: adapt ``configure`` script and Makefiles to support installation on
  Cygwin system.
* Detect terminal width on Windows ``cmd`` terminal with ``mode`` command.
* Improve Windows ``cmd`` shell support: error code returned, echoing text,
  shell alias creation and removal, working directory change.
* Raise error when an empty module name is passed to module sub-commands like
  ``load``, ``display`` or ``unload``.
* Raise error when an empty collection name is passed to module sub-commands
  like ``save``, ``saveshow`` or ``restore``.
* Raise error when an empty path is passed to module ``unuse`` sub-command,
  like already done on ``use`` sub-command.
* Clear argument list if an empty module command name is passed.
* Fix ``module`` function definition for all shells in ``autoinit`` command to
  correctly handle empty-string parameters or parameters containing
  white-spaces, quotes, escape characters.
* Fix ``module`` function definition for Python to accept being called with no
  argument.
* Fix parameter expansion on ``module`` function for all SH-kind shells when
  quarantine mode is activated.
* Escape ``\`` character when producing R shell code.


Modules 4.1.1 (2018-02-17)
--------------------------

* Make separator lines, used on ``display`` command result for instance, fit
  small screen width.
* Install: give ability to build and install Modules from git repository
  without documentation if ``sphinx-build`` cannot be found.
* Install: adapt ``configure`` script and Makefiles to support installation on
  FreeBSD, Solaris and OS X systems. (fix issue#147)
* Rework code generated by ``autoinit`` for sh-kind shells to avoid use of
  local variables as those are defined differently through the sh variants.
  (also fix issue#147)
* Init: use a default value on undefined variables in sh-kind scripts to avoid
  unbound variables in bash ``-eu`` mode. (fix issue#151)
* Correctly detect terminal column number on Solaris.
* Init: fix csh init script to get compatibility with pure csh shell
* Sanitize content of ``MODULEPATH`` before using it at run-time, to make
  potential relative paths absolute, remove trailing slashes, etc. (fix
  issue#152)
* Check loaded modulefiles still exists before displaying statistics on them
  during a ``list`` action.
* Use a specific reference counter variable name (``MODULES_MODSHARE_<VAR>``
  instead of ``<VAR>_modshare``) for DYLD-specific variables. (fix issue#153)
* No error raise when updating a DYLD or LD path-like variable on OS X when
  System Integrity Protection (SIP) is enabled. In this situation, these
  variables are not exported in subshell context, so they appear undefined.
* Init: protect arguments passed to the ``_moduleraw`` sh function from
  interfering content of current working directory. (fix issue#154)
* Install: move ``hostname`` RPM requirement to the compat sub-package.
* Start pager process only if some text has to be printed. (partially fix
  issue#146)
* Ignore ``PAGER`` environment variable to configure Modules pager to avoid
  side effects coming from a general pager configuration not compatible with
  Modules pager handling. (fix issue#146)
* Do not blank anymore default Modules pager options if default pager is
  ``less`` when the ``LESS`` environment variable is defined. (fix issue#146)

.. warning:: With this bugfix release, changes have been made on the pager
   setup to avoid side effects coming from the system general pager
   configuration. As a result ``PAGER`` environment variable is now ignored
   and ``MODULES_PAGER`` should be used instead to adapt Modules pager
   configuration at run-time.


Modules 4.1.0 (2018-01-15)
--------------------------

* Extend stderr output redirection on sh-kind shells to all terminal-attached
  shell session, not only interactive shell session.
* Extend shell code produced by the ``autoinit`` command to perform the same
  environment initialization as done in ``init`` shell scripts (default value
  set for module-specific environment variables, parse or source of
  configuration files).
* Make init shell scripts rely on ``autoinit`` command to define the
  ``module`` command and setup its default environment.
* Fix error rendering code for Tcl shell by producing a call to the ``error``
  procedure.
* Introduce pager support to handle informational messages, using ``less``
  command with ``-eFKRX`` options by default. Environment variable
  ``MODULES_PAGER`` or ``PAGER`` may be used to supersede default pager
  command and options. ``--paginate`` and ``--no-pager`` switches enable or
  disable pager from the command line.
* Install: add ``--with-pager`` and ``--with-pager-opts`` configure options to
  define default pager command and its relative command-line options.
* Introduce quarantine mechanism to protect module execution against side
  effect coming from the current environment definition. Variables whose name
  has been put in ``MODULES_RUN_QUARANTINE`` will be emptied or set to the
  value hold by ``MODULES_RUNENV_<VAR>`` in the modulecmd.tcl run-time
  environment. Quarantine variable original value is then restored within
  modulecmd.tcl execution context once it has started.
* Install: add ``--with-quarantine-vars`` configure option to define at build
  time the ``MODULES_RUN_QUARANTINE`` and ``MODULES_RUNENV_<VAR>`` environment
  variables set in initialization scripts.
* Add ``MODULES_SILENT_SHELL_DEBUG`` environment variable to disable on sh
  shell and derivatives any ``xtrace`` or ``verbose`` debugging property for
  the duration of either the module command or the module shell initialization
  script. (fix issue#121)
* Change error code produced by modulecmd.tcl for the Tcl, Perl, Python, Ruby,
  CMake and R scripting languages to return a 'false' boolean value in case of
  error rather raising a fatal exception.
* Adapt module function definition for Tcl, Perl, Python, Ruby, CMake and R
  scripting languages to always return a value, result of the modulecmd.tcl
  run. When modulecmd.tcl run does not produce a specific status, a 'true'
  boolean value is returned. On CMake, resulting value is returned though a
  ``module_result`` global variable.
* Spool content sent to the stdout channel with ``puts`` command during a
  modulefile interpretation, to effectively transmit this content to stdout
  after rendering the environment changes made by this modulefile. (fix
  issue#113)
* Introduce ``append-path``, ``prepend-path``, ``remove-path`` and
  ``is-loaded`` module sub-commands, based on existing modulefile-specific
  Tcl commands. (fix issue#116)
* Introduce ``is-saved``, ``is-used`` and ``is-avail`` modulefile Tcl commands
  and module sub-commands to test availability of collection, modulepath or
  modulefile.
* Raise error when a call to ``path`` or ``paths`` module sub-commands is
  attempted during a modulefile interpretation. Both commands now return text
  rather print text on scripting languages. An empty string is returned in
  no match case instead of a false boolean value.
* Introduce ``module-info loaded`` modulefile command and its module
  sub-command counterpart ``info-loaded``. This new command returns name of
  the modules currently loaded corresponding to the name passed as argument.
  (fix issue#3)
* Fix ``is-loaded`` command to correctly handle multiple module names passed
  as argument (fix issue#138)
* Support no argument on ``is-loaded``, ``is-saved`` and ``is-used`` commands
  to return if anything is respectively loaded, saved or used.
* Interpret ``module source`` command set in modulefile in ``unload`` mode
  when the modulefile itself is interpreted in this mode.
* Consider a modulefile passed with name starting by ``./`` or ``../`` a full
  path name modulefile, like those starting by ``/``. These kind of names are
  converted to absolute path names, for instance to register them in loaded
  modulefile list during a ``load`` command.
* Correlate modulefile passed as full path name (starting by either ``./``,
  ``../`` or ``/``) to already loaded modulefile registered with regular
  module name (file name without its modulepath prefix) to prevent for
  instance from loading twice same modulefile. Correlate in the same way
  regular module name to already loaded full path name modulefile.
* Introduce ``MODULES_COLLECTION_PIN_VERSION`` environment variable to record
  modulefile version number when saving collections even if version
  corresponds to the default one. (fix issue#89)
* Fix location of ``etc/rc`` global RC file to ``@prefix@/etc/rc`` instead of
  ``$MODULESHOME/etc/rc`` not to depend on ``MODULESHOME`` environment
  variable value.
* Strengthen argument check for ``append-path``, ``prepend-path`` and
  ``remove-path`` modulefile Tcl commands and module sub-commands. Raise error
  if argument list is not correct.
* Fix support for the ``--delim=C`` argument form on ``append-path``,
  ``prepend-path`` and ``remove-path`` commands.
* Fix path reference counter handling in case path element is an empty string.
  Distinguish an empty path element from a variable set empty to clear it.
* Pass multiple path elements separated by delimiter character as one string
  on ``append-path``, ``prepend-path`` and ``remove-path`` commands.
* Accept multiple path element arguments on ``append-path``, ``prepend-path``
  and ``remove-path`` commands.
* Introduce the ``--duplicates`` argument option to ``append-path`` and
  ``prepend-path`` commands to add a path element already registered in
  variable.
* Introduce the ``--index`` argument option to ``remove-path`` command to
  delete a path entry by passing its position index in variable.
* Provide the ability to setup a site-specific configuration sourced at the
  start of ``modulecmd.tcl`` main procedure. This configuration is a Tcl
  script named ``siteconfig.tcl`` which enables to supersede any Tcl
  definition made in ``modulecmd.tcl``. Location of this file is controlled
  at configure time with the ``--etcdir`` option.
* Add the ability to handle paths containing reference to environment variable
  in ``MODULEPATH``. When these kind of paths are used by ``module`` command,
  the variable references are converted to their corresponding value or to an
  empty string if they are not defined.
* Enclose value set to environment variable on Tcl within curly braces rather
  double quotes to protect special characters in it from interpretation.
* Correctly parse ``.modulespath`` initialization file to handle lines without
  any ``#`` character or to handle files with no content to extract.
* Re-introduce the ``--enable-versioning`` configure option, which appends
  Modules version to installation prefix and deploy a ``versions`` modulepath
  shared between all versioning enabled Modules installation. A modulefile
  corresponding to Modules version is added to the shared modulepath and
  enables to switch from one Modules version to another.
* Fix removal of CMake generated temporary script file by stripping newline
  character from script file name.
* Add ``MODULES_CMD`` environment variable to expose path to the currently
  active module command script. This variable is set at initialization time.
* Introduce ``modulecmd`` wrapper script, installed in binary directory, which
  executes the active module command.
* Fix modulefile Tcl interpreter reset when handling list variables. (fix
  issue#145)
* Introduce 'module-virtual' modulefile Tcl command to associate a virtual
  module name to a modulefile. This module can be located with its virtual
  name and the associated modulefile is the script interpreted when loading,
  unloading, etc.
* Resolution of relative paths occurring during a modulefile interpretation to
  target a modulefile or a modulepath now takes the directory of the currently
  interpreted modulefile as the current working directory to solve the
  relative paths.


Modules 4.0.0 (2017-10-16)
--------------------------

Starting with this release, modules-tcl has become Modules. The following
changes describe the differences with last modules-tcl release (1.923). To
learn about the changes between this release and last Modules 3.2 release,
please see the :ref:`MIGRATING` document.

* Relax constraint on command-line argument position so options and
  switches can be passed either before or after command name.
* Report ``unsupported option`` warning rather stop on error when
  compatibility-version specific command-line switches are passed (
  ``--force``, ``--human``, ``--verbose``, ``--silent``, ``--create``,
  ``--icase``, ``--userlvl``).
* Keep empty ``module load`` line in shell configuration files after running
  the ``initrm`` or ``initclear`` commands.
* Always return the value of ``tcl_platform(osVersion)`` for ``uname release``
* Optimize code output, for Perl to only return ``1;`` once for a no-operation
  situation and for Python to not ``import os`` when there is only an error
  to render.
* Use value of system command ``uname -n`` for ``uname nodename``.
* Add support for CMake *shell*
* Ignore ``/`` character used as suffix in modulefile name passed on command
  line.
* Rename Perl initialization script in ``perl.pm`` and Python in
  ``python.py``.
* Add support for Ruby *shell* (with contribution from Tammo Tjarks)
* Add support for R *shell* (with contribution from Roy Storey)
* When a *default* is set for a given module name, target modulefile can
  be referred on as *modulename/default* in addition to just *modulename*.
* Locate symbolic versions on ``avail`` command even these symbols are set
  over a module alias or another symbolic version. In this situation the
  symbol spread along the resolution path until reaching a modulefile.
* Define a more standard shebang on modulecmd.tcl script.
* Determine modulefile corresponding to given module name using the loaded
  context only on ``unload`` situation.
* Enable to unload *mod/dir/subdir/vers* when unload of *mod* or *mod/dir*
  asked. Was previously working only if deep module to unload was also the
  default version for these root names.
* Make -l/-t switches mutually exclusive. Last switch mentioned on the
  command-line is honored.
* Output parsable modulepath header when -l/-t switches are enabled.
* When searching for a module in a given modulepath directory, if a module
  alias or a symbolic version matches searched module but the target of
  this alias or symbol is not found in current modulepath directory,
  search for this target restarting search from the first modulepath in
  list to ensure modulepath priority.
* Solve aliases or symbolic versions looking for all modulepaths on ``search``
  and ``paths`` commands. Was previously solved if their target was found in
  same modulepath directory.
* Add support for hidden *dot* modulefiles. A hidden modulefile does not
  appear in case of wild search, it is only returned when search is about
  its exact name.
* No table header print in --long mode on an ``avail`` command if no result
  are returned.
* Add blank line between displayed list of elements, for instance between
  modulepath content on ``avail`` command.
* Improve readability of error messages encountered during modulefile
  execution by putting Tcl error message first after the ``Module ERROR``
  prefix.
* Do not exit immediately when an internal error occurs in currently
  interpreted modulefile. Consider this interpretation as failed and
  continue to proceed the other modulefile arguments.
* When multiple modulefiles are passed on ``display``, ``help`` and ``test``
  commands only output one separator line between 2 interpreted modulefiles.
* Fix environment settings stack handling issue when restoring stack after
  a failed attempt to load a modulefile in a modulefile.
* Failed attempt to load or unload a modulefile within a modulefile now
  leads to this upper modulefile load or unload failure. Previously upper
  modulefile were loaded respectively unloaded even if its dependent
  sub-modulefile failed to load or unload.
* During a ``switch`` command, if the unloading part fails the loading part
  will not be tried. Unloading part fails if module to unload does not
  exist or its unload interpretation raise error.
* Init: use ``module source`` rather shell command ``source`` to load modulerc
  system configuration in sh-kind, csh-kind and fish shell init scripts.
* Install: transform configuration options to bind to an existing
  compatibility Modules version into option (--enable-compat-version) to
  build and install this compatibility version along with main version.
* Init: adapt initialization scripts to handle both main and compatibility
  version. By default a shell script enables main version and if the
  environment variable MODULES_USE_COMPAT_VERSION is set to 1, the
  compatibility version is enabled instead of main version.
* Install: import from compatibility version and install ``add.modules``
  and ``mkroot`` utility scripts (scripts developed by R.K. Owen).
* Install: update RPM spec file to handle compatibility version as a
  ``compat`` sub-package.
* Add completion script for Fish shell (contribution from BEFH).
* Doc: extend content of diff_v3_v4 to details all noticeable changes
  between v3.2 and v4.0.
* Doc: introduce MIGRATING guide to learn the major changes when moving
  from v3.2 to v4.0.
* Fix ``list`` command when full pathname modulefile is loaded (fix bug#132)
* Install: handle version number though git tags in scripts, documentation
  and RPM spec file.
* Doc: migrate documents from POD format to reStructuredText to benefit from
  Sphinx documentation framework and Read The Docs publishing capabilities.

Above changes describe the differences with modules-tcl release 1.923. To
learn about the changes between Modules 4.0 and last Modules 3.2 release,
please see the :ref:`MIGRATING` document.


modules-tcl-1.923 (2017-07-20)
------------------------------

* Fix ``aliases`` command when a global or user RC file is set.
* Find and solve global or user RC aliases and symbolic versions on
  ``search``, ``whatis`` and ``paths`` commands.
* Do not look at currently loaded modules to resolve the target of a module
  alias.
* Rework default and latest versions search on ``avail`` command. Correct
  display when at a given level a sub-directory element is last element in
  directory among modulefiles. Previously sub-directory was printed but
  last file among modulefiles was also printed (2 latest versions at the
  same level). A directory tagged "default" does not appear anymore in
  default listing result as its content (the default version found in that
  directory) will be displayed.
* When an alias is set and overrides name of an existing directory, take
  this alias into account for default and latest choice and ignore
  directory content.
* Bad default set will lead to no result displayed for the corresponding
  module in case of default ``avail`` display.
* Correct inclusion of aliases in output result when these aliases are not
  part of the exact same module path than module path of the search.
* Rewrite existing shell initialization file with initadd, initprepend,
  initswitch, initrm and initclear commands rather than writing a new
  file then copying this new file to replace the existing initialization
  file. In addition only re-writes shell initialization file if its content
  need to be altered.
* Raise an error on initadd, initprepend, initswitch, initrm and initclear
  commands when no ``module load`` line are found in shell initialization
  file.
* Normalize error messages for the various collection-related commands
  when collection cannot be accessed.
* Cleanup existing reference counters of a path list variable when this
  variable is altered by a ``setenv`` or an ``unsetenv`` command.
* Init: do not pollute tab-completion with moduleraw command. (Bert Wesarg)
* Make use of the same Tcl interp for each modulefile interpretation and
  use another one for each modulerc (but the same for each modulerc). By
  doing so we proceed like on C-version where same interpreter is used
  across modulefile or modulerc interpretation. Huge performance
  improvement is achieved with this change on commands making intensive
  use of interp like ``avail``. Interpreter state is reset from one
  interpretation to another: the initial variable and procedure state is
  restored before each new interpretation to avoid spread of definitions
  from one interpretation to another. Also in case of nested interpretation
  each interpretation level has its own interpreter so a module loaded by
  another does not influence the interpretation of the module loading it.
* Improve performance of aliases and symbolic versions resolution by
  computing these resolution at definition time. As a consequence
  resolution loop are not registered anymore and produce an error message
  when spotted not at display time.
* Reduce number of ``access`` system call by trying access to modulefile
  when reading the content of a modulefile directory rather testing access
  before trying it.
* No error raise on empty argument list for load. To cope with initadd
  behavior that requires at least an empty ``module load`` line in startup
  files. (fix SF bug#88)
* Fix initadd to handle load line without trailing space. Was previously
  expecting load directive to be written "module load "  to get a match.
  With fix, ``module load`` line will also be matched.
* Like C-version catch raised error when break or continue are called from
  outside of a loop to handle them as when they are called from modulefile
  main body. (fix SF bug#87)
* Return error on ``module use`` command when an empty path string is
  provided rather ignoring it.
* Workaround ``min`` and ``max`` functions and ``lreverse`` procedure for
  correct operations under Tcl version 8.4.
* Install: add --with-tclsh configure option to give the ability to choose
  the Tcl interpreter shell to setup in initialization scripts.
* Handle error raised from the ModulesDisplay, ModulesHelp and ModulesTest
  procedures in the same way than for the evaluation of the modulefile
  content. An error occurring during the evaluation of the modulefile
  content will lead to no evaluation of the ``display``, ``help`` and
  ``test`` command specific functions.
* Remove ``debug`` module command
* Doc: describe ``path``, ``paths`` and ``autoinit`` module command.
* Correct use of xrdb tool when not installed in default path.
* Fix ``init*`` module commands to behave more like C-version and document
  remaining differences in diff_with_c-version.
* Init: make ``sh`` init script closer to POSIX specification to support sh
  flavors different than Bash or Zsh like Dash.
* Fix column-mode display for very short width terminal.
* Install: introduce an ``install`` non-regression testsuite which is
  triggered by the ``make testinstall`` command and checks modules-tcl
  installation is operational.
* Init: fix modulerc load test on ``fish`` init script.
* Init: fix interactive shell test on ``sh`` init script.
* Install: add --enable-example-modulefiles configure option that install
  by default some modulefiles provided as example in the system modulefiles
  directory.
* Install: when uninstalling, do not remove modulefiles directory if it is
  not empty.
* Add completion script for Zsh shell.
* Add ``module test`` command to trigger when called execution of a
  ModulesTest procedure in target modulefile following same kind of
  mechanism than ``module help``.


modules-tcl-1.832 (2017-04-29)
------------------------------

* Fix ``getenv`` sub-command to correctly return environment variable value.
* Clarify in man-pages display of module alias and symbolic version-name on
  ``avail`` command and management of file access issue when locating
  modulefiles.
* Distinguish access issue (permission denied) from find issue (cannot
  locate) when trying to access directly a directory or a modulefile as
  done on ``load``, ``display`` or ``whatis`` commands. In addition on this
  kind of access, not readable .modulerc files are ignored rather producing a
  missing magic cookie error.
* When mode is set to unload, ``module load`` commands in modulefile are
  interpreted as ``module unload`` commands. To guaranty correct behavior
  regarding requirements, the module list passed to the load command is
  reversed to unload the modulefiles in the reverse order than they have
  been loaded.
* Correct ``display`` command to only report module commands set in
  modulefile and not those set in the various .modulerc on the path toward
  this modulefile.
* Fix bash and tcsh completion scripts to eliminate symbolic version names
  from ``avail`` command result.
* Improve ``avail`` command when a symbolic version-name is passed as
  argument to return the modulefile target of this symbolic version-name.
* When looking for an implicit default in a directory, now a module alias is
  taken into account so it can be returned as the last element in it
  (highest numerically sorted version).
* Fix ``list`` command to correctly display the ``default`` tag along loaded
  modules when set via a .version file.
* Fix long output of ``list`` command to display the symbolic version-names
  associated to each loaded module if any.
* Improve ``avail`` command to return alias module when an alias name is
  passed as argument.
* On a ``--default`` listing, a modulefile does not appear anymore if a
  directory is set default at the same level. On a ``--latest`` listing, a
  directory does not appear anymore if set default but not the latest.
* Read modulerc and validate its header in a single open/read/close sequence
  instead of two in order to reduce to number of IO operations during an
  ``avail`` command.
* Drastically reduce grid size computation time which removes overhead when
  displaying module avail results in column-mode.
* Translate module name to currently interpreted module name when name
  correspond to the last part this interpreted module only in case of
  symbolic version-name or alias resolution.
* Avoid resetting regular path (/usr/bin) or manpath (/usr/share/man) when
  switching from Tcl to C version in switchml utility.
* Raise error on x-resource if DISPLAY environment variable is not set.
* Fix lisp init script which was broken for environment change actions.


modules-tcl-1.775 (2017-03-07)
------------------------------

* Improve README with examples, requirements, links, etc. Also update
  INSTALL documentation with details on the new configure/make/make install
  process.
* Add display of a release date next to the version number when calling for
  ``--help`` or ``--version``.
* Update diff_with_c-version document to describe the features of the
  Tcl-version that are not supported on the C-version. Also state that the
  diff takes C version 3.2.10 against Tcl version 1.729 as a basis.
* Introduce ``switchml`` tool, a shell function (or alias for csh or tcsh
  shells) that swap currently enabled Modules version (C or Tcl) by the
  other version (C or Tcl). Configure option ``--with-cver-initdir`` must be
  defined to enable ``switchml`` in initialization script.
* Define a PATH and MANPATH in shell initialization scripts that point to
  the defined modules-tcl installation directories.
* Give ability to generate distribution tarball from the git repository with
  Makefile ``dist`` target.
* Introduce an installation process for this software following the
  configure/make/make install fashion. Configure step enables to choose
  installation paths and init scripts features to activate. Make step mainly
  translates init scripts with the configuration set. Make install creates
  target directories and copy files into them.
* Fix MODULESHOME setup in autoinit command to define it as an absolute path
  and set it to the upper directory when modulecmd.tcl is located in a ``bin``
  or a ``libexec`` directory.
* Correct alias and version resolution on avail command which was erroneous
  in case of a modulefile holding symbols (like ``default``) and targeted by
  aliases. Avail output was showing the aliases holding the symbols instead
  of the modulefile.


modules-tcl-1.729 (2017-02-01)
------------------------------

* Add documentation in module(1) man page on the modulefile collection
  concept and the relative ``save``, ``restore``, ``saverm``, ``saveshow``
  and ``savelist`` commands.
* Add document to list the differences of the functionalities that can be
  found on the C-version of the Modules package compared to the
  Tcl-version.
* Improve modulecmd.tcl shebang to only search ``tclsh`` once if found in
  PATH.
* Add ``module-info mode`` check against ``remove`` and ``switch`` values.
* Introduce ``module-info command`` Modules-specific Tcl command to
  distinguish complex ``load`` or ``unload`` commands that cannot be
  determined with ``module-info mode`` alone. For instance a modulefile can
  now be aware that a ``switch``, a ``restore`` or a ``purge`` command is
  currently being run.
* Enable usage of ``module-info`` Modules-specific Tcl command from a
  modulerc file.
* Fix ``module-info specified`` Modules-specific Tcl command.
* No exit raise on modulefile or modulerc error during ``avail``, ``aliases``,
  ``whatis`` and ``search`` commands to avoid harming results from these
  global commands if error exists in a few modulefiles.
* Exit with error code when a critical error is encountered when
  interpreting a modulefile or a modulerc.
* Inhibit non-critical error report raised from modulefiles during ``avail``,
  ``aliases``, ``whatis`` and ``search`` commands to avoid error flood when
  parsing all modulefiles or modulercs.
* Handle multiple lines of ``module-whatis`` Modules-specific Tcl commands
  defined for the same modulefile.
* Handle multiple arguments passed to the ``module-whatis`` Modules-specific
  Tcl commands. They are joined to get a single line of text.
* Return error on ``whatis`` command if searched modulefile is not found.


modules-tcl-1.704 (2017-01-20)
------------------------------

* Set path variable counter to 1 for paths without a known reference count
  (was previously set to 999999999).
* Introduce ``envml`` utility which acts as an application launcher where
  module commands are instantiated to setup environment before launching
  the given application.
* Always register paths provided to be part of MODULEPATH environment
  variable as absolute paths to get independent from the current working
  directory.
* Inhibit next modulefiles interpretation with ``exit`` Modules-specific Tcl
  command only if current mode is ``load``.
* Add argument to ``module-info shell`` and ``module-info shelltype`` to test
  current shell or shelltype value.
* Fix use of ``default`` version-name to not consider it as a module symbol
  if a modulefile is named ``default``.
* Fix path variable counters when ``:`` character is used in elements of a
  path-like variable.
* Update module(1) and modulefile(4) man pages to clear content specific to
  the C version of Modules and add content specific to or adapt content that
  behave differently on this Tcl version.
* Fix TCLSH variable issue in Python init script.


modules-tcl-1.677 (2017-01-04)
------------------------------

* Make ``switch`` command handle a single argument. The modulefile to switch
  to is the one passed on the command-line and the modulefile to unload is
  assumed to be the currently loaded module with the same root name as
  this modulefile specified on the command-line.
* Make ``switch`` command idempotent by always ending up with ``old`` unloaded
  and ``new`` loaded, whatever the starting situation is.
* Fix ``exit`` Modules-specific Tcl command.
* Add ``refresh`` command as alias on ``reload`` command.
* Add dummy ``module-log``, ``module-trace``, ``module-user`` and
  ``module-verbosity`` Modules-specific Tcl commands to enable support for
  modulefiles using them.
* Fix ``system`` Modules-specific Tcl command  to behave like described on
  the man page.
* Fix ``module list`` when module loaded with full path
* Disable ``g_force`` property by default to avoid loading a modulefile
  already loaded. It also avoids path element reference counting to get
  increased when the same module is asked twice for load.
* Clarify module-info ``mode`` option and set ``help`` mode on ``module help``
  command.
* Clarify module-info ``flags`` and ``user`` options.
* Handle empty or separator path on ``add-path`` ad ``unload-path`` commands.
* Delete environment variable targeted by an ``unsetenv`` command on
  ``unload`` mode if no value has been provided along. On display mode, print
  environment variable value if any has been passed to ``unsetenv`` command.
* When setting Tcl variable, enclose value within double quotes.
* Fix perl quoting style for variable set, escape single quotes rather
  double quotes.
* Call ``unuse`` command instead of ``use`` command on a ``module unload``.
* Fix ``continue`` Modules-specific Tcl command.
* Add ``chdir`` Modules-specific Tcl command.
* Fix ``break`` Modules-specific Tcl command.


modules-tcl-1.655 (2016-11-23)
------------------------------

* No display of modulepath header if no module found in it.
* Remove call to ``module aliases`` on ``module avail`` command, as aliases
  are now directly included in the ``avail`` results.
* Include module aliases in the displayed result of an ``avail`` command.
  Also display aliases defined in a global or user modulerc file.
* Exit with error code if error occurred on display or help commands.
* Fix module-info symbols resolution.
* Better handling of .modulerc and .version files when searching for a
  modulefile.
* Fix module-info version resolution.
* Fix module-info alias resolution.
* Register alias and version by the short module name and improve their
  resolution to avoid loop.
* Source $MODULERCFILE/modulerc when $MODULERCFILE is dir.
* Make it so you can do ``module avail un``, wildcard ``*`` character
  implied.


modules-tcl-1.632 (2016-09-06)
------------------------------

* Raise error if command does not receive the excepted number of arguments.
* Improve column-mode display to get a denser output on ``avail`` command.
* Standardize the output of Warning, Error, InternalBug and ErrorAndExit
  messages.
* Add short option -d for --delim on prepend-path.
* Introduce collection target concept to distinguish between machines,
  environments or domains that are incompatible with each other.
* Introduce ``saveshow`` command, to display content of saved collections.
* Improve ``save`` and ``restore`` commands to handle collection specified as
  absolute or relative file path.
* Introduce ``saverm`` command, to delete saved collections.
* Enable to ``restore`` collection with multiple modulefiles specified on the
  same line.
* Fix ``restore`` command when there is no module to load in collection.
* Fix ``restore`` command when collection fully rewind module paths.
* Fix ``restore`` command to preserve module path order set in collection.
* Raise error if try to save an empty environment in a collection.


modules-tcl-1.602 (2016-08-13)
------------------------------

* Add support for Fish shell.
* Import recent tests added to C-version on 10-use and 50-cmds testsuites.
* Add short option -d for --delim on append-path and remove-path.
* Fix load and implement unload x-resource.
* Fix Python code that was broken or not Python3-compliant. Fixed code is
  used to define the module command, to render error and to process
  x-resource.
* Always dictionary-sort (also called numerical-sort) list of modulefiles
  or list of collections.
* Fix bash completion script to be compliant with bash posix mode.


modules-tcl-1.578 (2014-12-24)
------------------------------

* First release to be described in this NEWS file but it does not mean this
  is the first version of modules-tcl as this Modules flavor is born in
  2002.
* At this stage, modules-tcl handles a majority of the module commands and
  modulefile Tcl commands available on C version.
