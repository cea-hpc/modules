---------------------------------------
         Instructions
---------------------------------------


1. Instruct users to add this to their shell init scripts.

  If using sh/bash/ksh/zsh, add this to .profile

     eval `tclsh /path/to/modulecmd.tcl sh autoinit`

  If using csh/tcsh, add this to .login

     eval `tclsh /path/to/modulecmd.tcl csh autoinit`

  If using perl, put this in your script

     eval `tclsh /path/to/modulecmd.tcl perl autoinit`;


2. edit 'modulerc' as needed to point to the default modulefile
   directories. This file is sourced for all users. This file can 
   be either in the init/ subdirectory, or the same directory as the
   modulecmd.tcl, or both (not recommended).


NB. For backwards compatibility, the old init scripts are still here,
but they should be removed as soon as the new autoinit feature is
sufficiently tested.




