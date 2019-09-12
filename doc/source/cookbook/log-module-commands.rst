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
configuration that supersedes the definition of the ``module`` command to
execute the ``logger`` command, in order to send a message to the system log,
prior processing the module command itself. The log message sent describes the
module command called and who called it.

.. literalinclude:: ../../example/log-module-commands/siteconfig.tcl
   :caption: siteconfig.tcl
   :lines: 13-20

**Compatible with Modules v4.2+**

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
   version 4.3 or above::

       $ module config siteconfig

   On older version of Modules, check the ``modulecmd.tcl`` script::

       $ grep '^set g_siteconfig ' $MODULES_CMD

Usage example
-------------

Loading a bare modulefile::

    $ module load example

A log entry can then be retrieved from system log files::

    $ journalctl -q -t module -n 1
    Sep 12 20:24:01 hostname module[9925]: username: load example
