/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Use.c					     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	Prepends (and appends) directories to the MODULEPATH **
 **			environment variable to enable access to more	     **
 **			modulefiles.					     **
 ** 									     **
 **   Exports:		ModuleCmd_Use					     **
 **			ModuleCmd_UnUse					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Use.c,v 1.2 2001/06/09 09:48:46 rkowen Exp $";
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

static	char	module_name[] = "ModuleCmd_Use.c";	/** File name of this module **/
#if WITH_DEBUGGING_UTIL_1
static	char	_proc_append_to_modulesbeginenv[] = "append_to_modulesbeginenv";
#endif
#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Use[] = "ModuleCmd_Use";
static	char	_proc_ModuleCmd_UnUse[] = "ModuleCmd_UnUse";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	void	append_to_modulesbeginenv( Tcl_Interp*, char*);


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		append_to_modulesbeginenv			     **
 ** 									     **
 **   Description:	Append the passed variable (with value) to the begin-**
 **			ning environment				     **
 ** 									     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			char 		*var		Name of the variable **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	void	append_to_modulesbeginenv(	Tcl_Interp	*interp,
						char		*var)
{
    char	*filename,		/** The filename, where the begin-   **/
					/** ning environment resides	     **/
		*val;			/** Value of the passed variable     **/
    FILE	*file;			/** File read handle		     **/

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_append_to_modulesbeginenv, NULL);
#endif

    if( var) {

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_DEBUG, LOC, "Adding '", var, "'");
#endif

	/**
	 **  Get filename and the value of the passed variable
	 **/

	if( filename = Tcl_GetVar2( interp, "env", "_MODULESBEGINENV_",
	    TCL_GLOBAL_ONLY)) {
	    val = Tcl_GetVar2( interp, "env", var, TCL_GLOBAL_ONLY);

	    /**
	     **  Append the string <var>=<value>
	     **/

            if( NULL != (file = fopen( filename, "a+"))) {
                fprintf( file, "%s=%s\n", var, val);
                if( EOF == fclose( file))
		    ErrorLogger( ERR_CLOSE, LOC, filename, NULL);
            } else {
		ErrorLogger( ERR_OPEN, LOC, filename, "appending", NULL);
	    }

        } /** if( get filename) **/
    } /** if( var passed) **/

} /** End of 'append_to_modulesbeginenv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Use					     **
 ** 									     **
 **   Description:	Execution of the module-command 'use'		     **
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
 **   Attached Globals:	g_flags		Controllig the callback functions    **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int  ModuleCmd_Use(	Tcl_Interp	*interp,
		   	int		 argc,
		   	char		*argv[])
{
    struct stat	 stats;			/** Buffer for the stat() systemcall **/
    char	*pathargv[4];		/** Argument buffer for Tcl calls    **/
    int		 i;
  
    /**
     **  Parameter check. Usage is 'module use <path> [ <path> ... ]'
     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Use, NULL);
#endif

    if( argc < 1) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, "use [-a|--append] dir [dir ...]", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Remove is done by another subroutine
     **/

    if( g_flags & M_REMOVE) 
	return( ModuleCmd_UnUse( interp, argc, argv));
      
    /**
     **  Append or prepend the new module path
     **/

    if( append_flag ) {
	pathargv[0] = "append-path";
    } else {
	pathargv[0] = "prepend-path";
    }

    /**
     **  Append (prepend) all passed paths to MODULEPATH in case they do exist,
     **  and are readable directories
     **/

    pathargv[1] = "MODULEPATH";
    pathargv[3] = NULL;

    for( i=0; i < argc; i++) {

	/**
	 **  Check for existing, readable directories
	 **/

	if( stat( argv[i], &stats) < 0) {
	    if( OK != ErrorLogger( ERR_DIRNOTFOUND, LOC, argv[i], NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	} else if( !S_ISDIR( stats.st_mode))  {
	    if( OK != ErrorLogger( ERR_NODIR, LOC, argv[i], NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	}

	/**
	 **  Used the 'cmdSetPath' callback function to modify the MODULEPATH
	 **/

	pathargv[2] = argv[i];

	if( cmdSetPath((ClientData) 0, interp, 3, pathargv) == TCL_ERROR)
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

    } /** for **/
  
    /**
     **  What I'm going to do here is add the new value of MODULESPATH to the end
     **  of the beginenvcache so that update will be able to find its
     **  modulefiles.
     **/

    append_to_modulesbeginenv( interp, "MODULEPATH");

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Use, NULL);
#endif

    return( TCL_OK);

} /** End of 'ModuleCmd_Use' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_UnUse					     **
 ** 									     **
 **   Description:	Execution of the module-command 'unuse'		     **
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
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int  ModuleCmd_UnUse(	Tcl_Interp	*interp,
		     	int		 argc,
		     	char		*argv[])
{
    char	*pathargv[4];
    int		 i = 0;
  
#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_UnUse, NULL);
#endif

    /**
     **  Parameter check. Usage is 'module use <path> [ <path> ... ]'
     **/

    if( argc < 1) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, "unuse dir [dir ...]", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
  
    /**
     **  Remove all passed paths from MODULEPATH
     **	 Use the 'cmdSetPath' callback function to modify the MODULEPATH
     **/

    pathargv[0] = "remove-path";
    pathargv[1] = "MODULEPATH";
    pathargv[3] = NULL;

    for(i = 0; i < argc; i++) {

	pathargv[2] = argv[i];
	if( cmdRemovePath((ClientData) 0, interp, 3, pathargv) == TCL_ERROR)
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

    } /** for **/
  
    /**
     **  What I'm going to do here is add the new value of MODULESPATH to the end
     **  of the beginenvcache so that update will be able to find its
     **  modulefiles.
     **/

    append_to_modulesbeginenv( interp, "MODULEPATH");

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_UnUse, NULL);
#endif

    return( TCL_OK);
  
} /** End of 'ModuleCmd_UnUse' **/


