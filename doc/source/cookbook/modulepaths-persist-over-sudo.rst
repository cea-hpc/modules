.. _modulepaths-persist-over-sudo:

Make defined modulepaths persist over sudo
==========================================

When running a command as another user with ``sudo``, current user environment
is most of the time flushed for security concerns. As a result, if one would
like to use the ``modulecmd.tcl`` script in such context, an error is returned
as ``modulecmd.tcl`` does not find modulepath defined (``MODULEPATH`` variable
is not set). Following recipe describes how to ensure the default modulepaths
are set every time the ``modulecmd.tcl`` script is run.

Implementation
--------------

Every time the ``modulecmd.tcl`` script is run, it evaluates during its
start-up phase a global RC file following the same kind of evaluation than for
modulefiles.

To ensure modulepaths are always defined, a check could be added in this
global RC file to verify at least one modulepath is set (thanks to the
``is-used`` Tcl modulefile command). If no modulepath is found set, the
``.modulespath`` configuration file, which contains the default modulepaths,
can be parsed to enable on the fly the default modulepaths (with ``module
use`` Tcl modulefile command).

.. literalinclude:: ../../example/modulepaths-persist-over-sudo/rc
   :caption: Global RC file

**Compatible with Modules v4.1+**

Installation
------------

Copy the global RC file of this recipe in the configuration directory:

.. parsed-literal::

   $ cp example/modulepaths-persist-over-sudo/rc \ |etcdir|\ /

The location of the ``.modulespath`` file defined in the proposed RC script
should be adapted to reflect the location where this file is installed on your
setup.

Usage example
-------------

Without the proposed RC file installed, ``MODULEPATH`` environment variable
is lost through the ``sudo`` call::

   $ sudo $MODULES_CMD bash avail >/dev/null
   ERROR: No module path defined

Once RC file is installed, flushed ``MODULEPATH`` is restored on the fly based
on ``.modulespath`` configuration file::

   $ sudo $MODULES_CMD bash use >/dev/null
   Search path for module files (in search order):
     /usr/share/Modules/modulefiles
     /etc/modulefiles
     /usr/share/modulefiles

Thus available modulefiles are found again::

   $ sudo $MODULES_CMD bash avail >/dev/null
   --------------- /usr/share/Modules/modulefiles ---------------
   dot  module-git  module-info  modules  null  use.own
