##########################################################################

# MODSCAN.TCL, modulefile scan and extra match search procedures
# Copyright (C) 2022-2023 Xavier Delaruelle
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

##########################################################################

# optimized variant command for scan mode: init entry in ModuleVariant array
# to avoid variable being undefined when accessed during modulefile evaluation
# record variant definition in structure for extra match search or report
proc variant-sc {itrp args} {
   # parse args
   lassign [parseVariantCommandArgs {*}$args] name values defdflvalue\
      dflvalue isboolean

   # remove duplicate possible values for boolean variant
   if {$isboolean} {
      set values {on off}
      if {$defdflvalue} {
         set dflvalue [expr {$dflvalue ? {on} : {off}}]
      }
   }

   recordScanModuleElt [currentState modulename] $name variant

   lappend ::g_scanModuleVariant([currentState modulename]) [list $name\
      $values $defdflvalue $dflvalue $isboolean]

   # instantiate variant in modulefile context to an empty value
   reportDebug "Set variant on $itrp: ModuleVariant($name) = ''"
   $itrp eval set "{::ModuleVariant($name)}" "{}"
}

proc setenv-sc {args} {
   lassign [parseSetenvCommandArgs load set {*}$args] bhv var val

   recordScanModuleElt [currentState modulename] $var setenv envvar

   if {![info exists ::env($var)]} {
      set ::env($var) {}
   }
   return {}
}

proc recordScanModuleElt {mod name args} {
   if {![info exists ::g_scanModuleElt]} {
      set ::g_scanModuleElt [dict create]
   }
   foreach elt $args {
      if {![dict exists $::g_scanModuleElt $elt]} {
         dict set ::g_scanModuleElt $elt {}
      }
      if {![dict exists $::g_scanModuleElt $elt $name]} {
         dict set ::g_scanModuleElt $elt $name [list $mod]
      } else {
         ##nagelfar ignore Suspicious variable name
         dict with ::g_scanModuleElt $elt {lappend $name $mod}
      }
      reportDebug "Module $mod defines $elt:$name"
   }
}

# test given variant specification matches what scanned module defines
proc doesModVariantMatch {mod pvrlist} {
   set ret 1
   if {[info exists ::g_scanModuleVariant($mod)]} {
      foreach availvr $::g_scanModuleVariant($mod) {
         set availvrarr([lindex $availvr 0]) [lindex $availvr 1]
         set availvrisbool([lindex $availvr 0]) [lindex $availvr 4]
      }
      foreach pvr $pvrlist {
         set pvrarr([lindex $pvr 0]) [lindex $pvr 1]
         set pvrisbool([lindex $pvr 0]) [lindex $pvr 2]
      }

      # no match if a specified variant is not found among module variants or
      # if the value is not available
      foreach vrname [array names pvrarr] {
         # if variant is a boolean, specified value should be a boolean too
         # any value accepted for free-value variant
         if {![info exists availvrarr($vrname)] || ($availvrisbool($vrname)\
            && !$pvrisbool($vrname)) || (!$availvrisbool($vrname) && [llength\
            $availvrarr($vrname)] > 0 && $pvrarr($vrname) ni\
            $availvrarr($vrname))} {
            set ret 0
            break
         }
      }
   } else {
      set ret 0
   }
   return $ret
}

# test given extra specifiers match what scanned module defines
proc doesModExtraMatch {mod pxtlist} {
   set ret 1
   if {[info exists ::g_scanModuleElt]} {
      foreach pxt $pxtlist {
         lassign $pxt elt name
         # does mod defines named element
         if {![dict exists $::g_scanModuleElt $elt $name] || $mod ni [dict\
            get $::g_scanModuleElt $elt $name]} {
            set ret 0
            break
         }
      }
   } else {
      set ret 0
   }
   return $ret
}

# determine if current module search requires an extra match search
proc isExtraMatchSearchRequired {mod} {
   # an extra match search is required if:
   # * variant should be reported in output
   # * mod specification contains variant during avail/paths/whatis
   # * mod specification contains extra specifier during avail/paths/whatis
   return [expr {[isEltInReport variant 0] || (([llength\
      [getVariantListFromVersSpec $mod]] + [llength [getExtraListFromVersSpec\
      $mod]]) > 0 && [currentState commandname] in {avail paths whatis})}]
}

# perform extra match search on currently being built module search result
proc filterExtraMatchSearch {mod res_arrname versmod_arrname} {
   # link to variables/arrays from upper context
   upvar $res_arrname found_list
   upvar $versmod_arrname versmod_list

   # get extra match query properties
   set spec_vr_list [getVariantListFromVersSpec $mod]
   set check_variant [expr {[llength $spec_vr_list] > 0}]
   set spec_xt_list [getExtraListFromVersSpec $mod]
   set check_extra [expr {[llength $spec_xt_list] > 0}]
   set filter_res [expr {$check_variant || $check_extra}]

   # disable error reporting to avoid modulefile errors (not coping with scan
   # evaluation for instance) to pollute result
   set alreadyinhibit [getState inhibit_errreport]
   if {!$alreadyinhibit} {
      inhibitErrorReport
   }

   set unset_list {}
   # evaluate all modules found in scan mode to gather content information
   lappendState mode scan
   foreach elt [array names found_list] {
      switch -- [lindex $found_list($elt) 0] {
         modulefile - virtual {
            # skip evaluation of fully forbidden modulefile
            if {![isModuleTagged $elt forbidden 0]} {
               ##nagelfar ignore Suspicious variable name
               execute-modulefile [lindex $found_list($elt) 2] $elt $elt $elt\
                  0 0
            }
         }
      }
      # unset elements that do not match extra query
      if {$filter_res} {
         switch -- [lindex $found_list($elt) 0] {
            alias {
               # module alias does not hold properties to match extra query
               lappend unset_list $elt
            }
            modulefile - virtual {
               if {($check_variant && ![doesModVariantMatch $elt\
                  $spec_vr_list]) || ($check_extra && ![doesModExtraMatch\
                  $elt $spec_xt_list])} {
                  lappend unset_list $elt
               }
            }
         }
      }
   }
   lpopState mode

   # unset marked elements
   foreach elt $unset_list {
      unset found_list($elt)
      # also unset any symbolic version pointing to unset elt
      if {[info exists versmod_list($elt)]} {
         foreach eltsym $versmod_list($elt) {
            # getModules phase #2 may have already withdrawn symbol
            if {[info exists found_list($eltsym)]} {
               unset found_list($eltsym)
            }
         }
      }
   }

   # re-enable error report only is it was disabled from this procedure
   if {!$alreadyinhibit} {
      setState inhibit_errreport 0
   }
}

# ;;; Local Variables: ***
# ;;; mode:tcl ***
# ;;; End: ***
# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
