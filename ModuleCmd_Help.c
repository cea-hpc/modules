/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Help.c				     **
 **   First Edition:	91/10/23					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Help.c,v 1.1 2000/06/28 00:17:32 rk Exp $";
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

static	char	module_name[] = "ModuleCmd_Help.c";	/** File name of this module **/

#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Help[] = "ModuleCmd_Help";
#endif

#if WITH_DEBUGGING_MODULECMD || WITH_DEBUGGING_UTIL_1
static	char	_proc_PerModuleHelp[] = "PerModuleHelp";
#endif

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

int  ModuleCmd_Help(	Tcl_Interp	*interp,
		    	int         	 argc,
		    	char		*argv[])
{
    /**
     **  General or module specific help wanted. General help is done within
     **  here. In case of module specific help we'll call a subroutine to do
     **  it ...
     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Help, NULL);
#endif

    if( argc > 0) {
	PerModuleHelp( interp, argc, argv);
    } else {
	fprintf( stderr,
		"\n  Modules Release %s (Copyright GNU GPL v2 1991):\n",
                version_string);
	
	fprintf( stderr, "  Available Commands and Usage:\n");
	
	fprintf( stderr, "\t+ add|load\t\tmodulefile [modulefile ...]\n");
	fprintf( stderr, "\t+ rm|unload\t\tmodulefile [modulefile ...]\n");
	fprintf( stderr, "\t+ switch|swap\t\tmodulefile1 modulefile2\n");
	fprintf( stderr, "\t+ display|show\t\tmodulefile [modulefile ...]\n");
	fprintf( stderr, "\t+ avail\t\t\t[modulefile [modulefile ...]]\n");
	fprintf( stderr, "\t+ use [-append]\t\tdir [dir ...]\n");
	fprintf( stderr, "\t+ unuse\t\t\tdir [dir ...]\n");
	fprintf( stderr, "\t+ update\n");
	fprintf( stderr, "\t+ purge\n");
	fprintf( stderr, "\t+ list\n");
	fprintf( stderr, "\t+ clear\n");
	fprintf( stderr, "\t+ help\t\t\t[modulefile [modulefile ...]]\n");
	fprintf( stderr, "\t+ whatis\t\t[modulefile [modulefile ...]]\n");
	fprintf( stderr, "\t+ apropos|keyword\tstring\n");
	fprintf( stderr, "\t+ initadd\t\tmodulefile [modulefile ...]\n");
	fprintf( stderr, "\t+ initprepend\t\tmodulefile [modulefile ...]\n");
	fprintf( stderr, "\t+ initrm\t\tmodulefile [modulefile ...]\n");
	fprintf( stderr, "\t+ initswitch\t\tmodulefile1 modulefile2\n");
	fprintf( stderr, "\t+ initlist\n");
	fprintf( stderr, "\t+ initclear\n\n");
    }

    /**
     **  Return on success
     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Help, NULL);
#endif

    return( TCL_OK);

} /** End of 'ModuleCmd_Help' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		PerModuleHelp					     **
 ** 									     **
 **   Description:	Print module specific help			     **
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
    
#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_PerModuleHelp, NULL);
#endif

    /**
     **  Initialize the command buffer
     **/

    Tcl_DStringInit( &cmdbuf);
    flags |= M_HELP;

    /**
     **  Handle each passed module file. Create a Tcl interpreter for each 
     **  module file to be handled
     **/

    for(i=0; i<argc; i++) {

	help_interp = Tcl_CreateInterp();
	if( TCL_OK != (result = InitializeModuleCommands( help_interp))) {
	    Tcl_DeleteInterp( help_interp);
	    result = TCL_ERROR;
	    break;
	}

	/**
	 **  locate the filename related to the passed module
	 **/

	if( Locate_ModuleFile( help_interp, argv[i], modulename, modulefile)) {
	    if( OK != ErrorLogger( ERR_LOCATE, LOC, argv[i], NULL))
		continue;
	}

	/**
	 **  Now print the module specific help ...
	 **/

        current_module = modulename;
	fprintf( stderr,
		 "\n----------- Module Specific Help for '%s' %.*s-------\n\n", 
		 current_module, (int)(20-strlen( current_module)),
		 "--------------------");
	result = CallModuleProcedure( help_interp, &cmdbuf, modulefile,
	    "ModulesHelp", 1);

	/**
	 **  If there hasn't been any help ...
	 **/

	if( result == TCL_ERROR)
	    fprintf( stderr, "\t*** No Module Specific Help for %s ***\n", 
		current_module);

	/**
	 **  Finally clear up the Tcl interpreter and handle the next module
	 **/

	Tcl_DeleteInterp( help_interp);
    }

    /**
     **  Free the used command buffer and return on success
     **/

    flags &= ~M_HELP;
    fprintf(stderr, "\n");
    Tcl_DStringFree(&cmdbuf);

    return( TCL_OK);

} /** End of 'PerModuleHelp' **/
