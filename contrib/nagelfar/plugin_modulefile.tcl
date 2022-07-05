##Nagelfar Plugin : modulefile-specific notices
#
# PLUGIN_MODULEFILE.tcl, Nagelfar plugin to lint modulefiles
# Copyright (C) 2022 Xavier Delaruelle
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

# report deprecated/not implemented/not recommended usages
proc statementWords {words info} {
   set res {}
   switch [lindex $words 0] {
      module-log - module-trace - module-user - module-verbosity {
         set res [list warning "Command \"[lindex $words 0]\" is deprecated\
            and not implemented"]
      }
      module-info {
         if {[lindex $words 1] in {flags trace tracepat user}} {
            set res [list warning "Command \"[lrange $words 0 1]\" is\
               deprecated and not implemented"]
         }
      }
      extensions - add-property - remove-property {
         set res [list warning "Command \"[lindex $words 0]\" is not\
            implemented"]
      }
      module {
         if {[lindex $words 1] in {aliases available avail show display\
            initload initadd initclear initlist initprepend initunload initrm\
            initswitch list purge reload restore save savelist saverm\
            saveshow search test whatis}} {
            set res [list warning "Command \"[lrange $words 0 1]\" should not\
               be used in modulefile"]
         }
      }
   }
   return $res
}

# translate known '--option=value' arguments into '--option value' to
# correctly check them with Nagelfar
proc statementRaw {stmt info} {
   set res {}
   regexp {^(.+?)\s} $stmt match cmd
   if {$cmd in {append-path prepend-path remove-path always-load depends-on\
      module prereq prereq-all prereq-any}} {
      if {[regsub -all {(--(delim|tag))=} $stmt {\1 } newstmt] > 0} {
         set res [list replace $newstmt]
      }
   }
   return $res
}

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
