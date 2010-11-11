/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdMisc.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The Tcl 'system' command			     **
 ** 									     **
 **   Exports:		cmdSystem					     **
 ** 									     **
 **   Notes:		The Following code was written by Don Libes, NIST    **
 **			It was taken from his tool, 'expect' ...	     **
 **  			I have edited parts of it...			     **
 ** 									     **
 ** ************************************************************************ **
 ****/

/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1991-1994 by John L. Furlan.                      	     **
 ** see LICENSE.GPL, which must be provided, for details		     **
 ** 									     ** 
 ** ************************************************************************ **/

static char Id[] = "@(#)$Id: cmdMisc.c,v 1.4.22.1 2010/11/11 18:23:18 rkowen Exp $";
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

#define MAX_ARGLIST 10240

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "cmdMisc.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdSystem[] = "cmdSystem";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdSystem					     **
 ** 									     **
 **   Description:	Callback function for 'system'			     **
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

int	cmdSystem(	ClientData	 clientData, 
	  		Tcl_Interp	*interp, 
	  		int		 argc, 
	  		CONST84 char	*argv[])
{
    int		 i;
    int		 saved_stdout;
    char	 buf[ MAX_ARGLIST];
    char	*bufp = buf;
    int		 total_len = 0,
    		 arg_len;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdSystem, NULL);
#endif

    /**
     **  Whatis mode
     **/

    if( g_flags & (M_WHATIS | M_HELP))
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

    /**
     **  Display mode?
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t\t ", argv[ 0]);
	for( i=1; i<argc; i++)
	    fprintf( stderr, "%s ", argv[ i]);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }

    /**
     **  Prepare a buffer to hold the complete 'system' call
     **  Watch over the commands complete length while copying ...
     **/

    for( i = 1; i<argc; i++) {

        total_len += (1 + (arg_len = strlen(argv[i])));
        if( total_len > MAX_ARGLIST) {
	    if( OK != ErrorLogger( ERR_ARGSTOLONG, LOC, argv[0], (sprintf( buf,
		"%d", total_len), buf), NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
        }

	/**
 	 **  Copy the argument ov the buffer and put a space at its end
	 **/

        memcpy( bufp, argv[i], arg_len);
        bufp += arg_len;
        memcpy( bufp, " ", 1);
        bufp += 1;
    }

    /**
     **  For Modules, stdout must be directed to stderr so it
     **  isn't parsed by the evaluating shell.  We also must save it here so it
     **  can be restored after this command has been executed.
     **/

    saved_stdout = TieStdout();

    *(bufp-1) = '\0';
    i = system( buf);

    /**
     **  Following the style of Tcl_ExecCmd, we can just return the
     **  raw result (appropriately shifted and masked) to Tcl
     **/

    sprintf( buf, "%d", (0xff & (i >> 8)));
    Tcl_SetResult( interp, buf, TCL_VOLATILE);

    /*
     *  Restore stdout.
     */

    UnTieStdout( saved_stdout);
    
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdSystem, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdSystem' **/
