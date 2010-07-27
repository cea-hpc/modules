/*****
 ** ** Module Header ******************************************************* **
 **									     **
 **   File:		cmdChdir.c					     **
 **   First Edition:	2009-06-27					     **
 **									     **
 **   Authors:		Gerrit Renker					     **
 **   Description:	Implements a "chdir" command to set the working      **
 **			directory upon module load/switch.		     **
 **									     **
 **   Exports:		cmdChDir					     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			change_dir	Communicates the target directory.   **
 ** ************************************************************************ **
 ****/
#include "modules_def.h"

static	char	module_name[]	= __FILE__;
char		*change_dir	= NULL;

int cmdChDir(ClientData	client_data, Tcl_Interp	*interp,
	     int argc, CONST84 char *argv[])
{
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, __func__, NULL);
#endif

    if(g_flags & ~(M_LOAD|M_SWSTATE2))
        return TCL_OK;

    if (argc != 2) {
	if (OK != ErrorLogger( ERR_USAGE, LOC, argv[0], " directory", NULL))
	    return TCL_ERROR;
    }

    if (g_flags & M_NONPERSIST)
	return TCL_OK;

    if(g_flags & M_DISPLAY) {
	fprintf(stderr, "%s\t%s\n", argv[0], argv[1]);
        return TCL_OK;
    }

    /* The actual work happens here */
    if (change_dir != NULL)
	    free(change_dir);
    change_dir = strdup(argv[1]);

    return TCL_OK;
}
