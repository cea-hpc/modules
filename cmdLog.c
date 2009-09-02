/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdLog.c					     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:	The Tcl module-log routine which provides an	     **
 **			interface to the modulecmd syslog/stderr output.     **
 ** 									     **
 **   Exports:		cmdModuleLog					     **
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

static char Id[] = "@(#)$Id: cmdLog.c,v 1.15 2009/09/02 20:37:39 rkowen Exp $";
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

#define	PART_LEN	256		/** Length of the string parts for   **/
					/** facility list handling	     **/

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = __FILE__;
static	char	_stderr[] = "stderr";
static	char	_stdout[] = "stdout";
static	char	_null[] = "null";
static	char	_none[] = "none";

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleLog					     **
 ** 									     **
 **   Description:	Callback function for 'module-log'		     **
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

int cmdModuleLog(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	char          **facptr, **vptr, *s;
	int             i;
	uvec           *faclist,
		       *tmpuv = NULL;

    /**
     **  Whatis mode?
     **/
	if (g_flags & (M_WHATIS | M_HELP))
		return (TCL_OK);	/** ------- EXIT PROCEDURE -------> **/
    /**
     **  Parameter check
     **/
	if (objc < 3) {
		if (OK != ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				" error-weight", " facility", NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}
    /**
     **  Display mode?
     **/
	if (g_flags & M_DISPLAY) {
		fprintf(stderr, "%s\t ", Tcl_GetString(*objv++));
		while (--objc)
			fprintf(stderr, "%s ", Tcl_GetString(*objv++));
		fprintf(stderr, "\n");
		return (TCL_OK);	/** -------- EXIT PROCEDURE -------> **/
	}
    /**
     **  Get the current facility pointer.
     **/
	if (!(facptr = GetFacilityPtr((char *)Tcl_GetString(objv[1]))))
		return ((OK == ErrorLogger(ERR_INVWGHT_WARN, LOC,
				     Tcl_GetString(objv[1]), NULL))
			? TCL_OK : TCL_ERROR);
    /**
     **  Allocate memory for the facility list
     **/
	if (!(faclist = uvec_ctor(objc)))
		return ((OK == ErrorLogger(ERR_UVEC, LOC, NULL))
			? TCL_OK : TCL_ERROR);
    /**
     **  Scan all given facilities and add them to the list
     **  Which can be listed as individual arguments or joined together
     **  with a ':' (or both)
     **/
	for (i = 2; i < objc; i++) {
		s = Tcl_GetString(objv[i]);
		/* check for at least one ':' */
		if (strchr(s, ':')) {
			tmpuv = str2uvec(":", s);
			vptr = uvec_vector(tmpuv);
			while (vptr && *vptr) {
				if(0 > uvec_add(faclist,*vptr++))
					return ((OK == ErrorLogger(ERR_UVEC,
					LOC, NULL)) ? TCL_OK : TCL_ERROR);
			}
			uvec_dtor(&tmpuv);
		} else {
			if(0 > uvec_add(faclist, Tcl_GetString(objv[i])))
				return ((OK == ErrorLogger(ERR_UVEC, LOC, NULL))
					? TCL_OK : TCL_ERROR);
		}
	}
    /**
     **  Now scan the whole list and delete the invalid ones
     **/
	for (i = uvec_number(faclist) - 1; i >= 0; i--) {
		s = uvec_vector(faclist)[i];

		if (s && !*s)
			continue;	/* skip empty ones */

		if ('.' == *s || '/' == *s ||		       /** filename  **/
		    !strcmp(_stderr, s) || !strcmp(_stdout, s) ||
							       /** special   **/
		    !strcmp(_null, s) || !strcmp(_none, s) ||  /** null	     **/
		    CheckFacility(s, NULL, NULL)) {	       /** syslog    **/
			/* got a keeper */
		} else {
		    /**
		     **  bad facility found
		     **/
			if (OK != ErrorLogger(ERR_INVFAC_WARN, LOC, s, NULL))
				break; /** for **/
			if (0 > uvec_delete(faclist,i))
				return ((OK == ErrorLogger(ERR_UVEC, LOC, NULL))
					? TCL_OK : TCL_ERROR);
		}
	} /** for **/

	s = uvec2str(faclist,":");

    /**
     **  Now, 's' should contain the new list of facilities. Check whether
     **  there has been one allocated so far ...
     **  We do not need the original faclist any more.
     **/
	uvec_dtor(&faclist);

	if ((char *)NULL != *facptr)
		null_free((void *)facptr);

	*facptr = s;

	return (TCL_OK);

} /** End of 'cmdModuleLog' **/
