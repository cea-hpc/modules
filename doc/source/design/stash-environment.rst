.. _stashing-environment:

Stashing environment
====================

Stash the module environment changes compared to :ref:`initial-environment`
in current shell session away. Inspired from ``git stash`` mechanisms and made
as close as possible to them to enable people knowing these mechanisms to use
them on ``module``.

Stash mechanism on Modules rely on collection and initial environment
mechanisms. When stashing, current environment is saved in a collection and
initial environment is restored.

stash sub-command
-----------------

:subcmd:`stash` sub-command saves current environment in a collection then
restore initial environment.

General properties:

* Shortcut name: none
* Accepted option: :option:`--force`, :option:`--auto`, :option:`--no-auto`
* Expected number of argument: 0
* Accept boolean variant specification: no
* Parse module version specification: no
* Fully read modulefile when checking validity: yes
* Sub-command only called from top level: yes
* Lead to modulefile evaluation: yes (``unload`` and ``load`` modes)

``stash`` is equivalent to :subcmd:`save stash-\<current_unixtime\><save>`
then :subcmd:`reset`. Thus :mconfig:`reset_target_state` configuration value
is taken into account to determine what is the initial environment state.

As it evaluates modules, ``--force``, ``--auto`` and ``--no-auto`` options can
be set for ``stash`` sub-command. But, as for ``reset`` sub-command these
options should have no impact.

Nothing happens and a warning message is emitted when current environment
state equals initial environment state.

* When comparing current environment and initial one, only the extra tags set
  and state-specific tags (*auto-loaded*, *keep-loaded*) are compared by
  default.
* If :mconfig:`collection_pin_tag` is enabled, all tags set (extra and
  regular) are compared
* A corner case exists if initial environment is recorded with
  ``collection_pin_tag`` disabled, then enabled afterward: in this situation
  ``stash`` cannot detect current environment is the same than recorded
  initial environment, so a stash collection will be created but nothing is
  changed after that when restoring initial environment

Error is obtained:

* and processing stops if stash collection cannot be saved.
* if a *super-sticky* module has to be unloaded.
* and processing stops if collection to restore does not exist, cannot be read
  or is not valid.
* and processing stops if current environment is empty or if the module
  constraints described are not satisfied.

When saved, stash collection is named *stash-<milliseconds>*. *milliseconds*
is the current number of milliseconds since Unix Epoch. This 13-digit number
makes latest generated stash collection the highest stash collection name. If
:mconfig:`collection_target` configuration is set, target value is appended
to the stash collection name, like for any other collection. For example, if
collection target equals ``foo``, stash collection file name is
``stash-1662295591123.foo``.

Milliseconds is preferred over seconds as multiple stash operations may occur
during the same second. This is for instance the case when running the
non-regression testsuite.

By using current number of milliseconds we get a unique file name without the
need to check existing stash file names.

stashpop sub-command
--------------------

:subcmd:`stashpop` sub-command restore a stashed collection then delete this
collection file.

General properties:

* Shortcut name: none
* Accepted option: :option:`--force`, :option:`--auto`, :option:`--no-auto`
* Expected number of argument: 0 or 1
* Accept boolean variant specification: no
* Parse module version specification: no
* Fully read modulefile when checking validity: yes
* Sub-command only called from top level: yes
* Lead to modulefile evaluation: yes (``unload`` and ``load`` modes)

``stashpop`` is equivalent to :subcmd:`restore stash-\<xxx\><restore>` then
:subcmd:`saverm stash-\<xxx\><saverm>`.

As it evaluates modules, ``--force``, ``--auto`` and ``--no-auto`` options can
be set for ``stashpop`` sub-command. But, as for ``reset`` and ``stash``
sub-commands these options should have no impact.

The stash collection to restore can be specified as argument to ``stashpop``
sub-command. Either as:

* a stash index: 0 for the most recent stash, 1 for the one before it
* a stash collection name (e.g., *stash-<milliseconds>*)

Error is obtained and processing stops if:

* stash index does not correspond to an existing stash collection (for
  currently set collection target)
* stash collection name does not correspond to an existing collection (for
  currently set collection target)
* stash collection exists but cannot be accessed
* stash collection cannot be removed

If stash collection to pop is not different than current environment,
collection restore will not change a thing and stash collection will be
removed.

stashrm sub-command
-------------------

:subcmd:`stashrm` sub-command delete a stash collection file.

General properties:

* Shortcut name: none
* Accepted option: none
* Expected number of argument: 0 or 1
* Accept boolean variant specification: no
* Parse module version specification: no
* Fully read modulefile when checking validity: yes
* Sub-command only called from top level: yes
* Lead to modulefile evaluation: no

``stashrm`` is equivalent to :subcmd:`saverm stash-\<xxx\><saverm>`.

The stash collection to delete can be specified as argument to ``stashrm``
sub-command. Either as:

* a stash index: 0 for the most recent stash, 1 for the one before it
* a stash collection name (e.g., *stash-<milliseconds>*)

Error is obtained and processing stops if:

* stash index does not correspond to an existing stash collection (for
  currently set collection target)
* stash collection name does not correspond to an existing collection (for
  currently set collection target)
* stash collection exists but cannot be accessed
* stash collection cannot be removed

stashshow sub-command
---------------------

:subcmd:`stashshow` sub-command displays the content of a stash collection file.

General properties:

* Shortcut name: none
* Accepted option: none
* Expected number of argument: 0 or 1
* Accept boolean variant specification: no
* Parse module version specification: no
* Fully read modulefile when checking validity: yes
* Sub-command only called from top level: yes
* Lead to modulefile evaluation: no

``stashshow`` is equivalent to :subcmd:`saveshow stash-\<xxx\><saveshow>`.

The stash collection to display can be specified as argument to ``stashshow``
sub-command. Either as:

* a stash index: 0 for the most recent stash, 1 for the one before it
* a stash collection name (e.g., *stash-<milliseconds>*)

Error is obtained and processing stops if:

* stash index does not correspond to an existing stash collection (for
  currently set collection target)
* stash collection name does not correspond to an existing collection (for
  currently set collection target)
* stash collection exists but cannot be accessed
* stash collection cannot be removed

stashclear sub-command
----------------------

:subcmd:`stashclear` sub-command delete all stash collection files.

General properties:

* Shortcut name: none
* Accepted option: none
* Expected number of argument: 0
* Accept boolean variant specification: no
* Parse module version specification: no
* Fully read modulefile when checking validity: yes
* Sub-command only called from top level: yes
* Lead to modulefile evaluation: no

``stashclear`` removes stash collection of currently defined
``collection_target``.

``stashclear`` proceeds without a confirmation message and does not output
the list of stash collection removed, even in verbose mode.

Error is obtained and processing stops if:

* a stash collection cannot be removed

stashlist sub-command
---------------------

:subcmd:`stashlist` sub-command lists all stash collection files.

General properties:

* Shortcut name: none
* Accepted option: :option:`--terse`, :option:`--long`, :option:`--json`
* Expected number of argument: 0
* Accept boolean variant specification: no
* Parse module version specification: no
* Fully read modulefile when checking validity: yes
* Sub-command only called from top level: yes
* Lead to modulefile evaluation: no

``stashlist`` is equivalent to :subcmd:`savelist stash-*<savelist>`.

``stashlist`` lists stash collection of currently defined
``collection_target``.

Returned stash collections are listed in the reverse order (most recent
collection first) and list index starts at 0. Index is this way consistent
with stash index argument value that could be provided to ``stashpop``,
``stashshow`` or ``stashrm`` sub-commands.

``stashlist`` relies on ``savelist`` sub-command. A difference exists however:
when no collection target is set, only the stash collections without a target
defined are returned.

Error is obtained and processing stops if:

* stash collection exists but cannot be accessed

savelist sub-command
--------------------

:subcmd:`savelist` sub-command is adapted to filter out stash collections by
default. Stash collections are reported if :option:`--all` option is set.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
