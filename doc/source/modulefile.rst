.. _modulefile(4):

modulefile
==========


DESCRIPTION
-----------

*modulefiles* are written in the Tool Command Language, :manpage:`Tcl(n)` and
are interpreted by the :file:`modulecmd.tcl` program via the :ref:`module(1)`
user interface. *modulefiles* can be loaded, unloaded, or switched on-the-fly
while the user is working; and can be used to implement site policies
regarding the access and use of applications.

A *modulefile* begins with the magic cookie, ``#%Module``. A version number
may be placed after this string. The version number is useful as the
*modulefile* format may change thus it reflects the minimum version of
:file:`modulecmd.tcl` required to interpret the modulefile. If a version
number doesn't exist, then :file:`modulecmd.tcl` will assume the *modulefile*
is compatible. Files without the magic cookie or with a version number greater
than the current version of :file:`modulecmd.tcl` will not be interpreted. If
the :mconfig:`mcookie_version_check` configuration is disabled the version
number set is not checked.

Each *modulefile* contains the changes to a user's environment needed to
access an application. Tcl is a simple programming language which permits
*modulefiles* to be arbitrarily complex, depending upon the application's
and the *modulefile* writer's needs. If support for extended tcl (tclX)
has been configured for your installation of the Modules package, you may
use all the extended commands provided by tclX, too.

A typical *modulefile* is a simple bit of code that set or add entries
to the :envvar:`PATH`, :envvar:`MANPATH`, or other environment variables. A
Modulefile is evaluated against current :file:`modulecmd.tcl`'s mode which
leads to specific evaluation results. For instance if the *modulefile* sets a
value to an environment variable, this variable is set when modulefile is
loaded and unset when modulefile is unloaded.

Tcl has conditional statements that are evaluated when the *modulefile* is
interpreted. This is very effective for managing path or environment changes
due to different OS releases or architectures. The user environment
information is encapsulated into a single *modulefile* kept in a central
location. The same *modulefile* is used by every user on any machine. So,
from the user's perspective, starting an application is exactly the same
irrespective of the machine or platform they are on.

*modulefiles* also hide the notion of different types of shells. From the
user's perspective, changing the environment for one shell looks exactly
the same as changing the environment for another shell. This is useful
for new or novice users and eliminates the need for statements such as "if
you're using the C Shell do this ..., otherwise if you're using the Bourne
shell do this ...". Announcing and accessing new software is uniform and
independent of the user's shell. From the *modulefile* writer's perspective,
this means one set of information will take care of every type of shell.

.. _Modules specific Tcl commands:

Modules Specific Tcl Commands
-----------------------------

The Modules Package uses commands which are extensions to the "standard"
Tool Command Language :manpage:`Tcl(n)` package. Unless otherwise specified,
the Module commands return the empty string. Some commands behave differently
when a *modulefile* is loaded or unloaded. The command descriptions assume
the *modulefile* is being loaded.

.. mfcmd:: append-path [-d C|--delim C|--delim=C] [--duplicates] variable value...

 See :mfcmd:`prepend-path`.

.. mfcmd:: break

 This is not a Modules-specific command, it's actually part of Tcl, which
 has been overloaded similar to the :mfcmd:`continue` and :mfcmd:`exit`
 commands to have the effect of causing the module not to be listed as loaded
 and not affect other modules being loaded concurrently. All non-environment
 commands within the module will be performed up to this point and processing
 will continue on to the next module on the command line. The :mfcmd:`break`
 command will only have this effect if not used within a Tcl loop though.

 An example: Suppose that a full selection of *modulefiles* are needed for
 various different architectures, but some of the *modulefiles* are not
 needed and the user should be alerted. Having the unnecessary *modulefile*
 be a link to the following notavail *modulefile* will perform the task
 as required.

 .. code-block:: tcl

      #%Module1.0
      ## notavail modulefile
      ##
      proc ModulesHelp { } {
          puts stderr "This module does nothing but alert the user"
          puts stderr "that the [module-info name] module is not available"
      }

      module-whatis "Notifies user that module is not available."
      set curMod [module-info name]
      if { [ module-info mode load ] } {
          puts stderr "Note: '$curMod' is not available for [uname sysname]."
      }
      break

.. mfcmd:: chdir directory

 Set the current working directory to *directory*.

.. mfcmd:: conflict modulefile...

 :mfcmd:`prereq` and :mfcmd:`conflict` control whether or not the *modulefile*
 will be loaded. The :mfcmd:`prereq` command lists *modulefiles* which must
 have been previously loaded before the current *modulefile* will be loaded.
 Similarly, the :mfcmd:`conflict` command lists *modulefiles* which
 :mfcmd:`conflict` with the current *modulefile*. If a list contains more than
 one *modulefile*, then each member of the list acts as a Boolean OR
 operation. Multiple :mfcmd:`prereq` and :mfcmd:`conflict` commands may be
 used to create a Boolean AND operation. If one of the requirements have not
 been satisfied, an error is reported and the current *modulefile* makes no
 changes to the user's environment.

 If an argument for :mfcmd:`prereq` is a directory and any *modulefile* from
 the directory has been loaded, then the prerequisite is met. For example,
 specifying X11 as a :mfcmd:`prereq` means that any version of X11, X11/R4 or
 X11/R5, must be loaded before proceeding.

 If an argument for :mfcmd:`conflict` is a directory and any other
 *modulefile* from that directory has been loaded, then a conflict will occur.
 For example, specifying X11 as a :mfcmd:`conflict` will stop X11/R4 and
 X11/R5 from being loaded at the same time.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

.. mfcmd:: continue

 This is not a modules specific command but another overloaded Tcl command
 and is similar to the :mfcmd:`break` or :mfcmd:`exit` commands except the
 module will be listed as loaded as well as performing any environment or Tcl
 commands up to this point and then continuing on to the next module on
 the command line. The :mfcmd:`continue` command will only have this effect if
 not used within a Tcl loop though.

.. mfcmd:: exit [N]

 This is not a modules specific command but another overloaded Tcl command
 and is similar to the :mfcmd:`break` or :mfcmd:`continue` commands. However,
 this command will cause the immediate cessation of this module and any
 additional ones on the command line. This module and the subsequent
 modules will not be listed as loaded. No environment commands will be
 performed in the current module.

.. mfcmd:: getenv [--return-value] variable [value]

 Returns value of environment *variable*. If *variable* is not defined,
 *value* is returned if set, an empty string is returned otherwise. The
 :mfcmd:`getenv` command should be preferred over the Tcl global variable
 ``env`` to query environment variables.

 When modulefile is evaluated in *display* mode, :mfcmd:`getenv` returns
 *variable* name prefixed with dollar sign (e.g., ``$variable``) unless if the
 ``--return-value`` option is set. When this option is set the value of
 environment *variable* or defined fallback value is returned in *display*
 mode.

 .. only:: html

    .. versionadded:: 4.0

    .. versionchanged:: 5.0
       An empty string is returned instead of ``_UNDEFINED_`` when *variable*
       is not defined and no *value* is set

    .. versionchanged:: 5.1
       Option ``--return-value`` added.

.. mfcmd:: getvariant [--return-value] variant [value]

 Returns value of designated *variant*. If *variant* is not defined, *value*
 is returned if set, an empty string is returned otherwise. The
 :mfcmd:`getvariant` command should be preferred over the
 :mfvar:`ModuleVariant` Tcl array to query a variant value.

 When modulefile is evaluated in *display* mode, :mfcmd:`getvariant` returns
 *variant* name enclosed in curly braces (e.g., ``{variant}``) unless if the
 ``--return-value`` option is set. When this option is set the value of
 *variant* or defined fallback value is returned in *display* mode.

 .. only:: html

    .. versionadded:: 4.8

    .. versionchanged:: 5.1
       Option ``--return-value`` added.

.. mfcmd:: is-avail modulefile...

 The :mfcmd:`is-avail` command returns a true value if any of the listed
 *modulefiles* exists in enabled :envvar:`MODULEPATH`. If a list contains more
 than one *modulefile*, then each member acts as a boolean OR operation. If an
 argument for :mfcmd:`is-avail` is a directory and a *modulefile* exists in
 the directory :mfcmd:`is-avail` would return a true value.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 .. only:: html

    .. versionadded:: 4.1

.. mfcmd:: is-loaded [modulefile...]

 The :mfcmd:`is-loaded` command returns a true value if any of the listed
 *modulefiles* has been loaded or if any *modulefile* is loaded in case no
 argument is provided. If a list contains more than one *modulefile*, then
 each member acts as a boolean OR operation. If an argument for
 :mfcmd:`is-loaded` is a directory and any *modulefile* from the directory has
 been loaded :mfcmd:`is-loaded` would return a true value.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

.. mfcmd:: is-saved [collection...]

 The :mfcmd:`is-saved` command returns a true value if any of the listed
 *collections* exists or if any *collection* exists in case no argument is
 provided. If a list contains more than one *collection*, then each member
 acts as a boolean OR operation.

 If :envvar:`MODULES_COLLECTION_TARGET` is set, a suffix equivalent to the
 value of this variable is appended to the passed *collection* name. In case
 no *collection* argument is provided, a true value will only be returned if
 a collection matching currently set target exists.

 .. only:: html

    .. versionadded:: 4.1

.. mfcmd:: is-used [directory...]

 The :mfcmd:`is-used` command returns a true value if any of the listed
 *directories* has been enabled in :envvar:`MODULEPATH` or if any *directory*
 is enabled in case no argument is provided. If a list contains more than one
 *directory*, then each member acts as a boolean OR operation.

 .. only:: html

    .. versionadded:: 4.1

.. mfcmd:: module [sub-command] [sub-command-options] [sub-command-args]

 Contains the same *sub-commands* as described in the :ref:`module(1)` man
 page in the :ref:`Module Sub-Commands` section. Exception made for the
 following sub-commands that can only be used outside of a modulefile context:
 :subcmd:`path`, :subcmd:`paths`, :subcmd:`autoinit`, :subcmd:`help`,
 :subcmd:`clear`, :subcmd:`sh-to-mod`, :subcmd:`edit`, :subcmd:`config`,
 :subcmd:`refresh`, :subcmd:`source` and :subcmd:`state`. Also the following
 sub-commands cannot be used but have a modulefile command counterpart:
 :subcmd:`prepend-path`, :subcmd:`append-path`, :subcmd:`remove-path`,
 :subcmd:`is-loaded`, :subcmd:`is-saved`, :subcmd:`is-used`,
 :subcmd:`is-avail` and :subcmd:`info-loaded`.

 This command permits a *modulefile* to :subcmd:`load` or :subcmd:`unload`
 other *modulefiles*. No checks are made to ensure that the *modulefile* does
 not try to load itself. Often it is useful to have a single *modulefile* that
 performs a number of ``module load`` commands. For example, if every user on
 the system requires a basic set of applications loaded, then a core
 *modulefile* would contain the necessary ``module load`` commands.

 The ``--not-req`` option may be set for the ``load``, ``unload`` and
 ``switch`` sub-commands to inhibit the definition of an implicit prereq or
 conflict requirement onto specified modules.

 On ``try-load`` sub-command, if specified *modulefile* is not found thus
 loaded, no implicit prereq requirement is defined over this module.

 The ``unuse`` sub-command accepts the ``--remove-on-unload``,
 ``--noop-on-unload``, ``--append-on-unload`` and ``--prepend-on-unload``
 options to control the behavior to apply when *modulefile* is unloaded. See
 :mfcmd:`remove-path` for further explanation.

 Command line switches :option:`--auto`, :option:`--no-auto` and
 :option:`--force` are ignored when passed to a :mfcmd:`module` command set in
 a *modulefile*.

 .. only:: html

    .. versionchanged:: 4.7
       Sub-command option ``--no-req`` added

    .. versionchanged:: 5.0
       :subcmd:`source` sub-command is not allowed anymore and ``source`` Tcl
       command should be used instead

    .. versionchanged:: 5.1
       Options ``--remove-on-unload``, ``--noop-on-unload``,
       ``--append-on-unload`` and ``--prepend-on-unload`` added for ``use``
       sub-command

.. mfcmd:: module-alias name modulefile

 Assigns the *modulefile* to the alias *name*. This command should be
 placed in one of the :file:`modulecmd.tcl` rc files in order to provide
 shorthand invocations of frequently used *modulefile* names.

 The parameter *modulefile* may be either

 * a fully qualified *modulefile* with name and version

 * a symbolic *modulefile* name

 * another *modulefile* alias

.. mfcmd:: module-forbid [options] modulefile...

 Forbid use of *modulefile*. An error is obtained when trying to evaluate a
 forbidden module. This command should be placed in one of the
 :file:`modulecmd.tcl` rc files.

 :mfcmd:`module-forbid` command accepts the following options:

 * ``--after datetime``
 * ``--before datetime``
 * ``--not-user {user...}``
 * ``--not-group {group...}``
 * ``--message {text message}``
 * ``--nearly-message {text message}``

 If ``--after`` option is set, forbidding is only effective after specified
 date time. Following the same principle, if ``--before`` option is set,
 forbidding is only effective before specified date time. Accepted date time
 format is ``YYYY-MM-DD[THH:MM]``. If no time (``HH:MM``) is specified,
 ``00:00`` is assumed. ``--after`` and ``--before`` options are not supported
 on Tcl versions prior to 8.5.

 If ``--not-user`` option is set, forbidding is not applied if the username of
 the user currently running :file:`modulecmd.tcl` is part of the list of
 username specified. Following the same approach, if ``--not-group`` option is
 set, forbidding is not applied if current user is member of one the group
 specified. When both options are set, forbidding is not applied if a match is
 found for ``--not-user`` or ``--not-group``.

 Error message returned when trying to evaluate a forbidden module can be
 supplemented with the *text message* set through ``--message`` option.

 If ``--after`` option is set, modules are considered *nearly forbidden*
 during a number of days defined by the :mconfig:`nearly_forbidden_days`
 :file:`modulecmd.tcl` configuration option
 (see :envvar:`MODULES_NEARLY_FORBIDDEN_DAYS`), prior reaching the expiry
 date fixed by ``--after`` option. When a *nearly forbidden* module is
 evaluated a warning message is issued to inform module will soon be
 forbidden. This warning message can be supplemented with the *text message*
 set through ``--nearly-message`` option.

 If a :mfcmd:`module-forbid` command applies to a *modulefile* also targeted
 by a :mfcmd:`module-hide --hard<module-hide>` command, this module is
 unveiled when precisely named to return an access error.

 Forbidden modules included in the result of an :subcmd:`avail` sub-command
 are reported with a ``forbidden`` tag applied to them. Nearly forbidden
 modules included in the result of an :subcmd:`avail` or a :subcmd:`list`
 sub-command are reported with a ``nearly-forbidden`` tag applied to them. See
 :ref:`Module tags` section in :ref:`module(1)`.

 The parameter *modulefile* may leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 .. only:: html

    .. versionadded:: 4.6

.. mfcmd:: module-hide [options] modulefile...

 Hide *modulefile* to exclude it from available module search or module
 selection unless query refers to *modulefile* by its exact name. This command
 should be placed in one of the :file:`modulecmd.tcl` rc files.

 :mfcmd:`module-hide` command accepts the following options:

 * ``--soft|--hard``
 * ``--hidden-loaded``
 * ``--after datetime``
 * ``--before datetime``
 * ``--not-user {user...}``
 * ``--not-group {group...}``

 When ``--soft`` option is set, *modulefile* is also set hidden, but hiding is
 disabled when search or selection query's root name matches module's root
 name. This soft hiding mode enables to hide modulefiles from bare module
 availability listing yet keeping the ability to select such module for load
 with the regular resolution mechanism (i.e., no need to use module exact name
 to select it)

 When ``--hard`` option is set, *modulefile* is also set hidden and stays
 hidden even if search or selection query refers to *modulefile* by its exact
 name.

 When ``--hidden-loaded`` option is set, hidden state also applies to the
 *modulefile* when it is loaded. Hidden loaded modules do not appear on
 :subcmd:`list` sub-command output, unless :option:`--all` option is set.
 Their loading or unloading informational messages are not reported unless the
 :envvar:`verbosity of Modules<MODULES_VERBOSITY>` is set to a level higher
 than ``verbose``. Hidden loaded modules are detected in any cases by state
 query commands like :mfcmd:`is-loaded`.

 If ``--after`` option is set, hiding is only effective after specified date
 time. Following the same principle, if ``--before`` option is set, hiding is
 only effective before specified date time. Accepted date time format is
 ``YYYY-MM-DD[THH:MM]``. If no time (``HH:MM``) is specified, ``00:00`` is
 assumed. ``--after`` and ``--before`` options are not supported on Tcl
 versions prior to 8.5.

 If ``--not-user`` option is set, hiding is not applied if the username of the
 user currently running :file:`modulecmd.tcl` is part of the list of username
 specified. Following the same approach, if ``--not-group`` option is set,
 hiding is not applied if current user is member of one the group specified.
 When both options are set, hiding is not applied if a match is found for
 ``--not-user`` or ``--not-group``.

 If the :option:`--all` option is set on :subcmd:`avail`, :subcmd:`aliases`,
 :subcmd:`whatis` or :subcmd:`search` sub-commands, hiding is disabled thus
 hidden modulefiles are included in module search. Hard-hidden modules (i.e.,
 declared hidden with ``--hard`` option) are not affected by :option:`--all`
 and stay hidden even if option is set. :option:`--all` option does not apply
 to *module selection* sub-commands like :subcmd:`load`. Thus in such context
 a hidden module should always be referred by its exact full name (e.g.,
 ``foo/1.2.3`` not ``foo``) unless if it has been hidden in ``--soft`` mode. A
 hard-hidden module cannot be unveiled or selected in any case.

 If several :mfcmd:`module-hide` commands target the same *modulefile*, the
 strongest hiding level is retained which means if both a regular, a
 ``--soft`` hiding command match a given module, regular hiding mode is
 considered. If both a regular and a ``--hard`` hiding command match a given
 module, hard hiding mode is retained. A set ``--hidden-loaded`` option is
 retained even if the :mfcmd:`module-hide` statement on which it is declared
 is superseded by a stronger :mfcmd:`module-hide` statement with no
 ``--hidden-loaded`` option set.

 Hidden modules included in the result of an :subcmd:`avail` sub-command are
 reported with a ``hidden`` tag applied to them. Hidden loaded modules
 included in the result of a :subcmd:`list` sub-command are reported with a
 ``hidden-loaded`` tag applied to them. This tag is not reported on
 :subcmd:`avail` sub-command context. See :ref:`Module tags` section in
 :ref:`module(1)`.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 .. only:: html

    .. versionadded:: 4.6

    .. versionchanged:: 4.7
       Option ``--hidden-loaded`` added.

.. mfcmd:: module-info option [info-args]

 Provide information about the :file:`modulecmd.tcl` program's state. Some of
 the information is specific to the internals of :file:`modulecmd.tcl`.
 *option* is the type of information to be provided, and *info-args* are any
 arguments needed.

 **module-info alias** name

  Returns the full *modulefile* name to which the *modulefile* alias *name*
  is assigned

 **module-info command** [commandname]

  Returns the currently running :file:`modulecmd.tcl`'s command as a string
  if no *commandname* is given.

  Returns ``1`` if :file:`modulecmd.tcl`'s command is *commandname*.
  *commandname* can be: ``load``, ``unload``, ``refresh``, ``reload``,
  ``source``, ``switch``, ``display``, ``avail``, ``aliases``, ``list``,
  ``whatis``, ``search``, ``purge``, ``restore``, ``help``, ``test`` or
  ``try-load``.

  .. only:: html

     .. versionadded:: 4.0

 **module-info loaded** modulefile

  Returns the names of currently loaded modules matching passed *modulefile*.
  The parameter *modulefile* might either be a fully qualified *modulefile*
  with name and version or just a directory which in case all loaded
  *modulefiles* from the directory will be returned. The parameter
  *modulefile* may also be a symbolic modulefile name or a modulefile alias.

  This command only returns the name and version of designated loaded module.
  The defined variants of the loaded module are not included in the returned
  string.

  .. only:: html

     .. versionadded:: 4.1

 **module-info mode** [modetype]

  Returns the current :file:`modulecmd.tcl`'s mode as a string if no
  *modetype* is given.

  Returns ``1`` if :file:`modulecmd.tcl`'s mode is *modetype*. *modetype* can
  be: ``load``, ``unload``, ``remove`` (alias of ``unload``), ``switch``,
  ``refresh``, ``nonpersist`` (alias of ``refresh``), ``display``, ``help``,
  ``test`` or ``whatis``.

 **module-info name**

  Return the name of the *modulefile*. This is not the full pathname for
  *modulefile*. See the `Modules Variables`_ section for information on the
  full pathname.

  This command only returns the name and version of currently evaluating
  *modulefile*. The defined variants are not included in the returned string.
  See :mfcmd:`getvariant` command or :mfvar:`ModuleVariant` array variable to
  get defined variant values for currently evaluating *modulefile*.

 **module-info shell** [shellname]

  Return the current shell under which :file:`modulecmd.tcl` was invoked if
  no *shellname* is given. The current shell is the first parameter of
  :file:`modulecmd.tcl`, which is normally hidden by the :command:`module`
  alias.

  If a *shellname* is given, returns ``1`` if :file:`modulecmd.tcl`'s current
  shell is *shellname*, returns ``0`` otherwise. *shellname* can be: ``sh``,
  ``bash``, ``ksh``, ``zsh``, ``csh``, ``tcsh``, ``fish``, ``tcl``, ``perl``,
  ``python``, ``ruby``, ``lisp``, ``cmake``, ``r``.

 **module-info shelltype** [shelltypename]

  Return the family of the shell under which *modulefile* was invoked if no
  *shelltypename* is given. As of :mfcmd:`module-info shell<module-info>` this
  depends on the first parameter of :file:`modulecmd.tcl`. The output reflects
  a shell type determining the shell syntax of the commands produced by
  :file:`modulecmd.tcl`.

  If a *shelltypename* is given, returns ``1`` if :file:`modulecmd.tcl`'s
  current shell type is *shelltypename*, returns ``0`` otherwise.
  *shelltypename* can be: ``sh``, ``csh``, ``fish``, ``tcl``, ``perl``,
  ``python``, ``ruby``, ``lisp``, ``cmake``, ``r``.

 **module-info specified**

  Return the module designation (name, version and variants) specified that
  led to current *modulefile* evaluation.

 **module-info symbols** modulefile

  Returns a list of all symbolic versions assigned to the passed
  *modulefile*.  The parameter *modulefile* might either be a full qualified
  *modulefile* with name and version, another symbolic *modulefile* name
  or a *modulefile* alias.

 **module-info tags** [tag]

  Returns all tags assigned to currently evaluated *modulefile* as a list of
  strings if no *tag* name is given (see :ref:`Module tags` section in
  :ref:`module(1)`)

  When tags are assigned to specific module variants, they are returned only
  if this variant is the one currently evaluated.

  Returns ``1`` if one of the tags applying to currently evaluated
  *modulefile* is *tag*. Returns ``0`` otherwise.

  .. only:: html

     .. versionadded:: 4.7

 **module-info type**

  Returns either ``C`` or ``Tcl`` to indicate which :command:`module` command
  is being  executed, either the C version or the Tcl-only version, to allow
  the *modulefile* writer to handle any differences between the two.

 **module-info usergroups** [name]

  Returns all the groups the user currently running :file:`modulecmd.tcl` is
  member of as a list of strings if no *name* is given.

  Returns ``1`` if one of the group current user running :file:`modulecmd.tcl`
  is member of is *name*. Returns ``0`` otherwise.

  If the Modules Tcl extension library is disabled, the :manpage:`id(1)`
  command is invoked to fetch groups of current user.

  .. only:: html

     .. versionadded:: 4.6

 **module-info username** [name]

  Returns the username of the user currently running :file:`modulecmd.tcl` as
  a string if no *name* is given.

  Returns ``1`` if username of current user running :file:`modulecmd.tcl` is
  *name*. Returns ``0`` otherwise.

  If the Modules Tcl extension library is disabled, the :manpage:`id(1)`
  command is invoked to fetch username of current user.

  .. only:: html

     .. versionadded:: 4.6

 **module-info version** modulefile

  Returns the physical module name and version of the passed symbolic
  version *modulefile*.  The parameter *modulefile* might either be a full
  qualified *modulefile* with name and version, another symbolic *modulefile*
  name or a *modulefile* alias.

.. mfcmd:: module-tag [options] tag modulefile...

 Associate *tag* to designated *modulefile*. This tag information will be
 reported along *modulefile* on :subcmd:`avail` and :subcmd:`list`
 sub-commands (see :ref:`Module tags` section in :ref:`module(1)`). Tag
 information can be queried during *modulefile* evaluation with the
 :mfcmd:`module-info tags<module-info>` modulefile command.
 :mfcmd:`module-tag` commands should be placed in one of the
 :file:`modulecmd.tcl` rc files.

 :mfcmd:`module-tag` command accepts the following options:

 * ``--not-user {user...}``
 * ``--not-group {group...}``

 If ``--not-user`` option is set, the tag is not applied if the username of
 the user currently running :file:`modulecmd.tcl` is part of the list of
 username specified. Following the same approach, if ``--not-group`` option is
 set, the tag is not applied if current user is member of one the group
 specified. When both options are set, the tag is not applied if a match is
 found for ``--not-user`` or ``--not-group``.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

 Tags inherited from other modulefile commands or module states cannot be set
 with :mfcmd:`module-tag`. Otherwise an error is returned. Those special tags
 are: ``auto-loaded``, ``forbidden``, ``hidden``, ``hidden-loaded``,
 ``loaded`` and ``nearly-forbidden``.

 When *tag* equals ``sticky`` or ``super-sticky``, designated *modulefile* are
 defined :ref:`Sticky modules`.

 .. only:: html

    .. versionadded:: 4.7

.. mfcmd:: module-version modulefile version-name...

 Assigns the symbolic *version-name* to the *modulefile*. This command
 should be placed in one of the :file:`modulecmd.tcl` rc files in order to
 provide shorthand invocations of frequently used *modulefile* names.

 The special *version-name* default specifies the default version to be
 used for module commands, if no specific version is given. This replaces
 the definitions made in the :file:`.version` file in former
 :file:`modulecmd.tcl` releases.

 The parameter *modulefile* may be either

 * a fully or partially qualified *modulefile* with name / version. If
   name is ``.`` (dot) then the current directory name is assumed to be the
   module name. (Use this for deep *modulefile* directories.)

 * a symbolic *modulefile* name

 * another *modulefile* alias

.. mfcmd:: module-virtual name modulefile

 Assigns the *modulefile* to the virtual module *name*. This command should be
 placed in rc files in order to define virtual modules.

 A virtual module stands for a module *name* associated to a *modulefile*. The
 modulefile is the script interpreted when loading or unloading the virtual
 module which appears or can be found with its virtual name.

 The parameter *modulefile* corresponds to the relative or absolute file
 location of a *modulefile*.

 .. only:: html

    .. versionadded:: 4.1

.. mfcmd:: module-whatis string

 Defines a string which is displayed in case of the invocation of the
 :subcmd:`module whatis<whatis>` command. There may be more than one
 :mfcmd:`module-whatis` line in a *modulefile*. This command takes no actions
 in case of :subcmd:`load`, :subcmd:`display`, etc. invocations of
 :file:`modulecmd.tcl`.

 The *string* parameter has to be enclosed in double-quotes if there's more
 than one word specified. Words are defined to be separated by whitespace
 characters (space, tab, cr).

.. mfcmd:: prepend-path [-d C|--delim C|--delim=C] [--duplicates] variable value...

 Append or prepend *value* to environment *variable*. The
 *variable* is a colon, or *delimiter*, separated list such as
 ``PATH=directory:directory:directory``. The default delimiter is a colon
 ``:``, but an arbitrary one can be given by the ``--delim`` option. For
 example a space can be used instead (which will need to be handled in
 the Tcl specially by enclosing it in ``" "`` or ``{ }``). A space, however,
 can not be specified by the ``--delim=C`` form.

 A reference counter environment variable is also set to know the number of
 times *value* has been added to environment *variable* when it is added more
 than one time. This reference counter environment variable is named by
 prefixing *variable* by :envvar:`__MODULES_SHARE_<__MODULES_SHARE_\<VAR\>>`.

 When *value* is already defined in environment *variable*, it is not added
 again or moved at the end or at the beginning of *variable*. Exception is
 made when the ``--duplicates`` option is set in which case *value* is added
 again to *variable*.

 If the *variable* is not set, it is created. When a *modulefile* is
 unloaded, :mfcmd:`append-path` and :mfcmd:`prepend-path` become
 :mfcmd:`remove-path`.

 If *value* corresponds to the concatenation of multiple elements separated by
 colon, or *delimiter*, character, each element is treated separately.

 .. only:: html

    .. versionchanged:: 4.1
       Option ``--duplicates`` added

.. mfcmd:: prereq modulefile...

 See :mfcmd:`conflict`.

.. mfcmd:: remove-path [options] variable value... [--append-on-unload|--prepend-on-unload value...]

 Remove *value* from the colon, or *delimiter*, separated list in
 *variable*.

 :mfcmd:`remove-path` command accepts the following options:

 * ``-d C|--delim C|--delim=C``
 * ``--index``
 * ``--remove-on-unload|--noop-on-unload|--append-on-unload|--prepend-on-unload``

 See :mfcmd:`prepend-path` or :mfcmd:`append-path` for further
 explanation of using an arbitrary delimiter. Every string between colons, or
 delimiters, in *variable* is compared to *value*. If the two match, *value*
 is removed from *variable* if its reference counter is equal to 1 or unknown.

 When ``--index`` option is set, *value* refers to an index in *variable*
 list. The string element pointed by this index is set for removal.

 When *modulefile* is unloaded, no operation is performed by default or if the
 ``--noop-on-unload`` option is set. If the ``--remove-on-unload`` option is
 set, *value* is removed. If the ``--append-on-unload`` option is set, append
 back *value* removed at load time or specific *value* if any set. If the
 ``--prepend-on-unload`` option is set, prepend back *value* removed at load
 time or specific *value* if any set. These options cannot be set if
 ``--index`` option is also set.

 Reference counter of *value* in *variable* denotes the number of times
 *value* has been added to *variable*. This information is stored in
 environment :envvar:`__MODULES_SHARE_variable<__MODULES_SHARE_\<VAR\>>`. When
 attempting to remove *value* from *variable*, relative reference counter is
 checked and *value* is removed only if counter is equal to 1 or not defined.
 Otherwise *value* is kept in *variable* and reference counter is decreased by
 1. If counter equals 1 after being decreased, *value* and its counter are
 removed from reference counter variable.

 If *value* corresponds to the concatenation of multiple elements separated by
 colon, or *delimiter*, character, each element is treated separately.

 .. only:: html

    .. versionchanged:: 4.1
       Option ``--index`` added

    .. versionchanged:: 5.1
       Options ``--remove-on-unload``, ``--noop-on-unload``,
       ``--append-on-unload`` and ``--prepend-on-unload`` added

.. mfcmd:: set-alias alias-name alias-string

 Sets an alias or function with the name *alias-name* in the user's
 environment to the string *alias-string*. For some shells, aliases are not
 possible and the command has no effect. When a *modulefile* is unloaded,
 :mfcmd:`set-alias` becomes :mfcmd:`unset-alias`.

.. mfcmd:: set-function function-name function-string

 Creates a function with the name *function-name* in the user's environment
 with the function body *function-string*. For some shells, functions are not
 possible and the command has no effect. When a *modulefile* is unloaded,
 :mfcmd:`set-function` becomes :mfcmd:`unset-function`.

 .. only:: html

    .. versionadded:: 4.2

.. mfcmd:: setenv [--set-if-undef] variable value

 Set environment *variable* to *value*. The :mfcmd:`setenv` command will also
 change the process' environment. A reference using Tcl's env associative
 array will reference changes made with the :mfcmd:`setenv` command. Changes
 made using Tcl's ``env`` associative array will **NOT** change the user's
 environment *variable* like the :mfcmd:`setenv` command. An environment
 change made this way will only affect the module parsing process. The
 :mfcmd:`setenv` command is also useful for changing the environment prior to
 the ``exec`` or :mfcmd:`system` command. When a *modulefile* is unloaded,
 :mfcmd:`setenv` becomes :mfcmd:`unsetenv`. If the environment *variable* had
 been defined it will be overwritten while loading the *modulefile*. A
 subsequent :subcmd:`unload` will unset the environment *variable* - the
 previous value cannot be restored! (Unless you handle it explicitly ... see
 below.)

 When the ``--set-if-undef`` option is set, environment variable is defined
 when *modulefile* is loaded only if not yet defined.

    .. versionchanged:: 5.1
       Option ``--set-if-undef`` added

.. mfcmd:: source-sh shell script [arg...]

 Evaluate with *shell* the designated *script* with defined *arguments* to
 find out the environment changes it does. Those changes obtained by comparing
 environment prior and after *script* evaluation are then translated into
 corresponding *modulefile* commands, which are then applied during modulefile
 evaluation as if they were directly written in it.

 When modulefile is unloaded, environment changes done are reserved by
 evaluating in the ``unload`` context the resulting modulefile commands, which
 were recorded in the :envvar:`__MODULES_LMSOURCESH` environment variable at
 ``load`` time.

 Changes on environment variables, shell aliases, shell functions and current
 working directory are tracked.

 Changes made on environment variable intended for Modules private use (e.g.,
 :envvar:`LOADEDMODULES`, :envvar:`_LMFILES_`, ``__MODULES_*``) are ignored.

 *Shell* could be specified as a command name or a fully qualified pathname.
 The following shells are supported: sh, dash, csh, tcsh, bash, ksh, ksh93,
 zsh and fish.

 .. only:: html

    .. versionadded:: 4.6

    .. versionchanged:: 5.1
       Changes on Modules private environment variable are ignored

.. mfcmd:: system string

 Run *string* command through shell. On Unix, command is passed to the
 ``/bin/sh`` shell whereas on Windows it is passed to ``cmd.exe``.
 :file:`modulecmd.tcl` redirects stdout to stderr since stdout would be parsed
 by the evaluating shell. The exit status of the executed command is returned.

.. mfcmd:: uname field

 Provide lookup of system information. Most *field* information are retrieved
 from the ``tcl_platform`` array (see the :manpage:`tclvars(n)` man page).
 Uname will return the string ``unknown`` if information is unavailable for
 the *field*.

 :mfcmd:`uname` will invoke the :manpage:`uname(1)` command in order to get
 the operating system version and :manpage:`domainname(1)` to figure out the
 name of the domain.

 *field* values are:

 * ``sysname``: the operating system name

 * ``nodename``: the hostname

 * ``domain``: the name of the domain

 * ``release``: the operating system release

 * ``version``: the operating system version

 * ``machine``: a standard name that identifies the system's hardware

.. mfcmd:: unset-alias alias-name

 Unsets an alias with the name *alias-name* in the user's environment.

.. mfcmd:: unset-function function-name

 Removes a function with the name *function-name* from the user's environment.

 .. only:: html

    .. versionadded:: 4.2

.. mfcmd:: unsetenv [options] variable [value]

 Unsets environment *variable*. When a *modulefile* is unloaded, no operation
 is performed unless if an optional *value* is defined, in which case
 *variable* is to *value*. The :mfcmd:`unsetenv` command changes the process'
 environment like :mfcmd:`setenv`.

 If the ``--noop-on-unload`` option is set, no operation is performed when
 *modulefile* is unloaded. If the ``--unset-on-unload`` option is set,
 environment *variable* is also unset when *modulefile* is unloaded. These
 behaviors are applied even if an optional *value* is defined.

 .. only:: html

    .. versionchanged:: 5.0
       *variable* is not unset when unloading *modulefile* and no optional
       value is provided

    .. versionchanged:: 5.1
       Options ``--noop-on-unload`` and ``--unset-on-unload`` added

.. mfcmd:: variant [--boolean] [--default value] name [value...]

 Declare :ref:`module variant<Module variants>` *name* with list of accepted
 *value* and instantiate it in the :mfvar:`ModuleVariant` array variable.

 Variant's value is selected through the module designation that leads to the
 modulefile evaluation. See `Advanced module version specifiers`_ section to
 learn how variants could be specified.

 Selected variant value is transmitted to the evaluating modulefile. A value
 must be specified for variant *name* and it must corresponds to a value in
 the accepted value list if such list is defined. Otherwise an error is
 raised. An exception is made if modulefile is evaluated in ``display`` mode:
 no error is raised if no value is specified for a given variant and variant
 is not instantiated in the :mfvar:`ModuleVariant` array variable. When no
 list of accepted value is defined, variant could be set to any value.

 When the ``--default`` option is set, variant *name* is set to the *value*
 associated with this option in case no value is specified for variant in
 module designation.

 If the ``--boolean`` option is set, variant *name* is defined as a Boolean
 variant. No list of accepted value should be defined in this case. All values
 recognized as Boolean value in Tcl are accepted (i.e., ``1``, ``true``,
 ``t``, ``yes``, ``y``, ``on``, ``0``, ``false``, ``f``, ``no``, ``n`` or
 ``off``). Boolean variants are instantiated in :mfvar:`ModuleVariant` using
 Tcl canonical form of Boolean value (i.e., ``0`` or ``1``).

 A variant which is not defined as a Boolean variant cannot define Boolean
 values in its accepted value list, exception made for the ``0`` and ``1``
 integers. An error is raised otherwise.

 A variant cannot be named ``version``. An error is raised otherwise.

 .. only:: html

    .. versionadded:: 4.8

    .. versionchanged:: 5.0
       No error raised if a defined variant is not specified when modulefile
       is evaluated in ``display`` mode

    .. versionchanged:: 5.1
       Accepted value list is made optional

.. mfcmd:: versioncmp version1 version2

 Compare version string *version1* against version string *version2*. Returns
 ``-1``, ``0`` or ``1`` respectively if *version1* is less than, equal to or
 greater than *version2*.

 .. only:: html

    .. versionadded:: 4.7

.. mfcmd:: x-resource [resource-string|filename]

 Merge resources into the X11 resource database. The resources are used to
 control look and behavior of X11 applications. The command will attempt
 to read resources from *filename*. If the argument isn't a valid file
 name, then string will be interpreted as a resource. Either *filename*
 or *resource-string* is then passed down to be :manpage:`xrdb(1)` command.

 *modulefiles* that use this command, should in most cases contain one or
 more :mfcmd:`x-resource` lines, each defining one X11 resource. The
 :envvar:`DISPLAY` environment variable should be properly set and the X11
 server should be accessible. If :mfcmd:`x-resource` can't manipulate the X11
 resource database, the *modulefile* will exit with an error message.

 Examples:

 ``x-resource /u2/staff/leif/.xres/Ileaf``

  The content of the *Ileaf* file is merged into the X11 resource database.

 ``x-resource [glob ~/.xres/ileaf]``

  The Tcl glob function is used to have the *modulefile* read different
  resource files for different users.

 ``x-resource {Ileaf.popup.saveUnder: True}``

  Merge the Ileaf resource into the X11 resource database.


Modules Variables
-----------------

.. mfvar:: ModulesCurrentModulefile

 The :mfvar:`ModulesCurrentModulefile` variable contains the full pathname of
 the *modulefile* being interpreted.

.. mfvar:: ModuleTool

 The :mfvar:`ModuleTool` variable contains the name of the *module*
 implementation currently in use. The value of this variable is set to
 ``Modules`` for this implementation.

 .. only:: html

    .. versionadded:: 4.7

.. mfvar:: ModuleToolVersion

 The :mfvar:`ModuleToolVersion` variable contains the version of the *module*
 implementation currently in use. The value of this variable is set to
 |code version| for this version of Modules.

 .. only:: html

    .. versionadded:: 4.7

.. mfvar:: ModuleVariant

 The :mfvar:`ModuleVariant` array variable contains an element entry for each
 defined variant associated to the value of this variant (e.g., the
 ``$ModuleVariant(foo)`` syntax corresponds to the value of variant ``foo`` if
 defined). A Tcl evaluation error is obtained when accessing an undefined
 variant in :mfvar:`ModuleVariant` array. Use preferably the
 :mfcmd:`getvariant` command to retrieve a variant value when this variant
 state is not known.

 The list of the currently defined variants can be retrieved with
 ``[array names ModuleVariant]`` Tcl code.

 .. only:: html

    .. versionadded:: 4.8

.. _Locating Modulefiles:

Locating Modulefiles
--------------------

Every directory in :envvar:`MODULEPATH` is searched to find the
*modulefile*. A directory in :envvar:`MODULEPATH` can have an arbitrary number
of sub-directories. If the user names a *modulefile* to be loaded which
is actually a directory, the directory is opened and a search begins for
an actual *modulefile*. First, :file:`modulecmd.tcl` looks for a file with
the name :file:`.modulerc` in the directory. If this file exists, its contents
will be evaluated as if it was a *modulefile* to be loaded. You may place
:mfcmd:`module-version`, :mfcmd:`module-alias` and :mfcmd:`module-virtual`
commands inside this file.

Additionally, before seeking for :file:`.modulerc` files in the module
directory, the global modulerc file and the :file:`.modulerc` file found at
the root of the modulepath directory are sourced, too. If a named version
default now exists for the *modulefile* to be loaded, the assigned
*modulefile* now will be sourced. Otherwise the file :file:`.version` is
looked up in the module directory.

If the :file:`.version` file exists, it is opened and interpreted as Tcl code
and takes precedence over a :file:`.modulerc` file in the same directory. If
the Tcl variable ``ModulesVersion`` is set by the :file:`.version` file,
:file:`modulecmd.tcl` will use the name as if it specifies a *modulefile* in
this directory. This will become the default *modulefile* in this case.
``ModulesVersion`` cannot refer to a *modulefile* located in a different
directory.

If ``ModulesVersion`` is a directory, the search begins anew down that
directory. If the name does not match any files located in the current
directory, the search continues through the remaining directories in
:envvar:`MODULEPATH`.

Every :file:`.version` and :file:`.modulerc` file found is interpreted as Tcl
code. The difference is that :file:`.version` only applies to the current
directory, and the :file:`.modulerc` applies to the current directory and all
subdirectories. Changes made in these files will affect the subsequently
interpreted *modulefile*.

If no default version may be figured out, an implicit default is selected when
this behavior is enabled (see :envvar:`MODULES_IMPLICIT_DEFAULT` in
:ref:`module(1)`). If disabled, module names should be fully qualified when no
explicit default is defined for them, otherwise no default version is found
and an error is returned. If enabled, then the highest numerically sorted
*modulefile*, virtual module or module alias under the directory will be used.
The dictionary comparison method of the :manpage:`lsort(n)` Tcl command is
used to achieve this sort. If highest numerically sorted element is an alias,
search continues on its *modulefile* target.

For example, it is possible for a user to have a directory named X11 which
simply contains a :file:`.version` file specifying which version of X11 is to
be loaded. Such a file would look like:

.. code-block:: tcl

     #%Module1.0
     ##
     ##  The desired version of X11
     ##
     set ModulesVersion "R4"

The equivalent :file:`.modulerc` would look like:

.. code-block:: tcl

     #%Module1.0
     ##
     ##  The desired version of X11
     ##
     module-version "./R4" default

If the extended default mechanism is enabled (see
:envvar:`MODULES_EXTENDED_DEFAULT` in :ref:`module(1)`) the module version
specified is matched against starting portion of existing module versions,
where portion is a substring separated from the rest of version string by a
``.`` character.

When the implicit default mechanism and the `Advanced module version
specifiers`_ are both enabled, a ``default`` and ``latest`` symbolic versions
are automatically defined for each module name (also at each directory level
in case of deep *modulefile*). Unless a symbolic version, alias, or regular
module version already exists for these version names.

If user names a *modulefile* that cannot be found in the first *modulepath*
directory, *modulefile* will be searched in next *modulepath* directory
and so on until a matching *modulefile* is found. If search goes through
a module alias or a symbolic version, this alias or symbol is resolved by
first looking at the *modulefiles* in the *modulepath* where this alias or
symbol is defined. If not found, resolution looks at the other *modulepaths*
in their definition order.

When locating *modulefiles*, if a :file:`.modulerc`, a :file:`.version`, a
directory or a *modulefile* cannot be read during the search it is simply
ignored with no error message produced. Visibility of *modulefiles* can thus
be adapted to the rights the user has been granted. Exception is made when
trying to directly access a directory or a *modulefile*. In this case,
the access issue is returned as an error message.

Depending on their name, their file permissions or the use of specific
modulefile commands, *modulefile*, virtual module, module alias or symbolic
version may be set hidden which impacts available modules search or module
selection processes (see `Hiding modulefiles`_ section below).

.. _Hiding modulefiles:

Hiding modulefiles
------------------

A *modulefile*, virtual module, module alias or symbolic version whose name or
element in their name starts with a dot character (``.``) or who are targeted
by a :mfcmd:`module-hide` command are considered hidden. Hidden modules are
not displayed or taken into account except if they are explicitly named (e.g.,
``foo/1.2.3`` or ``foo/.2.0`` not ``foo``). If module has been hidden with the
``--soft`` option of the :mfcmd:`module-hide` command set, it is not
considered hidden if the root name of the query to search it matches module
root name (e.g., searching ``foo`` will return a ``foo/1.2.3`` modulefile
targeted by a ``module-hide --soft`` command). If module has been hidden with
the ``--hard`` option of the :mfcmd:`module-hide` command set, it is always
considered hidden thus it is never displayed nor taken into account even if
it is explicitly named.

A *modulefile*, virtual module, module alias or symbolic version who are
targeted by a :mfcmd:`module-hide --hard<module-hide>` command and a
:mfcmd:`module-forbid` command or whose file access permissions are restricted
are considered hard-hidden and forbidden. Such modules are not displayed or
taken into account. When explicitly named for evaluation selection, such
modules are unveiled to return an access error.

A symbolic version-name assigned to a hidden module is displayed or taken into
account only if explicitly named and if module is not hard-hidden. Non-hidden
module alias targeting a hidden *modulefile* appears like any other non-hidden
module alias. Finally, a hidden symbolic version targeting a non-hidden module
is displayed or taken into account only if not hard-hidden and explicitly
named to refer to its non-hidden target.

The automatic version symbols (e.g., ``default`` and ``latest``) are
unaffected by hiding. Moreover when a regular ``default`` or ``latest``
version is set hidden, the corresponding automatic version symbol takes the
left spot. For instance, if ``foo/default`` which targets ``foo/1.2.3`` is set
hard-hidden, the ``default`` automatic version symbol will be set onto
``foo/2.1.3``, the highest available version of ``foo``.

When loading a *modulefile* or a virtual module targeted by a
:mfcmd:`module-hide --hidden-loaded<module-hide>` command, this module
inherits the ``hidden-loaded`` tag. Hidden loaded modules are not reported
among :subcmd:`list` sub-command results.

If the :option:`--all` is set on :subcmd:`avail`, :subcmd:`aliases`,
:subcmd:`whatis` or :subcmd:`search` sub-commands, hidden modules are taken
into account in search. Hard-hidden modules are unaffected by this option.

If the :option:`--all` is set on :subcmd:`list` sub-command, hidden loaded
modules are included in result output.

Advanced module version specifiers
----------------------------------

When the advanced module version specifiers mechanism is enabled (see
:envvar:`MODULES_ADVANCED_VERSION_SPEC` in :ref:`module(1)`), the
specification of modulefile passed on Modules specific Tcl commands changes.
After the module name a version constraint and variants may be added.

Version specifiers
""""""""""""""""""

After the module name a version constraint prefixed by the ``@`` character may
be added. It could be directly appended to the module name or separated from
it with a space character.

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
:envvar:`MODULES_EXTENDED_DEFAULT` in :ref:`module(1)`) to use an abbreviated
notation like ``@1`` to refer to more precise version numbers like ``1.2.3``.
Range of versions on its side natively handles abbreviated versions.

In order to be specified in a range of versions or compared to a range of
versions, the version major element should corresponds to a number. For
instance ``10a``, ``1.2.3``, ``1.foo`` are versions valid for range
comparison whereas ``default`` or ``foo.2`` versions are invalid for range
comparison.

Range of versions can be specified in version list, for instance
``foo@:1.2,1.4:1.6,1.8:``. Such specification helps to exclude specific
versions, like versions ``1.3`` and ``1.7`` in previous example.

If the implicit default mechanism is also enabled (see
:envvar:`MODULES_IMPLICIT_DEFAULT` in :ref:`module(1)`), a ``default`` and
``latest`` symbolic versions are automatically defined for each module name
(also at each directory level for deep *modulefiles*). These automatic version
symbols are defined unless a symbolic version, alias, or regular module
version already exists for these ``default`` or ``latest`` version names.
Using the ``mod@latest`` (or ``mod/latest``) syntax ensures highest available
version will be selected.

Variants
""""""""

After the module name, variants can be specified. :ref:`Module variants` are
alternative evaluation of the same *modulefile*. A variant is specified by
associating a value to its name. This specification is then transmitted to the
evaluating *modulefile* which instantiates the variant in the
:mfvar:`ModuleVariant` array variable when reaching the :mfcmd:`variant`
modulefile command declaring this variant.

Variant can be specified with the ``name=value`` syntax where *name* is the
declared variant name and *value*, the value this variant is set to when
evaluating the *modulefile*.

Boolean variants can be specified with the ``+name`` syntax to set this
variant on and with the ``-name`` or ``~name`` syntaxes to set this variant
off. The ``-name`` syntax is not supported on :ref:`ml(1)` command as the
minus sign already means to unload designated module. The ``~name`` and
``+name`` syntaxes could also be defined appended to another specification
word (e.g., the module name, version or another variant specification),
whereas ``-name`` syntax must be the start of a new specification word.

Boolean variants may also be specified with the ``name=value`` syntax. *value*
should be set to ``1``, ``true``, ``t``, ``yes``, ``y`` or ``on`` to enable
the variant or it should be set to ``0``, ``false``, ``f``, ``no``, ``n`` or
``off`` to disable the variant.

Shortcuts may be used to abbreviate variant specification. The
:mconfig:`variant_shortcut` configuration option associates shortcut character
to variant name. With a shortcut defined, variant could be specified with the
``<shortcut>value`` syntax. For instance if character ``%`` is set as a
shortcut for variant ``foo``, the ``%value`` syntax is equivalent to the
``foo=value`` syntax.

Specific characters used in variant specification syntax cannot be used as
part of the name of a module. These specific characters are ``+``, ``~``,
``=`` and all characters set as variant shortcut. Exception is made for ``+``
character which could be set one or several consecutive times at the end of
module name (e.g., *name+* or *name++*).

.. only:: html

   .. versionadded:: 4.4

   .. versionchanged:: 4.8
      Use of version range is allowed in version list

   .. versionchanged:: 4.8
      Support for module variant added


Modulefile Specific Help
------------------------

Users can request help about a specific *modulefile* through the
:ref:`module(1)` command. The *modulefile* can print helpful information or
start help oriented programs by defining a ``ModulesHelp`` subroutine. The
subroutine will be called when the :subcmd:`module help modulefile<help>`
command is used.


Modulefile Specific Test
------------------------

Users can request test of a specific *modulefile* through the :ref:`module(1)`
command. The *modulefile* can perform some sanity checks on its
definition or on its underlying programs by defining a ``ModulesTest``
subroutine. The subroutine will be called when the
:subcmd:`module test modulefile<test>` command is used. The subroutine should
return 1 in case of success. If no or any other value is returned, test is
considered failed.


Modulefile Display
------------------

The :subcmd:`module display modulefile<display>` command will detail all
changes that will be made to the environment. After displaying all of the
environment changes :file:`modulecmd.tcl` will call the ``ModulesDisplay``
subroutine. The ``ModulesDisplay`` subroutine is a good place to put
additional descriptive information about the *modulefile*.


Compatibility with Lmod Tcl modulefile
--------------------------------------

The ``add-property``, ``remove-property`` and ``extensions`` modulefile
commands are evaluated as a *no-operation* command. No error is obtained if
these commands are used in modulefiles but no change occurs.


ENVIRONMENT
-----------

See the :ref:`ENVIRONMENT<module ENVIRONMENT>` section in the
:ref:`module(1)` man page.

SEE ALSO
--------

:ref:`module(1)`, :ref:`ml(1)`, :manpage:`Tcl(n)`, :manpage:`TclX(n)`,
:manpage:`id(1)`, :manpage:`xrdb(1)`, :manpage:`exec(n)`, :manpage:`uname(1)`,
:manpage:`domainname(1)`, :manpage:`tclvars(n)`, :manpage:`lsort(n)`


NOTES
-----

Tcl was developed by John Ousterhout at the University of California
at Berkeley.

TclX was developed by Karl Lehenbauer and Mark Diekhans.

