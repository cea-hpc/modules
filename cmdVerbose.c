/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdVerbose.c					     **
 **   First Edition:	1995/12/31					     **
 ** 									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The Tcl module-verbose routine allows switchin ver-  **
 **			bosity on and off during module file execution	     **
 ** 									     **
 **   Exports:		cmdModuleVerbose				     **
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

static char Id[] = "@(#)$Id: cmdVerbose.c,v 1.4.22.1 2010/11/11 18:23:18 rkowen Exp $";
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

static	char	module_name[] = "cmdVerbose.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdModuleVerbose[] = "cmdModuleVerbose";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleVerbose				     **
 ** 									     **
 **   Description:	Callback function for 'verbose'			     **
 ** 									     **
 **   First Edition:	1995/12/31					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	sw_verbose	The verbose level selector	     **
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModuleVerbose(	ClientData	 client_data,
		      		Tcl_Interp	*interp,
		      		int		 argc,
		      		CONST84 char	*argv[])
{
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModuleVerbose, NULL);
#endif
    char **argptr = (char **) argv;

    /**
     **  Whatis mode?
     **/

    if( g_flags & (M_WHATIS | M_HELP))
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
	
    /**
     **  Parameter check
     **/

    if( argc < 2) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argptr[0], " on|off|fmt [args]",
	    NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
  
    /**
     **  Display mode?
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s ", argptr[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argptr);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }

    /**
     **  on or off
     **/

    if( !strcmp( argptr[1], "on"))
	sw_verbose = 1;
    else if( !strcmp( argptr[1], "off"))
	sw_verbose = 0;
    else
	Module_Verbosity( --argc, ++argptr);

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModuleVerbose, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdModuleVerbose' **/
    
