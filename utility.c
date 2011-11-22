/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		utility.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:	General routines that are called throughout Modules  **
 **			which are not necessarily specific to any single     **
 **			block of functionality.				     **
 ** 									     **
 **   Exports:		SortedDirList					     **
 **			SplitIntoList					     **
 **			FreeList					     **
 **			ModulePathList					     **
 **			Global_Hash_Tables				     **
 **			Unwind_Modulefile_Changes			     **
 **			Output_Modulefile_Changes			     **
 **			IsLoaded_ExactMatch				     **
 **			IsLoaded					     **
 **			chk_marked_entry				     **
 **			set_marked_entry				     **
 **			Update_LoadedList				     **
 **			check_magic					     **
 **			regex_quote					     **
 **			xstrtok						     **
 **			xstrtok_r					     **
 **			chk4spch					     **
 **			xdup						     **
 **			xgetenv						     **
 **			countTclHash					     **
 **			ReturnValue					     **
 **			OutputExit					     **
 **			is_						     **
 **									     **
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

static char Id[] = "@(#)$Id: utility.c,v 1.45 2011/11/22 17:41:02 rkowen Exp $";
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

static	char	module_name[] = __FILE__;

static	FILE *aliasfile;		/** Temporary file to write aliases  **/
static	char *aliasfilename;		/** Temporary file name		     **/
static	char  alias_separator = ';';	/** Alias command separator	     **/
static	const int   eval_alias = 	/** EVAL_ALIAS macro		     **/
#ifdef EVAL_ALIAS
	1
#else
	0
#endif
;
static	const int   bourne_funcs = 	/** HAS_BOURNE_FUNCS macro	     **/
#ifdef HAS_BOURNE_FUNCS
	1
#else
	0
#endif
;
static	const int   bourne_alias = 	/** HAS_BOURNE_FUNCS macro	     **/
#ifdef HAS_BOURNE_ALIAS
	1
#else
	0
#endif
;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	int	 Output_Modulefile_Aliases( void);
static	int	 Output_Directory_Change( void);
static	int	 output_set_variable( Tcl_Interp *, const char*, const char*);
static	int	 output_unset_variable( const char* var);
static	void	 output_function( const char*, const char*);
static	int	 output_set_alias( const char*, const char*);
static	int	 output_unset_alias( const char*, const char*);
static	int	 __IsLoaded( Tcl_Interp*, char*, char**, char*, int);
static	char	*get_module_basename( char*);
static	char	*chop( const char*);
static  void     EscapeCshString(const char* in,
				 char* out);
static  void     EscapeShString(const char* in,
				 char* out);
static  void     EscapePerlString(const char* in,
				 char* out);
static  void     EscapeCmakeString(const char* in,
				 char* out);



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

static int filename_compare(
	const void *fi1,
	const void *fi2
) {
	return strcmp(*(char **)fi2, *(char **)fi1);
}

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
 **   Result:		uvec*		NULL	Any failure (alloc, param)   **
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

uvec           *SortedDirList(
	char *path,
	char *modulename,
	int *listcnt
) {
	struct dirent  *file;		/** Directory entry		     **/
	DIR            *subdirp;	/** Subdirectoy handle		     **/
	char           *full_path;	/** Sugg. full path (path + module)  **/
	uvec           *filelist;	/** Temp. uvec list		     **/
	int		pathlen;	/** String length of 'fullpath'	     **/
	is_Result	fstate;		/** file stat			     **/

    /**
     **  Allocate memory for the list to be created. Suggest a list size of
     **  20 Elements. This may be changed later on.
     **/
	if (!(filelist = uvec_ctor(20)))
		if (OK != ErrorLogger(ERR_UVEC, LOC, NULL))
			goto unwind0;
    /**
     **  Form the suggested module file name out of the passed path and 
     **  the name of the module. Alloc memory in order to do this.
     **/
	if (!(full_path = stringer(NULL, 0,
				  path, psep, modulename, NULL)))
		if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
			goto unwind1;
	pathlen = strlen(full_path);
    /**
     **  Check whether this file exists. If it doesn't free up everything
     **  and return on failure
     **/
	if (!(fstate = is_("what",full_path)))
		goto unwind2;
    /**
     **  If the suggested module file is a regular one, we've found what we've
     **  seeked for. Put it on the top of the list and return.
     **/
	if (fstate == IS_FILE) {
		if (uvec_add(filelist, modulename))
			if (OK != ErrorLogger(ERR_UVEC, LOC, NULL))
				goto unwind2;
		*listcnt = 1;
		return (filelist);	/** --- EXIT PROCEDURE (SUCCESS) --> **/
	}
    /**
     **  What we've found is a directory
     **/
	if (fstate == IS_DIR) {
		char           *tbuf;
				/** Buffer for the whole filename for each   **/
				/** content of the directory		     **/
		char           *mpath;
				/** Pointer into *tbuf where to write the dir**/
				/** entry				     **/
	/**
	 **  Open the directory for reading
	 **/
		if (!(subdirp = opendir(full_path))) {
#if 0
	/* if you can't open the directory ... is that really an error? */
			if (OK !=
			    ErrorLogger(ERR_OPENDIR, LOC, full_path, NULL))
#endif
				goto unwind2;
		}
	/**
	 **  Allocate a buffer for constructing complete file names
	 **  and initialize it with the directory part we do already know.
	 **/
		if (!(tbuf = stringer(NULL,MOD_BUFSIZE, full_path, psep, NULL)))
			if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
				goto unwind3;

		mpath = (tbuf + pathlen + 1);
	/**
	 **  Now scan all entries of the just opened directory
	 **/
		for (file = readdir(subdirp);
		     file != NULL;
		     file = readdir(subdirp)) {
	    /**
	     **  Now, if we got a real entry which is not '.*' or '..' and
	     **  finally is not a temporary file (which are defined to end
	     **  on '~' ...
	     **/
			if (file->d_name && *file->d_name != '.' &&
			    strcmp(file->d_name, "..") &&
			    file->d_name[NLENGTH(file) - 1] != '~') {
		/**
		 **  ... build the full pathname and check for the magic
		 **  cookie or for another directory level in order to
		 **  validate this as a modulefile entry we're seeking for.
		 **/
				strcpy(mpath, file->d_name);
				if (check_magic(tbuf, MODULES_MAGIC_COOKIE,
						MODULES_MAGIC_COOKIE_LENGTH) ||
						is_("what",tbuf) ) {
		    /**
		     **  Yep! Found! Put it on the list
		     **/
					if (uvec_add(filelist, stringer(NULL, 0,
					 modulename, psep, file->d_name, NULL)))
						if (OK !=
						    ErrorLogger(ERR_UVEC, LOC,
								NULL))
							goto unwindt;
				} /** if( mag. cookie or directory) **/
			} /** if( not a dotfile) **/
		} /** for **/
	/**
	 **  Put a terminator at the lists end and then sort the list
	 **/
		if (uvec_qsort(filelist, filename_compare))
			if (OK != ErrorLogger(ERR_UVEC, LOC, NULL))
				goto unwindt;
	/**
	 **  Free up temporary values ...
	 **/
		if (closedir(subdirp) < 0)
			if (OK !=
			    ErrorLogger(ERR_CLOSEDIR, LOC, full_path, NULL)) {
				goto unwindt;
			}
		null_free((void *)&full_path);
		null_free((void *)&tbuf);
	/**
	 **  Set up return values and pass the created list to the caller
	 **/
		*listcnt = uvec_number(filelist);
		return (filelist);	/** --- EXIT PROCEDURE (SUCCESS) --> **/

		if (0) {
unwindt:
			null_free((void *)&tbuf);
			goto unwind3;
		}
	}
    /**
     **  If it is not a regular file, neither a directory, we don't support
     **  it at all ...
     **/
unwind3:
	if (closedir(subdirp) < 0)
		ErrorLogger(ERR_CLOSEDIR, LOC, full_path, NULL);
unwind2:
	null_free((void *)&full_path);
unwind1:
	FreeList(&filelist);
unwind0:
	*listcnt = -1;
	return (NULL);			/** --- EXIT PROCEDURE (FAILURE) --> **/

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
 **   Parameters:	char		*pathenv	Path to split 	     **
 **			int		*numpaths	Buffer to write the  **
 **							number of array ele- **
 **							ments to.	     **
 **			char		*delim		Path delimiter	     **
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

uvec *SplitIntoList(
	const char *pathenv,
	int *numpaths,
	const char *delim
) {
	uvec		*pathlist = NULL;	/** Temporary uvec	     **/
	char		*givenpath = NULL,	/** modifiable buffer	     **/
			*dirname = NULL,	/** Token ptr		     **/
			*tpath = NULL;		/** temp xdup string	     **/
    /** 
     **  Parameter check
     **/
	if (!pathenv)
		if (OK != ErrorLogger(ERR_PARAM, LOC, "pathenv", NULL))
			goto unwind0;
    /** 
     **  Need a copy of pathenv for tokenization
     **/
	if (!(givenpath = stringer(NULL,0,pathenv,NULL)))
		if (OK != ErrorLogger(ERR_PARAM, LOC, "pathenv", NULL))
			goto unwind0;
    /**
     **  Split the path, first allocate a new vector
     **/
	if (!(pathlist = uvec_ctor(uvec_count_tok(delim,pathenv)+1))) {
		if( OK != ErrorLogger( ERR_UVEC, LOC, NULL))
			goto unwind1;
	}
    /**
     **  Split the given path environment variable into its components
     **	 May need to expand internal variables
     **/
	for (dirname = xstrtok(givenpath, delim);
	     dirname;
	     dirname = xstrtok( NULL, delim)) {
		tpath = xdup(dirname);
		/* chop off any ending \n's */
		if (tpath[strlen(tpath)-1] == '\n')
			tpath[strlen(tpath) - 1] = '\0';
		if(uvec_add(pathlist,tpath)) {
			if( OK != ErrorLogger( ERR_UVEC, LOC, NULL))
				goto unwind2;
		}
		null_free((void *) &tpath);
	}
    /**
     **  Free up the temporary string buffer
     **/
	if (givenpath)
		null_free((void *) &givenpath);
    /**
     **  Set up the return value (Number of elements allocated) and pass
     **  the uvec to the caller
     **/
	*numpaths = uvec_number(pathlist);
	return (pathlist);

unwind2:
	uvec_dtor(&pathlist);
unwind1:
	null_free((void *) &givenpath);
unwind0:
	return (NULL);			/** -------- EXIT FAILURE -------> **/
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

void FreeList(	uvec	**list)
{
	uvec_dtor(list);

} /** End of 'FreeList' **/

#endif
/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModulePathList					     **
 ** 									     **
 **   Description: 	Splits a MODULEPATH environment variable into	     **
 **			of vector list.					     **
 **			Memory must be released manually with FreeList().    **
 ** 									     **
 **   First Edition:	2009/08/28					     **
 ** 									     **
 **   Parameters:	(none)						     **
 ** 									     **
 **   Result:		uvec*		NULL	If env.var. does not exist   **
 **						or any failure		     **
 **					else	Base pointer of the created  **
 **						vector			     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
uvec *ModulePathList(
	void
) {
	char	*modulepath;			/** modulepath env.var.	     **/
	uvec	*modulevec;			/** modulepath vector	     **/
	int	numpaths;			/** number of paths	     **/
    /**
     **  Load the MODULEPATH and split it into a list of paths
     **/
	if (!(modulepath = xgetenv("MODULEPATH"))) {
		/* ErrorLogger(ERR_MODULE_PATH, LOC, NULL); */
		return NULL;
	}
	modulevec = SplitIntoList(modulepath, &numpaths, _colon);
	null_free((void *) &modulepath);
	return modulevec;
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Global_Hash_Tables				     **
 ** 									     **
 **   Description: 	Process the global hash tables in one fell swoop     **
 ** 									     **
 **   First Edition:	2009/09/01					     **
 ** 									     **
 **   Parameters:	GlobalHashAction	action			     **
 **   				 Clear		reinitialize hashes	     **
 **				 Delete		destroy them all	     **
 **				 Copy		return a copy of each	     **
 **			MHash **		globalhashtables	     **
 ** 									     **
 **   Result:		MHash **		newtables (copy only)	     **
 ** 									     **
 **   Attached Globals:	GlobalHashTables	(if null input)		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

MHash **Global_Hash_Tables(
	GHashAction	action,
	MHash		**globalhashtables
) {
	MHash	**newtable,
		**t_ptr = globalhashtables,
		**newt_ptr,
		*tmp;

    /**
     **  Loop for all the global hash tables. If there's no value stored
     **  in a hash table, skip to the next one. 
     **/
	if (!globalhashtables)
		t_ptr = GlobalHashTables;

	if (action == GHashCopy) {
		if (!(newt_ptr = newtable =
		(MHash **) module_malloc(5 * sizeof(MHash *))))
			if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
				return (MHash **) NULL;
	}
	while (t_ptr && *t_ptr) {
		switch (action) {
		case GHashClear:
			tmp = mhash_ctor(mhash_type(*t_ptr));
			mhash_dtor(t_ptr);
			*t_ptr = tmp;
			break;
		case GHashDelete:
			mhash_dtor(t_ptr);
			break;
		case GHashCopy:
			*newt_ptr++ = mhash_copy(*t_ptr);
			break;
		default:
			return (MHash **) NULL;
		}
		t_ptr++;
	}
	if (action == GHashCopy) {
		*newt_ptr = (MHash *) NULL;
		return newtable;
	} else
		return globalhashtables;

} /** End of 'Global_Hash_Tables' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:								     **
 ** 									     **
 **   Description:	Once the loading or unloading of a modulefile	     **
 **			fails, any changes it has made to the environment    **
 **			must be undone and reset to its previous state. This **
 **			function is responsible for unwinding any changes a  **
 **			modulefile has made.				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	 *interp	According TCL interp.**
 **			MHash		**oldTables	Hash tables storing  **
 **							the former environm. **
 **   Result:								     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Unwind_Modulefile_Changes(
	Tcl_Interp * interp,
	MHash ** oldTables
) {
	char           *val = NULL,	/** Stored value (is a pointer!)     **/
	    **keys;			/** Tcl hash key		     **/
	int             i;		/** Loop counter		     **/

	if (oldTables) {
	/**
	 **  Entries 0 and 1 which contain all changes to the 
	 **  shell variables (setenv and unsetenv)
	 **  Entries 2 and 3 which contain the aliase/unalias setting
	 **/
		for (i = 0; i < 4; i++) {
	    /**
	     **  The hash entry will contain the appropriate value for the
	     **  specified 'key' because it will have been acquired depending
	     **  upon whether the unset or set table was used.
	     **/
			if (!mhash_exists(oldTables[i]))
				continue;

			keys = mhash_keys(oldTables[i]);
			while (keys && *keys) {
				val = mhash_value(oldTables[i], *keys);
				if (val)
					(void) EMSetEnv(interp, *keys, val);
				keys++;
			}
		} /** for **/
	/**
	 **  Delete and reset the hash tables now the current contents have been
	 **  flushed.
	 **/
		Global_Hash_Tables(GHashDelete, NULL);

		setenvHashTable = oldTables[0];
		unsetenvHashTable = oldTables[1];
		aliasSetHashTable = oldTables[2];
		aliasUnsetHashTable = oldTables[3];
	} else {
		Global_Hash_Tables(GHashClear, NULL);
	}

	return (TCL_OK);

} /** End of 'Unwind_Modulefile_Changes' **/

static int keycmp(const void *a, const void *b) {
	return strcmp(*(const char **) a, *(const char **) b);
}
                                                                                
/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Output_Modulefile_Changes			     **
 ** 									     **
 **   Description:	Is used to flush out the changes of the current	     **
 **			modulefile in a manner depending upon whether the    **
 **			modulefile was successful or unsuccessful.	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		The attached Tcl in- **
 **							terpreter	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals:	GlobalHashTable,				     **
 **			unsetenvHashTable,				     **
 **			aliasSetHashTable,	via Output_Modulefile_Aliases**
 **			aliasUnsetHashTable	via Output_Modulefile_Aliases**
 **			change_dir		for the chdir command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Output_Modulefile_Changes(
	Tcl_Interp * interp
) {
	char           *val = NULL,	/** Stored value (is a pointer!)     **/
		      **keys;		/** hash keys			     **/
	uvec           *uvkeys;		/** vector of keys		     **/
	int             i;		/** Loop counter		     **/
	MHash	       *table[2];	/** setenv & unsetenv hashes	     **/

    /**
     **  The following hash tables contain all changes to be made on
     **  shell variables
     **/
	table[0] = setenvHashTable;
	table[1] = unsetenvHashTable;

	aliasfile = stdout;
    /**
     **  Scan both tables that are of interest for shell variables
     **/
	for (i = 0; i < 2; i++) {
		if (!mhash_exists(table[i]))
			continue;
		uvkeys = mhash_keys_uvec(table[i]);
		uvec_qsort(uvkeys, keycmp);

		/* output key/values */
		keys = uvec_vector(uvkeys);
		while (keys && *keys) {
			if (i == 1) {
				output_unset_variable(*keys);
			} else {
				if ((val = EMGetEnv(interp, *keys)))
					output_set_variable(interp, *keys, val);
			}
			keys++;
		}
	} /** for **/

	if (EOF == fflush(stdout))
		if (OK != ErrorLogger(ERR_FLUSH, LOC, _fil_stdout, NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/

	Output_Modulefile_Aliases();
	Output_Directory_Change();

    /**
     **  Delete and reset the hash tables since the current contents have been
     **  flushed.
     **/
	Global_Hash_Tables(GHashClear, NULL);

	return (TCL_OK);
} /* End of 'Output_Modulefile_Changes' */

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Open_Aliasfile					     **
 ** 									     **
 **   Description:	Creates/opens or closes temporary file for sourcing  **
 **			or aliases.					     **
 **			Passes back the filehandle and filename in global    **
 ** 			variables.					     **
 ** 									     **
 **   First Edition:	2005/09/26	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	int	action		if != 0 to open else close   **
 ** 									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals: aliasfile					     **
 **			aliasfilename					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static int Open_Aliasfile(
	int action
) {
	if (action) {
	/**
	 **  Open the file ...
	 **/
		if (tmpfile_mod(&aliasfilename, &aliasfile))
			if (OK != ErrorLogger(ERR_OPEN, LOC, aliasfilename,
					      _(em_appending), NULL))
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
	} else {
		if (EOF == fclose(aliasfile))
			if (OK !=
			    ErrorLogger(ERR_CLOSE, LOC, aliasfilename, NULL))
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
	}

	return (TCL_OK);
} /** End of 'Open_Aliasfile' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Output_Modulefile_Aliases			     **
 ** 									     **
 **   Description:	Is used to flush out the changes to the aliases of   **
 **			the current modulefile. But, some shells don't work  **
 **			well with having their alias information set via the **
 **			'eval' command.  So, what we'll do now is output the **
 **			aliases into a /tmp dotfile, have the shell source   **
 **			the /tmp dotfile and then have the shell remove the  **
 **			/tmp dotfile.					     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	void			none			     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals: aliasSetHashTable,	via Output_Modulefile_Aliases**
 **			aliasUnsetHashTable	via Output_Modulefile_Aliases**
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static int Output_Modulefile_Aliases(
	void
) {
	char           *val = NULL,	/** Stored value (is a pointer!)     **/
		      **keys = NULL,	/** hash keys			     **/
		       *sourceCommand;	/** Command used to source the alias **/
	int             i,		/** Loop counter		     **/
	                openfile = 0;	/** whether using a file or not	     **/

    /**
     **  The following hash tables do contain all changes to be made on
     **  shell aliases
     **/
	MHash  *table[2];

	table[0] = aliasSetHashTable;
	table[1] = aliasUnsetHashTable;

    /**
     **  If configured so, all changes to aliases are written into a temporary
     **  file which is sourced by the invoking shell ...
     **  In this case a temporary filename has to be assigned for the alias
     **  source file. The file has to be opened as 'aliasfile'.
     **  The default for aliasfile, if no shell sourcing is used, is stdout.
     **
     **  We only need to output stuff into a temporary file if we're setting
     **  stuff.  We can unset variables and aliases by just using eval.
     **/
	if (mhash_exists(aliasSetHashTable))
		keys = mhash_keys(aliasSetHashTable);
	while (keys && *keys) {
	/**
	 **  We must use an aliasfile if EVAL_ALIAS is not defined
	 **  or the sh shell does not do aliases (HAS_BOURNE_ALIAS)
	 **  and that the sh shell does do functions (HAS_BOURNE_FUNCS)
	 **/
		if (!eval_alias || (!strcmp(shell_name, "sh") && !bourne_alias
			&& bourne_funcs)) {
			if (OK != Open_Aliasfile(1))
				if (OK !=
				    ErrorLogger(ERR_OPEN, LOC, aliasfilename,
						_(em_appending), NULL))
					return (TCL_ERROR);
					/** -------- EXIT (FAILURE) -------> **/
			openfile = 1;
	/**
	 **  We only support sh and csh variants for aliases.  If not either
	 **  sh or csh print warning message and return
	 **/
			assert(shell_derelict != NULL);
			if (!strcmp(shell_derelict, "csh")) {
				sourceCommand = "source %s%s";
			} else if (!strcmp(shell_derelict, "sh")) {
				sourceCommand = ". %s%s";
			} else {
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
			}
		} /* if */
		if (openfile) {
	    /**
	     **  Only the source command has to be flushed to stdout. After
	     **  sourcing the alias definition (temporary) file, the source
	     **  file is to be removed.
	     **/
			alias_separator = '\n';

			fprintf(stdout, sourceCommand, aliasfilename,
				shell_cmd_separator);
			fprintf(stdout, "/bin/rm -f %s%s", aliasfilename,
				shell_cmd_separator);
		} /** openfile **/
		keys++;
	} /* while */
    /**
     **  Scan the hash tables involved in changing aliases
     **/
	for (i = 0; i < 2; i++) {
		if (!mhash_exists(table[i]))
			continue;
		keys = mhash_keys(table[i]);
		while (keys && *keys) {
		/**
		 **  The hashtable list index is used to differ between aliases
		 **  to be set and aliases to be reset
		 **/
			val = mhash_value(table[i], *keys);
			if (i == 1) {
				output_unset_alias(*keys, val);
			} else {
				output_set_alias(*keys, val);
			}
			keys++;
		}
	} /** for **/

	if (openfile) {
		if (OK != Open_Aliasfile(0))
			if (OK !=
			    ErrorLogger(ERR_CLOSE, LOC, aliasfilename, NULL))
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/

		null_free((void *)&aliasfilename);
	}

	return (TCL_OK);

} /** End of 'Output_Modulefile_Aliases' **/

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		Output_Directory_Change				     **
 **									     **
 **   Description:	Changes the current working directory.               **
 **									     **
 **   Parameters:	void			none			     **
 **									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 **				TCL_ERROR	When not applicable	     **
 **									     **
 **   Attached Global:  change_dir					     **
 **									     **
 ** ************************************************************************ **
 ++++*/
static int Output_Directory_Change(
	void
) {
	int             retval = TCL_OK;

	if (!change_dir)
		return retval;

	assert(shell_derelict != NULL);
	if (!strcmp(shell_derelict, "csh") || !strcmp(shell_derelict, "sh")) {
		fprintf(stdout, "cd '%s'%s", change_dir, shell_cmd_separator);
	} else if (!strcmp(shell_derelict, "perl")) {
		fprintf(stdout, "chdir '%s'%s", change_dir,
			shell_cmd_separator);
	} else if (!strcmp(shell_derelict, "python")) {
		fprintf(stdout, "os.chdir('%s')\n", change_dir);
	} else if( !strcmp( shell_derelict, "ruby")) {
		fprintf(stdout, "Dir.chdir('%s')\n", change_dir);
	} else {
		retval = TCL_ERROR;
	}

	null_free((void *)&change_dir);

	return retval;
}


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_set_variable				     **
 ** 									     **
 **   Description:	Outputs the command required to set a shell variable **
 **			according to the current shell			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp	The attached Tcl interpreter **
 **			const char	*var	Name of the variable to be   **
 **						set			     **
 **			const char	*val	Value to be assigned	     **
 **									     **
 **   Result:		int	TCL_OK		Finished successful	     **
 **				TCL_ERROR	Unknown shell type	     **
 **									     **
 **   Attached Globals:	shell_derelict					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static int output_set_variable(	Tcl_Interp	*interp,
				const char *var,
				const char *val) {

    /**
     **  Differ between the different kinds of shells at first
     **
     **  CSH
     **/
	chop(val);
	chop(var);

	assert(shell_derelict != NULL);

	if (!strcmp((char *)shell_derelict, "csh")) {

#ifdef LMSPLIT_SIZE
	/**
	 **  Many C Shells (specifically the Sun one) has a hard limit on
	 **  the size of the environment variables around 1k.  The
	 **  _LMFILES_ variable can grow beyond 1000 characters.  So, I'm
	 **  going to break it up here since I can put it back together
	 **  again when I use it.
	 **
	 **  You can set the split size using --with-split-size=<number>
	 **  it should probably be <1000.  I don't count the size of
	 **  "setenv _LMFILES_xxx" so subtract this from your limit.
	 **/
		if (!strcmp(var, "_LMFILES_")) {
			char            formatted[MOD_BUFSIZE],
				       *cptr;
			int             lmfiles_len,
					count = 0;
			char *escaped = stringer(NULL, strlen(val)*2 + 1, NULL);

			EscapeCshString(val, escaped);

			if ((lmfiles_len = strlen(escaped)) > LMSPLIT_SIZE) {

				char            buffer[LMSPLIT_SIZE + 1];

	    /**
	     **  Break up the _LMFILES_ variable...
	     **/
				while (lmfiles_len > LMSPLIT_SIZE) {

					strncpy(buffer, (escaped +
						 count * LMSPLIT_SIZE),
						LMSPLIT_SIZE);
					buffer[LMSPLIT_SIZE] = '\0';

					fprintf(stdout, "setenv %s%03d %s %s",
						var, count, buffer,
						shell_cmd_separator);

					lmfiles_len -= LMSPLIT_SIZE;
					count++;
				}

				if (lmfiles_len) {
					fprintf(stdout, "setenv %s%03d %s %s",
						var, count, (escaped +
						 count * LMSPLIT_SIZE),
						shell_cmd_separator);
					count++;
				}
		/**
		 ** Unset _LMFILES_ as indicator to use the multi-variable
		 ** _LMFILES_
		 **/
				fprintf(stdout, "unsetenv %s %s", var,
					shell_cmd_separator);

			} else {
			/** if ( lmfiles_len = strlen(val)) > LMSPLIT_SIZE) **/

				fprintf(stdout, "setenv %s %s %s", var, escaped,
					shell_cmd_separator);
			}
	    /**
	     ** Unset the extra _LMFILES_%03d variables that may be set
	     **/
			do {
				if (cptr)	null_free((void *) &cptr);
				sprintf(formatted, "_LMFILES_%03d", count++);
				cptr = EMGetEnv(interp, formatted);
				if (cptr && *cptr)
					fprintf(stdout, "unsetenv %s %s",
						formatted, shell_cmd_separator);
			} while (cptr && *cptr);

			null_free((void *) &cptr);
			null_free((void *) &escaped);

		} else {	/** if( var == "_LMFILES_") **/
#endif	/* not LMSPLIT_SIZE */

			char *escaped = stringer(NULL, strlen(val)*2 + 1, NULL);
			EscapeCshString(val, escaped);
			fprintf(stdout, "setenv %s %s %s", var, escaped,
				shell_cmd_separator);
			null_free((void *)&escaped);
#ifdef LMSPLIT_SIZE
		}
#endif	/* not LMSPLIT_SIZE */

	} else if (!strcmp((char *) shell_derelict, "sh")) {
    /**
     **  SH
     **/
		char *escaped = (char *) module_malloc(strlen(val) * 2 + 1);
		EscapeShString(val, escaped);

		fprintf(stdout, "%s=%s %sexport %s%s", var, escaped,
			shell_cmd_separator, var, shell_cmd_separator);
		null_free((void **)&escaped);

	} else if (!strcmp((char *) shell_derelict, "emacs")) {
    /**
     **  EMACS
     **/
		fprintf(stdout, "(setenv \"%s\" \'%s\')\n", var, val);

	} else if (!strcmp((char *) shell_derelict, "perl")) {
    /**
     **  PERL
     **/
		char *escaped = stringer(NULL, strlen(val) * 2 + 1, NULL);
		EscapePerlString(val, escaped);
		fprintf(stdout, "$ENV{'%s'} = '%s'%s", var, escaped,
			shell_cmd_separator);
		null_free((void *)&escaped);

	} else if (!strcmp((char *) shell_derelict, "python")) {
    /**
     **  PYTHON
     **/
		fprintf(stdout, "os.environ['%s'] = '%s'\n", var, val);

	} else if( !strcmp((char*) shell_derelict, "ruby")) {
    /**
     **  RUBY
     **/
	fprintf( stdout, "ENV['%s'] = '%s'\n", var, val);

	} else if( !strcmp((char*) shell_derelict, "cmake")) {
    /**
     **  CMAKE
     **/
		char* escaped = stringer(NULL, strlen(val)*2+1,NULL);
		EscapeCmakeString(val,escaped);
		fprintf(stdout, "set(ENV{%s} \"%s\")%s", var, escaped,
			shell_cmd_separator);
		null_free((void *) &escaped);

	} else if (!strcmp((char *) shell_derelict, "scm")) {
    /**
     ** SCM
     **/
		fprintf(stdout, "(putenv \"%s=%s\")\n", var, val);

	} else if (!strcmp((char *) shell_derelict, "mel")) {
    /**
     ** MEL (Maya Extension Language)
     **/
		fprintf(stdout, "putenv \"%s\" \"%s\";", var, val);

	} else {
    /**
     **  Unknown shell type - print an error message and 
     **  return on error
     **/
		if (OK != ErrorLogger(ERR_DERELICT, LOC, shell_derelict, NULL))
			return (TCL_ERROR);	/** ---- EXIT (FAILURE) ---> **/
	}

    /**
     **  Return and acknowldge success
     **/
	g_output = 1;
	return (TCL_OK);

} /** End of 'output_set_variable' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_unset_variable				     **
 ** 									     **
 **   Description:	Outputs the command required to unset a shell        **
 **			variable according to the current shell		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*var	Name of the variable to be   **
 **						unset			     **
 **									     **
 **   Result:		int	TCL_OK		Finished successful	     **
 **				TCL_ERROR	Unknown shell type	     **
 **									     **
 **   Attached Globals:	shell_derelict					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static int output_unset_variable(
	const char *var
) {
	chop(var);

	assert(shell_derelict != NULL);
    /**
     **  Display the 'unsetenv' command according to the current invoking shell.
     **/
	if (!strcmp(shell_derelict, "csh")) {
		fprintf(stdout, "unsetenv %s%s", var, shell_cmd_separator);
	} else if (!strcmp(shell_derelict, "sh")) {
		fprintf(stdout, "unset %s%s", var, shell_cmd_separator);
	} else if (!strcmp(shell_derelict, "emacs")) {
		fprintf(stdout, "(setenv \"%s\" nil)\n", var);
	} else if (!strcmp(shell_derelict, "perl")) {
		fprintf(stdout, "delete $ENV{'%s'}%s", var,
			shell_cmd_separator);
	} else if (!strcmp(shell_derelict, "python")) {
		fprintf(stdout, "os.environ['%s'] = ''\ndel os.environ['%s']\n",
			var, var);
	} else if (!strcmp(shell_derelict, "ruby")) {
		fprintf(stdout, "ENV['%s'] = nil\n", var);
	} else if( !strcmp( shell_derelict, "cmake")) {
		fprintf( stdout, "unset(ENV{%s})%s", var, shell_cmd_separator);
	} else if (!strcmp(shell_derelict, "scm")) {
		fprintf(stdout, "(putenv \"%s\")\n", var);
	} else if (!strcmp(shell_derelict, "mel")) {
		fprintf(stdout, "putenv \"%s\" \"\";", var);
	} else {
		if (OK != ErrorLogger(ERR_DERELICT, LOC, shell_derelict, NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}
    /**
     **  Return and acknowledge success
     **/
	g_output = 1;
	return (TCL_OK);

} /** End of 'output_unset_variable' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_function					     **
 ** 									     **
 **   Description:	Actually turns the Modules set-alias information     **
 **			into a string that a shell can source.  Previously,  **
 **			this routine just output the alias information to be **
 **			eval'd by the shell.				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*var	Name of the alias to be set  **
 **			const char	*val	Value to be assigned	     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	aliasfile,	The output file for alias commands.  **
 **					see 'Output_Modulefile_Aliases'      **
 **			alias_separator					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static void output_function(
	const char *var,
	const char *val
) {
	const char     *cptr = val;
	int             nobackslash = 1;

    /**
     **  This opens a function ...
     **/
	fprintf(aliasfile, "%s() { ", var);

    /**
     **  ... now print the value. Print it as a single line and remove any
     **  backslashes, and substitute a safe $*
     **/
	while (*cptr) {
		if (*cptr == '\\') {
			if (!nobackslash)
				putc(*cptr, aliasfile);
			else
				nobackslash = 0;
			cptr++;
			continue;
		} else if (*cptr == '$' && (cptr + 1) && (*(cptr + 1) == '*')) {
			/* found $* */
			fputs("${1+\"$@\"}", aliasfile);
			cptr++;
			cptr++;
			continue;
		} else
			nobackslash = 1;

		putc(*cptr++, aliasfile);
	} /** while **/
    /**
     **  Finally close the function
     **/
	fprintf(aliasfile, "%c}%c", alias_separator, alias_separator);

} /** End of 'output_function' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_set_alias				     **
 ** 									     **
 **   Description:	Flush the commands required to set shell aliases de- **
 **			pending on the current invoking shell		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*alias		Name of the alias    **
 **			const char	*val		Value to be assigned **
 ** 									     **
 **   Result:		int	TCL_OK		Operation successful	     **
 ** 									     **
 **   Attached Globals:	aliasfile, 	The alias command is written out to  **
 **			alias_separator Defined the command separator	     **
 **			shell_derelict	to determine the shell family	     **
 **			shell_name	to determine the real shell type     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static int output_set_alias(
	const char *alias,
	const char *val
) {
	int             nobackslash = 1;/** Controls whether backslashes are **/
					/** to be print			     **/
	const char     *cptr = val;	/** Scan the value char by char	     **/

	assert(shell_derelict != NULL);
    /**
     **  Check for the shell family
     **  CSHs need to switch $* to \!* and $n to \!\!:n unless the $ has a
     **  backslash before it
     **/
	if (!strcmp(shell_derelict, "csh")) {
	/**
	 **  On CSHs the command is 'alias <name> <value>'. Print the beginning
	 **  of the command and then print the value char by char.
	 **/
		fprintf(aliasfile, "alias %s '", alias);

		while (*cptr) {
	    /**
	     **  Convert $n to \!\!:n
	     **/
			if (*cptr == '$' && nobackslash) {
				cptr++;
				if (*cptr == '*')
					fprintf(aliasfile, "\\!");
				else
					fprintf(aliasfile, "\\!\\!:");
			}
	    /**
	     **  Recognize backslashes
	     **/
			if (*cptr == '\\') {
				if (!nobackslash)
					putc(*cptr, aliasfile);
				else
					nobackslash = 0;
				cptr++;
				continue;
			} else
				nobackslash = 1;
	    /**
	     **  print the read character
	     **/
			putc(*cptr++, aliasfile);
		} /** while **/
	/**
	 **  Now close up the command using the alias command terminator as
	 **  defined in the global variable
	 **/
		fprintf(aliasfile, "'%c", alias_separator);
    /**
     **  Bourne shell family: The alias has to be  translated into a
     **  function using the function call 'output_function'
     **/
	} else if (!strcmp(shell_derelict, "sh")) {
	/**
	 **  Shells supporting extended bourne shell syntax ....
	 **/
		if ((!strcmp(shell_name, "sh") && bourne_alias)
		    || !strcmp(shell_name, "bash")
		    || !strcmp(shell_name, "zsh")
		    || !strcmp(shell_name, "ksh")) {
	    /**
	     **  in this case we only have to write a function if the alias
	     **  takes arguments. This is the case if the value has '$'
	     **  somewhere in it without a '\' in front.
	     **/
			while (*cptr) {
				if (*cptr == '\\') {
					if (nobackslash)
						nobackslash = 0;
				} else {
					if (*cptr == '$') {
						if (nobackslash) {
							output_function(alias,
									val);
							return TCL_OK;
						}
					}
					nobackslash = 1;
				}
				cptr++;
			}
	    /**
             **  So, we can just output an alias with '\$' translated to '$'...
             **/
			fprintf(aliasfile, "alias %s='", alias);

			nobackslash = 1;
			cptr = val;

			while (*cptr) {
				if (*cptr == '\\') {
					if (nobackslash) {
						nobackslash = 0;
						cptr++;
						continue;
					}
				}
				nobackslash = 1;
				putc(*cptr++, aliasfile);
			} /** while **/

			fprintf(aliasfile, "'%c", alias_separator);

		} else if (!strcmp(shell_name, "sh") && bourne_funcs) {
	/**
	 **  The bourne shell itself
         **  need to write a function unless this sh doesn't support
	 **  functions (then just punt)
	 **/
			output_function(alias, val);
		} /** ??? Unknown derelict ??? **/
	} /** if( sh ) **/
	return (TCL_OK);

} /** End of 'output_set_alias' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_unset_alias				     **
 ** 									     **
 **   Description:	Flush the commands required to reset shell aliases   **
 **			depending on the current invoking shell		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*alias		Name of the alias    **
 **			const char	*val		Value which has been **
 **							assigned	     **
 ** 									     **
 **   Result:		int	TCL_OK		Operation successful	     **
 ** 									     **
 **   Attached Globals:	aliasfile, 	The alias command is writte out to   **
 **			alias_separator Defined the command separator	     **
 **			shell_derelict	to determine the shell family	     **
 **			shell_name	to determine the real shell type     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static int output_unset_alias(
	const char *alias,
	const char *val
) {
	int             nobackslash = 1;/** Controls wether backslashes are  **/
					/** to be print			     **/
	const char     *cptr = val;	/** read the value char by char      **/

	assert(shell_derelict != NULL);
    /**
     **  Check for the shell family at first
     **  Ahh! CSHs ... ;-)
     **/
	if (!strcmp(shell_derelict, "csh")) {

		fprintf(aliasfile, "unalias %s%c", alias, alias_separator);

	} else if (!strcmp(shell_derelict, "sh")) {
    /**
     **  Hmmm ... bourne shell types ;-(
     **  Need to unset a function in case of sh or if the alias took parameters
     **/

		if (!strcmp(shell_name, "sh")) {
			if (bourne_alias) {
				fprintf(aliasfile, "unalias %s%c", alias,
					alias_separator);
			} else if (bourne_funcs) {
				fprintf(aliasfile, "unset -f %s%c", alias,
					alias_separator);
			} /* else do nothing */
		} else if (!strcmp(shell_name, "bash")) {
	/**
	 **  BASH
	 **/
	    /**
             **  If we have what the old value should have been, then look to
             **  see if it was a function or an alias because bash spits out an
             **  error if you try to unalias a non-existent alias.
             **/
			if (val) {
		/**
                 **  Was it a function?
                 **  Yes, if it has arguments...
                 **/
				while (*cptr) {
					if (*cptr == '\\') {
						if (nobackslash) {
							nobackslash = 0;
						}
					} else {
						if (*cptr == '$') {
							if (nobackslash) {
		fprintf(aliasfile,"unset -f %s%c",alias,alias_separator);
								return TCL_OK;
							}
						}
						nobackslash = 1;
					}
					cptr++;
				}
		/**
                 **  Well, it wasn't a function, so we'll put out an unalias...
                 **/
				fprintf(aliasfile, "unalias %s%c", alias,
					alias_separator);

			} else { /** No value known (any more?) **/
		/**
                 **  We'll assume it was a function because the unalias command
                 **  in bash produces an error.  It's possible that the alias
                 **  will not be cleared properly here because it was an
                 **  unset-alias command.
                 **/
				fprintf(aliasfile, "unset -f %s%c", alias,
					alias_separator);
			}
	/**
	 **  ZSH or KSH
	 **  Put out both because we it could be either a function or an
	 **  alias.  This will catch both.
	 **/
		} else if (!strcmp(shell_name, "zsh")) {
			fprintf(aliasfile, "unalias %s%c", alias,
				alias_separator);
		} else if (!strcmp(shell_name, "ksh")) {
			fprintf(aliasfile, "unalias %s%c", alias,
				alias_separator);
			fprintf(aliasfile, "unset -f %s%c", alias,
				alias_separator);
		} /** if( bash, zsh, ksh) **/
	 /** ??? Unknown derelict ??? **/
	}
	  /** if( sh-family) **/
	return (TCL_OK);

} /** End of 'output_unset_alias' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		getLMFILES					     **
 ** 									     **
 **   Description:	Read in the _LMFILES_ environment variable. This one **
 **			may be split into several variables cause by limited **
 **			variable space of some shells (esp. the SUN csh)     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	void			none			     **
 ** 									     **
 **   Result:		char*	Value of the environment variable _LMFILES_  **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char           *getLMFILES(
	Tcl_Interp	*interp
) {
	static char    *lmfiles = NULL;	/** Buffer pointer for the value     **/

    /**
     **  Try to read the variable _LMFILES_. If the according buffer pointer
     **  contains a value, disallocate it before.
     **/
	if (lmfiles)	null_free((void *)&lmfiles);

	lmfiles = EMGetEnv(interp, "_LMFILES_");

    /**
     **  Now the pointer is NULL in case of the variable has not been defined.
     **  In this case try to read in the splitted variable from _LMFILES_xxx
     **/
	if (!lmfiles) {

		char            buffer[MOD_BUFSIZE];
					/** Used to set up the split variab- **/
					/** les name			     **/
		int             count = 0;
					/** Split part count		     **/
		int             lmsize = 0;
					/** Total size of _LMFILES_ content  **/
		int             old_lmsize;
					/** Size save buffer		     **/
		int             cptr_len;
					/** Size of the current split part   **/
		char           *cptr;	/** Split part read pointer	     **/

	/**
	 **  Set up the split part environment variable name and try to read it
	 **  in
	 **/
		sprintf(buffer, "_LMFILES_%03d", count++);
		cptr = EMGetEnv(interp, buffer);

		while (cptr && *cptr) {	/** Something available		     **/

	    /**
	     **  Count up the variables length
	     **/
			cptr_len = strlen(cptr);
			old_lmsize = lmsize;
			lmsize += cptr_len;

	    /**
	     **  Reallocate the value's buffer and copy the current split
	     **  part at its end
	     **/
			if (!(lmfiles = (char *) module_realloc(lmfiles,
						lmsize * sizeof(char) + 1))) {
				if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
					return (NULL);
						/** ---- EXIT (FAILURE) ---> **/
			}

			strncpy(lmfiles + old_lmsize, cptr, cptr_len);
			*(lmfiles + old_lmsize + cptr_len) = '\0';

	    /**
	     **  Read the next split part variable
	     **/
			if (cptr)	null_free((void *) &cptr);
			sprintf(buffer, "_LMFILES_%03d", count++);
			cptr = EMGetEnv(interp, buffer);
		}
		if (cptr)	null_free((void *) &cptr);

	} else { /** if( lmfiles) **/
	/**
	 **  If the environvariable _LMFILES_ has been set, copy the contents
	 **  of the returned buffer into a free allocated one in order to
	 **  avoid side effects.
	 **/
		char           *tmp = stringer(NULL, 0, lmfiles, NULL);

		if (!tmp)
			if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
				return (NULL);
					/** -------- EXIT (FAILURE) -------> **/

	/** 
	 **  Set up lmfiles pointing to the new buffer in order to be able to
	 **  disallocate when invoked next time.
	 **/
		lmfiles = tmp;

	} /** if( lmfiles) **/
    /**
     **  Return the received value to the caller
     **/
	return (lmfiles);

} /** end of 'getLMFILES' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		IsLoaded					     **
 ** 									     **
 **   Description:	Check whether the passed modulefile is currently     **
 **			loaded						     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char             *modulename	Name of the module to**
 **							be searched for	     **
 **			char            **realname	Buffer for the name  **
 **							and version of the   **
 **							module that has mat- **
 **							ched the query	     **
 **			char             *filename	Buffer to store the  **
 **							whole filename of a  **
 **							found loaded module  **
 **									     **
 **   Result:		int	0	Requested module not loaded	     **
 **				1	module is loaded		     **
 **									     **
 **			realname	points to the name of the module that**
 **					has matched the query. If this poin- **
 **					differs form 'modulename' after this **
 **					function has finished, the buffer for**
 **					to store the module name in has been **
 **					allocated here.			     **
 **					if (char **) NULL is passed, no buf- **
 **					fer will be allocated		     **
 **				??? Is this freed correctly by the caller ???**
 **									     **
 **			filename	will be filled with the full module  **
 **					file path of the module that has     **
 **					matched the query		     **
 **									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

/**
 **  Check all possibilities of module-versions
 **/
int IsLoaded(
	Tcl_Interp	 *interp,
	char		 *modulename,
	char		**realname,
	char		 *filename
) {
	return (__IsLoaded(interp, modulename, realname, filename, 0));
}

/**
 **  Check only an exact match of the passed module and version
 **/
int IsLoaded_ExactMatch(
	Tcl_Interp	 *interp,
	char		 *modulename,
	char		**realname,
	char		 *filename
) {
	return (__IsLoaded(interp, modulename, realname, filename, 1));
}

/**
 **  The subroutine __IsLoaded finally checks for the requested module being
 **  loaded or not.
 **/
static int __IsLoaded(
			Tcl_Interp	 *interp,
			char		 *modulename,
			char		**realname,
			char		 *filename,
			int		  exact)
{
    char *l_modules = NULL;		/** Internal module list buffer	     **/
    char *l_modulefiles = NULL;		/** Internal module file list buffer **/
    char *loaded = NULL;		/** Buffer for the module            **/
    char *basename = NULL;		/** Pointer to module basename       **/
    char *loadedmodule_path = NULL;	/** Pointer to one loaded module out **/
					/** of the loaded modules list	     **/
    int   count = 0;

    /**
     **  Get a list of loaded modules (environment variable 'LOADEDMODULES')
     **  and the list of loaded module-files (env. var. __LMFILES__)
     **/
    char	*loaded_modules = EMGetEnv(interp, "LOADEDMODULES");
    char	*loaded_modulefiles = getLMFILES(interp);
    
    /**
     **  If no module is currently loaded ... the requested module is surely
     **  not loaded, too ;-)
     **/
    if( !loaded_modules) 
	goto unwind0;
    
    /**
     **  Copy the list of currently loaded modules into a new allocated array
     **  for further handling. If this fails it will be assumed, that the 
     **  module is *NOT* loaded.
     **/
    if(!(l_modules = stringer(NULL,0,loaded_modules,NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;

    /**
     **  Copy the list of currently loaded modulefiles into a new allocated
     **  array for further handling. If this failes it will be assumed, that
     **  the module is *NOT* loaded.
     **/
    if(loaded_modulefiles)
	if(!(l_modulefiles = stringer(NULL,0, loaded_modulefiles,NULL)))
	    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		goto unwind1;

    /**
     **  Assume the modulename given was an exact match so there is no
     **  difference to return -- this will change in the case it wasn't an
     **  exact match below
     **/
    if( realname)
        *realname = modulename;

    if( *l_modules) {

	/**
	 **  Get each single module which is loaded by splitting up at colons
	 **  The variable LOADEDMODULES contains a list of modulefile like the
	 **  following:
	 **                gnu/2.0:openwin/3.0
	 **/
	loadedmodule_path = xstrtok( l_modules, ":");
	while( loadedmodule_path) {

	    if(!(loaded = stringer(NULL,0, loadedmodule_path,NULL)))
		if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		    goto unwind2;

	    /**
	     **  Get a modulefile without a version and check if this is the
	     **  requested one.
	     **/
	    if( !strcmp( loaded, modulename)) {	/** FOUND    **/

		null_free ((void *) &loaded);
		break;			/** leave the while loop	     **/

	    } else if( !exact) {		/** NOT FOUND	     **/

		/**
		 **  Try to more and more simplify the modulename by removing
		 **  all detail (version) information
		 **/
		basename = get_module_basename( loaded);
		while( basename && strcmp( basename, modulename)) {
		    basename = get_module_basename( basename);
                }

		/**
		 **  Something left after splitting again? If yes the requested
		 **  module is found!
                 **  Since the name given was a basename, return the fully
		 **  loaded path
		 **/
                if( basename) {
		    null_free ((void *) &loaded);
                    if( realname)
			if(!(*realname=stringer(NULL,0,loadedmodule_path,NULL)))
			    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
				goto unwind2;

		    break;		/** leave the while loop	     **/

		} /** if( basename) **/
	    } /** if not found with single basename **/

	    /**
	     **  Get the next entry from the loaded modules list
	     **/
	    loadedmodule_path = xstrtok( NULL, ":");
            count++;

	    null_free ((void *) &loaded); /** Free what has been alloc. **/

	} /** while **/
    } /** if( *l_modules) **/

    /**
     **  If we found something locate it's associated modulefile
     **/
    if( loadedmodule_path) {
        if( filename && l_modulefiles && *l_modulefiles) {

	    /**
	     **  The position of the loaded module within the list of loaded
	     **  modules has been counted in 'count'. The position of the 
	     **  associated modulefile should be the same. So tokenize the
	     **  list of modulefiles by the colon until the wanted position
	     **  is reached.
	     **/
            char* modulefile_path = xstrtok(l_modulefiles, ":");
	
            while( count) {
                if( !( modulefile_path = xstrtok( NULL, ":"))) {

		    /**
		     **  Oops! Fewer entries in the list of loaded modulefiles
		     **  than in the list of loaded modules. This will
		     **  generally suggest that _LMFILES_ has become corrupted,
		     **  but it may just mean we're working intermittantly with
		     **  an old version.  So, I'll just not touch filename which
		     **  means the search will continue using the old method of
		     **  looking through MODULEPATH.  
                     */
		    goto success0;
                }
                count--;

            } /** while **/

	    /**
	     **  Copy the result into the buffer passed from the caller
	     **/
            strcpy( filename, modulefile_path);
        }

	/**
	 **  FOUND.
	 **  free up everything which has been allocated and return on success
	 **/
	goto success0;
    }

    /**
     **  NOT FOUND. Free up everything which has been alloc'd and return on
     **  failure
     **/

unwind2:
    if( l_modulefiles)
	null_free((void *) &l_modulefiles);
unwind1:
    null_free((void *) &l_modules);
unwind0:
    return( 0);				/** -------- EXIT (FAILURE) -------> **/

success0:
    if( l_modulefiles)
	null_free((void *) &l_modulefiles);
    null_free((void *) &l_modules);
    return( 1);				/** -------- EXIT (SUCCESS) -------> **/

} /** End of '__IsLoaded' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		get_module_basename				     **
 ** 									     **
 **   Description:	Get the name of a module without its version.	     **
 **			This function modifies the string passed in.	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char	*modulename		Full module name     **
 ** 									     **
 **   Result:		char*		Module name without version	     **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static char    *get_module_basename(
	char *modulename
) {
	char           *version;	/** Used to locate the version sep.  **/
    /**
     **  Use strrchr to locate the very last version string on the module
     **  name.
     **/
	if ((version = strrchr(modulename, *psep))) {
		*version = '\0';
	} else {
		modulename = NULL;
	}
    /**
     **  Return the *COPIED* string
     **/
	return (modulename);

} /** End of 'get_module_basename' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Update_LoadedList				     **
 ** 									     **
 **   Description:	Add or remove the passed modulename and filename to/ **
 **			from LOADEDMODULES and _LMFILES_		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp      *interp		Attached Tcl Interp. **
 **			char            *modulename	Name of the module   **
 **			char            *filename	Full path name of the**
 **							related modulefile   **
 **									     **
 **   Result:		int	1	Successful operation		     **
 **									     **
 **   Attached Globals:	g_flags		Controls whether the modulename      **
 **					should be added (M_XXXX) or removed  **
 **					(M_REMOVE) from the list of loaded   **
 **					modules				     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Update_LoadedList(
	Tcl_Interp * interp,
	char *modulename,
	char *filename
) {
	char           *argv[4],		/** temporary arg vector     **/
	               *basename,		/** of module name	     **/
	               *module;			/** module name		     **/
	Tcl_Obj        **objv;			/** Tcl Object vector	     **/
	int             objc;			/** Tcl Object vector count  **/

    /**
     **  Apply changes to LOADEDMODULES first
     **/
	argv[1] = "LOADEDMODULES";
	argv[2] = modulename;
	argv[3] = NULL;

	if (g_flags & M_REMOVE) {
		argv[0] = "remove-path";
	} else {
		argv[0] = "append-path";
	}
	Tcl_ArgvToObjv(&objc, &objv, -1, (char **) argv);
	if (g_flags & M_REMOVE) {
		cmdRemovePath(0, interp, objc, objv);
	} else {
		cmdSetPath(0, interp, objc, objv);
	}

    /**
     **  Apply changes to _LMFILES_ now
     **/
	argv[1] = "_LMFILES_";
	argv[2] = filename;
	argv[3] = NULL;

	if (g_flags & M_REMOVE) {
		argv[0] = "remove-path";
	} else {
		argv[0] = "append-path";
	}
	/* RKO: may need to clean-up objv first */
	Tcl_ArgvToObjv(&objc, &objv, -1, (char **) argv);
	if (g_flags & M_REMOVE) {
		cmdRemovePath(0, interp, objc, objv);
	} else {
		cmdSetPath(0, interp, objc, objv);
	}

    /**
     **  A module with just the basename might have been added and now we're
     **  removing one of its versions. We'll want to look for the basename in
     **  the path too.
     **/
	if (g_flags & M_REMOVE) {
		module = stringer(NULL, 0, modulename, NULL);
		basename = module;
		if ((basename = get_module_basename(basename))) {
			argv[2] = basename;
			argv[0] = "remove-path";
			/* RKO: may need to clean-up objv first */
			Tcl_ArgvToObjv(&objc, &objv, -1,(char **) argv);
			cmdRemovePath(0, interp, objc, objv);
		}
		null_free((void *)&module);
	}

    /**
     **  Return on success
     **/
	return (1);

} /** End of 'Update_LoadedList' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		check_magic					     **
 ** 									     **
 **   Description:	Check the magic cookie of the file passed as para-   **
 **			meter if it is a valid module file		     **
 **			Based on check_magic in Richard Elling's	     **
 **			find_by_magic <Richard.Elling"@eng.auburn.edu>	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char   *filename	Name of the file to check    **
 **			char   *magic_name	Magic cookie		     **
 **			int     magic_len	Length of the magic cookie   **
 ** 									     **
 **   Result:		int	0	Magic cookie doesn't match or any    **
 **					I/O error			     **
 **				1	Success - Magic cookie has matched   **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int check_magic( char	*filename,
		 char	*magic_name,
		 int	 magic_len)
{
    int  fd;				/** File descriptor for reading in   **/
    int  read_len;			/** Number of bytes read	     **/
    char buf[BUFSIZ];			/** Read buffer			     **/

    /**
     **  Parameter check. The length of the magic cookie shouldn't exceed the
     **  length of out read buffer
     **/
    if( magic_len > BUFSIZ)
	return 0;

    /**
     **  Open the file and read in as many bytes as required for checking the
     **  magic cookie. If there's an I/O error (Unable to open the file or
     **  less than magic_len have been read) return on failure.
     **/
    if( 0 > (fd = open( filename, O_RDONLY)))
	if( OK != ErrorLogger( ERR_OPEN, LOC, filename, _(em_reading), NULL))
	    return( 0);			/** -------- EXIT (FAILURE) -------> **/

    read_len = read( fd, buf, magic_len);
    
    if( 0 > close(fd))
	if( OK != ErrorLogger( ERR_CLOSE, LOC, filename, NULL))
	    return( 0);			/** -------- EXIT (FAILURE) -------> **/

    if( read_len < magic_len)
	return( 0);

    /**
     **  Check the magic cookie now
     **/
    return( !strncmp( buf, magic_name, magic_len));

} /** end of 'check_magic' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		regex_quote					     **
 ** 									     **
 **   Description:	Copy the passed path into the new path buffer and    **
 **			devalue '.' and '+'				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char   *path	Original path		     **
 **			      char   *newpath	Buffer for copy of the new   **
 **						path			     **
 **			      int     len	Max length of the new path   **
 **									     **
 **   Result:		newpath		will be filled up with the new, de-  **
 **					valuated path			     **
 **									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void regex_quote( const char	*path,
		         char	*newpath,
			 int	 len)
{
    unsigned int path_len = strlen( path);	/** Length of the orig. path **/
    int 	 i,				/** Read index		     **/
    		 j;				/** Write index		     **/

    /**
     **  Stopping at (len - 1) ensures that the newpath string can be
     **  null-terminated below.
     **/
    for( i=0, j=0; i<path_len && j<(len - 1); i++, j++) {

        switch(*path) {
            case '.':
            case '+':
            case '$':
		*newpath++ = '\\';		/** devalue '.' and '+'	    **/
		j++;
		break;
        }

	/**
	 **  Flush the current character into the newpath buffer
	 **/
        *newpath++ = *path++;

    } /** for **/

    /**
     **  Put a string terminator at the newpaths end
     **/
    *newpath = '\0';

} /** End of 'cleanse_path' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		chop						     **
 ** 									     **
 **   Description:	Remove '\n' characters from the passed string	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char   *string	String to be chopped		     **
 **									     **
 **   Result:		string		The chopped string		     **
 **									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	char *chop( const char	*string)
{
    char	*s, *t;			/** source and target pointers       **/

    /**
     **  Remove '\n'
     **/
    s = t = (char *) string;
    while( *s) {
	if( '\n' == *s)
	    s++;
	else
	    *t++ = *s++;
    }

    /**
     **  Copy the trailing terminator and return
     **/
    *t++ = '\0';
    return( (char *) string);

} /** End of 'chop' **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		xstrtok_r,xstrtok				     **
 ** 									     **
 **   Description: 	Considers the string s to consist of a sequence of   **
 **			zero or more text tokens separated by spans of one   **
 **			or more characters from the separator string  delim. **
 **			Just like the "standard" strtok(3), and the
 **			reentrant version strtok_r(3).			     **
 **			Except this tokenizer will return "empty" tokens too.**
 **			Providing our own strtok() eliminates the various    **
 **			nuances of different implementations.		     **
 **									     **
 **   First Edition:	2006/04/17	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:							     **
 **   Result:								     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char *xstrtok_r(char *s, const char *delim, char **ptrptr) {
	register char	*ptr, *lptr;
	const char	*tok;

	/* return NULL if NULL string and at end of the line */
	if ((!s) && (!*ptrptr))
		return (NULL);

	/* do not skip leading (or trailing) delimiters ... */

	/* if non-NULL string then at beginning of token parsing */
	if (s != NULL)	*ptrptr = s;

	ptr = *ptrptr;
	while (ptr && *ptr) {
		/* cycle through delimiters */
		tok = delim;
		while (tok && *tok) {
			if (*ptr == *tok) {	/* match */
				*ptr = '\0';	/* null terminate */
				lptr = *ptrptr;
				*ptrptr = ++ptr;	/* set for next time */
				return  lptr;
			}
			tok++;
		}
		ptr++;
	}
	/* didn't find delimiter */
	lptr = *ptrptr;
	*ptrptr = NULL;
	return lptr;
} /** End of 'xstrtok_r' **/

char *xstrtok(char *s, const char *delim) {
	static char	*last;
	return	xstrtok_r(s,delim,&last);
} /** End of 'xstrtok' **/

#if 0
void tryxstrtok (char *string, char *delim) {
	char *start, *str;
	char *token;
	int n = 1;

	start = str = strdup(string);
	printf("string: %s\n", str);
	printf("delim : %s\n", delim);
	
	token = xstrtok(str,delim);
	printf("\t%d = %s\n", n, token);
	while (token = xstrtok(NULL, delim)) {
		printf("\t%d = %s\n", ++n, token);
	}
	null_free(&start);
}
int main () {
	tryxstrtok("abc:def;ghi,jkl", ":;,");
	tryxstrtok(":abc:def;ghi,jkl", ":;,");
	tryxstrtok("::abc:def;ghi,jkl", ":;,");
	tryxstrtok("abc:def;ghi,jkl:", ":;,");
	tryxstrtok("abc:def;ghi,jkl::", ":;,");
	tryxstrtok("abc", ":;,");
	tryxstrtok("", ":;,");
}
#endif

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		chk4spch					     **
 ** 									     **
 **   Description:	goes through the given string and changes any non-   **
 **			printable characters to question marks.		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char	*s		String to be checke	     **
 ** 									     **
 **   Result:		*s			Will be changed accordingly  **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void chk4spch(char* s)
{
    for( ; *s; s++)
	if( !isgraph( *s)) *s = '?';

} /** End of 'chk4spch' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		xdup						     **
 ** 									     **
 **   Description:	will return a string with 1 level of environment     **
 ** 			variables expanded. The limit is MOD_BUFSIZE.	     **
 ** 			An env.var. is denoted with either $name or ${name}  **
 **			\$ escapes the expansion and substitutes a '$' in    **
 **			its place.					     **
 ** 									     **
 **   First Edition:	2000/01/21	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	char	*string		Environment variable	     **
 ** 									     **
 **   Result:		char    *		An allocated string	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char *xdup(char const *string) {
	char *result = NULL;
	char *dollarptr;

	if (!string) return result;

	/** need to work from copy of string **/
	if (!(result = stringer(NULL,0, string, NULL)))
	    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		return( (char*) NULL);	/** -------- EXIT (FAILURE) -------> **/

	/** check for '$' else just pass copy of it **/
	if (!(dollarptr = strchr(result, '$'))) {
		return result;
	} else {
	/** found something **/
		char const *envvar;
		char  buffer[MOD_BUFSIZE];
		char  oldbuffer[MOD_BUFSIZE];
		size_t blen = 0;	/** running buffer length	**/
		char *slashptr = result;/** where to continue parsing	**/
		char  slashchr;		/** store slash char		**/
		int   brace;		/** flag if ${name}		**/
		pid_t pid;		/** the process id		**/

		/** zero out buffers */
		memset(   buffer, '\0', MOD_BUFSIZE);
		memset(oldbuffer, '\0', MOD_BUFSIZE);

		/** copy everything upto $ into old buffer **/
		*dollarptr = '\0';
		strncpy(oldbuffer, slashptr, MOD_BUFSIZE);
		*dollarptr = '$';

		while (dollarptr) {
			if (*oldbuffer) strncpy(buffer, oldbuffer, MOD_BUFSIZE);
			blen = strlen(buffer);

			/** get the env.var. name **/
			if (*(dollarptr + 1) == '{') {
				brace = 1;
				slashptr = strchr(dollarptr + 1, '}');
			} else if (*(dollarptr + 1) == '$') {
				slashptr = dollarptr + 2;
			} else {
				slashptr = dollarptr + 1
					+ strcspn(dollarptr + 1,"/:$\\");
				brace = 0;
			}
			if (*slashptr) {
				slashchr = *slashptr;
				*slashptr = '\0';
			} else slashptr = (char *)NULL;

			/** see if escaped **/
			if ((result != dollarptr) && *(dollarptr - 1) == '\\') {
				/** replace \ with 0 and copy rest of name **/
				buffer[blen - 1] = '\0';
				strncat(buffer, dollarptr, MOD_BUFSIZE-blen);
				blen = strlen(buffer);
				if(brace)
					strncat(buffer,"}",MOD_BUFSIZE-blen-1);
			} else {
				if (! strcmp(dollarptr + 1 + brace, "$")) {
					/** put in the process pid **/
					pid = getpid();
					sprintf(buffer + blen,"%ld",(long)pid);
				} else {
					/** get env.var. value **/
					envvar = getenv(dollarptr + 1 + brace);

					/** cat value to rest of string **/
					if (envvar) strncat(buffer,envvar,
						MOD_BUFSIZE-blen-1);
				}
			}
			blen = strlen(buffer);

			/** start at slashptr and find next $ **/
			if (slashptr) {
				*slashptr = slashchr;
				dollarptr = strchr(slashptr, '$');
				/** copy everything upto $ **/
				if (dollarptr) *dollarptr = '\0';
				strncat(buffer, slashptr + brace,
					MOD_BUFSIZE -blen -1);
				if (dollarptr) {
					*dollarptr = '$';
					strncpy(oldbuffer, buffer, MOD_BUFSIZE);
				}
			} else {		/** no more to show **/
				dollarptr = (char *)NULL;
			}
		}
		null_free((void *) &result);
		return stringer(NULL,0, buffer, NULL);
	}

} /** End of 'xdup' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		xgetenv						     **
 ** 									     **
 **   Description:	will return an expanded environment variable.	     **
 ** 			However, it will only expand 1 level.		     **
 ** 			See xdup() for details.				     **
 ** 									     **
 **   First Edition:	2000/01/18	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	char	*var		Environment variable	     **
 ** 									     **
 **   Result:		char    *		An allocated string	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char *xgetenv(char const * var) {
	char *result = NULL;

	if (!var) return result;

	return xdup(getenv(var));

} /** End of 'xgetenv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		EscapeCshString(char* in,char* out)                  **
 ** 									     **
 **   Description:	will translate input string to escaped output string **
 **                     out must be allocated first                          **
 ** 									     **
 **   First Edition:	2002/04/10					     **
 ** 									     **
 **   Parameters:	char	*in		input            	     **
 **              	char	*out		output               	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void EscapeCshString(
	const char *in,
	char *out
) {
	while (*in) {
		if (*in == ' ' ||
		    *in == '\t'||
		    *in == '\\'||
		    *in == '{' ||
		    *in == '}' ||
		    *in == '|' ||
		    *in == '<' ||
		    *in == '>' ||
		    *in == '!' ||
		    *in == ';' ||
		    *in == '#' ||
		    *in == '$' ||
		    *in == '^' ||
		    *in == '&' ||
		    *in == '*' ||
		    *in == '\''||
		    *in == '"' ||
		    *in == '(' ||
		    *in == ')') {
			*out++ = '\\';
		}
		*out++ = *in++;
	}
	*out = 0;
}

void EscapeShString(
	const char *in,
	char *out
) {
	while (*in) {
		if (*in == ' ' ||
		    *in == '\t'||
		    *in == '\\'||
		    *in == '{' ||
		    *in == '}' ||
		    *in == '|' ||
		    *in == '<' ||
		    *in == '>' ||
		    *in == '!' ||
		    *in == ';' ||
		    *in == '#' ||
		    *in == '$' ||
		    *in == '^' ||
		    *in == '&' ||
		    *in == '*' ||
		    *in == '\''||
		    *in == '"' ||
		    *in == '(' ||
		    *in == ')') {
			*out++ = '\\';
		}
		*out++ = *in++;
	}
	*out = 0;
}

void EscapePerlString(
	const char *in,
	char *out
) {

	while (*in) {
		if (*in == '\\'||
		    *in == ';' ||
		    *in == '\'') {
			*out++ = '\\';
		}
		*out++ = *in++;
	}
	*out = 0;
}

/* I think this needs a bunch of work --NJW */
void EscapeCmakeString(const char* in,
		     char* out) {
  for(;*in;in++) {
    if (*in == '\\'||
	*in == '"') {
      *out++ = '\\';
    }
    *out++ = *in;
  }
  *out = 0;
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		tmpfile_mod                                          **
 ** 									     **
 **   Description:	emulates tempnam  and tmpnam  and mktemp             **
 **                     Atomically creates a unique temp file and opens it   **
 **                     for writing. returns 0 on success, 1 on failure      **
 **                     Filename and file handle are returned through        **
 **                     argument pointers                                    **
 ** 									     **
 **   First Edition:	2002/04/22					     **
 ** 									     **
 **   Parameters:	char	**filename	pointer to char* 	     **
 **              	char	**file          pointer to FILE*     	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int tmpfile_mod(char** filename, FILE** file) {
  char* filename2;
  int trial = 0;

  if (!(filename2 =
	 stringer(NULL, strlen(TMP_DIR)+strlen("modulesource")+20, NULL)))
     if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	 return 1;
  
  do {
    int fildes;

    sprintf(filename2,"%s/modulesource_%d",TMP_DIR,trial++);
    fildes = open(filename2,O_WRONLY | O_CREAT | O_EXCL | O_TRUNC,0755);
#if 0
    fprintf(stderr,"DEBUG: filename=%s fildes=%d\n",
	   filename2,fildes);
#endif
    if (fildes >=0) {
      *file = fdopen(fildes,"w");
      *filename = filename2;
      return 0;
    }
  } while (trial < 1000);

  null_free((void *) &filename2);
  fprintf(stderr,
	_("FATAL: could not get a temp file! at %s(%d)"),__FILE__,__LINE__);
  
  return 1;
}


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ReturnValue					     **
 ** 									     **
 **   Description:	Handles the various possible return values	     **
 ** 									     **
 **   first edition:	2006/02/13	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	Tcl_Interp      *interp		Attached Tcl Interp. **
 **			int		retval		Return value to check**
 ** 									     **
 **   Result:		EM_RetVal		Limited set		     **
 ** 									     **
 **   Attached Globals:	g_retval	set to N if "exit N"		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

EM_RetVal ReturnValue(Tcl_Interp *interp, int retval) {
	EM_RetVal	 em_result;
	char		*startp		= (char *) NULL,
			*endp		= (char *) NULL;
	const char 	*tstr;
	int		 result;
	static Tcl_RegExp	exit__expPtr,
				break_expPtr,
				continue_expPtr;

	tstr = (const char *) TCL_RESULT(interp);

	/* Tcl caches upto 30 compiled regexps ... lost ones will be
	 * recompiled
	 */
	/* compile regular expression the first time through */
	if (!exit__expPtr)
		exit__expPtr = Tcl_RegExpCompile(interp, "^EXIT ([0-9]*)");

	/*  result = "invoked \"break\" outside of a loop" */
	if (!break_expPtr)
		break_expPtr = Tcl_RegExpCompile(interp, ".*\"break\".*");

	/*  result = "invoked \"continue\" outside of a loop" */
	if (!continue_expPtr)
		continue_expPtr = Tcl_RegExpCompile(interp, ".*\"continue\".*");

	/* intercept any "EXIT N" first */
	if(tstr && *tstr && 0 < Tcl_RegExpExec(interp, exit__expPtr,
		(CONST84 char *) tstr, (CONST84 char *) tstr)){
		/* found 'EXIT' */
		Tcl_RegExpRange(exit__expPtr, 1,
			(CONST84 char **) &startp, (CONST84 char **) &endp);
		if( startp != '\0')
			result = atoi((const char *) startp);

		g_retval = result;
		em_result = EM_EXIT;

	/* check for a break not within loop */
	} else if(tstr && *tstr && 0 < Tcl_RegExpExec(interp, break_expPtr,
		(CONST84 char *) tstr, (CONST84 char *) tstr)){
		em_result = EM_BREAK;

	/* check for a continue not within loop */
	} else if(tstr && *tstr && 0 < Tcl_RegExpExec(interp, continue_expPtr,
		(CONST84 char *) tstr, (CONST84 char *) tstr)){
		em_result = EM_CONTINUE;

	} else {
		switch (retval) {
		case TCL_OK:
			em_result = EM_OK;
			break;
		case TCL_BREAK:
			em_result = EM_BREAK;
			break;
		case TCL_CONTINUE:
			em_result = EM_CONTINUE;
			break;
		case TCL_ERROR:
		default:
			em_result = EM_ERROR;
			break;
		}
	}
	return em_result;
} /** End of 'ReturnValue' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		OutputExit					     **
 ** 									     **
 **   Description:	Outputs a 'test 0 = 1' line so command will eval     **
 ** 			with a non-zero	exit code			     **
 ** 									     **
 **   first edition:	2006/03/07	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	void			none			     **
 ** 									     **
 **   result:		void    		(nothing)		     **
 ** 									     **
 **   Attached Globals:	g_retval	if non-zero			     **
 **   			g_output	if non-zero			     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
void OutputExit() {

	if (!shell_derelict) {
		return;
	} else if( !strcmp( shell_derelict, "csh")) {
		/* OK shell derelict */
	} else if( !strcmp( shell_derelict, "sh")) {
		/* OK shell derelict */
	} else {
		return;
	}
	if (g_retval) {
		fprintf( stdout, " test 0 = 1;");
	}
} /** End of 'OutputExit' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		EMGetEnv					     **
 ** 									     **
 **   Description:	Wrap the Tcl_GetVar2() call and return an allocated  **
 ** 			string						     **
 ** 									     **
 **   first edition:	2011/08/15	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	Tcl_Interp	 *interp	 TCL interp.	     **
 **   			char	*var		Environment variable	     **
 ** 									     **
 **   Result:		char    *		An allocated string	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
char * EMGetEnv(	Tcl_Interp	 *interp, 
			char const	 *var) {

	char const *value, *string;

	Tcl_Preserve(interp);
	value = Tcl_GetVar2( interp, "env", var, TCL_GLOBAL_ONLY);
	Tcl_Release(interp);
	string = stringer(NULL, 0, (char *) value, NULL);

	if (!string)
		if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
			return (NULL);		/** ---- EXIT (FAILURE) ---> **/

	return (char *) string;

} /** End of 'EMGetEnv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		EMSetEnv					     **
 ** 									     **
 **   Description:	Wrap the Tcl_SetVar2() call and return an allocated  **
 ** 			string						     **
 ** 									     **
 **   first edition:	2011/09/26	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	Tcl_Interp	 *interp	 TCL interp.	     **
 **   			char	*var		Environment variable	     **
 **   			char	*val		New value		     **
 ** 									     **
 **   Result:		char    *		current value string	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
char const * EMSetEnv(	Tcl_Interp	 *interp, 
			char const	 *var,
			char const	 *val) {

	char const *value;

	Tcl_Preserve(interp);
	value = Tcl_SetVar2( interp, "env", var, val, TCL_GLOBAL_ONLY);
	Tcl_Release(interp);

	return value;

} /** End of 'EMSetEnv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		is_					     	     **
 ** 									     **
 **   Description:	Does a stat on the path and returns 0 if not X	     **
 **			else 1 if X, where X is <f>ile, or <d>ir, etc.	     **
 **			or returns -1 if the path does not exist.	     **
 **			If X is <w>hat, then return 1 if file, 2 if dir	     **
 ** 									     **
 **   First Edition:	2009/09/21					     **
 ** 									     **
 **   Parameters:	char	*path		Name of the object to be     **
 **						checked			     **
 ** 									     **
 **   Result:		int	 0	Not an X			     **
 **				 1	OK	if X			     **
 ** 									     **
 **				 0	does not exist if "what"	     **
 **				 1	file	if "what"		     **
 **				 2	dir	if "what"		     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

is_Result is_(
	const char *type,
	const char *path
) {
	struct stat	stats;		/** stat() system call buffer	     **/

	/* stat path */
	if (stat(path, &stats)) {
		if (*type == 'w' || *type == 'W')
			return IS_NOT;		/** does not exist even	     **/
	} else {
		if (*type == 'f' || *type == 'F') {
			if (S_ISREG(stats.st_mode))
				return IS_SO;
		} else if (*type == 'd' || *type == 'D') {
			if (S_ISDIR(stats.st_mode))
				return IS_SO;
		} else if (*type == 'w' || *type == 'W') {
			if (S_ISREG(stats.st_mode))
				return IS_FILE;
			if (S_ISDIR(stats.st_mode))
				return IS_DIR;
		}
	}
	return IS_NOT;
} /** is_ **/
