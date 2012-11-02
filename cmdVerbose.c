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
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
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

static char Id[] = "@(#)$Id$";
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

static	char	module_name[] = __FILE__;

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
 **			int		 objc		Number of arguments  **
 **			Tcl_Obj		*objv[]		Argument array	     **
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

int cmdModuleVerbose(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
    /**
     **  Whatis mode?
     **/
	if (g_flags & (M_WHATIS | M_HELP))
		return (TCL_OK);	/** ------- EXIT PROCEDURE -------> **/
    /**
     **  Parameter check
     **/
	if (objc < 2) {
		if (OK !=
		    ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				" on|off|fmt [args]", NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}
    /**
     **  Display mode?
     **/
	if (g_flags & M_DISPLAY) {
		fprintf(stderr, "%s ", Tcl_GetString(*objv++));
		while (--objc)
			fprintf(stderr, "%s ", Tcl_GetString(*objv++));
		fprintf(stderr, "\n");
		return (TCL_OK);	/** ------- EXIT PROCEDURE -------> **/
	}
    /**
     **  on or off
     **/
	if (!strcmp(Tcl_GetString(objv[1]), "on"))
		sw_verbose = 1;
	else if (!strcmp(Tcl_GetString(objv[1]), "off"))
		sw_verbose = 0;
	else {
		int	argc = --objc;
		char	**argv;

		Tcl_ObjvToArgv(&argc, &argv, argc,++objv);
		Module_Verbosity(argc, argv);
	}

	return (TCL_OK);

} /** End of 'cmdModuleVerbose' **/
