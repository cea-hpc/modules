/*****
 ** ** Module Header ******************************************************* **
 **									     **
 **   File:		cmdChdir.c					     **
 **   First Edition:	2009-06-27					     **
 **									     **
 **   Authors:		Gerrit Renker					     **
 **			R.K. Owen, rk@owen.sj.ca.us			     **
 **									     **
 **   Description:	Implements a "chdir" command to set the working      **
 **			directory upon module load/switch.		     **
 **									     **
 **   Exports:		cmdChDir					     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			change_dir	Communicates the target directory.   **
 ** ************************************************************************ **
 ****/

static char Id[] = "@(#)$Id: cmdChdir.c,v 1.6 2009/08/23 23:30:42 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

char		*change_dir	= NULL;

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

int cmdChDir(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	char			*dir;		/** directory name	     **/
	struct	stat		 statinfo;	/** directory access	     **/

	if (g_flags & ~(M_LOAD | M_SWSTATE2))
		return TCL_OK;

	if (objc != 2) {
		if (OK != ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				" directory", NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}

	if (g_flags & M_NONPERSIST)
		return TCL_OK;

	if (g_flags & M_DISPLAY) {
		fprintf(stderr, "%s\t%s\n", Tcl_GetString(objv[0]),
			Tcl_GetString(objv[1]));
		return TCL_OK;
	}

	/* check for directory existence */
	/* It is not an error if we can not ... */
	dir = Tcl_GetString(objv[1]);
	if ( stat(dir, &statinfo) < 0 ) {
		ErrorLogger(ERR_CHDIR, LOC, dir, g_current_module, NULL);
		return (TCL_OK);	    /** ------ EXIT PROCEDURE -----> **/
	}
	if (!S_ISDIR(statinfo.st_mode)) {
		ErrorLogger(ERR_CHDIR, LOC, dir, g_current_module, NULL);
		return (TCL_OK);	    /** ------ EXIT PROCEDURE -----> **/
	}

	/* The actual work happens here */
	if (change_dir != NULL)
		null_free((void *) &change_dir);
	change_dir = stringer(NULL, 0, dir, NULL);

	return TCL_OK;
}
