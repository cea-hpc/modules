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
 ** 									     **
 **   Description:	The Tcl module-trace routine which provides a con-   **
 **			trolling interface to the modulecmd tracing feature  **
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

static char Id[] = "@(#)$Id: cmdLog.c,v 1.5.18.2 2011/10/03 20:25:43 rkowen Exp $";
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

static	char	module_name[] = "cmdLog.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdModuleLog[] = "cmdModuleLog";
#endif

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
 **   Description:	Callback function for 'log'			     **
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
 **   Attached Globals:	tracelist	List containing all tracing settings **
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModuleLog(	ClientData	 client_data,
	      		Tcl_Interp	*interp,
	      		int		 argc,
	      		CONST84 char	*argv[])
{
    char	**facptr;
    int		  i, len = 0, alc_len = PART_LEN, save_len;
    char	 *faclist, *s, *tmp, *t;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModuleLog, NULL);
#endif

    /**
     **  Whatis mode?
     **/
    if( g_flags & (M_WHATIS | M_HELP))
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
	
    /**
     **  Parameter check
     **/
    if( argc < 3) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], " error-weight",
	    " facility", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
  
    /**
     **  Display mode?
     **/
    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }
	
    /**
     **  Get the current facility pointer.
     **/
    if((char **) NULL == (facptr = GetFacilityPtr( (char *) argv[1]))) 
	return(( OK == ErrorLogger(ERR_INVWGHT_WARN,LOC, argv[1],NULL))
	    ? TCL_OK : TCL_ERROR);

    /**
     **  Allocate memory for the facility list
     **/
    if((char *) NULL == (faclist = (char *) module_malloc( alc_len)))
	return(( OK == ErrorLogger( ERR_ALLOC, LOC, NULL)) ?
	    TCL_OK : TCL_ERROR);

    /**
     **  Scan all given facilities and add them to the list
     **/
    for( i=2; i<argc; i++) {
	save_len = len;
	len += strlen( argv[ i]) + 1;

	while( len + 1 > alc_len) {
	    alc_len += PART_LEN;
	    if(!(faclist = (char *) module_realloc( faclist, alc_len)))
		return(( OK == ErrorLogger( ERR_ALLOC, LOC, NULL)) ?
		    TCL_OK : TCL_ERROR);
	}

	faclist[save_len] = ':';
	strcpy( &faclist[save_len + 1], argv[ i]);
    }

    /**
     **  Now scan the whole list and copy all valid parts into a new buffer
     **/
    if((char *) NULL == (tmp = stringer(NULL, strlen( faclist), NULL))) {
	null_free((void *) &faclist);
	return(( OK == ErrorLogger( ERR_ALLOC, LOC, NULL)) ?
	    TCL_OK : TCL_ERROR);
    }

    for( t = tmp, s = xstrtok( faclist, ":, \t");
	 s;
	 s = xstrtok( NULL, ":, \t") ) {

	if (s && !*s) continue;		/* skip empty ones */
	if( '.' == *s || '/' == *s ||			       /** filename  **/
	    !strcmp( _stderr, s) || !strcmp( _stdout, s) ||    /** special   **/
	    !strcmp( _null, s) || !strcmp( _none, s) ||        /** null	     **/
	    CheckFacility( s, &i, &i) ) {		       /** syslog    **/

	    if( t != tmp) 
		*t++ = ':';
	    strcpy( t, s);

	    t += strlen( s);

	} else {

	    /**
	     **  bad facility found
	     **/

	    if( OK != ErrorLogger( ERR_INVFAC_WARN, LOC, s, NULL))
		break;  /** for **/
	}
    } /** for **/

    /**
     **  Now, 'tmp' should contain the new list of facilities. Check wheter
     **  there has been one allocated so far ...
     **  We do not need the orginal faclist any more.
     **/

    null_free((void *) &faclist);

    if((char *) NULL != *facptr)
	null_free((void *) facptr);

    *facptr = tmp;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModuleLog, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdModuleLog' **/
    
