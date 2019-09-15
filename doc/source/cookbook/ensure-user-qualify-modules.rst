.. _ensure-user-specify-module-version:

Ensure user fully qualify the modules they use
==============================================

When managing a vast software catalog accessible by multiple users, it is
often desired that these users understand exactly what are the software they
enable in their environment. This is especially important when new software
versions are introduced in the catalog.

On a regular Modules installation, if multiple versions of a given module are
available, latest version is selected when user load this software module by
specifying its generic name (``module load software``). So when a new version
is installed for software, users automatically end up with this new version
enabled in their environment.

Depending on software, a precise version control may be required to avoid
issues. In this case the implicit default version selection done by Modules
is not desired. This recipe describe the way to handle this kind of
situation.

Implementation
--------------

Starting version 4.3 of Modules, a new option called ``implicit_default`` is
introduced. When enabled (which means configuration option is set to 1) if a
module is specified by its generic name an implicit default is automatically
defined (latest version) if no explicit default is defined. When
``implicit_default`` configuration option is disabled (when set to 0), no
implicit default is computed and an error is returned to user in case module
has been specified by its generic name but no default version has been set.

So to ensure user fully qualify the modules they use, this
``implicit_default`` configuration option should be disabled. It could be
achieved by setting the option in the ``initrc`` file installed in the
configuration directory (or in the ``modulerc`` file installed in the
initialization script directory if this location is preferred).

.. literalinclude:: ../../example/ensure-user-qualify-modules/initrc
   :caption: initrc

It may be desired to lock this option, to ensure users do not alter it
afterward. The ``lock_configs`` configuration option fills this need. By
setting this option and the ``implicit_default`` in a site-specific
configuration script, users will not be able to change the
``implicit_default`` behavior you configure.

.. literalinclude:: ../../example/ensure-user-qualify-modules/siteconfig.tcl
   :caption: siteconfig.tcl
   :lines: 13-17

**Compatible with Modules v4.3+**

Installation
------------

Create site-specific configuration directory if it does not exist yet:

.. parsed-literal::

   $ mkdir \ |etcdir|

Then copy there the initialization RC file of this recipe:

.. parsed-literal::

   $ cp example/ensure-user-qualify-modules/initrc \ |etcdir|\ /

Or if you want to enforce the ``implicit_default`` disablement setup for
users, copy there the site-specific configuration script of this recipe:

.. parsed-literal::

   $ cp example/ensure-user-qualify-modules/siteconfig.tcl \ |etcdir|\ /

If you currently use Modules version 4.3, copy the configuration script
specific to this version:

.. parsed-literal::

   $ cp example/ensure-user-qualify-modules/siteconfig.tcl-4.3 \ |etcdir|\ /siteconfig.tcl

.. note::

   Defined location for the site-specific configuration script may vary from
   one installation to another. To determine the expected location for this
   file on your setup, check the value of the ``siteconfig`` option::

       $ module config siteconfig

Once file is installed, you could verify option value is correct::

   $ module config implicit_default
   Modules Release 4.3.0 (2019-07-26)
   
   - Config. name ---------.- Value (set by if default overridden) ---------------
   implicit_default          0 (locked)

Usage example
-------------

Enable the modulepath where the example modulefiles are located::

   $ module use example/ensure-user-qualify-modules/modulefiles

Attempt to load a module, which does not defined a default version, by its
generic name::

   $ module load -v softa
   ERROR: No default version defined for 'softa'

Load succeed if module is fully qualified::

   $ module load -v softa/1
   Loading softa/1

For modules which define explicitly a default version, they can still be
loaded by their generic name::

   $ module load -v softb
   Loading softb/1
