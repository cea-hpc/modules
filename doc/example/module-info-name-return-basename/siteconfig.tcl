#
# siteconfig.tcl - Site specific configuration script to override
#   'module-info' procedure to return for 'name' action file basename
#   when modulefile is specified as a full pathname
#
# Author: Xavier Delaruelle <xavier.delaruelle@cea.fr>
# Compatibility: Modules v4.2+
#
# Installation: put this file in the 'etc' directory of your Modules
#   installation. Refer to the "Modulecmd startup" section in the
#   module(1) man page to get this location.

# override 'module-info' procedure to return file basename for 'name' action
# when modulefile is specified as a full path file
rename ::module-info ::__module-info
proc module-info {what {more {}}} {
   if {$what eq {name}} {
      set name [currentModuleName]
      if {[isModuleFullPath $name]} {
         return [file tail $name]
      } else {
         return $name
      }
   } else {
      return [__module-info $what $more]
   }
}

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
