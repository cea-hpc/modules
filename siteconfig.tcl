# SITECONFIG.TCL, site-specific configuration script for Modules
#
# This Tcl script enables to supersede any global variable or procedure
# definition of modulecmd.tcl. See 'Site-specific configuration' section in
# module(1) manpage for detailed information.

##########################################################################

# uncomment the following line to forbid the definition of an extra
# site-specific configuration script
#lappendConf locked_configs extra_siteconfig

# uncomment the following line to forbid `implicit_default` config option
# superseding
#lappendConf locked_configs implicit_default

# define specific variables in modulefile interpreter context
#set modulefile_extra_vars {varname1 value1 varname2 value2}

# define specific commands in modulefile interpreter context based on
# procedures defined in this file
#set modulefile_extra_cmds {command1 procedure1 command2 procedure2}

# define specific variables in modulerc interpreter context
#set modulerc_extra_vars {varname1 value1 varname2 value2}

# define specific commands in modulerc interpreter context based on procedures
# defined in this file
#set modulerc_extra_cmds {command1 procedure1 command2 procedure2}

