# =======  Generic queries of loaded modules =====
# Author: Tom Payerle <payerle@umd.edu>

#--------------------------------------------------------------------
# GetLoadedModules
#
# Returns a tcl list of all modules loaded.  From $ENV{LOADEDMODULES}
proc GetLoadedModules { } {
   global env
   #Handle case if no modules loaded
   if { [info exists env(LOADEDMODULES)] == 0 } {
      #No modules loaded, return empty list
      return [ list ]
   }
   set loadedenv $env(LOADEDMODULES)
   set loaded [ split $loadedenv : ]
   return $loaded
}

#--------------------------------------------------------------------
# GetTagOfModuleLoaded(pkg)
#
# Looks for a loaded module matching ^$pkg/, and returns the tag matched.
# Returns {} if no tags matched (i.e. module pkg not loaded)
proc GetTagOfModuleLoaded { mymodule } {
   set loadedlist [ GetLoadedModules ]
   set regex "^$mymodule/"
   set fndidx [ lsearch -regex $loadedlist $regex ]
   if {  $fndidx == -1 }  { return {} }
   set found [ lindex $loadedlist $fndidx ]
   return $found
}
