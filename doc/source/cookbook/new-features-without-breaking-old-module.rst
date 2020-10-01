.. _new-features-without-breaking-old-module:

Use new features without breaking old module command
====================================================

When working on large infrastructure, sometimes the ``module`` command is not
deployed with the same version everywhere. You may have for instance some old
cluster that still uses Modules version 3.2 and a newer supercomputer where
the lastest version of Modules 4 is available. In such situation it may
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

Usage example
-------------

For this recipe, a *foo* module is available in version *1.1* and *1.2*.
Version *1.1* is outdated and it has been decided to hide and forbid it
starting October 2020. For that, the new ``module-hide`` and ``module-forbid``
modulefile commands introduced in Modules 4.6 are used.

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

