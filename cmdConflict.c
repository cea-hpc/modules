/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdConflict.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The Tcl conflict and prereq commands.		     **
 ** 									     **
 **   Exports:		cmdConflict					     **
 **			cmdPrereq					     **
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

static char Id[] = "@(#)$Id: cmdConflict.c,v 1.9.20.2 2011/10/06 21:33:39 rkowen Exp $";
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

static char error_module[ MOD_BUFSIZE];
static	char	module_name[] = "cmdConflict.c";	/** File name of this module **/
#if WITH_DEBUGGING_UTIL
static	char	_proc_checkConflict[] = "checkConflict";
#endif
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdConflict[] = "cmdConflict";
static	char	_proc_cmdPrereq[] = "cmdPrereq";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		checkConflict					     **
 ** 									     **
 **   Description:	Check whether the 'g_current_module' is in the list  **
 **			of passed modules				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **			char		*path		Modulepath to be chk.**
 **			char		**modulelist	List of loaded mod.  **
 **			int		 nummodules	Number of loaded mod.**
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **									     **
 **		  	g_current_module	Module to check for	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	checkConflict(	Tcl_Interp	*interp,
       		       		char		*path,
              			char		**modulelist,
              			unsigned	  int nummodules)
{
    char	**new_modulelist;
    int		  new_nummodules, k;
    struct stat	  stat_info;
    char	 *buffer;
    
#if WITH_DEBUGGING_UTIL
    ErrorLogger( NO_ERR_START, LOC, _proc_checkConflict, NULL);
#endif

    memset( error_module, '\0', MOD_BUFSIZE);

    /**
     **  Check all modules passed to me as parameter
     **  At first clarify if they really so exist ...
     **/

    for( k=0; k<nummodules; k++) {

	if ((char *) NULL == (buffer = stringer(NULL,0,
		path,"/", modulelist[k], NULL)))
	    if( OK != ErrorLogger( ERR_STRING, LOC,NULL))
		goto unwind0;

        if( stat( buffer, &stat_info) < 0) {
	    if( OK != ErrorLogger( ERR_FILEINDIR,LOC,modulelist[k], path,NULL))
		if ((char *) NULL == stringer(error_module,MOD_BUFSIZE,
			modulelist[k], NULL))
		    if( OK != ErrorLogger( ERR_STRING, LOC,NULL))
			goto unwind1;
	    goto unwind1;
	}

	/**
	 **  Is it a directory what has been passed? If it is, list the
	 **  according directory and call myself recursively in order to
	 **/

        if( S_ISDIR( stat_info.st_mode)) {

            if( NULL == (new_modulelist = SortedDirList( interp, path, 
		modulelist[k], &new_nummodules)))
                continue;

            if( TCL_ERROR == checkConflict( interp, path, new_modulelist,
		new_nummodules)) {
                FreeList( new_modulelist, new_nummodules);
		goto unwind1;
            }

            FreeList( new_modulelist, new_nummodules);

	/**
	 **  If it isn't a directory, check the current one for to be the
	 **  required module file
	 **/

        } else {

            if( IsLoaded_ExactMatch( interp, modulelist[k], NULL, NULL) &&
                strcmp( g_current_module, modulelist[k])) {

                /**
                 **  Save the name of the offending module in a buffer
                 **  for reporting purposes when we get back to the top.
                 **/

		if ((char *) NULL == stringer(error_module,MOD_BUFSIZE,
			modulelist[k], NULL))
		    if( OK != ErrorLogger( ERR_STRING, LOC,NULL))
			goto unwind1;
		goto unwind1;
            }

        } /** if( directory) **/
    } /** for **/

#if WITH_DEBUGGING_UTIL
    ErrorLogger( NO_ERR_END, LOC, _proc_checkConflict, NULL);
#endif
    /**
     ** free resources
     **/
    null_free((void *) &buffer);

    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind1:
    null_free((void *) &buffer);
unwind0:
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'checkConflict' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdConflict					     **
 ** 									     **
 **   Description:	Callback function for 'confilct'		     **
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

int	cmdConflict(	ClientData	 client_data,
	    		Tcl_Interp	*interp,
	    		int		 argc,
	    		CONST84 char 	*argv[])
{
    char	 **pathlist,		/** List of module-pathes	     **/
    		 **modulelist;		/** List of modules		     **/
    char	  *modulepath;		/** Contents of MODULEPATH	     **/
    int		   i, j,		/** Loop counters		     **/
   		   numpaths, nummodules;/** Size of the according arrays     **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdConflict, NULL);
#endif

    /**
     **  Whatis mode
     **/

    if( g_flags & (M_WHATIS | M_HELP))
	goto success0;

    /**
     **  Check the parameters. Usage is 'conflict <module> [<module> ...]'
     **/

    if( argc < 2)
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0],
	    "conflicting-modulefiles", NULL))
	    goto unwind0;

    /**
     **  There will be no conflicts in case of switch or unload
     **/

    if( g_flags & (M_REMOVE | M_SWITCH))
        goto success0;

    /**
     **  Load the MODULEPATH and split it into a list of paths. Assume success
     **  if no list to be built...
     **/
    if((char *) NULL == (modulepath = xgetenv( "MODULEPATH")))
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    goto unwind0;

    if((char **) NULL==(pathlist=SplitIntoList(interp, modulepath, &numpaths,
	_colon)))
        goto success1;

    /**
     **  Non-persist mode?
     **/
    
    if (g_flags & M_NONPERSIST) {
	return (TCL_OK);
    }

    /**
     **  Display?
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        goto success2;
    }

    /**
     **  Now check/display all passed modules ...
     **/

    for( i=1; i<argc && argv[i]; i++) {
        for( j = 0; j < numpaths; j++) {

            if((char **)NULL == (modulelist = SortedDirList(interp,
			pathlist[j], (char *) argv[i], &nummodules)))
                continue;		/** not browseable		     **/

	    /**
	     **  Actually checking for conflicts is done here
	     **/
            if( TCL_ERROR == checkConflict( interp, pathlist[j], modulelist,
		nummodules))
		if( OK != ErrorLogger( ERR_CONFLICT, LOC, g_current_module,
		    error_module, NULL)) {
		    FreeList( modulelist, nummodules);
		    goto unwind2;
		}

	    /**
	     **  Free the list of modules used in the loops body above.
	     **/
            FreeList( modulelist, nummodules);

        } /** for( j) **/
    } /** for( i) **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdConflict, NULL);
#endif

    /**
     ** free resources
     **/
success2:
    FreeList( pathlist, numpaths);
success1:
    null_free((void *) &modulepath);
success0:
    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind2:
    FreeList( pathlist, numpaths);
unwind1:
    null_free((void *) &modulepath);
unwind0:
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'cmdConflict' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdPrereq					     **
 ** 									     **
 **   Description:	Callback function for 'prereq'			     **
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

int	cmdPrereq(	ClientData	 client_data,
	    		Tcl_Interp	*interp,
	    		int		 argc,
	    		CONST84 char	*argv[])
{
    char	***savedlists = (char ***) NULL;
    int		  *savedlens = (int *) NULL;
    char	 **pathlist,
		 **modulelist,
		  *modulepath,
		  *notloaded_flag = (char *) argv[1];
    int     	   i, j, k, numpaths, nummodules, listcnt = 0,
		   Result = TCL_OK;
    char	   buffer[ MOD_BUFSIZE];
	
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdPrereq, NULL);
#endif

    /** 
     **  Parameter check. Usage is 'prereq <module> [<module> ...]'
     **/

    if( argc < 2)
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0],
	    " prerequisite-modules", NULL))
	    goto unwind0;

    /**
     **  There's no prerequisite check in case of removal
     **/

    if( g_flags & (M_REMOVE | M_WHATIS))
        goto success0;

  
    /**
     **  Non-persist mode?
     **/
    
    if (g_flags & M_NONPERSIST) {
	return (TCL_OK);
    }

    /**
     **  Display mode
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        goto success0;
    }

    /**
     **  Load the MODULEPATH and split it into a list of paths. Assume success
     **  if no list to be built...
     **/
    if((char *) NULL == (modulepath = xgetenv( "MODULEPATH")))
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    goto unwind0;

#if WITH_DEBUGGING_CALLBACK_1
    ErrorLogger( NO_ERR_DEBUG, LOC, "Got modulepath: '", modulepath, "'", NULL);
#endif

    if((char **) NULL==(pathlist=SplitIntoList(interp, modulepath, &numpaths,
	_colon)))
        goto success1;

    /**
     **  Allocate memory for the lists of conflict modules
     **/
    if((char ***) NULL==(savedlists=(char***) module_malloc(numpaths * (argc-1)
	* sizeof(char**))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind1;

    if((int *) NULL == (savedlens = (int*) module_malloc(numpaths * (argc-1)
	* sizeof( int))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind2;

    /**
     **  Check/Display all passed modules
     **/

#if WITH_DEBUGGING_CALLBACK_1
    ErrorLogger( NO_ERR_DEBUG, LOC, "Scanning all ", (sprintf( buffer, "%d",
	numpaths), buffer), "modulepaths", NULL);
#endif

    for( i=1; i<argc && argv[i] && notloaded_flag; i++) {
        for( j = 0; j < numpaths && notloaded_flag; j++) {

            if((char **) NULL == (modulelist = SortedDirList(interp,pathlist[j],
	        (char *) argv[i], &nummodules)))
                continue;

	    /**
	     **  save the list of file to be printed in case of missing pre-
	     **  requisites or 
	     **/

#if WITH_DEBUGGING_CALLBACK_1
	    ErrorLogger( NO_ERR_DEBUG, LOC, "Save directory list. # = ",
		(sprintf( buffer, "%d", listcnt), buffer), NULL);
#endif

	    savedlens[ listcnt]    = nummodules;
	    savedlists[ listcnt++] = modulelist;

	    /**
	     **  Now actually check if the prerequisites are fullfilled
	     **  The notloaded_flag controls the exit from both loops in case
	     **  a prerequisite is missing.
	     **/

            for( k=0; k < nummodules && notloaded_flag; k++) {
                if( !IsLoaded( interp, modulelist[k], NULL, NULL)) {
                    notloaded_flag = (char *) argv[i];
                } else {
                    notloaded_flag = NULL;
                }
            }
        } /** for( j) **/
    } /** for( i) **/

#if WITH_DEBUGGING_CALLBACK_1
    ErrorLogger( NO_ERR_DEBUG, LOC, "Done. Missing prerequisite: '",
	(notloaded_flag ? notloaded_flag : "none"), "'", NULL);
#endif

    /**
     **  Display an error message if this was *NOT* display mode and a
     **  missing prerequisite has been found
     **/
    if( notloaded_flag) {

	/**
	 **  Add the whole list of prerequired module files to the Tcl result
	 **  string
	 **/
	for( k=0; k<listcnt; k++) {
	    char **listptr = savedlists[k];

	    *buffer = '\0';
	    for( i=0; listptr && i<savedlens[k]; i++, listptr++) {
		if ((char *) NULL == stringer(
			buffer + strlen(buffer), MOD_BUFSIZE-strlen(buffer),
			*listptr, " ", NULL))
		    if( OK != ErrorLogger( ERR_STRING, LOC,NULL)) {
	    		FreeList( savedlists[k], savedlens[k]);
			goto unwind2;
		    }
	    }

	    FreeList( savedlists[k], savedlens[k]);
	}

	buffer[strlen(buffer)-1] = '\0';	/* remove last blank */

	if( OK != ErrorLogger( ERR_PREREQ, LOC, g_current_module, buffer, NULL))
	    Result = TCL_ERROR;	

    } else {

	/**
	 **  We have to free the saved module names again
	 **/

	for( k=0; k<listcnt; k++)
	    FreeList( savedlists[k], savedlens[k]);

    }

    /**
     **  Free up the list of prerequisites and return ...
     **/

    null_free((void *) &savedlens);
    null_free((void *) &savedlists);

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdPrereq, NULL);
#endif

success1:
    null_free((void *) &modulepath);
success0:
    return( Result);			/** -------- EXIT (Result)  -------> **/

unwind3:
    null_free((void *) &savedlens);
unwind2:
    null_free((void *) &savedlists);
unwind1:
    null_free((void *) &modulepath);
unwind0:
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/
} /** End of 'cmdPrereq' **/
