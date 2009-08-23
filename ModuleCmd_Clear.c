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
 **		R.K. Owen, rk@owen.sj.ca.us				     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Clear.c,v 1.7 2009/08/23 23:30:42 rkowen Exp $";
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

/** not applicable **/

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
    char		buf[10],	/** user response input **/
			*ptr,		/** reponse pointer **/
    			*clearargv[4];	/** argv vector **/
	Tcl_Obj       **objv;		/** Tcl Object vector **/
	int             objc;		/** Tcl Object vector count **/
    
    /**
     **  Ask the user if he's really sure about what he's doing ...
     **/
    if( argc == 1 && !strcmp( argv[0], "yes")) {
	buf[0] = 'y';
    } else {
	fprintf(stderr,
            "\nAre you sure you want to clear all loaded modules!? [n] ");
	 ptr = fgets(buf, 10, stdin);
    }

    /**
     **  Reset the shell variables 'LOADEDMODULES' and '_LMFILES_'
     **/
    if( *ptr == 'y' || *ptr == 'Y') {

        clearargv[0] = "setenv";
        clearargv[1] = "LOADEDMODULES";
        clearargv[2] = "";
        clearargv[3] = NULL;
	/* convert fomr argv to objv */
	Tcl_ArgvToObjv(interp, &objc, &objv, -1, (char **) clearargv);
        cmdSetEnv( (ClientData) 0, interp, objc, objv);

        clearargv[0] = "setenv";
        clearargv[1] = "_LMFILES_";
        clearargv[2] = "";
        clearargv[3] = NULL;
	/* convert fomr argv to objv */
	Tcl_ArgvToObjv(interp, &objc, &objv, -1, (char **) clearargv);
        cmdSetEnv( (ClientData) 0, interp, objc, objv);

    } else {
        fprintf( stderr, "\nLOADEDMODULES was NOT cleared.\n");
    }
    
    /**
     **  Return on success
     **/

    return( TCL_OK);
}
