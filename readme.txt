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

	Once you have all of the prerequisits you can "install modules"
	
To learn how to install modules see one of the following docs:
	For unix: doc/install.txt
        For windows: doc/installwin.txt

To have things running efficently you will need a lot of additional setup.
For an example take a look at doc/example.txt which will explain how
we have things setup at the University of Minnesota computer science department.

For futher help check out the website:
	http://sourceforge.net/projects/modules/

Thanks,

Kent Mein
mein@cs.umn.edu
