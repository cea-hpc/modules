##########################################################################

# COLL.TCL, collection management procedures
# Copyright (C) 2016-2022 Xavier Delaruelle
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

# build list of what to undo then do to move from an initial list to a target
# list, eventually checking element presence in extra from/to lists
proc getMovementBetweenList {from to {extfrom {}} {extto {}} {cmp eq}} {
   reportDebug "from($from) to($to) with extfrom($extfrom) extto($extto)"

   set undo {}
   set do {}

   # determine what element to undo then do
   # to restore a target list from a current list
   # with preservation of the element order
   set imax [if {[llength $to] > [llength $from]} {llength $to} {llength\
      $from}]
   set list_equal 1
   for {set i 0} {$i < $imax} {incr i} {
      set to_obj [lindex $to $i]
      set from_obj [lindex $from $i]
      # check from/to element presence in extra from/to list
      set in_extfrom [expr {$from_obj in $extfrom}]
      set in_extto [expr {$to_obj in $extto}]
      # are elts the sames and are both part of or missing from extra lists
      # when comparing modules, ask comparison against loaded module
      # alternative and simplified names (modEq will also compare variants)
      if {($cmp eq {modeq} && ![modEq $to_obj $from_obj equal 1 3 1]) ||\
         ($cmp eq {eq} && $to_obj ne $from_obj) || $in_extfrom != $in_extto} {
         set list_equal 0
      }
      if {$list_equal == 0} {
         if {$to_obj ne {}} {
            lappend do $to_obj
         }
         if {$from_obj ne {}} {
            lappend undo $from_obj
         }
      }
   }

   return [list $undo $do]
}

# build list of currently loaded modules where modulename is registered minus
# module version if loaded version is the default one
proc getSimplifiedLoadedModuleList {} {
   set curr_mod_list {}
   array set curr_tag_arr {}
   set modpathlist [getModulePathList]
   foreach mod [getLoadedModuleList] {
      set altandsimplist [getLoadedAltAndSimplifiedName $mod]

      set parentmod [file dirname $mod]
      set simplemod $mod
      # simplify to parent name as long as it is found in simplified name list
      while {$parentmod ne {.}} {
         if {$parentmod in $altandsimplist} {
            set simplemod $parentmod
            set parentmod [file dirname $parentmod]
         } else {
            set parentmod .
         }
      }

      # add each module specification as list to correctly enclose spaces in
      # module name or variant name or value
      set simplemodvr [list $simplemod {*}[getVariantList $mod 5 1]]
      lappend curr_mod_list $simplemodvr
      # record tags applying to module in simplified version form
      set curr_tag_arr($simplemodvr) [getSaveTagList $mod]
   }

   return [list $curr_mod_list [array get curr_tag_arr]]
}

# return saved collections found in user directory which corresponds to
# enabled collection target if any set.
proc findCollections {} {
   if {[info exists ::env(HOME)]} {
      set coll_search $::env(HOME)/.module/*
   } else {
      reportErrorAndExit {HOME not defined}
   }

   # find saved collections (matching target suffix)
   # a target is a domain on which a collection is only valid.
   # when a target is set, only the collections made for that target
   # will be available to list and restore, and saving will register
   # the target footprint
   set colltarget [getConf collection_target]
   if {$colltarget ne {}} {
      append coll_search .$colltarget
   }

   # glob excludes by default files starting with "."
   if {[catch {set coll_list [glob -nocomplain $coll_search]} errMsg]} {
      reportErrorAndExit "Cannot access collection directory.\n$errMsg"
   }

   return $coll_list
}

# get filename corresponding to collection name provided as argument.
# name provided may already be a file name. collection description name
# (with target info if any) is returned along with collection filename
proc getCollectionFilename {coll} {
   # initialize description with collection name
   set colldesc $coll

   if {$coll eq {}} {
      reportErrorAndExit [getEmptyNameMsg collection]
   # is collection a filepath
   } elseif {[string first / $coll] > -1} {
      # collection target has no influence when
      # collection is specified as a filepath
      set collfile $coll
   # elsewhere collection is a name
   } elseif {[info exists ::env(HOME)]} {
      set collfile $::env(HOME)/.module/$coll
      # if a target is set, append the suffix corresponding
      # to this target to the collection file name
      set colltarget [getConf collection_target]
      if {$colltarget ne {}} {
         append collfile .$colltarget
         # add knowledge of collection target on description
         append colldesc " (for target \"$colltarget\")"
      }
   } else {
      reportErrorAndExit {HOME not defined}
   }

   return [list $collfile $colldesc]
}

# generate collection content based on provided path and module lists
proc formatCollectionContent {path_list mod_list tag_arrser {sgr 0}} {
   set content {}
   array set tag_arr $tag_arrser

   # graphically enhance module command if asked
   set modcmd [expr {$sgr ? [sgr cm module] : {module}}]

   # start collection content with modulepaths
   foreach path $path_list {
      # enclose path if space character found in it
      if {[string first { } $path] != -1} {
         set path "{$path}"
      }
      # 'module use' prepends paths by default so we clarify
      # path order here with --append flag
      append content "$modcmd use --append $path" \n
   }

   # then add modules
   foreach mod $mod_list {
      # save tags associated to module (like auto-loaded tag)
      if {[info exists tag_arr($mod)] && [llength $tag_arr($mod)] > 0} {
         set opt "--tag=[join $tag_arr($mod) :] "
      } else {
         set opt {}
      }
      # no need to specifically enclose module specification if space char
      # used in it as $mod is a list so elements including space will be
      # automatically enclosed
      append content "$modcmd load $opt$mod" \n
   }

   return $content
}

# read given collection file and return the path and module lists it defines
proc readCollectionContent {collfile colldesc} {
   # init lists (maybe coll does not set mod to load)
   set path_list {}
   set mod_list {}
   set nuasked_list {}
   array set tag_arr {}

   # read file
   if {[catch {
      set fdata [split [readFile $collfile] \n]
   } errMsg ]} {
      reportErrorAndExit "Collection $colldesc cannot be read.\n$errMsg"
   }

   # analyze collection content
   foreach fline $fdata {
      if {[regexp {module use (.*)$} $fline match patharg] == 1} {
         # paths are appended by default
         set stuff_path append
         # manage multiple paths and path options specified on single line,
         # for instance "module use --append path1 path2 path3", with list
         # representation of patharg (which handles quoted elements containing
         # space in their name)
         foreach path $patharg {
            # following path is asked to be appended
            if {($path eq {--append}) || ($path eq {-a})\
               || ($path eq {-append})} {
               set stuff_path append
            # following path is asked to be prepended
            # collection generated with 'save' does not prepend
            } elseif {($path eq {--prepend}) || ($path eq {-p})\
               || ($path eq {-prepend})} {
               set stuff_path prepend
            } else {
               # ensure given path is absolute to be able to correctly
               # compare with paths registered in MODULEPATH
               set path [getAbsolutePath $path]
               # add path to end of list
               if {$stuff_path eq {append}} {
                  lappend path_list $path
               # insert path to first position
               } else {
                  set path_list [linsert $path_list 0 $path]
               }
            }
         }
      } elseif {[regexp {module load (.*)$} $fline match modarg] == 1} {
         # extract collection-specific flags from module specification
         switch -glob -- [lindex $modarg 0] {
            --notuasked {
               set tag_list [list auto-loaded]
               set cleanlist [lrange $modarg 1 end]
            }
            --tag=* {
               set tag_list [split [string range [lindex $modarg 0] 6 end] :]
               set cleanlist [lrange $modarg 1 end]
            }
            default {
               set tag_list {}
               set cleanlist $modarg
            }
         }
         # parse module specification to distinguish between module + variant
         # specified and multiple modules specified on a single line
         set parsedlist [parseModuleSpecification 0 {*}$cleanlist]
         foreach parsed $parsedlist {
            set tag_arr($parsed) $tag_list
         }
         lappend mod_list {*}$parsedlist
      }
   }
   return [list $path_list $mod_list [array get tag_arr]]
}

# ;;; Local Variables: ***
# ;;; mode:tcl ***
# ;;; End: ***
# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
