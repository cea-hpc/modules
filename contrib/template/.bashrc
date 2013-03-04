#This is an experimental bash resource file for our systems.
# email operator@cs.umn.edu if you have any problems with it.


#-----------------------------------------------------------------------------
#   The umask sets the access permissions for any new files you create.
#   Common umasks:
#     077 - our default - removes all permissions except the owner's
#     022 - the standard unix default - removes write permissions for
#           everyone other than the owner.  (Allows others to read most
#           of your files.)
#     027 - removes write permissions for the members of a file's group,
#           and removes all permissions for all others.
#   For an explanation of the octal encoding, see "man chmod".
#

umask 077

. /soft/rko-modules/tcl/init/bash

module load soft/gcc java perl gnu local compilers system
module load openwin math/mathematica scheme user

