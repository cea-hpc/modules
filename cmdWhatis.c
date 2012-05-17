/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdWhatis.c					     **
 **   First Edition:	1995/12/31					     **
 ** 									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The Tcl module-verbose routine allows switchin ver-  **
 **			bosity on and off during module file execution	     **
 ** 									     **
 **   Exports:		cmdModuleWhatis					     **
 **			cmdModuleWhatisInit				     **
 **			cmdModuleWhatisShut				     **
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

static char Id[] = "@(#)$Id: cmdWhatis.c,v 1.4.20.2 2011/10/03 20:25:43 rkowen Exp $";
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

#define	WHATIS_FRAG 100

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "cmdWhatis.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdModuleWhatis[] = "cmdModuleWhatis";
#endif

/**
 **  The whatis array ...
 **/

char		**whatis = (char **) NULL;
static	int	  whatis_size = 0, whatis_ndx = 0;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleWhatis					     **
 ** 									     **
 **   Description:	Callback function for 'verbose'			     **
 ** 									     **
 **   First Edition:	1995/12/31					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	sw_verbose	The verbose level selector	     **
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModuleWhatis(	ClientData	 client_data,
		      		Tcl_Interp	*interp,
		      		int		 argc,
		      		CONST84 char	*argv[])
{
    int i = 1;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModuleWhatis, NULL);
#endif

    /**
     **  Help mode
     **/

    if( g_flags & M_HELP)
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

    /**
     **  Parameter check
     **/

    if( argc < 2) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], " string", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
  
    /**
     **  If we don't have any whatis list buffer until now, we will create one
     **/

    if( !whatis) {
	whatis_size = WHATIS_FRAG;
	if((char **) NULL
		== (whatis = module_malloc(whatis_size * sizeof(char *)))){
	    ErrorLogger( ERR_ALLOC, LOC, NULL);
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
	}
    }

    /**
     **  Display mode?
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	for( i=1; i<argc; i++)
	    fprintf( stderr, "%s ", argv[ i]);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }

    /**
     **  Check if printing is requested 
     **/

    if( g_flags & M_WHATIS ) {
	while( i < argc) {

	    /**
	     **  Conditionally we have to enlarge our buffer
	     **/

	    while( whatis_ndx + 2 >= whatis_size) {
		whatis_size += WHATIS_FRAG;
		if(!(whatis = module_realloc( whatis, whatis_size *
		    sizeof( char *)))) {
		    ErrorLogger( ERR_ALLOC, LOC, NULL);
		    return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
		}
	    }

	    /**
	     **  Put the string on the buffer
	     **/

	    if((char *) NULL == (whatis[ whatis_ndx++] = strdup( argv[ i++]))) {
		if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		    return( TCL_ERROR);
		whatis_ndx--;
	    }

	} /** while **/
    } /** if **/

    /**
     **  Put a trailing terminator on the buffer
     **/

    whatis[ whatis_ndx] = (char *) NULL;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModuleWhatis, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdModuleWhatis' **/
   
/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleWhatisInit				     **
 **			cmdModuleWhatisShut				     **
 ** 									     **
 **   Description:	Initialization of internat data structures for the   **
 **			Module whatis command				     **
 ** 									     **
 **   First Edition:	1995/12/31					     **
 ** 									     **
 **   Parameters:	-						     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void	cmdModuleWhatisInit()
{
    whatis_ndx = 0;

} /** End of 'cmdModuleWhatisInit' **/

void	cmdModuleWhatisShut()
{
    char **ptr = whatis;

    if( whatis) {
	while( *ptr) {		/** go until NULL token **/
	    free( *ptr);
	    *ptr = (char *) NULL;
	    ptr++;
	}
	whatis_ndx = 0;
    }

} /** End of 'cmdModuleWhatisShut' **/
