/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdIsLoaded.c					     **
 **   First Edition:	2000/04/12					     **
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

static char Id[] = "@(#)$Id: cmdIsLoaded.c,v 1.6.20.1 2010/11/11 18:23:18 rkowen Exp $";
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

static	char	module_name[] = "cmdIsLoaded.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdIsLoaded[] = "cmdIsLoaded";
#endif

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
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
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

int	cmdIsLoaded(	ClientData	 client_data,
	    		Tcl_Interp	*interp,
	    		int		 argc,
	    		CONST84 char	*argv[])
{
    char	 **pathlist;
    char	 **modulelist;
    char	  *modulepath;
    char	  *notloaded_flag = (char *) argv[1];
    int     	   i, j, k, numpaths, nummodules;
	
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdIsLoaded, NULL);
#endif

    /** 
     **  Parameter check. Usage is 'is-loaded <module> [<module> ...]'
     **/

    if( argc < 2) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "is-loaded-modules", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  There's no prerequisite check in case of whatis
     **/

    if( g_flags & M_WHATIS)
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

    /**
     **  Load the MODULEPATH and split it into a list of paths
     **/

    if( !(modulepath = (char *) getenv( "MODULEPATH"))) {
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#if WITH_DEBUGGING_CALLBACK_1
    ErrorLogger( NO_ERR_DEBUG, LOC, "Got modulepath: '", modulepath, "'", NULL);
#endif

    if( !(pathlist = SplitIntoList( interp, modulepath, &numpaths, _colon)))
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

    /**
     **  Check/Display all passed modules
     **/

    for( i=1; i<argc && argv[i] && notloaded_flag; i++) {
        for( j = 0; j < numpaths && notloaded_flag; j++) {

            if( NULL == (modulelist = SortedDirList( interp, pathlist[j], 
	        (char *) argv[i], &nummodules)))
                continue;

	    /**
	     **  Now actually check if the prerequisites are fullfilled
	     **  The notloaded_flag controls the exit from both loops in case
	     **  a prerequisite is missing.
	     **/

            for( k=0; k < nummodules && notloaded_flag; k++) {
                if( !IsLoaded( interp, modulelist[k], NULL, NULL)) {
                    notloaded_flag = (char *) argv[i];
                } else {
                    notloaded_flag = NULL;
                }
            }

	    /**
	     **  Free what has been allocted in the loop
	     **/

            FreeList( modulelist, nummodules);

        } /** for( j) **/
    } /** for( i) **/

    /**
     **  Display an error message if this was *NOT* display mode and a
     **  missing prerequisite has been found
     **/

    Tcl_SetResult( interp, notloaded_flag ? "0" : "1", TCL_STATIC);

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdIsLoaded, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdIsLoaded' **/
