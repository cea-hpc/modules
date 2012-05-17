/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdPath.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The path manipulation routines. Much of the heart of **
 **			Modules is contained in this file. These routines    **
 **			are responsible for adding and removing directories  **
 **			from given PATH-like variables.			     **
 ** 									     **
 **   Exports:		cmdSetPath					     **
 **			cmdRemovePath					     **
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

static char Id[] = "@(#)$Id: cmdPath.c,v 1.10.4.5 2011/11/28 21:13:15 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"
#ifdef	HAS_SYS_PARAM_H
#include <sys/param.h>
#endif

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

#ifdef	MAXPATHLEN
#define	PATH_BUFLEN	MAXPATHLEN
#else
#define	PATH_BUFLEN	1024
#endif

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

#define _TCLCHK(a)	\
	{if (*(TCL_RESULT(a))) ErrorLogger(ERR_EXEC,LOC,TCL_RESULT(a),NULL);}

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "cmdPath.c";	/** File name of this module **/

#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdSetPath[] = "cmdSetPath";
static	char	_proc_cmdRemovePath[] = "cmdRemovePath";
static	char	_proc_Remove_Path[] = "Remove_Path";
#endif

static char buffer[ PATH_BUFLEN];

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static int	Remove_Path( Tcl_Interp	*interp, char *variable, char *item, 
			char *sw_marker, const char *delim);


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdSetPath					     **
 ** 									     **
 **   Description:	Add the passed value (argv[2]) to the specified vari-**
 **			able (argv[1]). argv[0] specifies, if the variable   **
 **			is to be appended or prepended.  Each directory in   **
 **			the path is checked to see whether it is already     **
 **			in the path.  If so it is not added.		     **
 **			(Handles options -d C, or --delim[=]C )		     **
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

int	cmdSetPath(	ClientData	 client_data,
	       		Tcl_Interp	*interp,
	       		int		 argc,
	       		CONST84 char	*argv[])
{
    Tcl_RegExp	chkexpPtr;			/** Regular expression for   **/
						/** marker checking	     **/
    char	 *oldpath,			/** Old value of 'var'	     **/
		 *newpath,			/** New value of 'var'	     **/
		 *sw_marker = APP_SW_MARKER,	/** arbitrary default	     **/
		 *startp=NULL, *endp=NULL,	/** regexp match endpts	     **/
		 *qualifiedpath,		/** List of dirs which 
						    aren't already in path   **/
		**pathlist;			/** List of dirs	     **/
    const char	 *delim = _colon;		/** path delimiter	     **/
    int		  append = 1,			/** append or prepend	     **/
		  numpaths,			/** number of dirs in path   **/
		  qpathlen,			/** qualifiedpath length     **/
		  arg1 = 1,			/** arg start		     **/
		  x;				/** loop index		     **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdSetPath, NULL);
#endif

    /**
     **  Whatis mode?
     **/
    if( g_flags & (M_WHATIS | M_HELP))
        goto success0;
	
    /**
     **   Check arguments. There should be at least 3 args:
     **     argv[0]  -  prepend/append
     **     ...
     **     argv[n-1]-  varname
     **     argv[n]  -  value
     **/
    if(argc < 3)
	if( OK != ErrorLogger(ERR_USAGE, LOC, argv[0],
	    " path-variable directory", NULL))
	    goto unwind0;

    /**
     **  Should this guy be removed from the variable ... If yes, do so!
     **/
    if(g_flags & M_REMOVE) 
	return( cmdRemovePath(client_data, interp, argc, argv));   /** ----> **/

    /**
     **  prepend or append. The default is append.
     **/
    if( !( append = !!strncmp( argv[0], "pre", 3)))
	sw_marker = PRE_SW_MARKER;
  
    /**
     **  Non-persist mode?
     **/
    
    if (g_flags & M_NONPERSIST) {
	return (TCL_OK);
    }

    /**
     **  Display only ... ok, let's do so!
     **/
    if(g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        goto success0;
    }

    /**
     **  Check for the delimiter option
     **/
    if(*(argv[arg1]) == '-') {
	if (!strcmp(argv[arg1], "-d")) {
		delim = argv[arg1+1];
		arg1 += 2;
	} else if (!strcmp(argv[arg1], "--delim")) {
		delim = argv[arg1+1];
		arg1 += 2;
	} else if (!strncmp(argv[arg1], "--delim=", 8)) {
		delim = argv[arg1]+8;
		arg1++;
	}
    }

    /**
     **  Get the old value of the variable. MANPATH defaults to a configure
     **  generated value.
     **  Put a \ in front of each '.' and '+'.
     **  (this is an intentional memory leak)
     **/
    oldpath =  EMGetEnv( interp, argv[arg1]);
    _TCLCHK(interp)

    if(!oldpath || !*oldpath) {
	null_free((void *) &oldpath);
	oldpath = !strcmp( argv[arg1], "MANPATH")
		? stringer(NULL,0,DEFAULTMANPATH,NULL)
		: stringer(NULL,0,"",NULL);
    }

    /**
     **  Split the new path into its components directories so each
     **  directory can be checked to see whether it is already in the 
     **  existing path.
     **/
    if( !( pathlist = SplitIntoList( interp, (char *) argv[arg1+1], &numpaths,
	delim)))
	goto unwind0;

    /**
     **  Some space for the list of paths which
     **  are not already in the existing path.
     **/
    if((char *) NULL == (qualifiedpath = stringer(NULL,0, argv[arg1+1],
	delim, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind1;

    qpathlen = strlen(qualifiedpath)+1;
    *qualifiedpath = '\0';		/** make sure null for later	     **/

    for( x = 0; x < numpaths; x++) {

	regex_quote( pathlist[x], buffer, PATH_BUFLEN);

	/**
	 **  Check to see if path is already in this path variable.
	 **  It could be at the 
	 **     beginning ... ^path:
	 **     middle    ... :path:
	 **     end       ... :path$
	 **     only one  ... ^path$
	 **/
	if((char *) NULL == (newpath = stringer(NULL,0,
		"(^", buffer, delim, ")|(",delim, buffer, delim,")|(",delim,
		buffer, "$)|(^", buffer, "$)",NULL)))
	    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		goto unwind2;

	chkexpPtr = Tcl_RegExpCompile(interp, newpath);
	_TCLCHK(interp)
	null_free((void *) &newpath);

	/**
	 **  If the directory is not already in the path, 
	 **  add it to the qualified path.
	 **/
	if( !Tcl_RegExpExec(interp, chkexpPtr, oldpath, oldpath))
	    if (!stringer(qualifiedpath + strlen(qualifiedpath),
		    qpathlen - strlen(qualifiedpath),
		    pathlist[x], delim, NULL))
		if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		    goto unwind2;

    }					/** End of loop that checks for 
					 ** already existent path
					 **/
    /**
     **  If all of the directories in the new path already exist,
     **  exit doing nothing.
     **/
    if( ! *qualifiedpath)
	goto success1;

    /* remove trailing delimiter */
    qualifiedpath[strlen(qualifiedpath) - 1] = '\0';

    /**
     **  Some space for our newly created path.
     **  We size at the oldpath plus the addition.
     **/
    if(!(newpath = stringer(NULL, strlen( oldpath) +
	strlen(qualifiedpath) + 2,NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind2;
    *newpath = '\0';

    /**
     **  Easy job to do, if the old path has not been set up so far ...
     **/
    if( !strcmp( oldpath, "")) {
	strcpy( newpath, qualifiedpath);

    /**
     **  Otherwise we have to take care on prepending vs. appending ...
     **  If there is a append or prepend marker within the variable (see
     **  modules_def.h) the changes are made according to this markers. Other-
     **  wise append and prepend will be relative to the strings begin or end.
     **/

    } else {

	Tcl_RegExp	markexpPtr = Tcl_RegExpCompile(interp, sw_marker);
	_TCLCHK(interp)

	strcpy( newpath, oldpath);

	if( Tcl_RegExpExec(interp, markexpPtr, oldpath, oldpath)) {
	    _TCLCHK(interp)
	    Tcl_RegExpRange(markexpPtr, 0,
		(CONST84 char **) &startp, (CONST84 char **) &endp);

	    /**
	     **  Append/Prepend marker found
	     **/
	    if( append) {
		char ch = *startp;
		*startp = '\0';
		strcpy(newpath, oldpath);
                /**
                 ** check that newpath has a value before adding delim
                 **/
		if (strlen(newpath) > 0 && newpath[strlen(newpath)-1] != *delim)
			strcat(newpath, delim);
		strcat(newpath, qualifiedpath);
		if (newpath[strlen(newpath)-1] != *delim)
			strcat(newpath, delim);
		*startp = ch;
		strcat(newpath, startp);
               
	    } else {
                char ch = *endp;
		*endp = '\0';
		strcpy(newpath, oldpath);
		if (newpath[strlen(newpath)-1] != *delim)
			strcat(newpath, delim);
		strcat(newpath, qualifiedpath);
		*endp = ch;
		strcat(newpath, endp);
	    }

	} else {

	    /**
	     **  No marker set
	     **/
	    if(append) {
		strcpy(newpath, oldpath);
		if (newpath[strlen(newpath)-1] != *delim)
			strcat(newpath, delim);
		strcat(newpath, qualifiedpath);
	    } else {
		strcpy(newpath, qualifiedpath);
		if (*oldpath != *delim)	strcat(newpath, delim);
		strcat(newpath, oldpath);
	    }

	} /** if( marker) **/

    } /** if( strcmp) **/

    /**
     **  Now the new value to be set resides in 'newpath'. Set it up.
     **/
    moduleSetenv( interp, (char *) argv[arg1], newpath, 1);
    _TCLCHK(interp)

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdSetPath, NULL);
#endif

    /**
     ** Free resources
     **/
    null_free((void *) &newpath);
success1:
    null_free((void *) &oldpath);
    null_free((void *) &qualifiedpath);
    FreeList( pathlist, numpaths);
success0:
    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind2:
    null_free((void *) &qualifiedpath);
unwind1:
    FreeList( pathlist, numpaths);
unwind0:
    null_free((void *) &oldpath);
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'cmdSetPath' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:cmdRemovePath					     **
 ** 									     **
 **   Description:	Remove the passed value (argv[2]) from the specified **
 **			variable (argv[1]). In case of switching this pro-   **
 **			cedure removes markers from the path, too. argv[0]   **
 **			specifies, if the append- or prepend-marker is af-   **
 **			fected						     **
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

int	cmdRemovePath(	ClientData	 client_data,
	       		Tcl_Interp	*interp,
	       		int		 argc,
	       		CONST84 char	*argv[])
{
    char	 *sw_marker = APP_SW_MARKER,	/** arbitrary default	     **/
		**pathlist;			/** List of dirs	     **/
    const char	 *delim = _colon;		/** path delimiter	     **/
    int		  numpaths,			/** number of dirs in path   **/
		  arg1 = 1,			/** arg start		     **/
		  x;				/** loop index		     **/


#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdRemovePath, NULL);
#endif

    /**
     **   Check arguments. There should be give 3 args:
     **     argv[0]  -  prepend/append/remove-path
     **     ...
     **     argv[n-1]-  varname
     **     argv[n]  -  value
     **/
    if(argc < 3)
	if( OK != ErrorLogger(ERR_USAGE,LOC,argv[0],"path-variable directory",
	    NULL))
	    goto unwind0;

  
    /**
     **  Non-persist mode?
     **/
    
    if (g_flags & M_NONPERSIST) {
	return (TCL_OK);
    }

    /**
     **  Display only ... ok, let's do so!
     **/
    if(g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        goto success0;
    }
  
    /**
     **  prepend or append. The default is append.
     **/
    if( ! strncmp( argv[0], "pre", 3))
	sw_marker = PRE_SW_MARKER;
  
    /**
     ** For switch state3, we're looking to remove the markers.
     **/
    if( g_flags & M_SWSTATE3) 
	argv[arg1+1] = sw_marker;

    /**
     **  Check for the delimiter option
     **/
    if(*(argv[arg1]) == '-') {
	if (!strcmp(argv[arg1], "-d")) {
		delim = argv[arg1+1];
		arg1 += 2;
	} else if (!strcmp(argv[arg1], "--delim")) {
		delim = argv[arg1+1];
		arg1 += 2;
	} else if (!strncmp(argv[arg1], "--delim=", 8)) {
		delim = argv[arg1]+8;
		arg1++;
	}
    }

    /**
     **  Split the path into its components so each item can be removed
     **  individually from the variable.
     **/
    if( !( pathlist = SplitIntoList( interp, (char *) argv[arg1+1], &numpaths,
	delim)))
	goto unwind0;

    /**
     ** Remove each item individually
     **/
    for( x = 0; x < numpaths; x++)
	if(TCL_OK != Remove_Path(interp,(char *) argv[arg1],pathlist[x],
	sw_marker, delim))
	    goto unwind1;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdRemovePath, NULL);
#endif

    /**
     ** Free resources
     **/
    FreeList(pathlist, numpaths);

success0:
    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind1:
    FreeList(pathlist, numpaths);
unwind0:
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'cmdRemovePath' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Remove_Path					     **
 ** 									     **
 **   Description:	This function actually does the work of removing     **
 **			the item from the path.  It is done this way to      **
 **			support multiple items (often directories)	     **
 **			separated by colons in the variable value.	     **
 ** 									     **
 **   First Edition:	2001/08/08					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **			char		*variable	Variable from which  **
 **							to remove item	     **
 **			char		*item		Item to remove	     **
 **			char		*sw_marker	Switch marker	     **
 **			const char	*delim		path delimiter	     **
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

static int Remove_Path(
	Tcl_Interp * interp,
	char *variable,
	char *item,
	char *sw_marker,
	const char *delim
) {
	char           *oldpath,	/** current path  **/
	               *olditem;	/** item from path **/
	int             i,		/** counter **/
	                found = 0,	/** flag to indicate item was found **/
			pcount = 0,	/** count of items in path **/
			addsep = 0,	/** flag to add separator **/
			marked = 0,	/** flag path contains sw_marker **/
			oldpathlen = 0;
	Tcl_DString     _newpath;
	Tcl_DString    *newpath = &_newpath;
	Tcl_DStringInit(newpath);
    /**
     **  Get the current value of the "PATH" environment variable
     **/
	oldpath = (char *) EMGetEnv(interp, variable);
	if (!oldpath || !*oldpath) {
		null_free((void *) &oldpath);
		_TCLCHK(interp);
		goto success0;		/** -------- EXIT (SUCCESS) -------> **/
	}
	/* copy oldpath to not mess with the TCL value of env(PATH) */
	if (!(oldpath = stringer(NULL,0, oldpath, NULL)))
		if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
			goto unwind0;

	/* get length of oldpath before it gets modified by xstrtok */
	oldpathlen = strlen(oldpath);

	/* determine if sw_marker is in the path */
	olditem = xstrtok(oldpath, delim);
	while (olditem) {
		if (!strcmp(olditem, sw_marker)) {
			marked = 1;
		}
		pcount++;
		olditem = xstrtok(NULL, delim);
	}

	/** pointer arithmetic on oldpath
	 ** if olditem starts at terminating null string of oldpath,
	 ** it means the last character in oldpath was ":", meaning
	 ** the last element was the empty string.  use <= to catch
	 **  this case and process the last empty element
	 */
	for (olditem = oldpath; olditem <= oldpath + oldpathlen;
	     olditem += strlen(olditem) + 1) {

		if (strcmp(olditem, item)) {
			/* not the droids we're looking for */
			if (Tcl_DStringLength(newpath)) {
				if (!Tcl_DStringAppend(newpath, delim, 1))
					if (OK !=
					    ErrorLogger(ERR_STRING, LOC, NULL))
						goto unwind1;
			}
			if (!Tcl_DStringAppend(newpath, olditem, -1))
				if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
					goto unwind1;
		} else {
			/* bingo! Don't add it to new path  */
			found++;

			if ((g_flags & M_SWSTATE1) && !marked) {
			/**
			 **  In state1, we're actually replacing old paths with
			 **  the markers for future appends and prepends.
			 **  
			 **  We only want to do this once to mark the location
			 **  the module was formed around.
			 **/
				marked = 1;
				if (Tcl_DStringLength(newpath)) {
					if (!Tcl_DStringAppend
					    (newpath, delim, 1))
						if (OK !=
						    ErrorLogger(ERR_STRING, LOC,
								NULL))
							goto unwind1;
				}
				if (!Tcl_DStringAppend(newpath, sw_marker, -1))
					if (OK !=
					    ErrorLogger(ERR_STRING, LOC, NULL))
						goto unwind1;
			}
		}
	}

	if (!found) {
		goto success1;
	}

	if (Tcl_DStringLength(newpath)) {
		/**
		**  Cache the set.  Clear the variable from the unset table just
		**  in case it was previously unset.
		**/
		store_hash_value(setenvHashTable, variable,
			Tcl_DStringValue(newpath));
		clear_hash_value(unsetenvHashTable, variable);

		/**
		**  Store the new PATH value into the environment.
		**/
		(void) EMSetEnv( interp, variable, Tcl_DStringValue(newpath));

		_TCLCHK(interp);
	} else {
		/**
		 **  In this case, I should go ahead and unset the variable
		 **  from the environment because I'm removing the very last
		 **  path.
		 **
		 **  First I'm going to clear the variable from the
		 **  setenvHashTable just in case its already been altered
		 **  and had a significant value at the time. It's very
		 **  possible that I'm removing the only two or three paths
		 **  from this variable. If that's the case, then all the
		 **  earlier paths were marked for output in this hashTable.
		 **
		 **  Secondly, I actually mark the the environment variable
		 **  to be unset when output.
		 **/
		clear_hash_value(setenvHashTable, variable);
		moduleUnsetenv(interp, variable);

		/**
		 **  moduleUnsetenv doesn't unset the variable in the Tcl
		 **  space because the $env variable might need to be
		 **  used again in the modulefile for locating other
		 **  paths.  BUT, since this was a path-type environment
		 **  variable, the user is expecting this to be empty
		 **  after removing the only remaining path.  So, I set
		 **  the variable empty here.
		 **/
		(void) EMSetEnv( interp, variable, "");
		_TCLCHK(interp);
	}

/**
 **  Free what has been used and return on success
 **/
success1:
	null_free((void *)&oldpath);
success0:
	Tcl_DStringFree(newpath);
	return (TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

unwind1:
	null_free((void *)&oldpath);
unwind0:
	Tcl_DStringFree(newpath);
	return (TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

} /** End of 'Remove_Path' **/
