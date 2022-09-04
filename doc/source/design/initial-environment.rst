.. _initial-environment:

Initial environment
===================

Initial environment corresponds to the environment state at the end of Modules
initialization.

* initialization performed by :subcmd:`autoinit` sub-command
* which evaluates :file:`modulespath` and :file:`initrc` initialization files
  if they exist and if :envvar:`MODULEPATH` and :envvar:`LOADEDMODULES` are
  both found unset or empty
* environment state is composed of:

  * enabled modulepaths
  * loaded modules
  * with manual and modulerc tags applied onto loaded modules
  * with variant values applied onto loaded modules

Initial environment is what you get when you connect to a machine, with the
environment configuration (modulepaths, loaded modules) defined by the
system administrator of this machine. Plus your own addition if the
:file:`initrc` file set by system administrator calls ``module restore`` to
load your default collection.


Persistency
-----------

Once defined after the evaluation of :file:`modulespath` and :file:`initrc`
initialization files, the initial environment is stored in an environment
variable: :envvar:`__MODULES_LMINIT`.

* colon-separated list serializing content of a collection
* each element is a ``module use`` or ``module load`` command describing the
  initial environment (modulepaths, loaded modules, their tags and variants)
* variable is named following *private* name convention as it is not expected
  to be modified by user

Initial environment is stored as a collection in user environment to:

* use collection mechanisms to restore or view the initial environment
* stored in user environment to attach to the current shell session

Like for other ``__MODULES_LM*`` persistency variables, ``:`` character in
content (like tag list separator) is translated into ``<`` character. Not to
be confused with element separator in such variable.

As what is stored in ``__MODULES_LMINIT`` corresponds to the loaded
environment after Modules initialization, this environment is consistent
(i.e., no missing dependency for loaded modules).


reset sub-command
-----------------

:subcmd:`reset` sub-command enables to restore the initial environment. It
means to unuse enabled modulepaths and unload loaded modules then use initial
modulepaths and load initial modules.

General properties:

* Shortcut name: none
* Accepted option: :option:`--force`, :option:`--auto`, :option:`--no-auto`
* Expected number of argument: 0
* Accept boolean variant specification: no
* Parse module version specification: no
* Fully read modulefile when checking validity: yes
* Sub-command only called from top level: yes
* Lead to modulefile evaluation: yes (``unload`` and ``load`` modes)

``reset`` is equivalent to ``restore __init__``. It relies totally on
:subcmd:`restore` sub-command that unsets current environment and restore
initial environment, like done for any collection.

Behavior of ``reset`` sub-command can be changed with `reset_target_state
configuration option`_.

As it evaluates modules, ``--force``, ``--auto`` and ``--no-auto`` options can
be set for ``reset`` sub-command. But, as for ``restore`` sub-command these
options should have no impact, as:

* collection fully describes dependencies to load, no automatic resolution
  needed
* current environment unload is processed module by module, no automatic
  resolution expected
* *sticky* modules are unloaded by default, no need to force unload
* *super-sticky* cannot be unloaded even if forced

``reset`` sub-command outputs *Loading*, *Unloading*, *Tagging* messages like
``restore`` sub-command. Which is the default output mode for a sub-command
triggering multiple module evaluations that cannot be guessed by user.

*FUTURE*: an ``init`` tag could be applied onto loaded modules and used
modulepaths of initial environment to let users spot what parts of their
environment is from the initial one.


restore/saveshow sub-commands
-----------------------------

:subcmd:`restore` and :subcmd:`saveshow` sub-commands respectively restore and
show the initial environment when called with the ``__init__`` argument.

* instead of reading a collection file, the two sub-commands read the content
  of the :envvar:`__MODULES_LMINIT` variable
* specific name ``__init__`` is used not to confuse with an existing
  collection

``restore`` sub-command is changed to restore initial environment when no
argument is provided in case no default collection exists. This is done to
align behavior with Lmod.

Same behavior change is applied on ``saveshow`` sub-command: initial
environment is displayed when no argument provided and no default collection
exists.


reset_target_state configuration option
---------------------------------------

:mconfig:`reset_target_state` defines targeted state of ``reset`` sub-command.
What environment to restore.

* equals ``__init__`` by default, which corresponds to the behavior described
  above (initial environment is restored)
* when set with :subcmd:`config` sub-command, defines environment variable
  :envvar:`MODULES_RESET_TARGET_STATE` for value persistency
* other value accepted:

  * ``__purge__``: sub-command performs a ``module purge``
  * any other value: sub-command restore designated collection (an error is
    obtained if no collection with such name exists)

The ability to reset to a given collection enables user to define what is
their initial environment state. Useful if they do not want to rely on the
initial environment setup by sysadmins. For instance when sysadmins do not set
a ``module restore`` in :file:`initrc` to restore user's default collection
when user's session initializes.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
