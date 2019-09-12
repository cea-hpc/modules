.. _inhibit-report-info:

Inhibit output of informative messages
======================================

Since Modules v4.2, additional module load or unload triggered by the load
or the unload of a modulefile are reported to the user to help understand
what happened automatically. These informative messages may not be desired
sometimes and here is a proposed way to inhibit them.

Implementation
--------------

Starting version v4.3, a ``verbosity`` configuration option is introduced to
increase or decrease the variety of the messages produced by the ``module``
command. To inhibit the output of the info-level messages, the ``concise``
verbosity level should be selected::

   $ module config verbosity concise

For v4.2 versions, a site-specific configuration script is proposed to inhibit
the output of the info-level messages.

.. literalinclude:: ../../example/inhibit-report-info/siteconfig.tcl
   :caption: siteconfig.tcl
   :lines: 14-23

**Compatible with Modules v4.2**

Installation (only for version older than v4.3)
-----------------------------------------------

Create site-specific configuration directory if it does not exist yet:

.. parsed-literal::

   $ mkdir \ |etcdir|

Then copy there the site-specific configuration script of this recipe:

.. parsed-literal::

   $ cp example/inhibit-report-info/siteconfig.tcl \ |etcdir|\ /

.. note::

   Defined location for the site-specific configuration script may vary from
   one installation to another. To determine the expected location for this
   file on your setup, check the ``modulecmd.tcl`` script::

       $ grep '^set g_siteconfig ' $MODULES_CMD

Usage example
-------------

With a bare ``bar`` modulefile:

.. literalinclude:: ../../example/inhibit-report-info/modulefiles/bar
   :caption: bar

And a ``foo`` modulefile that pre-requires ``bar``:

.. literalinclude:: ../../example/inhibit-report-info/modulefiles/foo
   :caption: foo

Enable the modulepath where the example modulefiles are located::

   $ module use example/inhibit-report-info/modulefiles

Load ``foo`` with auto handling mode enabled. The info-level message
inhibition should let ``foo`` load quiet::

   $ module load --auto foo
   $
