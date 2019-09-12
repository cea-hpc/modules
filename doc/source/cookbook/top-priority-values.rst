.. _top-priority-values:

Top priority environment variable values
========================================

Multiple modulefiles may alter the same environment variable. It may be
wanted that among those modulefiles one should have the priority over the
others and the value it sets should persist even if another modulefile
loaded after attempts to alter the variable.

When using the ``setenv`` modulefile command, a *top priority value* should
persist over later setenv tries. On ``append-path`` modulefile command,
this top priority value should stay last position and for ``prepend-path``
modulefile command, value should stay first position.

Implementation
--------------

*Top priority values* are implemented by the use of a site-specific
configuration that supersedes the definition of the ``setenv``,
``append-path`` and ``prepend-path`` commands to introduce a ``--top``
argument. This argument enables a value set with this flag on to hold top
priority and thus cannot be altered unless by another top priority value.

For ``append-path`` command, ``--top`` ensure value will stay at last position
in path-like variable. Same goes for ``prepend-path`` where ``--top`` ensure
first position in path-like variable.

.. literalinclude:: ../../example/top-priority-values/siteconfig.tcl
   :caption: siteconfig.tcl
   :lines: 17-138

**Compatible with Modules v4.2**

Installation
------------

Create site-specific configuration directory if it does not exist yet:

.. parsed-literal::

   $ mkdir \ |etcdir|

Then copy there the site-specific configuration script of this recipe:

.. parsed-literal::

   $ cp example/top-priority-values/siteconfig.tcl \ |etcdir|\ /

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

With a ``bar/1`` modulefile that sets environment variables in a regular way:

.. literalinclude:: ../../example/top-priority-values/modulefiles/bar/1
   :caption: bar/1

And a ``foo/1`` modulefile that sets the same variables than ``bar/1`` but
with the ``--top`` priority flag:

.. literalinclude:: ../../example/top-priority-values/modulefiles/foo/1
   :caption: foo/1

Enable the modulepath where the example modulefiles are located::

   $ module use example/top-priority-values/modulefiles

Load ``foo/1`` then ``bar/1`` modulefiles and check value of the environment
variable set::

   $ module load foo/1 bar/1
   $ echo $TESTVAR
   topvalue
   $ echo $TESTPATH
   topprevalue:prevalue:postvalue:toppostvalue
