/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdUlvl.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:	The Tcl module-user routine which changes the cur-   **
 **			rent user level					     **
 ** 									     **
 **   Exports:		cmdModuleUser					     **
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

/**
 **  names for the user level
 **/

static	char	_str_ul_novice[] = "novice";
static	char	_str_ul_advanced[] = "advanced";
static	char	_str_ul_expert[] = "expert";

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleUser					     **
 ** 									     **
 **   Description:	Callback function for 'info'			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
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
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int cmdModuleUser(
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
				"level ", NULL))
			return (TCL_ERROR);
					/** -------- EXIT (FAILURE) -------> **/
	}
    /**
     **  Non-persist mode?
     **/
	if (g_flags & M_NONPERSIST) {
		return (TCL_OK);
	}
    /**
     **  Display mode?
     **/
	if (g_flags & M_DISPLAY) {
		fprintf(stderr, "%s\t ", Tcl_GetString(*objv++));
		while (--objc)
			fprintf(stderr, "%s ", Tcl_GetString(*objv++));
		fprintf(stderr, "\n");
		return (TCL_OK);	/** ------- EXIT PROCEDURE -------> **/
	}
    /**
     **  Change the user level
     **/
	return (cmdModuleUser_sub((char *)Tcl_GetString(objv[1])));

} /** End of 'cmdModuleUser' **/
    

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleUser_sub				     **
 ** 									     **
 **   Description:	Check the user level passed as argument and set up   **
 **			the global user level flag			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char	*user_level	User level string to be chk. **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	sw_userlvl	Contains the binary coded userlevel  **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModuleUser_sub(	char	*user_level)
{
    char *s;
    int len_arg;

    len_arg = strlen( user_level);
    s = user_level;
    while( s && *s) {
	*s = tolower( *s);
	s++;
    }

    if( !strncmp( user_level, _str_ul_novice, len_arg)) {
	sw_userlvl = UL_NOVICE;
    } else if( !strncmp( user_level, _str_ul_advanced, len_arg)) {
	sw_userlvl = UL_ADVANCED;
    } else if( !strncmp( user_level, _str_ul_expert, len_arg)) {
	sw_userlvl = UL_EXPERT;
    } else {
	if( OK != ErrorLogger( ERR_USERLVL, LOC, user_level, NULL))
	    return( TCL_ERROR);			/** ---- EXIT (FAILURE) ---> **/
    }

    /**
     **  Success
     **/

    return( TCL_OK);

} /** End of 'cmdModuleUser_sub' **/
