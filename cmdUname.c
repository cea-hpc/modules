/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdUname.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, rk@owen.sj.ca.us				     **
 ** 									     **
 **   Description:	Provides fast aquisition of the information available**
 **			via uname.  This shows a 10x improvement over having **
 **			to exec the actual uname program from within a	     **
 **			modulefile.					     **
 ** 									     **
 **   Exports:		cmdUname					     **
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

static char Id[] = "@(#)$Id: cmdUname.c,v 1.7 2009/08/23 06:57:17 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

#ifdef HAVE_UNAME
#  include <sys/utsname.h>
#else
/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/
typedef	struct	utsname {
	char	sysname[ NAMELEN];	/** System name			     **/
	char	nodename[ NAMELEN];	/** Node name			     **/
	char	release[ NAMELEN];	/** OS Release			     **/
	char	version[ NAMELEN];	/** OS Version			     **/
	char	machine[ NAMELEN];	/** Machine type		     **/
} UTS_NAME;
#endif

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

#define	NAMELEN		(8 + 1)		/** 8 chars + 1 terminator	     **/
#define	DOMAINLEN	(64 + 1)	/** 64 chars + 1 terminator	     **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = __FILE__;

#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdUname[] = "cmdUname";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdUname					     **
 ** 									     **
 **   Description:	Callback function for  the commands 'sysname',	     **
 **			'nodename', 'release', 'version' and 'machine'       **
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
 **   Attached Globals:	flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int cmdUname(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
#ifdef PHOSTNAME
#  ifndef HAVE_GETHOSTNAME
	FILE           *hname;
#  endif
#endif
	char            domain[DOMAINLEN],
					/** host domain			     **/
	               *arg1;		/** argument string		     **/
	int             length,		/** argument string length	     **/
	                namestruct_init = 0;
					/** namestruct init flag	     **/
#ifdef HAVE_UNAME
	struct utsname  namestruct;
#else
	UTS_NAME        namestruct;
	strncat(namestruct.sysname, _(em_unknown), NAMELEN);
	strncat(namestruct.nodename, _(em_unknown), NAMELEN);
	strncat(namestruct.release, _(em_unknown), NAMELEN);
	strncat(namestruct.version, _(em_unknown), NAMELEN);
	strncat(namestruct.machine, _(em_unknown), NAMELEN);
#endif

#if WITH_DEBUGGING_CALLBACK
	ErrorLogger(NO_ERR_START, LOC, _proc_cmdUname, NULL);
#endif

    /**
     **  Parameter check. One parameter should be given providing a selector
     **  do differ between:
     **/
	if (objc != 2) {
		if (OK !=
		    ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				"member", NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}
#ifdef HAVE_UNAME
    /**
     **  Process the system call
     **/
	if (!namestruct_init && uname(&namestruct) < 0) {
		if (OK != ErrorLogger(ERR_UNAME, LOC, NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}
#else				/* not HAVE_UNAME */
    /**
     **  If we do not have the uname system call, fixed values defined 
     **  at compile time will be returned. The only differenc is the
     **  nodename, which may be seeked for using 'gethostname' or the
     **  PHOSTNAME file.
     **/
#  ifdef HAVE_GETHOSTNAME
	if (-1 == gethostname(namestruct.nodename, NAMELEN))
		if (OK != ErrorLogger(ERR_GETHOSTNAME, LOC, NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/

#  else				/* not HAVE_GETHOSTNAME */
#    ifdef PHOSTNAME
	if (NULL == (hname = popen(PHOSTNAME, "r"))) {
		if (OK != ErrorLogger(ERR_POPEN, LOC, PHOSTNAME,
				      _(em_reading), NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}

	fgets(namestruct.nodename, NAMELEN, hname);
	namestruct.nodename[strlen(namestruct.nodename) - 1] = '\0';

	if (-1 == pclose(hname))
		if (OK != ErrorLogger(ERR_PCLOSE, LOC, PHOSTNAME, NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/

#    endif			/* not PHOSTNAME */
#  endif			/* not HAVE_GETHOSTNAME */
#endif				/* not HAVE_UNAME */
    /**
     **  Set up the domain name
     **/
#ifdef HAVE_GETDOMAINNAME
	if (!namestruct_init)
		if (-1 == getdomainname(domain, DOMAINLEN))
			if (OK != ErrorLogger(ERR_GETDOMAINNAME, LOC, NULL))
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
#else
	strncat(domain, _(em_unknown), DOMAINLEN);
#endif
    /**
     **  Now the name structure surely IS initialized
     **/
	namestruct_init = 1;
    /**
     **  Return the selected value
     **/

	arg1 = Tcl_GetString(objv[1]);
	length = strlen(arg1);

	if (!strncmp(arg1, "sysname", length)) {
		Tcl_SetResult(interp, namestruct.sysname, TCL_VOLATILE);
	} else if (!strncmp(arg1, "nodename", length)) {
		Tcl_SetResult(interp, namestruct.nodename, TCL_VOLATILE);
	} else if (!strncmp(arg1, "release", length)) {
		Tcl_SetResult(interp, namestruct.release, TCL_VOLATILE);
	} else if (!strncmp(arg1, "version", length)) {
		Tcl_SetResult(interp, namestruct.version, TCL_VOLATILE);
	} else if (!strncmp(arg1, "machine", length)) {
		Tcl_SetResult(interp, namestruct.machine, TCL_VOLATILE);
	} else if (!strncmp(arg1, "domain", length)) {
		Tcl_SetResult(interp, domain, TCL_VOLATILE);
	} else {
		if (OK != ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
		"{sysname|nodename|release|version|machine|domain}", NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}

#if WITH_DEBUGGING_CALLBACK
	ErrorLogger(NO_ERR_END, LOC, _proc_cmdUname, NULL);
#endif

	return (TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

} /** End of 'cmdUname' **/
