# example shell script to test 'sh-to-mod' sub-command

if ( $?TESTSUITE_SHTOMOD_SHELLVAR ) then
   setenv FOOSHELL $shell
endif

if ( ! $?TESTSUITE_SHTOMOD_NOVAR ) then
   setenv FOO value
   setenv FOOEM ""
   setenv FOOSP "value "
   setenv FOOCB va\{ue
   set i=1
   while ( $#argv > 0 )
      setenv FOOARG$i "$1"
      shift
      @ i++
   end
endif

if ( ! $?TESTSUITE_SHTOMOD_NOPATH ) then
   setenv FOOPATH /path/to/dir1:/path/to/dir2:/path/to/dir3
   setenv FOOPATHEM :/path/to/dir1:/path/to/dir2:/path/to/dir3
   setenv FOOPATHSP "/path/to/dir1 /path/to/dir2 /path/to/dir3"
   setenv FOOPATHSPEM "/path/to/dir1 /path/to/dir2 /path/to/dir3 "
   setenv FOOPATHCB /path/to/dir1:/path/to/d\{r2:/path/to/dir3
endif

if ( $?TESTSUITE_SHTOMOD_PATHDUP ) then
   setenv FOOPATHDUP "/path/to/dir1:/path/to/dir1:/path/to/dir2:/path/to/dir1:/path/to/dir3:/path/to dir4:/path/to/dir3"
   setenv FOOPATHDUPEM :/path/to/dir1:/path/to/dir1:/path/to/dir2:/path/to/dir1:/path/to/dir3::/path/to/dir3
   setenv FOOPATHDUPSP "/path/to/dir1 /path/to/dir1 /path/to/dir2 /path/to/dir1 /path/to/dir3 /path/to:dir4 /path/to/dir3"
   setenv FOOPATHDUPSPEM "/path/to/dir1 /path/to/dir1 /path/to/dir2 /path/to/dir1 /path/to/dir3 /path/to/dir3 "
   setenv FOOPATHDUPMIX ":/path/to/dir1:/path/to/dir1 /path/to/dir2 /path/to/dir1:/path/to/dir3:/path/to/dir3:"
endif

if ( $?TESTSUITE_SHTOMOD_FUZZYOUT1 ) then
   echo '%ModulesShToMod%'
endif
if ( $?TESTSUITE_SHTOMOD_FUZZYOUT2 ) then
   # exit to provoke fuzz
   exec true
endif

if ( ! $?TESTSUITE_SHTOMOD_NOALIAS ) then
   alias alfoo 'echo `grep "report .Modules " ../../modulecmd.tcl | tr -d \\ | awk '"'"'{print \$3}'"'"'`'
   alias alcb 'echo f\{o'
   alias alem ''
   alias alsp 'echo f\"o; echo b\\\"r; echo f'"\'"'o'
endif

if ( ! $?TESTSUITE_SHTOMOD_NOFUNC ) then
   # no function support on csh shell
endif

if ( ! $?TESTSUITE_SHTOMOD_NOCD ) then
   cd $TESTSUITEDIR/modulefiles.2
endif

if ( $?TESTSUITE_SHTOMOD_OUTMSG ) then
   echo "output message"
endif

if ( $?TESTSUITE_SHTOMOD_ERRMSG ) then
   echo "error message" > /dev/stderr
endif

if ( $?TESTSUITE_SHTOMOD_EXIT1 ) then
   # provoke an immediate exit with error code
   exec false
endif

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
