Instructions

1. cvs update this directory in the public or local area
2. run 'make' to generate the init files with the proper path to the
   modulecmd.tcl
3. edit 'modulerc' as needed to point to the default modulefile
   directories. This file is sourced for all users.

Do not edit the script files (eg "sh"). Edit the input templates (eg "sh.in")
instead, unless you have a good reason to edit the output files. Note that
that the output files are not stored in cvs anymore.
