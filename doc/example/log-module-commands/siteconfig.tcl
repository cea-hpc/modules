#
# siteconfig.tcl - Site specific configuration script changing the module
#   procedure to generate a log entry each time a module command is called
#   by user (module command called within modulefiles are not reported).
#
# Author: Xavier Delaruelle <xavier.delaruelle@cea.fr>
# Compatibility: Modules v4.2+
#
# Installation: put this file in the 'etc' directory of your Modules
#   installation. Refer to the "Modulecmd startup" section in the
#   module(1) man page to get this location.

# override 'module' procedure to log each call made by user
rename ::module ::__module
proc module {command args} {
   if {[getEvalModuleStackDepth] == 0} {
      exec logger -t module "[get-env USER]: $command [join $args]"
   }
   return [eval __module "{$command}" $args]
}

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
