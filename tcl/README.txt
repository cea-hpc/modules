This is an experiment tcl implementation of the module command. 

The goals have been

1. written in pure TCL, so that there are no porting issues
2. faster native implementation as well
3. 100% compatibility with existing modules (as done in practice)
4. some new command line features
5. path variable counters, to allow shared usage of particular path
   elements. I.e. modules can append /usr/bin, which is not unloaded
   until all the modules that loaded it unload too.


The new command line supports

 module switch <module>/<newrev>

which is the equivalent of

 module unload <module>
 module load  <module>/<newrev>


0. make sure you tclsh install in a sane place, ie /usr/bin or
   /usr/local/bin. 
(if not, you can hack modulecmd.tcl to change the path.)

1. untar in a global place that all users can see
   (hint: something that is available on NFS and SAMBA is a good idea
   if you
    have a mix of Unix and WinNT/2000/XP users)

2. cd to init/

3. run 'make' (this just hardcodes the installation path into the
   init/ files 
   with a perl one liner. if you don't have perl, then you can hand
   edit the 
   files. i suppose this should have been done using tclsh, but i'm a
   perl
   person.)

4. edit the 'modulerc' file to point to the place where the
   modulefiles are 
   stored. 

5. instruct users to source the appropriate file inside the init/
   directory, eg

 
 source /foo/bar/modules/init/tcsh

or

 . /foo/bar/modules/init/zsh


Be sure to run 'make' in the init directory, to create the
initialization scripts.


-Mark Lakata <lakata@mips.com>
Nov 29, 2002
May 30, 2003