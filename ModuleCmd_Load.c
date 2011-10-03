/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Load.c				     **
 **   First Edition:	1991/10/23					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Load.c,v 1.8.4.3 2011/10/03 19:31:52 rkowen Exp $";
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
 **   First Edition:	1991/10/23					     **
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
 **   Attached Globals:	g_specified_module	The module name from the     **
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
    EM_RetVal		  em_return_val = EM_OK;

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Load, NULL);
#endif

    /**
     **  Set up the flags controling the Tcl callback functions
     **/

    if( load) {
        g_flags |= M_LOAD;
        g_flags &= ~M_REMOVE;
    } else {
        g_flags |= M_REMOVE;
        g_flags &= ~M_LOAD;
    }
    
    /**
     **  Handle all module files in the order they are passed to me
     **/

    for( i=0; i<argc && argv[i]; i++) {
	/**
	 ** Set the name of the module specified on the command line
	 **/

	g_specified_module = argv[i];

	/**
	 **  Create a Tcl interpreter and initialize it with the module commands
	 **/

        tmp_interp = EM_CreateInterp();

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
                if ((char *) NULL == stringer(modulename, MOD_BUFSIZE,
			tmpname, NULL))
		    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
			goto unwind0;
                if( !filename[0])
                    if( TCL_ERROR == (return_val = Locate_ModuleFile(
			tmp_interp, argv[i], tmpname, filename))) 
			ErrorLogger( ERR_LOCATE, LOC, argv[i], NULL);

                /**
                 **  If IsLoaded() created tmpname, then we must free it.
                 **/

                if( tmpname && (tmpname != argv[i]))
                    null_free((void *) &tmpname);

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
	if( TCL_OK == return_val) {
	    return_val = Read_Modulefile(tmp_interp, filename);
	    em_return_val = ReturnValue(tmp_interp, return_val);

	    switch (em_return_val) {
	    case EM_OK:
		/**
		 ** If module terminates TCL_OK, add it to the loaded list ...
		 **/
	    case EM_CONTINUE:
		/**
		 ** If module terminates TCL_CONTINUE, add it to the
		 ** loaded list ... but further processing of that module
		 ** has ceased
		 **/
		Tcl_ResetResult(tmp_interp);
		Update_LoadedList( tmp_interp, modulename, filename);

		/**
		 **  Save the current environment setup before the next module
		 **  file is (un)loaded in case something is broken ...
		 **  ... for Unwind_Modulefile_Changes later on
		 **/
		Tcl_ResetResult(tmp_interp);
        	if( oldTables) {
                    Delete_Hash_Tables( oldTables);
                    null_free((void *) &oldTables);
        	}
        	oldTables = Copy_Hash_Tables();
		a_successful_load = 1;
		break;	/* switch */

	    case EM_BREAK:
		/**
		 ** If module terminates TCL_BREAK, don't add it to the list,
		 ** but assume that everything was OK with the module anyway.
		 ** (The original code wasn't correct)
		 **/
	    case EM_EXIT:
		/**
		 ** If module terminates TCL_EXIT, find the return value
		 ** and exit modulecmd with that ... also none of the
		 ** following modules should be loaded.
		 **/
	    case EM_ERROR:
	    default:
		/**
		 **  Reset what has been changed.
		 **/
		Unwind_Modulefile_Changes( tmp_interp, oldTables);
            
        	oldTables = NULL;
		return_val = TCL_ERROR;
		break;	/* switch */
	    }
	}
        EM_DeleteInterp(tmp_interp);

	/* do not process any further modulefiles if EM_EXIT */
	if (em_return_val == EM_EXIT) break; /* for */

    } /** for **/

    /**
     **  There may only be a spare save environment left, if the final module
     **  has been load successfully. Remove it in this case
     **/
    if( em_return_val == EM_OK && oldTables) {
        Delete_Hash_Tables( oldTables);
        null_free((void *) &oldTables);
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

unwind0:
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'ModuleCmd_Load' **/
