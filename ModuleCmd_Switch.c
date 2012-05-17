/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Switch.c				     **
 **   First Edition:	1991/10/23					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Switch.c,v 1.7.22.1 2010/11/11 18:23:18 rkowen Exp $";
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
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals:	g_flags			Controlling the callback     **
 **						functions.		     **
 **			g_specified_module	The module name from the     **
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
		*oldfile,
		*newfile,
		*oldname,
		*newname,
                *oldmodule_buffer	= (char *) NULL;
    int		 ret_val = TCL_OK;
    
#if WITH_DEBUGGING_MODULECMD
    fprintf( stderr, "ModuleCmd_Switch(%d):DEBUG: Starting\n", __LINE__);
#endif
    /**
     ** allocate buffer memory
     **/
    if ((char *) NULL == (oldfile = stringer(NULL, MOD_BUFSIZE, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;

    if ((char *) NULL == (newfile = stringer(NULL, MOD_BUFSIZE, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind1;

    if ((char *) NULL == (oldname = stringer(NULL, MOD_BUFSIZE, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind2;

    if ((char *) NULL == (newname = stringer(NULL, MOD_BUFSIZE, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind3;

    /**
     **  Parameter check. the required syntax is:
     **    module switch [ <old> ] <new>
     **  If <old> is not specified, then the pathname of <new> is assumed.
     **/

    if( argc == 1) {
      newmodule = argv[0];
      if((char *) NULL == (oldmodule_buffer = stringer(NULL,0,newmodule,NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
          goto unwind4;

      /* starting from the end of the module name, find the first
       * forward slash and replace with null 
       */
      if ((oldmodule = strrchr(oldmodule_buffer, '/'))) {
	  *oldmodule = 0;
      }
      oldmodule = oldmodule_buffer;
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

    g_specified_module = oldmodule;

    /**
     **  First try to find a match for the modulefile out of the LOADEDMODULES.
     **/

    if( !IsLoaded( interp, oldmodule, &realname, oldfile)) 
	if( OK != ErrorLogger( ERR_NOTLOADED, LOC, oldmodule, NULL))
	    goto unwind4;
    
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
		goto unwind4;

	/**
	 **  OK, this one is known. Is it loaded, too?
	 **/

        if( !IsLoaded( interp, oldname, NULL, oldfile)) 
	    if( OK != ErrorLogger( ERR_NOTLOADED, LOC, oldmodule, NULL))
		goto unwind4;
    }

    /**
     ** Set the name of the module specified on the command line
     **/

    g_specified_module = newmodule;

    /**
     **  Now try to find the new file to swap with.
     **/

    if( TCL_ERROR == (ret_val = Locate_ModuleFile( interp, newmodule, newname,
	newfile)))
	if( OK != ErrorLogger( ERR_LOCATE, LOC, newmodule, NULL))
	    goto unwind4;
    
    ErrorLogger( NO_ERR_VERBOSE, LOC, "Switching '$1' to '$2'", oldmodule,
	newmodule, NULL);

    /**
     **  We'll remove the current modulefile with the SWITCH1 state set.
     **  This means that instead of really removing the paths, markers will
     **  be put in its place for later use.
     **/

    g_flags |= (M_REMOVE | M_SWSTATE1);
    
    g_specified_module = oldmodule;
    g_current_module = oldname;
    if( Read_Modulefile( interp, oldfile) == 0)
	Update_LoadedList( interp, oldname, oldfile);
    else {
        ErrorLogger( NO_ERR_VERBOSE, LOC, "failed", NULL);
	goto unwind4;
    }
    
    g_flags &= ~(M_REMOVE | M_SWSTATE1);

    /**
     **  Move on to state SWITCH2.  This loads the modulefile at the append
     **  and prepend markers.
     **/

    g_flags |= M_SWSTATE2;

    g_specified_module = newmodule;
    g_current_module = newname;
    if( Read_Modulefile( interp, newfile) == 0)
	Update_LoadedList( interp, newname, newfile);
    else {
        ErrorLogger( NO_ERR_VERBOSE, LOC, "failed", NULL);
	goto unwind4;
    }

    g_flags &= ~M_SWSTATE2;

    /**
     **  This actually unsets environment variables and gets rid of the
     **  markers.
     **/

    g_flags |= (M_REMOVE | M_SWSTATE3);

    g_specified_module = oldmodule;
    g_current_module = oldname;
    if( Read_Modulefile( interp, oldfile) == 0)
	Update_LoadedList( interp, newname, newfile);
    else {
        ErrorLogger( NO_ERR_VERBOSE, LOC, "failed", NULL);
	goto unwind4;
    }
 
    /**
     **  Return on success
     **/

    ErrorLogger( NO_ERR_VERBOSE, LOC, "done", NULL);

#if WITH_DEBUGGING_MODULECMD
    fprintf( stderr, "ModuleCmd_Switch(%d):DEBUG: End\n", __LINE__);
#endif
    /**
     ** free space
     **   assume don't need what's pointed to by g_current_module
     **   and g_specified_module
     **/
    null_free((void *) &newname);
    null_free((void *) &oldname);
    null_free((void *) &newfile);
    null_free((void *) &oldfile);

    return( TCL_OK);			/** ------- EXIT (SUCCESS) --------> **/

unwind4:
    if (oldmodule == oldmodule_buffer)
    	null_free((void *) &oldmodule);
    null_free((void *) &newname);
unwind3:
    null_free((void *) &oldname);
unwind2:
    null_free((void *) &newfile);
unwind1:
    null_free((void *) &oldfile);
unwind0:
    return( TCL_ERROR);			/** ------- EXIT (FAILURE) --------> **/

} /** End of 'ModuleCmd_Switch' **/

