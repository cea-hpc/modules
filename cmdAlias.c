/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdAlias.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The Tcl set-alias command			     **
 ** 									     **
 **   Exports:		cmdSetAlias					     **
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

static char Id[] = "@(#)$Id: cmdAlias.c,v 1.4.22.1 2010/11/11 18:23:18 rkowen Exp $";
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

static	char	module_name[] = "cmdAlias.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdSetAlias[] = "cmdSetAlias";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdSetAlias					     **
 ** 									     **
 **   Description:	Callback function for (re)setting aliases	     **
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

int cmdSetAlias( ClientData	 client_data,
	  	 Tcl_Interp	*interp,
	  	 int 		 argc,
	  	 CONST84 char	*argv[])
{

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdSetAlias, NULL);
#endif

    /**
     **  Whatis mode?
     **/

    if( g_flags & (M_WHATIS | M_HELP)) 
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
	
    /**
     **  Parameter check. Valid commands are:
     **
     **     unset-alias <alias>
     **     set-alias <alias> <value>
     **/

    if( (!strncmp( argv[0], "un", 2) && (argc != 2)) ||
        (!strncmp( argv[0], "set", 3) && (argc != 3))) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "variable", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Display only mode?
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }

    /**
     **  Switch command
     **/

    if( g_flags & M_SWSTATE1) {
        set_marked_entry(markAliasHashTable, (char *) argv[1], M_SWSTATE1);
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    } else if( g_flags & M_SWSTATE2) {
        set_marked_entry(markAliasHashTable, (char *) argv[1], M_SWSTATE2);
    } else if( g_flags & M_SWSTATE3) {
        int marked_val;
        if(marked_val = chk_marked_entry(markAliasHashTable,(char *) argv[1])) {
            if( marked_val == M_SWSTATE1)
                store_hash_value(aliasUnsetHashTable, argv[1], argv[2]);
            else
		return( TCL_OK);	/** ------- EXIT PROCEDURE -------> **/
      }
    } else if( g_flags & M_REMOVE) {
        store_hash_value( aliasUnsetHashTable, argv[1], argv[2]);
    }

    /**
     **  Finally remove or set the alias
     **/

    if( *argv[0] == 'u' || (g_flags & M_REMOVE))
        store_hash_value( aliasUnsetHashTable, argv[1], argv[2]);
    else
        store_hash_value( aliasSetHashTable, argv[1], argv[2]);
  
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdSetAlias, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdSetAlias' **/
