/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		locate_module.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, rk@owen.sj.ca.us				     **
 ** 									     **
 **   Description: 	Contains the routines which locate the actual	     **
 **			modulefile given a modulefilename by looking in all  **
 **			of the paths in MODULEPATH. 			     **
 ** 									     **
 **   Exports:		Locate_ModuleFile				     **
 **			SortedDirList					     **
 **			SplitIntoList					     **
 **			FreeList					     **
 **			SourceRC					     **
 **			SourceVers					     **
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

static char Id[] = "@(#)$Id: locate_module.c,v 1.7 2002/08/14 21:07:52 lakata Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"
#include "uvec.h"

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

#define	SRCFRAG	100

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "locate_module.c";	/** File name of this module **/

#if WITH_DEBUGGING_LOCATE
static	char	_proc_Locate_ModuleFile[] = "Locate_ModuleFile";
#endif
#if WITH_DEBUGGING_LOCATE_1
static	char	_proc_GetModuleName[] = "GetModuleName";
#endif
#if WITH_DEBUGGING_UTIL_1
static	char	_proc_SortedDirList[] = "SortedDirList";
static	char	_proc_SplitIntoList[] = "SplitIntoList";
#endif
#if WITH_DEBUGGING_UTIL_2
static	char	_proc_FreeList[] = "FreeList";
#endif

static	char	buf[ MOD_BUFSIZE];
static	char	modfil_buf[ MOD_BUFSIZE];

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	int	  filename_compare( const void*, const void*);
static	char	 *GetModuleName( Tcl_Interp*, char*, char*, char*);

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		filename_compare				     **
 ** 									     **
 **   Description: 	This is a reverse compare function to reverse the    **
 **			filename list. The function is used as compare func- **
 **			tion for qsort.					     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const void	*fi1	First filename to compare    **
 **			const void	*fi2	Second filename to compare   **
 ** 									     **
 **   Result:		int	-1	filename 1 > filename 2		     **
 **				0	filename 1 == filename 2	     **
 **				1	filename 1 < filename 2		     **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static int  filename_compare(	const void	*fi1,
				const void	*fi2)
{
    return strcmp(*(char**)fi2, *(char**)fi1);
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Locate_ModuleFile				     **
 ** 									     **
 **   Description:	Searches for a modulefile given a string argument    **
 **			which is either a full path or a modulefile name     **
 **			-- usually the argument the user gave. If it's not a **
 **			full path, the directories in the MODULESPATH	     **
 **			environment variable are searched to find a match    **
 **			for the given name.				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl interpr.**
 **			char		*modulename	Name of the module to**
 **							be located	     **
 **			char		*realname	Real modulename (with**
 **							version)	     **
 **			char		*filename	Real    full module  **
 **							file path	     **
 ** 									     **
 **   Result:		int		TCL_OK or TCL_ERROR		     **
 **			filename	the full path of the required module **
 **					file is copied in here		     **
 ** 									     **
 **   Attached Globals:	g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Locate_ModuleFile(	Tcl_Interp	*interp,
                  	char		*modulename,
                  	char		*realname,
                  	char		*filename)
{
    char	*p;			/** Tokenization pointer	     **/
    char	*result = NULL;		/** This functions result	     **/
    char	**pathlist;		/** List of paths to scan	     **/
    int		  numpaths,		/** Size of this list		     **/
    		  i;			/** Loop counter		     **/
    char	*modulespath;		/** Buffer for the contents of the   **/
					/** environment variable MODULEPATH  **/
    const char	*mod, *vers;		/** Module and version name for sym- **/
					/** bolic name lookup		     **/

    /**
     **  If it is a full path name, that's the module file to load.
     **/
#if WITH_DEBUGGING_LOCATE
    ErrorLogger( NO_ERR_START, LOC, _proc_Locate_ModuleFile, "modulename = '",
	modulename, "'", NULL);
#endif

    if( !modulename) 
	if( OK != ErrorLogger( ERR_PARAM, LOC, "modulename", NULL))
	    goto unwind0;
	
    /**
     ** given a full or relative path
     **/
    if( modulename[0] == '/' || modulename[0] == '.') {

	p = (char*) strrchr( modulename, '/');
        if(p) {
            *p = '\0';
	    /**
	     **  Check, if what has been specified is a valid version of
	     **  the specified module ...
	     **/
            if(! (result = GetModuleName(interp, modulename, NULL,(p+1)))) 
		goto unwind0;
	    /**
	     **  Reinstall the 'modulefile' which has been corrupted by
	     **   tokenization
	     **/
	    *p = '/';

	    /**
	     **  ... Looks good! Conditionally (if there has been no version
	     **  specified) we have to add the default version
	     **/
            if( !strcmp((p + 1), result)) {
                if ((char *) NULL == stringer( filename, MOD_BUFSIZE,
		modulename, NULL))
		    goto unwind1;
            } else {
                if ((char *) NULL == stringer( filename, MOD_BUFSIZE,
		modulename,"/",result, NULL))
		    goto unwind1;
            }
        } else {
	    /**
	     **  Hmm! There's no backslash in 'modulename'. So it MUST begin
	     **  on '.' and MUST be part of the current directory
	     **/
            if( NULL == (result = GetModuleName( interp, modulename, NULL,
		modulename)))
		goto unwind0;
	
            if( !strcmp( modulename, result) ||
		(strlen( modulename) + 1 + strlen( result) + 1 > MOD_BUFSIZE)) {
                if ((char *) NULL == stringer( filename, MOD_BUFSIZE,
		modulename, NULL))
		    goto unwind1;
            } else {
                if ((char *) NULL == stringer( filename, MOD_BUFSIZE,
		modulename,"/",result, NULL))
		    goto unwind1;
            }
        }
    /**
     **  So it is not a full path name what has been specified. Scan the 
     **  MODULESPATH
     **/
    } else {
	/**
	 **  If I don't find a path in MODULEPATH, there's nothing to search.
	 **/
	if( !( modulespath = (char *) getenv( "MODULEPATH"))) {
	    if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL)) {
		g_current_module = NULL;
		goto unwind0;
	    }
	}
	/**
	 ** strip off any extraneous new lines
	 **/
	{ char *end;
	if ((char *) NULL != (end = strrchr(modulespath, '\n'))) *end = '\0';
	}
#if DEEP_DIRS
	/**
	 **  Expand the module name (in case it is a symbolic one). This must
	 **  be done once here in order to expand any aliases
	 **/
	if( VersionLookup( modulename, &mod, &vers)) {
	    if ((char *) NULL == stringer( buf, MOD_BUFSIZE,
	    mod,"/",vers, NULL))
		goto unwind0;
	    modulename = buf;
	}
#endif
	/**
	 **  Split up the MODULEPATH values into multiple directories
	 **/
	if( NULL == (pathlist = SplitIntoList(modulespath, &numpaths)))
	    goto unwind0;
	/**
	 **  Check each directory to see if it contains the module
	 **/
	for(i=0; i<numpaths; i++) {
	    if( NULL != (result = GetModuleName( interp, pathlist[i], NULL,
		modulename))) {

		if ((char *) NULL == stringer( filename, MOD_BUFSIZE,
			result, NULL))
		    goto unwind1;
		break;
	    }
#if DEEP_DIRS
	    /**
	     **  If we haven't found it, we should try to re-expand the module
	     **  name, because some rc file have been sourced
	     **/
	    if( VersionLookup( modulename, &mod, &vers)) {
                if ((char *) NULL == stringer( buf, MOD_BUFSIZE,
		mod,"/",vers, NULL))
		    goto unwind1;
		modulename = buf;
	    }
#endif
	} /** for **/
	/**
	 **  Free the memory created from the call to SplitIntoList()
	 **/
	FreeList( pathlist, numpaths);
	/**
	 **  If result still NULL, then we really never found it and we should
	 **  return ERROR and clear the full_path array for cleanliness.
	 **/
	if( !result) {
	    filename[0] = '\0';
	    goto unwind0;
	}
    } /** not a full path name **/	
    /**
     **  Free up what has been allocated and pass the result back to
     **  the caller and save the real module file name returned by
     **  GetModuleName
     **/
    strncpy( realname, result, MOD_BUFSIZE);
    if ((char *) NULL == stringer( realname, MOD_BUFSIZE, result, NULL))
	goto unwind1;
    null_free((void *) &result);

#if WITH_DEBUGGING_LOCATE
    ErrorLogger( NO_ERR_END, LOC, _proc_Locate_ModuleFile, NULL);
#endif

    return( TCL_OK);

unwind1:
    null_free((void *) &result);
unwind0:
    return( TCL_ERROR);
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		GetModuleName					     **
 ** 									     **
 **   Description:	Given a path and a module filename, this function    **
 **			checks to find the modulefile.			     **
 ** 									     **
 **   Notes:		This function is RECURSIVE			     **
 **									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl Interp.**
 **			char		*path		Path to start seeking**
 **			char		*prefix		Module name prefix   **
 **			char		*modulename	Name of the module   **
 ** 									     **
 **   Result:		char*	NULL	Any failure( parameters, alloc)	     **
 **				else	Pointer to an allocated buffer con-  **
 **					taining the complete module file path**
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	char	*GetModuleName(	Tcl_Interp	*interp,
			     	char		*path,
				char		*prefix,
			     	char		*modulename)
{
    struct stat	  stats;		/** Buffer for the stat() systemcall **/
    char	 *fullpath = NULL,	/** Buffer for creating path names   **/
		 *Result = NULL,	/** Our return value		     **/
		 *newprefix,		/** new prefix string (if needed)    **/
		 *dflt,			/** default version		     **/
		 *safedflt;		/** default version (local copy)     **/
    
#if WITH_DEBUGGING_LOCATE_1
    ErrorLogger( NO_ERR_START, LOC, _proc_GetModuleName, NULL);
#endif
    /**
     **  Check whether path/prefix/modulename is a directory
     **/
    if( prefix) {
	if((char *) NULL == (fullpath = stringer(NULL, 0,
	path,"/",prefix,"/",modulename, NULL)))
	    goto unwind0;
    } else {
	if((char *) NULL == (fullpath = stringer(NULL, 0,
	path,"/",modulename, NULL)))
	    goto unwind0;
    }

    if( !stat( fullpath, &stats)) {
	/**
	 **  If it is a directory
	 **/
    	if(S_ISDIR( stats.st_mode)) {
	    /**
	     ** Get the default version string
	     **   return NULL if not found
	     **/
	    if (!(dflt = GetDefault(interp, fullpath)))
		    goto unwind1;

	    if((char *) NULL == (safedflt = stringer(NULL, 0, dflt, NULL)))
		goto unwind1;

	    /**
	     **  This is the recursion
	     **/
	    if( prefix) {
		if((char *) NULL == (newprefix = stringer(NULL, 0,
		prefix,"/",modulename, NULL)))
		    goto unwind2;
	    } else {
		if((char *) NULL == (newprefix = stringer(NULL, 0,
		modulename, NULL)))
		    goto unwind2;
	    }

	    Result = GetModuleName( interp, path, newprefix, safedflt);
	    goto success2;

	} else {     /** if( path/prefix/modulename is a directory) **/
	    /**
	     **  Now 'modulename' should be a file
	     **/
	    Result = fullpath;
	    goto success1;

	} /** mod is a file **/
    } /** mod exists **/
    /**
     **  Free up temporary values and return what we've found
     **/
success2:
    null_free((void *) &fullpath);
success1:
    null_free((void *) &safedflt);
    null_free((void *) &newprefix);
success0:
    
#if WITH_DEBUGGING_LOCATE_1
    ErrorLogger( NO_ERR_END, LOC, _proc_GetModuleName, NULL);
#endif
    return( Result);			/** -------- EXIT (SUCCESS) -------> **/

unwind3:
    null_free((void *) &newprefix);
unwind2:
    null_free((void *) &safedflt);
unwind1:
    null_free((void *) &fullpath);
unwind0:
    return(NULL);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'GetModuleName' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		SortedDirList					     **
 ** 									     **
 **   Description:	Checks the given path for the given modulefile.	     **
 **			If the path + the module filename is the modulefile, **
 **			then it is returned as the first element in the list.**
 **			If the path + the module filename is a directory, the**
 **			directory is read and sorted as the list.	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char		*path		Path to start seeking**
 **			char		*modulename	Name of the module   **
 **			int		*listcnt	Buffer to return the **
 **							size of the created  **
 **							list in elements     **
 ** 									     **
 **   Result:		char**		NULL	Any failure (alloc, param)   **
 **					else	Base pointer to the newly    **
 **						created list.		     **
 **			*listcnt		Number of elements in the    **
 **						list if one was created, un- **
 **						changed otherwise	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char	**SortedDirList(	char		*path,
		     		char		*modulename,
		     		int		*listcnt)
{
    struct dirent	*file;		/** Directory entry		     **/
    struct stat    	 stats;		/** Stat buffer			     **/
    DIR			*subdirp;	/** Subdirectoy handle		     **/
    char		*full_path;	/** Sugg. full path (path + module)  **/
    char		**filelist;	/** Temp. base pointer of the list   **/
    int			 pathlen;	/** String length of 'fullpath'	     **/
    uvec		*uv;		/** unix vector			     **/
 
#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_SortedDirList, NULL);
#endif
    /**
     **  Create uvec object with a capacity of 20
     **/
    if (!(uv = uvec_ctor_(10,module_str_fns)))
	if( OK != ErrorLogger( ERR_MODLIB, LOC, "uvec_ctor_", NULL))
	    goto unwind0;
    /**
     **  Form the suggested module file name out of the passed path and 
     **  the name of the module. Alloc memory in order to do this.
     **/
    if((char *) NULL == (full_path = stringer(NULL, 0,
	path,"/",modulename, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;
    pathlen = strlen(full_path);
    
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_DEBUG, LOC, "full_path='", full_path, "'", NULL);
#endif
    /**
     **  Check whether this file exists. If it doesn't free up everything
     **  and return on failure
     **/
    if( stat( full_path, &stats))
	goto unwind2;
    /**
     **  If the suggested module file is a regular one, we've found what we've
     **  seeked for. Put it on the top of the list and return.
     **/
    if( S_ISREG( stats.st_mode)) {
	*listcnt = 1;

	if (uvec_push(uv,modulename))
	    if(OK != ErrorLogger(ERR_MODLIB, LOC, "uvec_push",NULL))
		goto unwind2;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_DEBUG, LOC, "Module '", modulename, "' found", NULL);
#endif

    	goto success;
    }
    /**
     **  What we've found is a directory
     **/
    if( S_ISDIR( stats.st_mode)) {
        char	*tbuf;		/** Buffer for the whole filename for each   **/
				/** content of the directory		     **/
        char	*mpath;		/** Pointer into *tbuf where to write the dir**/
				/** entry				     **/
        char	*mstart;	/** Pointer into *tbuf where the module name **/
				/** starts				     **/

	/**
	 **  Open the directory for reading
	 **/
	if( NULL == (subdirp = opendir( full_path))) {
#if 0
	/* if you can't open the directory ... is that really an error? */
	    if( OK != ErrorLogger( ERR_OPENDIR, LOC, full_path, NULL))
#endif
	    goto unwind2;
	}
	/**
	 **  Allocate a buffer for constructing complete file names
	 **  and initialize it with the directory part we do already know.
	 **/
        if( NULL == (tbuf = stringer(NULL, MOD_BUFSIZE, full_path,"/", NULL)))
	    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		goto unwind3;

	mstart = tbuf + strlen(path) + 1;	/* "path/" */
        mpath = (tbuf + pathlen + 1);
	/**
	 **  Now scan all entries of the just opened directory
	 **/
#if WITH_DEBUGGING_UTIL_2
	ErrorLogger( NO_ERR_DEBUG, LOC, "Reading directory '", full_path, "'", NULL);
#endif
	for(file = readdir( subdirp); file != NULL; file = readdir( subdirp)) {
	    /**
	     **  Now, if we got a real entry which is not '.*' or '..' and
	     **  finally is not a temporary file (which are defined to end
	     **  on '~' ...
	     **  And it's not a RCS or CVS directory entry
	     **/
	    if( file->d_name                && 
                *file->d_name != '.'        && 
                strcmp( file->d_name, "..")  &&
                file->d_name[ NLENGTH( file) - 1] != '~') {
		if (!strcmp( file->d_name, "RCS")  ||
                    !strcmp( file->d_name, "CVS") ) {
                	strcpy( mpath, file->d_name);
    			stat( tbuf, &stats);
    			if( S_ISDIR( stats.st_mode)) /* skip if dir */
				continue;
		}
		/**
		 **  ... build the full pathname and check for the magic
		 **  cookie or for another directory level in order to
		 **  validate this as a modulefile entry we're seeking for.
		 **/
                strcpy( mpath, file->d_name);
                if( check_magic( tbuf, MODULES_MAGIC_COOKIE,
			MODULES_MAGIC_COOKIE_LENGTH) ||
		    !stat(tbuf, &stats) ) {
		    /**
		     **  Yep! Found! Put it on the list
		     **/
		    if (uvec_push(uv,mstart))
			if(OK != ErrorLogger(ERR_MODLIB, LOC, "uvec_push",NULL))
			    goto unwindt;

                } /** if( mag. cookie or directory) **/
	    } /** if( not a dotfile) **/
	} /** for **/
	/**
	 **  sort the list
	 **/
	if (uvec_qsort(uv,filename_compare))
	    if(OK != ErrorLogger(ERR_MODLIB, LOC, "uvec_qsort",NULL))
		goto unwindt;
	*listcnt = uvec_number(uv);
	/**
	 **  Free up temporary values ...
	 **/
	if( -1 == closedir( subdirp))
	    if( OK != ErrorLogger( ERR_CLOSEDIR, LOC, full_path, NULL)) {
		goto unwind2;
	    }

    	goto success;
	if(0) {	
unwindt:
	    null_free((void*) &tbuf);
	    goto unwind3;
	}
    }
    if(0) {
success:
	null_free((void*) &full_path);
	/**
	 **  pathlist is uvec contents
	 **/
	if (!(filelist = uvec_vector(uv)))
		if( OK != ErrorLogger( ERR_MODLIB, LOC, "uvec_vector", NULL))
			goto unwind1;
	/**
	 **  destroy the uvec object, but leave the vector alone
	 **/
	if (uvec_dealloc(&uv))
		if( OK != ErrorLogger( ERR_MODLIB, LOC, "uvec_dealloc", NULL))
			goto unwind1;
	/**
	 **  Set up return values and pass the created list to the caller
	 **/
	return(filelist);		/** --- EXIT PROCEDURE (SUCCESS) --> **/
    }
    /**
     **  If it is nor a regular file, neither a directory, we don't support
     **  it at all ...
     **/
/** ??? What about links ??? **/

unwind3:
    if( -1 == closedir( subdirp))
	ErrorLogger( ERR_CLOSEDIR, LOC, full_path, NULL);
unwind2:
    null_free((void*) &full_path);
unwind1:
    if (uvec_dtor(&uv))
	ErrorLogger( ERR_MODLIB, LOC, "uvec_dealloc", NULL);

unwind0:
#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_SortedDirList, NULL);
#endif

    return( NULL);			/** --- EXIT PROCEDURE (FAILURE) --> **/

} /** End of 'SortedDirList' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		SplitIntoList					     **
 ** 									     **
 **   Description: 	Splits a path-type environment variable into an array**
 **			of char* list.					     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl Interp.**
 **			char		*pathenv	Path to split 	     **
 **			int		*numpaths	Buffer to write the  **
 **							number of array ele- **
 **							ments to.	     **
 ** 									     **
 **   Result:		char**		NULL	Any failure (alloc, param.)  **
 **					else	Base pointer of the created  **
 **						array			     **
 **			*numpaths		Number of elements if an ar- **
 **						ray has been created, unchan-**
 **						ged otherwise.		     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static char *list_xdup (char const *str, size_t n) {
	char *xstr = (char *) str;
	char *out = (char *) NULL;
	/* diddle with chars to force a null terminated string for xdup */
	char save = *(xstr+n);
	/* this violates char const * */
	*(xstr+n) = '\0';
	out = xdup(str);
	*(xstr+n) = save;
	return out;
}

static int list_free (char **str) {
	null_free((void *) str);
	return 0;
}

uvec_str list_fns = {
	UVEC_USER,
	list_xdup,
	list_free
};

char	**SplitIntoList(	char		*pathenv, 
		     		int		*numpaths) 
{
    char	**pathlist = NULL;	/** Temporary base pointer for the   **/
					/** array to be created		     **/
    uvec	 *uv;			/** temporary uvec object	     **/

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_SplitIntoList, NULL);
#endif
    /** 
     **  Parameter check
     **/
    if( !pathenv)
	if( OK != ErrorLogger( ERR_PARAM, LOC, "pathenv", NULL))
	    goto unwind0;

    /** 
     **  Create uvec object, split pathenv on ':'s in process
     **/
    if (!(uv = uvec_alloc_(list_fns)))
	if( OK != ErrorLogger( ERR_MODLIB, LOC, "uvec_alloc_", NULL))
	    goto unwind0;
    if (uvec_copy_str(uv,":",pathenv))
	if( OK != ErrorLogger( ERR_MODLIB, LOC, "uvec_copy_str", NULL))
	    goto unwind1;

    /**
     **  pathlist is uvec contents
     **/
    if (!(pathlist = uvec_vector(uv)))
	if( OK != ErrorLogger( ERR_MODLIB, LOC, "uvec_vector", NULL))
	    goto unwind1;
    *numpaths = uvec_number(uv);

    /**
     **  destroy the uvec object, but leave the vector alone
     **/
    if (uvec_dealloc(&uv))
	if( OK != ErrorLogger( ERR_MODLIB, LOC, "uvec_dealloc", NULL))
	    goto unwind1;

    return( pathlist);

unwind1:
    uvec_dtor(&uv);
unwind0:
    return( NULL);			/** -------- EXIT FAILURE -------> **/
} /** End of 'SplitIntoList' **/

#ifndef FreeList

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		FreeList					     **
 ** 									     **
 **   Description:	Frees a char* array type list.			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char	**list		Pointer to the list	     **
 **			int	  numelem	Number of elements in the    **
 ** 						list			     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void FreeList(	char	**list,
		int	  numelem)
{
    register int j;
    
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_FreeList, NULL);
#endif
    /**
     **  Nothing to do ?
     **/
    if( !list)
	return;
    /**
     **  Free all elements of the list
     **/
    for( j = 0; j < numelem; j++)
	if( list[j] != NULL)
	    null_free((void *) (list + j));
    /**
     **  Free the entire list
     **/
    null_free((void *) &list);

} /** End of 'FreeList' **/

#endif

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		SourceRC					     **
 ** 									     **
 **   Description:	Source the passed global RC file		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Tcl interpreter	     **
 **			char		*path		Path to be used      **
 **			char		*name		Name of the RC file  **
 **									     **
 **   Result:		int		TCL_OK		Success		     **
 **					TCL_ERROR	Failure		     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int SourceRC( Tcl_Interp *interp, char *path, char *name)
{
    struct stat	  stats;		/** Buffer for the stat() systemcall **/
    int 	  save_flags, i;
    char	 *buffer;
    int		  Result = TCL_OK;
#if 0
    static char	**srclist = (char **) NULL;
#endif
    static int	  listsize = 0, listndx = 0;

    /**
     **  If there's a problem with the input parameters it means, that
     **  we do not have to source anything
     **  Only a valid TCL interpreter should be there
     **/
    if( !path || !name)
	return( TCL_OK);

    if( !interp)
	return( TCL_ERROR);
    /**
     **  Build the full name of the RC file
     **  Avoid duplicate sourcing
     **/
    if ((char *) NULL == (buffer = stringer(NULL, 0, path,"/",name, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;
#if 0
    for( i=0; i<listndx; i++)
	if( !strcmp( srclist[ i], buffer))
	    goto unwind1;
#endif
    /**
     **  Check whether the RC file exists and has the magic cookie inside
     **/
    if( !stat( buffer, &stats)) {
	if( check_magic( buffer, MODULES_MAGIC_COOKIE, 
	    MODULES_MAGIC_COOKIE_LENGTH)) {
	    /**
	     **  Set the flags to 'load only'. This prevents accidently
	     **  printing something
	     **/
	    save_flags = g_flags;
	    g_flags = M_LOAD;
	    /**
	     **  Source now
	     **/
	    if( TCL_ERROR == Execute_TclFile( interp, buffer)) 
		if( OK != ErrorLogger( ERR_SOURCE, LOC, buffer, NULL)) 
		    Result = TCL_ERROR;
	    g_flags = save_flags;
	    /**
	     **  Save the currently sourced file in the list
	     **  Check whether the list is big enough to fit in a new entry
	     **/
#if 0
	    if( !listsize) {
		listsize = SRCFRAG;
		if((char **) NULL == (srclist = (char **) malloc( listsize *
		    sizeof( char **)))) {
		    ErrorLogger( ERR_ALLOC, LOC, NULL);
		    goto unwind1;
		}
	    } else if( listndx + 1 >= listsize) {
		listsize += SRCFRAG;
		if((char **) NULL == (srclist = (char **) realloc( srclist, 
		    listsize * sizeof( char **)))) {
		    ErrorLogger( ERR_ALLOC, LOC, NULL);
		    goto unwind1;
		}
	    }
	    /**
	     **  Put the current RC files name on the list
	     **/
	    srclist[ listndx++] = buffer;
#endif

	} else {
	    ErrorLogger( ERR_MAGIC, LOC, buffer, NULL);
	    null_free((void *) &buffer);
	}
    } /** if( !stat) **/
    /**
     **  Return our result
     **/
    return( Result);

unwind1:
    null_free((void *) &buffer);
unwind0:
    return( TCL_ERROR);

} /** End of 'SourceRC' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		SourceVers					     **
 ** 									     **
 **   Description:	Source the '.version' file			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Tcl interpreter	     **
 **			char		*path		Path to be used      **
 **			char		*name		Name of the module   **
 **									     **
 **   Result:		int		TCL_OK		Success		     **
 **					TCL_ERROR	Failure		     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int SourceVers( Tcl_Interp *interp, char *path, char *name)
{
    struct stat	  stats;		/** Buffer for the stat() systemcall **/
    int save_flags;
    char *buffer;
    int Result = TCL_OK;
    char *version;
    char *new_argv[4];
    char *mod, *ver;

    /**
     **  If there's a problem with the input parameters it means, that
     **  we do not have to source anything
     **  Only a valid TCL interpreter should be there
     **/
    if( !path || !name)
	return( TCL_OK);
    if( !interp)
	return( TCL_ERROR);
#if 0
    /**
     **  No default version defined so far?
     **/
    if( VersionLookup( name, &mod, &ver) &&
	strcmp( ver, _default))
	return( TCL_OK);
#endif
    /**
     **  Build the full name of the RC file and check whether it exists and
     **  has the magic cookie inside
     **/
    if ((char *) NULL == (buffer = stringer(NULL, 0, path,"/",version_file,
	NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    return( TCL_ERROR);
    if( !stat( buffer, &stats)) {
	if(
#if VERSION_MAGIC != 0
	    check_magic( buffer, MODULES_MAGIC_COOKIE, 
	    MODULES_MAGIC_COOKIE_LENGTH)
#else
	1
#endif
	) {
	    save_flags = g_flags;
	    g_flags = M_LOAD;

	    if( TCL_ERROR != (Result = Execute_TclFile( interp, buffer)) && 
		(version = Tcl_GetVar( interp, "ModulesVersion", 0))) {
		/**
		 **  The version has been specified in the
		 **  '.version' file. Set up the result code
		 **/
		null_free((void *) &buffer);
		if ((char *) NULL == (buffer = stringer(NULL, 0,
		name,"/",version, NULL)))
		    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
			return( TCL_ERROR);

		new_argv[0] = "module-version";
		new_argv[1] = buffer;
		new_argv[2] = _default;
		new_argv[3] = NULL;
		/**
		 **  Define the default version
		 **/
		if( TCL_OK != cmdModuleVersion( (ClientData) 0,
		    (Tcl_Interp *) NULL, 3, new_argv)) {
			Result = TCL_ERROR;
		}
	    } /** if( Execute...) **/

	    g_flags = save_flags;

	} else
	    ErrorLogger( ERR_MAGIC, LOC, buffer, NULL);

    } /** if( !stat) **/
    /**
     ** free buffer memory
     **/
    null_free((void *) &buffer);
    /**
     **  Result determines if this was successfull
     **/

    return( Result);

} /** End of 'SourceVers' **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		GetDefault					     **
 ** 									     **
 **   Description:	Source the '.version' and '.modulerc' file	     **
 **			check the dirlist if neither is successful	     **
 **			return the default version as an malloc'd string.    **
 ** 									     **
 **   First Edition:	2002/06/19					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Tcl interpreter	     **
 **			char		*path		Path to use	     **
 **									     **
 **   Result:		char *default	string		Success		     **
 **					NULL		Failure		     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char *GetDefault(Tcl_Interp *interp, char *path)
{
    struct stat stats;			/** for stat() system call	**/
    int save_flags,			/** save state flag info	**/
	Result = TCL_OK,		/** Tcl command return		**/
	dircount;			/** dir list count		**/
    char *buffer,			/** fullpath buffer		**/
    	 *save_module_path,		/** save module path state	**/
	 *version = (char *) NULL,	/** return string		**/
	**dirlist;			/** dir listing (if needed)	**/
    const char *mod;
    char *ver;                  	/** module,version		**/

    /**
     **  If there's a problem with the input parameters it means, that
     **  we do not have to source anything
     **  Only a valid TCL interpreter should be there
     **/
    if (!path || !interp)
	return (version);
    /**
     **  If path is not a directory don't do anything either
     **/
    if (!stat(path, &stats) && S_ISDIR(stats.st_mode)) {
    /**
     ** save current state flag and set to M_LOAD
     ** do same for module_path
     **/
	    save_flags = g_flags;
	    g_flags = M_LOAD;
	    save_module_path = g_module_path;
	    g_module_path = path;
    /**
     **  Build the full name of the RC file and check whether it exists and
     **  has the magic cookie inside (start with .modulerc - name is longer)
     **/
	if ((char *) NULL ==
	    (buffer = stringer(NULL, 0, path, "/", modulerc_file, NULL)))
	    if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
		goto unwind0;
    /**
     ** check if .modulerc file exists
     **/
	if (!stat(buffer, &stats) && !S_ISDIR(stats.st_mode) &&
	    check_magic(buffer, MODULES_MAGIC_COOKIE,
			MODULES_MAGIC_COOKIE_LENGTH)
	    ) {

	    if (TCL_ERROR != (Result = Execute_TclFile(interp, buffer))) {
		/**
		 **  The version has been specified in the
		 **  '.modulerc' file. Set up the result code
		 **/
		    /* RKO : need to call VersionLookup here */
		if (VersionLookup(path,&mod,&ver)) {
			version = ver;
			goto unwind1;
		}
	    }
	}

    /**
     ** check if .version file exists
     **/
	if (strlen(modulerc_file) < strlen(version_file))
	    if (OK != ErrorLogger(ERR_INTERNAL, LOC, NULL))
		goto unwind1;
	if ((char *) NULL ==
	    (stringer(buffer + strlen(path) + 1, strlen(modulerc_file),
		version_file, NULL)))
	    if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
		goto unwind1;
	if (!stat(buffer, &stats) && !S_ISDIR(stats.st_mode) &&
#if VERSION_MAGIC != 0
	    check_magic(buffer, MODULES_MAGIC_COOKIE,
			MODULES_MAGIC_COOKIE_LENGTH)
#else
	    1
#endif
	    ) {

	    if (TCL_ERROR != (Result = Execute_TclFile(interp, buffer)) &&
		(version = Tcl_GetVar(interp, "ModulesVersion", 0))) {
		/**
		 **  The version has been specified in the
		 **  '.version' file. Set up the result code
		 **/
		goto unwind1;
	    }
	}

	if ((dirlist = SortedDirList(path, ".", &dircount))) {
	/**
	 **  grab the first entry in the dir listing (which is in
	 **  reverse order)
	 **  and return that (all entries have "./" prepended)
	 **/
	    if ((char *) NULL ==
		(version = stringer(NULL, 0, (*dirlist + 2), NULL)))
		if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
		    goto unwindlist;

	    goto unwindlist;
        }
	/**
	 ** free buffer memory
	 **/
	if (0) unwindlist: {
	    FreeList( dirlist, dircount);
	}
	unwind1:
	null_free((void *) &buffer);
	/**
	 ** reset state flags
	 **/
	unwind0:
	g_flags = save_flags;
	g_module_path = save_module_path;

    } /** if( !stat  && !S_ISDIR) **/
    /**
     **  Result determines if this was successfull
     **/
    success0:
    return (version);

} /** End of 'GetDefault' **/

