# =======  Information re modules beneath current dir =====
# Author: Tom Payerle <payerle@umd.edu>

#--------------------------------------------------------------------
# FirstChildModuleInList
#
# Given a list of names of modulefile components, returns the
# first one found in the directory of the current modulefile
# (typically a .modulerc file).  Returns empty string if none
# of them were found.
# NOTE: we do NOT check for module magic signature or even that symlink
# targets exist.
proc FirstChildModuleInList { modlist } {
   global ModulesCurrentModulefile
   # Get directory for current modulefile
   set moduledir [ file dirname $ModulesCurrentModulefile ]
   # See if any names in $modlist in moduledir
   foreach mcomp $modlist {
      if [ file exists $moduledir/$mcomp ] { return $mcomp }
   }
   # Nothing found
   return
}

#--------------------------------------------------------------------
# ChildModuleExists
#
# Takes a module path component, and returns true if that path component
# exists beneath the current level, i.e. in the directory from which
# this .modulerc file was called.
# NOTE: we do NOT check for module magic signature or even that symlink
# targets exist.
proc ChildModuleExists { pathcomponent } {
   global ModulesCurrentModulefile
   # Get directory for current modulefile
   set moduledir [ file dirname $ModulesCurrentModulefile ]
   # See if file exists in moduledir
   return [ file exists $moduledir/$pathcomponent ]
}
