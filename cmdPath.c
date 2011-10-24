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
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
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

static char Id[] = "@(#)$Id: cmdPath.c,v 1.25 2011/10/24 20:44:31 rkowen Exp $";
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

static	char	module_name[] = __FILE__;
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
 **			int		 objc		Number of arguments  **
 **			Tcl_Obj		*objv[]		Argument array	     **
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

int cmdSetPath(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	char		*pathlist,	   /** path to add **/ 
			*oldpath,	   /** old value of 'var'   **/
			*sw_marker = APP_SW_MARKER,
			*arg,		   /** argument ptr	    **/
			tmpchar,	   /** tmp holder for displaced char**/
			*marked = NULL,    /** flag path contains sw_marker **/
			*item;		   /** item from path **/
	const char	*delim = _colon;   /** path delimiter	    **/
	int		append = 1,	   /** append or prepend    **/
			i,		   /** loop counter, etc.   **/
			oldpathlen = 0,
			arg1 = 1;	   /** arg start	    **/
	Tcl_DString _qualifiedpath;
	Tcl_DString *qualifiedpath = &_qualifiedpath;
	Tcl_DStringInit(qualifiedpath);
	Tcl_DString _newpath;
	Tcl_DString *newpath = &_newpath;
	Tcl_DStringInit(newpath);
	Tcl_HashTable _oldpathhash;	   /** hash table for old path **/
	Tcl_HashTable *oldpathhash = &_oldpathhash;
	Tcl_InitHashTable(oldpathhash , TCL_STRING_KEYS);
	Tcl_HashEntry *hentry;


    /**
     **  Whatis mode?
     **/
	if (g_flags & (M_WHATIS | M_HELP))
		goto success0;

    /**
     **   Check arguments. There should be at least 3 args:
     **     argv[0]  -  prepend/append
     **     ...
     **     argv[n-1]-  varname
     **     argv[n]  -  value
     **/
	if (objc < 3)
		if (OK != ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				      " path-variable directory", NULL))
			goto unwind0;

    /**
     **  Should this guy be removed from the variable ... If yes, do so!
     **/
	if (g_flags & M_REMOVE)
		return (cmdRemovePath(client_data, interp, objc, objv));
								   /** ----> **/

    /**
     **  prepend or append. The default is append.
     **/
	if (!(append = !!strncmp(Tcl_GetString(objv[0]), "pre", 3)))
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
	if (g_flags & M_DISPLAY) {
		fprintf(stderr, "%s\t ", Tcl_GetString(objv[0]));
		while (--objc)
			fprintf(stderr, "%s ", Tcl_GetString(*++objv));
		fprintf(stderr, "\n");
		goto success0;
	}

    /**
     **  Check for the delimiter option
     **/
	arg = Tcl_GetString(objv[arg1]);
	if (*arg == '-') {
		if (!strcmp(arg, "-d")) {
			delim = Tcl_GetString(objv[arg1 + 1]);
			arg1 += 2;
		} else if (!strcmp(arg, "--delim")) {
			delim = Tcl_GetString(objv[arg1 + 1]);
			arg1 += 2;
		} else if (!strncmp(arg, "--delim=", 8)) {
			delim = arg + 8;
			arg1++;
		}
	}

    /**
     **  Get the old value of the variable.
     **  (MANPATH defaults to "/usr/man" -- removed).
     **  Put a \ in front of each '.' and '+'.
     **  (this is an intentional memory leak)
     **/
	oldpath =
	    (char *) TclGetEnv(interp, Tcl_GetString(objv[arg1]));
	_TCLCHK(interp);

#if 0
	    if (!oldpath)
		oldpath = !strcmp(Tcl_GetString(objv[arg1]),
			    "MANPATH") ? DEFAULTMANPATH : "";
#else
	    if (oldpath) {
		if(!(oldpath = strdup(oldpath)))
			if( OK != ErrorLogger( ERR_STRING ,LOC ,NULL) )
				goto unwind0;
	    } else {
		oldpath = "";
	    }
#endif

	/* determine if sw_marker is in the path */
	item = xstrtok(oldpath ,delim);
	while (item) {
		/* put the delimiter removed by xstrtok back in place */
		if (item > oldpath) {;
			*(item - 1) = *delim;
		}
		if (strcmp(item ,sw_marker)) {
			/* this function uses the key, not the value */
			hentry = Tcl_CreateHashEntry(oldpathhash ,item ,&i);
			if (i) {
				Tcl_SetHashValue(hentry, item);
			}
		} else { /* found sw_marker */
			/* we want the first occurrence */
			if (!marked) {
				marked = item;
			}
		}
		item = xstrtok(NULL ,delim);
	}

	/* only include new paths that are not already in the existing path */
	pathlist = Tcl_GetString(objv[arg1+1]);
	item = xstrtok(pathlist ,delim);
	i = 0;
	while (item) {
		if (!Tcl_FindHashEntry(oldpathhash ,item)) {
			if (i) {
				Tcl_DStringAppend(qualifiedpath ,delim ,-1);
			}
			Tcl_DStringAppend(qualifiedpath ,item ,-1);
			i++;
		}
		item = xstrtok(NULL ,delim);
	}

    /**
     **  If all of the directories in the new path already exist,
     **  exit doing nothing.
     **/
	if (!Tcl_DStringValue(qualifiedpath))
		goto success1;

	if (!strlen(oldpath)) {
		Tcl_DStringAppend(newpath ,Tcl_DStringValue(qualifiedpath) ,-1);
	} else if (marked) {
		if (!append) {
		/* +1 to pick up delimiter, should be present via oldpath */
			marked+= strlen(sw_marker) + 1;
		}
		tmpchar = *marked;
		/* marked is a pointer into oldpath */
		*marked = '\0';
		Tcl_DStringAppend(newpath ,oldpath ,-1);
		*marked = tmpchar;
		/* delimiter should be in place in newpath (via oldpath) */
		Tcl_DStringAppend(newpath ,Tcl_DStringValue(qualifiedpath) ,-1);
		if (strlen(marked)) {
			Tcl_DStringAppend(newpath ,delim ,-1);
			Tcl_DStringAppend(newpath ,marked ,-1);
		}
	} else {
		if (append) {
			Tcl_DStringAppend(newpath ,oldpath ,-1);
		/* add a delimiter whether or not one is already there this
		**preserves POSIX semantics of delimiter at end meaning
		**"current directory"
		*/

		/* TODO: should trailing signifying "current directory" always
		** float to the end of the path?
		*/
			Tcl_DStringAppend(newpath , delim ,-1);
			Tcl_DStringAppend(newpath ,
				Tcl_DStringValue(qualifiedpath) ,-1);
		} else {
			Tcl_DStringAppend(newpath ,
				Tcl_DStringValue(qualifiedpath) ,-1);
			Tcl_DStringAppend(newpath ,delim ,-1);
			Tcl_DStringAppend(newpath ,oldpath ,-1);
		}
	}

    /**
     **  Now the new value to be set resides in 'newpath'. Set it up.
     **/
	moduleSetenv(interp, Tcl_GetString(objv[arg1]),
		Tcl_DStringValue(newpath), 1);
	_TCLCHK(interp);
    /**
     ** Free resources
     **/
success1:
	Tcl_DStringFree(newpath);
	Tcl_DStringFree(qualifiedpath);
	Tcl_DeleteHashTable(oldpathhash);
	/* FreeList(pathlist, numpaths); */
success0:
	return (TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
unwind2:
	Tcl_DStringFree(newpath);
	Tcl_DStringFree(qualifiedpath);
	Tcl_DeleteHashTable(oldpathhash);
unwind1:
	/* FreeList(pathlist, numpaths); */
unwind0:
	return (TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
}
  /** End of 'cmdSetPath' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:cmdRemovePath						     **
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
 **			int		 objc		Number of arguments  **
 **			Tcl_Obj		*objv[]		Argument array	     **
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

int cmdRemovePath(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	char           *sw_marker = APP_SW_MARKER,
						/** arbitrary default	     **/
	    **pathlist,				/** List of dirs	     **/
	    *arg;				/** argument ptr	     **/
	const char     *delim = _colon;		/** path delimiter	     **/
	int             numpaths,		/** number of dirs in path   **/
	                arg1 = 1,		/** arg start		     **/
	    x;					/** loop index		     **/
    /**
     **   Check arguments. There should be give 3 args:
     **     argv[0]  -  prepend/append/remove-path
     **     ...
     **     argv[n-1]-  varname
     **     argv[n]  -  value
     **/
	if (objc < 3)
		if (OK != ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				"path-variable directory", NULL))
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
	if (g_flags & M_DISPLAY) {
		fprintf(stderr, "%s\t ", Tcl_GetString(objv[0]));
		while (--objc)
			fprintf(stderr, "%s ", Tcl_GetString(*++objv));
		fprintf(stderr, "\n");
		goto success0;
	}

    /**
     **  prepend or append. The default is append.
     **/
	if (!strncmp(Tcl_GetString(objv[0]), "pre", 3))
		sw_marker = PRE_SW_MARKER;

    /**
     ** For switch state3, we're looking to remove the markers.
     **/
	if (g_flags & M_SWSTATE3)
		Tcl_SetStringObj(objv[arg1 + 1], sw_marker, -1);

    /**
     **  Check for the delimiter option
     **/
	arg = Tcl_GetString(objv[arg1]);
	if (*arg == '-') {
		if (!strcmp(arg, "-d")) {
			delim = Tcl_GetString(objv[arg1 + 1]);
			arg1 += 2;
		} else if (!strcmp(arg, "--delim")) {
			delim = Tcl_GetString(objv[arg1 + 1]);
			arg1 += 2;
		} else if (!strncmp(arg, "--delim=", 8)) {
			delim = arg + 8;
			arg1++;
		}
	}

    /**
     **  Split the path into its components so each item can be removed
     **  individually from the variable.
     **/
	if (! (pathlist =
	     uvec_vector(SplitIntoList(Tcl_GetString(objv[arg1 + 1]), &numpaths,
			   delim))))
		goto unwind0;

    /**
     ** Remove each item individually
     **/
	for (x = 0; x < numpaths; x++)
		if (TCL_OK !=
		    Remove_Path(interp, (char *)Tcl_GetString(objv[arg1]),
				pathlist[x], sw_marker, delim))
			goto unwind1;
    /**
     ** Free resources
     **/
	/* FreeList(pathlist, numpaths); */

success0:
	return (TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

unwind1:
	/* FreeList(pathlist, numpaths); */
unwind0:
	return (TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

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
	if(!(oldpath=(char *) TclGetEnv( interp, variable ))) {
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
		mhash_add(setenvHashTable, variable, Tcl_DStringValue(newpath));
		mhash_del(unsetenvHashTable, variable);

		/**
		**  Store the new PATH value into the environment.
		**/
		(void) TclSetEnv( interp, variable, Tcl_DStringValue(newpath));

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
		mhash_del(setenvHashTable, variable);
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
		(void) TclSetEnv(interp, variable, "");
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
