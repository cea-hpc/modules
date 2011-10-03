/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Display.c				     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	Displays what changes a modulefile will make to the  **
 **			environment including any conflics or prerequisits.  **
 ** 									     **
 **   Exports:		ModuleCmd_Display				     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Display.c,v 1.4.22.2 2011/10/03 19:31:52 rkowen Exp $";
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

char local_line[] =
    "-------------------------------------------------------------------\n";
static	char	module_name[] = "ModuleCmd_Display.c";	/** File name of this module **/
#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Display[] = "ModuleCmd_Display";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Display				     **
 ** 									     **
 **   Description:	Execution of the module-command 'display'	     **
 **			Display every change a module 'load' would apply to  **
 **			the environment					     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals:	g_specified_module	The module name from the     **
 **						command line.		     **
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int ModuleCmd_Display(	Tcl_Interp	*interp,
			int		 argc,
                  	char		*argv[])
{
    Tcl_Interp	*disp_interp;
    Tcl_DString	 cmdbuf;
    int		 i,
    		 result;
    char	 modulefile[ MOD_BUFSIZE];
    char	 modulename[ MOD_BUFSIZE];
    
#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Display, NULL);
#endif

    /**
     **  Initialize the command buffer and set up the modules flag to 'display
     **  only'
     **/

    Tcl_DStringInit( &cmdbuf);
    g_flags |= M_DISPLAY;

    /**
     **  Handle each passed module file. Create a Tcl interpreter for each 
     **  module file to be handled and initialize it with custom module commands
     **/

    for(i=0; i<argc && argv[i]; i++) {
        /**
         ** Set the name of the module specified on the command line
         **/

        g_specified_module = argv[i];

	disp_interp = EM_CreateInterp();
	if( TCL_OK != (result = InitializeModuleCommands( disp_interp))) {
	    EM_DeleteInterp( disp_interp);
	    return( result);		/** -------- EXIT (FAILURE) -------> **/
	}

	/**
	 **  locate the filename related to the passed module
	 **/

	if( Locate_ModuleFile(disp_interp, argv[i], modulename, modulefile) == 
            TCL_ERROR) {
	    EM_DeleteInterp( disp_interp);
	    if( OK != ErrorLogger( ERR_LOCATE, LOC, argv[i], NULL)) 
		break;
	    else
		continue;
	}

	/**
	 **  Print out everything that would happen if the module file were
	 **  executed ...
	 **/

        g_current_module = modulename;

        fprintf( stderr, local_line);
	fprintf( stderr, "%s:\n\n", modulefile);

	result = CallModuleProcedure( disp_interp, &cmdbuf, modulefile, 
	    "ModulesDisplay", 0);

        fprintf( stderr, local_line);

	/**
	 **  Remove the Tcl interpreter that has been used for printing ...
	 **/

	EM_DeleteInterp( disp_interp);

    } /** for **/

    /**
     **  Leave the 'display only mode', free up what has been used and return
     **/

    g_flags &= ~M_DISPLAY;
    fprintf( stderr, "\n");

    Tcl_DStringFree( &cmdbuf);

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Display, NULL);
#endif

    return( TCL_OK);

} /** End of 'ModuleCmd_Display' **/

