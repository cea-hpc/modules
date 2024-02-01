# example shell script to test 'bash-eval' mode of 'sh-to-mod' sub-command

if [ "${TESTSUITE_SHTOMOD_FUZZYOUT1:+x}" = 'x' ]; then
   echo %ModulesShToMod%
fi
if [ "${TESTSUITE_SHTOMOD_FUZZYOUT2:+x}" = 'x' ]; then
   echo() {
      :
   }
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

modcmd="$TESTSUITEDIR/../modulecmd.tcl"
tmpmod="$TESTSUITEDIR/bash-eval-tmp"
export MODULES_ADVANCED_VERSION_SPEC=0
$modcmd bash sh-to-mod bash "$TESTSUITEDIR/example/sh-to-mod.sh" "$@" 2>"$tmpmod"
$modcmd bash load "$tmpmod"
rm "$tmpmod"

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
