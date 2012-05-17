/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_List.c				     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
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

static char Id[] = "@(#)$Id: ModuleCmd_List.c,v 1.5.18.1 2010/11/11 18:23:18 rkowen Exp $";
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
 **				TCL_OK		Successful operation	     **
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
    char	*list[ MOD_BUFSIZE];
    char	*files[ MOD_BUFSIZE];
    char	*tmplist[ MOD_BUFSIZE], *s;
    int	  	 len;

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
	 ** but double check, because if they aren't you will get a crash.
	 **/

	count1 = 1;
        for( list[ 0] = xstrtok( loaded, ":");
	     list[ count1] = xstrtok( NULL, ":");
	     count1++ );

	count2 = 1;
        for( files[ 0] = xstrtok( lmfiles, ":");
	     files[ count2] = xstrtok( NULL, ":");
	     count2++ );
	if (count1 != count2) {
	  ErrorLogger( ERR_ENVVAR, LOC, NULL);
	}
	  

	/**
	 **  We have to build a single list of files for each loaded entry
	 **  in order to be able to figure out the length of the directory
	 **  part
	 **/

	for( i=0; i<count1; i++) {

	    len = strlen( files[i]) - strlen( list[i]);
	    tmplist[i] = files[i];

	    /**
	     **  We have to source all relevant .modulerc and .version files
	     **  on the path
	     **/

	    s = files[i] + len;
	    while( s) {
		if( s = strchr( s, '/'))
		    *s = '\0';

		SourceRC( interp, files[i], modulerc_file);
		SourceVers( interp, files[i], list[i]);

		if( s)
		    *s++ = '/';
	    }

	    /** 
	     **  Print this guy
	     **/
	}
	print_aligned_files( interp, NULL, NULL, tmplist, count1, 1);
    }

    /**
     **  Return on success
     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_List, NULL);
#endif

    return( TCL_OK);

} /** End of 'ModuleCmd_List' **/
