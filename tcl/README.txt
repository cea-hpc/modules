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



Be sure to run 'make' in the init directory, to create the
initialization scripts.


-Mark Lakata <lakata@mips.com>
Nov 29, 2002
