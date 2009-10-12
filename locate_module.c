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
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description: 	Contains the routines which locate the actual	     **
 **			modulefile given a modulefilename by looking in all  **
 **			of the paths in MODULEPATH. 			     **
 ** 									     **
 **   Exports:		Locate_ModuleFile				     **
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

static char Id[] = "@(#)$Id: locate_module.c,v 1.30 2009/10/12 19:41:22 rkowen Exp $";
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

static	char	module_name[] = __FILE__;
static	char	strbuffer[ MOD_BUFSIZE];
static	char	modfil_buf[ MOD_BUFSIZE];

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	char	 *GetModuleName( Tcl_Interp*, char*, char*, char*);

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

int Locate_ModuleFile(
	Tcl_Interp * interp,
	char *modulename,
	char *realname,
	char *filename
) {
	char           *p;		/** Tokenization pointer	     **/
	char           *result = NULL;	/** This functions result	     **/
	char          **pathlist;	/** List of paths to scan	     **/
	int             numpaths,	/** Size of this list		     **/
	                i;		/** Loop counter		     **/
	char           *mod, *vers;	/** Module and version name for sym- **/
					/** bolic name lookup		     **/
    /**
     **  If it is a full path name, that's the module file to load.
     **/
	if (!modulename)
		if (OK != ErrorLogger(ERR_PARAM, LOC, "modulename", NULL))
			goto unwind0;

	if (modulename[0] == '/' || modulename[0] == '.') {
		p = (char *)strrchr(modulename, '/');
		if (p) {
			*p = '\0';
	    /**
	     **  Check, if what has been specified is a valid version of
	     **  the specified module ...
	     **/
			if ((char *)NULL == (result =
			     GetModuleName(interp, modulename, NULL, (p + 1))))
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
			if (!strcmp((p + 1), result)) {
				if ((char *)NULL ==
				    stringer(filename, MOD_BUFSIZE, modulename,
					     NULL))
					goto unwind1;
			} else {
				if ((char *)NULL ==
				    stringer(filename, MOD_BUFSIZE, modulename,
					     "/", result, NULL))
					goto unwind1;
			}
		} else {
	    /**
	     **  Hmm! There's no backslash in 'modulename'. So it MUST begin
	     **  on '.' and MUST be part of the current directory
	     **/
			if (NULL ==
			    (result =
			     GetModuleName(interp, modulename, NULL,
					   modulename)))
				goto unwind0;

			if (!strcmp(modulename, result) ||
			    (strlen(modulename) + 1 + strlen(result) + 1 >
			     MOD_BUFSIZE)) {
				if ((char *)NULL ==
				    stringer(filename, MOD_BUFSIZE, modulename,
					     NULL))
					goto unwind1;
			} else {
				if ((char *)NULL ==
				    stringer(filename, MOD_BUFSIZE, modulename,
					     "/", result, NULL))
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
		if (!ModulePathVec || !uvec_number(ModulePathVec)) {
			if (OK != ErrorLogger(ERR_MODULE_PATH, LOC, NULL)) {
				g_current_module = NULL;
				goto unwind0;
			}
		}
	/**
	 **  Expand the module name (in case it is a symbolic one). This must
	 **  be done once here in order to expand any aliases
	 **/
		if (VersionLookup(modulename, &mod, &vers)) {
			if ((char *)NULL == stringer(strbuffer, MOD_BUFSIZE,
						     mod, "/", vers, NULL))
				goto unwind0;
			modulename = strbuffer;
		}
	/**
	 **  Split up the MODULEPATH values into multiple directories
	 **/
		numpaths = uvec_number(ModulePathVec);
		pathlist = ModulePath;
	/**
	 **  Check each directory to see if it contains the module
	 **/
		for (i = 0; i < numpaths; i++) {
			/* skip empty paths */
			if (*pathlist[i] && (NULL != (result =
				GetModuleName(interp, pathlist[i],
				    NULL, modulename)))) {

				if (strlen(pathlist[i]) + 2 + strlen(result) >
				    MOD_BUFSIZE) {
					if ((char *)NULL ==
					    stringer(filename, MOD_BUFSIZE,
						     pathlist[i], NULL))
						goto unwind1;
				} else {
					if ((char *)NULL ==
					    stringer(filename, MOD_BUFSIZE,
						     pathlist[i], "/", result,
						     NULL))
						goto unwind1;
				}
				break;
			}
	    /**
	     **  If we havn't found it, we should try to re-expand the module
	     **  name, because some rc file have been sourced
	     **/
			if (VersionLookup(modulename, &mod, &vers)) {
				if ((char *)NULL ==
				    stringer(strbuffer, MOD_BUFSIZE, mod, "/",
					     vers, NULL))
					goto unwind1;
				modulename = strbuffer;
			}
		}
	  /** for **/
	/**
	 **  If result still NULL, then we really never found it and we should
	 **  return ERROR and clear the full_path array for cleanliness.
	 **/
		if (!result) {
			filename[0] = '\0';
			goto unwind0;
		}
	}
      /** not a full path name **/
    /**
     **  Free up what has been allocated and pass the result back to
     **  the caller and save the real module file name returned by
     **  GetModuleName
     **/
	strncpy(realname, result, MOD_BUFSIZE);
	if ((char *)NULL == stringer(realname, MOD_BUFSIZE, result, NULL))
		goto unwind1;
	null_free((void *)&result);

	return (TCL_OK);

unwind1:
	null_free((void *)&result);
unwind0:
	return (TCL_ERROR);
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
    uvec	 *filelist = NULL;	/** Buffer for a list of possible    **/
					/** module files		     **/
    int		  numlist;		/** Size of this list		     **/
    int		  i, slen, is_def;
    char	 *s, *t;		/** Private string buffer	     **/
    char	 *mod, *ver;		/** Pointer to module and version    **/
    char	 *mod1, *ver1;		/** Temp pointer		     **/
    
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
    if( (ver = strchr( mod, '/')) )
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
    is_def = !strcmp( mod, _(em_default));

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
		path,"/",mod, NULL))
		    goto unwind2;
	    } else {
		if((char *) NULL == stringer(modfil_buf, MOD_BUFSIZE,mod, NULL))
		    goto unwind2;
	    }

	    if((char *) NULL == stringer(fullpath, MOD_BUFSIZE,
	    path,"/",modfil_buf, NULL))
		goto unwind2;
	    g_current_module = modfil_buf;

	    if( TCL_ERROR == SourceRC( interp, fullpath, modulerc_file) ||
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
		if( (ver = strchr( s, '/')) )
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
		if( NULL == (filelist = SortedDirList( path, prefix,&numlist)))
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
	char	 *filename;
		    /**
		     **  Build the full path name and check if it is a
		     **  directory. If it is, recursively try to find there what
		     **  we're seeking for
		     **/
			filename = uvec_vector(filelist)[i];
		    if ((char *)NULL == stringer(fullpath, MOD_BUFSIZE,
			path, "/", filename, NULL))
			    goto unwind2;

		    if( !stat( fullpath, &stats) && S_ISDIR( stats.st_mode)) {
			Result = GetModuleName( interp, path, prefix, filename);
		    } else {
			/**
			 **  Otherwise check the file for a magic cookie ...
			 **/
			if( check_magic( fullpath, MODULES_MAGIC_COOKIE, 
			    MODULES_MAGIC_COOKIE_LENGTH)) 
			    Result = filename;
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
    FreeList( &filelist);
    
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
    for( i=0; i<listndx; i++)
	if( !strcmp( srclist[ i], buffer))
	    goto unwind1;
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
		if((char **) NULL == (srclist = (char **) module_realloc(
		    srclist, listsize * sizeof( char **)))) {
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
    } else {	/** if( !stat) - presumably not found **/
	null_free((void *) &buffer);
    }
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
     **  No default version defined so far?
     **/
    if( VersionLookup( name, &mod, &ver) &&
	strcmp( ver, _(em_default)))
	return( TCL_OK);
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
		int	objc = 3;
		Tcl_Obj **objv;
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
		new_argv[2] = _(em_default);
		Tcl_ArgvToObjv(&objc, &objv, 3, new_argv);
		/**
		 **  Define the default version
		 **/
		if( TCL_OK != cmdModuleVersion( (ClientData) 0,
		    (Tcl_Interp *) NULL, 3, objv)) {
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

