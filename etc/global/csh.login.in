#----------------------------------------------------------------------#
# system-wide csh.login                                                #
# Variables and settings for all csh-derivative shells                 #
#----------------------------------------------------------------------#
# note .cshrc runs before .login
#
# these represent the minimum set of environment variables that should
# be set and used for environment modules ... you need to tailor this
# to your own system and include it within the global shell start-up
# files in /etc
#
# UNIX man documentation search path
if ( ! $?MANPATH ) then
        setenv MANPATH		/usr/man:/usr/share/man:/usr/X11R6/man
endif
        setenv MANPATH		"$MANPATH":/usr/local/man:/usr/local/X11/man
        setenv MANPATH		"$MANPATH":/usr/local/X11R6/man

# GNU info documentation search path
if ( ! $?INFOPATH ) then
        setenv INFOPATH		/usr/info:/usr/lib/info:/usr/gnu/info
endif
	setenv INFOPATH		"$INFOPATH":/usr/local/info

# gcc include directory search path
if ( ! $?C_INCLUDE_PATH ) then
	setenv C_INCLUDE_PATH	/usr/include:/usr/X11R6/include
endif
	setenv C_INCLUDE_PATH	"$C_INCLUDE_PATH":/usr/local/include

# g++ include directory search path
if ( ! $?CPLUS_INCLUDE_PATH ) then
	setenv CPLUS_INCLUDE_PATH  /usr/include:/usr/X11R6/include
endif
	setenv CPLUS_INCLUDE_PATH  "$CPLUS_INCLUDE_PATH":/usr/local/include

# generic ld library directory search path
if ( ! $?LIBRARY_PATH ) then
	setenv LIBRARY_PATH	/lib:/usr/lib:/usr/X11R6/lib
endif
	setenv LIBRARY_PATH	"$LIBRARY_PATH":/usr/local/lib

# Linux (and most Unixes)  run-time dynamically loaded libraries search path
if ( ! $?LD_LIBRARY_PATH ) then
	setenv LD_LIBRARY_PATH	/lib:/usr/lib:/usr/X11R6/lib
endif
	setenv LD_LIBRARY_PATH	"$LD_LIBRARY_PATH":/usr/local/lib

# ELF object settable run-time dynamically loaded library directory search path
#   see ld -rpath
if ( ! $?LD_RUN_PATH ) then
	setenv LD_RUN_PATH	/lib:/usr/lib:/usr/X11R6/lib
endif
	setenv LD_RUN_PATH	"$LD_RUN_PATH":/usr/local/lib

# X11 resource location search path
if ( ! $?XAPPLRESDIR ) then
	setenv XAPPLRESDIR	/usr/lib/X11/app-defaults
endif
	setenv XAPPLRESDIR	"$XAPPLRESDIR":/usr/local/X11/app-defaults

# GNOME application and library package search path
if ( ! $?GNOME_PATH ) then
	setenv GNOME_PATH	/usr
endif
	setenv GNOME_PATH	"$GNOME_PATH":/usr/local

# GNOME library definition script search path (generally `$GNOME_PATH'/lib)
if ( ! $?GNOME_LIBCONFIG_PATH ) then
	setenv GNOME_LIBCONFIG_PATH	/usr/lib
endif
	setenv GNOME_LIBCONFIG_PATH	"$GNOME_LIBCONFIG_PATH":/usr/local/lib

# executable search path (this sets $path too)
if ( ! $?PATH ) then
	setenv PATH	/bin:/usr/bin:/usr/X11R6/bin:/sbin:/usr/sbin
endif
	setenv PATH	/usr/local/bin:/usr/local/X11/bin:"$PATH"
	setenv PATH	"$PATH":/etc:/usr/etc:/usr/local/etc:.


#----------------------------------------------------------------------#
# module initialization
#
if ( -x /etc/custom/csh.modules ) then
	source /etc/custom/csh.modules
# put system-wide module loads here
#	module load null
endif

#----------------------------------------------------------------------#
# set this if bash exists on your system and to use it
# instead of sh - so per-process dot files will be sourced.
#----------------------------------------------------------------------#

setenv ENV $HOME/.bashrc

#----------------------------------------------------------------------#
# further system customizations can be added here
#
