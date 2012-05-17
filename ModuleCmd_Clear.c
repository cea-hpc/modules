/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Clear.c				     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	Clears out Modules' concept of the currently loaded  **
 **			modules.					     **
 ** 									     **
 **   Exports:		ModuleCmd_Clear					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Clear.c,v 1.3.22.1 2010/11/11 18:23:18 rkowen Exp $";
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

#if WITH_DEBUGGING_MODULECMD
static	char	module_name[] = "ModuleCmd_Clear.c";	/** File name of this module **/
static	char	_proc_ModuleCmd_Clear[] = "ModuleCmd_Clear";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Clear					     **
 ** 									     **
 **   Description:	Execution of the module-command 'clear'		     **
 **			Resets the modules runtime information but doesn't   **
 **			apply further changes to the environment at all      **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int ModuleCmd_Clear(	Tcl_Interp	*interp,
		    	int         	 argc,
		    	char		*argv[])
{
    char		buf[10];
    char*		clearargv[4];
    
#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Clear, NULL);
#endif

    /**
     **  Ask the user if he's really sure about what he's doing ...
     **/

    if( argc == 1 && !strcmp( argv[0], "yes")) {
	buf[0] = 'y';
    } else {
	fprintf( stderr,
            "\nAre you sure you want to clear all loaded modules!? [n] ");
	fgets( buf, 10, stdin);
    }

    /**
     **  Reset the shell variables 'LOADEDMODULES' and '_LMFILES_'
     **/
	
    if( buf[0] == 'y') {

        clearargv[0] = "setenv";
        clearargv[1] = "LOADEDMODULES";
        clearargv[2] = "";
        clearargv[3] = NULL;
        cmdSetEnv( (ClientData) 0, interp, 3, (CONST84 char **) clearargv);

        clearargv[0] = "setenv";
        clearargv[1] = "_LMFILES_";
        clearargv[2] = "";
        clearargv[3] = NULL;
        cmdSetEnv( (ClientData) 0, interp, 3, (CONST84 char **) clearargv);

    } else {
        fprintf( stderr, "\nLOADEDMODULES was NOT cleared.\n");
    }
    
    /**
     **  Return on success
     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Clear, NULL);
#endif

    return( TCL_OK);
}
