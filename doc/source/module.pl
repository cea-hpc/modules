#!/usr/bin/perl

print "Run pod2html module.pl > ../html/module.html to generate html " .
	"documentation for the module command.\n";
print "Run pod2man module.pl > ../man/man1/module.1 to generate man " .
	"documentation for the module command.\n";

=head1 NAME

module - command interface to the Modules package

=head1 SYNOPSIS

module [switches] [[sub-command] [sub-command-args]]

=head1 DESCRIPTION

B<module> is a user interface to the Modules package.  The Modules package 
provides for the dynamic modification of the user's environment via I<modulefiles>.

Each I<modulefile> contains the information needed to configure the shell for
an application.  Once the Modules package is initialized, the environment can
be modified on a per-module basis using the B<module> command which interprets
I<modulefiles>.  Typically I<modulefiles> instruct the B<module> command to
alter or set shell environment variables such as PATH, MANPATH, etc.
I<Modulefiles> may be shared by many users on a system and users may have
their own collection to supplement or replace the shared I<modulefiles>.

The I<modulefiles> are added to and removed from the current environment by 
the user.  The environment changes contained in a I<modulefile> can be 
summarized through the B<module> command as well.
If no arguments are given, a summary of the B<module> usage and I<sub-commands>
are shown.

The action for the B<module> command to take is described by the I<sub-command>
and its associated arguments.

=head2 Package Initialization

The Modules package and the B<module> command are initialized when a 
shell-specific initialization script is sourced into the shell.
The script creates the B<module> command as either an alias or function,
creates Modules environment variables, and a snapshot of the environment
is saved in ${HOME}/.modulesbeginenv.  The B<module> alias or function
executes the B<modulecmd.tcl> program located in ${MODULESHOME}/ and has the
shell evaluate the command's output.  The first argument to B<modulecmd.tcl>
specifies the type of shell.

The initialization scripts are kept in ${MODULESHOME}/init/[shellname]
where [shellname] is the name of the sourcing shell.  For example, a C Shell
user sources the ${MODULESHOME}/init/csh script.  The sh, csh, tcsh, bash,
ksh, and zsh shells are supported by B<modulecmd.tcl>.  In addition, python and
perl "shells" are supported which writes the environment changes to stdout
as python or perl code.

=head2 Examples of initialization

In the following examples, replace ${MODULESHOME} with the actual directory
name.

C Shell initialization (and derivatives):
        source ${MODULESHOME}/init/csh
        module load modulefile modulefile ...

Bourne Shell (sh) (and derivatives):
        . ${MODULESHOME}/init/sh
        module load modulefile modulefile ...

Perl:
        require "${MODULESHOME}/init/perl";
        &module("load modulefile modulefile ...");

=head2 Modulecmd startup

Upon invocation B<modulecmd.tcl> sources rc files which contain global, user
and I<modulefile> specific setups. These files are interpreted as 
I<modulefiles>.  See I<modulefile(4)> for detailed information.

Upon invocation of B<modulecmd.tcl> module RC files are sourced in the
following order:

        Global RC file as specified by ${MODULERCFILE} or 
		${MODULESHOME}/etc/rc

        User specific module RC file ${HOME}/.modulerc

        All .modulerc and .version files found during I<modulefile>
		seeking.

=head2 Command line switches

The B<module> command accepts command line switches as its first parameter.
These may be used to control output format of all information displayed and
the B<module> behavior in case of locating and interpreting I<modulefiles>.

All switches may be entered either in short or long notation. The following
switches are accepted:

=head3 --terse, -t

Display B<avail> and B<list> output in short format.

=head3 --long, -l

Display B<avail> and B<list> output in long format.

=head2 Module Sub-Commands

=head3 help [modulefile...]

Print the usage of each sub-command.
If an argument is given, print the Module specific help information for the
modulefile.

=head3 load modulefile [modulefile...]

=head3 add modulefile [modulefile...]

Load modulefile into the shell environment.

=head3 unload modulefile [modulefile...]

=head3 rm modulefile [modulefile...]

Remove modulefile from the shell environment.

=head3 switch modulefile1 modulefile2

=head3 swap modulefile1 modulefile2

Switch loaded modulefile1 with modulefile2

=head3 display modulefile [modulefile...]

=head3 show modulefile [modulefile...]

Display information about a modulefile.
The display sub-command will list the full path of the modulefile
and all (or most) of the environment changes the modulefile
will make if loaded.  (It will not display any
environment changes found within conditional statements.)

=head3 list

List loaded modules.

=head3 avail [path...]

List all available modulefiles in the current \s-1MODULEPATH\s0.
All directories in the \s-1MODULEPATH\s0 are recursively searched for
files containing the modulefile magic cookie.  If an argument is
given, then each directory in the \s-1MODULEPATH\s0 is searched for 
modulefiles whose pathname match the argument.
Multiple versions of an application can be supported by creating a
subdirectory for the application containing modulefiles for each
version.

=head3 use directory [directory...]

=head3 use [-a|--append] directory [directory...]

Prepend directory to the \s-1MODULEPATH\s0 environment variable.
The --append flag will append the directory to \s-1MODULEPATH\s0.

=head3 unuse directory [directory...]

Remove directory from the \s-1MODULEPATH\s0 environment variable.

=head3 update

Attempt to reload all loaded modulefiles.  The environment will be
reconfigured to match the saved .I ${\s-1HOME\s0}/.modulesbeginenv
and the modulefiles will be reloaded.  update will only change the
environment variables that the modulefiles set.

=head3 clear

Force the Modules Package to believe that no modules are currently loaded.

=head3 purge

Unload all loaded modulefiles.

=head3 whatis [modulefile [modulefile...]]

Display the modulefile information set up by the \fImodule-whatis\fP
commands inside the specified modulefiles. If no modulefiles are
specified all whatis information lines will be shown.

=head3 apropos string

=head3 keyword string

Seeks thru the whatis informations of all modulefiles for the
specified string.  All module whatis informations matching the
string search will be displayed.

=head3 initadd modulefile [modulefile...]

Add modulefile to the shell's initialization file in the user's
home directory.  The startup files checked are .cshrc, .login,
and .csh_variables for the C Shell;
.profile for the Bourne and Korn Shells; .bashrc, .bash_env, 
and .bash_profile for the GNU Bourne Again Shell;
.zshrc, .zshenv, and .zlogin for zsh. The .modules file is checked
for all shells.

If a 'module load' line is found in any of these files, the
modulefile(s) is(are) appended to any existing list of modulefiles.
The 'module load' line must be located in at least one of the
files listed above for any of the 'init' sub-commands to work properly.
If the 'module load' line is found in multiple shell initialization
files, all of the lines are changed.

=head3 initprepend modulefile [modulefile...]

Does the same as initadd but prepends the given modules to the
beginning of the list.

=head3 initrm modulefile [modulefile...]

Remove modulefile from the shell's initialization files.

=head3 initswitch modulefile1 modulefile2

Switch modulefile1 with modulefile2 in the shell's initialization files.

=head3 initlist

List all of the modulefiles loaded from the shell's initialization file.

=head3 initclear

Clear all of the modulefiles from the shell's initialization files.

=head1 Modulefiles

modulefiles are written in the Tool Command Language (tcl) and are
interpreted by modulecmd. modulefiles can use conditional statements.
Thus the effect a modulefile will have on the environment may change depending
upon the current state of the environment.

Environment variables are unset when unloading a modulefile.  Thus, it is
possible to load a modulefile and then unload it without having the
environment variables return to their prior state.

=head1 ENVIRONMENT

=head2 MODULESHOME

The location of the master Modules package file directory containing
module command initialization scripts, the executable program modulecmd,
and a directory containing a collection of master modulefiles.

=head2 MODULEPATH

The path that the module command searches when looking for modulefiles.
Typically, it is set to the master modulefiles directory,
${MODULESHOME}/modulefiles, by the initialization script.
MODULEPATH can be set using 'module use' or by the module
initialization script to search group or personal modulefile
directories before or after the master modulefile directory.

=head2 LOADEDMODULES

A colon separated list of all loaded modulefiles.

=head2 _LOADED_MODULEFILES_

A colon separated list of the full pathname for all loaded modulefiles.

=head2 _MODULESBEGINENV_

The filename of the file containing the initialization environment snapshot.

=head1 FILES

=head2 /soft/rko-modules/3.1.6

The MODULESHOME directory.

=head2 ${MODULESHOME}/etc/rc

The system-wide modules rc file.  The location of this file can be changed
using the MODULERCFILE environment variable as described above.

=head2 ${HOME}/.modulerc

The user specific modules rc file.

=head2 ${MODULESHOME}/modulefiles

The directory for system-wide modulefiles.  The location of the directory
can be changed using the MODULEPATH environment variable as described above.

=head2 ${MODULESHOME}/bin/modulecmd

The modulefile interpreter that gets executed upon each invocation of module.

=head2 ${MODULESHOME}/init/shellname

The Modules package initialization file sourced into the user's environment.

=head1 SEE ALSO

L<modulefile(4)>
