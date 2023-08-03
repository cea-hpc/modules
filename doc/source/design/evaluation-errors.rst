.. _evaluation-errors:

Evaluation errors
=================

This document describes the different evaluation errors, depending on the
evaluation modes. The obtained behaviors when these errors raise and the
configuration options or command-line switches that change these behaviors.

Error kinds
-----------

The following list describes the different kind of evaluation error:

* *bad code*: modulefile has some code error (e.g., bad Tcl syntax, using
  undefined commands)
* *break*: use of ``break`` Tcl command in modulefile
* *error*: use of ``error`` Tcl command in modulefile
* *not found*: designated modulefile cannot be found
* *not loaded*: when unloading a module, designated modulefile is not found
  loaded
* *already loaded*: when loading a module, designated modulefile is already
  loaded
* *conflict*: dependency issue where loading modulefile conflicts with at
  least one loaded module
* *missing requirement*: dependency issue where a loading modulefile has at
  least one requirement module not loaded
* *dependent reload*: dependency issue where a loading or unloading modulefile
  has at least one dependent module that fails to reload (either during the
  unload or load phase)
* *unloading dependent*: dependency issue where an unloading modulefile has at
  least one loaded module requiring it
* *forbidden*: evaluated modulefile is tagged forbidden
* *sticky unload*: when unloading a module tagged sticky
* *super-sticky unload*: when unloading a module tagged super-sticky

.. note:: Use of ``continue`` Tcl command in modulefile shortens evaluation
   but it is not considered an error.

.. note:: When a *dependent reload* issue occurs during the load or unload of
   a modulefile, the dependent module failing to reload has raised one of the
   following error kind: bad code, break, error, conflict, missing
   requirement or forbidden.

Behavior when error raises
--------------------------

This section describes the behaviors generally obtained when the different
error kinds raise.

Other specific behavior depending on evaluation mode or configuration options
are described in the following parts of the document.

.. _default:

Default
^^^^^^^

Default behavior when an error raises:

* current modulefile evaluation is aborted
* an error message is reported
* exit code is set to 1

Ignored errors
^^^^^^^^^^^^^^

The following error kinds are *ignored errors*:

* not loaded
* already loaded

These errors lead to a different behavior:

* current modulefile evaluation is skipped
* no message reported
* no error exit code set

Evaluation mode or options specific behaviors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some error kinds only occur during specific evaluation mode or when
configuration options are set to a given value.

+---------------------+------------+-------------------------------+
| Error kind          | Evaluation | Configuration option          |
|                     | mode       |                               |
+=====================+============+===============================+
| conflict            | load       |                               |
+---------------------+------------+-------------------------------+
| missing requirement | load       |                               |
+---------------------+------------+-------------------------------+
| dependent reload    | load,      |                               |
|                     | unload     |                               |
+---------------------+------------+-------------------------------+
| unloading dependent | unload     | when :mconfig:`auto_handling` |
|                     |            | is disabled                   |
+---------------------+------------+-------------------------------+
| forbidden           | load       |                               |
+---------------------+------------+-------------------------------+
| already loaded      | load       |                               |
+---------------------+------------+-------------------------------+
| not loaded          | unload     |                               |
+---------------------+------------+-------------------------------+
| sticky unload       | unload     |                               |
+---------------------+------------+-------------------------------+
| super-sticky unload | unload     |                               |
+---------------------+------------+-------------------------------+

When evaluation mode and/or configuration option matches for these error kinds
to raise, a :ref:`default error behavior<default>` error behavior is applied.

Load sub-command
----------------

Specific error behavior for modulefile load evaluation by :subcmd:`load`
sub-command.

Force mode
^^^^^^^^^^

When :option:`--force` command-line switch is used, load evaluation by-pass
following errors:

* conflict
* missing requirement
* dependent reload

Following behavior is observed:

* evaluation continues (error is by-passed)
* warning message reported (instead of an error message)
* no error exit code set

.. warning:: *Missing requirement* and *dependent reload* errors currently
   returns an error exit code. This behavior might be aligned with the above
   one in the future. Or the above behavior may be adapted the other way
   around.

No effect on other error kinds as it is not useful to mark loaded a broken or
nonexistent modulefile.

Multiple modulefiles passed as argument
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When multiple modulefiles are passed to the ``load`` sub-command for
evaluation. If the evaluation of one modulefile raises an error, behavior for
this error is applied and:

* already evaluated modulefiles from the argument list are kept loaded
* evaluation continues with next modulefile in argument list

The above description only applies to ``load`` sub-command executed from the
top level context and not from a modulefile evaluation. Multiple arguments on
a ``module load`` command in modulefile are evaluated independently as an
*AND* requirement list.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
