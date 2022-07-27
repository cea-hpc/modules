.. _new-features-without-breaking-old-module:

Use new features without breaking old module command
====================================================

When working on large infrastructure, sometimes the ``module`` command is not
deployed with the same version everywhere. You may have for instance some old
cluster that still uses Modules version 3.2 and a newer supercomputer where
the latest version of Modules 4 is available. In such situation it may
however be desired to share the same modulefile catalog.

People providing software that build software and generate the modulefiles to
access them fall in a similar situation: a large variety of Modules version
may be in use by the end-users of their build product.

This recipe describes how to use features from the latest version of Modules 4
in modulefiles without breaking the use of these modulefiles from older
Modules version.

Implementation
--------------

The Tcl language provides some introspection mechanisms that help to know what
Tcl procedures are available. In the modulefile or modulerc evaluation
context, it helps to determine what Tcl modulefile commands are known. So by
crafting a conditional test using such mechanism (with Tcl ``info commands``
command) it is possible to test if a new feature is available prior using it.

.. literalinclude:: ../../example/new-features-without-breaking-old-module/modulefiles/.modulerc
   :language: tcl
   :caption: .modulerc

**Compatible with Modules v3.2+**

Starting version 4.7 of Modules, two new Tcl variables are introduced in the
modulefile and modulerc evaluation context: :mfvar:`ModuleTool` and
:mfvar:`ModuleToolVersion`. These two variables help to determine respectively
what is the module implementation running and what is its version. With this
knowledge it is possible to adapt modulefile and modulerc code to cope with a
behavior changing over module versions or with different behaviors between
different module implementation. The Tcl modulefile command
:mfcmd:`versioncmp` has been added along to help comparing software version
number (e.g. ``4.10`` is newer than ``4.7``).

Starting its version ``8.4.8``, the Lmod_ project also supports the
:mfvar:`ModuleTool` and :mfvar:`ModuleToolVersion` variables and the
:mfcmd:`versioncmp` modulefile command. It enables having modulefiles
compatible with both module implementations without restricting yourself from
using the advanced features from both projects.

.. _Lmod: https://github.com/TACC/Lmod

**Compatible with Modules v4.7+**

Usage example
-------------

For this recipe, a *foo* module is available in version *1.1* and *1.2*.
Version *1.1* is outdated and it has been decided to hide and forbid it
starting October 2020. For that, the new :mfcmd:`module-hide` and
:mfcmd:`module-forbid` modulefile commands introduced in Modules 4.6 are used.

Enable the modulepath where the example modulefiles are located::

   $ module use example/new-features-without-breaking-old-module/modulefiles

By using the conditional test code in the modulepath rc file (see
`Implementation`_ section above), the two new modulefile commands are not
used. So older versions of Modules still in use do not benefit from the
dynamic hiding and forbidding features however no error are obtained on
these setups::

   $ module -V | grep ^VERSION=
   VERSION=3.2.13
   $ module avail -t foo
   /path/to/example/new-features-without-breaking-old-module/modulefiles:
   foo/1.1
   foo/1.2

Yet the dynamic hiding and forbidding features are enabled for setup using
Modules 4.6 or newer version::

   $ module -V
   Modules Release 4.6.0 (2020-09-16)
   $ module avail -t foo
   /path/to/example/new-features-without-breaking-old-module/modulefiles:
   foo/1.2

Now take a look at *bar* module which provides a version for each Unix group
the current user is member of. User group membership can be retrieved with the
``usergroups`` sub-command of :mfcmd:`module-info` starting Modules version
4.6. With older version of Modules, the external command ``groups`` has to be
used to get this information. By using the :mfvar:`ModuleTool` and
:mfvar:`ModuleToolVersion` Tcl variables it will be possible to determine
if ``usergroups`` sub-command is available on ``module-info``.

.. literalinclude:: ../../example/new-features-without-breaking-old-module/modulefiles/bar/.modulerc
   :language: tcl
   :caption: bar/.modulerc

Querying available *bar* module versions should match the list of groups of
current user::

   $ groups
   grp1 grp2
   $ module avail -t bar
   /path/to/example/new-features-without-breaking-old-module/modulefiles:
   bar/grp1
   bar/grp2

.. note::

   As the new Tcl variables are introduced in Modules 4.7, the use of the new
   ``usergroups`` sub-command will only be triggered starting Modules 4.7.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
