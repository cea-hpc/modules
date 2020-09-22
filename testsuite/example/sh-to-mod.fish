# example shell script to test 'sh-to-mod' sub-command

if set -q TESTSUITE_SHTOMOD_SHELLVAR
   set -xg FOOSHELL $SHELL
end

if not set -q TESTSUITE_SHTOMOD_NOVAR
   set -xg FOO value
   set -xg FOOEM ""
   set -xg FOOSP "value "
   set -xg FOOCB va\{ue
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

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
