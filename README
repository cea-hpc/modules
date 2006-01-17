
                             The Modules Package
                                Version 3.1
                                
                               John L. Furlan
                               jlf@behere.com

			        Peter W. Osel
                                 pwo@Osel.DE

                                Jens  Hamisch
                          Jens.Hamisch@Strawberry.COM

                                  R.K. Owen
                              rk@owen.sj.ca.us


                               2  October 1996
                                2  June 2000

1. Introduction
_______________

This directory contains a copy of The Modules Package that uses Tcl (Tool
Command Language) developed by John Ousterhout at Sun Microsystems.  You
must obtain a copy of Tcl (at least version 8.3).  Tcl should be available
either at the same site you obtained The Modules Package or from
http://dev.scriptics.com/ .

For an introduction to The Modules Package, see the paper, "Modules: Providing
a Flexible User Environment", in the Proceeding of the 1991 Large Installation
Systems Administrators USENIX Conference (USENIX LISA V).  A copy of the paper
is included in Postscript form in doc/Modules-Paper.ps.  The paper describes
the concepts behind Modules and a prototype/proof-of-concept implementation
based on shell scripts being sourced into the user environment.  The
implementation details have changed, but the package concepts haven't.


2. Documentation
________________

The ./doc directory contains both the paper and man pages describing the
user's and the module writer's usage.
Look at ChangeLog for detailed information regarding changes.

Big changes to 3.2:
    o  Using the automake/autoconf mechanism for creating the configure script.

    o  Have the configure script test and set more things with regards to
       /bin/sh and /bin/csh characterstics.

    o  Lots of code clean-up.

    o  Made the tests conditional on configure options.

    o  Handle the silent changes in Tcl 8.4

Big changes to 3.1:
    o  Changed the license to GPL

    o  Source code cross-referencing documented in ./xref/*.html files.

Big changes since 3.0 prior to 3.1:
    o  Ported to Linux.

    o  Removed any dependence on low-level Tcl routines.

    o  Can use environment variable references in MODULEPATH for indirection.

    o  Can switch between different versions of the module command.

    o  Fixed "module whatis" to work as advertised.

    o  Added the INSTALL document to aid in first-time and upgrading
       installations.

    o  Fixed "module avail" and "module list" to show levels deeper than 2.

    o  Added "is-loaded" and "module-info specified" to modulefile commands.

Big changes for 3.0:
    o  Testsuite added

    o  Support for Tcl/TclX 7.6 and Tk 4.2

    o  Introduction of user levels for generation of error messages

    o  Enhanced module logging facilities using stdout, stderr, files
       and the syslog

    o  Enhanced module trace feature

    o  Modulefile configuration management. Symbolic module file names
       and aliases

    o  Hierarchical modulecmd rc-file structure

    o  Command line switches

    o  Traceable output for module list and module command (configurable
       and on request)

    o  Active dependecy resolution. This is experimental at the moment!!!

Big changes for 2.3:
    o  Support for Tcl/TclX 7.5 and Tk 4.1

    o  Code cleanup. Added inline documentation. Unification of source
        module layouts.

    o  Added alloc tracer for locating memory leaks

    o  Added 'uname domain'.

    o  Added M_HELP as a modetype, so that 'module-info mode [help]' will
	be possible

    o  Optional support for extended Tcl (tclX) 7.x

    o  Modules now uses the standard Tcl initialization (init.tcl),
	enabling autoloading of Tcl functions.

Big changes for 2.2:
    o  Installation changed to use GNU's configure script (version 2.x).

    o  Updated to support Tcl 7.x

    o  Paths and modulefile names with '+'s in them should work now.

    o  Multi-level paths should work.  Well, at least better than 2.0.

    o  "module load ." doesn't dump core.

    o  Support for "perl" output.

    o  Ability to exit without having modulefile listed as "loaded".
        Use an argument to exit other than 0 or 1.

    o  More stringent checking of user-level command names.

    o  A sysconf.h file to help with multi-platform support.
        This area and the installation process still needs work.

    o  A new and improved cmdXResource.c

    
Big changes for 2.0:

    o  Added a new environment variable, _LOADED_MODULEFILES_, which keeps
        track of which file and directory each modulefile was loaded from.
        This is necessary for locating modulefiles if the MODULEPATH variable
        has changed or if a full pathname was specified as the modulefile to
        load.

    o  Added an update sub-command which attempts to reload all of the
        currently loaded modulefiles.  When Modules is first initialized a
        snapshot of the environment is saved into a file in the user's home
        directory.  When the update sub-command is run, the environment
        variables are restored to their earlier state and the modulefiles are
        reloaded.  Only the variables that modulefiles touch are changed.
        
    o  Added an purge sub-command which unloads all currently loaded
        modulefiles.
    
    o  set-alias should be fixed and should work for all shell types.  In the
        case of the Bourne shell-variants, it creates functions.

    o  If multiple modulefiles are listed to be added or removed and one of
        them has an error, only that modulefile will fail.  All of the other
        modulefiles will load or unload successfully (assuming they don't in
        turn have errors).

    o  INCOMPATABILITY ISSUE:  stdout is not tied to stderr any more.  So, if
        you to a 'puts stdout' in any of your modulefiles, that will go
        directly to the shell.  You should change all puts statements that
        goto stdout to goto stderr.
        
    o  avail prints out all of the modulefiles by recursively looking through
        all directories under every entry in MODULEPATH.  It will also cache
        the information since it can take much longer than the previous
        version.  Finally, only files containing the "#%Module" magic cookie
        header and that don't end in '~' will be listed.  Other files will be
        ignored.

    o  A .version file in a modulefile directory will be parsed to find out
        which modulefile is the default version for the directory name.

    o  Locating modulefiles is recursive and ignores files ending in '~' and
        which don't have the "#%Module" magic header.

    o  The switch command really works now.

    o  Added uname command for fast access to system information.  Not sure if
        uname is portable.

    o  Unload using a directoryname will look for a version that is already
        loaded instead of the default version specified by the .version file
        or by the highest lexicographical name in the directory.

    o  Added ability for modulefile-specific help and modulefile-specific
        display.



3. Building and Installing Modules
__________________________________
(Read the INSTALL file for a more recent description of installation
procedures.)

First, you'll need to decide on a location to keep Modules on your network.

The path name to the initialization files must be the same on all of the
systems using Modules.  This is because your users' .cshrc or other shell
startup files must hard code this directory in order to source their Modules
initialization file.

You will also need to choose a location for the 'modulecmd' program and all of
your modulefiles.  The location of these can be different on different
systems, but it is not recommended.  These paths are used in the Modules
initialization files.  If you decide not to use this structure, then you
will need to edit the initialization files by hand.  Use the --prefix
configure option to control the installation of Modules.  See below for more
configure options.

Example:
	If you decide to keep Modules in /depot/Modules, run
            "configure --prefix=/depot/Modules" 

	initialization file   -->  /depot/Modules/init
	modulefiles           -->  /depot/Modules/modulefiles
	modulecmd             -->  /depot/Modules/bin
 	manual pages          -->  /depot/Modules/man
 	Tcl libraries	      -->  /depot/Modules/lib/tcl	(if any)

The Modules Package is written in conformant ANSI C and should build on any
UNIX machine with an ANSI C compiler like the GNU C compiler.  Check the
MACHINES file for a list of machines and OSs Modules has been test built on.
The module error logger requires the ANSI C 'stdarg.h' being installed. At
the moment there's no support for traditional vargargs handling using 
'varargs.h'.

To build Modules, first build Tcl and run the tests provided with Tcl to
verify it built correctly.

If you want to use the testsuite, you should install dejagnu-1.2 or better
before configuring modules. Be sure to have your PATH pointing to 'runtest'
when calling 'configure'.

Then run the GNU autoconf script, ./configure.
A Makefile will be automatically generated.

Some Modules-specific configure options...  (See also ./configure --help)

Autoconf @variable@'s can be passed along to the configure script, in
particular for setting paths.  The one of interest is @VERSION@ for
using the current module version. e.g.  --prefix=/usr/local/Modules/@VERSION@

------------------------------------------------------------------------
    --with-etc-path=<path>      use etc path=<path>		[/etc]
    --with-skel-path=<path>     use skel path=<path>		[/etc/skel]

  o Place to find the csh.modules * profile.modules files and where
    the new user dot files are (both are needed by add.modules script).
  o The skeleton user dot files need to point to the etc path for
    setting up modules environment on login.

------------------------------------------------------------------------
    --with-split-size=<size>	With split size=<size>		[TEST]
    --without-split-size	Without  - " -

  o Most C Shell implementations have a size requirement on the length of
      the line which can be evaluated.  If you run into very long
      environment changes, you may run into this problem.  Select size
      to be something less than the size your C Shell will accept.
      The configure script will test this in your /bin/csh upto
      10,000 characters.  If you do not specify anything configure
      will set the split-size for you if the limit it finds is less
      than 4000 characters.  This can be overridden by specifying
      --with-split-size='some_value' specifying --without-split-size
      will disable this feature.  Just specifying --with-split-size
      without a value will default to a value of 1000.

------------------------------------------------------------------------
    --enable-shell-funcs	With sh functions		[TEST]
    --disable-shell-funcs	Without  - " -

  o Does your Bourne Shell support functions?  If so, then the modulefile
      set-alias will work properly.  If not, aliases will not be available
      when using the Bourne Shell.
      The default is to test your /bin/sh when configuring build, but
      specifying either option will override the tested characteristic.

------------------------------------------------------------------------
    --enable-shell-alias	With sh functions		[TEST]
    --disable-shell-alias	Without  - " -

  o Does your Bourne Shell support aliases?  A true Bourne Shell doesn't
      but most are just a link to some other Bourne-like shell.  If so,
      then modules will use the alias mechanism when using set-alias.
      If not, aliases will not be available unless sh functions are
      supported when using the Bourne Shell.
      The default is to test your /bin/sh when configuring build, but
      specifying either option will override the tested characteristic.

------------------------------------------------------------------------
    --enable-shell-eval		With shell alias eval		[DEF]
    --disable-shell-eval	Without  - " -

  o This specifies whether aliases will be evaluated by the shell or
      sourced from a temporary file.  Some shells can't handle the
      specification of aliases in the eval line very well.  To get around
      this problem, when aliases are set by a modulefile, a temporary file
      is created and then sourced into the shell.  This guarentees the
      aliases are evaluated correctly, but may leave lots of cruft
      in the /tmp directory, which is generally not a good idea.
 
------------------------------------------------------------------------
    --enable-free		With free() calls
    --disable-free		Without  -" -		[DEF]

  o Since the modulecmd is transient, disabling calls to free() can provide
      some small performance enhancement since the memory usage is low and
      the program will exit rapidly.  If you want the calls to free() back
      in for some reason, set this option.

------------------------------------------------------------------------
    --enable-cache		With directory cache
    --disable-cache		Without  - " -			[DEF]

  o Modules will maintain a cache of the available modulefiles for each
      directory in the modulefile search path.  The cache greatly
      accelerates how long it takes to determine the list of available
      modulefiles, but for automatic cache updates this requires a
      world writeable file.

------------------------------------------------------------------------
    --with-cache-umask=<umask> With cache files umask=<umask>
    --without-cache-umask      0				[DEF]

  o Set the umask for creating cache files.  The default of 0 creates
      modulecachefiles that are writable by anyone.  If you don't like
      world writable files, or mount the Modules file system read-only
      set it to 002.  Don't forget to occasionally refresh the cache
      files by running module avail with an account that has write
      permissions.

------------------------------------------------------------------------
     --with-static		Link static (don't use dynamic libraries)
     --without-static		Use dynamic libraries		[DEF]

  o A note.  I've found that at least the X11 libraries should be linked
      statically.  If you encounter problems, e.g. getting some warnings
      about version mismatch of dynamic libraries, link the modulecmd
      statically.

  o If you're using Tcl/TclX 7.5 on an SunOS or Solaris box, the dynamic
      loader library (libdl.so.1) will be liked in. Since this is not
      available as a staic library, using  --without-static is recommended.

------------------------------------------------------------------------
     --with-tclx		With extended TCL (tclX) commands
     --without-tclx		Without  - " -			[DEF]

  o Extended Tcl is a superset of standard Tcl and is built alongside
      the standard Tcl sources.  It adds many new commands to standard
      Tcl, e.g. Unix Access Commands, File I/O Commands, String and
      Character Manipulation Commands, ...  See the manual pages that
      come with tclX for a complete list of added features.

------------------------------------------------------------------------
     --with-module-path=<path>	With user defined MODULEPATH
     --without-module-path	Default $prefix/modulefiles	[DEF]

  o Set the MODULEFILE environment variable in all initialization
      files to the given <path> (colon separated!).  The default
      is to use $prefix/modulefiles (which usually contains version
      specific modulefiles).
  o Every site should use this to specify where local modulefiles
      are placed which are independent of module versions.

------------------------------------------------------------------------
    --with-version-path         use module path=<path>
                                [/usr/local/Modules/versions]

  o Place to put module version modulefiles for switching between different
      versions of the module command.
------------------------------------------------------------------------
     --with-autoload-path=<path>	directories where Tcl will search
					for libraries to load
     --without-autoload-path		Default $prefix/lib/tcl	[DEF]

  o <path> will be prepended to Tcl's auto_path variable.  These
      directories are searched for library functions that can be
      autoloaded.  Note that this path has to be *space* separated and
      enclosed in double quotes!!
      The default is to use "$prefix/lib/tcl".

------------------------------------------------------------------------
     --with-tcl=<path>		directory containting Tcl configuration
				(tclConfig.sh)
     --with-tclx=<path>		directory containting TclX configuration
				(tclxConfig.sh)

  o the tclConfig.sh file was created when Tcl was built, and has pointers
  to where the libraries and header files were installed. It is usually
  found in $prefix/lib, where $prefix is the directory you installed Tcl.
  (Same for TclX too.)

------------------------------------------------------------------------
     --with-debug=<level>		Set up the debug level to the
					specified value
     --without-debug			Disable debugging [DEF]

  o The debug level will be set to the specified value. Available values
      may be looked up in 'modules_def.h'. Debugging may be configured in
      order to trace:

	- module commands
	- TCL interpreter initialization
	- callback functions
	- module file localization
	- utility functions

      Enable this feature for debugging only! The higher the debugging level,
      the greater the amount of runtime information printed! All debug mes-
      sages will be spooled to <stderr>.

      The debug-level cannot be changed at runtime at the moment! It is
      configured as a fixed value at compile time!

------------------------------------------------------------------------
     --enable-version-magic             set .version magic requirement[DEF]
     --disable-version-magic            unset requirement

  o Enable or disable the requirement that .version files need the
      "#%MOdule1.0" magic cookie at the file start to be properly recognized
      and parsed.

------------------------------------------------------------------------
NO LONGER AVAILABLE
     --enable-parseable-msgs		Configure parseable error messages
     --disable-parseable-msgs		Use the traditional ones

  o Switch between traditional error messages in multi-line format or parse-
      able ones that look like the GNU messages. The parseable message format
      is:

	module(line):Message-Type:Message-Number: Text

      with Message-Type being one of:

	INFO, WARNING, PROBLEM, ERROR, FATAL, PANIC, DEBUG

      and an unique message id in decimal format.

------------------------------------------------------------------------
      --enable-logging                  Enable the module logging feature
      --disable-logging                 No module logging [DEF]
      --with-log-facility-verbose[=<facility>] 
      --with-log-facility-info[=<facility>]
      --with-log-facility-debug[=<facility>]
      --with-log-facility-debug[=<facility>]
      --with-log-facility-warn[=<facility>]
      --with-log-facility-problem[=<facility>]
      --with-log-facility-problem[=<facility>]
      --with-log-facility-fatal[=<facility>]
      --with-log-facility-panic[=<facility>]

   o Turn logging on and define the logging facilities for the different
     error levels. Each facility may be a syslog-facility as 'local7.debug',
     a standard stream as 'stderr', a filename or the word 'no' for dis-
     abling a single log facility.

------------------------------------------------------------------------
      --with-trace-load=<value>  
      --with-trace-unload=<value>
      --with-trace-switch=<value>
      --with-trace-disp=<value>
      --with-trace-list=<value>
      --with-trace-avail=<value>
      --with-trace-help=<value>
      --with-trace-init=<value>
      --with-trace-use=<value>
      --with-trace-unuse=<value>
      --with-trace-update=<value>
      --with-trace-purge=<value>
      --with-trace-clear=<value>
      --with-trace-whatis=<value>
      --with-trace-apropos=<value>

   o Turns tracing for the specific module command on or off. The value
     is a list of colon separated TCL regular expressions matching the
     module files to be traced. This maybe overridden using the 
     'module-trace' command. See 'modulefile(4)' for detailed information.



A number of example modulefiles have been provided  in the example-modulefiles
directory.  This should help provide you with some idea on how to write
modulefiles.

If you upgrade from a previous module version and have adapted the
MODULEPATH environment variable in your installed init files, don't
forget to set the MODULEFILE in the configuration step:
(See the INSTALL document for more current information.)
 
	./configure --with-module-path=$MODULEPATH

Just type 'make' and it should build.

If you have dejagnu installed, you may want to run the testsuite now. Type
'make check' and it should run.

Finally, type 'make install' and 'modulecmd' along with the initfiles will be
installed.


4. modules-interest@lists.sourceforge.net
_________________________________________

As of release 2.0, I have worked with Richard Elling to create a
Modules interest alias for discussion about Modules and other
Modules-related packages such as Richard's user-setup.

The email list was moved from eng.auburn.edu to SourceForge.net with
version 3.0.  If you would like to be added to the modules-interest alias,
email majordomo@lists.sourceforge.net with "subscribe modules-interest
address" in the body of the message where "address" is your Internet
e-mail address.


5. Miscellaneous
________________

tcsh(1) user might want to add the following to their tcsh startup file
(~/.cshrc or ~/.tcshrc): 

# program tcsh's completion for module command:
complete module \
	'n/use/d/' \
	'n/unuse/d/' \
	'n/*/(load unload switch display avail use \
		unuse update purge list clear help \
		initadd initrm initswitch initlist initclear)/'


You might want to add the following line to /etc/magic, so that file(1)
recognizes modulefiles:

#
# Environment Modules modulefiles
#
0	string		#%Module1.0	Environment Modules version 1.0


6. Special Thanks
_________________

Ken Manheimer and Don Libes at the National Institute of Standards and
Technology deserve special thanks for their help and ideas toward the
original paper, design considerations, and the use of Tcl.

Maureen Chew and others at Sun Microsystems have provided me with an
test site and many ideas on how to improve my Tcl implementation of
Modules.

Leif Hedstrom added GNU's autoconf support, x-resource command, helped
significantly with the port to Tcl 7.x and has provided valuable input.

I would also like to thank Richard Elling at Auburn University for his 
comments, help with the Modules' man pages and his application
'user-setup'.  A paper on 'user-setup' was presented at this year's
USENIX LISA VI conference.  The paper and the application can be
acquired via anonymous ftp from ftp.eng.auburn.edu.

Peter W. Osel <pwo@guug.de> added support for Extended Tcl (TclX),
autoloading of Tcl functions, autoconf 2.x;
 
There are many others that deserve thanks but too many to list here --
thanks to everyone who has helped.

R.K.Owen <rk@owen.sj.ca.us> added the modules versioning, ported to Linux,
reworked the code to use higher level Tcl calls eliminating the need for
Tcl internal header files, various code fixes and changes, added some useful
scripts, and is the current maintainer of the modules-3.1 distribution.

Robert Minsk <egbert@centropolisfx.com>, added the "module-info specified"
and the "is-loaded" modulefile commands, and for various code and script
fixes.

Further & updated list of contributors can be found in the ChangeLog file.


7. Bugs and Comments
____________________

Report bugs to 'modules-interest@Eng.Auburn.EDU'.  Please try to provide
a full environment listing and a copy of the modulefiles you're trying
to manipulate.  Be as explicit and detailed as possible.

Comments and suggestions for improvement are always welcome.


8. Getting the Sources
______________________

There are two ways to get the sources.  If you just intend to use
modules and have no interest in modifying or improving them, then
get them via anonymous ftp from

Primary-site:	ftp1.sourceforge.net	/pub/sourceforge/m/mo/modules
Alternate-site:	sunsite.unc.edu		/pub/Linux/utils/shell/
Alternate-site: owen.sj.ca.us		/pub/rkowen/modules/

However, if you have coding expertise and wish to contribute code
modifications ... or you want the ``bleeding edge'' latest sources
then get them via the Modules CVS repository server:

This will download the current checked-in version

  cvs -d:pserver:anonymous@cvs.modules.sourceforge.net:/cvsroot/modules login
        Just hit return when prompted for the password.

  cvs -d:pserver:anonymous@cvs.modules.sourceforge.net:/cvsroot/modules \
	checkout modules 

(The preceeding commands may be wrong - visit <http://modules.sf.net> for
other CVS download instructions.)

If you have modules installed already from a recent distribution, then
you can use the aliases defined by the module-cvs modulefile.  Typically,
it can be loaded with:

  module load modules module-cvs

Then use the modules-login and modules-get aliases.

You will need recent versions of automake and autoconf if you use the CVS
version of modules.

After your initial checkout you must run the command:

 % autoreconf -f -i

to generate the rest of the build infrastructure.

For a brief HOWTO on CVS read http://kooz.sj.ca.us/rkowen/howto/cvsF.html .

To contribute code patches, please "cvs diff -u" them against
the modules CVS repository.


Thanks,
John L. Furlan
(with additions by R.K.Owen and Harlan Stenn)
