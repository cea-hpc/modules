/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Purge.c				     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	Removes all loaded modulefiles from the environment. **
 ** 									     **
 **   Exports:		ModuleCmd_Purge					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Purge.c,v 1.1 2000/06/28 00:17:32 rk Exp $";
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

static	char	module_name[] = "ModuleCmd_Purge.c";	/** File name of this module **/

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Purge					     **
 ** 									     **
 **   Description:	Execution of the module-command 'purge'		     **
 ** 									     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successfull operation	     **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	ModuleCmd_Purge(	Tcl_Interp	*interp,
                		int		 argc,
                		char		*argv[])
{
    char	*lmodules = NULL,
		*cur_module = NULL,
		*loaded_modules,
		*unload_argv[ MOD_BUFSIZE];
    int		 unload_argc = 0,
    		 status;

#if WITH_DEBUGGING_MODULECMD
    fprintf( stderr, "ModuleCmd_Purge(%d):DEBUG: Starting\n", __LINE__);
#endif

    /**
     **  Get the list of currently loaded modules from the environment variable
     **  LOADEDMODULES
     **/

    if( NULL == (loaded_modules = Tcl_GetVar2( interp, "env", "LOADEDMODULES",
	TCL_GLOBAL_ONLY))) {
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) --------> **/
	else
	    return( TCL_OK);		/** ---- EXIT (Nothing to list) ----> **/
    }

    if( NULL == (lmodules = strdup( loaded_modules))) 
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) --------> **/

    /**
     **  Build a NULL terminated list of loaded modules
     **/

    for( cur_module = strtok( lmodules, ":");
         cur_module && unload_argc < MOD_BUFSIZE-1;
         cur_module = strtok( NULL, ":"))
        unload_argv[ unload_argc++] = cur_module;
    
    unload_argv[ unload_argc] = NULL;
    
    /**
     **  Unload 'em all
     **  We always say the load succeeded.  ModuleCmd_Load will
     **  output any necessary error messages.
     **/

    ModuleCmd_Load( interp, 0, unload_argc, unload_argv);
    status = TCL_OK;

    /**
     **  Free, what has been allocated and pass the load's result to the caller
     **/

    free( lmodules);

#if WITH_DEBUGGING_MODULECMD
    fprintf( stderr, "ModuleCmd_Purge(%d):DEBUG: End\n", __LINE__);
#endif

    return( status);

} /** End of 'ModuleCmd_Purge' **/
