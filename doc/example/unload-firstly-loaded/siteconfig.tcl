#
# siteconfig.tcl - Site specific configuration changing getLoadedMatchingName
#   procedure to return firstly loaded module by default instead of lastly
#   loaded module. With this the firsly loaded module will be unloaded instead
#   of the lastly loaded in case multiple loaded modules match passed name to
#   unload. Which was the behavior of Modules v3.2 (compatbility) version.
#
# Author: Xavier Delaruelle <xavier.delaruelle@cea.fr>
# Compatibility: Modules v4.2
#
# Installation: put this file in the 'etc' directory of your Modules
#   installation. Refer to the "Modulecmd startup" section in the
#   module(1) man page to get this location.

# override 'getLoadedMatchingName' procedure to set behavior argument to the
# 'returnfirst' value by default
rename ::getLoadedMatchingName ::__getLoadedMatchingName
proc getLoadedMatchingName {name {behavior returnfirst} {loading 0}} {
   return [__getLoadedMatchingName $name $behavior $loading]
}

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
