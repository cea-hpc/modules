# example shell script to test 'sh-to-mod' sub-command

if [ "${TESTSUITE_SHTOMOD_SHELLVAR:+x}" = 'x' ]; then
   export FOOSHELL=$BASH
fi

if [ "${TESTSUITE_SHTOMOD_NOVAR:-x}" = 'x' ]; then
   export FOO=value
   export FOOEM=""
   export FOOSP="value "
   export FOOCB=va{ue
   export FOOWC=va\*ue
   i=1
   while [ $# -gt 0 ]; do
      export FOOARG$i="$1"
      shift
      let i++
   done
fi

if [ "${TESTSUITE_SHTOMOD_NOPATH:-x}" = 'x' ]; then
   export FOOPATH=/path/to/dir1:/path/to/dir2:/path/to/dir3
   export FOOPATHEM=:/path/to/dir1:/path/to/dir2:/path/to/dir3
   export FOOPATHSP="/path/to/dir1 /path/to/dir2 /path/to/dir3"
   export FOOPATHSPEM="/path/to/dir1 /path/to/dir2 /path/to/dir3 "
   export FOOPATHCB=/path/to/dir1:/path/to/d{r2:/path/to/dir3
   export FOOPATHWC=/path/to/dir1:/path/to/d\*r2:/path/to/dir3
fi

if [ "${TESTSUITE_SHTOMOD_PATHDUP:+x}" = 'x' ]; then
   export FOOPATHDUP="/path/to/dir1:/path/to/dir1:/path/to/dir2:/path/to/dir1:/path/to/dir3:/path/to dir4:/path/to/dir3"
   export FOOPATHDUPEM=:/path/to/dir1:/path/to/dir1:/path/to/dir2:/path/to/dir1:/path/to/dir3::/path/to/dir3
   export FOOPATHDUPSP="/path/to/dir1 /path/to/dir1 /path/to/dir2 /path/to/dir1 /path/to/dir3 /path/to:dir4 /path/to/dir3"
   export FOOPATHDUPSPEM="/path/to/dir1 /path/to/dir1 /path/to/dir2 /path/to/dir1 /path/to/dir3 /path/to/dir3 "
   export FOOPATHDUPMIX=":/path/to/dir1:/path/to/dir1 /path/to/dir2 /path/to/dir1:/path/to/dir3:/path/to/dir3:"
fi

if [ "${TESTSUITE_SHTOMOD_FUZZYOUT1:+x}" = 'x' ]; then
   echo %ModulesShToMod%
fi
if [ "${TESTSUITE_SHTOMOD_FUZZYOUT2:+x}" = 'x' ]; then
   echo() {
      :
   }
fi

if [ "${TESTSUITE_SHTOMOD_NOALIAS:-x}" = 'x' ]; then
   alias alfoo='echo $(grep "report .Modules " ../../modulecmd.tcl | tr -d \\ 2>/dev/null | awk '"'"'{print $3}'"'"')'
   alias alcb='echo f{o'
   alias alem=''
   alias alsp="echo f\\\"o; echo b\\\\\\\"r; echo f\'o"
fi

if [ "${TESTSUITE_SHTOMOD_NOFUNC:-x}" = 'x' ]; then
   funcfoo() {
      echo foo
   }
   funcnl() {
      echo foo
      echo bar
   }
   funccb() {
      echo f{o
   }
   funcsp() {
      echo f\"o
      echo b\\\"r
      echo f\'o
   }
   funcwc() {
      echo sou*sh
   }
fi

if [ "${TESTSUITE_SHTOMOD_NOCOMP:-x}" = 'x' ]; then
   complete -o default -F _cmd cmd
   complete -j -P '"%' -S '"' mycmd
   complete -u othercmd
fi

if [ "${TESTSUITE_SHTOMOD_NOCD:-x}" = 'x' ]; then
   cd $TESTSUITEDIR/modulefiles.2 
fi

if [ "${TESTSUITE_SHTOMOD_OUTMSG:+x}" = 'x' ]; then
   echo "output message"
fi

if [ "${TESTSUITE_SHTOMOD_ERRMSG:+x}" = 'x' ]; then
   echo "error message" >&2
fi

if [ "${TESTSUITE_SHTOMOD_EXIT1:+x}" = 'x' ]; then
   exit 1
fi

if [ "${TESTSUITE_SHTOMOD_UNSETENV:+x}" = 'x' ]; then
   unset TESTSUITE_SHTOMOD_UNSETENV
fi

if [ "${TESTSUITE_SHTOMOD_EMPTYAPPEND:+x}" = 'x' ]; then
   TESTSUITE_SHTOMOD_EMPTYAPPEND=$TESTSUITE_SHTOMOD_EMPTYAPPEND:
fi

if [ "${TESTSUITE_SHTOMOD_EMPTYPREPEND:+x}" = 'x' ]; then
   TESTSUITE_SHTOMOD_EMPTYPREPEND=:$TESTSUITE_SHTOMOD_EMPTYPREPEND
fi

if [ "${TESTSUITE_SHTOMOD_MODULE:+x}" = 'x' ]; then
   eval $($TCLSH $MODULES_CMD sh autoinit)
   module config collection_target bar
   module load setenv/1.0
   unset -f ml module _module_raw
   unset MODULESHOME
fi

if [ "${TESTSUITE_SHTOMOD_UNSETALFUNCCOMP:+x}" = 'x' ]; then
   unalias alfoo
   unset -f funcfoo
   complete -r cmd
fi

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
