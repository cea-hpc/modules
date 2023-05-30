##########################################################################

# MODEVAL.TCL, module evaluation procedures
# Copyright (C) 2002-2004 Mark Lakata
# Copyright (C) 2004-2017 Kent Mein
# Copyright (C) 2016-2023 Xavier Delaruelle
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

# gather for the current top evaluation the information on all evaluations
# happening under its umbrella
proc registerModuleEval {context msgrecid {failedmod {}} {failedcontext {}}} {
   set evalid [topState evalid]
   set unset [expr {$failedmod eq {} ? 0 : 1}]
   set contextset 0

   # add msgrecid to existing evaluation context list
   if {[info exists ::g_moduleEval($evalid)]} {
      for {set i 0} {$i < [llength $::g_moduleEval($evalid)]} {incr i 1} {
         set contextevallist [lindex $::g_moduleEval($evalid) $i]
         if {[lindex $contextevallist 0] eq $context} {
            if {$unset} {
               set contextevallist [replaceFromList $contextevallist\
                  $msgrecid]
            } else {
               lappend contextevallist $msgrecid
            }
            set ::g_moduleEval($evalid) [expr {[llength $contextevallist] > 1\
               ? [lreplace $::g_moduleEval($evalid) $i $i $contextevallist]\
               : [lreplace $::g_moduleEval($evalid) $i $i]}]
            set contextset 1
            break
         }
      }
   }

   # add msgrecid to new evaluation context list
   if {!$unset && !$contextset} {
      lappend ::g_moduleEval($evalid) [list $context $msgrecid]
   }

   # add mod to failed evaluation list
   if {$unset} {
      lappend ::g_moduleFailedEval($evalid) $failedcontext $failedmod
   }
}

# record that module evaluation is set hidden
proc registerModuleEvalHidden {context msgrecid} {
   set evalid [topState evalid]
   lappend ::g_moduleHiddenEval($evalid:$context) $msgrecid
}

# get context of currently evaluated module
proc currentModuleEvalContext {} {
   return [lindex $::g_moduleEvalAttempt([currentState modulenamevr]) end]
}

# record module evaluation attempt and corresponding context
proc registerModuleEvalAttempt {context mod} {
   appendNoDupToList ::g_moduleEvalAttempt($mod) $context
}

proc unregisterModuleEvalAttempt {context mod} {
   set ::g_moduleEvalAttempt($mod) [replaceFromList\
      $::g_moduleEvalAttempt($mod) $context]
}

# is at least one module passed as argument evaluated in passed context
proc isModuleEvaluated {context exclmod args} {
   set ret 0
   # look at all evaluated mod except excluded one (currently evaluated mod)
   foreach evalmod [lsearch -all -inline -not [array names\
      ::g_moduleEvalAttempt] $exclmod] {
      set evalmatch 0
      # test arguments against all names of evaluated module (translate
      # eventual modspec in evalmod into module names, in case module
      # evaluation stopped prior module name setup)
      # retrieve variants to pass them directly to modEq
      set modvrlist [getVariantList $evalmod 0 0 1]
      foreach mod [getAllModulesFromVersSpec $evalmod] {
         foreach name $args {
            # indicate module is loading to also compare against all alt names
            if {[modEq $name $mod eqstart 1 1 1 $modvrlist]} {
               set evalmatch 1
               if {$context eq {any} || $context in\
                  $::g_moduleEvalAttempt($evalmod)} {
                  set ret 1
               }
               break
            }
         }
         if {$evalmatch} {
            break
         }
      }
      if {$ret} {
         break
      }
   }
   return $ret
}

# was passed mod already evaluated for context and failed
proc isModuleEvalFailed {context mod} {
   set ret 0
   set evalid [topState evalid]
   if {[info exists ::g_moduleFailedEval($evalid)]} {
      foreach {curcon curmod} $::g_moduleFailedEval($evalid) {
         if {$context eq $curcon && $mod eq $curmod} {
            set ret 1
            break
         }
      }
   }
   return $ret
}

# return list of currently loading modules in stack
proc getLoadingModuleList {} {
   set modlist [list]
   for {set i 0} {$i < [depthState modulename]} {incr i 1} {
      if {[lindex [getState mode] $i] eq {load}} {
         lappend modlist [lindex [getState modulename] $i]
      }
   }
   return $modlist
}

# return list of currently loading modulefiles in stack
proc getLoadingModuleFileList {} {
   set modlist [list]
   for {set i 0} {$i < [depthState modulefile]} {incr i 1} {
      if {[lindex [getState mode] $i] eq {load}} {
         lappend modlist [lindex [getState modulefile] $i]
      }
   }
   return $modlist
}

# return list of currently unloading modules in stack
proc getUnloadingModuleList {} {
   set modlist [list]
   for {set i 0} {$i < [depthState modulename]} {incr i 1} {
      if {[lindex [getState mode] $i] eq {unload}} {
         lappend modlist [lindex [getState modulename] $i]
      }
   }
   return $modlist
}

# sort passed module list following both loaded and dependency orders
proc sortModulePerLoadedAndDepOrder {modlist {nporeq 0} {loading 0}} {
   # sort per loaded order
   set sortlist {}
   if {[llength $modlist] > 0} {
      foreach lmmod [getLoadedModulePropertyList name] {
         if {$lmmod in $modlist} {
            lappend sortlist $lmmod
         }
      }
      # also sort eventual loading modules if asked
      if {$loading} {
         foreach loadingmod [lreverse [getLoadingModuleList]] {
            if {$loadingmod in $modlist} {
               lappend sortlist $loadingmod
            }
         }
      }
   }

   # then refine sort with dependencies between loaded modules: a dependent
   # module should be placed prior the loaded module requiring it
   set reqListVar [expr {$nporeq ? {::g_moduleNPODepend} :\
      {::g_moduleDepend}}]
   set i 0
   set imax [llength $sortlist]
   while {$i < $imax} {
      set mod [lindex $sortlist $i]
      set jmin $imax

      ##nagelfar ignore #4 Suspicious variable name
      if {[info exists ${reqListVar}($mod)]} {
         # goes over all dependent modules to find the first one in the loaded
         # order list located after requiring mod
         foreach lmmodlist [set ${reqListVar}($mod)] {
            foreach lmmod $lmmodlist {
               set j [lsearch -exact $sortlist $lmmod]
               if {$j > $i && $j < $jmin} {
                  set jmin $j
                  set jminmod $lmmod
               }
            }
         }
      }

      # move first dependent module found after currently inspected mod right
      # before it
      if {$jmin != $imax} {
         set sortlist [linsert [lreplace $sortlist $jmin $jmin] $i $jminmod]
      # or go to next element in list if current element has not been changed
      } else {
         incr i
      }
   }

   return $sortlist
}

# return list of loaded modules having an unmet requirement on passed mod
# and their recursive dependent
proc getUnmetDependentLoadedModuleList {modnamevr} {
   set unmetdeplist {}
   set depmodlist {}
   defineModEqProc [isIcase] [getConf extended_default] 1
   set mod [getModuleNameAndVersFromVersSpec $modnamevr]
   set vrlist [getVariantList $modnamevr 0 0 1]

   # skip dependent analysis if mod has a conflict with a loaded module
   lassign [doesModuleConflict $mod] doescon modconlist
   if {!$doescon} {
      foreach ummod [array names ::g_unmetDepHash] {
         if {[modEq $ummod $mod eqstart 1 2 1 $vrlist]} {
            foreach depmod $::g_unmetDepHash($ummod) {
               lappend depmodlist $depmod
               # temporarily remove prereq violation of depmod if mod
               # load solves it (no other prereq is missing)
               if {[info exists ::g_prereqViolation($depmod)]} {
                  foreach prereq $::g_prereqViolation($depmod) {
                     foreach modpre $prereq {
                        # also temporarily remove prereq violation for
                        # requirements loaded after dependent module
                        if {[modEq $modpre $mod eqstart 1 2 1 $vrlist] ||\
                           [is-loaded $modpre]} {
                           # backup original violation to restore it later
                           if {![info exists preunvioarr($depmod)]} {
                              set preunvioarr($depmod)\
                                 $::g_prereqViolation($depmod)
                           }
                           # temporarily remove matching violation
                           set ::g_prereqViolation($depmod) [replaceFromList\
                              $::g_prereqViolation($depmod) $prereq]
                           if {[llength $::g_prereqViolation($depmod)] == 0} {
                              unset ::g_prereqViolation($depmod)
                           }
                           break
                        }
                     }
                  }
               }
            }
         }
      }
   }

   # select dependent if all its constraint are now satisfied (after removing
   # eventual prereq violation toward mod)
   foreach depmod $depmodlist {
      if {[areModuleConstraintsSatisfied $depmod]} {
         appendNoDupToList unmetdeplist $depmod
      }
   }

   # get dependent of dependent
   set deplist [getDependentLoadedModuleList $unmetdeplist 0 0 0 0 1]

   # restore temporarily lift prereq violation
   if {[array exists preunvioarr]} {
      foreach depmod [array names preunvioarr] {
         set ::g_prereqViolation($depmod) $preunvioarr($depmod)
      }
   }

   set sortlist [sortModulePerLoadedAndDepOrder [list {*}$unmetdeplist\
      {*}$deplist]]
   reportDebug "got '$sortlist'"
   return $sortlist
}

# return list of loaded modules declaring a prereq on passed mod with
# distinction made with strong prereqs (no alternative loaded) or weak and
# also with prereq loaded after their dependent module
proc getDirectDependentList {mod {strong 0} {nporeq 0} {loading 0}\
   {othmodlist {}}} {
   set deplist {}

   # include or not requirements loaded after their dependent
   if {$nporeq} {
      set depListVar ::g_dependNPOHash
      set reqListVar ::g_moduleNPODepend
   } else {
      set depListVar ::g_dependHash
      set reqListVar ::g_moduleDepend
   }

   ##nagelfar ignore #2 Suspicious variable name
   if {[info exists ${depListVar}($mod)]} {
      foreach depmod [set ${depListVar}($mod)] {
         set add 1
         # skip optional dependency if only looking for strong ones
         # look at an additionally processed mod list to determine if all
         # mods from a dependent list (composed of optional parts) are part
         # of the search, which means mod is not optional but strong dependent
         if {$strong && [llength $depmod] > 1} {
            ##nagelfar ignore Suspicious variable name
            foreach lmmodlist [set ${reqListVar}([lindex $depmod 0])] {
               if {$mod in $lmmodlist} {
                  foreach lmmod $lmmodlist {
                     # other mod part of the opt list is not there so mod
                     # is considered optional
                     if {$lmmod ni $othmodlist} {
                        set add 0
                        break
                     }
                  }
                  break
               }
            }
         }

         if {$add} {
            lappend deplist [lindex $depmod 0]
         }
      }
   }

   # take currently loading modules into account if asked
   if {$loading} {
      set modlist [getLoadedModulePropertyList name]
      defineModEqProc [isIcase] [getConf extended_default] 1
      # reverse list to get closest match if returning lastly loaded module
      if {[getConf unload_match_order] eq {returnlast}} {
         set modlist [lreverse $modlist]
      }
      foreach loadingmod [getLoadingModuleList] {
         foreach prereq [getLoadedPrereq $loadingmod] {
            set lmprelist {}
            set moddep 0
            foreach modpre $prereq {
               foreach lmmod $modlist {
                  if {[modEq $modpre $lmmod eqstart 1 2 1]} {
                     lappend lmprelist $lmmod
                     if {$lmmod eq $mod} {
                        set moddep 1
                     }
                     break
                  }
               }
            }
            if {$moddep && (!$strong || [llength $lmprelist] == 1)} {
               lappend deplist $loadingmod
               break
            }
         }
      }
   }

   return $deplist
}

# gets the list of all loaded modules which are dependent of passed modlist
# ordered by load position. strong argument controls whether only the active
# dependent modules should be returned or also those that are optional. direct
# argument controls if only dependent module directly requiring passed mods
# should be returned or its full dependent tree. nporeq argument tells if
# requirement loaded after their dependent should be returned. sat_constraint
# argument controls whether only the loaded module satisfying their constraint
# should be part or not of the resulting list. being_unload argument controls
# whether loaded modules in conflict with one or multiple modules from modlist
# should be added to the dependent list as these modules are currently being
# unloaded and these conflicting loaded modules should be refreshed.
proc getDependentLoadedModuleList {modlist {strong 1} {direct 1} {nporeq 0}\
   {loading 1} {sat_constraint 0} {being_unload 0}} {
   reportDebug "get loaded mod dependent of '$modlist' (strong=$strong,\
      direct=$direct, nporeq=$nporeq, loading=$loading,\
      sat_constraint=$sat_constraint, being_unload=$being_unload)"

   set deplist {}
   set fulllist $modlist
   # look at consistent requirements for unloading modules
   set unlonporeq [expr {$being_unload ? 0 : $nporeq}]
   foreach mod $modlist {
      # no duplicates or modules from query list
      appendNoDupToList fulllist {*}[getDirectDependentList $mod $strong\
         $unlonporeq $loading $fulllist]
   }

   if {$being_unload} {
      # invite modules in violation with mods to be part of the dependent list
      # with their own dependent modules as mod is being unloaded. Achieve so
      # by faking that conflict violation is gone
      foreach mod $modlist {
         lassign [doesModuleConflict $mod] doescon modconlist
         if {$doescon} {
            unsetModuleConflictViolation $mod
            set conunvioarr($mod) $modconlist
            appendNoDupToList fulllist {*}$modconlist
         }
      }
   }
   set unloadingmodlist [getUnloadingModuleList]
   for {set i [llength $modlist]} {$i < [llength $fulllist]} {incr i 1} {
      set depmod [lindex $fulllist $i]

      # skip already added mod or mod violating constraints if asked
      if {!$sat_constraint || [areModuleConstraintsSatisfied $depmod\
         $nporeq]} {
         # get dependent mod of dep mod when looking at full dep tree
         if {!$direct} {
            appendNoDupToList fulllist {*}[getDirectDependentList $depmod\
               $strong $nporeq 0 $fulllist]
         }
         # avoid module currently unloading from result list
         if {$depmod ni $unloadingmodlist} {
            lappend deplist $depmod
         }
      }
   }

   # restore conflict violation if any
   if {[array exists conunvioarr]} {
      foreach conunvio [array names conunvioarr] {
         setModuleConflictViolation $conunvio $conunvioarr($conunvio)
      }
   }

   # sort complete result list to match both loaded and dependency orders
   set sortlist [sortModulePerLoadedAndDepOrder $deplist $nporeq $loading]
   reportDebug "got '$sortlist'"
   return $sortlist
}

# test if passed 'mod' could be automatically unloaded or not, which means it
# has been loaded automatically and no loaded modules require it anymore.
# unmodlist: pass a list of modules that are going to be unloaded
proc isModuleUnloadable {mod {unmodlist {}}} {
   set ret 1
   # get currently unloading modules if no specific unmodlist set
   if {[llength $unmodlist] == 0} {
      set unmodlist [getUnloadingModuleList]
   }

   # loaded tag means module was not auto-loaded. keep-loaded means module is
   # not auto-unloadable even if auto-loaded
   if {[isModuleTagged $mod loaded 1] || [isModuleTagged $mod keep-loaded\
      1]} {
      set ret 0
   } else {
      # mod is unloadable if all its dependent are unloaded or unloading
      foreach depmod [getDirectDependentList $mod] {
         if {$depmod ni $unmodlist} {
            set ret 0
            break
         }
      }
   }

   return $ret
}

# gets the list of all loaded modules which are required by passed modlist
# ordered by load position.
proc getRequiredLoadedModuleList {modlist} {
   # search over all list of loaded modules, starting with passed module
   # list, then adding in turns their requirements
   set fulllist $modlist
   for {set i 0} {$i < [llength $fulllist]} {incr i 1} {
      # gets the list of loaded modules which are required by depmod
      appendNoDupToList fulllist {*}$::g_moduleDepend([lindex $fulllist $i])
   }

   # sort complete result list to match both loaded and dependency orders
   set sortlist [sortModulePerLoadedAndDepOrder [lrange $fulllist [llength\
      $modlist] end]]
   reportDebug "got '$sortlist'"
   return $sortlist
}

# finds required modules that can be unloaded if passed modules are unloaded:
# they have been loaded automatically and are not depended (mandatory or
# optionally) by other module
proc getUnloadableLoadedModuleList {modlist} {
   # search over all list of unloaded modules, starting with passed module
   # list, then adding in turns unloadable requirements
   set fulllist $modlist
   for {set i 0} {$i < [llength $fulllist]} {incr i 1} {
      set depmod [lindex $fulllist $i]
      # gets the list of loaded modules which are required by depmod
      set deplist {}
      foreach lmmodlist $::g_moduleDepend($depmod) {
         foreach lmmod $lmmodlist {
            if {$lmmod ni $fulllist} {
               lappend deplist $lmmod
            }
         }
      }

      # get those required module that have been automatically loaded and are
      # only required by modules currently being unloaded
      foreach lmmod $deplist {
         if {[isModuleUnloadable $lmmod $fulllist]} {
            lappend fulllist $lmmod
         }
      }
   }

   # sort complete result list to match both loaded and dependency orders
   set sortlist [sortModulePerLoadedAndDepOrder [lrange $fulllist [llength\
      $modlist] end]]
   reportDebug "got '$sortlist'"
   return $sortlist
}

# how many settings bundle are currently saved
proc getSavedSettingsStackDepth {} {
   return [llength $::g_SAVE_g_loadedModules]
}

# manage settings to save as a stack to have a separate set of settings
# for each module loaded or unloaded in order to be able to restore the
# correct set in case of failure
proc pushSettings {} {
   foreach var {env g_clearedEnvVars g_Aliases g_stateEnvVars g_stateAliases\
      g_stateFunctions g_Functions g_stateCompletes g_Completes\
      g_newXResources g_delXResources g_loadedModules g_loadedModuleFiles\
      g_loadedModuleVariant g_loadedModuleConflict g_loadedModulePrereq\
      g_loadedModulesRefresh g_loadedModuleAltname g_loadedModuleAutoAltname\
      g_loadedModuleAliasAltname g_moduleDepend g_dependHash\
      g_moduleNPODepend g_dependNPOHash g_prereqViolation\
      g_prereqNPOViolation g_conflictViolation g_moduleUnmetDep\
      g_unmetDepHash g_moduleEval g_moduleHiddenEval g_scanModuleVariant} {
      ##nagelfar ignore Suspicious variable name
      lappend ::g_SAVE_$var [array get ::$var]
   }
   # save non-array variable and indication if it was set
   foreach var {g_changeDir g_stdoutPuts g_prestdoutPuts g_return_text} {
      ##nagelfar ignore #4 Suspicious variable name
      if {[info exists ::$var]} {
         lappend ::g_SAVE_$var [list 1 [set ::$var]]
      } else {
         lappend ::g_SAVE_$var [list 0 {}]
      }
   }
   reportDebug "settings saved (#[getSavedSettingsStackDepth])"
}

proc popSettings {} {
   set flushedid [getSavedSettingsStackDepth]
   foreach var {env g_clearedEnvVars g_Aliases g_stateEnvVars g_stateAliases\
      g_stateFunctions g_Functions g_stateCompletes g_Completes\
      g_newXResources g_delXResources g_changeDir g_stdoutPuts\
      g_prestdoutPuts g_return_text g_loadedModules g_loadedModuleFiles\
      g_loadedModuleVariant g_loadedModuleConflict g_loadedModulePrereq\
      g_loadedModulesRefresh g_loadedModuleAltname g_loadedModuleAutoAltname\
      g_loadedModuleAliasAltname g_moduleDepend g_dependHash\
      g_moduleNPODepend g_dependNPOHash g_prereqViolation\
      g_prereqNPOViolation g_conflictViolation g_moduleUnmetDep\
      g_unmetDepHash g_moduleEval g_moduleHiddenEval g_scanModuleVariant} {
      ##nagelfar ignore Suspicious variable name
      set ::g_SAVE_$var [lrange [set ::g_SAVE_$var] 0 end-1]
   }
   reportDebug "previously saved settings flushed (#$flushedid)"
}

proc restoreSettings {} {
   foreach var {g_clearedEnvVars g_Aliases g_stateEnvVars g_stateAliases\
      g_stateFunctions g_Functions g_stateCompletes g_Completes\
      g_newXResources g_delXResources g_loadedModules g_loadedModuleFiles\
      g_loadedModuleVariant g_loadedModuleConflict g_loadedModulePrereq\
      g_loadedModulesRefresh g_loadedModuleAltname g_loadedModuleAutoAltname\
      g_loadedModuleAliasAltname g_moduleDepend g_dependHash\
      g_moduleNPODepend g_dependNPOHash g_prereqViolation\
      g_prereqNPOViolation g_conflictViolation g_moduleUnmetDep\
      g_unmetDepHash g_moduleEval g_moduleHiddenEval g_scanModuleVariant} {
      # clear current $var arrays
      ##nagelfar ignore #5 Suspicious variable name
      if {[info exists ::$var]} {
         unset ::$var
         array set ::$var {}
      }
      array set ::$var [lindex [set ::g_SAVE_$var] end]
   }
   # specific restore mechanism for ::env as unsetting this array will make
   # Tcl stop monitoring env accesses and not update env variables anymore
   set envvarlist [list]
   foreach {var val} [lindex $::g_SAVE_env end] {
      lappend envvarlist $var
      interp-sync-env set $var $val
   }
   foreach var [array names ::env] {
      if {$var ni $envvarlist} {
         interp-sync-env unset $var
      }
   }
   # restore non-array variable if it was set
   foreach var {g_changeDir g_stdoutPuts g_prestdoutPuts g_return_text} {
      ##nagelfar ignore #6 Suspicious variable name
      if {[info exists ::$var]} {
         unset ::$var
      }
      lassign [lindex [set ::g_SAVE_$var] end] isdefined val
      if {$isdefined} {
         set ::$var $val
      }
   }
   reportDebug "previously saved settings restored\
      (#[getSavedSettingsStackDepth])"
}

# load modules passed as args designated as requirement
proc loadRequirementModuleList {tryload optional tag_list args} {
   set ret 0
   set prereqloaded 0

   # calling procedure must have already parsed module specification in args
   set loadedmod_list {}
   foreach mod $args {
      # get all loaded or loading mod in args list
      if {[set loadedmod [getLoadedMatchingName $mod returnfirst]] ne {} ||\
         [set loadedmod [getLoadedMatchingName $mod returnfirst 1]] ne {}} {
         lappend loadedmod_list $loadedmod
      }
   }

   if {[llength $loadedmod_list] == 0} {
      set imax [llength $args]
      # if prereq list specified, try to load first then
      # try next if load of first module not successful
      for {set i 0} {$i<$imax && $prereqloaded==0} {incr i 1} {
         set arg [lindex $args $i]

         # hold output of each evaluation until they are all done to drop
         # those that failed if one succeed or if optional
         set curholdid load-$i-$arg
         lappendState reportholdid $curholdid
         if {[catch {set retlo [cmdModuleLoad reqlo 0 $tryload 0 $tag_list\
            $arg]} errorMsg]} {
            # if an error is raised, release output and rethrow the error
            # (could be raised if no modulepath defined for instance)
            lpopState reportholdid
            lappend holdidlist $curholdid report
            releaseHeldReport {*}$holdidlist
            knerror $errorMsg
         }
         # update return value if an issue occurred in cmdModuleLoad
         if {$retlo != 0} {
            set ret $retlo
         }
         lpopState reportholdid

         if {[is-loaded $arg]} {
            set prereqloaded 1
            # set previous reports to be dropped as this one succeed
            if {[info exists holdidlist]} {
               foreach {holdid action} $holdidlist {
                  lappend newholdidlist $holdid drop
               }
               set holdidlist $newholdidlist
            }
         }
         # drop report if not loaded and optional
         set action [expr {$prereqloaded || !$optional ? {report} : {drop}}]
         lappend holdidlist $curholdid $action
      }
      # output held messages
      releaseHeldReport {*}$holdidlist
   } else {
      set prereqloaded 1
      # apply missing tag to all loaded module found
      cmdModuleTag 0 0 $tag_list {*}$loadedmod_list
   }

   return [list $ret $prereqloaded]
}

# unload phase of a list of modules reload process
proc reloadModuleListUnloadPhase {lmname {force 0} {errmsgtpl {}} {context\
   unload}} {
   upvar $lmname lmlist
   # unload one by one to ensure same behavior whatever auto_handling state
   foreach mod [lreverse $lmlist] {
      # record hint that mod will be reloaded (useful in case mod is sticky)
      lappendState reloading_sticky $mod
      lappendState reloading_supersticky $mod
      # save user asked state before it vanishes
      set isuasked($mod) [isModuleTagged $mod loaded 1]
      # save variants set for modules
      set vr($mod) [getVariantList $mod 1 2]
      # save extra tags set
      set extratag($mod) [getExtraTagList $mod]
      # force unload even if requiring mods are not part of the unload list
      # (violation state) as modules are loaded again just after
      if {[cmdModuleUnload $context match 0 1 0 0 $mod]} {
         # avoid failing module on load phase
         set lmlist [replaceFromList $lmlist $mod]
         set errMsg [string map [list _MOD_ [getModuleDesignation loaded\
            $mod]] $errmsgtpl]
         if {$force} {
            # errMsg will always be set as force mode could not be enabled
            # for reload sub-cmd which provides an empty msg template
            reportWarning $errMsg 1
         # stop if one unload fails unless force mode enabled
         } else {
            lpopState reloading_sticky
            lpopState reloading_supersticky
            knerror $errMsg
         }
      }
      lpopState reloading_sticky
      lpopState reloading_supersticky
   }
   return [list [array get isuasked] [array get vr] [array get extratag]]
}

# load phase of a list of modules reload process
proc reloadModuleListLoadPhase {lmname isuaskedlist vrlist extrataglist\
   {force 0} {errmsgtpl {}} {context load}} {
   upvar $lmname lmlist
   array set isuasked $isuaskedlist
   array set vr $vrlist
   array set extratag $extrataglist

   # loads are made with auto handling mode disabled to avoid disturbances
   # from a missing prereq automatically reloaded, so these module loads may
   # fail as prereq may not be satisfied anymore
   setConf auto_handling 0
   foreach mod $lmlist {
      # if an auto set default was excluded, module spec need parsing
      lassign [parseModuleSpecification 0 0 0 0 $mod {*}$vr($mod)] modnamevr
      # reload module with user asked property and extra tags preserved
      if {[cmdModuleLoad $context $isuasked($mod) 0 0 $extratag($mod)\
         $modnamevr]} {
         set errMsg [string map [list _MOD_ [getModuleDesignation spec\
            $modnamevr]] $errmsgtpl]
         if {$force} {
            # errMsg will always be set as force mode could not be enabled
            # for reload sub-cmd which provides an empty msg template
            reportWarning $errMsg 1
         # stop if one load fails unless force mode enabled
         } else {
            knerror $errMsg
         }
      }
   }
   setConf auto_handling 1
}

# test if loaded module 'mod' is sticky and if stickiness definition applies
# to one of the reloading module
proc isStickinessReloading {mod reloading_modlist {tag sticky}} {
   set res 0
   set modname [getModuleNameAndVersFromVersSpec $mod]
   if {[isModuleTagged $modname $tag 1]} {
      # evaluate the module-tag commands that are related to mod
      set modfile [getModulefileFromLoadedModule $modname]
      set dir [getModulepathFromModuleName $modfile $modname]
      getModules $dir $modname 0 [list rc_defs_included]

      # tag set over full path module designation only applies to fully
      # qualified module
      set tmodspec [getModuleTag $mod $modfile $tag equal 1]
      # if tag specifically applies to fully qualified module, exact same
      # module should be found in reload list
      if {$tmodspec ne {}} {
         set tmodspec $mod
      } else {
         set tmodspec [getModuleTag $mod {} $tag eqstart 1]
      }

      # check if a loading mod satisfies sticky rules
      foreach modlo $reloading_modlist {
         if {[modEq $tmodspec $modlo eqstart 1 0 1]} {
            # found match
            set res 1
            break
         }
      }
      reportDebug "stickiness ($tag) applies to '$tmodspec', is\
         reloading=$res"
   }

   return $res
}

# ;;; Local Variables: ***
# ;;; mode:tcl ***
# ;;; End: ***
# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
