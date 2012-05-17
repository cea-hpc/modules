/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdSetenv.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description: 	The routines for setting and unsetting environment   **
 **			variables from within modulefiles.		     **
 ** 									     **
 **   Exports:		cmdSetEnv					     **
 **			cmdUnsetEnv					     **
 **			moduleSetenv					     **
 **			moduleUnsetenv					     **
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

static char Id[] = "@(#)$Id: cmdSetenv.c,v 1.6.22.4 2011/11/28 21:13:15 rkowen Exp $";
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

static	char	module_name[] = "cmdSetenv.c";	/** File name of this module **/

#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdSetEnv[] = "cmdSetEnv";
static	char	_proc_cmdUnsetEnv[] = "cmdUnsetEnv";
#endif
#if WITH_DEBUGGING_UTIL_1
static	char	_proc_moduleSetenv[] = "moduleSetenv";
static	char	_proc_moduleUnsetenv[] = "moduleUnsetenv";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdSetEnv					     **
 ** 									     **
 **   Description:	Callback function for the 'setenv' command	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
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

int	cmdSetEnv(	ClientData	 client_data,
	  		Tcl_Interp	*interp,
	  		int		 argc,
	  		CONST84 char	*argv[])
{
    int		 force;			/** Force removale of variables	     **/
    char	*var;			/** Varibales name		     **/
    char	*val;			/** Varibales value		     **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdSetEnv, NULL);
#endif

    /**
     **  Check parameters. Usage is:  [-force] variable value
     **/

    if( argc < 3 || argc > 4) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "[-force] variable value",
	    NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Get variables name and value from the argument array
     **/

    if( *argv[1] == '-') {
        if( !strncmp( argv[1], "-force", 6)) {
            force = 1;
            var = (char *) argv[2];
            val = (char *) argv[3];
        } else {
	    if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "[-force] variable value",
		NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
        }            
    } else  {
        force = 0;
        var = (char *) argv[1];
        val = (char *) argv[2];
    }

    moduleSetenv( interp, var, val, force);

    /**
     **  This has to be done after everything has been set because the
     **  variables may be needed later in the modulefile.
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
    }

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdSetEnv, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdSetEnv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		moduleSetenv					     **
 ** 									     **
 **   Description:	Set or unset environment variables		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **			char		*variable	Name of the variable **
 **			char		*value		Value to be set	     **
 **			int		 force		Force removal	     **
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

int	moduleSetenv(	Tcl_Interp	*interp,
             		char		*variable,
             		char		*value,
             		int  		 force)
{
    char	*oldval;			/** Old value of 'variable'  **/

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_moduleSetenv, NULL);
#endif

    oldval = EMGetEnv( interp, variable);
    if (!oldval || !*oldval)
	null_free((void *)&oldval);
  
    /**
     **  Check to see if variable is already set correctly... 
     **/

    if( !(g_flags & (M_REMOVE|M_DISPLAY|M_SWITCH|M_NONPERSIST)) && oldval) {
        if( !strcmp( value, oldval)) {
            return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
        }
    }

    /**
     **  If I'm in SWSTATE1, I'm removing stuff from the old modulefile, so
     **  I'll just mark the variables that were used with the SWSTATE1 flag and
     **  return.
     **
     **  When I come back through in SWSTATE2, I'm setting the variables that
     **  are in the new modulefile.  So, I'll keep track of these by marking
     **  them as touched by SWSTATE2 and then actually setting their values in
     **  the environment down below.
     **
     **  Finally, in SWSTATE3, I'll check to see if the variables in the old
     **  modulefiles that have been marked are still marked as SWSTATE1.  If
     **  they are still the same, then I'll just unset them and return.
     **
     **  And, if I'm not doing any switching, then just unset the variable if
     **  I'm in remove mode. 
     **/

    if( g_flags & M_SWSTATE1) {
        set_marked_entry( markVariableHashTable, variable, M_SWSTATE1);
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
    } else if( g_flags & M_SWSTATE2) {
        set_marked_entry( markVariableHashTable, variable, M_SWSTATE2);
    } else if( g_flags & M_SWSTATE3) {
        intptr_t marked_val;
        marked_val = chk_marked_entry( markVariableHashTable, variable);
        if( marked_val) {
            if( marked_val == M_SWSTATE1)
                return( moduleUnsetenv(interp, variable));	/** -------> **/
            else
		return( TCL_OK);	/** -------- EXIT (SUCCESS) -------> **/
        }
    } else if( (g_flags & M_REMOVE) && !force) {
	return( moduleUnsetenv( interp, variable));		/** -------> **/
    }

    /**
     **  Keep track of our changes just in case we have to bail out and restore
     **  the environment.
     **/

    if( !(g_flags & (M_NONPERSIST | M_DISPLAY | M_WHATIS | M_HELP))) {
        store_hash_value( setenvHashTable, variable, value);
        clear_hash_value( unsetenvHashTable, variable);
    }

    /**
     **  Store the value into the environment
     **/

    EMSetEnv( interp, variable, value);

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_moduleSetenv, NULL);
#endif

    return( TCL_OK);

} /** End of 'moduleSetenv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdUnsetEnv					     **
 ** 									     **
 **   Description:	Callback function for the 'unset' command	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
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

int	cmdUnsetEnv(	ClientData	 client_data,
	  		Tcl_Interp	*interp,
	  		int		 argc,
	  		CONST84 char	*argv[])
{
    /**
     **  Parameter check. The name of the variable has to be specified
     **/
    
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdUnsetEnv, NULL);
#endif

    if( argc < 2 || argc > 3) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "variable [value]",
	    NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

  
    /**
     **  Non-persist mode?
     **/
    
    if (g_flags & M_NONPERSIST) {
	return (TCL_OK);
    }

    /**
     **  Unset the variable or just display what to do ...
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
    } else if( g_flags & M_REMOVE && argc == 3) {
	int save_flags = g_flags;
	/** allow an optional 3rd argument to set the env.var. to on removal **/
	g_flags = (g_flags & ~M_REMOVE) | M_LOAD;
	moduleSetenv( interp, (char *) argv[1], (char *) argv[2], 0);
	g_flags = save_flags;
    } else {
	moduleUnsetenv( interp, (char *) argv[1]);
    }

    /**
     **  Return on success
     **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdUnsetEnv, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdUnsetEnv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		moduleUnsetenv					     **
 ** 									     **
 **   Description:	Unset environment variables			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **			char		*variable	Name of the variable **
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

int	moduleUnsetenv(	Tcl_Interp	*interp,
             		char		*variable)
{

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_moduleUnsetenv, NULL);
#endif

    /**
     ** Don't unset the variable in Tcl Space.
     ** If module writer *REALLY* wants it gone, use $env
     **/

    if( !(g_flags & (M_NONPERSIST | M_DISPLAY | M_WHATIS | M_HELP))) {
        store_hash_value( unsetenvHashTable, variable, NULL);
        clear_hash_value( setenvHashTable, variable);
    }
  
#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_moduleUnsetenv, NULL);
#endif

    return( TCL_OK);

} /** end of 'moduleUnsetenv' **/

