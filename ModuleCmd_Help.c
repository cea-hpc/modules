/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Help.c				     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	Provides some general help by listing the different  **
 **			commands. It will also allow for a modulefile to     **
 **			provide some module-specific help by calling a Tcl   **
 **			procedure.					     **
 ** 									     **
 **   Exports:		ModuleCmd_Help					     **
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

static char Id[] = "@(#)$Id$";
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

static	char	module_name[] = __FILE__;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	int	PerModuleHelp(	Tcl_Interp*, int, char*[]);

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Help					     **
 ** 									     **
 **   Description:	Execution of the module-command 'help'		     **
 **			Called w/o parameters it will print a list of avail- **
 **			able commands. If it is called with a parameter it   **
 **			will print module specific help.		     **
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

int  ModuleCmd_Help(	Tcl_Interp	*interp,
		    	int         	 argc,
		    	char		*argv[])
{
    /**
     **  General or module specific help wanted. General help is done within
     **  here. In case of module specific help we'll call a subroutine to do
     **  it ...
     **/
    if( argc > 0)
	PerModuleHelp( interp, argc, argv);
    else
	module_usage(stderr);

    /**
     **  Return on success
     **/

    return( TCL_OK);

} /** End of 'ModuleCmd_Help' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		PerModuleHelp					     **
 ** 									     **
 **   Description:	Print module specific help			     **
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
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	PerModuleHelp(	Tcl_Interp	*interp,
		    		int         	 argc,
		    		char		*argv[])
{
    Tcl_Interp	*help_interp;
    Tcl_DString	 cmdbuf;
    int		 i,
    		 result;
    char	 modulefile[ MOD_BUFSIZE];
    char	 modulename[ MOD_BUFSIZE];
    
    /**
     **  Initialize the command buffer
     **/
    Tcl_DStringInit( &cmdbuf);
    g_flags |= M_HELP;

    /**
     **  Handle each passed module file. Create a Tcl interpreter for each 
     **  module file to be handled
     **/
    for(i=0; i<argc; i++) {

	help_interp = EM_CreateInterp();
	if( TCL_OK != (result = Module_Init( help_interp))) {
	    EM_DeleteInterp( help_interp);
	    result = TCL_ERROR;
	    break;
	}

	/**
	 **  locate the filename related to the passed module
	 **/

	if( Locate_ModuleFile( help_interp, argv[i], modulename, modulefile)) {
	    if( OK != ErrorLogger( ERR_LOCATE, LOC, argv[i], NULL)) {
		g_retval = 1;  
		continue;
	}
	    EM_DeleteInterp( help_interp);
	}

	/**
	 **  Now print the module specific help ...
	 **/

        g_current_module = modulename;
	fprintf( stderr,
		 "\n----------- %s '%s' %.*s-------\n\n", 
		 _("Module Specific Help for"),
		 g_current_module, (int)(20-strlen( g_current_module)),
		 "--------------------");
	result = CallModuleProcedure( help_interp, &cmdbuf, modulefile,
	    "ModulesHelp", 1);

	/**
	 **  If there hasn't been any help ...
	 **/

	if( result == TCL_ERROR)
	    fprintf( stderr, "\t*** %s %s ***\n", 
		_("No Module Specific Help for"), 
		g_current_module);

	/**
	 **  Finally clear up the Tcl interpreter and handle the next module
	 **/

	EM_DeleteInterp( help_interp);
    }

    /**
     **  Free the used command buffer and return on success
     **/

    g_flags &= ~M_HELP;
    Tcl_DStringFree(&cmdbuf);

    return( TCL_OK);

} /** End of 'PerModuleHelp' **/
