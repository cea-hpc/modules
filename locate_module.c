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

static char Id[] = "@(#)$Id: locate_module.c,v 1.14.18.2 2011/10/03 20:25:43 rkowen Exp $";
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
    char	*mod, *vers;		/** Module and version name for sym- **/
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
	
    if( modulename[0] == '/' || modulename[0] == '.') {

	p = (char*) strrchr( modulename, '/');
        if(p) {
            *p = '\0';
	    /**
	     **  Check, if what has been specified is a valid version of
	     **  the specified module ...
	     **/
            if((char *) NULL ==
		(result = GetModuleName(interp, modulename, NULL,(p+1)))) 
		goto unwind0;
	    /**
	     **  Reinstall the 'modulefile' which has been corrupted by
	     **   tokenization
	     **/
	    *p = '/';
	    /**
	     **  Reinstall the 'modulefile' which has been corrupted by
	     **  tokenization
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
	/**
	 **  Split up the MODULEPATH values into multiple directories
	 **/
	if( NULL == (pathlist = SplitIntoList(interp, modulespath, &numpaths,
	_colon)))
	    goto unwind0;
	/**
	 **  Check each directory to see if it contains the module
	 **/
	for(i=0; i<numpaths; i++) {
	    /* skip empty paths */
	    if(*pathlist[i] && (NULL != (result =
		GetModuleName( interp, pathlist[i], NULL, modulename)))) {

		if( strlen( pathlist[i]) + 2 + strlen( result) > MOD_BUFSIZE) {
		    if ((char *) NULL == stringer( filename, MOD_BUFSIZE,
		    pathlist[i], NULL))
			goto unwind1;
		} else {
		    if ((char *) NULL == stringer( filename, MOD_BUFSIZE,
		    pathlist[i],"/",result, NULL))
			goto unwind1;
		}
		break;
	    }
	    /**
	     **  If we havn't found it, we should try to re-expand the module
	     **  name, because some rc file have been sourced
	     **/
	    if( VersionLookup( modulename, &mod, &vers)) {
                if ((char *) NULL == stringer( buf, MOD_BUFSIZE,
		mod,"/",vers, NULL))
		    goto unwind1;
		modulename = buf;
	    }
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
    char	 *fullpath = NULL;	/** Buffer for creating path names   **/
    char	 *Result = NULL;	/** Our return value		     **/
    char	**filelist = NULL;	/** Buffer for a list of possible    **/
					/** module files		     **/
    int		  numlist;		/** Size of this list		     **/
    int		  i, slen, is_def;
    char	 *s, *t;		/** Private string buffer	     **/
    char	 *mod, *ver;		/** Pointer to module and version    **/
    char	 *mod1, *ver1;		/** Temp pointer		     **/
    
#if WITH_DEBUGGING_LOCATE_1
    ErrorLogger( NO_ERR_START, LOC, _proc_GetModuleName, NULL);
#endif
    /**
     **  Split the modulename into module and version. Use a private buffer
     **  for this
     **/
    if((char *) NULL == (s = stringer(NULL, 0,  modulename, NULL))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	goto unwind0;
    }
    slen = strlen( s) + 1;
    mod = s;
    if( ver = strrchr( mod, '/'))
	*ver++ = '\0';
    /**
     **  Allocate a buffer for full pathname building
     **/
    if((char *) NULL == (fullpath = stringer(NULL, MOD_BUFSIZE, NULL))) {
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL)) {
	    goto unwind1;
	}
    }
    /**
     **  Check whether $path/$prefix/$modulename is a directory.
     **/
    if( prefix) {
	if((char *) NULL == stringer(fullpath, MOD_BUFSIZE,
	path,"/",prefix,"/",modulename, NULL))
	    goto unwind1;
    } else {
	if((char *) NULL == stringer(fullpath, MOD_BUFSIZE,
	path,"/",modulename, NULL))
	    goto unwind1;
    }
    if( !stat( fullpath, &stats) && S_ISDIR( stats.st_mode)) {
	/**
	 ** So the full modulename is $modulename/default.  Recurse on that.
	 **/
	if((char *) NULL == (t = stringer(NULL, 0, modulename, "/",
					  _default, NULL)))
	    goto unwind1;
	Result = GetModuleName( interp, path, prefix, t);
	null_free((void *) &t);
	null_free((void *) &fullpath);
	null_free((void *) &s);
	return( Result);
    }

    /**
     **  Check whether $path/$prefix/$mod is a directory
     **/
    if( prefix) {
	if((char *) NULL == stringer(fullpath, MOD_BUFSIZE,
	path,"/",prefix,"/",mod, NULL))
	    goto unwind1;
    } else {
	if((char *) NULL == stringer(fullpath, MOD_BUFSIZE,
	path,"/",mod, NULL))
	    goto unwind1;
    }
    is_def = !strcmp( mod, _default);

    if( is_def || !stat( fullpath, &stats)) {
	/**
	 **  If it is a directory
	 **/
    	if( !is_def && S_ISDIR( stats.st_mode)) {
	    /**
	     **  Source the ".modulerc" file if it exists
	     **  For compatibility source the .version file, too
	     **/
	    if( prefix) {
		if((char *) NULL == stringer(modfil_buf, MOD_BUFSIZE,
		prefix,"/",mod, NULL))
		    goto unwind2;
	    } else {
		if((char *) NULL == stringer(modfil_buf, MOD_BUFSIZE,mod, NULL))
		    goto unwind2;
	    }

	    if((char *) NULL == stringer(fullpath, MOD_BUFSIZE,
	    path,"/",modfil_buf, NULL))
		goto unwind2;
	    g_current_module = modfil_buf;

	    if(	TCL_ERROR == SourceRC( interp, fullpath, modulerc_file) ||
		TCL_ERROR == SourceVers( interp, fullpath, modfil_buf)) {
		/* flags = save_flags; */
		    goto unwind2;
	    }
	    /**
	     **  After sourcing the RC files, we have to look up versions again
	     **/
	    if( VersionLookup( modulename, &mod1, &ver1)) {
		int len = strlen( mod1) + strlen( ver1) + 2;
		/**
		 **  Maybe we have to enlarge s
		 **/
		if( len > slen) {
		    null_free((void *) &s);
		    if((char *) NULL == (s = stringer( NULL, len, NULL))) {
			ErrorLogger( ERR_STRING, LOC, NULL);
			goto unwind2;
		    }
		    slen = len;
		}
		/**
		 **  Print the new module/version in the buffer
		 **/
		if((char *) NULL == stringer( s, len, mod1,"/", ver1, NULL)) {
		    ErrorLogger( ERR_STRING, LOC, NULL);
		    goto unwind2;
		}
		mod = s;
		if( ver = strchr( s, '/'))
		    *ver++ = '\0';
	    }
	    /**
	     **  recursively delve into subdirectories (until ver == NULL).
	     **/
	    if( ver) {
		int len = strlen( mod) + 1;

		if( prefix)
		    len += strlen( prefix) +1;
		/**
		 **  Build the new prefix
		 **/
		if((char *) NULL == (t = stringer(NULL, len, NULL))) {
		    ErrorLogger( ERR_STRING, LOC, NULL);
		    goto unwind2;
		}

		if( prefix) {
		    if((char *) NULL == stringer(t, len, prefix,"/",mod, NULL)){
			ErrorLogger( ERR_STRING, LOC, NULL);
			goto unwindt;
		    }
		} else {
		    if((char *) NULL == stringer(t, len, mod, NULL)){
			ErrorLogger( ERR_STRING, LOC, NULL);
			goto unwindt;
		    }
		}
		/**
		 **  This is the recursion
		 **/
		Result = GetModuleName( interp, path, t, ver);

		/**
		 **  Free our temporary prefix buffer
		 **/
		null_free((void *) &t);
		if (0) {	/* an error occurred */
unwindt:
		    null_free((void *) &t);
		    goto unwind2;
		}
	    } 
	} else {     /** if( $path/$prefix/$mod is a directory) **/
	    /**
	     **  Now 'mod' should be either a file or the word 'default'
	     **  In case of default get the file with the highest version number
	     **  in the current directory
	     **/
	    if( is_def) {
		if( !prefix)
		    prefix = ".";
		if( NULL == (filelist = SortedDirList( interp, path, prefix,
		&numlist)))
		    goto unwind1;

		prefix = (char *) NULL;
		/**
		 **  Select the first one on the list which is either a
		 **  modulefile or another directory. We start at the highest
		 **  lexicographical name in the directory since the filelist
		 **  is reverse sorted.
		 **  If it's a directory, we delve into it.
		 **/
		for( i=0; i<numlist && Result==NULL; i++) {
		    /**
		     **  Build the full path name and check if it is a
		     **  directory. If it is, recursively try to find there what
		     **  we're seeking for
		     **/
		    if ((char *)NULL == stringer(fullpath, MOD_BUFSIZE,
			path, "/", filelist[i], NULL))
			    goto unwind2;

		    if( !stat( fullpath, &stats) && S_ISDIR( stats.st_mode)) {
			Result = GetModuleName( interp, path, prefix,
			    filelist[ i]);
		    } else {
			/**
			 **  Otherwise check the file for a magic cookie ...
			 **/
			if( check_magic( fullpath, MODULES_MAGIC_COOKIE, 
			    MODULES_MAGIC_COOKIE_LENGTH)) 
			    Result = filelist[ i];
		    } /** if( !stat) **/
		} /** for **/
	    } else {  /** default **/
		/**
		 **  If mod names a file, we have to check wheter it exists and
		 **  is a valid module file
		 **/
		if( check_magic( fullpath, MODULES_MAGIC_COOKIE, 
		    MODULES_MAGIC_COOKIE_LENGTH)) 
		    Result = mod;
		else {
		    ErrorLogger( ERR_MAGIC, LOC, fullpath, NULL);
		    Result = NULL;
		}
	    } /** if( mod is a filename) **/
	    /**
	     **  Build the full filename (using prefix and Result) if
	     **  Result is defined
	     **/
	    if( Result) {
		int len = strlen( Result) + 1;

		if( prefix)
		    len += strlen( prefix) + 1;

		if((char *) NULL == (t = stringer(NULL, len, NULL))) {
		   ErrorLogger( ERR_STRING, LOC, NULL);
		   goto unwind2;
		}
		if( prefix) {
		    if((char *) NULL == stringer(t,len, prefix,"/",Result,NULL))
			goto unwindt2;
		} else {
		    if((char *) NULL == stringer(t,len, Result,NULL))
			goto unwindt2;
		}
		Result = t;
		if (0) {	/* an error occurred */
unwindt2:
		    null_free((void *) &t);
		    goto unwind2;
		}
	    } 
	} /** mod is a file **/
    } /** mod exists **/
    /**
     **  Free up temporary values and return what we've found
     **/
    null_free((void*) &fullpath);
    null_free((void*) &s);
    FreeList( filelist, numlist);
    
#if WITH_DEBUGGING_LOCATE_1
    ErrorLogger( NO_ERR_END, LOC, _proc_GetModuleName, NULL);
#endif
    return( Result);			/** -------- EXIT (SUCCESS) -------> **/

unwind2:
    null_free((void *) &fullpath);
unwind1:
    null_free((void *) &s);
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
 **   Parameters:	Tcl_Interp	*interp		According Tcl Interp.**
 **			char		*path		Path to start seeking**
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

char	**SortedDirList(	Tcl_Interp	*interp,
		     		char		*path,
		     		char		*modulename,
		     		int		*listcnt)
{
    struct dirent	*file;		/** Directory entry		     **/
    struct stat    	 stats;		/** Stat buffer			     **/
    DIR			*subdirp;	/** Subdirectoy handle		     **/
    char		*full_path;	/** Sugg. full path (path + module)  **/
    char		**filelist;	/** Temp. base pointer of the list   **/
    int			 i,		/** Number of entries in the subdir  **/
			 j,		/** Counts the number of list-entries**/
			 n,		/** Size of the allocated array	     **/
			 pathlen;	/** String length of 'fullpath'	     **/
 
#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_SortedDirList, NULL);
#endif
    /**
     **  Allocate memory for the list to be created. Suggest a list size of
     **  100 Elements. This may be changed later on.
     **/
    if(!(filelist = (char**) module_malloc((n = 100)*sizeof(char*))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
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
	filelist[0] = strdup( modulename);

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_DEBUG, LOC, "Module '", modulename, "' found", NULL);
#endif

	null_free((void*) &full_path);
	return( filelist);		/** --- EXIT PROCEDURE (SUCCESS) --> **/
    }
    /**
     **  What we've found is a directory
     **/
    if( S_ISDIR( stats.st_mode)) {
        char	*tbuf;		/** Buffer for the whole filename for each   **/
				/** content of the directory		     **/
        char	*mpath;		/** Pointer into *tbuf where to write the dir**/
				/** entry				     **/

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

        mpath = (tbuf + pathlen + 1);
	/**
	 **  Now scan all entries of the just opened directory
	 **/
#if WITH_DEBUGGING_UTIL_2
	ErrorLogger( NO_ERR_DEBUG, LOC, "Reading directory '", full_path, "'", NULL);
#endif
	for( file = readdir( subdirp), i = 0, j = 0;
	     file != NULL;
	     file = readdir( subdirp), i++) {
	    /**
	     **  Oops! This one exceeds our array. Enlarge it.
	     **/
	    if( j == n)
		if(!(filelist = (char**)
			module_realloc((char*) filelist, (n*=2)*sizeof(char*))))
		    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
			goto unwindt;
	    /**
	     **  Now, if we got a real entry which is not '.*' or '..' and
	     **  finally is not a temporary file (which are defined to end
	     **  on '~' ...
	     **/
	    if( file->d_name                && 
                *file->d_name != '.'        && 
                strcmp( file->d_name, "..")  &&
                file->d_name[ NLENGTH( file) - 1] != '~') {
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
                    if((char *) NULL == (filelist[j] = stringer(NULL,0,
			modulename,"/",file->d_name, NULL)))
			if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
			    goto unwindt;
		    j++;
                } /** if( mag. cookie or directory) **/
	    } /** if( not a dotfile) **/
	} /** for **/
	/**
	 **  Put a terminator at the lists end and then sort the list
	 **/
        filelist[ j] = NULL;
	qsort( (void*) filelist, (size_t) j, sizeof(char*), filename_compare);
	/**
	 **  Free up temporary values ...
	 **/
	if( -1 == closedir( subdirp))
	    if( OK != ErrorLogger( ERR_CLOSEDIR, LOC, full_path, NULL)) {
		goto unwind2;
	    }
	null_free((void*) &full_path);
	null_free((void*) &tbuf);
	/**
	 **  Set up return values and pass the created list to the caller
	 **/
	*listcnt = j;
	return( filelist);		/** --- EXIT PROCEDURE (SUCCESS) --> **/
	
	if(0) {	
unwindt:
	    null_free((void*) &tbuf);
	    goto unwind3;
	}
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
    FreeList( filelist, n);

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

char	**SplitIntoList(	Tcl_Interp	*interp,
		     		char		*pathenv, 
		     		int		*numpaths,
				const char	*delim) 
{
    char	**pathlist = NULL;	/** Temporary base pointer for the   **/
					/** array to be created		     **/
    char	 *givenpath = NULL;	/** Temporary buffer used to tokenize**/
					/** the passed input path	     **/
    char	 *dirname = NULL;	/** Token pointer		     **/
    int     	  i, 			/** Counts the number of elements    **/
		  n;			/** Size of the array		     **/

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_SplitIntoList, NULL);
#endif
    /** 
     **  Paramter check
     **/
    if( !pathenv)
	if( OK != ErrorLogger( ERR_PARAM, LOC, "pathenv", NULL))
	    goto unwind0;
    /**
     **  Allocate space to copy in the value of the path value to
     **  split. Thus this procedure doesn't change its input parameters.
     **/
    if( (char *) NULL == (givenpath = stringer(NULL,0, pathenv,NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;
    /**
     **  Allocate the list which is an array of char*.  n is used to
     **  manage the array's growth if there are more than 100 paths in
     **  the list.
     **  Copy the passed path into the new buffer.
     **/
    if(!(pathlist = (char**) module_malloc((n = 100)*sizeof( char*))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind1;
    /**
     **  Split the given path environment variable into its components.
     **/
    for( i=0, dirname = xstrtok( givenpath, delim);
         dirname;
	 dirname = xstrtok( NULL, delim)) {
	/**
	 **  Oops! The number of tokens exceed my array - reallocate it
	 **  and double its size!
	 **/
	if( i == n )
	    if(!(pathlist = (char**) module_realloc((char*) pathlist,
		(n *= 2)*sizeof(char*))))
		if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		    goto unwind1;
	/**
	 **  Put the token into the array. Therefor a new area is allocated for
	 **  the token using 'xdup' - which expands 1 level of env.vars.
	 **/
	if( NULL == (pathlist[ i++] = xdup( dirname)))
	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL)) {
		FreeList( pathlist, --i);
		goto unwind1;
	    }
    } /** for **/
    /**
     **  Free up the temporary working array
     **/
    if( givenpath)
	null_free((void*) &givenpath);
    /**
     **  Set up the return value (Number of elements allocated) and pass
     **  the arrays base pointer to the caller
     **/
    *numpaths = i;
    return( pathlist);

unwind1:
    null_free((void *) &givenpath);
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
    static char	**srclist = (char **) NULL;
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
    /**
     **  Check whether the RC file exists and has the magic cookie inside
     **/
    if( !stat( buffer, &stats)) {
	if( check_magic( buffer, MODULES_MAGIC_COOKIE, 
	    MODULES_MAGIC_COOKIE_LENGTH)) {
	    /**
	     **  Set the flags to 'load only'. This prevents from accidently
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
	    if( !listsize) {
		listsize = SRCFRAG;
		if((char **) NULL
			== (srclist = (char **) module_malloc( listsize *
		    sizeof( char **)))) {
		    ErrorLogger( ERR_ALLOC, LOC, NULL);
		    goto unwind1;
		}
	    } else if( listndx + 1 >= listsize) {
		listsize += SRCFRAG;
		if(!(srclist = (char **) module_realloc( srclist, 
		    listsize * sizeof( char **)))) {
		    ErrorLogger( ERR_ALLOC, LOC, NULL);
		    goto unwind1;
		}
	    }
	    /**
	     **  Put the current RC files name on the list
	     **/
	    srclist[ listndx++] = buffer;

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
    char *modname;			/** ptr to module part of name	     **/
    int Result = TCL_OK;
    char *version;
    char *new_argv[3];
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
		(version = (char *) Tcl_GetVar(interp, "ModulesVersion", 0))) {
		/**
		 **  The version has been specified in the
		 **  '.version' file. Set up the result code
		 **/
		/* for deep modulefile dirs ... just use lowest part */
		if (!(modname = (char*) strrchr( name, '/'))) {
			modname = name;
		} else {
			modname++;
		}
		null_free((void *) &buffer);
		if ((char *) NULL == (buffer = stringer(NULL, 0,
		modname,"/",version, NULL)))
		    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
			return( TCL_ERROR);

		new_argv[0] = "module-version";
		new_argv[1] = buffer;
		new_argv[2] = _default;
		/**
		 **  Define the default version
		 **/
		if( TCL_OK != cmdModuleVersion( (ClientData) 0,
		    (Tcl_Interp *) NULL, 3, (CONST84 char **) new_argv)) {
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
     **  Result determines if this was successful
     **/

    return( Result);

} /** End of 'SourceVers' **/

