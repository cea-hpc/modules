.. _module-info-name-return-basename:

Return file basename on module-info name for full path modulefile
=================================================================

When module name is specified as a full pathname, the ``module-info name``
command used in modulefile was returning the file basename on Modules
compatibility version. Starting version 4 of Modules, the full pathname is
returned when module is specified this way as once loaded this module is
identified by its full pathname. This recipe describes a way to get back
the behavior of Modules compatibility version for the ``module-info name``
modulefile command.

Implementation
--------------

Return file basename on ``module-info name`` for modules specified as full
path modulefile is implemented  by the use of a site-specific configuration
that supersedes the definition of the ``module-info name`` command to return
modulefile basename instead of full pathname.

.. literalinclude:: ../../example/module-info-name-return-basename/siteconfig.tcl
   :caption: siteconfig.tcl
   :lines: 13-27

**Compatible with Modules v4.2+**

Installation
------------

Create site-specific configuration directory if it does not exist yet:

.. parsed-literal::

   $ mkdir \ |etcdir|

Then copy there the site-specific configuration script of this recipe:

.. parsed-literal::

   $ cp example/module-info-name-return-basename/siteconfig.tcl \ |etcdir|\ /

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

With an ``info/name`` modulefile that sets an environment variable with the
result of the ``module-info name`` modulefile command:

.. literalinclude:: ../../example/module-info-name-return-basename/modulefiles/info/name
   :caption: info/name

Load ``info/name`` by its full pathname then check value of the environment
variable set::

   $ module load ./example/module-info-name-return-basename/modulefiles/info/name
   $ echo $MODNAME
   name
