/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Switch.c				     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	Switches two modulefiles such that the paths are     **
 **			switched in-place.				     **
 ** 									     **
 **   Exports:		ModuleCmd_Switch				     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Switch.c,v 1.3 2002/04/29 18:45:25 lakata Exp $";
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

static	char	module_name[] = "ModuleCmd_Switch.c";	/** File name of this module **/

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Switch				     **
 ** 									     **
 **   Description:	Execution of the module-command 'switch'	     **
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
 **   Attached Globals:	g_flags			Controlling the callback     **
 **						functions.		     **
 **			specified_module	The module name from the     **
 **						command line.		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	ModuleCmd_Switch(	Tcl_Interp	*interp,
                 		int		 argc,
                 		char		*argv[])
{
    char	*oldmodule,
		*newmodule,
		*realname,
		 oldfile[ MOD_BUFSIZE],
		 newfile[ MOD_BUFSIZE],
		 oldname[ MOD_BUFSIZE],
		 newname[ MOD_BUFSIZE],
                 oldmodule_buffer[ MOD_BUFSIZE];
    int		 ret_val = TCL_OK;
    
#if WITH_DEBUGGING_MODULECMD
    fprintf( stderr, "ModuleCmd_Switch(%d):DEBUG: Starting\n", __LINE__);
#endif

    /**
     **  Parameter check. the required syntax is:
     **    module switch [ <old> ] <new>
     **  If <old> is not specified, then the pathname of <new> is assumed.
     **/

    if( argc == 1) {
      char* o;
      oldmodule = oldmodule_buffer;
      newmodule = argv[0];
      strncpy(oldmodule_buffer,newmodule,MOD_BUFSIZE);
      oldmodule_buffer[MOD_BUFSIZE-1]=0; /* terminate just in case */
      /* starting from the end of the module name, find the first
       * forward slash and replace with null 
       */
      for(o = &oldmodule_buffer[strlen(oldmodule_buffer)-1];
	  o>oldmodule_buffer;
	  o--) {
	if (*o == '/') {
	  *o = 0;
	  break;
	}
      }
    } else if( argc == 2) {
      oldmodule = argv[0];
      newmodule = argv[1];
    } else {
      if( OK != ErrorLogger( ERR_USAGE, LOC, "switch oldmodule newmodule",
			     NULL))
	return( TCL_ERROR);		/** ------- EXIT (FAILURE) --------> **/
    }


    /**
     ** Set the name of the module specified on the command line
     **/

    specified_module = oldmodule;

    /**
     **  First try to find a match for the modulefile out of the LOADEDMODULES.
     **/

    if( !IsLoaded( interp, oldmodule, &realname, oldfile)) 
	if( OK != ErrorLogger( ERR_NOTLOADED, LOC, oldmodule, NULL))
	    return( TCL_ERROR);		/** ------- EXIT (FAILURE) --------> **/
    
    /**
     **  If we have another name to try, try finding it on disk.
     **/

    if( realname) 
        ret_val = Locate_ModuleFile( interp, realname, oldname, oldfile);

    /**
     **  If we've made it this far without finding a file, then look using the
     **  exact name the user gave me -- i.e. the old method. 
     **/

    if( ret_val == TCL_ERROR) {

        if( TCL_ERROR == (ret_val = Locate_ModuleFile( interp, oldmodule,
	    oldname, oldfile)))
	    if( OK != ErrorLogger( ERR_LOCATE, LOC, oldmodule, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/

	/**
	 **  OK, this one is known. Is it loaded, too?
	 **/

        if( !IsLoaded( interp, oldname, NULL, oldfile)) 
	    if( OK != ErrorLogger( ERR_NOTLOADED, LOC, oldmodule, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
    }

    /**
     ** Set the name of the module specified on the command line
     **/

    specified_module = newmodule;

    /**
     **  Now try to find the new file to swap with.
     **/

    if( TCL_ERROR == (ret_val = Locate_ModuleFile( interp, newmodule, newname,
	newfile)))
	if( OK != ErrorLogger( ERR_LOCATE, LOC, newmodule, NULL))
	    return( TCL_ERROR);		/** ------- EXIT (FAILURE) --------> **/
    
    ErrorLogger( NO_ERR_VERBOSE, LOC, "Switching '$1' to '$2'", oldmodule,
	newmodule, NULL);

    /**
     **  We'll remove the current modulefile with the SWITCH1 state set.
     **  This means that instead of really removing the paths, markers will
     **  be put in its place for later use.
     **/

    g_flags |= (M_REMOVE | M_SWSTATE1);
    
    specified_module = oldmodule;
    g_current_module = oldname;
    if( Read_Modulefile( interp, oldfile) == 0)
	Update_LoadedList( interp, oldname, oldfile);
    else {
        ErrorLogger( NO_ERR_VERBOSE, LOC, "failed", NULL);
	return( TCL_ERROR);		/** ------- EXIT (FAILURE) --------> **/
    }
    
    g_flags &= ~(M_REMOVE | M_SWSTATE1);

    /**
     **  Move on to state SWITCH2.  This loads the modulefile at the append
     **  and prepend markers.
     **/

    g_flags |= M_SWSTATE2;

    specified_module = newmodule;
    g_current_module = newname;
    if( Read_Modulefile( interp, newfile) == 0)
	Update_LoadedList( interp, newname, newfile);
    else {
        ErrorLogger( NO_ERR_VERBOSE, LOC, "failed", NULL);
	return( TCL_ERROR);		/** ------- EXIT (FAILURE) --------> **/
    }

    g_flags &= ~M_SWSTATE2;

    /**
     **  This actually unsets environment variables and gets rid of the
     **  markers.
     **/

    g_flags |= (M_REMOVE | M_SWSTATE3);

    specified_module = oldmodule;
    g_current_module = oldname;
    if( Read_Modulefile( interp, oldfile) == 0)
	Update_LoadedList( interp, newname, newfile);
    else {
        ErrorLogger( NO_ERR_VERBOSE, LOC, "failed", NULL);
	return( TCL_ERROR);		/** ------- EXIT (FAILURE) --------> **/
    }
 
    /**
     **  Return on success
     **/

    ErrorLogger( NO_ERR_VERBOSE, LOC, "done", NULL);

#if WITH_DEBUGGING_MODULECMD
    fprintf( stderr, "ModuleCmd_Switch(%d):DEBUG: End\n", __LINE__);
#endif

    return( TCL_OK);

} /** End of 'ModuleCmd_Switch' **/

