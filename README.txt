This is a tcl implementation of the module command.   The project
was started by Mark Lakata.

The goals have been

1. Written in pure TCL, so that there are no porting issues
2. Faster,smaller (implements most common features of the application)
3. 100% compatibility with existing modules (as done in practice)
4. Some new command line features
5. Path variable counters, to allow shared usage of particular path
   elements. I.e. modules can append /usr/bin, which is not unloaded
   until all the modules that loaded it unload too.
6. Support for "deep" modules.

Getting things running:
	There are a couple of steps to getting things working:

	You must have a working version of tclsh installed on your
	system.  Tclsh is a part of TCL (http://www.tcl.tk/software/tcltk/)

	If you want to build the testsuite you need to install: dejagnu
	(http://www.gnu.org/software/dejagnu/)
	The unix makefiles require: make and perl

	Once you have all of the prerequisits you can "INSTALL modules"
	

INSTALLING ON UNIX:

	1. make sure tclsh is installed in your path.
		(if not, you can hack modulecmd.tcl so it knows where 
		to find it)

	2. untar the modules/tcl directory in a global place that all users 
		can see.  (A NFS shared directory or a SAMBA share for example)
		I use the following scheme:
			/soft/rko-modules/tcl contains everything in this dir 
				including the init directory.
			/soft/rko-modules/modulefiles (contains my module files)

	3. run 'make' 
		This adds the path to your MODULESHOME directory to the 
		"init scripts". It also creates a default modulesrc file.

	4. Setup your modulesfiles directory.  If your using modules for the 
		first time, you probably want to copy the main modulefiles 
		that get installed with the c version of modules.

	5. If needed, edit the 'modulerc' file to point to the place where
		your modulefiles are stored. 

	6. instruct users to source the appropriate file inside the init/
   		directory.  For example lets say I have installed my modules in
		the directory /soft/rko-modules and the init files are in:
			/soft/modules/init 
		a tcsh user I would tell to add the following line to their 
		.tcshrc:
 			source /soft/rko-modules/init/tcsh
		a zsh user I would tell to add the following line to their 
		.profile:
 			. /soft/rko-modules/init/zsh


INSTALLING ON WINDOWS:
	1.  Install active state tcl on your computer.
		Edit your path and add c:\tcl\bin to it so tclsh is in your
		path.
	2.  Untar the modules/tcl directory in a global place for
		all users.  A share is a good place.  For this example,
		I'm using c:\modules
		Make an environment variable  MODULESHOME which is
		set to that path. (example: c:\modules)
	3.  Edit your path and add c:\modules\windows to your path.

	4.  Setup your modulesfiles directory.  If your using modules for the
		first time you probably want to copy the main modulefiles
		that get installed with the c version of modules.
	5. If needed edit c:\modules\modulesinit.cmd so it points to
		where you have your modulesfiles directory.

	6.  Have users run modulesinit


	There is also a module-tcl-win.chm file in the windows directory
	that has more detailed information.


For futher help check out the website:
	http://sourceforge.net/projects/modules/

Thanks,

Kent Mein
mein@cs.umn.edu
