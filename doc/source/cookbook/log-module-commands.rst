.. _log-module-command:

Log module command
==================

It is sometimes desired to better understand the ``module`` usage on the
system we manage. Especially to determine what are the modulefiles most used
and what are those never used that could be removed. This recipe describes a
way to track the ``module`` usage by logging each request made.

Implementation
--------------

Logging module commands is implemented by the use of a site-specific
configuration that traces every call to a modulefile evaluation.

.. literalinclude:: ../../example/log-module-commands/siteconfig.tcl
   :language: tcl
   :caption: siteconfig.tcl
   :lines: 26-54
   :lineno-start: 26

This code defines a ``logModfileInterp`` procedure which is set to be
evaluated after each evaluation of the ``execute-modulefile`` procedure with
the `trace`_ Tcl command. Thanks to the ``trace`` mechanism
``logModfileInterp`` receives the arguments passed to ``execute-modulefile``.

The ``logModfileInterp`` procedure applies filter to only log *load* and
*unload* modulefile evaluations. It may be extended to the other evaluation
modes (*help*, *display*, *test*, *whatis* and *refresh*) by adapting the
following line:

.. literalinclude:: ../../example/log-module-commands/siteconfig.tcl
   :language: tcl
   :lines: 36-37
   :lineno-start: 36

In the proposed code, log entries are formatted as a JSON record which is
convenient to push these logs in a search and analytics engine like
`Elasticsearch`_ or `Splunk`_. Such tools help to globally monitor the whole
set of log entries produced from thousands of computing nodes.

.. literalinclude:: ../../example/log-module-commands/siteconfig.tcl
   :language: tcl
   :lines: 47-49
   :lineno-start: 47

The :command:`logger` command is run to generate the log message. This is done
through a specific ``execLogger`` procedure ensuring that the current user
environment does not confuse :command:`logger` with unexpected version of the
libraries it requires.

.. literalinclude:: ../../example/log-module-commands/siteconfig.tcl
   :language: tcl
   :lines: 13-24
   :lineno-start: 13

Example code also defines a ``logModuleCmd`` procedure which is set to be
evaluated after each evaluation of the ``module`` and the ``ml`` procedures
with `trace`_ Tcl command.

.. literalinclude:: ../../example/log-module-commands/siteconfig.tcl
   :language: tcl
   :caption: siteconfig.tcl
   :lines: 56-76
   :lineno-start: 56

.. note::

   This code example may be extended to log for instance additional
   information in each message. The `upvar`_ Tcl command may be used to
   retrieve variables from the calling context. However beware that the
   internal code of Modules may change, so if you rely on internal variables
   please re-check the code in the :file:`siteconfig.tcl` file deployed after
   each upgrade of Modules.

**Compatible with Modules v4.2+**

.. _trace: https://www.tcl-lang.org/man/tcl8.5/TclCmd/trace.html
.. _Elasticsearch: https://www.elastic.co/elasticsearch/
.. _Splunk: https://www.splunk.com/
.. _upvar: https://www.tcl-lang.org/man/tcl8.5/TclCmd/upvar.html

Installation
------------

Create site-specific configuration directory if it does not exist yet:

.. parsed-literal::

   $ mkdir \ |etcdir|

Then copy there the site-specific configuration script of this recipe:

.. parsed-literal::

   $ cp example/log-module-commands/siteconfig.tcl \ |etcdir|\ /

.. note::

   Defined location for the site-specific configuration script may vary from
   one installation to another. To determine the expected location for this
   file on your setup, check the value of the ``siteconfig`` option on Modules
   version 4.3 or above:

   .. parsed-literal::

       :ps:`$` module config siteconfig

   On older version of Modules, check the ``modulecmd.tcl`` script:

   .. parsed-literal::

       :ps:`$` grep '^set g_siteconfig ' $MODULES_CMD

Usage example
-------------

Listing available modulefiles, then loading a bare modulefile, then another
one with a dependency and purging everything in the end:

.. parsed-literal::

    :ps:`$` ml av
    --------------- :sgrdi:`/path/to/modulefiles` ---------------
    bar/1.0  foo/1.0  qux/1.0
    :ps:`$` ml foo
    :ps:`$` module load bar
    Loading :sgrhi:`bar/1.0`
      :sgrin:`Loading requirement`: qux/1.0
    :ps:`$` module purge

A log entry can then be retrieved from system log files:

.. parsed-literal::

    :ps:`$` journalctl -q -t module
    Sep 12 20:24:01 hostname module[9925]: { "user": "username", "cmd": "ml", "args": "av" }
    Sep 12 20:24:02 hostname module[9925]: { "user": "username", "mode": "load", "module": "foo/1.0", "auto": false }
    Sep 12 20:24:02 hostname module[9925]: { "user": "username", "cmd": "ml", "args": "foo" }
    Sep 12 20:24:03 hostname module[9925]: { "user": "username", "mode": "load", "module": "qux/1.0", "auto": true }
    Sep 12 20:24:03 hostname module[9925]: { "user": "username", "mode": "load", "module": "bar/1.0", "auto": false }
    Sep 12 20:24:03 hostname module[9925]: { "user": "username", "cmd": "module", "args": "load bar" }
    Sep 12 20:24:04 hostname module[9925]: { "user": "username", "mode": "unload", "module": "bar/1.0" }
    Sep 12 20:24:04 hostname module[9925]: { "user": "username", "mode": "unload", "module": "qux/1.0" }
    Sep 12 20:24:04 hostname module[9925]: { "user": "username", "mode": "unload", "module": "foo/1.0" }
    Sep 12 20:24:04 hostname module[9925]: { "user": "username", "cmd": "module", "args": "purge" }

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
