# example shell script to test 'sh-to-mod' sub-command

if ( $?TESTSUITE_SHTOMOD_SHELLVAR ) then
   setenv FOOSHELL $shell
endif

if ( ! $?TESTSUITE_SHTOMOD_NOVAR ) then
   setenv FOO value
   setenv FOOEM ""
   setenv FOOSP "value "
   setenv FOOCB va\{ue
   setenv FOOWC va\*ue
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
   setenv FOOPATHWC /path/to/dir1:/path/to/d\*r2:/path/to/dir3
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

if ( ! $?TESTSUITE_SHTOMOD_NOCOMP ) then
   complete cmd 'n/-h/n/' 'n/--help/n/' \
      'n/-V/n/' 'n/--version/n/' \
      "p/1/(-h -V --help --version)/"
   complete mycmd 'n/help/`_module_avail`/' 'n/add/`_module_not_yet_loaded; echo "--auto --no-auto --force -f --icase -i --tag --tag="`/'
   complete othercmd 'n/--help/n/'
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

if ( $?TESTSUITE_SHTOMOD_UNSETENV ) then
   unsetenv TESTSUITE_SHTOMOD_UNSETENV
endif

if ( $?TESTSUITE_SHTOMOD_EMPTYAPPEND ) then
   setenv TESTSUITE_SHTOMOD_EMPTYAPPEND ${TESTSUITE_SHTOMOD_EMPTYAPPEND}:
endif

if ( $?TESTSUITE_SHTOMOD_EMPTYPREPEND ) then
   setenv TESTSUITE_SHTOMOD_EMPTYPREPEND :$TESTSUITE_SHTOMOD_EMPTYPREPEND
endif

if ( $?TESTSUITE_SHTOMOD_MODULE ) then
   eval `$TCLSH $MODULES_CMD csh autoinit`
   module config collection_target bar
   module load setenv/1.0
   unalias ml
   unalias module
   unsetenv MODULESHOME
endif

if ( $?TESTSUITE_SHTOMOD_UNSETALFUNCCOMP ) then
   unalias alfoo
   uncomplete cmd
endif

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
