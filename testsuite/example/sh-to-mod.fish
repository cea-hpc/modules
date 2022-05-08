# example shell script to test 'sh-to-mod' sub-command

if set -q TESTSUITE_SHTOMOD_SHELLVAR
   set -xg FOOSHELL $SHELL
end

if not set -q TESTSUITE_SHTOMOD_NOVAR
   set -xg FOO value
   set -xg FOOEM ""
   set -xg FOOSP "value "
   set -xg FOOCB va\{ue
   set -xg FOOWC va\*ue
   if test (count $argv) -gt 0
      for i in (seq (count $argv))
         set -xg FOOARG$i $argv[$i]
      end
   end
end

if not set -q TESTSUITE_SHTOMOD_NOPATH
   set -xg FOOPATH /path/to/dir1 /path/to/dir2 /path/to/dir3
   set -xg FOOPATHEM '' /path/to/dir1 /path/to/dir2 /path/to/dir3
   set -xg FOOPATHSP "/path/to/dir1 /path/to/dir2 /path/to/dir3"
   set -xg FOOPATHSPEM "/path/to/dir1 /path/to/dir2 /path/to/dir3 "
   set -xg FOOPATHCB /path/to/dir1 /path/to/d\{r2 /path/to/dir3
   set -xg FOOPATHWC /path/to/dir1 /path/to/d\*r2 /path/to/dir3
end

if set -q TESTSUITE_SHTOMOD_PATHDUP
   set -xg FOOPATHDUP "/path/to/dir1:/path/to/dir1:/path/to/dir2:/path/to/dir1:/path/to/dir3:/path/to dir4:/path/to/dir3"
   set -xg FOOPATHDUPEM :/path/to/dir1:/path/to/dir1:/path/to/dir2:/path/to/dir1:/path/to/dir3::/path/to/dir3
   set -xg FOOPATHDUPSP "/path/to/dir1 /path/to/dir1 /path/to/dir2 /path/to/dir1 /path/to/dir3 /path/to:dir4 /path/to/dir3"
   set -xg FOOPATHDUPSPEM "/path/to/dir1 /path/to/dir1 /path/to/dir2 /path/to/dir1 /path/to/dir3 /path/to/dir3 "
   set -xg FOOPATHDUPMIX ":/path/to/dir1:/path/to/dir1 /path/to/dir2 /path/to/dir1:/path/to/dir3:/path/to/dir3:"
end

if set -q TESTSUITE_SHTOMOD_FUZZYOUT1
   echo '%ModulesShToMod%'
end
if set -q TESTSUITE_SHTOMOD_FUZZYOUT2
   function echo
      true
   end
end

if not set -q TESTSUITE_SHTOMOD_NOALIAS
   alias alfoo='echo (grep "report .Modules " ../../modulecmd.tcl | tr -d \\\\ 2>/dev/null | awk \'{print $3}\')'
   alias alcb='echo f\{o'
   # alias with empty body cannot be set on fish shell
   #alias alem=''
   alias alsp="echo f\\\"o; echo b\\\\\\\"r; echo f\'o"
end

if not set -q TESTSUITE_SHTOMOD_NOFUNC
   function funcfoo
      echo foo
   end
   function funcnl
      echo foo
      echo bar
   end
   function funccb
      echo f\{o
   end
   function funcsp
      echo f\"o
      echo b\\\"r
      echo f\'o
   end
   function funcwc
      echo sou*sh
   end
   function func_nested
      function func_in
         echo foo
      end
      func_in
   end
end

if not set -q TESTSUITE_SHTOMOD_NOCOMP
   complete -c cmd -s V -l version --description 'Command version'
   complete --command cmd -s h -l help --description 'Command help'
   complete --no-files --require-parameter -c mycmd -s h --description 'Command help'
   # test complete definition without -c/--complete opt arg on fish>=3.2
   if test (string sub -s 1 -l 1 $version) -ge 3; and test (string sub -s 3 -l 1 $version) -ge 2
      complete -f othercmd -s h -l help --description 'Command help'
      complete othercmd -s V -l version --description 'Command version'
   else
      complete -f --command othercmd -s h -l help --description 'Command help'
      complete -c othercmd -s V -l version --description 'Command version'
   end
end

if not set -q TESTSUITE_SHTOMOD_NOCD
   cd $TESTSUITEDIR/modulefiles.2
end

if set -q TESTSUITE_SHTOMOD_OUTMSG
   echo "output message"
end

if set -q TESTSUITE_SHTOMOD_ERRMSG
   echo "error message" >&2
end

if set -q TESTSUITE_SHTOMOD_EXIT1
   exit 1
end

if set -q TESTSUITE_SHTOMOD_UNSETENV
   set -e TESTSUITE_SHTOMOD_UNSETENV
end

if set -q TESTSUITE_SHTOMOD_EMPTYAPPEND
   set -xg TESTSUITE_SHTOMOD_EMPTYAPPEND $TESTSUITE_SHTOMOD_EMPTYAPPEND:
end

if set -q TESTSUITE_SHTOMOD_EMPTYPREPEND
   set -xg TESTSUITE_SHTOMOD_EMPTYPREPEND :$TESTSUITE_SHTOMOD_EMPTYPREPEND
end

if set -q TESTSUITE_SHTOMOD_MODULE
   eval $TCLSH $MODULES_CMD fish autoinit | source -
   module config collection_target bar
   module load setenv/1.0
   functions -e ml module _module_raw
   set -e MODULESHOME
end

if set -q TESTSUITE_SHTOMOD_UNSETALFUNCCOMP
   functions -e alfoo
   functions -e funcfoo
   complete -e -c cmd
end

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
