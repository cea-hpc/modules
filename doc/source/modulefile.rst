.. _modulefile(4):

modulefile
==========


DESCRIPTION
-----------

*modulefiles* are written in the Tool Command Language, **Tcl**\ (n) and are
interpreted by the **modulecmd.tcl** program via the :ref:`module(1)` user
interface. *modulefiles* can be loaded, unloaded, or switched on-the-fly
while the user is working; and can be used to implement site policies
regarding the access and use of applications.

A *modulefile* begins with the magic cookie, '#%Module'. A version number may
be placed after this string. The version number is useful as the *modulefile*
format may change thus it reflects the minimum version of **modulecmd.tcl**
required to interpret the modulefile. If a version number doesn't exist, then
**modulecmd.tcl** will assume the *modulefile* is compatible. Files without
the magic cookie or with a version number greater than the current version of
**modulecmd.tcl** will not be interpreted.

Each *modulefile* contains the changes to a user's environment needed to
access an application. Tcl is a simple programming language which permits
*modulefiles* to be arbitrarily complex, depending upon the application's
and the *modulefile* writer's needs. If support for extended tcl (tclX)
has been configured for your installation of the Modules package, you may
use all the extended commands provided by tclX, too.

A typical *modulefile* is a simple bit of code that set or add entries
to the **PATH**, **MANPATH**, or other environment variables. A Modulefile is
evaluated against current **modulecmd.tcl**'s mode which leads to specific
evaluation results. For instance if the *modulefile* sets a value to an
environment variable, this variable is set when modulefile is loaded and unset
when modulefile is unloaded.

Tcl has conditional statements that are evaluated when the *modulefile* is
interpreted. This is very effective for managing path or environment changes
due to different OS releases or architectures. The user environment
information is encapsulated into a single *modulefile* kept in a central
location. The same *modulefile* is used by every user on any machine. So,
from the user's perspective, starting an application is exactly the same
irrespective of the machine or platform they are on.

*modulefiles* also hide the notion of different types of shells. From the
user's perspective, changing the environment for one shell looks exactly
the same as changing the environment for another shell. This is useful
for new or novice users and eliminates the need for statements such as "if
you're using the C Shell do this ..., otherwise if you're using the Bourne
shell do this ...". Announcing and accessing new software is uniform and
independent of the user's shell. From the *modulefile* writer's perspective,
this means one set of information will take care of every type of shell.


Modules Specific Tcl Commands
-----------------------------

The Modules Package uses commands which are extensions to the "standard"
Tool Command Language **Tcl**\ (n) package. Unless otherwise specified, the
Module commands return the empty string. Some commands behave differently
when a *modulefile* is loaded or unloaded. The command descriptions assume
the *modulefile* is being loaded.

**break**

 This is not a Modules-specific command, it's actually part of Tcl, which
 has been overloaded similar to the **continue** and **exit** commands
 to have the effect of causing the module not to be listed as loaded and
 not affect other modules being loaded concurrently. All non-environment
 commands within the module will be performed up to this point and processing
 will continue on to the next module on the command line. The **break**
 command will only have this effect if not used within a Tcl loop though.

 An example: Suppose that a full selection of *modulefiles* are needed for
 various different architectures, but some of the *modulefiles* are not
 needed and the user should be alerted. Having the unnecessary *modulefile*
 be a link to the following notavail *modulefile* will perform the task
 as required.

 .. code-block:: tcl

      #%Module1.0
      ## notavail modulefile
      ##
      proc ModulesHelp { } {
          puts stderr "This module does nothing but alert the user"
          puts stderr "that the [module-info name] module is not available"
      }

      module-whatis "Notifies user that module is not available."
      set curMod [module-info name]
      if { [ module-info mode load ] } {
          puts stderr "Note: '$curMod' is not available for [uname sysname]."
      }
      break

**chdir** directory

 Set the current working directory to *directory*.

**continue**

 This is not a modules specific command but another overloaded Tcl command
 and is similar to the **break** or **exit** commands except the module
 will be listed as loaded as well as performing any environment or Tcl
 commands up to this point and then continuing on to the next module on
 the command line. The **continue** command will only have this effect if
 not used within a Tcl loop though.

**exit** [N]

 This is not a modules specific command but another overloaded Tcl command
 and is similar to the **break** or **continue** commands. However,
 this command will cause the immediate cessation of this module and any
 additional ones on the command line. This module and the subsequent
 modules will not be listed as loaded. No environment commands will be
 performed in the current module.

**setenv** variable value

 Set environment *variable* to *value*. The **setenv** command will also
 change the process' environment. A reference using Tcl's env associative
 array will reference changes made with the **setenv** command. Changes
 made using Tcl's *env* associative array will **NOT** change the user's
 environment *variable* like the **setenv** command. An environment change
 made this way will only affect the module parsing process. The **setenv**
 command is also useful for changing the environment prior to the **exec**
 or **system** command. When a *modulefile* is unloaded, **setenv** becomes
 **unsetenv**. If the environment *variable* had been defined it will
 be overwritten while loading the *modulefile*. A subsequent **unload**
 will unset the environment *variable* - the previous value cannot be
 restored! (Unless you handle it explicitly ... see below.)

**unsetenv** variable [value]

 Unsets environment *variable*. However, if there is an optional *value*,
 then when unloading a module, it will set *variable* to *value*. The
 **unsetenv** command changes the process' environment like **setenv**.

**getenv** variable [value]

 Returns value of environment *variable*. If *variable* is not defined *value*
 is returned if set *_UNDEFINED_* is returned elsewhere. **getenv** command
 should be preferred over Tcl global variable **env** to query environment
 variables.

**append-path** [-d C|--delim C|--delim=C] [--duplicates] variable value...

 See **prepend-path**.

**prepend-path** [-d C|--delim C|--delim=C] [--duplicates] variable value...

 Append or prepend *value* to environment *variable*. The
 *variable* is a colon, or *delimiter*, separated list such as
 ``PATH=directory:directory:directory``. The default delimiter is a colon
 ':', but an arbitrary one can be given by the *--delim* option. For
 example a space can be used instead (which will need to be handled in
 the Tcl specially by enclosing it in " " or { }). A space, however,
 can not be specified by the *--delim=C* form.

 A reference counter environment variable is also set to increase the
 number of times *value* has been added to environment *variable*. This
 reference counter environment variable is named by suffixing *variable*
 by *_modshare*.

 When *value* is already defined in environement *variable*, it is not added
 again except if *--duplicates* option is set.

 If the *variable* is not set, it is created. When a *modulefile* is
 unloaded, **append-path** and **prepend-path** become **remove-path**.

 If *value* corresponds to the concatenation of multiple elements separated by
 colon, or *delimiter*, character, each element is treated separately.

**remove-path** [-d C|--delim C|--delim=C] [--index] variable value...

 Remove *value* from the colon, or *delimiter*, separated list in
 *variable*. See **prepend-path** or **append-path** for further explanation
 of using an arbitrary delimiter. Every string between colons, or delimiters,
 in *variable* is compared to *value*. If the two match, *value* is removed
 from *variable* if its reference counter is equal to 1 or unknown.

 When *--index* option is set, *value* refers to an index in *variable* list.
 The string element pointed by this index is set for removal.

 Reference counter of *value* in *variable* denotes the number of times
 *value* has been added to *variable*. This information is stored in
 environment *variable_modshare*. When attempting to remove *value* from
 *variable*, relative reference counter is checked and *value* is removed
 only if counter is equal to 1 or not defined. Elsewhere *value* is kept
 in *variable* and reference counter is decreased by 1.

 If *value* corresponds to the concatenation of multiple elements separated by
 colon, or *delimiter*, character, each element is treated separately.

**prereq** modulefile...

 See **conflict**.

**conflict** modulefile...

 **prereq** and **conflict** control whether or not the *modulefile* will
 be loaded. The **prereq** command lists *modulefiles* which must have been
 previously loaded before the current *modulefile* will be loaded. Similarly,
 the **conflict** command lists *modulefiles* which **conflict** with the
 current *modulefile*. If a list contains more than one *modulefile*, then
 each member of the list acts as a Boolean OR operation. Multiple **prereq**
 and **conflict** commands may be used to create a Boolean AND operation. If
 one of the requirements have not been satisfied, an error is reported
 and the current *modulefile* makes no changes to the user's environment.

 If an argument for **prereq** is a directory and any *modulefile* from
 the directory has been loaded, then the prerequisite is met. For example,
 specifying X11 as a **prereq** means that any version of X11, X11/R4 or
 X11/R5, must be loaded before proceeding.

 If an argument for **conflict** is a directory and any other *modulefile*
 from that directory has been loaded, then a conflict will occur. For
 example, specifying X11 as a **conflict** will stop X11/R4 and X11/R5
 from being loaded at the same time.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**is-loaded** [modulefile...]

 The **is-loaded** command returns a true value if any of the listed
 *modulefiles* has been loaded or if any *modulefile* is loaded in case no
 argument is provided. If a list contains more than one *modulefile*, then
 each member acts as a boolean OR operation. If an argument for **is-loaded**
 is a directory and any *modulefile* from the directory has been loaded
 **is-loaded** would return a true value.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**is-saved** [collection...]

 The **is-saved** command returns a true value if any of the listed
 *collections* exists or if any *collection* exists in case no argument is
 provided. If a list contains more than one *collection*, then each member
 acts as a boolean OR operation.

 If **MODULES_COLLECTION_TARGET** is set, a suffix equivalent to the value
 of this variable is appended to the passed *collection* name. In case no
 *collection* argument is provided, a true value will only be returned if
 a collection matching currently set target exists.

**is-used** [directory...]

 The **is-used** command returns a true value if any of the listed
 *directories* has been enabled in **MODULEPATH** or if any *directory* is
 enabled in case no argument is provided. If a list contains more than one
 *directory*, then each member acts as a boolean OR operation.

**is-avail** modulefile...

 The **is-avail** command returns a true value if any of the listed
 *modulefiles* exists in enabled **MODULEPATH**. If a list contains more than
 one *modulefile*, then each member acts as a boolean OR operation. If an
 argument for **is-avail** is a directory and a *modulefile* exists in the
 directory **is-avail** would return a true value.

 The parameter *modulefile* may also be a symbolic modulefile name or a
 modulefile alias. It may also leverage a specific syntax to finely select
 module version (see `Advanced module version specifiers`_ section below).

**module** [sub-command] [sub-command-args]

 Contains the same *sub-commands* as described in the :ref:`module(1)`
 man page in the Module Sub-Commands section. This command permits a
 *modulefile* to **load** or **unload** other *modulefiles*. No checks are
 made to ensure that the *modulefile* does not try to load itself. Often
 it is useful to have a single *modulefile* that performs a number of
 **module load** commands. For example, if every user on the system
 requires a basic set of applications loaded, then a core *modulefile*
 would contain the necessary **module load** commands.

 Command line switches **--auto**, **--no-auto** and **--force** are ignored
 when passed to a **module** command set in a *modulefile*.

**module-info** option [info-args]

 Provide information about the **modulecmd.tcl** program's state. Some of the
 information is specific to the internals of **modulecmd.tcl**. *option*
 is the type of information to be provided, and *info-args* are any
 arguments needed.

 **module-info type**

  Returns either "C" or "Tcl" to indicate which **module** command is being
  executed, either the "C" version or the Tcl-only version, to allow the
  *modulefile* writer to handle any differences between the two.

 **module-info mode** [modetype]

  Returns the current **modulecmd.tcl**'s mode as a string if no *modetype*
  is given.

  Returns 1 if **modulecmd.tcl**'s mode is *modetype*. *modetype* can be:
  load, unload, remove, switch, display, help, test or whatis.

 **module-info command** [commandname]

  Returns the currently running **modulecmd.tcl**'s command as a string
  if no *commandname* is given.

  Returns 1 if **modulecmd.tcl**'s command is *commandname*. *commandname*
  can be: load, unload, reload, source, switch, display, avail, aliases,
  list, whatis, search, purge, restore, help or test.

 **module-info name**

  Return the name of the *modulefile*. This is not the full pathname for
  *modulefile*. See the Modules Variables section for information on the
  full pathname.

 **module-info specified**

  Return the name of the *modulefile* specified on the command line.

 **module-info shell** [shellname]

  Return the current shell under which **modulecmd.tcl** was invoked if
  no *shellname* is given. The current shell is the first parameter of
  **modulecmd.tcl**, which is normally hidden by the **module** alias.

  If a *shellname* is given, returns 1 if **modulecmd.tcl**'s current shell
  is *shellname*, returns 0 elsewhere. *shellname* can be: sh, bash, ksh,
  zsh, csh, tcsh, fish, tcl, perl, python, ruby, lisp, cmake, r.

 **module-info shelltype** [shelltypename]

  Return the family of the shell under which *modulefile* was invoked if no
  *shelltypename* is given. As of **module-info shell** this depends on the
  first parameter of **modulecmd.tcl**. The output reflects a shell type
  determining the shell syntax of the commands produced by **modulecmd.tcl**.

  If a *shelltypename* is given, returns 1 if **modulecmd.tcl**'s current
  shell type is *shelltypename*, returns 0 elsewhere. *shelltypename*
  can be: sh, csh, fish, tcl, perl, python, ruby, lisp, cmake, r.

 **module-info alias** name

  Returns the full *modulefile* name to which the *modulefile* alias *name*
  is assigned

 **module-info version** modulefile

  Returns the physical module name and version of the passed symbolic
  version *modulefile*.  The parameter *modulefile* might either be a full
  qualified *modulefile* with name and version, another symbolic *modulefile*
  name or a *modulefile* alias.

 **module-info symbols** modulefile

  Returns a list of all symbolic versions assigned to the passed
  *modulefile*.  The parameter *modulefile* might either be a full qualified
  *modulefile* with name and version, another symbolic *modulefile* name
  or a *modulefile* alias.

 **module-info loaded** modulefile

  Returns the names of currently loaded modules matching passed *modulefile*.
  The parameter *modulefile* might either be a fully qualified *modulefile*
  with name and version or just a directory which in case all loaded
  *modulefiles* from the directory will be returned. The parameter
  *modulefile* may also be a symbolic modulefile name or a modulefile alias.

**module-version** modulefile version-name...

 Assigns the symbolic *version-name* to the *modulefile*. This command
 should be placed in one of the **modulecmd.tcl** rc files in order to
 provide shorthand invocations of frequently used *modulefile* names.

 The special *version-name* default specifies the default version to be
 used for module commands, if no specific version is given. This replaces
 the definitions made in the *.version* file in former **modulecmd.tcl**
 releases.

 The parameter *modulefile* may be either

 * a fully or partially qualified *modulefile* with name / version. If
   name is '.' then the current directory name is assumed to be the module
   name. (Use this for deep *modulefile* directories.)

 * a symbolic *modulefile* name

 * another *modulefile* alias

**module-alias** name modulefile

 Assigns the *modulefile* to the alias *name*. This command should be
 placed in one of the **modulecmd.tcl** rc files in order to provide
 shorthand invocations of frequently used *modulefile* names.

 The parameter *modulefile* may be either

 * a fully qualified *modulefile* with name and version

 * a symbolic *modulefile* name

 * another *modulefile* alias

**module-virtual** name modulefile

 Assigns the *modulefile* to the virtual module *name*. This command should be
 placed in rc files in order to define virtual modules.

 A virtual module stands for a module *name* associated to a *modulefile*. The
 modulefile is the script interpreted when loading or unloading the virtual
 module which appears or can be found with its virtual name.

 The parameter *modulefile* corresponds to the relative or absolute file
 location of a *modulefile*.

**module-whatis** string

 Defines a string which is displayed in case of the invocation of the
 **module whatis** command. There may be more than one **module-whatis**
 line in a *modulefile*. This command takes no actions in case of **load**,
 **display**, etc. invocations of **modulecmd.tcl**.

 The *string* parameter has to be enclosed in double-quotes if there's more
 than one word specified. Words are defined to be separated by whitespace
 characters (space, tab, cr).

**set-alias** alias-name alias-string

 Sets an alias or function with the name *alias-name* in the user's
 environment to the string *alias-string*. For some shells, aliases are not
 possible and the command has no effect. When a *modulefile* is unloaded,
 **set-alias** becomes **unset-alias**.

**unset-alias** alias-name

 Unsets an alias with the name *alias-name* in the user's environment.

**set-function** function-name function-string

 Creates a function with the name *function-name* in the user's environment
 with the function body *function-string*. For some shells, functions are not
 possible and the command has no effect. When a *modulefile* is unloaded,
 **set-function** becomes **unset-function**.

**unset-function** function-name

 Removes a function with the name *function-name* from the user's environment.

**system** string

 Run *string* command through shell. On Unix, command is passed to the
 ``/bin/sh`` shell whereas on Windows it is passed to ``cmd.exe``.
 **modulecmd.tcl** redirects stdout to stderr since stdout would be parsed by
 the evaluating shell. The exit status of the executed command is returned.

**uname** field

 Provide lookup of system information. Most *field* information are retrieved
 from the **tcl_platform** array (see **tclvars**\ (n) man page). Uname will
 return the string "unknown" if information is unavailable for the *field*.

 **uname** will invoke **uname**\ (1) command in order to get the operating
 system version and **domainname**\ (1) to figure out the name of the domain.

 *field* values are:

 * sysname: the operating system name

 * nodename: the hostname

 * domain: the name of the domain

 * release: the operating system release

 * version: the operating system version

 * machine: a standard name that identifies the system's hardware

**x-resource** [resource-string|filename]

 Merge resources into the X11 resource database. The resources are used to
 control look and behavior of X11 applications. The command will attempt
 to read resources from *filename*. If the argument isn't a valid file
 name, then string will be interpreted as a resource. Either *filename*
 or *resource-string* is then passed down to be **xrdb**\ (1) command.

 *modulefiles* that use this command, should in most cases contain one or
 more **x-resource** lines, each defining one X11 resource. The **DISPLAY**
 environment variable should be properly set and the X11 server should be
 accessible. If **x-resource** can't manipulate the X11 resource database,
 the *modulefile* will exit with an error message.

 Examples:

 **x-resource** /u2/staff/leif/.xres/Ileaf

  The content of the *Ileaf* file is merged into the X11 resource database.

 **x-resource** [glob ~/.xres/ileaf]

  The Tcl glob function is used to have the *modulefile* read different
  resource files for different users.

 **x-resource** {Ileaf.popup.saveUnder: True}

  Merge the Ileaf resource into the X11 resource database.


Modules Variables
-----------------

The **ModulesCurrentModulefile** variable contains the full pathname of
the *modulefile* being interpreted.


Locating Modulefiles
--------------------

Every directory in **MODULEPATH** is searched to find the
*modulefile*. A directory in **MODULEPATH** can have an arbitrary number
of sub-directories. If the user names a *modulefile* to be loaded which
is actually a directory, the directory is opened and a search begins for
an actual *modulefile*. First, **modulecmd.tcl** looks for a file with
the name *.modulerc* in the directory. If this file exists, its contents
will be evaluated as if it was a *modulefile* to be loaded. You may place
**module-version**, **module-alias** and **module-virtual** commands inside
this file.

Additionally, before seeking for *.modulerc* files in the module directory,
the global modulerc file and the *.modulerc* file found at the root of the
modulepath directory are sourced, too. If a named version default now exists
for the *modulefile* to be loaded, the assigned *modulefile* now will be
sourced. Otherwise the file *.version* is looked up in the module directory.

If the *.version* file exists, it is opened and interpreted as Tcl code and
takes precedence over a *.modulerc* file in the same directory. If the Tcl
variable **ModulesVersion** is set by the *.version* file, **modulecmd.tcl**
will use the name as if it specifies a *modulefile* in this directory. This
will become the default *modulefile* in this case. **ModulesVersion** cannot
refer to a *modulefile* located in a different directory.

If **ModulesVersion** is a directory, the search begins anew down that
directory. If the name does not match any files located in the current
directory, the search continues through the remaining directories in
**MODULEPATH**.

Every *.version* and *.modulerc* file found is Tcl interpreted. The
difference is that *.version* only applies to the current directory, and the
*.modulerc* applies to the current directory and all subdirectories. Changes
made in these files will affect the subsequently interpreted *modulefile*.

If no default version may be figured out, an implicit default is selected when
this behavior is enabled (see **MODULES_IMPLICIT_DEFAULT** in
:ref:`module(1)`). If disabled, module names should be fully qualified when no
explicit default is defined for them, elsewhere no default version is found
and an error is returned. If enabled, then the highest numerically sorted
*modulefile*, virtual module or module alias under the directory will be used.
The dictionary comparison method of the **lsort**\ (n) Tcl command is used to
achieve this sort. If highest numerically sorted element is an alias, search
continues on its *modulefile* target.

For example, it is possible for a user to have a directory named X11 which
simply contains a *.version* file specifying which version of X11 is to
be loaded. Such a file would look like:

.. code-block:: tcl

     #%Module1.0
     ##
     ##  The desired version of X11
     ##
     set ModulesVersion "R4"

The equivalent *.modulerc* would look like:

.. code-block:: tcl

     #%Module1.0
     ##
     ##  The desired version of X11
     ##
     module-version "./R4" default

If the extended default mechanism is enabled (see
**MODULES_EXTENDED_DEFAULT** in :ref:`module(1)`) the module version specified
is matched against starting portion of existing module versions, where portion
is a substring separated from the rest of version string by a ``.`` character.

If user names a *modulefile* that cannot be found in the first *modulepath*
directory, *modulefile* will be searched in next *modulepath* directory
and so on until a matching *modulefile* is found. If search goes through
a module alias or a symbolic version, this alias or symbol is resolved by
first looking at the *modulefiles* in the *modulepath* where this alias or
symbol is defined. If not found, resolution looks at the other *modulepaths*
in their definition order.

When locating *modulefiles*, if a *.modulerc*, a *.version*, a directory
or a *modulefile* cannot be read during the search it is simply ignored
with no error message produced. Visibility of *modulefiles* can thus be
adapted to the rights the user has been granted. Exception is made when
trying to directly access a directory or a *modulefile*. In this case,
the access issue is returned as an error message.

A *modulefile* whose name or element in its name starts with a '.' dot is
considered hidden. Hidden *modulefile* is not displayed or taken into account
except if it is explicitly named. By inheritance, a symbolic version-name
assigned to a hidden *modulefile* is displayed or taken into account only
if explicitly named. Module alias targeting a hidden *modulefile* appears
like any other module alias.


Advanced module version specifiers
----------------------------------

When the advanced module version specifiers mechanism is enabled (see
**MODULES_ADVANCED_VERSION_SPEC** in :ref:`module(1)`), the specification of
modulefile passed on Modules specific Tcl commands changes. After the module
name a version constraint prefixed by the ``@`` character may be added. It
could be directly appended to the module name or separated from it with a
space character.

Constraints can be expressed to refine the selection of module version to:

* a single version with the ``@version`` syntax, for instance ``foo@1.2.3``
  syntax will select module ``foo/1.2.3``
* a list of versions with the ``@version1,version2,...`` syntax, for instance
  ``foo@1.2.3,1.10`` will match modules ``foo/1.2.3`` and ``foo/1.10``
* a range of versions with the ``@version1:``, ``@:version2`` and
  ``@version1:version2`` syntaxes, for instance ``foo@1.2:`` will select all
  versions of module ``foo`` greater than or equal to ``1.2``, ``foo@:1.3``
  will select all versions less than or equal to ``1.3`` and ``foo@1.2:1.3``
  matches all versions between ``1.2`` and ``1.3`` including ``1.2`` and
  ``1.3`` versions

Advanced specification of single version or list of versions may benefit from
the activation of the extended default mechanism (see
**MODULES_EXTENDED_DEFAULT** in :ref:`module(1)`) to use an abbreviated
notation like ``@1`` to refer to more precise version numbers like ``1.2.3``.
Range of versions on its side natively handles abbreviated versions.

In order to be specified in a range of versions or compared to a range of
versions, the version major element should corresponds to a number. For
instance ``10a``, ``1.2.3``, ``1.foo`` are versions valid for range
comparison whereas ``default`` or ``foo.2`` versions are invalid for range
comparison.


Modulefile Specific Help
------------------------

Users can request help about a specific *modulefile* through the
:ref:`module(1)` command. The *modulefile* can print helpful information or
start help oriented programs by defining a **ModulesHelp** subroutine. The
subroutine will be called when the **module help modulefile** command
is used.


Modulefile Specific Test
------------------------

Users can request test of a specific *modulefile* through the :ref:`module(1)`
command. The *modulefile* can perform some sanity checks on its
definition or on its underlying programs by defining a **ModulesTest**
subroutine. The subroutine will be called when the **module test modulefile**
command is used. The subroutine should return 1 in case of success. If no
or any other value is returned, test is considered failed.


Modulefile Display
------------------

The **module display modulefile** command will detail all changes that
will be made to the environment. After displaying all of the environment
changes **modulecmd.tcl** will call the **ModulesDisplay** subroutine. The
**ModulesDisplay** subroutine is a good place to put additional descriptive
information about the *modulefile*.


ENVIRONMENT
-----------

**MODULEPATH**

 Path of directories containing *modulefiles*.


SEE ALSO
--------

:ref:`module(1)`, **Tcl**\ (n), **TclX**\ (n), **xrdb**\ (1), **exec**\
(n), **uname**\ (1), **domainname**\ (1), **tclvars**\ (n), **lsort**\ (n)


NOTES
-----

Tcl was developed by John Ousterhout at the University of California
at Berkeley.

TclX was developed by Karl Lehenbauer and Mark Diekhans.

