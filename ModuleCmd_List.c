/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_List.c				     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, rk@owen.sj.ca.us				     **
 ** 									     **
 **   Description:	Lists the currently loaded modulefiles.		     **
 ** 									     **
 **   Exports:		ModuleCmd_List					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_List.c,v 1.3 2002/09/16 16:49:20 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"
#include "uvec.h"

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

static	char	module_name[] = "ModuleCmd_List.c";	/** File name of this module **/
static	char	_proc_ModuleCmd_List[] = "ModuleCmd_List";

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_List					     **
 ** 									     **
 **   Description:	Execution of the module-command 'list'		     **
 **			Lists all modules stored in the environment variable **
 **			'LOADEDMODULES'					     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of args	     **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successfull operation	     **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	ModuleCmd_List(	Tcl_Interp	*interp,
			int		 argc,
                   	char		*argv[])
{
    /**
     **  Get the list of loaded modules at first
     **/

    char	*loaded, *lmfiles;
    int		 i, count1, count2;
    uvec	*uvlist,		/* list unix vector */
    		*uvfiles;		/* file unix vector */

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_List, NULL);
#endif

    lmfiles = getLMFILES( interp);
    loaded = getenv( "LOADEDMODULES");
    
    if( !loaded || !*loaded) {
	if( sw_format & (SW_TERSE | SW_LONG | SW_HUMAN) )
	    fprintf(stderr, "No Modulefiles Currently Loaded.\n");
    } else {

	/**
	 **  Now tokenize it, form a list and print it out.
	 **/

	if( sw_format & SW_LONG ) {
	    fprintf( stderr, long_header);
	}
	if( sw_format & (SW_TERSE | SW_LONG | SW_HUMAN) )
	    fprintf( stderr, "Currently Loaded Modulefiles:\n");

	/**
	 **  LOADEDMODULES and _LMFILES_ should provide a list of loaded
	 **  modules and assigned files in the SAME ORDER
	 ** but double check, because if they aren't you will crash.
	 **/

	uvlist = uvec_alloc_(module_str_fns);
	uvec_copy_str(uvlist,":",loaded);
	count1 = uvec_number(uvlist);

	uvfiles = uvec_alloc_(module_str_fns);
	uvec_copy_str(uvfiles,":",lmfiles);
	count2 = uvec_number(uvfiles);

	if (count1 != count2) {
	  ErrorLogger( ERR_ENVVAR, LOC, NULL);
	}

	/** 
	 **  Print this guy
	 **/
	print_aligned_files(interp, NULL, NULL, uvec_vector(uvfiles),count1,1);

#if USE_FREE
	/**
	 ** free up vectors
	 **/
	uvec_dtor(&uvlist);
	uvec_dtor(&uvfiles);
#endif
    }

    /**
     **  Return on success
     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_List, NULL);
#endif

    return( TCL_OK);

} /** End of 'ModuleCmd_List' **/
