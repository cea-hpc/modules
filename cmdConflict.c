/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdConflict.c					     **
 **   First Edition:	91/10/23					     **
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

static char Id[] = "@(#)$Id: cmdConflict.c,v 1.1 2000/06/28 00:17:32 rk Exp $";
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

static	int	checkConflict(	Tcl_Interp*, char*, char**, unsigned int);

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		checkConflict					     **
 ** 									     **
 **   Description:	Check whether the 'current_module' is within the list**
 **			of passed modules				     **
 ** 									     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **			char		*path		Modulepath to be chk.**
 **			char		**modulelist	List of loaded mod.  **
 **			int		 nummodules	Number of loaded mod.**
 ** 									     **
 **   Result:		int	TCL_OK		Successfull completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **									     **
 **		  	current_module	Module to check for		     **
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
    static struct stat stat_info;
    static char	  buffer[ MOD_BUFSIZE];
    
#if WITH_DEBUGGING_UTIL
    ErrorLogger( NO_ERR_START, LOC, _proc_checkConflict, NULL);
#endif

    memset( error_module, '\0', MOD_BUFSIZE);

    /**
     **  Check all modules passed to me as parameter
     **  At first clarify if they really so exist ...
     **/

    for( k=0; k<nummodules; k++) {

        /* sprintf( buffer, "%s/%s", path, modulelist[k]); */
        strcpy( buffer, path);
        strcat( buffer, "/");
        strcat( buffer, modulelist[k]);
        if( stat( buffer, &stat_info) < 0) {
	    if( OK != ErrorLogger( ERR_FILEINDIR, LOC, modulelist[k], path,
		NULL))
		strcpy( error_module, modulelist[k]);
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	}

	/**
	 **  Is it a directory what has been passed? If it is, list the
	 **  according directory and call myself recursivly in order to 
	 **/

        if( S_ISDIR( stat_info.st_mode)) {

            if( NULL == (new_modulelist = SortedDirList( interp, path, 
		modulelist[k], &new_nummodules)))
                continue;

            if( TCL_ERROR == checkConflict( interp, path, new_modulelist,
		new_nummodules)) {
                FreeList( new_modulelist, new_nummodules);
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
            }

            FreeList( new_modulelist, new_nummodules);

	/**
	 **  If it isn't a directory, check the current one for to be the
	 **  required module file
	 **/

        } else {

            if( IsLoaded_ExactMatch( interp, modulelist[k], NULL, NULL) &&
                strcmp( current_module, modulelist[k])) {

                /**
                 **  Save the name of the offending module in a buffer
                 **  for reporting purposes when we get back to the top.
                 **/

                strcpy( error_module, modulelist[k]);
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
            }

        } /** if( directory) **/
    } /** for **/

#if WITH_DEBUGGING_UTIL
    ErrorLogger( NO_ERR_END, LOC, _proc_checkConflict, NULL);
#endif

    return( TCL_OK);

} /** End of 'checkConflict' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdConflict					     **
 ** 									     **
 **   Description:	Callback function for 'confilct'		     **
 ** 									     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successfull completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdConflict(	ClientData	 client_data,
	    		Tcl_Interp	*interp,
	    		int		 argc,
	    		char	 	*argv[])
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

    if( flags & (M_WHATIS | M_HELP))
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

    /**
     **  Check the parameters. Usage is 'conflict <module> [<module> ...]'
     **/

    if( argc < 2) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0],
	    "conflicting-modulefiles", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  There will be no conflicts in case of switch or unload
     **/

    if( flags & (M_REMOVE | M_SWITCH))
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

    /**
     **  Load the MODULEPATH and split it into a list of paths. Assume success
     **  if not list is to be build...
     **/

    if( !(modulepath = (char *) getenv( "MODULEPATH"))) {
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    if( !( pathlist = SplitIntoList( interp, modulepath, &numpaths))) {
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
    }

    /**
     **  Display?
     **/

    if( flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }

    /**
     **  Now check/display all passed modules ...
     **/

    for( i=1; i<argc && argv[i]; i++) {
        for( j = 0; j < numpaths; j++) {

            if( NULL == (modulelist = SortedDirList( interp, pathlist[j], 
		argv[i], &nummodules)))
                continue;		/** not browseable		     **/

	    /**
	     **  Actually checking for conflicts is done here
	     **/

            if( TCL_ERROR == checkConflict( interp, pathlist[j], modulelist,
		nummodules)) {
		if( OK != ErrorLogger( ERR_CONFLICT, LOC, current_module,
		    error_module, NULL)) {
		    FreeList( pathlist, numpaths);
		    FreeList( modulelist, nummodules);
		    return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
		}
            }

	    /**
	     **  Free the list of modules used in the loops body above.
	     **/

            FreeList( modulelist, nummodules);

        } /** for( j) **/
    } /** for( i) **/

    FreeList( pathlist, numpaths);

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdConflict, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdConflict' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdPrereq					     **
 ** 									     **
 **   Description:	Callback function for 'prereq'			     **
 ** 									     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successfull completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdPrereq(	ClientData	 client_data,
	    		Tcl_Interp	*interp,
	    		int		 argc,
	    		char		*argv[])
{
    char	***savedlists = (char ***) NULL;
    int		  *savedlens = (int *) NULL;
    char	 **pathlist;
    char	 **modulelist;
    char	  *modulepath;
    char	  *notloaded_flag = argv[1];
    int     	   i, j, k, numpaths, nummodules, listcnt = 0;
    int		   Result = TCL_OK;
    char	   buffer[ MOD_BUFSIZE], *s;
	
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdPrereq, NULL);
#endif

    /** 
     **  Parameter check. Usage is 'prereq <module> [<module> ...]'
     **/

    if( argc < 2) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "prerequsite-modules", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  There's no prerequisite check in case of removal
     **/

    if( flags & (M_REMOVE | M_WHATIS))
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

    /**
     **  Display mode
     **/

    if( flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }

    /**
     **  Load the MODULEPATH and split it into a list of paths
     **/

    if( !(modulepath = (char *) getenv( "MODULEPATH"))) {
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#if WITH_DEBUGGING_CALLBACK_1
    ErrorLogger( NO_ERR_DEBUG, LOC, "Got modulepath: '", modulepath, "'", NULL);
#endif

    if( !(pathlist = SplitIntoList( interp, modulepath, &numpaths)))
        return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

    /**
     **  Allocate memory for the lists of conflict modules
     **/

    if( NULL == (savedlists = (char***) malloc( numpaths * sizeof(char**)))) {
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
    if( NULL == (savedlens = (int*) malloc( numpaths * sizeof( int)))) {
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL)) {
	    free( savedlists);
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
	}
    }

    /**
     **  Check/Display all passed modules
     **/

#if WITH_DEBUGGING_CALLBACK_1
    ErrorLogger( NO_ERR_DEBUG, LOC, "Scanning all ", (sprintf( buffer, "%d",
	numpaths), buffer), "modulepaths", NULL);
#endif

    for( i=1; i<argc && argv[i] && notloaded_flag; i++) {
        for( j = 0; j < numpaths && notloaded_flag; j++) {

            if( NULL == (modulelist = SortedDirList( interp, pathlist[j], 
	        argv[i], &nummodules)))
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
                    notloaded_flag = argv[i];
                } else {
                    notloaded_flag = NULL;
                }
            }

	    /**
	     **  Free what has been allocted in the loop
	     **/

            FreeList( modulelist, nummodules);

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

	s = buffer;
	for( k=0; k<listcnt; k++) {
	    char **listptr = savedlists[k];

	    for( i=0; listptr && i<savedlens[k]; i++, listptr++) {
		strcpy( s, *listptr);
		s += strlen( *listptr);
		*s++ = ' ';
	    }

	    FreeList( savedlists[k], savedlens[k]);
	}

	*s++ = '\0';

	if( OK != ErrorLogger( ERR_PREREQ, LOC, current_module, buffer, NULL))
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

    free( savedlists);
    free( savedlens);

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdPrereq, NULL);
#endif

    return( Result);

} /** End of 'cmdPrereq' **/
