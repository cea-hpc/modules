/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdPath.c					     **
 **   First Edition:	91/10/23					     **
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

static char Id[] = "@(#)$Id: cmdPath.c,v 1.1 2000/06/28 00:17:32 rk Exp $";
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

/** not applicable **/
#define _TCLCHK(a)	\
	{if (*(a)->result) ErrorLogger(ERR_EXEC,LOC,(a)->result,NULL);}

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "cmdPath.c";	/** File name of this module **/

#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdSetPath[] = "cmdSetPath";
static	char	_proc_cmdRemovePath[] = "cmdRemovePath";
#endif

static char buffer[ PATH_BUFLEN];

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdSetPath					     **
 ** 									     **
 **   Description:	Add the passed value (argv[2]) to the specified vari-**
 **			able (argv[1]). argv[0] specifies, if the variable   **
 **			is to be appended or prepended.			     **
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

int	cmdSetPath(	ClientData	 client_data,
	       		Tcl_Interp	*interp,
	       		int		 argc,
	       		char		*argv[])
{
    Tcl_RegExp	chkexpPtr;			/** Regular expression for   **/
						/** marker checking	     **/
    char	*oldpath;			/** Old value of 'var'	     **/
    char	*newpath;			/** New value of 'var'	     **/
    char	*sw_marker = APP_SW_MARKER;	/** arbitrary default	     **/
    char	*startp, *endp;			/** regexp match endpts	     **/
    int		 append = 1;			/** append or prepend	     **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdSetPath, NULL);
#endif

    /**
     **  Whatis mode?
     **/

    if( flags & (M_WHATIS | M_HELP))
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
	
    /**
     **   Check arguments. There should be give 3 args:
     **     argv[0]  -  prepend/append
     **     argv[1]  -  varname
     **     argv[2]  -  value
     **/

    if(argc != 3) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "path-variable directory",
	    NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Should this guy be removed from the variable ... If yes, do so!
     **/

    if(flags & M_REMOVE) 
	return( cmdRemovePath(client_data, interp, argc, argv));   /** ----> **/

    /**
     **  prepend or append. The default is append.
     **/

    if( !( append = strncmp( argv[0], "pre", 3)))
	sw_marker = PRE_SW_MARKER;
  
    /**
     **  Display only ... ok, let's do so!
     **/

    if(flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }
  
    /**
     **  Get the old value of the variable. MANPATH defaults to "/usr/man".
     **  Put a \ in front of each '.' and '+'.
     **/

    oldpath = Tcl_GetVar2( interp, "env", argv[1], TCL_GLOBAL_ONLY);
    _TCLCHK(interp)

    if( oldpath == NULL)
	oldpath = !strcmp( argv[1], "MANPATH") ? "/usr/man" : "";

    cleanse_path( argv[2], buffer, PATH_BUFLEN);

    /**
     **  Check to see if path is already in this path variable.
     **  It could be at the 
     **     beginning ... ^path:
     **     middle    ... :path:
     **     end       ... :path$
     **     only one  ... ^path$
     **/

    if( NULL == (newpath = (char*) malloc( 4*( strlen( buffer) + 5)))) {
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /* sprintf( newpath, "(^%s:)|(:%s:)|(:%s$)|(^%s$)",
	buffer, buffer, buffer, buffer); */
    strcpy( newpath, "(^");
    strcat( newpath, buffer);
    strcat( newpath, ":)|(:");
    strcat( newpath, buffer);
    strcat( newpath, ":)|(:");
    strcat( newpath, buffer);
    strcat( newpath, "$)|(^");
    strcat( newpath, buffer);
    strcat( newpath, "$)");
    chkexpPtr = Tcl_RegExpCompile(interp, newpath);
    _TCLCHK(interp)
    free( newpath);

    /**
     **  If the one is set in the variable so far, we may return on
     **  doing nothing
     **/

    if( Tcl_RegExpExec(interp, chkexpPtr, oldpath, oldpath)) {
	return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
    }

    /**
     **  Some space for our newly created path.
     **  We size at the oldpath plus the addition.
     **/

    if( NULL == (newpath = (char*) malloc( strlen( oldpath) +
	strlen( argv[2]) + 2))) {
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Easy job to do, if the old path has not been set up so far ...
     **/

    if( !strcmp( oldpath, "")) {
	strcpy( newpath, argv[2]);

    /**
     **  Otherwise we have to take care on prepending vs. appending ...
     **  If there is a append or prepend marker within the variable (see
     **  modules_def.h) the changes are made according to this markers. Other-
     **  wise append and prepaend will be relative to the strings begin or end.
     **/

    } else {

	Tcl_RegExp	markexpPtr = Tcl_RegExpCompile(interp, sw_marker);
	_TCLCHK(interp)

	strcpy( newpath, oldpath);

	if( Tcl_RegExpExec(interp, markexpPtr, oldpath, oldpath)) {
	    _TCLCHK(interp)
	    Tcl_RegExpRange(markexpPtr, 0, &startp, &endp);

	    /**
	     **  Append/Prepend marker found
	     **/

	    if( !append) {
		char ch = *endp;
		*endp = '\0';
		/* sprintf(newpath, "%s:%s", oldpath, argv[2]); */
		strcpy(newpath, oldpath);
		strcat(newpath, ":");
		strcat(newpath, argv[2]);
		*endp = ch;
		strcat(newpath, endp);
	    } else {
		char ch = *startp;
		*startp = '\0';
		/* sprintf(newpath, "%s%s:", oldpath, argv[2]); (error?) */
		strcpy(newpath, argv[2]);
		strcat(newpath, ":");
		strcat(newpath, oldpath);
		*startp = ch;
		strcat(newpath, startp);
	    }

	} else {

	    /**
	     **  No marker set
	     **/

	    if( !append) {
		/* sprintf( newpath, "%s:%s", argv[2], oldpath); */
		strcpy(newpath, argv[2]);
		strcat(newpath, ":");
		strcat(newpath, oldpath);
	    } else {
		/* sprintf( newpath, "%s:%s", oldpath, argv[2]); */
		strcpy(newpath, oldpath);
		strcat(newpath, ":");
		strcat(newpath, argv[2]);
	    }

	} /** if( marker) **/

	free((char*) markexpPtr);

    } /** if( strcmp) **/

    /**
     **  Now the new value to be set resides in 'newpath'. Set it up, free what
     **  has been allocated and return on success
     **/

    moduleSetenv( interp, argv[1], newpath, 1);
    _TCLCHK(interp)
    free( newpath);

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdSetPath, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdSetPath' **/

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

int	cmdRemovePath(	ClientData	 client_data,
	       		Tcl_Interp	*interp,
	       		int		 argc,
	       		char		*argv[])
{
    char	*oldpath,			/** Old value of 'var'	     **/
    		*tmppath,			/** Temp. buffer for 'var'   **/
    		*searchpath,
    		*sw_marker = APP_SW_MARKER;	/** arbitrary default	     **/
    int  	 start_offset = 0,
    		 end_offset = 0;
    Tcl_RegExp	regexpPtr  = (Tcl_RegExp) NULL,
    		begexpPtr  = (Tcl_RegExp) NULL,
    		midexpPtr  = (Tcl_RegExp) NULL,
    		endexpPtr  = (Tcl_RegExp) NULL,
    		onlyexpPtr = (Tcl_RegExp) NULL,
    		markexpPtr = (Tcl_RegExp) NULL;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdRemovePath, NULL);
#endif

    /**
     **   Check arguments. There should be give 3 args:
     **     argv[0]  -  prepend/append
     **     argv[1]  -  varname
     **     argv[2]  -  value
     **/

    if(argc != 3) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "path-variable directory",
	    NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Display only ... ok, let's do so!
     **/

    if(flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	while( --argc)
	    fprintf( stderr, "%s ", *++argv);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }
  
    /**
     **  prepend or append. The default is append.
     **/

    if( ! strncmp( argv[0], "pre", 3))
	sw_marker = PRE_SW_MARKER;
  
    /**
     ** For switch state3, we're looking to remove the markers.
     **/

    if( flags & M_SWSTATE3) 
	argv[2] = sw_marker;

    /**
     **  Get the current value of the "PATH" environment variable
     **/

    if( NULL == (tmppath = Tcl_GetVar2( interp, "env", argv[1],
	TCL_GLOBAL_ONLY))) {
	_TCLCHK(interp)
	return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
    }    

    /**
     **  We want to make a local copy of old path because we're going
     **  to be butchering it and the environ one doesn't need to be
     **  changed in this manner.
     **  Put a \ in front od each . and + in the passed value to remove.
     **/

    if( NULL == (oldpath = (char*) malloc( strlen( tmppath) + 1))) {
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    strcpy( oldpath, tmppath);

    cleanse_path( argv[2], buffer, PATH_BUFLEN);

    /**
     **  Now we need to find it in the path variable.  So, we create
     **  space enough to build search expressions.
     **/

    if( NULL == (searchpath = (char*)  malloc( strlen( buffer) + 3))) {
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL)) {
	    free( oldpath);
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
	}
    }

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

    /* sprintf( searchpath, "^%s:", buffer); */
    strcpy( searchpath, "^");
    strcat( searchpath, buffer);
    strcat( searchpath, ":");
    begexpPtr = Tcl_RegExpCompile(interp,  searchpath);
    _TCLCHK(interp)

    if( Tcl_RegExpExec(interp, begexpPtr, oldpath, oldpath)) {
	_TCLCHK(interp)
	regexpPtr = begexpPtr;

	/**
	 **  Copy from the beginning of the stringp[0] to one
	 **  character before endp[0]
	 **/

	start_offset = 0; end_offset = -1;

    } else {

	/* sprintf( searchpath, ":%s:", buffer); */
	strcpy( searchpath, ":");
	strcat( searchpath, buffer);
	strcat( searchpath, ":");
	midexpPtr = Tcl_RegExpCompile(interp, searchpath);
	_TCLCHK(interp)

	if( Tcl_RegExpExec(interp, midexpPtr, oldpath, oldpath)) {
	    _TCLCHK(interp)
	    regexpPtr = midexpPtr;

	    /**
	     **  Copy from one character after stringp[0] to one
	     **  character before endp[0]
	     **/

	    start_offset = 1; end_offset = -1;

	} else {

	    /* sprintf( searchpath, ":%s$", buffer); */
	    strcpy( searchpath, ":");
	    strcat( searchpath, buffer);
	    strcat( searchpath, "$");
	    endexpPtr = Tcl_RegExpCompile(interp, searchpath);
	    _TCLCHK(interp)

	    if( Tcl_RegExpExec(interp, endexpPtr, oldpath, oldpath)) {
		_TCLCHK(interp)
		regexpPtr = endexpPtr;

		/**
		 **  Copy from one character after stringp[0] 
		 **  through endp[0]
		 **/

		start_offset = 0; end_offset = 0;

	    } else {

		/* sprintf( searchpath, "^%s$", buffer); */
		strcpy( searchpath, "^");
		strcat( searchpath, buffer);
		strcat( searchpath, "$");
		onlyexpPtr = Tcl_RegExpCompile(interp, searchpath);
		_TCLCHK(interp)

		if(Tcl_RegExpExec(interp, onlyexpPtr, oldpath, oldpath)) {
		    _TCLCHK(interp)

		    /**
		     **  In this case, I should go ahead and unset the variable
		     **  from the environment because I'm removing the very last
		     **  path.
		     **
		     **  First I'm going to clear the variable from the
		     **  setenvHashTable just in case it has already been altered
		     **  and had a significant value at the time. It's very
		     **  possible that I'm removing the only two or three paths
		     **  from this variable. If that's the case, then all the
		     **  earlier paths were marked for output in this hashTable.
		     **
		     **  Secondly, I actually mark the the environment variable
		     **  to be unset when output.
		     **/

		    clear_hash_value( setenvHashTable, argv[1]);
		    moduleUnsetenv( interp, argv[1]);

		    /**
		     **  moduleUnsetenv doesn't unset the variable in the Tcl
		     **  space because the $env variable might need to be
		     **  used again in the modulefile for locating other
		     **  paths.  BUT, since this was a path-type environment
		     **  variable, the user is expecting this to be empty
		     **  after removing the only remaining path.  So, I set
		     **  the variable empty here.
		     **/

		    (void) Tcl_SetVar2( interp, "env", argv[1], "",
			TCL_GLOBAL_ONLY);
		    _TCLCHK(interp)
		    free( searchpath);
		    free( oldpath);
		    return( TCL_OK);	/** -------- EXIT (SUCCESS) -------> **/
		}
	    }
	}
    }

    free( searchpath);

    /**
     **  If I couldn't find it assume it wasn't there
     **  and return.
     **/

    if( !regexpPtr) {
	free( oldpath);
	return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
    }

    markexpPtr = Tcl_RegExpCompile(interp, sw_marker);
    _TCLCHK(interp)

    /**
     **  In state1, we're actually replacing old paths with
     **  the markers for future appends and prepends.
     **  
     **  We only want to do this once to mark the location
     **  the module was formed around.
     **/

    if((flags & M_SWSTATE1) && Tcl_RegExpExec(interp,
	 markexpPtr, oldpath, oldpath)) {

	/**
	 **  If I don't have enough space to replace the oldpath with the
	 **  marker, then I must create space for the marker and thus
	 **  recreate the PATH variable.
	 **/

	char* newenv, *startp=NULL, *endp=NULL;
	_TCLCHK(interp)
	Tcl_RegExpRange(markexpPtr, 0, &startp, &endp);
      
	if( NULL == ( newenv = (char*) malloc(
	    startp + start_offset - oldpath + strlen(sw_marker) +
	    strlen(argv[1]) + strlen(endp) + end_offset + 1) )) {

	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL)) {
		free( oldpath);
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	    }
	}

	Tcl_RegExpRange(regexpPtr, 0, &startp, &endp);
	*(startp + start_offset) = '\0';

	if(*(endp + end_offset) == '\0') {
	    /* sprintf( newenv, "%s:%s", oldpath, sw_marker); */
	    strcpy( newenv, oldpath);
	    strcat( newenv, ":");
	    strcat( newenv, sw_marker);
	} else {
	    /* sprintf( newenv, "%s%s%s", oldpath, sw_marker, endp +
		end_offset); */
	    strcpy( newenv, oldpath);
	    strcat( newenv, sw_marker);
	    strcat( newenv, endp + end_offset);
	}

	/**
	 **  Just store the value with the marker into the environment.  I'm not
	 **  checking to see if it changed because the only case that a PATH-type
	 **  variable will be unset is when I remove the only path remaining in
	 **  the variable.  So, using moduleSetenv is not necessary here.
	 **/

	Tcl_SetVar2( interp, "env", argv[1], newenv, TCL_GLOBAL_ONLY);
	_TCLCHK(interp)
	free( newenv);

    } else {  /** SW_STATE1 **/

	char *startp, *endp;

	/**
	 **  We must be in SW_STATE3 or not in SW_STATE at all.
	 **  Removing the marker should be just like removing any other path.
	 **/

	Tcl_RegExpRange(regexpPtr, 0, &startp, &endp);
	strcpy( startp + start_offset, endp);

	/**
	 **  Cache the set.  Clear the variable from the unset table just
	 **  in case it was previously unset.
	 **/

	store_hash_value( setenvHashTable, argv[1], oldpath);
	clear_hash_value( unsetenvHashTable, argv[1]);

	/**
	 **  Store the new PATH value into the environment.
	 **/

	Tcl_SetVar2( interp, "env", argv[1], oldpath, TCL_GLOBAL_ONLY);
	_TCLCHK(interp)

    }  /** SW_STATE1 **/

    /**
     **  Free what has been used and return on success
     **/

    free( oldpath);

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdRemovePath, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdRemovePath' **/
