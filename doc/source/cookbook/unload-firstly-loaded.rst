.. _unload-firstly-loaded:

Unload firstly loaded module matching name
==========================================

Since Modules v4, unloading a given module name unloads lastly loaded module
matching this given name. On Modules v3 the module selected for the unload
was the firstly loaded not the lastly loaded. This recipe gives a way to
restore the behavior of the v3 version.

Implementation
--------------

Starting version v4.3, an ``unload_match_order`` configuration option is
introduced to control whether firstly loaded module or lastly loaded module
should be selected for the unload. To select firstly loaded module::

   $ module config unload_match_order returnfirst

For older v4 versions, a site-specific configuration script is proposed to
select firstly loaded module matching name rather lastly loaded.

.. literalinclude:: ../../example/unload-firstly-loaded/siteconfig.tcl
   :caption: siteconfig.tcl
   :lines: 15-20

**Compatible with Modules v4.2**

Installation (only for version older than v4.3)
-----------------------------------------------

Create site-specific configuration directory if it does not exist yet:

.. parsed-literal::

   $ mkdir \ |etcdir|

Then copy there the site-specific configuration script of this recipe:

.. parsed-literal::

   $ cp example/unload-firstly-loaded/siteconfig.tcl \ |etcdir|\ /

.. note::

   Defined location for the site-specific configuration script may vary from
   one installation to another. To determine the expected location for this
   file on your setup, check the ``modulecmd.tcl`` script::

       $ grep '^set g_siteconfig ' $MODULES_CMD

Usage example
-------------

With a bare ``foo/1`` modulefile:

.. literalinclude:: ../../example/unload-firstly-loaded/modulefiles/foo/1
   :caption: foo/1

And a bare ``foo/2`` modulefile:

.. literalinclude:: ../../example/unload-firstly-loaded/modulefiles/foo/2
   :caption: foo/2

Enable the modulepath where the example modulefiles are located::

   $ module use example/unload-firstly-loaded/modulefiles

Load both ``foo`` modulefiles then attempt to unload ``foo`` name::

   $ module load foo/1 foo/2
   $ module list
   Currently Loaded Modulefiles:
    1) foo/1   2) foo/2
   $ module unload foo
   $ module list
   Currently Loaded Modulefiles:
    1) foo/2
