/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Use.c					     **
 **   First Edition:	1991/10/23					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Use.c,v 1.6.20.4 2011/11/28 21:13:15 rkowen Exp $";
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


#ifdef BEGINENV
/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		append_to_modulesbeginenv			     **
 ** 									     **
 **   Description:	Append the passed variable (with value) to the begin-**
 **			ning environment				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
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

    if(	var
#if BEGINENV == 99
	&& Tcl_GetVar2( interp,"env","MODULESBEGINENV", TCL_GLOBAL_ONLY)
#endif
	) {

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_DEBUG, LOC, "Adding '", var, "'");
#endif

	/**
	 **  Get filename and the value of the passed variable
	 **/

	filename = EMGetEnv( interp, "_MODULESBEGINENV_");
	if(filename && *filename) {
	    val = EMGetEnv( interp, var);

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
	null_free((void *)&filename);
    } /** if( var passed) **/

} /** End of 'append_to_modulesbeginenv' **/
#else
#  define append_to_modulesbeginenv( a, b) {}
#endif

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Use					     **
 ** 									     **
 **   Description:	Execution of the module-command 'use'		     **
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
	 **  Check for -a|--append flag (if in modulefile - it's not parsed
	 **	by getoptlong) (keep -append for backward compatibility)
	 **/
	if( (!strcmp("-a",argv[i])) || (!strcmp("--append",argv[i]))
	|| (!strcmp("-append",argv[i]))) {

		pathargv[0] = "append-path";
		continue;

	} else if( stat( argv[i], &stats) < 0) {
	/**
	 **  Check for existing, readable directories
	 **/
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

	if( cmdSetPath((ClientData) 0, interp, 3, (CONST84 char **) pathargv)
	== TCL_ERROR)
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

    } /** for **/
  
    /**
     **  Add the new value of MODULESPATH to the end
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
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
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
	if(cmdRemovePath((ClientData) 0, interp, 3, (CONST84 char **) pathargv)
	== TCL_ERROR)
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    } /** for **/
  
    /**
     **  Add the new value of MODULESPATH to the end
     **  of the beginenvcache so that update will be able to find its
     **  modulefiles.
     **/

    append_to_modulesbeginenv( interp, "MODULEPATH");

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_UnUse, NULL);
#endif

    return( TCL_OK);
  
} /** End of 'ModuleCmd_UnUse' **/


