.. _INSTALL-win:

Installing Modules on Windows
=============================

This document is an overview of building and installing Modules on a Windows
platform.


Requirements
------------

Modules consists of one Tcl script so to run it from a user shell the only
requirement is to have a working version of ``tclsh`` (version 8.5 or later)
available on your system. ``tclsh`` is a part of `Tcl`_.

.. _Tcl: http://www.tcl-lang.org/software/tcltk/

A specific distribution zipball is provided to install Modules on a Windows
platform. Content of this distribution zipball is ready for use and does not
require a specific build step. All scripts and documentation found in this
zipball are pre-built so there is no specific tools are required to install
Modules from the Windows-specific distribution zipball.


Installation instructions
-------------------------

1. Install a Tcl binary distribution for Windows like `ActiveTcl`_ or
   `Magicsplat Tcl/Tk for Windows`_. Follow instructions provided with the
   chosen distribution to install it.

.. _ActiveTcl: https://www.activestate.com/products/tcl/
.. _Magicsplat Tcl/Tk for Windows: https://www.magicsplat.com/tcl-installer/

2. Once installed, verify that the ``tclsh`` command is correctly found in
   defined ``PATH`` by typing the following command from a Windows ``cmd``
   shell (``windows`` string should be obtained as result)::

        > echo puts $tcl_platform(platform) | tclsh
        windows

3. Download Modules specific distribution zipball for Windows from
   `SourceForge`_ or `GitHub`_. Such distribution archives are available
   for Modules release starting version ``4.5.0`` and can be distinguished
   from the source tarball by the ``-win`` suffix in their name.

.. _SourceForge: https://sourceforge.net/projects/modules/files/Modules/
.. _GitHub: https://github.com/cea-hpc/modules/releases

4. Unpack downloaded zip file then enter deflated directory and execute the
   ``INSTALL.bat`` script file found in it. This script installs files by
   default in ``C:\Program Files\Environment Modules\`` directory and adds the
   ``bin`` directory in this installation location to the system-wide ``PATH``
   environment variable.

.. note:: ``INSTALL.bat`` script may require to be run with administrator
   rights to perform installation correctly.

5. Once installed, verify that the ``module`` command is correctly found in
   defined ``PATH`` by typing the following command from a Windows ``cmd``
   shell::

        > module -V
        Modules Release 4.4.1 (2020-01-03)

Installation location can be adapted by running the ``INSTALL.bat`` script
from a ``cmd`` console shell and passing desired installation target as
argument. For instance to install Modules in ``C:\EnvironmentModules``
directory::

        > INSTALL.bat C:\EnvironmentModules

Modules installation is now operational and you can setup your modulefiles. By
default, the ``modulefiles`` directory in installation directory is defined as
a modulepath and contains few modulefile examples::

        > module avail
        ------- C:/Program Files/Environment Modules/modulefiles --------
        module-git  module-info  null

Documentation of the :ref:`module(1)` and :ref:`ml(1)` commands and
:ref:`modulefile(5)` syntax can be found in the ``doc`` directory in
installation directory.
