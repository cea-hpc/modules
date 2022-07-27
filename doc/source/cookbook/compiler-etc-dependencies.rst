.. _compiler-etc-dependencies:

Handling Compiler and other Package Dependencies
================================================

When creating a collection of software (applications and libraries)
for users to use, there is the problem of ensuring that the
user is using the correct builds of everything. Generally,
if an user is attempting to compile code making use of the system
software collection, you want to ensure that the user is compiling
his code with the same compiler that was used to compile the library.
This tends to be particularly true of C++ and Fortran code using modules,
and parallel codes using MPI libraries.

As a result, in environments supporting multiple compilers,
software libraries often end up with multiple installs of libraries
and applications of the same version, depending on the compiler
and other libraries used to build them. Sometimes there are
even additional installs for variants with different threading
models, number formats, level of vectorization support, etc.
This cookbook describes various strategies for handling the modulefiles
to support all these different builds for each package.

For each strategy, we will provide an overview of how it works,
and then show how an user might interact with it, usually a similar
sequence for each case. We wish to explore how, and how well, each strategy
succeeds in handling the multiple builds of the same version of a package,
including

#. Basic dependency handling: seeing how well the strategy supports
   the loading of the correct build of a package depending on the previously
   loaded dependencies. And if no appropriate build is available, they should
   error accordingly.
#. The ``module switch`` command and more advanced dependency handling: how
   well the strategy supports more advanced cases. E.g. a case wherein several
   modules are loaded and the user replaces a module upon which other modules
   currently loaded depend. In general, how well the strategy prevents the
   user's set of loaded modules from being incompatible.
#. Visibility into what packages are available. This includes being able
   to readily see all of the packages installed, seeing what versions of packages
   are available for a given compiler/MPI/etc combination, and seeing for
   which compiler/MPI/etc combinations a specific version of a package is available.
#. How easily the user can navigate the modules for the builds. This
   includes how well partial modulenames (e.g. omitting version, etc) are
   handled by the different strategies.

We then try to summarize the strengths, weaknesses, and other attributes of
each strategy. We also try to discuss differences in using on older (3.x)
and newer (4.x) Environment Modules versions.

In addition to displaying examples for each strategy in this
document, we have set up a the test environment as a playground in
which you can explore.


Contents
--------

#. `Overview of Examples`_
#. :ref:`Flavours_strategy`
#. :ref:`Homebrewed_flavors_strategy`
#. :ref:`Modulerc-based_strategy`
#. :ref:`Modulepath-based_strategy`
#. `Comparison of Strategies`_


Overview of Examples
--------------------

The examples are a bit more elaborate than in some other cookbooks, so
the directory structure under ``doc/example/compiler-etc-dependencies``
is similarly more complicated.

Example Software Library
^^^^^^^^^^^^^^^^^^^^^^^^

For the purpose of the examples and the playground, we have created
a fake example software library, rooted at the subdirectory
``doc/example/compiler-etc-dependencies/dummy-sw-root`` beneath where
you placed the modules source files. This software tree is intended
to represent some of the features you might see in a real software
tree, which supports various compiler and MPI libraries, and that has
been added to over time, and not always in the most systematic way.

The example software library does not contain any real code; there are
dummy scripts for e.g. ``gcc``, ``mpirun``, etc. which just echo then
name of the code and what version, compiler, etc. it was supposed to be
built for, which is handy to show in the examples that the modulefiles
are working as expected. It also shows how such a directory tree might
be laid out --- the details of the layout will affect some of the code
in the modulefiles, etc. The directory structure can be altered to
fit your standards, but would require some minor modification to the
modulefiles, etc.
Note that there are also a bunch of subdirectories named ``1``
containing symlinks, these are for the strategy using the Flavours add-on and are
discussed in that section.

The software in the example software library consists of:

* GNU compiler versions ``8.2.0`` and ``9.1.0``
* Intel Parallel Studio suite versions ``2018`` and ``2019`` (includes compilers, MPI and MKL)
* PGI compiler suite versions ``18.4`` and ``19.4``
* OpenMPI version ``4.0``, built for:

  * ``gcc/9.1.0``
  * ``intel/2019``
  * ``pgi/19.4``

* OpenMPI version ``3.1``, built for:

  * ``gcc`` versions ``8.2.0`` and ``9.1.0``
  * ``intel`` versions ``2018`` and ``2019``
  * ``pgi`` versions ``18.4`` and ``19.4``

* mvapich version 2.3.1, built for:

  * ``gcc/9.1.0``
  * ``intel/2019``
  * ``pgi/19.4``

* mvapich version ``2.1``, built for:

  * ``gcc`` versions ``8.2.0`` and ``9.1.0``
  * ``intel`` versions ``2018`` and ``2019``
  * ``pgi`` versions ``18.4`` and ``19.4``

* foo version ``2.4``, built for:

  * ``gcc/9.1.0`` and ``openmpi/4.0``
  * ``gcc/9.1.0`` and ``mvapich/2.3.1``
  * ``gcc/9.1.0`` and no MPI
  * ``intel/2019`` and ``openmpi/4.0``
  * ``intel/2019`` and ``mvapich/2.3.1``
  * ``intel/2019`` and ``intelmpi``
  * ``intel/2019`` and no MPI
  * ``pgi/19.4`` and ``openmpi/3.1``
  * ``pgi/19.4`` and no MPI

* foo version ``1.1``, built for:

  * ``gcc/8.2.0`` and ``openmpi/3.1``
  * ``gcc/8.2.0`` and ``mvapich/2.1``
  * ``gcc/8.2.0`` and no MPI
  * ``intel/2018`` and ``openmpi/3.1``
  * ``intel/2018`` and ``mvapich/2.1``
  * ``intel/2018`` and ``intelmpi``
  * ``intel/2018`` and no MPI
  * ``pgi/18.4`` and ``openmpi/3.1``
  * ``pgi/18.4`` and ``mvapich/2.1``
  * ``pgi/18.4`` and no MPI

* bar version ``5.4``, built with:

  * ``gcc/9.1.0`` and supporting ``avx2``
  * ``gcc/9.1.0`` and supporting ``avx``

* bar version ``4.7``, built for:

  * ``gcc/8.2.0`` and supporting ``avx``
  * ``gcc/8.2.0`` and supporting ``sse4.1``

I.e., we have 3 families of compiler suites with 2 different versions each. And
two MPI families (openmpi and mvapich) with two versions each, with the
most recent version only built with the latest compiler version of each family,
and the older version built with both versions of each compiler family.
In addition, it is assumed that the intel compiler suites include Intel's MPI
library built for that compiler. The application foo depends on the compiler and optionally on MPI libraries
and has two versions; the newer version *mostly* has builds for the latest compiler
and MPI (for pgi it only supports the latest compiler and older openmpi), and the
older version *mostly* has builds for the older compiler and MPI. The bar application
depends on compiler and has variants depending on size of integers used in the API.

We also assume that the ``gcc/8.2.0`` compiler is the system default; i.e. it is
the compiler provided by default by the Linux distro used by the system, and
therefore might potentially be available to users without loading any modules.

More directories under doc/example/compiler-etc-dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The modulefiles for the different strategies do not play well with each
other, in part because we use the same names for many of the modules
between strategies. So in addition to the ``dummy-sw-root`` subdirectory,
each strategy has its own modulepath tree subdirectory
underneath ``doc/example/compiler-etc-dependencies``. There are some minor
differences between the modulefiles for the :ref:`Modulerc-based_strategy`
depending on whether Environment Modules 3.x or 4.x is being used,
so we actually have two trees for that case (``modulerc3`` and ``modulerc4``).

As there are a fair number of modulefiles, we make use of various tricks
in the cookbook :ref:`tips-for-code-reuse` to minimize the amount of repeated
code. In general, the actual modulefiles are small "stubfiles", setting one or a few
Tcl variables, and then sourcing a ``common`` tcl file which does all the
real work. Symlinks are used where possible to avoid duplicating files.
The :ref:`Modulerc-based_strategy` also uses some complicated ``.modulerc`` files; these
are fairly generic and to avoid redundancy are symlinked into the appropriate
places in modulepath tree from the ``modrc_common`` directory.

We also in some cases use Tcl procedures; for the sake of the
examples these our sourced in the files as needed, but if one were to
use the strategies needing such in production it would be better to follow
the suggestions in :ref:`expose-procs-to-modulefiles` and
place the required procedures in site config script.
The various tcl procedures are placed in the ``tcllib`` sub-directory, outside
of the modulepaths. These are actually broken up into multiple files
for the purpose of this cookbook (so that smaller chunks of code can be
looked at in this document).

The ``example-sessions`` subdirectory contains various shell scripts
used for the usage examples for each strategy (shell scripts are used
because there are some slight variations required between the strategies),
as well as the outputs of running such scripts. Subdirectories exist
for each strategy, and beneath them for each of the two Environment
Modules versions (``3.2.10`` and ``4.3.1``) used; for brevity not all of them
are shown in this document, especially as the 3.x and 4.x differences
are often small. In the example outputs, the Environment Modules
version and the strategy being employed is indicated in the shell prompt
(e.g. ``mod3-flavours`` or ``mod4 (modulerc)``).

Using the playground environments
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Although we strive to provide a decent discussion in this cookbook,
you are encouraged to try things out in the playground in order to
get a better feel for things.

Because we use some modulefile names (e.g. gcc, intel, pgi, openmpi, etc)
that likely are present on your system as well, it is recommended
that if you wish to explore the playground environment that you
spawn a new shell, do a "module purge", and then set your ``MODULEPATH``
environment variable appropriately for the specific strategy.

The `Flavours_strategy`, as will be discussed, requires some modifications
to your Environment Modules installation. It is recommended that
you you make a copy or new installation (for Flavours, a 3.x install
works best), and then spawn a new shell and initialize the new Flavours
install in that first. Flavours code is not provided with this cookbook.

Some of the modulefiles, etc. require knowledge of where they were
installed. To avoid requiring you to update lines in numerous
files, we require you to set the environment variable
``MOD_GIT_ROOTDIR`` to location where the modules git working directory
was cloned. E.g., if you issued the command
``git clone https://github.com/cea-hpc/modules.git ~/modules.test``
you should set ``MOD_GIT_ROOTDIR`` to ``~/modules.test``. Please
ensure it is exported (use ``setenv`` in csh and related shells,
or ``export`` in Bourne derived shells like bash). This is just
a hack to make the examples work better; if you opt to use one of
these strategies in production, you will want to hard code some
relevant paths; the comments in the modulefiles will describe
what needs to be done.

Some more detail on setting up the playground is given at the
start of the Examples section for each strategy.

.. _Flavours_strategy:

Flavours Strategy
-----------------

The Flavours strategy uses the `Flavours extension`_ to Environment Modules.
Unlike the other strategies discussed, this requires the separate download and
installation of an extension to Environment Modules.

.. _Flavours extension: https://sourceforge.net/projects/flavours/

Installation and Implementation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

More details can be found at the website for this extension, but to install this
you basically just need to:

#. Clone the git repo somewhere (``git clone https://git.code.sf.net/p/flavours/code flavours-code``)
#. Rename the standard Environment Modules ``modulecmd`` file (in the ``bin`` subdirectory
   under the installation root) to ``modulecmd.wrapped``. (It is recommended that you do this
   in a copy of your production installation, or better yet, in a new install of the 3.x Environment
   Modules (as Flavours has been developed for Modules 3.x))
#. Copy the ``modulecmd.wrapper`` file from Flavours to the ``bin`` subdirectory above. Make sure
   the ``modulecmd.wrapper`` file is executable.
#. Symlink ``modulecmd.wrapper`` to ``modulecmd``
#. Edit ``modulecmd.wrapper`` where indicated to give fully qualified path to ``modulecmd.wrapped``
#. Copy the ``flavours.tcl`` and ``pkgIndex.tcl`` files to some (possibly new) directory under
   the modules installation root, and set ``TCLLIBPATH`` to that directory (you probably will want to
   add that to the various modules init scripts)

The ``module`` command invokes ``modulecmd``, which in this case is results
in the Flavours wrapper bash script ``modulecmd.wrapper`` being invoked. This
calls the renamed standard ``modulecmd.wrapped`` command. This wrapper
command catches and processes certain output from the modulefile evaluation
intended for its consumption.

The modulefiles themselves make use of various commands in the Tcl module ``flavours``.
Many of these are just flavours variants of standard modulefile commands, e.g.
``flavours prepend-path`` versus ``prepend-path``. Some important flavours
commands:

*  ``package require flavours``: This loads the Tcl package flavours, and should occur
   near the top of your modulefile
*  ``flavours init``: This initializes the flavours package, and should be the first
   of the flavours commands issued. Typically call right after the package load.
*  ``flavours prereq``: Like the standard ``prereq`` command, this declares a prerequisite.
   But it also does quite a bit more, as is discussed further below.
*  ``flavours root``: This is used to set the root for where the package is actually
   installed. This is used when generating the ``flavours path``
*  ``flavours revision``: seem intended to allow for changes in the path format in future
   versions of ``flavours``. It is used in constructing the final path to the package.
*  ``flavours conflict``: This is similar to the standard conflict command, but enhanced
   to recognize the flavours prereqs above.
*  ``flavours commit``: This should be called after the ``root``, ``revision``, and ``prereq``
   sub-commands of ``flavours`` are called, and before any of the ``path`` sub-commands.
   It seems to be responsible for taking all those values to above and constructing the
   path to the package.
*  ``flavours path``: This returns a string with the path to the specific build of the package.
*  ``flavours prepend-path``, ``flavour append-path``: These work much like the standard
   ``prepend-path`` and ``append-path``, except that the value being prepended/appended
   to the environment variable has the path (as returned by ``flavours path``) prepended
   to it with the appropriate directory separator. E.g., to add to the ``PATH`` variable the bin subdirectory
   of the root directory where the specific build was installed, use ``flavours prepend-path PATH bin``
*  ``flavours cleanup``: This should be called after all ``flavours`` sub-commands are finished
   and before exiting the script to ensure proper cleanup. Among other things, it ensures
   that any packages that depend on this package will get reloaded if this package is switched out.

The ``flavours prereq`` command accepts the new ``-class`` parameter, allowing
it to require a class of packages; e.g. one could use ``-class compiler`` to indicate
that it has a prereq on a compiler (any of the modules ``gnu``, ``intel``, or ``pgi``).
The allowable classes, and the package basenames that are in each class, is defined in
``flavours.tcl`` in the Tcl associative array ``_class``. The ones shipped by default are

* compiler: consisting of gnu, intel, and pgi
* mpi: consisting of openmpi, mvapich2, mvapich, intelmpi
* linalg: consisting of mkl, atlas, acml, netlib

You will likely want to adjust these if you go with flavours in production.

The ``flavours prereq`` command also accepts the parameter ``-optional``, which declares
optional prerequisites. Although it sounds a little oxymoronic, this comes into play
with the secondary purpose of the command in declaring the components of the path, as
discussed below. If a prereq is not optional, the modulefile will complain if nothing
satisfying the prereq has been module loaded previously. If the prereq is optional,
the modulefile will not complain if it was not loaded, but will use the prereq in
constructing the path to the build of the package if it was loaded.

The ``flavours prereq`` command also defines the components which will comprise the final
path to the directory containing the specific build of the package. The order of the
prereq commands controls the order of the components in the path.

The modulefile will check that all non-optional ``flavours prereq`` commands are satisfied,
and then construct a path to the installation root for this build of the package using
the packages satisfying the prereqs. The resultant path is composed of:

* the value from ``flavours root``
* directory separator (``/``)
* the value from ``flavours revision``
* directory separator (``/``)
* a ``prefix`` created by concatenating the package names satisfying the prereqs, in order.
  The package name and version will be separated by a hyphen (-), as will the different
  components.

So if ``flavours root`` was set to ``/local/software/foo/1.7``, ``revision`` to 1, and the
package had prereqs compiler and mpi, and ``gnu/9.1.0`` and ``openmpi/4.0`` were loaded, the
resulting path would be ``/local/software/foo/1.7/1/gnu-9.1.0-openmpi-4.0``.

The modulefile actually will test for the existence of that directory, and if not found will
return an error to the that the package was not built for that combination of prereqs.
You either need to install your packages using the above directory schema, or create
symlinks linking that scheme to where you actually install the packages.

Examples
^^^^^^^^

We now look at the example modulefiles for flavours. To use the examples,
you must

#. Have `Flavours extension`_ installed. **NOTE** these examples will NOT work without
   the Flavours installed.
#. Set (and export) ``MOD_GIT_ROOTDIR`` to where you git-cloned the modules source
#. Do a ``module purge``, and then set your ``MODULEPATH``
   to ``$MOD_GIT_ROOTDIR/doc/example/compiler-etc-dependencies/flavours``

We start with the ``module avail`` command:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/flavours/modules4/modavail.out
    :language: console

We note that we only see the package names and versions; e.g. ``foo/2.4``, without any mention
of the compilers and MPI libraries for which it is built. This terser type was an intentional
design goal of the authors. Also of note are the intelmpi and simd packages. The Flavours
approach relies on seeing what modules have been loaded previously in order to determine what
'flavor' of the requested package should be loaded. To support the different builds of ``bar``
which depend on the CPU vectorization commands supported, we need to add a "dummy" package ``simd``.
The module definition is quite trivial; a simple stub file like

.. literalinclude:: ../../example/compiler-etc-dependencies/flavours/simd/avx
    :language: tcl

and the main content in the ``common`` file:

.. literalinclude:: ../../example/compiler-etc-dependencies/flavours/simd/common
    :language: tcl

Basically it just declares a help procedure and whatis text. This way, an user can
load the appropriate simd module to control which variant of bar they will get. The
only interesting aspect is that near the beginning of the file we do a
``package require flavours`` and ``flavours init``, and add a ``flavours cleanup``
near the bottom. The lines at the beginning instruct Tcl command to load the Flavours
package, and then initialize the package.
The ``flavours cleanup`` is required so that if the simd module is switched out,
any modulefiles that depend on it get reloaded.

In our example, we assumed that the Intel MPI libraries are automatically set up properly if one
were to load the ``intel`` module, and we assumed the Intel MPI libraries were not supported
for either the GNU or PGI compilers. However, we also wished to allow for ``foo`` to
be used without any MPI support. So we need a way to distinguish if someone wants to
use an Intel compiler build of ``foo`` without MPI or with the Intel MPI libraries. Our
choice for this example was to require one to explicitly module load ``intelmpi`` if one
wished to use the Intel MPI variant --- we do not bother with a real version number because
assuming the version is determined by the version of ``intel`` (the Intel Parallel Studio version).
So the intelmpi modulefile is similar to the simd modulefiles, a dummy modulefile. Again,
it includes the ``flavours init`` and ``flavours cleanup`` wrapping to ensure proper reloading
of dependent modules should it be switched out.

If you were to support Intel MPI for non-intel compilers, you could create your intelmpi
modulefiles as usual, and then add a ``default`` or ``intel`` "dummy" version to use the
version that is part of the ``intel`` Parallel Studio. Or you could separate the intelmpi
bits from the ``intel`` modulefile so both non-intel and intel compilers need to explicitly
module load intelmpi.

The modulefiles for the various compilers are all pretty much standard, except for the
same three ``flavours`` lines as the simd modulefile: ``package require flavours``,
``flavours init``, and ``flavours cleanup``. These are required to
ensure dependent modulefiles get reloaded if the compiler is switched out.
We also note that the modulefile for the GNU Compiler Collection is referred to as ``gnu``, not ``gcc``
(this is due to how the ``compiler`` class is defined in ``flavours.tcl``, and we did not bother
to change that for the purposes of this cookbook).

With the openmpi and mvapich MPI libraries, things start to get interesting. These all
should setup the environment for a different build depending on the compiler loaded. The
real work is done in the ``common`` tcl file, as shown below:

.. literalinclude:: ../../example/compiler-etc-dependencies/flavours/openmpi/common
    :language: tcl

Like the previous cases, the file starts with the Tcl command to load the
package, followed by the ``flavours init`` command.

The ``flavours prereq`` command states that this package requires a compiler to have been previously
loaded, and that the path to the specific build to use will depend on that. We note the use
of the ``-class`` parameter; the exact definition of the compiler class is in the ``compiler``
field of the Tcl associative hash ``_class`` defined in ``flavours.tcl``.

The ``flavours root`` sets the root directory of where the builds for this package is installed.
We use the ``MOD_GIT_ROOTDIR`` environment variable for convenience in this example, but in production
you would generally hardcode a path. The result of all the directives is that the build will be
found in a path named after the compiler (since in this case there is only one ``flavour prereq``);
e.g. for gcc version ``9.1.0``, we expect to find the build in ``$swroot/openmpi/4.0/1/gnu-9.1.0``.
If you do not use that naming convention for your installation directories, you can use symlinks
to fake it.

The ``flavours path`` command in the ``setenv MPI_DIR`` statement sets MPI_DIR to the aforementioned
build path. The ``flavours prepend-path`` commands prepend to the environment variable specified
by the first argument the result of prepending the ``flavours path`` to their second argument. E.g.,
the first such, assuming openmpi version ``4.0`` was requested and ``gnu/9.1.0`` loaded, would be basically
the same as a standard Modules command:

.. code-block:: tcl

   prepend-path PATH $swroot/openmpi/4.0/1/gnu-9.1.0/bin

The following shows how this would appear to the user:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/flavours/modules4/ompi-loads1.out
    :language: console

Here we note that once a compiler is loaded, the ``PATH`` and the other environment
variables are set appropriately to point to the bin dir for the particular build of
``openmpi/4.0``, as evidenced by the output of our dummy mpirun command. At the end, we attempt
to load ``openmpi/4.0`` for ``gnu/8.2.0``, and receive an error because our dummy SW library does not contain
a matching build. This is determined from the ``flavours path``; if the
path does not exist (in this example ``$swroot/openmpi/4.0/1/gnu-8.2.0``) it will
abort in this fashion.

In the above, we have explicitly unloaded openmpi, switched the compilers, and
then reloaded openmpi. A nice feature of `Flavours <Flavours extension>`_ is that it can handle the
switching out of compilers or other modulefiles which other modulefiles depend on,
as:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/flavours/modules3/ompi-switch.out
    :language: console

Note that when we switched between the pgi and intel compilers above, Flavours
automatically "unloaded" and "reloaded" the openmpi module. This happens in
the ``flavours cleanup`` portion of the compiler modulefiles, and is due to
``openmpi`` declaring a ``flavours prereq`` on the compiler class.

.. note:: The above behavior with ``switch`` was done with version ``3.2.10``
   of Environment Modules; it does *not* appear to work with ``4.3.1``.

Note that when we further tried to replace version ``2019`` of the intel compiler with the ``2018`` version,
the module switch of the compilers failed because ``openmpi/4.0`` was not built with
``intel/2018``. Since the user never explicitly requested version ``4.0`` of openmpi (it was
defaulted in the initial load as the latest version of openmpi available for ``pgi/19.4``),
it would have been nicer had the attempted reload of openmpi allowed it t:o default to the ``3.1``
version (as the latest version available for ``intel/2018``). Nevertheless, it behaved well
in this situation; the module switch failed with a reasonable error message and the resulting
set of modules was still consistent.

We also note that if we attempt to load openmpi without having previously loading a compiler,
we will get an error:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/flavours/modules4/ompi-defaults.out
    :language: console

In particular, there is no support for a "default" compiler; if e.g. you wished to make the
distribution supplied gcc the default compiler, you will need to have the initializations
scripts automatically do a module load of that compiler (possibly
a dummy modulefile like simd/intelmpi if the compiler is already in the user's path)
in your user's start up dot files or similar. We also note that there is no additional
intelligence in the version defaulting --- in the last example, we have ``gnu/8.2.0`` loaded and if
we try to load openmpi without specifying a version, it defaults to version ``4.0``
as that is the latest version of openmpi without regard for the fact that there
is no build of openmpi version ``4.0`` for ``gnu/8.2.0`` (but there is such for ``openmpi/3.1``).

The situation for ``foo`` is more complicated, as it depends both on the compiler and
optionally on the MPI library. But with Flavours, the modulefile is only slightly more
complicated, e.g. for the common file is:

.. literalinclude:: ../../example/compiler-etc-dependencies/flavours/foo/common
    :language: tcl

Basically, the main difference is the addition of the
line ``flavours prereq -optional -class mpi``.
This instructs Flavours that there is an additional, optional prereq. The
order of the prereq lines matter, as that controls the resultant ``flavors path``.
With the current configuration, assuming ``gnu/9.1.0`` and ``openmpi/4.0`` were loaded,
the path would become ``$swroot/foo/2.4/1/gnu-9.1.0-openmpi-4.0``. If the order were
reversed, the ``openmpi-4.0`` would precede the ``gnu-9.1.0``. Because the MPI requirement
is optional, if ``gnu/9.1.0`` was loaded and no MPI library loaded, the path would
evaluate to ``$swroot/foo/2.4/1/gnu-9.1.0``.

We show how it works below:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/flavours/modules4/foo-loads.out
    :language: console

So basically, if the user loads a compiler, the
the environment variables (``PATH``, etc) are set up for the correct build of foo.
If no MPI library was loaded, a version of foo built without MPI will be loaded,
otherwise, a version of foo built with the loaded MPI library will be loaded.
This is shown by the output of the ``foo`` command. Note
also how we use the dummy ``intelmpi`` package to indicate a desire for the
intelmpi enabled version.

The 3.x version of Environment Modules supports using the switch command on
either the compiler or MPI library, and will result in reloading of foo and the MPI
library.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/flavours/modules3/foo-switch.out
    :language: console

In particular note the final case, wherein we load ``intel/2019`` then foo, and get the
version of foo built without MPI. When we subsequently load openmpi, foo is reloaded
to be the openmpi version (this is because the hooks to reload foo are in the ``flavours cleanup``
part of the openmpi modulefile, and foo declared its optional dependency on MPI).
Also, we don't bother showing it, but if you were to attempt to load foo without
at least a compiler loaded, it would display an error.

Our final example for flavours is the ``bar`` command. Here in addition to the
compiler dependency, we have versions for different SIMD vectorization supported.
Again, the difference in the modulefile is small, e.g.

.. literalinclude:: ../../example/compiler-etc-dependencies/flavours/bar/common
    :language: tcl

Basically, the optional ``flavours prereq`` on the mpi class from the ``foo`` package
is replaced by a (mandatory) ``flavours prereq`` on the ``simd`` dummy package.
We note that Flavours package knows nothing about our ``simd`` dummy package until
we add it as a prereq for bar. (This is in contrast to the compiler and mpi classes).
Usage would be like:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/flavours/modules4/bar-loads.out
    :language: console

Here we note that as both the compiler and simd prereqs are non-optional, it complains
unless both have been previously loaded. When both have been loaded, the ``PATH`` and
other environment variables are set appropriately for the requested build; and if
it does not exist and error is produced.

Summary of Flavours
^^^^^^^^^^^^^^^^^^^

* It is an external extension to Environment Modules, requiring additional installation steps.
* The git repository appears to have been last updated in 2013; although which means
  that it has not been updated for Environment Modules 4.x, a simple experimentation
  indicates that it still works, with the exception of automatic reloading of a module if any of
  the modules it depends on are switched.
  However, the `Flavours extension`_ does not appear to be actively supported.
* The Flavours package (using Environment Modules 3.x) fully supports the ``module switch`` syntax,
  with the switching out of a dependency (e.g. a compiler) causing the reload of all modulefiles depending on it.
  (however the test of this feature failed when using Environment Modules 4.x.)
* The syntax for modulefiles is elegant, and one can easily extend the basic compiler dependency
  modulefile to add additional dependencies. Even for packages/dummy packages that the Flavours
  extension knows nothing about (e.g. simd in the above example).
* The Flavours package will provide a shorter module avail output, only e.g. giving package name and
  version and not listing a separate modulefile for each combination of package, version, compiler+version,
  MPI library+version, etc.
* The Flavours package will fail with an error message if user tries to load a package which was
  not built for the values of the dependency packages loaded.
* The Flavours package will fail to load with an error message if any dependent package is not
  already loaded. In particular, it will not attempt to default these. So if such defaults are
  desired, you will need to have initialization scripts automatically load the appropriate modules.
* The Flavours package does not include any mechanism for more intelligent defaulting. I.e., if an
  user requests to load a package without specifying the version desired, the version will be defaulted
  to the latest version (or whatever the ``.modulerc`` file specifies) without regard for which versions
  support the versions of the compiler and other prereq-ed packages the user has loaded. While one
  could write custom ``.modulerc`` files for such, Flavours does not provide any tools for simplifying
  such.


.. _Homebrewed_flavors_strategy:

Homebrewed Flavors Strategy
---------------------------

Although the `Flavours extension`_ described above has an elegance about it,
one can achieve much of the same functionality in modulefiles using standard
Environment Modules and Tcl commands. This can be facilitated by the definition
of some useful Tcl procedures. For lack of a better name, we will refer to
this strategy as *Homebrewed flavors*.

Implementation
^^^^^^^^^^^^^^

This strategy just makes use of standard Environment Modules and Tcl procedures
to query what modules of a given type are loaded and to construct the path to the
software package accordingly. To avoid needless (and error prone) repetition of
code, we collect these into several Tcl procedures of our own. Ideally, these
should be placed in a site configuration Tcl file and exposed to modulefiles
as explained in the cookbook :ref:`expose-procs-to-modulefiles`.
However, to avoid the need for that in these examples, we instead have placed them
into a file and use the ``MOD_GIT_ROOTDIR`` to locate and source that file in the
relevant modulefiles. (Actually, we have a single tcl file that is sourced both
for this and some other strategies, and it sources several files so that we can
break up the discussion of the the Tcl procedures. All of that is just for the
purposes of this cookbook; normally you just put the procedures you need in the
one site config file).

We discuss the various Tcl procedures here, as they are what provide most of the
functionality. We start with the routines for generic loaded modules:

.. literalinclude:: ../../example/compiler-etc-dependencies/tcllib/LoadedModules.tcl
    :language: tcl
    :lines: 4-

This defines the two Tcl procedures:

* **GetLoadedModules** : this returns the list of loaded modules, from the ``LOADEDMODULES``
* **GetTagOfModuleLoaded** : this takes as argument the base name of a package,
  and returns the first full spec for the matching package, or an empty
  string if no matching package found.

The Tcl procedure **GetTagOfModuleLoaded** can be used to find out what version
of a given package is loaded, and is enough for many packages. However, for
compilers, and similar, a bit more is needed. For compilers:

.. literalinclude:: ../../example/compiler-etc-dependencies/tcllib/CompilerUtils.tcl
    :language: tcl
    :lines: 4-

We defined four procedures above:

* **GetDefaultCompiler** : this simply returns the name of our default compiler, which for
  this example is ``gcc/8.2.0``
* **RequireCompiler** : this simply does a module load on the specified compiler.
  It is kept as a separate procedure just in case you wish to intercept and prevent
  the loading of the default compiler (e.g. because no modulefile exists for it).
  In our example, there is a modulefile for it and so it is just a wrapper for ``module load``.
* **GetKnownCompilerFamilies** : this simply returns a Tcl list of known
  compiler families.
* **GetLoadedCompiler**: this is the procedure that does the main work, and is
  described in detail below.

The **GetLoadedCompiler** procedure basically checks if any packages matching
the names in **GetKnownCompilerFamilies** have been previously loaded. If so,
it returns the modulefile specification for the first one found, and returns.
If not, if ``pathDefault`` is set and there is a recognized compiler name and version
in the last two components of the module specification, it will return
that compiler. Otherwise, if the optional flag ``useDefault`` is set, it will return the
value from **GetDefaultCompiler**. If all else fails, returns the empty string.

If the optional parameter ``loadIt`` is set, if a compiler was defaulted (i.e.
not returned because it was already loaded), the procedure will call ``RequireCompiler``
to module load it.

If the optional parameter ``requireIt`` is set, we invoke ``prereq`` on the compiler
found before returning.

A similar set of procedures exist for the MPI libraries, namely:

.. literalinclude:: ../../example/compiler-etc-dependencies/tcllib/MpiUtils.tcl
    :language: tcl
    :lines: 4-

The three procedures here are analogues of the compiler versions:

* **RequireMPI** : this basically does a module load of the specified MPI library. It
  has some added logic so that it will not do a module load if the MPI library is ``nompi``.
  Also, if the optional parameter ``noLoadIntel`` is set, if the MPI library is ``intelmpi``
  (or a variant of that name) and the loaded compiler in ``intel``, we assume that no
  additional module needs to be loaded. For this strategy, we want to load ``intelmpi``
  modules, because, just like in the :ref:`Flavours_strategy`, we need to provide dummy
  ``intelmpi`` modules to allow one to request the use of the Intel MPI library.
* **GetKnownMpiFamilies** : this returns a list of known MPI library family names.
  Used in **GetLoadedMPI**
* **GetLoadedMPI** : This is the analogue of **GetLoadedCompiler**. If an MPI library is
  loaded, it will return the name of that module. If the optional ``requireIt`` flag
  is set, it will do a ``prereq`` on the MPI library before returning. The first optional
  argument, ``useIntel``, indicates whether this module should return ``intelmpi`` if no
  MPI library is loaded but an Intel compiler is loaded.

The modulefiles for the compilers are basically standard; unlike the
:ref:`Flavours_strategy` there is nothing special needed in these. Likewise
for the dummy ``simd`` and ``intelmpi`` modules (the latter is only this
basic because we assume intelmpi is only available if an Intel compiler
is loaded. If one allowed for intelmpi with other compilers, it would
more closely resemble the other MPI libraries).

We also define some Tcl procedures for generating warning and error messages, namely

.. literalinclude:: ../../example/compiler-etc-dependencies/tcllib/WarningUtils.tcl
    :language: tcl
    :lines: 4-

These procedures:

* **PrintIfLoading**: will print supplied text to stderr only when in ``load`` mode
* **PrintLoadInfo**: will print supplied text as informational text, but only when trying to load a module
* **PrintLoadWarning**: will print supplied text as warning text, but only when trying to load a module
* **PrintLoadError**: will print supplied text as error text and abort, but only when trying to load a module

The gist of this is that we might wish to print errors if an user tries to
load an incompatible modulefile, but do not wish to print errors if they are merely
doing a help, display, or whatis command.

The interesting bit begins with the openmpi and mvapich modulefiles. These
both depend on the compiler, we show the main part of the openmpi modulefile
below:

.. literalinclude:: ../../example/compiler-etc-dependencies/homebrewed/openmpi/common
    :language: tcl

We begin by sourcing the ``common_utilities`` file which defined the previously
described Tcl procedures. Normally it is recommended that you put those
procedures in a site config Tcl script and expose them to the modulefiles
using the techniques described in the cookbook :ref:`expose-procs-to-modulefiles`.
Even if you opt against that and decide to source a Tcl file, it is recommended
to hard code the path.

The next interesting bit comes when we set the local Tcl variable ``ctag``
by calling the ``GetLoadedCompiler`` procedure. We allow the procedure to
default the compiler, and because we have a default compiler defined we
should always get a value. (If no default compiler was defined, one would
have to handle the error if no compiler was loaded/defaulted.) We then use
the value of ``ctag`` to set the path to the build of the package. To ensure
that the package is built for this compiler, we do a quick check that the
package installation path exists.

The modulefile for ``foo`` is a bit more complex:

.. literalinclude:: ../../example/compiler-etc-dependencies/homebrewed/foo/common
    :language: tcl

The main difference between this modulefile, depending on both compiler and
optionally MPI, and the openmpi modulefile above, is that in addition to
detecting which compiler is loaded, we call ``GetLoadedMPI`` to determine
the MPI library which was loaded, and use both of them in constructing the
prefix to the installed foo.

Examples
^^^^^^^^

We now look at the example modulefiles for the *Homebrewed flavors* strategy.
To use the examples, you must
#. Set (and export) ``MOD_GIT_ROOTDIR`` to where you git-cloned the modules source
#. Do a ``module purge``, and then set your ``MODULEPATH`` to:

.. code-block:: shell

   $MOD_GIT_ROOTDIR/doc/example/compiler-etc-dependencies/homebrewed

The *Homebrewed flavors* strategy behaves much like the :ref:`Flavours_strategy`
in practice. The module avail command,

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/modavail.out
    :language: console

looks basically the same, showing the a concise listing of packages and
versions without information on the compilers and MPI libraries they were
built with.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/ompi-loads1.out
    :language: console

Again, once a compiler is loaded, loading openmpi will set the ``PATH``, etc. for
the correct build of openmpi, as evidenced by the output of the dummy
mpirun command. Notice that the ``module list`` command only shows
the version of openmpi loaded, and contains no information about what
compiler it was built with (you just have to assume it matches the loaded
compiler). And again we note that if one attempts to load a version
of openmpi (e.g. ``4.0``) that was not built for the specified compiler (e.g.
``gcc/8.2.0``), an error is generated.

Unlike in :ref:`Flavours_strategy`, we did not put any code in the modulefiles to cause
dependent modulefiles to be reloaded if a module they depend on gets switched
out. However, starting with Environment Modules ``4.2``, a feature called
:ref:`Automated module handling <v42-automated-module-handling-mode>` was added. Without this feature, attempting to
switch out a module upon which other modules depended could be problematic,
as evidenced in this sequence below (using Environment Modules ``3.2.10`` and
so without automated module handling mode):

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules3/ompi-switch.out
    :language: console

Here we note that we were able to switch out the pgi compiler for the intel compiler,
but the openmpi module was not reloaded and the environment is still set for openmpi
compiled with the pgi compiler, and that this inconsistency is not readily determined
from the module list command.

Environment Modules 4.x, even with automated module handling disabled, is better ---
in a command sequence as above the module switch from pgi to intel would fail due to the
prereq module. However, with the :ref:`Automated module handling <v42-automated-module-handling-mode>`
enabled (this feature is disabled by default but could be enabled on a per-command basis with the ``--auto``
flag), things work much better, as evidenced below:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/ompi-switch.out
    :language: console

When we switch out the pgi compiler for the ``intel/2019`` compiler, the openmpi
module is automatically unloaded before the compiler switch and reload afterwards,
so we end up with the correct build of openmpi for the newly loaded ``intel/2019``
compiler. If one then switches out ``intel/2019`` replacing it with ``intel/2018``,
the openmpi module is first unloaded, the compilers are switched, and as there
is no ``openmpi/4.0`` build for ``intel/2018``, a warning is given and the openmpi module
is left unloaded. Since the user never specifically requested version ``4.0`` of openmpi
(it was defaulted in the original module load of openmpi as that was the latest version
available for ``pgi/19.4``), it would have been nicer if on the switch of intel compiler
versions the reload only attempted a ``module load openmpi`` instead of ``module load openmpi/4.0``,
but nevertheless this well behaved. The openmpi module is dropped with a warning and the
user has a consistent set of modules loaded.

We note that the modulefile is able to
default the compiler, so when we attempt to load openmpi without having
previously loaded a compiler, as in

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/ompi-defaults.out
    :language: console

it will default to the default compiler, ``gcc/8.2.0``. Note however, that if
one does not specify version ``3.1`` of openmpi, it will still default to ``4.0``
and fail to load as there is no build of ``openmpi/4.0`` for ``gcc/8.2.0``.

The situation is similar for foo:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/foo-loads.out
    :language: console

Again, one can load a compiler without an MPI library to get the non-MPI version
of foo, or a compiler and MPI library to get the MPI version. The dummy
intelmpi modulefile is used to allow one to indicate that the Intel MPI library
is desired. The automated module handling mode can again allow the switch
functionality work properly, as in

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/foo-switch.out
    :language: console

Here we note a deficiency in the switch support as compared to :ref:`Flavours_strategy`. In the last example
after loading ``intel/2019`` and foo, we have the non-MPI build of foo as expected. However, upon
subsequently loading the openmpi module, we still have the non-MPI version of foo loaded, as evidenced
by the output of the dummy foo command. I.e., the foo package was *not* automatically reloaded, as
there was no prereq in the foo modulefile on an MPI library (as in the non-MPI build there is no MPI
library to prereq). Also note that module list does not really inform one of this fact.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/foo-defaults.out
    :language: console

Above, we see once more that the compiler can be defaulted, but that the
defaulting mechanism is not smart enough to default the version of foo based
on the compiler loaded (or defaulted to).

The situation with bar is basically the same; with a compiler and simd
module loaded, the environment for the appropriate build of bar is loaded
when you module load bar.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/bar-loads.out
    :language: console

And an error is generated if there is no build for that combination of
compiler and simd. The automatic handling of modules again allows the
switch command to work as expected:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/bar-switch.out
    :language: console

and both the simd level and compiler can be defaulted, but one still has
to choose a version of bar which supports the defaults.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/homebrewed/modules4/bar-defaults.out
    :language: console

Summary of Homebrewed flavors strategy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* The :ref:`Automated module handling <v42-automated-module-handling-mode>` feature (introduced in Environment Modules
  ``4.2``) allows for the switching out of a dependency (e.g. a compiler)
  to cause the reload of all modulefiles depending on it. Without the
  automated module handling (as is default for 4.x, and the only option for
  3.x), the switching of a compiler only changes the compiler and leaves the
  modulefiles that depend on the compiler unchanged.
* The various Tcl procedures make
  it somewhat easy to determine which compiler, MPI, etc. modules have been
  loaded and set the paths appropriately. Not as elegant or easy to use
  as :ref:`Flavours_strategy`, but not difficult
* Like the Flavours package, the module avail output is concise, only e.g.
  giving package name and version rather than listing a separate modulefile
  for each combination of package, version, compiler+version,
  MPI library+version, etc.
* Modules will fail with an error message if user tries to load a package
  which was not built for the values of the dependency packages loaded.
* With :ref:`v42-automated-module-handling-mode`, the dependencies of a module
  asked for load could automatically be loaded. But it requires that these
  dependencies are clearly specified with mention of the versions for which
  a build is available.
* It does not include a mechanism for more intelligent defaulting. I.e., if
  an user requests to load a package without specifying the version desired,
  the version will be defaulted to the latest version (or whatever
  the ``.modulerc`` file specifies) without regard for which versions
  support the versions of the compiler and other prereq-ed packages the user
  has loaded.
* Note that future releases of Environment Modules will introduce additional
  mechanisms to the :ref:`v42-automated-module-handling-mode`, which will
  improve the user experience on such *Homebrewed flavors* setup.


.. _Modulerc-based_strategy:

Modulerc-based Strategy
-----------------------

The previous two strategies used additional code in the modulefile to
determine which compiler, etc. was loaded and adjust the values for
``PATH``, etc. accordingly. The *Modulerc-based* strategy instead uses
``.modulerc`` files to direct the module command to the proper modulefile
depending on what compiler, etc. was previously loaded. Because of
this, there are a number of differences in behavior and what is seen
by the user, most notably many, many more modulefiles. Whether this is good
or bad is a matter of taste.

Implementation
^^^^^^^^^^^^^^

Whereas the :ref:`Homebrewed_flavors_strategy` had the modulefile invoke
a Tcl procedure to determine which, if any, version of a module like a compiler
was loaded and then adjust paths, the *Modulerc-based* strategy instead
uses the same Tcl procedures to default the modulefile which will be loaded.
This implies that there is a distinct modulefile for every build of the package,
and an immediate consequence is that this strategy has many more modulefiles
than the others. We make use of the techniques in the cookbook
:ref:`tips-for-code-reuse` to reduce the total amount of code; the actual modulefiles
for each build are typically small stubfiles defining a couple of Tcl variables
and then sourcing a ``common`` script (unique to each package) which does all
the real work.

The modules will be named with components for the different dependencies,
so the one for openmpi version ``4.0`` built with gcc version ``9.1.0`` would
be ``openmpi/4.0/gcc/9.1.0``; similarly the module for foo version ``1.1``
built for pgi version ``18.4`` and mvapich ``2.1`` would be
``foo/1.1/pgi/18.4/mvapich/2.1``.

The ``.modulerc`` files themselves are not trivial, but these
can generally be written in a generic fashion, usable by multiple packages,
and can just be symlinked to the appropriate locations.
We define five such files which can be linked as ``.modulerc``:

Two of these files can be linked in the module tree
at various places as ``.modulerc`` for defaulting the compiler. One to
default to the family portion of the compiler (e.g. gcc, intel, or pgi),
and one for the version. For the family portion of the compiler,
we have the file ``modulerc.select_compiler_family`` as below:

.. literalinclude:: ../../example/compiler-etc-dependencies/modrc_common/modulerc.select_compiler_family
    :language: tcl

The file starts by sourcing a set of useful Tcl procedures. For the purpose
of the example for this cookbook this is done based on the ``MOD_GIT_ROOTDIR``
environment variable. If you were to use this in production, it is
recommended that the Tcl procedures be placed in a site configuration script
and exposed to modulefiles via the techniques described in the cookbook
:ref:`expose-procs-to-modulefiles`. At the minimum, it is
recommended to hardcode the path to the ``common_utilities.tcl`` file.

The modulerc script then determines the directory it is in using the
Tcl variable ``ModulesCurrentModulefile``. It then uses the
``GetLoadedCompiler`` Tcl procedure (which was discussed in the section
on the :ref:`Homebrewed_flavors_strategy`. We then parse the resulting module
name into family and version pieces (we will discuss the procedure
``GetPackageFamilyVersion`` later; for now suffice to say it takes
a module name and returns a Tcl list with family and version).

We do some trickery to support the use of either gcc or gnu as family names for
the GNU compiler suite, and then see if there is a directory or modulefile
underneath the directory containing the ``.modulerc`` file, and if so defaults
to it. For this purpose, we use the Tcl procedure ``FirstChildModuleInList``
(defined in ``common_utilities.tcl``). This and a related procedure are
defined as:

.. literalinclude:: ../../example/compiler-etc-dependencies/tcllib/ChildModules.tcl
    :language: tcl
    :lines: 4-

and basically make use of the Tcl variable ``ModulesCurrentModulefile``. They
are limited, however, in that they will not work properly if the module is
split across multiple modulepaths. That is, ``ChildModuleExists`` and
``FirstChildModuleInList`` will only detect children in the same directory
as the modulerc file they are called from; e.g., if two paths in the ``MODULEPATH``
both have directories for openmpi, one with an intel subdirectory and one
with a pgi subdirectory and the modulerc invoking ``ChildModuleExists``, the
ChildModuleExists procedure will not see the intel directory.

The ``modulerc.select_compiler_version`` file is similar,

.. literalinclude:: ../../example/compiler-etc-dependencies/modrc_common/modulerc.select_compiler_version
    :language: tcl

Again, we source the ``common_utitilies.tcl`` file and use
``ModulesCurrentModulefile`` to get the directory in which the ``.modulerc``
script resides. It then uses ``GetLoadedCompiler`` to determine what
if any compiler was loaded. If one was, we split into family and version,
and ensure that the directory containing the ``.modulerc`` file matches the
family name. If so, it checks if there is a directory or modulefile in that
directory matching the version name, and if so defaults to it.

For both of the ``.modulerc`` scripts we examined, we note that in case of
errors, etc., the script just exits without defaulting. One might be
tempted to have the ``.modulerc`` script actually return an error in such
cases (perhaps using ``module-info mode`` to only have it output during
load operations). This, however, runs into issues:

*  For Environment Modules 3.x: for some reason, when modulecmd processes
   ``.modulerc`` scripts, ``module-info mode load`` always returns true. Because
   of this, users would get spurious errors when doing a ``module avail``
   if the ``.modulerc`` scripts error-ed, because they all get processed regardless
   of what compiler is loaded.
*  For versions 4.x: the modulecmd process tends to process all ``.modulerc``
   files underneath the package root during a load, which will similarly
   generate spurious errors if the ``.modulerc`` scripts throw errors.

Because of this, if an user explicitly requests a modulefile that conflicts
with the loaded compiler (e.g. the user does a ``module load pgi/19.4``
followed by a ``module load openmpi/4.0/intel/2019``), the modulefile needs
to detect this and error appropriately. A similar situation can happen if
one of the ``.modulerc`` scripts fail to default, presumably because there is no
appropriate build for the requested package; the modulecmd will default to
something, but likely not what is wanted. To facilitate this, we define
the Tcl procedure ``LoadedCompilerMatches``:

.. literalinclude:: ../../example/compiler-etc-dependencies/tcllib/LoadedCompMatches.tcl
    :language: tcl
    :lines: 4-

The procedure takes a string for the family and version of the compiler
that the modulefile expects, and ensures that if a compiler is loaded, they
match. If they match, the procedure just returns, and if not, it spits out
an error indicating a compiler mismatch. It also takes optional boolean flags:
``requireIt`` to determine if the procedure should ``prereq`` the compiler
before returning, and ``loadIt`` to determine if, when no compiler was
previously loaded, whether the procedure should load it (using
``RequireCompiler`` so it can properly handle the default compiler specially
if needed). The ``requireIt`` flag should generally be set for Environment
Modules 4.x (as that will allow the automatic handling of modules to
properly recognize dependencies, even though as we will see that does not
work too well for this strategy as yet), and should not be set for versions
3.x (as the loading of modules with ``loadIt`` does not occur until *after*
the ``prereq``, causing module loads to fail).

The resulting modulefile for something depending only on the compiler,
using mvapich as an example, then would look like:

.. literalinclude:: ../../example/compiler-etc-dependencies/modulerc4/mvapich/common
    :language: tcl

Basically, the modulefile knows what compiler it wants (in the above example,
that is set by the stubfile and passed as the Tcl variable ``compilerTag`` into the common
script above), and then calls the ``LoadedCompilerMatches`` procedure
above to ensure the loaded compiler matches what the modulefile wants (and
to load it if no compiler is loaded).

So the mvapich directory in the ``MODULEPATH`` consists of subdirectories for
each version of mvapich supported (e.g. ``2.1`` and ``2.3.1``). In each of the
version subdirectories, we symlink ``modulerc.select_compiler_family`` as
``.modulerc``, and create an additional layer of subdirectories, one for
each compiler family for which the version of mvapich is built (e.g.
gcc, intel, pgi). In each compiler family directory under each mvapich
version, we symlink ``modulerc.select_compiler_version`` as ``.modulerc``,
and create a stub modulefile named for the compiler version. The stub
modulefile then defines some Tcl variables for the version of mvapich
and the compiler family/version, and sources the ``common`` file above.
E.g., for ``mvapich/2.3.1/intel/2019``, the stubfile would look like

.. literalinclude:: ../../example/compiler-etc-dependencies/modulerc4/mvapich/2.3.1/intel/2019
    :language: tcl

So the mvapich directory in ``MODULEPATH`` would have a structure like

.. code-block:: console

    mvapich
    ├── 2.1
    │   ├── gcc
    │   │   ├── 8.2.0
    │   │   ├── 9.1.0
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── intel
    │   │   ├── 2018
    │   │   ├── 2019
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── .modulerc -> symlink to modulerc.select_compiler_family
    │   └── pgi
    │       ├── 18.4
    │       ├── 19.4
    │       └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── 2.3.1
    │   ├── gcc
    │   │   ├── 9.1.0
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── intel
    │   │   ├── 2019
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── .modulerc -> symlink to modulerc.select_compiler_family
    │   └── pgi
    │       ├── 19.4
    │       └── .modulerc -> symlink to modulerc.select_compiler_version
    └── common

With this directory structure, an user can load a compiler and then
module load a specific version of mvapich, and if a build of that version of
mvapich exists for that compiler, the environment will be set up properly,
and otherwise an error reported. However, if the user module loads
mvapich without specifying a version, it will simply default the version
to the latest version of mvapich available, regardless of whether there
is a build of that version of mvapich for the loaded compiler. Ideally,
we would like it so that
if one issues a module load of a package without specifying a version, it
should load the latest version available that is compatible with any loaded
compilers or other modules.

A simple change can enable that. We add a symlink to
``modulerc.select_compiler_family`` as ``.modulerc`` directly under the
mvapich directory, and create an additional subdirectory under the mvapich
directory for each compiler family. We symlink
``modulerc.select_compiler_version`` as ``.modulerc`` under each compiler
family subdirectory, along with a subdirectory for each version of that
compiler family that a version of mvapich is built for. We then symlink
the various stub modulefiles under the ``mvapich/VERSION`` into the
corresponding ``mvapich/COMPILER_FAMILY/COMPILER_VERSION`` directory, with
the symlink's name being the mvapich version number. So the mvapich
directory tree will now look like:

.. code-block:: console

    mvapich
    ├── 2.1
    │   ├── gcc
    │   │   ├── 8.2.0
    │   │   ├── 9.1.0
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── intel
    │   │   ├── 2018
    │   │   ├── 2019
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── .modulerc -> symlink to modulerc.select_compiler_family
    │   └── pgi
    │       ├── 18.4
    │       ├── 19.4
    │       └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── 2.3.1
    │   ├── gcc
    │   │   ├── 9.1.0
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── intel
    │   │   ├── 2019
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── .modulerc -> symlink to modulerc.select_compiler_family
    │   └── pgi
    │       ├── 19.4
    │       └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── common
    ├── gcc
    │   ├── 8.2.0
    │   │   └── 2.1 -> symlink to ../../2.1/gcc/8.2.0
    │   ├── 9.1.0
    │   │   ├── 2.1 -> symlink to ../../2.1/gcc/9.1.0
    │   │   └── 2.3.1 -> symlink to ../../2.3.1/gcc/9.1.0
    │   └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── intel
    │   ├── 2018
    │   │   └── 2.1 -> symlink to ../../2.1/intel/2018
    │   ├── 2019
    │   │   ├── 2.1 -> symlink to ../../2.1/intel/2019
    │   │   └── 2.3.1 -> symlink to ../../2.3.1/intel/2019
    │   └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── .modulerc -> symlink to modulerc.select_compiler_family
    └── pgi
        ├── 18.4
        │   └── 2.1 -> symlink to ../../2.1/pgi/18.4
        ├── 19.4
        │   ├── 2.1 -> symlink to ../../2.1/pgi/19.4
        │   └── 2.3.1 -> symlink to ../../2.3.1/pgi/19.4
        └── .modulerc -> symlink to modulerc.select_compiler_version

When one attempts to module load mvapich without specifying a version,
the ``.modulerc`` file will default to the family of the loaded compiler (or
the default compiler if none loaded). It then descends into the corresponding
compiler family directory, where the ``.modulerc`` there will default to the
version of the loaded or defaulted compiler. After descending into the
compiler version directory, there is no ``.modulerc``, so the standard modulecmd
defaulting mechanism will select the latest version.

We now have two ways to refer to the same build of mvapich, namely
one using the traditional form ::

  PACKAGE/PKGVERSION/COMPILER_FAMILY/COMPILER_VERSION
  (e.g. mvapich/2.3.1/pgi/19.4)

and a new one giving the compiler family and version immediately
after package name, with the version of the package coming last ::

  PACKAGE/COMPILER_FAMILY/COMPILER_VERSION/PKGVERSION
  (e.g. mvapich/pgi/19.4/2.3.1)

These both refer to the same build of the package (e.g. version ``2.3.1``
of mvapich, built for version ``19.4`` of the PGI compiler suite).
Indeed, through the judicious use of symlinks, these actually refer to the
modulefile on disk, but with two distinct names depending on the path
used to get to it. In general, we allow for
modules to be named with multiple dependency packages and/or flags if
doing so would make it easier for a user to default to the correct package.

This is the primary reason for the procedure ``GetPackageFamilyVersion`` ---
it splits a module name into components and takes the first component
as the family name. It then examines the second component, and if it
matches the name of a known package (like a compiler family), it uses
the last component as the version. Otherwise, the second component is
assumed to be the version. The code is as follows:

.. literalinclude:: ../../example/compiler-etc-dependencies/tcllib/GetFamVer.tcl
    :language: tcl
    :lines: 4-

Similarly, there are two corresponding ``.modulerc`` scripts for defaulting
the MPI library: one to default the family (e.g. openmpi, mvapich, intelmpi)
and one to default the version. The family version, shown below:

.. literalinclude:: ../../example/compiler-etc-dependencies/modrc_common/modulerc.select_mpi_family
    :language: tcl

is similar to the corresponding compiler version, but contains additional
logic to handle the intelmpi case. We treat intelmpi specially, because in
our example here we assume the environment for Intel MPI is setup properly
when the user loads the intel module. If no MPI library is explicitly loaded,
but the intel module is loaded, than we look for a subdirectory named
intelmpi (or one of its aliases) and default to it if it exists. Otherwise,
if no MPI module is loaded, it looks for and will default to a directory
or modulefile named nompi if it exists.

The ``modulerc.select_mpi_version`` script is also similar to its compiler
counterpart,

.. literalinclude:: ../../example/compiler-etc-dependencies/modrc_common/modulerc.select_mpi_version
    :language: tcl

It checks if any MPI library was explicitly loaded, and if so it checks
if the family of the loaded MPI module matches the name of the parent directory
of this ``.modulerc`` file. If so, it checks for the existence of a subdirectory
matching the version, and if found it defaults to it. There is no special
handling needed for the ``nompi`` case, as since there is no version attached
to the MPI library in the nompi case, this ``.modulerc`` is not present there.
Similarly, for the case of Intel MPI when the Intel compiler suite is loaded, we
expect the Intel MPI version to be that from the Intel compiler suite, so
no version or this ``.modulerc`` is needed.

As the MPI modules depend themselves on the compiler, it is assumed that
any package depending on the MPI libraries also depend on the compiler,
and so will have a structure like the one described below for foo

.. code-block:: console

    foo
    ├── 1.1
    │   ├── gcc
    │   │   ├── 8.2.0
    │   │   │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │   │   │   ├── mvapich
    │   │   │   │   ├── 2.1
    │   │   │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   │   ├── nompi
    │   │   │   └── openmpi
    │   │   │       ├── 3.1
    │   │   │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── intel
    │   │   ├── 2018
    │   │   │   ├── intelmpi
    │   │   │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │   │   │   ├── mvapich
    │   │   │   │   ├── 2.1
    │   │   │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   │   ├── nompi
    │   │   │   └── openmpi
    │   │   │       ├── 3.1
    │   │   │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── .modulerc -> symlink to modulerc.select_compiler_family
    │   └── pgi
    │       ├── 18.4
    │       │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │       │   ├── mvapich
    │       │   │   ├── 2.1
    │       │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │       │   ├── nompi
    │       │   └── openmpi
    │       │       ├── 3.1
    │       │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │       └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── 2.4
    │   ├── gcc
    │   │   ├── 9.1.0
    │   │   │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │   │   │   ├── mvapich
    │   │   │   │   ├── 2.3.1
    │   │   │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   │   ├── nompi
    │   │   │   └── openmpi
    │   │   │       ├── 4.0
    │   │   │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── intel
    │   │   ├── 2019
    │   │   │   ├── intelmpi
    │   │   │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │   │   │   ├── mvapich
    │   │   │   │   ├── 2.3.1
    │   │   │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   │   ├── nompi
    │   │   │   └── openmpi
    │   │   │       ├── 4.0
    │   │   │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   ├── .modulerc -> symlink to modulerc.select_compiler_family
    │   └── pgi
    │       ├── 19.4
    │       │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │       │   ├── nompi
    │       │   └── openmpi
    │       │       ├── 3.1
    │       │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │       └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── common
    ├── gcc
    │   ├── 8.2.0
    │   │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │   │   ├── mvapich
    │   │   │   ├── 2.1
    │   │   │   │   └── 1.1 -> ../../../../1.1/gcc/8.2.0/mvapich/2.1
    │   │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   ├── nompi
    │   │   │   └── 1.1 -> ../../../1.1/gcc/8.2.0/nompi
    │   │   └── openmpi
    │   │       ├── 3.1
    │   │       │   └── 1.1 -> ../../../../1.1/gcc/8.2.0/openmpi/3.1
    │   │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │   ├── 9.1.0
    │   │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │   │   ├── mvapich
    │   │   │   ├── 2.3.1
    │   │   │   │   └── 2.4 -> ../../../../2.4/gcc/9.1.0/mvapich/2.3.1
    │   │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   ├── nompi
    │   │   │   └── 2.4 -> ../../../2.4/gcc/9.1.0/nompi
    │   │   └── openmpi
    │   │       ├── 4.0
    │   │       │   └── 2.4 -> ../../../../2.4/gcc/9.1.0/openmpi/4.0
    │   │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │   └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── intel
    │   ├── 2018
    │   │   ├── intelmpi
    │   │   │   └── 1.1 -> ../../../1.1/intel/2018/intelmpi
    │   │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │   │   ├── mvapich
    │   │   │   ├── 2.1
    │   │   │   │   └── 1.1 -> ../../../../1.1/intel/2018/mvapich/2.1
    │   │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   ├── nompi
    │   │   │   └── 1.1 -> ../../../1.1/intel/2018/nompi
    │   │   └── openmpi
    │   │       ├── 3.1
    │   │       │   └── 1.1 -> ../../../../1.1/intel/2018/openmpi/3.1
    │   │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │   ├── 2019
    │   │   ├── intelmpi
    │   │   │   └── 2.4 -> ../../../2.4/intel/2019/intelmpi
    │   │   ├── .modulerc -> symlink to modulerc.select_mpi_family
    │   │   ├── mvapich
    │   │   │   ├── 2.3.1
    │   │   │   │   └── 2.4 -> ../../../../2.4/intel/2019/mvapich/2.3.1
    │   │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
    │   │   ├── nompi
    │   │   │   └── 2.4 -> ../../../2.4/intel/2019/nompi
    │   │   └── openmpi
    │   │       ├── 4.0
    │   │       │   └── 2.4 -> ../../../../2.4/intel/2019/openmpi/4.0
    │   │       └── .modulerc -> symlink to modulerc.select_mpi_version
    │   └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── .modulerc -> symlink to modulerc.select_compiler_family
    └── pgi
        ├── 18.4
        │   ├── .modulerc -> symlink to modulerc.select_mpi_family
        │   ├── mvapich
        │   │   ├── 2.1
        │   │   │   └── 1.1 -> ../../../../1.1/pgi/18.4/mvapich/2.1
        │   │   └── .modulerc -> symlink to modulerc.select_mpi_version
        │   ├── nompi
        │   │   └── 1.1 -> ../../../1.1/pgi/18.4/nompi
        │   └── openmpi
        │       ├── 3.1
        │       │   └── 1.1 -> ../../../../1.1/pgi/18.4/openmpi/3.1
        │       └── .modulerc -> symlink to modulerc.select_mpi_version
        ├── 19.4
        │   ├── .modulerc -> symlink to modulerc.select_mpi_family
        │   ├── nompi
        │   │   └── 2.4 -> ../../../2.4/pgi/19.4/nompi
        │   └── openmpi
        │       ├── 3.1
        │       │   └── 2.4 -> ../../../../2.4/pgi/19.4/openmpi/3.1
        │       └── .modulerc -> symlink to modulerc.select_mpi_version
        └── .modulerc -> symlink to modulerc.select_compiler_version

Although the directory tree above is somewhat lengthy, it is similar
to the case for a module depending only on the compiler, but expanded
to handle MPI dependencies as well. There is a subdirectory under ``foo``
for each version of foo. Each of these subdirectories have further subdirectories
for the compiler families for which that version of foo was built, and under
the subdirectories for each compiler family are subdirectories for each version
of that compiler family for which foo was built. Underneath these
are stub modulefiles for nompi (and for intel compilers, intelmpi) builds of
the package, and subdirectories for each MPI family, containing stub modulefiles
for each version of the MPI family the package was built for.

Additionally, there are subdirectories directly under ``foo`` for each compiler
family the package was built for, with each having subdirectories for the
version of the compiler. Beneath each of those are one or two more layers
of subdirectories for the MPI family and version (the version layer is omitted
in the nompi or intelmpi cases), underneath which are symlinks to the corresponding
stub modulefile from the ``foo/FOOVERSION`` path.

By placing the appropriate ``.modulerc`` files (actually, symlinks to the correct
modulerc file), when the user enters a partial modulename the modulecmd will
descend this directory tree based on the previously loaded compiler and MPI
library to get to correct build of the package, if it exists. If no MPI library
was previously loaded, it will search for an intelmpi build if an intel compiler
was loaded, or a nompi build if no intelmpi build found or a non-intel compiler
was used. If only MPI builds were made, it will default (using standard
module defaulting rules) to one of the MPI builds, and the appropriate MPI
library will be loaded when the foo module gets loaded (via the ``LoadedMPIMatches``
Tcl procedure described below). If a compiler and MPI library were loaded but no
build for that combination exists, again a modulefile will be defaulted to
(using standard module defaulting rules), but an error will be generated by
the one of the ``LoadedCompilerMatches`` or ``LoadedMPIMatches`` calls in the
foo modulefile (see below).

The common code of the modulefile is fairly standard, as shown below

.. literalinclude:: ../../example/compiler-etc-dependencies/modulerc4/foo/common
    :language: tcl

The main difference from a standard modulefile is the inclusion of the
invocations of ``LoadedCompilerMatches`` and ``LoadedMpiMatches``. This
is needed to ensure that the compiler and MPI for the modulefile being
processed are compatible with any which are loaded. The ``LoadedMpiMatches``
procedure is defined by:

.. literalinclude:: ../../example/compiler-etc-dependencies/tcllib/LoadedMpiMatches.tcl
    :language: tcl
    :lines: 4-

Basically, it determines what if any MPI library was previously module loaded.
If one was loaded, it ensures the family and version match what was expected,
and if not exit with an error. The use of the ``GetPackageFamilyVersion``
procedure is important, because as discussed previously packages can be
represented by more than one name (e.g. the modulefile for version ``4.0`` of
openmpi for ``gcc/9.1.0`` could be named ``openmpi/4.0/gcc/9.1.0`` or
``openmpi/gcc/9.1.0/4.0``) and we need to ensure either is recognized.

Although we only showed cases for compilers and MPI libraries, the techniques
above can be adapted to other cases as well. For simple cases, where everything
falls under a single package name, one can use ``GetTagOfModuleLoaded`` to
determine the version of the package loaded in the ``.modulerc`` and in the
modulefile to ensure the version matches.

For bar, we added variants on CPU vectorization support. Rather than add
a simd modulefile, we just add variants to the bar modules. So the bar
modulefile tree would look like:

.. code-block:: console

    bar
    ├── 4.7
    │   ├── gcc
    │   │   ├── 8.2.0
    │   │   │   ├── avx
    │   │   │   ├── .modulerc -> symlink to modulerc.default_lowest_simd
    │   │   │   └── sse4.1
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   └── .modulerc -> symlink to modulerc.select_compiler_family
    ├── 5.4
    │   ├── gcc
    │   │   ├── 9.1.0
    │   │   │   ├── avx
    │   │   │   └── avx2
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   └── .modulerc -> symlink to modulerc.select_compiler_family
    ├── avx
    │   ├── gcc
    │   │   ├── 8.2.0
    │   │   │   └── 4.7 -> ../../../4.7/gcc/8.2.0/avx
    │   │   ├── 9.1.0
    │   │   │   └── 5.4 -> ../../../5.4/gcc/9.1.0/avx
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   └── .modulerc -> symlink to modulerc.select_compiler_family
    ├── avx2
    │   ├── gcc
    │   │   ├── 9.1.0
    │   │   │   └── 5.4 -> ../../../5.4/gcc/9.1.0/avx2
    │   │   └── .modulerc -> symlink to modulerc.select_compiler_version
    │   └── .modulerc -> symlink to modulerc.select_compiler_family
    ├── common
    ├── gcc
    │   ├── 8.2.0
    │   │   ├── avx
    │   │   │   └── 4.7 -> ../../../4.7/gcc/8.2.0/avx
    │   │   ├── .modulerc -> symlink to modulerc.default_lowest_simd
    │   │   └── sse4.1
    │   │       └── 4.7 -> ../../../4.7/gcc/8.2.0/sse4.1
    │   ├── 9.1.0
    │   │   ├── avx
    │   │   │   └── 5.4 -> ../../../5.4/gcc/9.1.0/avx
    │   │   ├── avx2
    │   │   │   └── 5.4 -> ../../../5.4/gcc/9.1.0/avx2
    │   │   └── .modulerc -> symlink to modulerc.default_lowest_simd
    │   └── .modulerc -> symlink to modulerc.select_compiler_version
    ├── .modulerc -> symlink to modulerc.select_compiler_family
    └── sse4.1
        ├── gcc
        │   ├── 8.2.0
        │   │   └── 4.7 -> ../../../4.7/gcc/8.2.0/sse4.1
        │   └── .modulerc -> symlink to modulerc.select_compiler_version
        └── .modulerc -> symlink to modulerc.select_compiler_family

Here we added yet another alternate set of module names. So the module
for bar version ``5.4`` built with gcc version ``9.1.0`` and ``avx2`` support can be
called:

*  ``bar/5.4/gcc/9.1.0/avx2``
*  ``bar/gcc/9.1.0/avx2/5.4``
*  ``bar/avx2/gcc/9.1.0/5.4``

Due to this multiplicity of names, if an user does a module load bar, the
``.modulerc`` immediately under bar will cause modulecmd to default along the
path for the loaded (or defaulted) compiler, and then the
``modulerc.default_lowest_simd`` will default to the lowest simd level.
If the user module loads bar/VERSION, it will find the build for that version
of bar with the appropriate compiler (and again, default to the lowest simd
level). And if the user module loads ``bar/avx`` or another simd level, then
it will load the latest version of bar built for the loaded compiler and
simd specified.

The ``modulerc.default_lowest_simd`` script looks like:

.. literalinclude:: ../../example/compiler-etc-dependencies/modrc_common/modulerc.default_lowest_simd
    :language: tcl

It will default to the lowest SIMD level, but could easily be adapted to
do something else.

Examples
^^^^^^^^

We now look at the example modulefiles for the *Modulerc-based* strategy.
As noted previously, the best choice of whether to set the ``requireIt`` flag
to ``LoadedCompilerMatches`` and ``LoadedMPIMatches`` (instructing them to
to a ``prereq`` on the requesting compiler/MPI module) depends on whether
one is using a 3.x or 4.x version of Environment Modules. Due to this,
we provide two modulefile trees, one for 3.x and one for 4.x; they are
basically identical except for that matter. This leads to slightly
different instructions on how to use the examples, depending on which
version of Environment Modules is being used, namely:

#. Set (and export) ``MOD_GIT_ROOTDIR`` to where you git-cloned the modules source
#. Do a ``module purge``
#. If using Environment Modules 3.x, set your ``MODULEPATH`` to ``$MOD_GIT_ROOTDIR/doc/example/compiler-etc-dependencies/modulerc3``
#. If using Environment Modules 4.x, set your ``MODULEPATH`` to ``$MOD_GIT_ROOTDIR/doc/example/compiler-etc-dependencies/modulerc4``

As with the previous cases, we start with a ``module avail`` command, and here
we see the first big difference:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/modavail.out
    :language: console

Unlike the previous cases wherein only package names and versions were shown
(because a single modulefile handled all the builds for that package and
version), here we see a listing for every build of a package and version.
Indeed, we not only see one such listing, but multiple listings per
build in many cases (e.g. ``openmpi/3.1/intel/2018`` and
``openmpi/intel/2018/3.1``).

While admittedly the output of a ``module avail`` command without
specifying any package is rather overwhelming, when a package is specified
the output tends to be more reasonable, informing one of which builds
of the package are available. This strategy deliberately opts for the
presentation of more rather than less information.

The standard functionality of selecting the correct build of a package
based on the loaded compiler, e.g.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/ompi-loads1.out
    :language: console

works as expected (and fails with the expected error when one attempts to
load a module not compatible with the loaded compiler).
The only significant difference between the previously
examined strategies is that the module list command provides information
about what variant of each package is loaded.

The ``module switch`` command, however, does not work as well as one would
like. While it indeed switches the specified module, it does not
successfully reload the modules which depend on the replaced module, even
with the :ref:`Automated module handling <v42-automated-module-handling-mode>`
feature enabled. As currently implemented,
the automated module handling feature attempts to reload dependent modules
using the fully qualified module name, and as in this strategy the fully
qualified modulename includes the information about the module that was switched
out (e.g. ``pgi/19.4`` in the example below), it will be incompatible with the
replacement module (e.g. ``intel/2019`` in example below). It
is hoped a future version of modulecmd will allow for reloading based on the
name specified when the module was loaded. But as things currently stand,
the automatic module handling will throw an error attempting to reload the
depend module, resulting in the the dependent modules being unloaded.

Without
automated module handling mode (i.e. for older Environment Modules or without
the --auto flag), the dependent modules remain loaded and there is inconsistency
in the loaded modules. But at least module list clearly shows such.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/ompi-switch.out
    :language: console

The defaulting of modules is more successful, however, as seen below:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/ompi-defaults.out
    :language: console

Here it not only defaults to the default compiler, but if one tries to load
openmpi without specifying a version, it will find and load the latest version
compatible with the loaded compiler.

The situation is similar for foo:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/foo-loads.out
    :language: console

As expected, the correct version of foo is loaded depending on the previously
loaded compiler and MPI libraries. Note however that there is no dummy
module for Intel MPI, and that when the intel compiler is loaded but no MPI
library is explicitly loaded, module load foo/2.4 gets the Intel MPI build.
Such only occurs if there is an Intel MPI build in the tree of modulefiles;
otherwise a nompi build will be loaded if present. If both versions are
present, as in this case, one needs to give the explicit
``foo/2.4/intel/2019/nompi`` specification.

Again, the ``module switch`` command only succeeds in switching the
specified module, and does not successfully reload the modules depending
on the switched module. With automated module handling mode, there will be errors
reported and the dependent modules will be unloaded; without it the dependent
modules will not get unloaded, and there will be inconsistent dependencies
(but at least module list will show such).

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/foo-switch.out
    :language: console

The defaulting of modules works relatively well, as shown below:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/foo-defaults.out
    :language: console

If one attempts to load foo without specifying a version or having previously loaded a compiler
module, the compiler will be defaulted (to ``gcc/8.2.0`` compiler, as that is what
we declared via the ``GetDefaultCompiler`` Tcl procedure), and the latest
version of foo compatible with that compiler (and no MPI) will be loaded.

The situation with bar is similar. We do not have a dummy simd module,
so the builds with different CPU vectorization support are specified by
appending ``/avx``, etc. to the bar package name. As with previous strategies,
if one attempts to load a simd variant which was not built for the compiler
loaded, an error will occur.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/bar-loads.out
    :language: console

Summary of Modulerc-based strategy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* The modulefiles are fairly standard. The only extra logic needed
  is to ensure that any loaded compiler/MPI or other dependency matches
  what the modulefile expects, and that can be handled fairly easily
  with the addition of one or two calls to Tcl procedures.
* The ``.modulerc`` files do the work of routing partial specifications to
  the correct modulefile, and follow a fairly standard pattern. For the
  most part, these are generic/independent of the package, and typically
  can be written once and symlinked to the appropriate places in the
  directory tree.
* This strategy involves the use of many more modulefiles than the
  previously examined strategies, having at least one modulefile per
  build, and in some cases multiple per build. The output of the module
  avail command without any module specified is rather overwhelming,
  although when a module is specified, module avail will provide information
  about what builds are available.
* Because each build has a specific module, the module list command shows
  exactly what builds of the various modules are loaded.
* The module switch command does not work very well. In
  particular, when one switches out a module on which other modules depend,
  the dependent modules are not successfully reloaded. Even with the
  :ref:`Automated module handling <v42-automated-module-handling-mode>`
  feature (introduced in ``4.2``), the dependent
  modules are unloaded but do not get reloaded properly since currently
  this feature attempts to reload based on the fully qualified name of the
  loaded module (which includes a dependency on the module switched out),
  not the specification used when the module was loaded.
* In general, when the user tries to load a module based on a partially
  specified modulename, the ``.modulerc`` files handle it well, and the
  latest version of the module consistent with the specification and
  previously loaded modules will be used. If no compiler was previously
  loaded, will use the default compiler, or the latest compiler if nothing
  matches the default compiler.
* Modules will fail with an error message if user tries to load a package
  which was not built for the values of the dependency packages loaded.


.. _Modulepath-based_strategy:

Modulepath-based Strategy
-------------------------

This strategy makes use of the ability of modules to support multiple
directories in the ``MODULEPATH`` environment variable. Every time a module is loaded
on which other modules might depend, a new path is added to ``MODULEPATH``
containing the modulefiles which depend on the newly added module.

This is basically similar to the strategy that is used in the hierarchical
modulefile approach of Lmod_.

.. _Lmod: https://github.com/TACC/Lmod

Implementation
^^^^^^^^^^^^^^

The :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` and
:ref:`Modulerc-based <Modulerc-based_strategy>` strategies were based on
modifying the modulefiles of modules which depended on other modules.
The *Modulepath-based* strategy, in contrast, is based on modifying the
modulefiles for modules which other modules might depend on.

To begin with, we set ``MODULEPATH`` to a ``Core`` directory containing
modulefiles for modules which do not depend on any other modules.
The modulefiles for compilers will typically be put in here.
For this example, we opt not to use a dummy simd module (for reasons
explained later), but instead add ``avx``, ``avx2``, ``sse4.1`` variants to the
bar modules, but otherwise they might belong here. Other modules which
do not depend on compilers, etc. could be place in here as well, e.g.
applications which do not expose libraries, etc. So the directory
structure would look like:

.. code-block:: console

    Core
    ├── gcc
    │   ├── 8.2.0
    │   ├── 9.1.0
    │   └── common
    ├── intel
    │   ├── 2018
    │   ├── 2019
    │   └── common
    └── pgi
        ├── 18.4
        ├── 19.4
        └── common

A typical common file for the gcc compiler would be something like

.. literalinclude:: ../../example/compiler-etc-dependencies/modulepath/Core/gcc/common
    :language: tcl

The most interesting aspect is the ``module use`` at the end. We add
to ``MODULEPATH`` a directory under ``Compiler`` for this specific compiler
and version. Other compilers/versions would add their own specific path
to ``MODULEPATH``.

In each of these compiler specific directories, modulefile directory
trees exist for the supported MPI libraries and bar. We also include
a foo directory containing the non-MPI variants of foo. It would look
like:

.. code-block:: console

    Compiler
    ├── gcc
    │   ├── 8.2.0
    │   │   ├── bar
    │   │   │   ├── 4.7
    │   │   │   │   ├── avx
    │   │   │   │   ├── .modulerc -> symlink to modulerc.default_lowest_simd
    │   │   │   │   └── sse4.1
    │   │   │   └── common -> symlink to bar/common
    │   │   ├── foo
    │   │   │   ├── 1.1
    │   │   │   └── common -> symlink to foo/common
    │   │   ├── mvapich
    │   │   │   ├── 2.1
    │   │   │   └── common -> symlink to mvapich/common
    │   │   └── openmpi
    │   │       ├── 3.1
    │   │       └── common -> symlink to openmpi/common
    │   └── 9.1.0
    │       ├── bar
    │       │   ├── 5.4
    │       │   │   ├── avx
    │       │   │   ├── avx2
    │       │   │   └── .modulerc -> symlink to modulerc.default_lowest_simd
    │       │   └── common -> symlink to bar/common
    │       ├── foo
    │       │   ├── 2.4
    │       │   └── common -> symlink to foo/common
    │       ├── mvapich
    │       │   ├── 2.1
    │       │   ├── 2.3.1
    │       │   └── common -> symlink to mvapich/common
    │       └── openmpi
    │           ├── 3.1
    │           ├── 4.0
    │           └── common -> symlink to openmpi/common
    ├── intel
    │   ├── 2018
    │   │   └── ... modules depending on compiler=intel/2018
    │   └── 2019
    │       └── ... modules depending on compiler=intel/2019
    └── pgi
        ├── 18.4
        │   └── ... modules depending on compiler=pgi/18.4
        └── 19.4
            └── ... modules depending on compiler=pgi/18.4

For brevity, we only show the directory structure for the modules depending on
the two versions of gcc in detail; the other compilers will have similar
structures.

Basically, there is a separate modulepath for each compiler, containing only the
modulefiles depending on that specific build of the compiler (and not also depending
on something else, like MPI library).
This certainly enforces the consistency of loaded modules; one could not load
a specific version of gcc (say ``gcc/9.1.0``) and an incompatible version of foo (e.g. ``foo/1.1``),
because all of the foo modulefiles are in compiler specific module trees and there is
no ``foo/1.1`` in the ``gcc/9.1.0`` module tree. Conflict statements in the compiler modulefiles
will prevent one from loading multiple compilers, thereby preventing multiple compiler
specific modulepaths (unless the user explicitly does a ``module use`` or similar, and
there is only so far one can go in preventing users from shooting themselves in the foot).

The modulefiles for foo and bar are fairly straightforward; we have a common file which
does the heavy lifting (and since this can be made independent of version and compiler, this
is actually a symlink to a package specific common file in a ``common`` directory external
to the modulepath trees).
We then add some small stubfiles which set variables for the specific build information, which
mostly are determined by their position in the tree structure (e.g. the stubfiles under
``Compiler/gcc/9.1.0`` are all going to set ``compilerTag`` to ``gcc/9.1.0``, etc), and
then invoke the common file.

The modulefiles for openmpi are largely similar. E.g., for ``gcc/9.1.0`` we have a small
stubfile like the following (for version ``4.0``)

.. literalinclude:: ../../example/compiler-etc-dependencies/modulepath/Compiler/gcc/9.1.0/openmpi/4.0
    :language: tcl

which defines the version and compilerTag variables for the OpenMPI version and
compiler version, and then invokes the common script

.. literalinclude:: ../../example/compiler-etc-dependencies/modulepath/Compiler/gcc/9.1.0/openmpi/common
    :language: tcl

which does the usual stuff (define a help function, whatis string, and sets assorted environment
variables like ``PATH``, ``LIBRARY_PATH``, etc. for using OpenMPI), and then adds another directory
to the ``MODULEPATH``. This new directory, under ``CompilerMPI`` is for modulefiles which
depend on the compiler AND the MPI library. (It is assumed that all modulefiles depending on
MPI libraries also depend on the compiler). This new branch in the modulepath would have
a structure like

.. code-block:: console

    CompilerMPI
    ├── gcc
    │   ├── 8.2.0
    │   │   ├── mvapich
    │   │   │   └── 2.1
    │   │   │       └── foo
    │   │   │           ├── 1.1
    │   │   │           └── common -> symlink to foo/common
    │   │   └── openmpi
    │   │       └── 3.1
    │   │           └── foo
    │   │               ├── 1.1
    │   │               └── common -> symlink to foo/common
    │   └── 9.1.0
    │       ├── mvapich
    │       │   └── 2.3.1
    │       │       └── ... modules depending on gcc/9.1.0 and mvapich/2.3.1
    │       └── openmpi
    │           └── 4.0
    │               └── ... modules depending on gcc/9.1.0 and openmpi/4.0
    ├── intel
    │   ├── 2018
    │   │   ├── intelmpi
    │   │   │   └── default
    │   │   │       └── ... modules depending on intel/2018 and its included MPI lib
    │   │   ├── mvapich
    │   │   │   └── 2.1
    │   │   │       └── ... modules depending on intel/2018 and mvapich/2.1
    │   │   └── openmpi
    │   │       └── 3.1
    │   │           └── ... modules depending on intel/2018 and openmpi/3.1
    │   └── 2019
    │       ├── intelmpi
    │       │   └── default
    │       │       └── ... modules depending on intel/2019 and its included MPI lib
    │       ├── mvapich
    │       │   └── 2.3.1
    │       │       └── ... modules depending on intel/2019 and mvapich/2.3.1
    │       └── openmpi
    │           └── 4.0
    │               └── ... modules depending on intel/2019 and openmpi/4.0
    └── pgi
        ├── 18.4
        │   ├── mvapich
        │   │   └── 2.1
        │   │       └── ... modules depending on pgi/18.4 and mvapich/2.1
        │   └── openmpi
        │       └── 3.1
        │           └── ... modules depending on pgi/18.4 and openmpi/3.1
        └── 19.4
            └── openmpi
                ├── 3.1
                │   └── ... modules depending on pgi/19.4 and openmpi/3.1
                └── 4.0
                    └── ... modules depending on pgi/19.4 and openmpi/3.1

Basically, there is a separate modulepath for each combination of compiler and
MPI library, listing all modulefiles depending on that compiler and MPI library.
The modulefiles underneath each directory simply do what is needed to load that
version of the package built with the specified compiler and MPI library.

This process could be continued further, as needed. E.g., if you had packages which
depended on NetCDF, and you had multiple builds of NetCDF for a given compiler/MPI
combination, you could add another modulepath tree, e.g. ``CompilerMPINetCDF``,
branching on each compiler, MPI, and NetCDF triplet. But things can also get
unwieldy if carried too far.

One could also add modulepath trees for disjoint features like SIMD level,
but this turns out to be a bit tricky. E.g., if you had a simd branch as well
as a compiler branch, with both simd and compiler appearing in Core, things are
fine for the Simd and Compiler trees. However, if there were to be modules depending
on both, e.g. a CompilerSimd branch, then because compiler and simd are disjoint
and could be loaded in either order, the modulefiles for both would need to handle
adding the CompilerSimd branch depending on whether the other was previously loaded.

Examples
^^^^^^^^

We now look at the example usage for the *Modulepath-based* strategy.
To use these examples, you must:

#. Set (and export) ``MOD_GIT_ROOTDIR`` to where you git-cloned the modules source
#. Do a ``module purge``
#. Set your ``MODULEPATH`` to ``$MOD_GIT_ROOTDIR/doc/example/compiler-etc-dependencies/modulepath/Core``

Note that we set the ``MODULEPATH`` to the Core subdirectory; the Core branch is for those modulefiles
that do not depend on compiler or MPI library, and that should be available from the start.

As with the previous cases, we start with a ``module avail`` command, and here
we see the first big difference:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/modavail.out
    :language: console

When we first do a ``module avail``, we only see the modulefiles for the compilers.
The MPI libraries, foo, bar, etc. all depend on at least the compiler, and so
will not become "available" until a compiler (and possibly MPI library as well) is loaded.
In a production environment there would likely be other modulefiles available in Core
(i.e. an application which is only used as a standalone application and does not
provide an API/libraries to link against would likely be placed in Core), but in our
simple example, only the compilers appear in Core. As we load a compiler and then an
MPI library, additional modulefiles appear available. In a production environment, one
might wish to set things up so that a compiler (and maybe MPI library) modulefile is
automatically loaded when the user logs in.

The downside of this is that it becomes difficult for an user to know what software
is available, at least via the module command. I.e., if application foobar is only built
for a single compiler/MPI combination, it will not show up in module avail unless that
specific compiler/MPI combination were previously loaded. Lmod_ adds a ``module spider``
command which allows the user to list all packages installed for any compiler/MPI
combination, and can be used to also find out which compiler/MPI combinations are needed
to access a specific modulefile, but Environment Modules does not have a comparable function
at this time. If you were to use this strategy in a production environment, you would
likely need to generate lists of available packages (and their compiler/MPI/etc dependencies)
in a web page or other documentation area which can be frequently updated.

The standard functionality of selecting the correct build of a package
based on the loaded compiler, e.g.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/ompi-loads1.out
    :language: console

works as expected. As shown in the last command above, when requesting a module not
built for the loaded compiler (e.g. ``openmpi/4.0`` when ``gcc/8.2.0`` is loaded), the module command
simply returns ``Unable to locate a modulefile`` as there is no corresponding modulefile
in the current list of MODULEPATHS.

The functionality of the ``module switch`` command depends on version and/or settings
of Environment Modules. For version 3.x, or 4.x with the :ref:`Automated module handling <v42-automated-module-handling-mode>`
feature disabled, it does not work well, modules which depend on the switched out module
are not reloaded, leading to inconsistent environments. But for 4.x versions with
the automated module handling feature enabled (as indicated by the --auto after the
switch, although that can be made the default), it works as expected. Modules depending
on the module being switched out get unloaded and reloaded if possible, as shown below:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/ompi-switch.out
    :language: console

The modulefiles themselves basically have no support for defaulting a compiler; the modulefiles for
the various MPI libraries are simply not even available until a modulefile for a compiler is loaded, as
seen below:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/ompi-defaults.out
    :language: console

However, this could be somewhat mitigated by having the modulefile for the default compiler automatically
loaded for the user upon login (e.g. in their dot files).

The situation is similar for foo:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/foo-loads.out
    :language: console

As expected, the correct version of foo is loaded depending on the previously
loaded compiler and MPI libraries. Here again we use an ``intelmpi`` modulefile
to indicate when we wish to use Intel MPI libraries, even though they are enabled
by the ``intel`` module. The ``intelmpi`` modulefile basically just adds the modulepath
for the intel-compiler and intelmpi dependent modules.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/foo-switch.out
    :language: console

Note again the same deficiency in the switch report as the :ref:`Homebrewed_flavors_strategy`
had; in the last case above wherein we loaded foo with ``intel/2019`` loaded
but no MPI module. As expected, a non-MPI build of foo was loaded. However, when an
openmpi module is subsequently loaded, foo does not get reloaded and we still have
the non-MPI build of foo, as evidenced by the output of the foo command. And that
this fact is not obvious from the module list output.

The behavior when defaulting is nicer. Without any compiler or MPI libraries
Here we note a deficiency in the switch support as compared to :ref:`Flavours <Flavours_strategy>`. In the last example
after loading ``intel/2019`` and foo, we have the non-MPI build of foo as expected. However, upon
subsequently loading the openmpi module, we still have the non-MPI version of foo loaded, as evidenced
by the output of the dummy foo command. I.e., the foo package was *not* automatically reloaded, as
there was no prereq in the foo modulefile on an MPI library (as in the non-MPI build there is no MPI
library to prereq). Also note that module list does not really inform one of this fact.

The ability to default partial modulenames in this strategy is mixed. Without any compiler loaded,
most modulefiles are not even visible/available. However, once a compiler is loaded, trying to
load a module without specifying the version will end up loading the latest version compatible
with the loaded compiler (as no incompatible versions are visible) as seen below:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/foo-defaults.out
    :language: console

This is better than in the :ref:`Flavours <Flavours_strategy>` or :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` strategies. If one were to use
this strategy in production, it is recommended to have a default compiler module (and maybe even
an MPI library, etc) automatically loaded when the user logs in, thereby allowing for a reasonable defaulting
ability (and more reasonable module avail output).

The situation with bar is similar. We do not have a dummy simd module,
so the builds with different CPU vectorization support are specified by
appending ``/avx``, etc. to the bar package name. As with previous strategies,
if one attempts to load a simd variant which was not built for the compiler
loaded, an error will occur.

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/bar-loads.out
    :language: console

Defaulting is handled well, as shown

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/bar-defaults.out
    :language: console

In particular, one case specify ``bar/avx2`` or ``bar/avx`` or ``bar/sse4.1`` and the
latest version of bar consistent with the specification and any previously
loaded compiler (or default compiler) will be loaded

Summary of Modulepath-based strategy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* It is quite difficult for an user to get an inconsistent environment,
  at least when automated module handling mode is used. The modulepaths are
  managed so that only modulefiles consistent with the previously loaded
  compiler/MPI library are visible.
* With automated module handling mode, the module switch command is very nicely
  supported.
* The module command does not lend itself to readily seeing all the packages
  which are installed on the system, nor seeing for which compiler/MPI combinations
  a certain package is built. The module avail command just does not show
  any modules not compatible with the currently loaded compiler and/or MPI
  libraries. Lmod needed to add a ``module spider`` command to address this,
  but no such functionality currently exists in Environment Modules.
  If one were to use this in production, you would need to provide something
  similar to the Lmod spider sub-command, or at least provide frequently
  updated web pages or similar with this information.
* This strategy involves the use of many more modulepaths than the
  previously examined strategies, having at least one modulepath per
  compiler installed, and typically also one for each compiler/MPI library
  combination. If you add additional layers of dependency, things get even
  more complicated, which can be limiting. It is especially problematic if
  you have two disjoint dependencies -- i.e. neither dependencies is dependent on
  the other; in this case you might load one or the other, or both (in either
  order). If there are modulefiles dependent on both of these dependencies, the
  last one loaded (and *only* the last one) needs to handle adding the appropriate
  modulepath for the modulefiles depending on both.
* In general, a module will fail to load with an error message
  if any dependent module is not already loaded. The error will actually
  state that the module cannot be found, as the modulepath containing it will
  not be added to the ``MODULEPATH`` until the modules depended on are loaded.
  So if you need to default a compiler, etc., you will need to have the
  user's initialization scripts automatically load the appropriate
  modulefile upon login.
* However, what we have been referring to as "more intelligent defaulting" is
  effectively supported. I.e., if a user requests to load a package without
  specifying the version desired, the version will be defaulted to the latest
  version *compatible* with the currently loaded compiler and other dependencies.
  This is because the incompatible versions are not visible in the module tree.


Comparison of Strategies
------------------------

All of the strategies discussed above have their peculiar strengths and weaknesses.
The decision of which strategy to use will depend on how these strengths and weaknesses
impact your design goals. It is advised to play around in the sandbox environments
a bit, as actual use tends to help make clear which downsides you are willing to accept
and which you are not.

With the exception of the :ref:`Flavours <Flavours_strategy>`, which works best with Environment Modules
version 3.x, all of the other strategies work as well or better
on the newer 4.x versions of Environment Modules. This difference is most visible
in the discussion of features around the module switch command. So in the following
discussions we will assume Environment Modules version 3.x for the Flavours strategies,
as that is the version it works best with. For all other strategies, we assume
Environment Modules version 4.x, with automated module handling mode enabled, as these
strategies work best in that scenario.

The :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>`,
:ref:`Modulerc-based <Modulerc-based_strategy>`, and :ref:`Modulepath-based <Modulepath-based_strategy>` strategies all
require a significant amount of "assembly" in order to get them working for a
production environment; the example scripts and procedures provided here should
help significantly, but should not be considered as a polished product. You will
likely need to customize and extend for your environment. The
:ref:`Flavours_strategy`, on the other hand, has been packaged to present as a finished product
and so requires less "assembly" to get working, but does not appear to be actively
maintained, so the reduced up-front work might be neutralized by the costs of
self support.

Basic Dependency Handling
^^^^^^^^^^^^^^^^^^^^^^^^^

All of the strategies discussed support a basic level of dependency
handling. If a user attempts to load a package, they get the build of the
package appropriate for the previously loaded compiler or other dependencies,
or, if no appropriate build is found, an error message indicating such.
E.g, if the user does:

.. code-block:: tcl

   module load gcc/8.2.0
   module load foo/1.1

their environment will be set up to run foo version ``1.1`` built
with gcc version ``8.2.0``.

All of the strategies discussed meet this criterion, with both 3.x and 4.x versions
of Environment Modules.

Advanced Dependency Handling (e.g. the module switch sub-command)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Things are more complicated when we allow for the modules upon which
other loaded modules might depend to be changed. This generally involves
the module switch command.

While all of the strategies handle well the easy case, when we switch
a module upon which no other loaded modules depend, the trick comes
when one or more currently loaded modules depend on the module being
switched out. E.g., if we assume that module foo depends on the
previously loaded compiler, then ideally, we would like for a sequence like:

.. code-block:: tcl

   module load intel/2019
   module load foo
   module switch intel pgi/18.4

to have the same effective outcome as if the user issued the commands:

.. code-block:: tcl

   module load pgi/18.4
   module load foo

I.e., switching out the compiler
will cause foo to be reloaded with the new compiler dependency. And ideally
in this example we would have ``pgi/18.4`` loaded along with the latest version
of foo compatible with that compiler.

This is handled reasonably well with the :ref:`Flavours <Flavours_strategy>` (using Environment Modules
3.x) and the :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` and
:ref:`Modulepath-based <Modulepath-based_strategy>` (both using Environment
Modules 4.x) strategies. For the :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` and
:ref:`Modulepath-based <Modulepath-based_strategy>`
strategies, this relies on the :ref:`Automated module handling <v42-automated-module-handling-mode>` feature (As of
version ``4``, this is disabled by default. It can be enabled in ``modulecmd.tcl``,
or by running ``module config auto_handling 1``, or by adding the ``--auto`` flag
to the modules command.)

For these strategies, swapping out a compiler or other module upon which a loaded
module depends will cause the the module with a dependency to be unloaded, the
module being switched being swapped out, and the module that had a dependency being
reloaded.

Our example sequence above actually exposes a subtle issue. For the :ref:`Flavours <Flavours_strategy>`
and :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` strategies, the sequence of loading ``pgi/18.4`` followed
by foo (using our example software library) would fail, as foo would be defaulted to
version ``2.4`` and as there was no build of ``foo/2.4`` for ``pgi/18.4``, the module load foo
would fail. And indeed, the switch sequence above would not be completely successful
under either strategy: for the :ref:`Homebrewed_flavors_strategy`, the compiler would be
swapped out and the reload of foo would fail with an error (so effectively
equivalent to the second sequence). For the :ref:`Flavours_strategy`, it detects that
it would be unable to reload foo, and the switch command fails (i.e. the compiler
remains ``intel/2019``).

For the :ref:`Modulepath-based_strategy`, the second sequence (loading ``pgi/18.4`` then foo) would
actually work, as the modulepath exposed by loading ``pgi/18.4`` only has versions of
foo that are compatible with ``pgi/18.4``. But the switch command does not work
as well as would be desired. This is because that as currently implemented, when the
automated module handling code does the reload of the module that was unloaded
due to dependencies, it attempts to reload based on the fully qualified name of the
module that was loaded, and not based on the (possibly partial) name specified
when the module was originally loaded. So, after the module load ``intel/2019``,
the module load foo will result in ``foo/2.4`` being loaded. When we switch out ``intel/2019``
for ``pgi/18.4``, foo gets unloaded, but after the compiler swap, it tries to reload
``foo/2.4``. As there is no build of ``foo/2.4`` for ``pgi/18.4`` (and thus no modulefile in
the current modulepath), the module is not found and is unable to be loaded.
As in the :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` case, the compiler is switched but foo is left unloaded
(with an appropriate warning to that effect).

This same issue prevents the :ref:`Automated module handling <v42-automated-module-handling-mode>` feature as currently implemented
from being very useful with the :ref:`Modulerc-based <Modulerc-based_strategy>`. Basically, whenever you have a module
loaded that depends on another module, switching out the module depended upon will
fail to reload the other module. This is true even in less pathological cases, like:

.. code-block:: tcl

   module load intel/2019
   module load foo
   module switch intel pgi/19.4

which succeeds under the other three strategies (as ``foo/2.4`` *is* built for ``pgi/19.4``).
This is because when foo is initially loaded in the above scenario, the actual modulename
loaded is ``foo/intel/2019/2.4``. With the switch command, foo gets unloaded, the compilers
are switched, and the automated modules handling code tries to reload ``foo/intel/2019/2.4``.
That modulename clearly depends on ``intel/2019``, not ``pgi/19.4``, and so will fail to load
(with an error message). The net result is that the compiler gets switched, but any
modules depending on the compiler (or whatever module being switched) get unloaded
(with a warning to that effect). It is hoped that a future version of the
:ref:`Automated module handling <v42-automated-module-handling-mode>` feature will have an option to reload the dependent
modules using the modulename they were loaded with; this should allow for the
switch command to work well in the :ref:`Modulerc-based_strategy`, as well as make
edges case (like the ``pgi/18.4`` example discussed above) work better with the
:ref:`Modulepath-based_strategy`.

There is another edge case which only the :ref:`Flavours_strategy` handles well. Consider a
scenario like:

.. code-block:: tcl

   module purge
   module load intel/2019
   module load foo
   foo
   # this should be foo/2.4 built for intel/2019 without MPI
   module load openmpi
   foo
   # What build is this, with or without MPI?

With the :ref:`Flavours_strategy`, the final foo will be built for ``intel/2019``
with openmpi support; i.e. Flavours supports the concept of *optional* prerequisites, and foo has
an optional prerequisite on the MPI libraries, so when openmpi is loaded, any previously loaded modules
which depend on it (through a regular or optional prerequisite) get reloaded.

All of the other strategies rely on automated module handling mode for automatic reloads, and
in this case no reload of foo will be initiated as foo does not have
MPI libraries listed as a prerequisite (otherwise a no MPI version could not be loaded). This is especially
problematic for the :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` and
:ref:`Modulepath-based <Modulepath-based_strategy>` strategies, since it is not easily to
tell from a module list or similar command which version of foo the environment is set for (for the
:ref:`Modulerc-based_strategy`, the full modulename from module list will indicate that).

However, despite some potential issues with some edge cases, the :ref:`Flavours <Flavours_strategy>`,
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>`, and
:ref:`Modulepath-based <Modulepath-based_strategy>` strategies all handle this challenge well.
The :ref:`Modulerc-based <Modulerc-based_strategy>`
approach has a poor showing (although at least the modules depending on the module switched
out will be unloaded, so the set of loaded modules is consistent).

Note also that this topic shows the most dependence on the version of Environment
Modules. For the :ref:`Flavours_strategy`, the switching of a module upon which
other modules depended does not work (the wrapper script returned weird errors). The
other strategies all rely on the :ref:`Automated module handling <v42-automated-module-handling-mode>` feature to enable the unload
and reload of modules which depend on the module being switched out. Thus for 3.x versions
of Environment Modules, the switch of a compiler, etc. will only result in the compiler being replaced, and any
modules which depend on the compiler will not be reloaded. This means they will still be
pointing to versions built for the old compiler, leading to an inconsistent set of
modules being loaded. This is particularly bad in the
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` case, as a module
load will not even inform one of that fact. Even if automated module handling mode is disabled
on version 4.x of Environment Modules situation is better as loaded environment consistency
:ref:`is assured<v42-prereq-constraints-consistency>`: the switch of a compiler will be denied
since a dependent module is detected loaded.

Visibility into what packages are available
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Another set of criteria to weigh has to do with visibility into the available
packages. We are interested in

#. Viewing all of the packages installed on a system
#. Determining what versions of a specific package are available for a
   given compiler/MPI/etc combination.
#. Seeing what compiler/MPI/etc combinations a specific version of a
   package
#. For packages that we have currently loaded, determining which compiler/MPI/etc
   they were built for.

In terms of seeing all of packages that are available, the :ref:`Flavours <Flavours_strategy>` and
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` strategies
are probably the best. The ``module avail`` command will list all modulefiles available, which will
typically just be a list of packagename/version for each installed version. On a production system
with many packages installed, even this can be a bit overwhelming to the user.

The ``module avail`` command will also list all modulefiles for every installed package in the
:ref:`Modulerc-based_strategy`, but here every build of every version of every package will have a distinct modulefile. I.e.,
you will not just have ``foo/1.1`` and ``foo/2.4`` listings, but ``foo/1.1/gcc/8.2.0/nompi``, ``foo/1.1/gcc/8.2.0/mvapich/2.1``,
``foo/1.1/gcc/8.2.0/openmpi/3.1``, ``foo/1.1/intel/2018/nompi``, ``foo/1.1/intel/2018/intelmpi``, etc. modulefiles.
Actually, the situation is even worse than that, as in this strategy there are often multiple
modulefiles for the same build in order to ease navigation of the module tree, e.g. we could have
``foo/1.1/gcc/8.2.0/nompi`` and ``foo/gcc/8.2.0/nompi/1.1`` representing the same build of the same package.
Whereas an unqualified ``module avail`` in the :ref:`Flavours <Flavours_strategy>` or
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` strategies can be a bit overwhelming
in a large environment, with the :ref:`Modulerc-based_strategy` it can become practically unusable.

While an unqualified ``module avail`` in the :ref:`Flavours <Flavours_strategy>`,
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>`, and especially :ref:`Modulerc-based <Modulerc-based_strategy>` strategies
can inundate the user with modulenames, the :ref:`Modulepath-based_strategy` has the opposite problem.
With the :ref:`Modulepath-based_strategy` strategy, the modulefiles are split across multiple, often mutually incompatible, modulepaths, so the module avail
command will never return a list of all modulefiles installed, only those available given the previously loaded
compiler/MPI libraries/etc. E.g., if a package foobar is only installed for a particular compiler/MPI combination,
it will not appear in any module avail listing unless that particular compiler and MPI were previously loaded.

While the number of modulefiles listed in an unqualified ``module avail`` command in the :ref:`Modulerc-based_strategy`
is unwieldy, if one adds a partial package specification argument, the number of modulefiles returned is greatly
reduced. This smaller list can provide information about which compilers/MPI/etc. are compatible with
a specific version of a package. For example, to see the compilers for which ``foo/2.4`` is built, we can
do something like:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/foo-avail1.out
    :language: console

Similarly, to see the builds of foo using gcc compilers, one can do something like:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulerc/modules4/foo-avail2.out
    :language: console

The other strategies do not readily provide that information. The next best case is the
:ref:`Modulepath-based_strategy`, because here at least a module avail will tell you what versions
of a package are compatible with the currently loaded compiler/MPI/etc, e.g.:

.. literalinclude:: ../../example/compiler-etc-dependencies/example-sessions/modulepath/modules4/foo-avail1.out
    :language: console

But the :ref:`Flavours <Flavours_strategy>` and :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>`
strategies do not readily show what versions of packages are built
for which compilers, etc. You would need to load a compiler/MPI/etc combination, and then try loading
a particular version of a package.

Lastly, with the :ref:`Modulerc-based_strategy`, the module list command explicitly shows information
about the compiler/MPI/etc used to build the loaded modules, as that is part of the full modulename.
This information is not explicitly visible in the other three strategies, but that is usually not
a problem. As long as the various user commands result in a set of modules wherein every module depending
on either a compiler or MPI library, etc. is built for the currently loaded compiler/MPI library/etc.,
that information is redundant.

The only cases where this is potentially an issue are the sort of edge cases described in the previous
section, e.g. if one were to do something like:

.. code-block:: tcl

   module purge
   module load intel/2019
   module load foo
   foo
   # this should be foo/2.4 built for intel/2019 without MPI
   module load openmpi
   foo
   # What build is this, with or without MPI?

In the above example, for the :ref:`Flavours <Flavours_strategy>`,
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>`, and :ref:`Modulepath-based <Modulepath-based_strategy>` strategies a
module list at the end would simply list something like:

.. code-block:: shell

   Currently Loaded Modulefiles:
   1) intel/2019   2) foo/2.4   3) openmpi/4.0

For the :ref:`Flavours_strategy`, foo would be built with MPI support, but for the other
two, foo would still be the non-MPI build, which is not readily apparent from the
above output (although possibly could be inferred by the ordering of the modules).

Ease of Navigating the Module Tree
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The final criterion to be discussed is the ease of navigating the module tree.
Ideally, we would like it so that when an user gives a partial modulename the
system resolves in a sensible manner. Typically a partial modulename will be
a package name without a version, in which case ideally the latest version of
the package (compatible with previously loaded compiler, etc), should be loaded.
And indeed, for a simple case like:

.. code-block:: tcl

   module purge
   module load intel
   module load foo

all the strategies do just that. The intel module will default to ``intel/2019``,
and foo to version ``2.4`` built for ``intel/2019``. However, with even a slight modification
things begin to diverge, e.g.:

.. code-block:: tcl

   module purge
   module load intel/2018
   module load foo

In this second case, the :ref:`Flavours <Flavours_strategy>` and
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` strategies still default
the version of foo to version ``2.4``, despite there being no build of ``foo/2.4`` for ``intel/2018``.
That is because they do not do intelligent defaulting, and will simply default
the version of foo to the highest version they see
even though it is not compatible with the loaded compiler.
Thus the load of foo will fail for :ref:`Flavours <Flavours_strategy>` and
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>` strategies.
However, for the :ref:`Modulerc-based <Modulerc-based_strategy>` and
:ref:`Modulepath-based <Modulepath-based_strategy>` strategies, the module load of foo
will successfully load foo version ``1.1`` built for ``intel/2018``. For the :ref:`Modulepath-based_strategy`,
this is because the only foo modules currently visible are compatible with the
loaded ``intel/2018`` compiler, and it simply defaults to the highest version seen. For
the :ref:`Modulerc-based_strategy`, the ``.modulerc`` file directly under foo directs it down the
subdirectory for the intel compiler family, which has a ``.modulerc`` defaulting to the 2018 subdirectory
(as the currently loaded compiler version), which has a ``.modulerc`` defaulting to the nompi
subdirectory (the MPI family as no MPI library was loaded). At this point, there is not
another ``.modulerc`` file, so the latest version will be defaulted to, and
``foo/intel/2018/nompi/1.1`` module will be loaded.

The situation is analogous when there are chained dependencies (e.g. compiler and MPI
libraries); the :ref:`Flavours <Flavours_strategy>` and :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>`
strategies will default to the latest
version of the package they know about, regardless of whether it is compatible with the
loaded environment. If it is compatible, it will be loaded and all is well, if not the
load command will fail with an error message. Not horrible; the set of loaded modules is
still consistent, but not terribly user-friendly either.

For the :ref:`Modulerc-based <Modulerc-based_strategy>` and :ref:`Modulepath-based <Modulepath-based_strategy>`
strategies, the latest build compatible with the various dependencies will be loaded (or if no build is
compatible, the load will fail with an appropriate error).

For the :ref:`Flavours <Flavours_strategy>`, :ref:`Homebrewed flavors <Homebrewed_flavors_strategy>`,
and :ref:`Modulepath-based <Modulepath-based_strategy>` strategies, typically modulenames
only consist of the package name and version components, so generally only partial modulenames
seen are packages without a version. The :ref:`Modulerc-based_strategy` has much longer modulenames,
and therefore a greater variety of partial modulenames, which can allow for interesting
possibilities. For example, one could do ``module load foo/pgi``, which (assuming no compiler
was previously loaded), would load the latest version of foo built with the latest version of
the pgi compiler. The module command will start looking for a modulefile in the
``foo/pgi`` subdirectory of the foo module structure,
where the ``.modulerc`` file will attempt to default to the version of pgi which was loaded, but as
no compiler was loaded, the modulerc file will terminate without setting a default, and the
modulecmd will default to the latest version, and descend into that directory (``19.4``). Here,
the ``.modulerc`` will attempt to default to the MPI family, but since none is loaded (as that
would require a compiler to have been loaded), it defaults to the nompi directory, and then
to the latest version in the nompi subdirectory. So the module ``foo/pgi/19.4/nompi/2.4`` will
get loaded, which will also cause ``pgi/19.4`` to get loaded. Should we have issued that module
load after having loaded a non-pgi compiler, the same module file would get loaded, but it
would detect an incompatible previously loaded compiler and abort with an error. Should a
pgi compiler have been previously loaded, we would have gotten the latest version of foo built
for that compiler (and possibly with any previously loaded MPI library).

The above is supported because we create a ``PACKAGE/COMPILER_FAMILY/COMPILER_VERSION/.../PKGVERSION``
tree in the package's modulefile directory for packages depending on the compiler. Possibly
with additional components to the modulename for MPI libraries, etc. These are created
to allow for the more basic ``module load foo`` case, but because the module path is
descended component by component, it works if you jump in in the middle as well
(and will fail if you try something stupid like :

.. code-block:: tcl

   module load intel/2019
   module load foo/pgi

But a more interesting case is when there are somewhat disjoint options on the builds.
Consider the case of the bar package from the examples, which in addition to the compiler
and builds supporting different SIMD levels. For the :ref:`Flavours <Flavours_strategy>` and
:ref:`Homebrewed flavors <Homebrewed_flavors_strategy>`
strategies, we handled this with a pseudo-simd package and modules. This worked, and allowed
for one to select a build that matched both compiler and SIMD level, but you likely need to
manually select a bar version compatible with both of these, as these strategies will not
do intelligent defaulting of versions.

For the :ref:`Modulepath-based_strategy`, we added the SIMD label as a final component to the
modulename, but the defaulting was somewhat simple (e.g. always default to lowest SIMD).

With the :ref:`Modulerc-based_strategy`, we added an additional naming scheme for the bar modulefiles,
so they can start with bar/BARVERSION, or bar/COMPILER_FAMILY, or bar/SIMD_LEVEL. This not
only makes it easier to view which versions of bar for which compilers are built with support
for a given SIMD level (e.g. module avail bar/avx2), but also one can do something like:

.. code-block:: tcl

   module load pgi/19.4
   module load bar/avx

to load the latest version of bar built for ``pgi/19.4`` with AVX support. While there is
overhead associated with adding a new naming scheme like that (you add another set of
modulefiles to bar and a new subdirectory tree), this can be done on a per package
basis and when done judiciously it can be convenient for the users.


Conclusions
-----------

We have presented four strategies for dealing with modulefiles for packages
with multiple builds depending on compiler, MPI, and/or other factors. All
four strategies can deal with the basic requirement of loading of the correct
build of a package depending on the previously loaded dependencies, or failing
with a reasonable error message if no such build is available. They all have
their own strengths and weaknesses. This document tried to present these
strategies objectively and has been made to help you to evaluate how to handle
this issue at your site.

.. vim:set tabstop=2 shiftwidth=2 expandtab autoindent:
