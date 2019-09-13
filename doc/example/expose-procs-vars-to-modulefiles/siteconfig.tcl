#
# siteconfig.tcl - Site specific configuration script exposing additionnal
#   procedures and variables to the modulefile and modulerc evaluation
#   contexts
#
# Author: Xavier Delaruelle <xavier.delaruelle@cea.fr>
# Compatibility: Modules v4.2+
#
# Installation: put this file in the 'etc' directory of your Modules
#   installation. Refer to the "Modulecmd startup" section in the
#   module(1) man page to get this location.

# append site-specific procedures referenced in the g_siteProcsToExposeToItrp
# variable defined below to the list of procedures exposed in the modulefile
# and modulerc evaluation interpreters
proc addSiteProcsToItrpAliasList {itrpaliasvname keyname args} {
   # ensure this proc is only called once at itrpaliasvname initialization
   trace remove variable $itrpaliasvname write addSiteProcsToItrpAliasList
   upvar #0 $itrpaliasvname itrpaliasv
   # register each site-specific procedure
   foreach procname $::g_siteProcsToExposeToItrp {
      if {$keyname ne {}} {
         set itrpaliasv($procname) $procname
      } else {
         lappend itrpaliasv $procname $procname
      }
   }
}
trace add variable ::g_modfileBaseAliases write addSiteProcsToItrpAliasList
trace add variable ::g_modrcAliases write addSiteProcsToItrpAliasList


# Define here site-specific procedures that should be exposed to modulefile
# and modulerc interpreter contexts
# *Beware* not to override an existing procedure of modulecmd.tcl script
proc mysiteproc {} {
   return sitevalue
}

# list all site-specific procedures to expose to modulefile and modulerc
# interpreter contexts
set g_siteProcsToExposeToItrp [list mysiteproc]

# Define here site-specific variables that should be exposed to modulefile
# and modulerc interpreter contexts. Use environment variable env array to
# transmit these variables
# *Beware* not to override an existing environment variable
set env(mysitevar) sitevarvalue

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
