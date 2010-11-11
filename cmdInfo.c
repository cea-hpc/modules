/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdInfo.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The Tcl module-info routine which provides informa-  **
 **			tion about the state of Modules as the modulefile    **
 **			is being parsed. 				     **
 ** 									     **
 **   Exports:		cmdModuleInfo					     **
 ** 									     **
 **   Notes:								     **
 ** 									     **
 ** ************************************************************************ **
 ****/

/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1991-1994 by John L. Furlan.                      	     **
 ** see LICENSE.GPL, which must be provided, for details		     **
 ** 									     ** 
 ** ************************************************************************ **/

static char Id[] = "@(#)$Id: cmdInfo.c,v 1.8.22.1 2010/11/11 18:23:18 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "cmdInfo.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdModuleInfo[] = "cmdModuleInfo";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleInfo					     **
 ** 									     **
 **   Description:	Callback function for 'module-info'		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	g_flags			These are set up accordingly **
 **						before this function is	     **
 **						called in order to control   **
 **						everything.		     **
 **			g_specified_module	The module name from the     **
 **						command line.		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModuleInfo(	ClientData	 client_data,
	      		Tcl_Interp	*interp,
	      		int		 argc,
	      		CONST84 char	*argv[])
{
    char *s, *t, buf[ BUFSIZ];

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModuleInfo, NULL);
#endif

    /**
     **  Parameter check
     **/

    if( argc < 2) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "info-descriptor ",
	    "descriptor-args", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
  
    /**
     **  'module-info flags' ?
     **/

    if( !strcmp( argv[1], "flags")) {
        char tmpbuf[6];
        sprintf( tmpbuf, "%d", g_flags);
        Tcl_SetResult( interp, tmpbuf, TCL_VOLATILE);

    /**
     **  'module-info mode'
     **  without suggestion this will return the name of the state we're in.
     **/

    } else if( !strcmp( argv[1], "mode")) {

        if( argc < 3) {
            if( g_flags & M_SWSTATE1)
                Tcl_SetResult( interp, "switch1", TCL_STATIC);
            else if( g_flags & M_SWSTATE2)
                Tcl_SetResult( interp, "switch2", TCL_STATIC);
            else if( g_flags & M_SWSTATE3)
                Tcl_SetResult( interp, "switch3", TCL_STATIC);
            else if( g_flags & M_LOAD)
                Tcl_SetResult( interp, "load", TCL_STATIC);
            else if( g_flags & M_REMOVE)
                Tcl_SetResult( interp, "remove", TCL_STATIC);
            else if( g_flags & M_DISPLAY)
                Tcl_SetResult( interp, "display", TCL_STATIC);
            else if( g_flags & M_HELP)
                Tcl_SetResult( interp, "help", TCL_STATIC);
            else if( g_flags & M_WHATIS)
                Tcl_SetResult( interp, "whatis", TCL_STATIC);
            else if( g_flags & M_NONPERSIST)
                Tcl_SetResult( interp, "nonpersist", TCL_STATIC);
            else
                Tcl_SetResult( interp, "unknown", TCL_STATIC);
            
        } else {
 
	    /**
	     **  'module-info mode <my_suggestion>'
	     **  results in a boolean return indicating whether this is the
	     **  correct state ...
	     **/

	    Tcl_SetResult( interp, "0", TCL_STATIC);

	    if( !strcmp( argv[2], "load")) {
		if( g_flags & M_LOAD) Tcl_SetResult( interp, "1", TCL_STATIC);
	    }
	    else if( !strcmp( argv[2], "remove")) {
		if( g_flags & M_REMOVE) Tcl_SetResult( interp, "1", TCL_STATIC);
	    }
	    else if( !strcmp( argv[2], "display")) {
		if( g_flags & M_DISPLAY) Tcl_SetResult( interp,"1", TCL_STATIC);
	    }
	    else if( !strcmp( argv[2], "nonpersist")) {
		if( g_flags & M_NONPERSIST) Tcl_SetResult( interp,"1", TCL_STATIC);
	    }
	    else if( !strcmp( argv[2], "help")) {
		if( g_flags & M_HELP) Tcl_SetResult( interp, "1", TCL_STATIC);
	    }
	    else if( !strcmp( argv[2], "whatis")) {
		if( g_flags & M_WHATIS) Tcl_SetResult( interp, "1", TCL_STATIC);
	    }
	    else if( !strcmp( argv[2], "switch1")) {
		if( g_flags & M_SWSTATE1) Tcl_SetResult( interp,"1",TCL_STATIC);
	    }
	    else if( !strcmp(argv[2], "switch2")) {
		if( g_flags & M_SWSTATE2) Tcl_SetResult( interp,"1",TCL_STATIC);
	    }
	    else if( !strcmp(argv[2], "switch3")) {
		if( g_flags & M_SWSTATE3) Tcl_SetResult( interp,"1",TCL_STATIC);
	    }
	    else if( !strcmp(argv[2], "switch")) {
		if( g_flags & M_SWITCH) Tcl_SetResult( interp, "1", TCL_STATIC);
	    } else {
		ErrorLogger( ERR_USAGE, LOC, argv[0], "mode ", "[load|remove|"
		    "display|help|whatis|switch1|switch2|switch3|switch|nonpersist]", NULL);
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	    }
	}

    /**
     **  'module-info user'
     **  without suggestion this will return the current user level
     **/

    } else if( !strcmp( argv[1], "user")) {

        if( argc < 3) {
            if( UL_NOVICE == sw_userlvl)
                Tcl_SetResult( interp, "novice", TCL_STATIC);
            else if( UL_ADVANCED == sw_userlvl)
                Tcl_SetResult( interp, "advanced", TCL_STATIC);
            else if( UL_EXPERT == sw_userlvl)
                Tcl_SetResult( interp, "expert", TCL_STATIC);
            else
                Tcl_SetResult( interp, "unknown", TCL_STATIC);
            
        } else {
 
	    /**
	     **  'module-info user <my_suggestion>'
	     **  results in a boolean return indicating whether this is the
	     **  correct user level ...
	     **/

	    Tcl_SetResult( interp, "0", TCL_STATIC);

	    if( !strcmp( argv[2], "novice")) {
		if( UL_NOVICE == sw_userlvl)
		    Tcl_SetResult( interp, "1", TCL_STATIC);
	    }
	    else if( !strcmp( argv[2], "advanced")) {
		if( UL_ADVANCED == sw_userlvl)
		    Tcl_SetResult( interp, "1", TCL_STATIC);
	    }
	    else if( !strcmp( argv[2], "expert")) {
		if( UL_EXPERT == sw_userlvl)
		    Tcl_SetResult( interp, "1", TCL_STATIC);
	    } else {
		ErrorLogger( ERR_USAGE, LOC, argv[0], "user ",
		    "[novice|advanced|expert]", NULL);
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	    }
	}

    /**
     **  'module-info name'
     **  returns the name of the current module
     **/

    } else if( !strcmp(argv[1], "name")) {
        Tcl_SetResult( interp, g_current_module, TCL_VOLATILE);

    /**
     **  'module-info shell'
     **  returns the name of the current user shell
     **/

    } else if( !strcmp(argv[1], "shell")) {
        if( argc < 3) {
	    Tcl_SetResult( interp, shell_name, TCL_VOLATILE);
        } else {
	    if( !strcmp( argv[2], shell_name))
		Tcl_SetResult( interp, "1", TCL_STATIC);
	    else
		Tcl_SetResult( interp, "0", TCL_STATIC);
	}

    } else if( !strcmp(argv[1], "shelltype")) {
        if( argc < 3) {
	    Tcl_SetResult( interp, shell_derelict, TCL_VOLATILE);
        } else {
	    if( !strcmp( argv[2], shell_derelict))
		Tcl_SetResult( interp, "1", TCL_STATIC);
	    else
		Tcl_SetResult( interp, "0", TCL_STATIC);
	}

    /**
     **  'module-info trace'
     **  Check whether tracing is turned on
     **/

    } else if( !strcmp(argv[1], "trace")) {
	char *cmd, *module;

	if( argc > 2) 
	    cmd = (char *) argv[ 2];
	else
	    cmd = module_command;

	if( argc > 3) 
	    module = (char *) argv[ 3];
	else
	    module = g_current_module;

	if( CheckTracing(interp, cmd, module))
	    Tcl_SetResult( interp, "1", TCL_STATIC);
	else
	    Tcl_SetResult( interp, "0", TCL_STATIC);

    } else if( !strcmp(argv[1], "tracepat")) {
	char *cmd, *pattern;

	if( argc > 2) 
	    cmd = (char *) argv[ 2];
	else
	    cmd = module_command;

	if((char *) NULL == (pattern = GetTraceSel(interp, cmd)))
	    Tcl_SetResult( interp, "*undef*", TCL_STATIC);
	else
	    Tcl_SetResult( interp, pattern, TCL_VOLATILE);

    /**
     **  'module-info alias'
     **  Print the value of the passed alias
     **/

    } else if( !strcmp(argv[1], "alias")) {

	if( argc < 3) {
	    if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "alias ",
		"name", NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	}
      
	if( AliasLookup( (char *) argv[2], &s, &t)) {
	    /* sprintf( buf, "%s/%s", s, t); */
	    strcpy( buf, s);
	    strcat( buf, "/");
	    strcat( buf, t);
	    Tcl_SetResult( interp, buf, TCL_VOLATILE);
	} else {
	    Tcl_SetResult( interp, "*undef*", TCL_STATIC);
	}

    /**
     **  'module-info symbols'
     **  List all symbolic names of the passed or current module file
     **/

    } else if( !strcmp(argv[1], "symbols")) {
	char *name;

	name = (argc < 3) ? g_current_module : (char *) argv[2];

	if((char *) NULL == (s = ExpandVersions( name)))
	    Tcl_SetResult( interp, "*undef*", TCL_STATIC);
	else
	    Tcl_SetResult( interp, (char *) s, TCL_VOLATILE);

    /**
     **  'module-info version'
     **  Returns the full qualified module name and version of the passed
     **  symbolic version specifier
     **/

    } else if( !strcmp(argv[1], "version")) {
	if( VersionLookup( (char *) argv[2], &s, &t)) {
	    if( t) {
		/* sprintf( buf, "%s/%s", s, t); */
		strcpy( buf, s);
		strcat( buf, "/");
		strcat( buf, t);
	    } else {
		/* sprintf( buf, "%s", s); */
		strcpy( buf, s);
	    }
	    Tcl_SetResult( interp, buf, TCL_VOLATILE);
	} else {
	    Tcl_SetResult( interp, "*undef*", TCL_STATIC);
	}

    /**
     **  'module-info specified'
     **   gives the module name as specified on the command line
     **/

    } else if( !strcmp(argv[1], "specified")) {
	if( g_specified_module) {
	    /* TCL_STATIC because it comes from the command line */
	    Tcl_SetResult( interp, g_specified_module, TCL_STATIC);
	} else {
	    Tcl_SetResult( interp, "*undef*", TCL_STATIC);
	}

    /**
     **  unknown command ....
     **/

    } else {
	if( OK != ErrorLogger( ERR_INFO_DESCR, LOC, argv[1], NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModuleInfo, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdModuleInfo' **/
