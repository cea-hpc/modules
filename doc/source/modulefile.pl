#!/usr/bin/perl

print "Run pod2html modulefile.pl > ../html/modulefile.html to generate html " .
	"documentation for the module file syntax.\n";
print "Run pod2man modulefile.pl > ../man/man4/modulefile.4 to generate man " .
	"documentation for the module file syntax.\n";
=head1 NAME

modulefile - files containing Tcl code for the Modules package

=head1 DESCRIPTION

I<modulefiles> are written in the Tool Command Language, Tcl and are
interpreted by the modulecmd program via the B<module> user interface.
I<modulefiles> can be loaded, unloaded, or switched on-the-fly while the
user is working.

A I<modulefile> begins with the magic cookie, '#%Module'.  A version number may
be placed after this string.  The version number is useful as the format of
I<modulefiles> may change.  If a version number doesn't exist, then 
B<modulecmd.tcl> will assume the I<modulefile> is compatible with the latest
version.  The current version for I<modulefiles> is 1.0.  Files without the
magic cookie will not be interpreted by B<modulecmd.tcl>.

Each modulefile contains the changes to a user's environment needed to
access an application.  Tcl is a simple programming language which
permits modulefiles to be arbitrarily complex, depending upon the
application's and the modulefile writer's needs.  If support for
extended tcl (tclX) has been configured for your installation of
modules, you may use all the extended commands provided by tclX, too.
modulefiles can be used to implement site policies regarding the access
and use of applications.

A typical modulefiles is a simple bit of code that set or add entries
to the PATH, MANPATH, or other environment variables.  Tcl has
conditional statements that are evaluated when the modulefile is
loaded.  This is very effective for managing path or environment
changes due to different OS releases or architectures.  The user
environment information is encapsulated into a single modulefile kept
in a central location.  The same modulefile is used by every user on
any machine.  So, from the user's perspective, starting an application
is exactly the same irregardless of the machine or platform they are on.
modulefiles also hide the notion of different types of shells.  From
the user's perspective, changing the environment for one shell looks
exactly the same as changing the environment for another shell.  This
is useful for new or novice users and eliminates the need for
statements such as "if you're using the C Shell do this ..., otherwise
if you're using the Bourne shell do this ...".  Announcing and accessing
new software is uniform and independent of the user's shell.  From the
modulefile writer's perspective, this means one set of information will
take care of every type of shell.

=head1 Modules Specific Tcl Commands

The Modules Package uses commands which are extensions to the
"standard" Tool Command Language Tcl package.  Unless otherwise
specified, the Module commands return the empty string.  Some commands
behave differently when a modulefile is loaded or unloaded.  The command
descriptions assume the modulefile is being loaded.

=head2 break

This is not a modules specific command, it's actually part of Tcl.  However,
it will have the effect of causing the module not to be listed as loaded and
not affect other modules being loaded concurrently.  All commands within the
module will be performed though.  An example: Suppose that a full
selection of modulefiles are needed for various different architectures, but
some of the modulefiles are not needed and the user should be alerted.  Having
the unnecessary modulefile be a link to the following notavail modulefile
will perform the task as required.

	#%Module1.0
	## notavail modulefile
	##
	proc ModulesHelp { } {
              puts stderr "This module does nothing but alert the user"
              puts stderr "that the [module-info name] module is not available"
	}

	module-whatis  "Notifies user that module is not available."
	set curMod [module-info name]
	if { [ module-info mode load ] } {
		puts stderr "Note: '$curMod' is not available for [uname sysname]."
	}
	break

=head2 setenv variable value

Set environment variable to value.  The setenv command will also change the
process' environment.  A reference using Tcl's env associative array will 
reference changes made with the setenv command.  Changes made using Tcl's env
associative array will NOT change the user's environment variable like the
setenv command.  An environment change made this way will only affect the
module parsing process. The setenv command is also useful for changing the
environment prior to the exec or system command.  When a modulefile is 
unloaded, setenv becomes unsetenv.  If the environment variable had been
defined it will be overwritten while loading the modulefile.   A subsequent
unload will unset the environment variable - the previous value cannot be
restored!   (Unless you handle it explicitly ... see below.)

=head2 unsetenv variable [value]

Unsets environment variable.  However, if there is an optional value, then
when unloading a module, it will set variable to value.  The unsetenv command
changes the process' environment like setenv.

=head2 append-path variable value

Append value to the end of environment variable.  The variable is a colon
separated list such as "PATH=directory:directory:directory".  If the variable
is not set, it is created.  When a modulefile is unloaded,  append-path
becomes remove-path.

=head2 prepend-path variable value

Append value to the front of environment variable.  The variable is a colon
separated list such as "PATH=directory:directory:directory".  If the variable
is not set, it is created.  When a modulefile is unloaded, prepend-path 
becomes remove-path.

=head2 remove-path variable value

Remove value from the colon separated list in variable.  Every string between
colons in variable is compared to value.  If the two match, value is removed
from variable.

=head2 prereq modulefile [ modulefile ...  ]

Prereq and conflict control whether or not the modulefile will be loaded.  The
prereq command lists modulefiles which must have been previously loaded 
before the current modulefile will be loaded.  If any of the modules
in the prereq list have not been loaded an error is reported and the current
modulefile makes no changes to the user's environment.

If an argument for prereq is a directory and any modulefile from the directory
has been loaded, then the prerequisite is met. For example, specifying X11 as
a prereq means that any version of X11, X11/R4 or X11/R5, must be loaded
before proceeding.

=head2 conflict modulefile [ modulefile ...  ]

Prereq and conflict control whether or not the modulefile will be loaded.  
The conflict command lists modulefiles which conflict with the current
modulefile.  If any of the modules in the conflict list have been loaded
an error is reported and the current modulefile makes no changes to the
user's environment.

If an argument for conflict is a directory and any other modulefile from that
directory has been loaded, then a conflict will occur.  For example,
specifying X11 as a conflict will stop X11/R4 and X11/R5 from being loaded 
at the same time.

=head2 is-loaded modulefile [ modulefile ...  ]

The is-loaded command returns a true value if any of the listed modulefiles
has been loaded.  If a list contains more than one modulefile, then each
member acts as a boolean OR operation.  If an argument for is-loaded is a
directory and any modulefile from the directory has been loaded is-loaded
would return a true value.

=head2 module [ sub-command ] [ sub-command-args ]

Contains the same sub-commands as described in the module man page in the
Module Sub-Commands section.  This command permits a modulefile to load or
remove other modulefiles.  No checks are made to ensure that the modulefile
does not try to load itself.  Often it is useful to have a single modulefile
that performs a number of module load commands.  For example, if every user on
the system requires a basic set of applications loaded, then a core
modulefile would contain the necessary module load commands.

=head2 module-info option [ info-args ]

Provide information about the modulecmd program's state.  Some of the
information is specific to the internals of modulecmd.  Option is the type
of information to be provided, and info-args are any arguments needed.

=head3 module-info flags

Returns the integer value of modulecmd's flags state.

=head3 module-info mode [modetype]

Returns the current modulecmd's mode as a string if no modetype is given.  
Returns 1 if modulecmd's mode is modetype. modetype can be: load, remove,
display, help, whatis, switch, switch1, switch2, or switch3.

=head3 module-info name

Return the name of the modulefile. This is not the full pathname for
modulefile.  See the Modules Variables section for information on the full
pathname.

=head3 module-info specified

Return the name of the modulefile specified on the command line.

=head3 module-info shell

Return the current shell under which modulecmd was invoked.  This is
the first parameter of modulecmd, which is normally hidden by the module alias.

=head3 module-info shelltype

Return the family of the shell under which modulefile was invoked.  As of
module-info shell this depends on the first parameter of modulecmd. The
output reflects a shell type determining the shell syntax of the commands
produced by modulecmd.

=head3 module-info alias name

Returns the full module file name to which the module file alias name is
assigned

=head3 module-info version module-file

Returns a list of all symbolic versions assigned to the passed module-file.  
The parameter module-file might either be a full qualified module file with
name and version, another symbolic module file name or a module file alias.

=head2 module-version module-file version-name [version-name ...]

Assigns the symbolic version-name to the module file module-file This
command should be placed in one of the modulecmd rc files in order to provide
shorthand invocations of frequently used module file names.

The special version-name default specifies the default version to be used for
module commands, if no specific version is given.  This replaces the 
definitions made in the .version file in former modulecmd releases.

The parameter module-file may be either

	a fully qualified modulefile with name and version
	a symbolic module file name
	another module file alias

=head2 module-alias name module-file

Assigns the module file module-file to the alias name.  This command should
be placed in one of the modulecmd rc files in order to provide shorthand
invocations of frequently used module file names.

The parameter module-file may be either

	a fully qualified modulefile with name and version
	a symbolic module file name
	another module file alias

=head2 module-trace  {on | off}  [command  [command  ...]]   [-module modulefile
       [modulefile ...]]

Switches tracing on or off. Without parameters this command will affect
globally all tracing setups for all commands and modulefiles.  The command
parameter may be used to affect tracing of specified module commands only and
the switch -module finally limits the affect of the module-trace command to a
well defined set of module files.

The command may be one of the following

	avail - 'module avail' command
	clear - 'module clear' command
	display - 'module display' command
	init - 'module init' command
	help - 'module help' command
	list - 'module list' command
	load - 'module load' command
	purge - 'module purge' command
	switch - 'module switch' command
	unuse - 'module unuse' command
	unload - 'module unload' command
	update - 'module update' command
	use - 'module use' command

The module parameter specifies a set of module files using TCL regular
expressions. For example:

	.*  will affect all module files
	*/2.0 affects all module files at version 2.0
	gnu/.*  affects all versions of the gnu modulefile
	gnu/2.0 affects only version 2.0 of the gnu modulefile

The module parameter is prepended to the current tracing pattern list for the
specified module command.  It is evaluated from the left to the right.  The
first matching pattern defines the tracing parameter.

The internal trace pattern list is stored as a colon separated list.  In
advanced user level only, colons may be specified on the module parameter of
the module-trace command. This will directly take effect in the internal trace
pattern list.  In novice or expert user level a warning message will be
generated.

=head2 module-user level

Defines the user level under which module-cmd runs. This takes effect on the
error messages being produced and on the behavior of modulecmd in case of
detecting an outage.

The level parameter specifies the user level and may be one of the following
values:

	advanced, adv - advanced user level
	expert, exp - expert user level
	novice, nov - novice user level

=head2 module-verbosity {on | off}

Switches verbose modulecmd message display on or off.

=head2 module-log error-weight log-facility

Defines whether error messages of the specified weight should be logged and
conditionally assigns a log-facility. alias-name

The error-weight parameter specifies the error level to be logged.  It may be
one of the following values:

	verb - verbose messages
	info - informal messages
	debug - debugging messages
	trace - tracing output
	warn - warnings
	prob - problems (normally the modulecmd may be completed)
	error - errors (which normally leads to unsuccessful  end
		of the modulecmd)
	fatal - fatal system errors
	panic  -  very fatal system errors, e.g. internal program
		inconsistencies.

The log-facility parameter specifies the log destination.  This may either
switch off logging for the specified error-weight, direct log messages to a
special stream or a file or specify a syslog facility for logging. The
following values are allowed:

	stderr, stdout - predefined output streams for normal and
	error outputs. Note, that stdout  is  normally  used  for
	passing parameters to the invoking shell. Directing error
	output to  this  stream  might  screw  up  the  modulecmd
	integration to your shell.
	a  syslog  facility  - directs logging to the syslog. See
	syslog.conf for  detailed  description  of  the  valid
	syslog facilities.
	null,  none  -  will  suppress  logging  of the specified
	error-weight.
	a filename - is recognized by the first  character  being
	either  a '.' or a '/'. You must have write permission to
	the file you specify.

=head2 module-whatis string

Defines a string which is displayed in case of the invocation of the 'module
whatis' command.  There may be more than one module-whatis line in a
modulefile.  This command takes no actions in case of load, display, etc.
invocations of modulecmd.

The string parameter has to be enclosed in double-quotes if there's more than
one word specified.  Words are defined to be separated by whitespace
characters (space, tab, cr).

=head2 set-alias alias-name alias-string

Sets an alias or function with the name alias-name in the user's environment 
to the string alias-string.  Arguments can be specified using the Bourne Shell
style of function arguments.

If the string contains "$1", then this will become the first argument when the
alias is interpreted by the shell.  The string "$*" corresponds to all of the
arguments given to the alias.  The character '$' may be escaped using the '\'
character.

For some shells, aliases are not possible and the command has no effect.  
For Bourne shell derivatives, a shell function will be written (if supported)
to give the impression of an alias.  When a modulefile is unloaded, set-alias
becomes unset-alias.

=head2 unset-alias alias-name

Unsets an alias with the name alias-name in the user's environment.  If the
shell supports functions then the shell is instructed to unset function
alias-name.

=head2 system string

Pass string to the C library routine system.  For the system call
modulecmd redirects stdout to stderr since stdout would be parsed by the
evaluating shell.  The exit status of the executed command is returned.

=head2 uname field

Provide fast lookup of system information on systems that support uname.
uname is significantly faster than using system to execute a program to
return host information.  If uname is not available, gethostname or
some program will make the nodename available.  Uname will return the string
"unknown" if information is unavailable for the field.

Uname will invoke getdomainname in order to figure out the name of the domain.
Field values are:

	sysname - the operating system name
	nodename - the hostname
	domain - the name of the domain
	release - the operating system release
	version - the operating system version
	machine  -  a  standard name that identifies the system's hardware

=head2 x-resource [resource-string | filename ]

Merge resources into the X11 resource database.  The resources are used to 
control look and behavior of X11 applications.  The command will attempt to
read resources from filename.  If the argument isn't a valid file name, then
string will be interpreted as a resource.  If a file is found, it will be
filtered through the cpp preprocessor, just as xrdb would do.

modulefiles that use this command, should in most cases contain one or more
x-resource lines, each defining one X11 resource.  Reading resources from
filename is much slower, due to the preprocessing.  The DISPLAY environment
variable should be properly set and the X11 server should be accessible.  
If x-resource can't manipulate the X11 resource database, the modulefile
will exit with an error message.  Examples:

=head3 x-resource /u2/staff/leif/.xres/Ileaf

The file Ileaf is preprocessed by cpp and the result is merged into the
X11 resource database.

=head3 x-resource [glob ~/.xres/ileaf]

The Tcl glob function is used to have the modulefile read different resource
files for different users.

=head3 x-resource {Ileaf.popup.saveUnder: True}

Merge the Ileaf resource into the X11 resource database.

=head1 Modules Variables

The ModulesCurrentModulefile variable contains the full pathname of the
modulefile being interpreted.

=head1 Locating Modulefiles

Every directory in MODULEPATH is searched to find the modulefile.  A directory
in MODULEPATH can have an arbitrary number of sub-directories.  If the user 
names a modulefile to be loaded which is actually a directory, the directory
is opened and a search begins for an actual modulefile.  First, modulecmd
looks for a file with the name .modulerc in the directory.  If this file
exists, its contents will be evaluated as if it was a module file to be load.
You may place module-version and module-alias commands inside this file.
Additionally, before seeking for .modulerc files in the module directory, the
global .modulerc file is sourced, too.  If a named version default now exists
for the module file to be load, the assigned modulefile now will be sourced.
Otherwise the file .version is looked up in the directory.  If the .version
file exists, it is opened and interpreted as Tcl code.  If the Tcl variable
ModulesVersion is set by the .version file, modulecmd will use the name as if
it specifies a modulefile in the directory.  This will become the default
module file in this case.  If ModulesVersion is a directory, the search
begins anew down that directory.  If the name does not match any files located
in the current directory, the search continues through the remaining
directories in MODULEPATH.

Every .version and .modulerc file found is Tcl interpreted.  So, changes made
in these file will affect the subsequently interpreted modulefile.

If no default version may be figured out, then the highest lexicographically
sorted modulefile under the directory will be used.

For example, it is possible for a user to have a directory named X11 which
simply contains a .version file specifying which version of X11 is to be
loaded.  Such a file would look like:

	#%Module1.0
	##
	##  The desired version of X11
	##
	set ModulesVersion "R4"

=head1 Modulefile Specific Help

Users can request help about a specific I<modulefile> through the
module command.  The modulefile can print helpful information or
start help oriented programs by defining a I<ModulesHelp> subroutine.  The
subroutine will be called when the 'module help I<modulefile>' command is
used.

=head1 Modulefile Display

The 'module display I<modulefile>' command will detail all changes that
will be made to the environment.  After displaying all of the environment
 changes I<modulecmd.tcl> will call the ModulesDisplay subroutine.
The ModulesDisplay subroutine is a good place to put additional
descriptive information about the I<modulefile>.

=head1 ENVIRONMENT

=head2 ${MODULEPATH}

Path of directories containing I<modulefiles>.

=head1 SEE ALSO

L<module>, L<Tcl>, L<TclX>, L<xrdb>, L<cpp>, L<system>,
L<uname>, L<gethostname>, L<getdomainname>

=head1 NOTES

Tcl was developed by John Ousterhout at the University of California at
Berkeley.

TclX was developed by Karl Lehenbauer and Mark Diekhans.



