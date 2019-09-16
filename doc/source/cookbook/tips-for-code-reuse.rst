.. _tips-for-code-reuse:

Tips for Code Reuse in Modulefiles
==================================

Although Modules allow one to manage many packages and versions of packages
on a system, managing the modulefiles themselves can become error prone, 
especially since the differences between modulefiles for different versions
of the same package are often rather small.  This can lead to issues with
the consistency of the module definitions.

E.g., let us say you have a package foo, version 1.0 with a corresponding
modulefile.  A bit later, foo version 2.0 comes out, and you add a modulefile
for that, likely copying the 1.0 version and editing a few lines.  Then a
bug is discovered in the version 1.0 modulefile, e.g. an environment variable
that would be useful in some cases was omitted, or a typo in the help 
function.  The version 1.0 modulefile is fixed, but did you remember to
fix the 2.0 modulefile which, since it was created by copying the version 1.0
modulefile, likely has the same error?

This page includes some tips to reduce the amount of redundant code
in your modulefiles (i.e. increase the amount of code reuse) so that 
they are easier to manage and more consistent.

A Simple Example
----------------

We return to our "foo" example.  
A fairly traditional set up would have a "foo" directory somewhere under your MODULEPATH with 
modulefiles 1.0 and 2.0 underneath it.  Each of 1.0 and 2.0 will likely be largely the same, having
basically the same help text, whatis, and variables set, with only some
changes to some text and paths to reflect the version difference.  Example
code for this and other cases discussed in this document can be found in the
"doc/example/tips-for-code-reuse" subdirectory of this package.  The modulefiles
for this simple example start in the "foo" subdirectory.  For example, the
version 1.0 modulefile might look something like

.. include:: ../../example/tips-for-code-reuse/foo/1.0 
    :literal:

Although the above is simple enough, there is still much redundancy between
the version 1.0 and 2.0 files, and we do not bother showing the file for 2.0
because it is so similar (only 5 chars change between the 1.0 and 2.0
versions, basically all the "1.0" strings become "2.0").  The command
"module avail foo" shows versions 1.0 and 2.0, and loading either will
set the environmental variables PATH, MANPATH, and LD_LIBRARY_PATH appropriately,
conflict with itself (so only one version of foo can be loaded at a time),
and define the help and whatis texts.

To refactor this to increase code reuse, we copy one of the original 
modulefiles to "common", and change all the references to the version number
to a "$version" variable. To avoid confusion with the previous version,
we change the package name from "foo" to "fooA", and the new versions can be
found in the "fooA" subdirectory of "doc/example/tips-for-code-reuse".  After 
adding some comments, etc, the common module becomes

.. include:: ../../example/tips-for-code-reuse/fooA/common 
    :literal:

We then create a pair of small stub files to replace "1.0" and "2.0".  These
simply just set the version variable appropriately, and then "source" the
"common" file.  For "1.0", the file would look like

.. include:: ../../example/tips-for-code-reuse/fooA/1.0 
    :literal:

A similar file would be created for "2.0", just changing the version number 
in the "set version" line.
The "set moduledir" line sets the Tcl variable moduledir to contain the name of
the directory in which the final modulefile (i.e. "1.0") is located, so we
can find the "common" file.  

The common file then handles all the work of defining the PATH, MANPATH,
and LD_LIBRARY_PATH environmental variables, using the value of the Tcl
variable "version" passed to it by the stub file.  It also handles defining
the help procedure and the whatis text, and the conflict with itself.

Note that the "common" file does **NOT** start 
with the Modules magic "#%Module1.0"; this will keep "common" from showing 
up in "module avail fooA".  The "1.0" and "2.0" files do need to start with
the magic "#%Module1.0" tag so the module command will "see" them.  Because
of this, the "module avail fooA" command will just show the 1.0 and 2.0
versions as expected.

The files for this example are in the "doc/example/tips-for-code-reuse/fooA"
directory.

A Simple Example, revisited
---------------------------

The "fooA" case above is fairly common, and we can actually improve upon
what we did above.  To avoid confusion, we will repeat using the package
name "fooB", and the files for this example will be in the 
"doc/example/tips-for-code-reuse/fooB" directory.  

Using introspection in the modulefiles, we can get the version
number of fooB from the name of the modulefile.  So we add code to the 
top of "common" to default the version variable from the modulename.

We then copy the "1.0" stub modulefile to ".generic", and remove the line
which sets the version.  Because is starts with a leading ".", the generic
modulefile will not be displayed in "module avail" and such.  Since this 
generic modulefile is now version independent, we can replace "1.0" and "2.0" 
with symlinks to the .generic file.  

The new common file looks like

.. include:: ../../example/tips-for-code-reuse/fooB/common
    :literal:

The .generic stub file looks like

.. include:: ../../example/tips-for-code-reuse/fooB/1.0
    :literal:

Again, the common file handles setting the environmental variables
PATH, MANPATH, and LD_LIBRARY_PATH, using the Tcl variable version
which was defaulted from modulename.  This works even in the case
where the user does not specify the full module path, and the modulecmd
defaults the version (e.g. if the user types "module load fooB" without
specifying the version of fooB.)  The common file also handles defining
the help procedure and whatis text.  When the command "module avail fooB" is issued,
the common file does not get listed (because it does not start with the magic
"#%Module1.0" tag), nor does the .generic file get listed (because it starts
with a period (".")), but the two 1.0 and 2.0 symlinks do get listed, just
as one wants.

This replaces a file for each version of fooB with one common file doing
all the work, one generic file, and a bunch of symlinks.  And the .generic
file is not even specific to fooB.  Indeed, at our site, we put the 
generic modulefile in an "utilities" directory outside of the MODULEPATH,
and just symlink it to ".generic" in each application directory, and then
symlink the versions to this symlinked ".generic".  So we end up with one
"common" file for each application, one shared generic modulefile, and a 
bunch of symlinks.

A More Complicated Example
---------------------------

Although the simple cases like the "foo" example above are not uncommon,
many packages are more complicated.  We now consider the fictitious "bar"
package (example modulefiles in "example/tips-for-code-reuse/bar" directory).
This is admittedly a contrived example, but it displays some of the ways in
which we can modify the above simple cases to handle the more complicated
needs of some packages.  Our "bar" package has the follow characteristics:

  * We have two versions of "bar" built, "1.0" and "2.0"
  * For each bar version, we have builds for three different threading models: nothreads, pthreads, and openmp.
  * We expect users to use commands specifying both the bar version and the threading model, e.g. "module load fooB/1.0/pthreads" or "module load fooB/2.0/openmp"
  * The path to the installation directories include both the version number of "bar" and the threading model
  * The package requires the environmental variable BAR_LICENSE_FILE to be set appropriately
  * All builds of bar version 1.0 use one license file which is different than that used by bar version2.0
  * Bar 2.0 nothreads and openmp builds use the same license file, with a completely different name than that used by bar version 1.0.
  * The bar 2.0 pthreads build uses its very own license file with a very different filename than those used by bar version 1.0 or the other threading models of version 2.0.
  * The "nothreads" builds have a prerequisite on "fooB", with bar 1.0 wanting fooB version 1.1, and bar 2.0 wanting fooB version 3.2

Even with the above exceptions, there are still more similarities than 
differences between the various modulefile definitions.  Like in the "fooA"
case, we will have a "common" script that does almost all the work, and stub
files for each of the variants, but in this case the stub files are expected
to define some more variables.  Our "common" file, which can be found in 
the "example/tips-for-code-reuse/bar" directory, looks like
name "fooB", and the files for this example will be in the 

.. include:: ../../example/tips-for-code-reuse/bar/common
    :literal:

Here we have extended the parameters which are allowed to be passed in from
the build specific "stub" files; in addition to the version of "bar", the stub
files are expected to have set the variables "threadingmodel" and optionally
"licensefile".  If "licensefile" was not set, the common script will default
it based on the version of "bar".  The rest of the "common" file is similar
to the "fooA" case, except that we use both "version" and "threadingmodel"
in setting the paths, and now define BAR_LICENSE_FILE based on "licensefile".
Again, almost all the work is done in the common file, including setting the
relevant environmental variables, defining the help procedure and whatis text,
and preventing multiple versions of bar being loaded at the same time.

We now create stubfiles for each of the three threading models underneath directories
for each bar version.  These can all be found under the
"example/tips-for-code-reuse/bar" directory.  The one for bar 1.0 and openmp
looks like


.. include:: ../../example/tips-for-code-reuse/bar/1.0/openmp
    :literal:

The pthreads version is almost identical, just setting threadingmodel to
"pthreads" instead of "openmp".  The "openmp" build for bar version 2.0
is also very similar, differing only in value "version" is set to.  In
all three cases, these stubfiles are basically the same as those used in
the "fooA" example; we set a couple of Tcl variables (version and threadingmodel), 
and then source the common file which does all the work.  The licensefile
variable is left unset so the common file will default it correctly based
on the bar version.

The stub file for the pthreads build for bar version 2.0 is as below:

.. include:: ../../example/tips-for-code-reuse/bar/2.0/pthreads
    :literal:

This version is similar to the previous three, with the addition of explicitly
setting the "licensefile" variable.  This value will be used in the common file
to set BAR_LICENSE_FILE, rather than the default which would have been set in
the common file had no value been provided.

Finally, the two "nothreads" build also have a slightly different stub file than
the other builds.  The one for bar version 2.0 is shown below:

.. include:: ../../example/tips-for-code-reuse/bar/2.0/nothreads
    :literal:

Here we add a "prereq fooB/3.2" to require that the correct version of fooB
was loaded.  The bar version 1.0 stubfile is almost the same, just changing
the setting of the version variable from 2.0 to 1.0, and changing the version
of fooB needed as a prerequisite.

So instead of having six somewhat lengthy modulefiles (~40 lines each), 
one for each combination of bar version and threading model, we put almost
all the work into a single, slightly longer (55 lines) common file, with
six small (8 lines or less) stub files which mostly just set variables
to define the behavior of the common file.  

When customization is needed for a specific build, we showed two ways of
accommodating such.  First, for simple, more-or-less "one-off" cases, 
you can add some code to the 
appropriate "stubfiles" to handle the added complexity, like was done
with the "prereq fooB" for the "nothreads" builds in the examples.

Alternatively, one can add some additional logic to the common script
to accommodate the complexity, like was done with the license file
differences between builds.  This is the recommended way to handle
complexity which is likely to recur in multiple places.  Generally,
this is done by:

  * adding a parameter to be passed from the stub file to the common file and setting appropriately in the stub files (e.g. licensefile in bar/2.0/pthreads)

  * adding logic to the common file to default it appropriately (e.g. licensefile for the other builds)

The above was only a simple example of what could be done, but it shows how
modulefiles can leverage the fact that the modulefiles are evaluated in a 
full programming language to increase the amount of code re-use, which should
reduce errors, improve consistency, and in general make things more manageable
over the long run.

The tips given above should work with both 3.x and 4.x versions of the Tcl
based environmental packages.  They have been explicitly tested on versions
3.2.10 and 4.3.0.

