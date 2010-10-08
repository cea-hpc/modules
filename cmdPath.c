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

static char Id[] = "@(#)$Id: cmdPath.c,v 1.21 2010/10/08 21:40:19 rkowen Exp $";
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
	Tcl_RegExp      chkexpPtr;		/** Regular expression for   **/
						/** marker checking	     **/
	char           *oldpath,		/** Old value of 'var'	     **/
	               *newpath,		/** New value of 'var'	     **/
	               *sw_marker = APP_SW_MARKER,
						/** arbitrary default	     **/
	    *startp = NULL, *endp = NULL,	/** regexp match endpts	     **/
	    *qualifiedpath,			/** List of dirs which 
						    aren't already in path   **/
	    **pathlist,				/** List of dirs	     **/
	    *arg;				/** argument ptr	     **/
	const char     *delim = _colon;		/** path delimiter	     **/
	int             append = 1,		/** append or prepend	     **/
	    numpaths,				/** number of dirs in path   **/
	    qpathlen,				/** qualifiedpath length     **/
	    arg1 = 1,				/** arg start		     **/
	    x;					/** loop index		     **/
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
	    (char *)Tcl_GetVar2(interp, "env", Tcl_GetString(objv[arg1]),
				TCL_GLOBAL_ONLY);
	_TCLCHK(interp);

#if 0
	    if (!oldpath)
		oldpath = !strcmp(Tcl_GetString(objv[arg1]),
			    "MANPATH") ? DEFAULTMANPATH : "";
#else
	    if (!oldpath)
		oldpath	= "";
#endif

    /**
     **  Split the new path into its components directories so each
     **  directory can be checked to see whether it is already in the 
     **  existing path.
     **/
	if (!(pathlist = uvec_vector(SplitIntoList(
			Tcl_GetString(objv[arg1 + 1]), &numpaths, delim))))
		goto unwind0;

    /**
     **  Some space for the list of paths which
     **  are not already in the existing path.
     **/
	if (!(qualifiedpath =
	     stringer(NULL, 0, Tcl_GetString(objv[arg1 + 1]), delim, NULL)))
		if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
			goto unwind1;

	qpathlen = strlen(qualifiedpath) + 1;
	*qualifiedpath = '\0';		/** make sure null for later	     **/

	for (x = 0; x < numpaths; x++) {

		cleanse_path(pathlist[x], buffer, PATH_BUFLEN);

	/**
	 **  Check to see if path is already in this path variable.
	 **  It could be at the 
	 **     beginning ... ^path:
	 **     middle    ... :path:
	 **     end       ... :path$
	 **     only one  ... ^path$
	 **/
		if (!(newpath = stringer(NULL, 0,
			"(^", buffer, delim, ")|(", delim, buffer,
			delim, ")|(", delim, buffer, "$)|(^", buffer,
			"$)", NULL)))
			if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
				goto unwind2;

		chkexpPtr = Tcl_RegExpCompile(interp, newpath);
		_TCLCHK(interp);
		null_free((void *)&newpath);

	/**
	 **  If the directory is not already in the path, 
	 **  add it to the qualified path.
	 **/
		if (!Tcl_RegExpExec(interp, chkexpPtr, oldpath, oldpath))
			if (!stringer(qualifiedpath + strlen(qualifiedpath),
				     qpathlen - strlen(qualifiedpath),
				     pathlist[x], delim, NULL))
				if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
					goto unwind2;

	}				/** End of loop that checks for 
					 ** already existent path
					 **/
    /**
     **  If all of the directories in the new path already exist,
     **  exit doing nothing.
     **/
	if (!*qualifiedpath)
		goto success1;

	/* remove trailing delimiter */
	qualifiedpath[strlen(qualifiedpath) - 1] = '\0';

    /**
     **  Some space for our newly created path.
     **  We size at the oldpath plus the addition.
     **/
	if (!(newpath = stringer(NULL, strlen(oldpath) +
		strlen(qualifiedpath) + 2, NULL)))
		if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
			goto unwind2;
	*newpath = '\0';

    /**
     **  Easy job to do, if the old path has not been set up so far ...
     **/
	if (!strcmp(oldpath, "")) {
		strcpy(newpath, qualifiedpath);

    /**
     **  Otherwise we have to take care on prepending vs. appending ...
     **  If there is a append or prepend marker within the variable (see
     **  modules_def.h) the changes are made according to this markers. Other-
     **  wise append and prepend will be relative to the strings begin or end.
     **/

	} else {

		Tcl_RegExp      markexpPtr =
		    Tcl_RegExpCompile(interp, sw_marker);
		_TCLCHK(interp);

		    strcpy(newpath, oldpath);

		if (Tcl_RegExpExec(interp, markexpPtr, oldpath, oldpath)) {
			_TCLCHK(interp);
			Tcl_RegExpRange(markexpPtr, 0,
			    (CONST84 char **)&startp,
			    (CONST84 char **)&endp);

	    /**
	     **  Append/Prepend marker found
	     **/
			if (append) {
				char            ch = *startp;
				*startp = '\0';
				strcpy(newpath, oldpath);
		/**
                 ** check that newpath has a value before adding delim
                 **/
				if (strlen(newpath) > 0
				    && newpath[strlen(newpath) - 1] != *delim)
					strcat(newpath, delim);
				strcat(newpath, qualifiedpath);
				if (newpath[strlen(newpath) - 1] != *delim)
					strcat(newpath, delim);
				*startp = ch;
				strcat(newpath, startp);

			} else {
				char            ch = *endp;
				*endp = '\0';
				strcpy(newpath, oldpath);
				if (newpath[strlen(newpath) - 1] != *delim)
					strcat(newpath, delim);
				strcat(newpath, qualifiedpath);
				*endp = ch;
				strcat(newpath, endp);
			}

		} else {

	    /**
	     **  No marker set
	     **/
			if (!append) {
				strcpy(newpath, qualifiedpath);
				if (*oldpath != *delim)
					strcat(newpath, delim);
				strcat(newpath, oldpath);
			} else {
				strcpy(newpath, oldpath);
				if (newpath[strlen(newpath) - 1] != *delim)
					strcat(newpath, delim);
				strcat(newpath, qualifiedpath);
			}
		} /** if( marker) **/
	} /** if( strcmp) **/
    /**
     **  Now the new value to be set resides in 'newpath'. Set it up.
     **/
	moduleSetenv(interp, Tcl_GetString(objv[arg1]), newpath, 1);
	_TCLCHK(interp);
    /**
     ** Free resources
     **/
	null_free((void *)&newpath);
success1:
	null_free((void *)&qualifiedpath);
	/* FreeList(pathlist, numpaths); */
success0:
	return (TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
unwind2:
	null_free((void *)&qualifiedpath);
unwind1:
	/* FreeList(pathlist, numpaths); */
unwind0:
	return (TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
}
  /** End of 'cmdSetPath' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdRemovePath					     **
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

static int	Remove_Path(	Tcl_Interp	*interp,
	       			char		*variable,
	       			char		*item,
				char		*sw_marker,
				const char	*delim)
{
    char	*oldpath,			/** Old value of 'var'	     **/
    		*tmppath,			/** Temp. buffer for 'var'   **/
    		*searchpath,
		*startp=NULL, *endp=NULL;	/** regexp match endpts	     **/
    int  	 start_offset = 0,		/** match offsets	     **/
    		 end_offset = 0,
		 path_len = 0;			/** length of unmatched path **/
    Tcl_RegExp	regexpPtr  = (Tcl_RegExp) NULL,
    		begexpPtr  = (Tcl_RegExp) NULL,
    		midexpPtr  = (Tcl_RegExp) NULL,
    		endexpPtr  = (Tcl_RegExp) NULL,
    		onlyexpPtr = (Tcl_RegExp) NULL,
    		markexpPtr = (Tcl_RegExp) NULL;
    /**
     **  Get the current value of the "PATH" environment variable
     **/
    if(!(tmppath=(char *)Tcl_GetVar2(interp,"env",variable,TCL_GLOBAL_ONLY))) {
	_TCLCHK(interp);
	goto success0;			/** -------- EXIT (SUCCESS) -------> **/
    }    

    /**
     **  We want to make a local copy of old path because we're going
     **  to be butchering it and the environ one doesn't need to be
     **  changed in this manner.
     **  Put a \ in front of each . and + in the passed value to remove.
     **/
    if(!(oldpath = stringer(NULL,0, tmppath, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;
    path_len = strlen(oldpath);
    cleanse_path( item, buffer, PATH_BUFLEN);

    /**
     **  Now we need to find it in the path variable.  So, we create
     **  space enough to build search expressions.
     **/
    if(!(searchpath = stringer(NULL,0,"^", buffer,
	delim, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind1;

    /**
     **  This section searches for the oldpath in the PATH variable.
     **  There are four cases because the colons must be used as markers
     **  in order to guarantee that a partial match isn't being found.
     ** 
     **  The start_offset and end_offset variables indicate how much to 
     **  cut or not to cut off of each end of the located
     **  string.  They differ for each case...beginning of
     **  the path, middle of the path, end of the path, 
     **  and for the only path.
     ** 
     **  This is to ensure the colons get cut off properly.
     **/

    begexpPtr = Tcl_RegExpCompile(interp,  searchpath);
    _TCLCHK(interp);

    if( Tcl_RegExpExec(interp, begexpPtr, oldpath, oldpath) > 0) {
	_TCLCHK(interp);
	regexpPtr = begexpPtr;

	/**
	 **  Copy from the beginning of the startp to one
	 **  character before endp
	 **/

	Tcl_RegExpRange(begexpPtr, 0,
		(CONST84 char **) &startp, (CONST84 char **) &endp);
	start_offset = 0; end_offset = -1;
	path_len -= (endp - startp - 1);

    } else {

	*searchpath = *delim;	/* :buffer: */
	midexpPtr = Tcl_RegExpCompile(interp, searchpath);
	_TCLCHK(interp);

	if( Tcl_RegExpExec(interp, midexpPtr, oldpath, oldpath) > 0) {
	    _TCLCHK(interp);
	    regexpPtr = midexpPtr;

	    /**
	     **  Copy from one character after stringp[0] to one
	     **  character before endp[0]
	     **/

	    Tcl_RegExpRange(midexpPtr, 0,
		(CONST84 char **) &startp, (CONST84 char **) &endp);
	    start_offset = 1; end_offset = -1;
	    path_len -= (endp - startp - 2);

	} else {

	    searchpath[strlen(searchpath)-1] = '$';	/* :buffer$ */
	    endexpPtr = Tcl_RegExpCompile(interp, searchpath);
	    _TCLCHK(interp);

	    if( Tcl_RegExpExec(interp, endexpPtr, oldpath, oldpath) > 0) {
		_TCLCHK(interp);
		regexpPtr = endexpPtr;

		/**
		 **  Copy from one character after stringp[0] 
		 **  through endp[0]
		 **/

		Tcl_RegExpRange(endexpPtr, 0,
			(CONST84 char **) &startp, (CONST84 char **) &endp);
		start_offset = 0; end_offset = 0;
		path_len -= (endp - startp - 1);

	    } else {

		*searchpath = '^';	/* ^buffer$ */
		onlyexpPtr = Tcl_RegExpCompile(interp, searchpath);
		_TCLCHK(interp);

		if(Tcl_RegExpExec(interp, onlyexpPtr, oldpath, oldpath) > 0) {
		    _TCLCHK(interp);

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
		    (void) Tcl_SetVar2( interp, "env", variable, "",
			TCL_GLOBAL_ONLY);
		    _TCLCHK(interp);
		    null_free((void *) &searchpath);
		    goto success1;
		}
	    }
	}
    }
    null_free((void *) &searchpath);

    /**
     **  If I couldn't find it assume it wasn't there
     **  and return.
     **/
    if( !regexpPtr)
	goto success1;

    markexpPtr = Tcl_RegExpCompile(interp, sw_marker);
    _TCLCHK(interp);

    /**
     **  In state1, we're actually replacing old paths with
     **  the markers for future appends and prepends.
     **  
     **  We only want to do this once to mark the location
     **  the module was formed around.
     **/
    if((g_flags & M_SWSTATE1) && ! (Tcl_RegExpExec(interp,
	 markexpPtr, oldpath, oldpath) > 0)) {

	/**
	 **  If I don't have enough space to replace the oldpath with the
	 **  marker, then I must create space for the marker and thus
	 **  recreate the PATH variable.
	 **/
	char* newenv;
	int newlen = path_len + strlen(sw_marker) + 1;
      
	if(!(newenv = stringer(NULL, newlen ,NULL) ))
	    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		goto unwind1;

	*(startp + start_offset) = '\0';

	if(*(endp + end_offset) == '\0') {
	    (void) stringer(newenv, newlen, oldpath,delim,sw_marker,NULL);
	} else {
	    (void) stringer(newenv, newlen, oldpath,sw_marker,
		endp + end_offset, NULL);
	}

	/**
	 **  Just store the value with the marker into the environment.  I'm not
	 **  checking if it changed because the only case that a PATH-type
	 **  variable will be unset is when I remove the only path remaining in
	 **  the variable.  So, using moduleSetenv is not necessary here.
	 **/
	Tcl_SetVar2( interp, "env", variable, newenv, TCL_GLOBAL_ONLY);
	_TCLCHK(interp);
	null_free((void *) &newenv);

    } else {  /** SW_STATE1 **/

	/**
	 **  We must be in SW_STATE3 or not in SW_STATE at all.
	 **  Removing the marker should be just like removing any other path.
	 **/
	strcpy( startp + start_offset, endp);

	/**
	 **  Cache the set.  Clear the variable from the unset table just
	 **  in case it was previously unset.
	 **/
	mhash_add(setenvHashTable, variable, oldpath);
	mhash_del(unsetenvHashTable, variable);

	/**
	 **  Store the new PATH value into the environment.
	 **/
	Tcl_SetVar2( interp, "env", variable, oldpath, TCL_GLOBAL_ONLY);
	_TCLCHK(interp);

    }  /** ! SW_STATE1 **/
    /**
     **  Free what has been used and return on success
     **/
success1:
    null_free((void *) &oldpath);
success0:
    return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

unwind1:
    null_free((void *) &oldpath);
unwind0:
    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

} /** End of 'Remove_Path' **/
