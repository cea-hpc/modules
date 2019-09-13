.. _expose-procs-to-modulefiles:

Expose procedures and variables to modulefiles
==============================================

Same piece of code may be relevant for multiple modulefiles in your setup.
Sharing code (procedures and variables) is preferred to redefining it in each
modulefile. The following recipe describes an efficient way to define Tcl
procedures and variables that will be then available from the modulefile
evaluation context.

Implementation
--------------

To expose site-specific procedures and variables across all modulefiles during
their evaluation a site-specific configuration script is proposed.

This script exposes registered procedures to the Tcl interpreters that
evaluate either modulerc or modulefile scripts.

By using the ``env`` global array, which holds environment variables,
variables could also be exposed to those Tcl interpreters. Variables exposed
this way, will be found set within modulefile or modulerc evaluation context
but will not be exported to the environment changes the ``modulecmd.tcl``
script produces.

.. literalinclude:: ../../example/expose-procs-vars-to-modulefiles/siteconfig.tcl
   :caption: siteconfig.tcl
   :lines: 13-48

**Compatible with Modules v4.2+**

Installation
------------

Create site-specific configuration directory if it does not exist yet:

.. parsed-literal::

   $ mkdir \ |etcdir|

Then copy there the site-specific configuration script of this recipe:

.. parsed-literal::

   $ cp example/expose-procs-vars-to-modulefiles/siteconfig.tcl \ |etcdir|\ /

.. note::

   Defined location for the site-specific configuration script may vary from
   one installation to another. To determine the expected location for this
   file on your setup, check the value of the ``siteconfig`` option on Modules
   version 4.3 or above::

       $ module config siteconfig

   On older version of Modules, check the ``modulecmd.tcl`` script::

       $ grep '^set g_siteconfig ' $MODULES_CMD

The configuration script proposed should then be adapted to your needs:

* define your own procedures
* register them into the ``g_siteProcsToExposeToItrp`` list variable to expose
  them to the modulefile and modulerc evaluation contexts
* define your own variables using the ``env`` array variable


Usage example
-------------

Enable the modulepath where the example modulefiles are located::

   $ module use example/expose-procs-vars-to-modulefiles/modulefiles

Display one of the example modulefiles that makes use of the site-specific
procedure and variable defined in the ``siteconfig.tcl`` script::

   $ module show foo
   -------------------------------------------------------------------
   .../modulefiles/foo/1:
   
   setenv          FOO1 sitevalue
   setenv          FOO2 sitevarvalue
   -------------------------------------------------------------------

Load one example modulefile and check that the environment variable it
defines, which rely on the site-specific procedure and variable, are well
set::

   $ module load bar
   $ echo $BAR1
   sitevalue
   $ echo $BAR2
   sitevarvalue

