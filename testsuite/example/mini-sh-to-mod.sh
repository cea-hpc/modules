# mini example shell script to test 'sh-to-mod' sub-command

export FOOMINI=value
export FOOPATH=/path/to/mini:$FOOPATH
alias almini='echo mini'

if [ "${TESTSUITE_SOURCESH_ESCCHAR:+x}" = 'x' ]; then
   export FOOESC=:
   export FOOPATH=/path/\{\|:$FOOPATH:/path/\}\&
fi

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
