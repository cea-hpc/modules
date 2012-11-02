/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdIsLoaded.c					     **
 **   First Edition:	2000/04/12					     **
 ** 									     **
 **   Authors:								     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:	The Tcl conflict and prereq commands.		     **
 ** 									     **
 **   Exports:		cmdIsLoaded					     **
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

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdIsLoaded					     **
 ** 									     **
 **   Description:	Callback function for 'is-loaded'		     **
 ** 									     **
 **   First Edition:	2000/04/12					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 objc		Number of arguments  **
 **			Tcl_Obj		*objv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			ModulePathVec	MODULEPATHS uvec		     **
 **			ModulePath	Vector of MODULEPATHS		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int cmdIsLoaded(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	uvec           *modulelist;		/** List of modules          **/
	char           *notloaded_flag;		/** conflicting module	     **/
	int             i, j, k,		/** Loop counters            **/
	                numpaths, nummodules;
					/** Size of the according arrays     **/
    /** 
     **  Parameter check. Usage is 'is-loaded <module> [<module> ...]'
     **/
	if (objc < 2) {
		if (OK !=
		    ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				"is-loaded-modules", NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}
    /**
     **  There's no prerequisite check in case of whatis
     **/
	if (g_flags & M_WHATIS)
		return (TCL_OK);	/** -------- EXIT (SUCCESS) -------> **/
    /**
     **  Load the MODULEPATH and split it into a list of paths
     **/
	if (!ModulePathVec)
		if (OK != ErrorLogger(ERR_MODULE_PATH, LOC, NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	if (!(numpaths = uvec_number(ModulePathVec)))
		return (TCL_OK);	/** -------- EXIT (SUCCESS) -------> **/
    /**
     **  Check/Display all passed modules
     **/
	notloaded_flag = Tcl_GetString(objv[1]);
	for (i = 1; i < objc && Tcl_GetString(objv[i]) && notloaded_flag; i++) {
		for (j = 0; j < numpaths && notloaded_flag; j++) {
			if (!(modulelist = SortedDirList( ModulePath[j],
				   Tcl_GetString(objv[i]),&nummodules)))
				continue;
	    /**
	     **  Now actually check if the prerequisites are fullfilled
	     **  The notloaded_flag controls the exit from both loops in case
	     **  a prerequisite is missing.
	     **/
			for (k = 0; k < nummodules && notloaded_flag; k++) {
				if (!IsLoaded(interp,
				uvec_vector(modulelist)[k], NULL, NULL)) {
					notloaded_flag = Tcl_GetString(objv[i]);
				} else {
					notloaded_flag = NULL;
				}
			}
	    /**
	     **  Free what has been allocted in the loop
	     **/
			FreeList(&modulelist);
		} /** for( j) **/
	} /** for( i) **/
    /**
     **  Display an error message if this was *NOT* display mode and a
     **  missing prerequisite has been found
     **/
	Tcl_SetResult(interp, notloaded_flag ? "0" : "1", TCL_STATIC);

	return (TCL_OK);

} /** End of 'cmdIsLoaded' **/
