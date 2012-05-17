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

static char Id[] = "@(#)$Id: cmdUname.c,v 1.3.20.1 2010/11/11 18:23:18 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

#ifdef HAVE_UNAME
#include <sys/utsname.h>
#endif

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

#ifndef HAVE_UNAME

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
#define	DOMAINLEN	(64 + 1)	/** 8 chars + 1 terminator	     **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "cmdUname.c";	/** File name of this module **/

#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdUname[] = "cmdUname";
#endif

static	struct	utsname	 namestruct = {
	UNAME_SYSNAME, UNAME_NODENAME, UNAME_RELEASE, UNAME_VERSION,
	UNAME_MACHINE
};

static	char	domain[ DOMAINLEN] = UNAME_DOMAIN;

static	int	namestruct_init = 0;

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
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
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

int	cmdUname(	ClientData	 client_data,
	  		Tcl_Interp	*interp,
	  		int		 argc,
	  		CONST84 char	*argv[])
{
    int  length;
#ifdef PHOSTNAME
#ifndef HAVE_GETHOSTNAME
    FILE* hname;
#endif
#endif

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdUname, NULL);
#endif

    /**
     **  Parameter check. One parameter should be given providing a selector
     **  do differ between:
     **/

    if( argc != 2) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "member", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#ifdef HAVE_UNAME

    /**
     **  Proceed the system call
     **/

    if( !namestruct_init && uname( &namestruct) < 0) {
	if( OK != ErrorLogger( ERR_UNAME, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#else /* not HAVE_UNAME */

    /**
     **  If we do not have the uname system call, fixed values defined 
     **  at compile time will be returned. The only differenc is the
     **  nodename, which may be seeked for using 'gethostname' or the
     **  PHOSTNAME file.
     **/

#ifdef HAVE_GETHOSTNAME

    if( -1 == gethostname( namestruct.nodename, NAMELEN)) 
	if( OK != ErrorLogger( ERR_GETHOSTNAME, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

#else /* not HAVE_GETHOSTNAME */

#ifdef PHOSTNAME

    if( NULL == (hname = popen( PHOSTNAME, "r"))) {
	if( OK != ErrorLogger( ERR_POPEN, LOC, PHOSTNAME, "reading", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    fgets( namestruct.nodename, NAMELEN, hname);
    namestruct.nodename[ strlen( namestruct.nodename)-1] = '\0';

    if( -1 == pclose( hname))
	if( OK != ErrorLogger( ERR_PCLOSE, LOC, PHOSTNAME, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

#endif /* not PHOSTNAME */
#endif /* not HAVE_GETHOSTNAME */

#endif /* not HAVE_UNAME */
 
    /**
     **  Set up the domain name
     **/

#ifdef HAVE_GETDOMAINNAME
    if( !namestruct_init)
        if( -1 == getdomainname( domain, DOMAINLEN))
	    if( OK != ErrorLogger( ERR_GETDOMAINNAME, LOC, NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
#endif

    /**
     **  Now the name structure surely IS initialized
     **/

    namestruct_init = 1;

    /**
     **  Return the selected value
     **/

    length = strlen( argv[1]);

    if( !strncmp( argv[1], "sysname", length)) {
        Tcl_SetResult( interp, namestruct.sysname, TCL_VOLATILE);
    } else if( !strncmp( argv[1], "nodename", length)) {
        Tcl_SetResult( interp, namestruct.nodename, TCL_VOLATILE);
    } else if( !strncmp( argv[1], "release", length)) {
        Tcl_SetResult( interp, namestruct.release, TCL_VOLATILE);
    } else if( !strncmp( argv[1], "version", length)) {
        Tcl_SetResult( interp, namestruct.version, TCL_VOLATILE);
    } else if( !strncmp( argv[1], "machine", length)) {
        Tcl_SetResult( interp, namestruct.machine, TCL_VOLATILE);
    } else if( !strncmp( argv[1], "domain", length)) {
        Tcl_SetResult( interp, domain, TCL_VOLATILE);
    } else {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "{sysname|nodename|"
	    "release|version|machine|domain}", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdUname, NULL);
#endif

    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

} /** End of 'cmdUname' **/
