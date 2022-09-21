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

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent: