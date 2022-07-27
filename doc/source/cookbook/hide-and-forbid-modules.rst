.. _hide-and-forbid-modules:

Hide and forbid modules
=======================

:command:`module` is often used to give access to hundreds of different
software to a large variety of users, each of them provided in several
versions. In such situation users ends up facing thousands of modulefiles
which could be confusing.

On the other side, site's staff needs to decommission older software version
to ensure for instance that only the best matches for their hardware resources
remain. Such work needs specific care to avoid breaking the computing workflow
of users.

This recipe provides examples on how to hide or/and forbid modulefiles to
better cope with software life cycles and help both user and staff to get
their bearings in large environments.

Implementation
--------------

The :mfcmd:`module-hide` and :mfcmd:`module-forbid` modulefile commands block
respectively the visibility or the load evaluation of the modulefiles they
target. These two commands have a variety of options to inhibit their action
or display specific messages. Combining both commands enables to hide and
forbid specified modulefiles.

**Compatible with Modules v4.6+**

.. note::

   Modules v4.7+ is required to use the ``--hidden-loaded`` option of
   :mfcmd:`module-hide` command.

Usage examples
--------------

The use cases below describe situations where hiding or/and forbidding use of
modulefiles can be helpful.

The following set of modules will be used in the examples of this recipe. Each
use case will progressively add :mfcmd:`module-hide` and
:mfcmd:`module-forbid` statements in modulepath's :file:`.modulerc` file to
improve adapt module visibility or forbid access to them.

.. code-block:: console

    $ module avail
    -- ../example/hide-and-forbid-modules/modulefiles --
    appA/1.0  bioappA/1.0  chemappA/1.0  deplibA/1.0
    appA/2.0  bioappA/2.0  chemappA/2.0  deplibA/2.0
    appB/1.0  bioappB/1.0  chemappB/1.0  deplibB/1.0
    appB/2.0  bioappB/2.0  chemappB/2.0  deplibB/2.0

Limiting view to the useful software only
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With a large number of available modules it is interesting to reduce user's
visibility to only report the modules of interest.

Some modules may require other to be loaded as dependency. This is the case
for ``appA`` and ``appB`` which depend on ``deplibA`` or ``deplibB``. These
two dependency libraries are not interesting by themselves so they could be
hidden by default but they should stay findable for ``appA`` or ``appB``
modules to load their dependency.

:mfcmd:`module-hide --soft<module-hide>` could be used to address this need.
Designated module will be hidden unless searched. Following lines are added to
modulepath's :file:`.modulerc` file

.. code-block:: tcl

    # hide modules only loaded as dependency
    module-hide --soft deplibA
    module-hide --soft --hidden-loaded deplibB

As a result ``deplibA`` and ``deplibB`` are not returned anymore on a global
:subcmd:`avail` sub-command::

    $ module avail
    -- ../example/hide-and-forbid-modules/modulefiles --
    appA/1.0  appB/2.0     bioappB/1.0   chemappA/2.0
    appA/2.0  bioappA/1.0  bioappB/2.0   chemappB/1.0
    appB/1.0  bioappA/2.0  chemappA/1.0  chemappB/2.0

Yet these modules are still found when loading the modules of the application
requiring them::

    $ module load appA
    Loading appA/2.0
      Loading requirement: deplibA/2.0

In some cases it is desirable to hide such dependency modules also when they
are loaded. This could be achieved by using the ``--hidden-loaded`` option of
the :mfcmd:`module-hide` command. Hidden loaded modules do not appear by
default on :subcmd:`list` sub-command output unless :option:`--all` is set.
Queries like :subcmd:`is-loaded` still detect such modules as loaded even if
hidden. In addition loading or unloading informational messages related to
these modules are not reported unless a :envvar:`verbosity
mode<MODULES_VERBOSITY>` higher than ``verbose`` is configured.

.. code-block:: console

    $ module load appB
    $ module list
    Currently Loaded Modulefiles:
     1) appB/2.0  
    $ module list --all
    Currently Loaded Modulefiles:
     1) deplibB/2.0   2) appB/2.0
    $ module is-loaded deplibB
    $ echo $?
    0

Going further, among scientific applications some are only useful for a given
scientific field. A site may provide many software covering many scientific
fields but a user may only be concerned by one of these fields. In our example
software are provided for biology users (*bioappA* and *bioappB*) and other
software for chemistry users (*chemappA* and *chemappB*).

Say every biology users are part of a ``bio`` Unix group and every chemistry
users are member of a ``chem`` Unix group. Available software visibility could
be improved by only reporting the *bio* software to the *bio* users and the
*chem* software to the *chem* users.

.. code-block:: tcl

    # hide modules not from user's scientific field
    module-hide --soft --not-group bio bioappA bioappB
    module-hide --soft --not-group chem chemappA chemappB

With the above statements put in modulepath's :file:`.modulerc` file, the
*bio* software are not seen anymore by *chem* users::

    $ module avail
    -- ../example/hide-and-forbid-modules/modulefiles --
    appA/1.0  appB/1.0  chemappA/1.0  chemappB/1.0
    appA/2.0  appB/2.0  chemappA/2.0  chemappB/2.0

However as they are softly hidden, these modules can still be seen if queried
or if ``--all`` option is used::

    $ module avail bioappA
    -- ../example/hide-and-forbid-modules/modulefiles --
    bioappA/1.0  bioappA/2.0
    $ module avail --all
    -- ../example/hide-and-forbid-modules/modulefiles --
    appA/1.0  bioappA/1.0  chemappA/1.0  deplibA/1.0
    appA/2.0  bioappA/2.0  chemappA/2.0  deplibA/2.0
    appB/1.0  bioappB/1.0  chemappB/1.0  deplibB/1.0
    appB/2.0  bioappB/2.0  chemappB/2.0  deplibB/2.0

Software requiring administrative actions prior usage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some software may require an administrative step to get allowed to use them,
like the signature of a user agreement. Such software should be reported among
the available modules but should not be used prior the administrative step
achieved.

In our example, the ``chemappA`` application requires a user charter to be
signed and then sent to the site staff. The access to the ``chemappA`` is
controlled by the ``chemappA`` Unix group: a user needs to be part of this
group to access and use the application.

.. code-block:: tcl

    # forbid use of modules unless software user agreement is signed
    set msg {User agreement for ChemAppA application must be validated to use it
    1. Please connect to https://ChemAppA.example.org
    2. Read the Term of Use and sign it
    3. Send back this signed agreement to our service desk}
    module-forbid --not-group chemappA --message $msg chemappA

With the above :mfcmd:`module-forbid` statement put in modulepath's
:file:`.modulerc` file, the software is still visible but its load is denied
unless if the user is part of the ``chemappA`` group.

.. code-block:: console

    $ module avail chemappA
    -- ../example/hide-and-forbid-modules/modulefiles --
    chemappA/1.0  chemappA/2.0
    $ module load chemappA
    ERROR: Access to module 'chemappA/2.0' is denied
      User agreement for ChemAppA application must be validated to use it
      1. Please connect to https://ChemAppA.example.org
      2. Read the Term of Use and sign it
      3. Send back this signed agreement to our service desk

A specific message is provided through the ``--message`` option to guide the
user to complete the required administrative step. Once group membership is
acquired, the module can be seamlessly loaded.

.. code-block:: console

    $ id --groups --name
    chemappA chem
    $ module load -v chemappA
    Loading chemappA/2.0

.. note::

   Do not forget to protect the access to the directory where the software is
   installed to really ensure that only allowed users can use it.

Software limited to particular users
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some applications may be restricted to a limited set of users. For instance
because such application should not be disclosed or because it requires a
license that is paid only by a few users.

In our example, the ``appC`` application works with token-based licenses. The
only users that can use this software are those that have paid for a license
token. Other users should not access nor even see the availability of this
application. Users that have bought a license token are added to the ``appC``
Unix group.

.. code-block:: tcl

    # fully hide and forbid modules unless user owns a license token
    module-hide --hard --not-group appC appC
    set msg {Access is restricted to owners of license token}
    module-forbid --not-group appC --message $msg appC

The above statements have been added in modulepath's :file:`.modulerc` file.
The :mfcmd:`module-hide --hard<module-hide>` command is used to completely
remove visibility for non-authorized users.

.. code-block:: console

    $ module avail
    -- ../example/hide-and-forbid-modules/modulefiles --
    appA/1.0  appB/1.0  chemappA/1.0  chemappB/1.0
    appA/2.0  appB/2.0  chemappA/2.0  chemappB/2.0
    $ module avail appC
    $ module load appC
    ERROR: Unable to locate a modulefile for 'appC'

The :mfcmd:`module-forbid` statement added for ``appC`` helps to get a clear
error message for the non-authorized users that are aware of the existence of
the module name and version (instead of getting a modulefile location error).

.. code-block:: console

    $ module load appC/2.0
    ERROR: Access to module 'appC/2.0' is denied
      Access is restricted to owners of license token

Alternatively such restrictions on modulefiles can be achieved by adapting
file permission mode instead of adding statements in modulepath's
:file:`.modulerc` file.

.. code-block:: console

    $ chmod 640 ../example/hide-and-forbid-modules/modulefiles/appC/*
    $ chgrp appC ../example/hide-and-forbid-modules/modulefiles/appC/*

However restricting file permission mode does not enable to authorize several
Unix groups or users to access those modulefiles or to have specific error
messages unlike when :mfcmd:`module-hide` and :mfcmd:`module-forbid` commands
are used.

.. note::

   Do not forget to protect the access to the directory where the software is
   installed to really ensure that only the authorized users can use it.

Software life cycle
^^^^^^^^^^^^^^^^^^^

When providing a new software version, it may be interesting to have a test
phase, for instance during one week, to make this new version checked by some
some pilot users prior the general availability.

In our example, we are currently November 13th and ``chempappB/2.0`` has been
installed two days ago. A test phase is ongoing and users that are member of
the ``pilot`` Unix group can already access this software prior its general
availability set for November 18th.

.. code-block:: tcl

    # test new version of chemappB prior general availability
    module-hide --before 2020-11-18 --not-group pilot chemappB/2.0

With the above statement added to modulepath's :file:`.modulerc` file, common
users will not see the new module until November 18th.

.. code-block:: console

    $ date
    Fri 13 Nov 2020 02:04:21 PM CET
    $ module avail chemappB
    -- ../example/hide-and-forbid-modules/modulefiles --
    chemappB/1.0

At some point software need to be decommissioned to ensure for instance that
buggy or under-optimized versions are not used anymore by users.

In our example, ``appA/1.0`` needs to be removed as a newer version is
available for *appA* and this new version fixes a lot of issues that were
encountered with version 1.0. Decommission date for ``appA/1.0`` is planned on
November 18th.

.. code-block:: none

    # decommission of old version of appA
    set nearmsg {appA/1.0 will be decommissioned, please use appA/2.0}
    set msg {appA/1.0 is decommissioned, please use appA/2.0}
    module-hide --hard --after 2020-11-18 --not-group eol appA/1.0
    module-forbid --after 2020-11-18 --not-group eol \
       --nearly-message $nearmsg --message $msg appA/1.0

The above statements added to modulepath's :file:`.modulerc` file will remove
visibility and access to the decommissioned module on November 18th. Only
users part of the ``eol`` Unix group will still keep an access to the software
(which is useful in case some users are unfortunately stuck on this specific
version of *appA*). Specific messages are set to guide users toward selecting
another version for *appA*.

.. code-block:: console

    $ module avail appA
    -- ../example/hide-and-forbid-modules/modulefiles --
    appA/1.0  appA/2.0
    $ module load appA/1.0
    Loading appA/1.0
      WARNING: Access to module will be denied starting '2020-11-18'
        appA/1.0 will be decommissioned, please use appA/2.0
      Loading requirement: deplibA/1.0

Prior decommission date, module will still be visible and loadable. However
when loading the module a warning message will appear when the expiry date
will be close. The :envvar:`nearly_forbidden_days<MODULES_NEARLY_FORBIDDEN_DAYS>`
module configuration defines starting when such warning message should be
reported prior decommission date (14 days by default).

.. code-block:: console

    $ module config nearly_forbidden_days
    Modules Release 4.6.0 (2020-09-16)

    - Config. name ---------.- Value (set by if default overridden) ---------------
    nearly_forbidden_days     14

Once the decommissioned date is over, ``appA/1.0`` has disappeared unless for
users member of the ``eol`` Unix group.

.. code-block:: console

    $ date
    Thu 19 Nov 2020 02:46:27 PM CET
    $ module avail appA
    -- ../example/hide-and-forbid-modules/modulefiles --
    appA/2.0
    $ module load appA/1.0
    ERROR: Access to module 'appA/1.0' is denied
      appA/1.0 is decommissioned, please use appA/2.0

.. note::

   Do not forget to protect the access to the directory where the software is
   installed prior its general availability and after its decommissioned date.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
