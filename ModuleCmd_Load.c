/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Load.c				     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The load and unload procedure that takes care of     **
 **			adding and removing modulefiles to and from the	     **
 **			user's environment.				     **
 ** 									     **
 **   Exports:		ModuleCmd_Load					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Load.c,v 1.3 2001/06/09 09:48:46 rkowen Exp $";
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

static	char	module_name[] = "ModuleCmd_Load.c";	/** File name of this module **/

#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Load[] = "ModuleCmd_Load";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Load					     **
 ** 									     **
 **   Description:	Execution of the module-commands 'load' and 'unload' **
 ** 									     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 load		Controls 'load' or   **
 **							'unload' to be done  **
 **			int		 argc		Number of arguments  **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	0		None of the passed modules   **
 **						has been load		     **
 **				1		At least one module has been **
 **						read			     **
 ** 									     **
 **   Attached Globals:	specified_module	The module name from the     **
 **						command line		     **
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	ModuleCmd_Load(	Tcl_Interp	*interp,
               		int		 load,
               		int		 argc,
               		char		*argv[])
{
    int			  i,
    			  return_val,		/** Subroutine return values **/
    			  a_successful_load = 0;	/** Command return   **/
    char		  filename[ MOD_BUFSIZE],	/** Module filename  **/
    			  modulename[ MOD_BUFSIZE];	/** Real module name **/
    Tcl_Interp		 *tmp_interp;		/** Tcl interpreter to be    **/
						/** used internally	     **/
    Tcl_HashTable	**oldTables = NULL;

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Load, NULL);
#endif

    /**
     **  Set up the flags controling the Tcl callback functions
     **/

    if( load)
        g_flags |= M_LOAD;
    else
        g_flags |= M_REMOVE;
    
    /**
     **  Handle all module files in the order they are passed to me
     **/

    for( i=0; i<argc && argv[i]; i++) {
	/**
	 ** Set the name of the module specified on the command line
	 **/

	specified_module = argv[i];

	/**
	 **  Create a Tcl interpreter and initialize it with the module commands
	 **/

        tmp_interp = Tcl_CreateInterp();
	if( TCL_OK != (return_val = InitializeModuleCommands( tmp_interp)))
	    return( return_val);	/** -------- EXIT (FAILURE) -------> **/
        filename[0] = '\0';

	/**
	 **  UNLOAD to be done
	 **  At first check if it is loaded ...
	 **/

        if( !load) {

            char	*tmpname;

            if( !IsLoaded( tmp_interp, argv[i], &tmpname, filename)) {
#if 0
		/** do we really care if it's not loaded ... **/
		if( OK != ErrorLogger( ERR_NOTLOADED, LOC, argv[i], NULL))
#endif
		    return_val = TCL_ERROR;
            } else {

		/**
		 **  So it is loaded ...
		 **  Do we know the filename?
		 **/

                strcpy( modulename, tmpname);
                if( !filename[0])
                    if( TCL_ERROR == (return_val = Locate_ModuleFile(
			tmp_interp, argv[i], tmpname, filename))) 
			ErrorLogger( ERR_LOCATE, LOC, argv[i], NULL);

                /**
                 **  If IsLoaded() created tmpname, then we must free it.
                 **/

                if( tmpname && tmpname != argv[i]) 
                    free( tmpname);

            } /** if loaded **/

	/**
	 **  LOAD to be done
	 **  Only check the filename
	 **/

        } else {

	    if( TCL_ERROR == (return_val = Locate_ModuleFile( tmp_interp,
		argv[i], modulename, filename)))
		ErrorLogger( ERR_LOCATE, LOC, argv[i], NULL);
        }

        /**
         **  If return_val has been set to something other than TCL_OK,
         **  then read_status should not be set -- by the ANSI definition.
	 **
	 **  The functions Read_Modulefile and Update_LoadedList will take 
	 **  respect to the 'flags' which are set to M_LOAD or M_REMOVE
	 **  according to the intention of calling this procedure.
         **/

	g_current_module = modulename;
	if( TCL_OK == return_val &&
            0 == Read_Modulefile( tmp_interp, filename)) {

	    Update_LoadedList( tmp_interp, modulename, filename);

	    /**
	     **  Save the current environment setup before the next module
	     **  file is (un)loaded in case something is broken ...
	     **  ... for Unwind_Modulefile_Changes later on
	     **/

            if( oldTables) {
                Delete_Hash_Tables( oldTables);
                free((char*) oldTables);
            }
            oldTables = Copy_Hash_Tables();
            a_successful_load = 1;

	} else {

	    /**
	     **  Reset what has been changed.
	     **/

	    Unwind_Modulefile_Changes( tmp_interp, oldTables);
            
            oldTables = NULL;
	    return_val = TCL_ERROR;
	}

        Tcl_DeleteInterp(tmp_interp);

    } /** for **/
    
    /**
     **  There may only be a spare save environment left, if the final module
     **  has been load successfully. Remove it in this case
     **/

    if( return_val == TCL_OK && oldTables) {
        Delete_Hash_Tables( oldTables);
        free((char*) oldTables);
    }

    /**
     **  Clean up the flags and return 
     **/

    if( load)
        g_flags &= ~M_LOAD;
    else
        g_flags &= ~M_REMOVE;

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Load, NULL);
#endif

    return( a_successful_load);

} /** End of 'ModuleCmd_Load' **/
