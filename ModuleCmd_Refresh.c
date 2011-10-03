/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Refresh.c				     **
 **   First Edition:	2005/06/02					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 		Andrew Ruder, aer@absoft.com 				     **
 ** 									     **
 **   Description:	Similar to a 'update' command but only refreshes     **
 **			the non-persistent elements			     **
 ** 									     **
 **   Exports:		ModuleCmd_Refresh				     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Refresh.c,v 1.3.18.2 2011/10/03 19:31:52 rkowen Exp $";
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

static	char	module_name[] = "ModuleCmd_Refresh.c";	/** File name of this module **/
#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Refresh[] = "ModuleCmd_Refresh";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Refresh				     **
 ** 									     **
 **   Description:	Execution of the module-command 'refresh'	     **
 **			Does only the non-persistent modules settings        **
 **			(aliases)					     **
 ** 									     **
 **   First Edition:	2005/06/02					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals:	specified_module	The module name
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int ModuleCmd_Refresh(	Tcl_Interp	*interp,
			int		 argc,
                  	char		*argv[])
{
    Tcl_Interp	*refr_interp;
    Tcl_DString	 cmdbuf;
    int		 i,
    		 result,
		 count;
    char	*list[ MOD_BUFSIZE];
    char	*files[ MOD_BUFSIZE];
    char	*lmenv;
    char	*loaded;
    
#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Refresh, NULL);
#endif

    /**
     ** Begin by getting the list of loaded modules.
     **/

    loaded = getenv( "LOADEDMODULES" );
    if (!loaded || !*loaded)
	goto success0;

    loaded = strdup(loaded);
    if( !loaded )
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind0;

    if (!(lmenv = getLMFILES(interp))) {
	if ( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    goto unwind1;
	else
	    goto success1;
    }

    if (*lmenv == '\0')
	goto success1;

    count = 1;
    for( list[ 0] = xstrtok( loaded, ":");
	 list[ count] = xstrtok( NULL, ":");
	 count++ );

    count = 1;
    for( files[ 0] = xstrtok( lmenv, ":");
	 files[ count] = xstrtok( NULL, ":");
	 count++ );

    /**
     **  Initialize the command buffer and set up the modules flag
     **  to 'non-persist only'
     **/

    Tcl_DStringInit( &cmdbuf);
    g_flags |= M_NONPERSIST;

    /**
     **  Handle each loaded module file. Create a Tcl interpreter for each 
     **  module file to be handled and initialize it with custom module commands
     **/

    for(i=0; i < count && list[i]; i++) {
        /**
         ** Set the name of the current module
         **/
	g_specified_module = list[i];

	refr_interp = EM_CreateInterp();
	if ( TCL_OK != (result = InitializeModuleCommands ( refr_interp ))) {
	    EM_DeleteInterp( refr_interp );
	    null_free((void *) &loaded);
	    return (result);
	}

	/**
	 **  Execute the module 
	 **/

        g_current_module = list[i];

	result = CallModuleProcedure( refr_interp, &cmdbuf, files[i], 
	    "ModulesNonPersist", 0);

	/**
	 **  Remove the Tcl interpreter ...
	 **/

	EM_DeleteInterp( refr_interp);

    } /** for **/

    /**
     **  Leave the 'nonpersist mode', free up what has been used and return
     **/

    g_flags &= ~M_NONPERSIST;

    Tcl_DStringFree( &cmdbuf);

success1:
    null_free((void *) &loaded);

success0:
#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Refresh, NULL);
#endif

    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind1:
    null_free((void *) &loaded);

unwind0:
    return( TCL_ERROR );		/** -------- EXIT (FAILURE) -------> **/
} /** End of 'ModuleCmd_Refresh' **/
