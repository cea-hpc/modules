.. _modulepath-option:

``--modulepath`` option
=======================

This document describes the ``--modulepath`` option added on several
modulefile commands.

Supported modulefile commands
-----------------------------

``--modulepath`` option is available on the following modulefile commands:

* :mfcmd:`always-load`
* :mfcmd:`prereq` (and its alias :mfcmd:`prereq-any`)
* :mfcmd:`prereq-all` (and its alias :mfcmd:`depends-on`)

Option value
------------

``--modulepath`` option accepts a list of modulepaths as value. Multiple
modulepaths are separated by colon character.

Behavior
--------

When option is set, required module should be located in one of the listed
modulepaths. It should be located either:

* in a modulepath, currently enabled (part of :envvar:`MODULEPATH`), whose
  starting path elements correspond to a directory listed in the option value
  list

* in a modulepath, not part of :envvar:`MODULEPATH`, listed in the option
  value list

For instance if value list of ``--modulepath`` option equals
``/path/to:/another/path`` and :envvar:`MODULEPATH` currently equals
``/path/to/modulefiles``. Required module should be located into either
``/path/to/modulefiles`` or ``/another/path`` directories.

If required module name is not loaded, a module search is performed in the
resolved directories, as explained above.

If required module name is already loaded, its path location is checked
against the directories listed in ``--modulepath`` option to verify one
corresponds to its first path elements. Paths elements are expected to be full
directory name. For instance ``/path/to/mod`` will not match
``/path/to/modulefiles`` directory.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
