/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdVersion.c					     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM,			     **
 **		R.K. Owen, rk@owen.sj.ca.us				     **
 ** 									     **
 **   Description:	The Tcl module-version routine which provides the    **
 **			definition of symbolic version names and the module- **
 **			alias command providing the definition of module and **
 **			version aliases					     **
 ** 									     **
 **   Exports:		cmdModuleVersion				     **
 **			cmdModuleAlias					     **
 **			InitVersion					     **
 **			CleanupVersion					     **
 **			AliasLookup					     **
 **			ExpandVersions					     **
 ** 									     **
 **   Notes:	This module defines the callback functions for the defi-     **
 **		nition of symbolic module names and module aliases. The      **
 **		syntax of the according commands is defined as:		     **
 ** 									     **
 **	    Module-Versions:						     **
 **		module-version [/]<version> <name> [ <name> ... ]	     **
 **		module-version <alias> <name> [ <name> ... ]		     **
 ** 									     **
 **	    Module-Alias:						     **
 **		module-alias <alias> <string>				     **
 ** 									     **
 ** ************************************************************************ **
 ****/

/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1991-1994 by John L. Furlan.                      	     **
 ** see LICENSE.GPL, which must be provided, for details		     **
 ** 									     ** 
 ** ************************************************************************ **/

static char Id[] = "@(#)$Id: cmdVersion.c,v 1.5 2002/08/14 21:06:00 lakata Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"
#include "avec.h"
#include "list.h"
#include "uvec.h"

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/** ************************************************************************ **/
/**									     **/
/**   The whole thing is handled in memory. The structure is build of module **/
/**   and name records. There are 3 types of name records: version, name     **/
/**   and alias. 							     **/
/**									     **/
/**	 modlist					     aliaslist	     **/
/**         X                                                    X	     **/
/**     +---+---+                                            +---+---+	     **/
/**     | module| ------------------------+                  | alias |	     **/
/**     +---+---+ --------+               |                  +-------+	     **/
/**         X   ^    +----+----+          X                      X	     **/
/**         |        | version |       +--+--+               +---+---+	     **/
/**         |        +----+----+       | name|               | alias |	     **/
/**         |             |            +--+--+               +---+---+	     **/
/**         |        +----+----+          X                      X	     **/
/**         |        | version |       +--+--+               +---+---+	     **/
/**         |        +----+----+<-+--- | name|               | alias |	     **/
/**         |             |       |    +--+--+               +---+---+	     **/
/**         |        +----+----+  |       X                      X	     **/
/**         |        | version |  |    +--+--+               +---+---+	     **/
/**         |        +----+----+  | +--| name|               | alias |	     **/
/**         |             |       | |  +--+--+               +---+---+	     **/
/**         |                     | |     X                      X	     **/
/**         X                     | +->+--+--+               +---+---+	     **/
/**     +---+---+                 +--- | name|               | alias |	     **/
/**     | module|                      +--+--+               +---+---+	     **/
/**     +---+---+                         X                      X	     **/
/**         X								     **/
/**	hash ordered	         hash ordered           hash ordered	     **/
/**	list of module	    list of names depending    list of aliases	     **/
/**	paths		    to a single module file			     **/
/**									     **/
/**   Each module name points to a list of symbolic names and versions.	     **/
/**   The versions themselves can be symbolic names and therefore are of the **/
/**   same record type as the names.					     **/
/**   The name and the version list is alphabetically sorted (even the       **/
/**   module list is). A version record points to a related name record	     **/
/**   containing a symbolic name for the version. Starting at this record,   **/
/**   the name records built a queue of symbolic names for the version.	     **/
/**   Both, the version and the name record do have a backward pointer to    **/
/**   the module record.						     **/
/**									     **/
/**   The alias list is an associative array where the alias is a substitute **/
/**   string, which may contain other aliases.  An alias string	can contain  **/
/**   a path, a module name, a version, or any sensible combination of each. **/
/**									     **/
/** ************************************************************************ **/

/* modules - the key is the module path */
typedef	struct	_elem_module	{
    uvec		*versions;	/** version queue   		     **/
    avec		*names2vers;	/** name queue      		     **/
    char		*module;	/** the pathless name itself	     **/
    char		*deflt;		/** if a designated default	     **/
} ElemModule;

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

#define	HISTTAB	100

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "cmdVersion.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdModuleVersion[] = "cmdModuleVersion";
static	char	_proc_cmdModuleAlias[] = "cmdModuleAlias";
#endif
#if WITH_DEBUGGING_UTIL_2
static	char	_proc_InitVersion[] = "InitVersion";
static	char	_proc_CleanupVersion[] = "CleanupVersion";
#endif
#if WITH_DEBUGGING_UTIL_1
static	char	_proc_AddModule[] = "AddModule";
static	char	_proc_FindModule[] = "FindModule";
static	char	_proc_AddName[] = "AddName";
static	char	_proc_FindName[] = "FindName";
#endif

/**
 **  The module and aliases list
 **/

static	avec		*modlist   = (avec *) NULL;
static	avec		*aliaslist = (avec *) NULL;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	ElemModule	*FindModule(	const	char	 *pathmodule);

static	char		*FindName(	ElemModule	 *modelem,
					const	char	 *name);

#if 0
static	void		 CleanupVersionSub(	ModModule	 *ptr);

static	void		 CleanupName(		ModName		 *ptr);

static	ModModule	*AddModule(		const	char	 *path,
						const	char	 *name);

static	ModName		*AddName(		const	char	 *name,
						ModName		**start,
						ModModule	 *module);

static	char		*CheckModuleVersion(	const	char 	 *name);

static	char		*scan_versions(		char	 	 *buffer,
						const	char	 *base,
						ModName 	 *ptr,
						ModModule 	 *modptr);
#endif
/**
 ** these functions are only to aid in interactive debugging
 **/
	void		dump_both();
	void		dump_modlist();
	void		dump_aliaslist();

/*++++
 ** ** module-static functions ********************************************* **
 ** 									     **
 ** 									     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
/* ---------------------------------------------------------------------- */
/* wrappers for the StdC string functions - to handle names2versi
 */
static int n2v_add (void **data, va_list ap) {
	char const *str = va_arg(ap,char *);
	int retval = 0;
	if (!data) return -1;
	if (*data) {
		/* replace  current string */
		null_free(data);
		retval = 1;
	}
	if((*data = stringer(NULL,0,str, NULL))) {
		return retval;
	}
	return -2;
}

static int n2v_rm (void **data, va_list ap) {
	null_free((void *) data);
	return 0;
}

static avec_fns n2v_fns = {
	AVEC_USER,
	n2v_add,
	n2v_rm,
	n2v_rm,
};

/* module related static functions */
/* each module element is actually two lists (names & versions are only added)
 *
 * pass in the module modulepath version *names[] (NULL terminated vector)
 */
static	int	module_element_add(void **data, va_list ap) {
	char const *version = va_arg(ap,char *);
	char **name;
	ElemModule *em;
	int retval = 0;

	if (!data) return -1;	/* no module object */

	if (*data) {		/* add to existing module object */
		retval = 1;
	} else if (!(*data = calloc(1,sizeof(ElemModule)))) {
		return -2;	/* memory error */
	} else {
		em = (ElemModule *) *data;
		if (!(em->versions = uvec_ctor(5))) {
			return -3;	/* memory error */
		}
		if (!(em->names2vers = avec_ctor_(11,n2v_fns))) {
			return -4;	/* memory error */
		}
	}
	em = (ElemModule *) *data;
	/* add on version to list */
	if (0> uvec_push(em->versions, version)) {
		return -5;		/* some insert error */
	}
	/* get list of names and add to name list mapped to the version */
	name = va_arg(ap,char **);
	while (*name) {
		if (0 > avec_insert(em->names2vers, *name, version))
			return -6;	/* some insert error */
		name++;
	}
	return retval;
}

static	int	module_element_rm(void **data, va_list ap) {

	if (!data) return -1;				/* no element object */
	/* destroy element */
	uvec_dtor(&(((ElemModule *) *data)->versions));
	avec_dtor(&(((ElemModule *) *data)->names2vers));
	null_free(data);
	return 0;
}

static avec_fns elem_module_fns = {
	AVEC_USER,
	module_element_add,
	module_element_rm,
	module_element_rm
};

static	void	moduleslist_init() {
	if (!modlist) {
		modlist = avec_ctor_(10,elem_module_fns);
		if (!modlist )
			ErrorLogger( ERR_ALLOC, LOC, NULL);
	}
}

static	void	moduleslist_final() {
	if (modlist) {
		if (avec_dtor(&modlist))
			ErrorLogger( ERR_ALLOC, LOC, NULL);
	}
}


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleVersion				     **
 ** 									     **
 **   Description:	Callback function for 'version'			     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successfull completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	modlist		List containing all version names    **
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_module_path	path to the module		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModuleVersion(	ClientData	 client_data,
	      			Tcl_Interp	*interp,
	      			int		 argc,
	      			char		*argv[])
{
    char	*version, *module;
    int 	 i;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModuleVersion, NULL);
#endif

    /**
     **  Whatis mode?
     **/

    if( g_flags & M_WHATIS) 
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
	
    /**
     **  Parameter check
     **/
    if( argc < 3) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], " modulename ",
	    " symbolic-version [symbolic-version ...] ", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    if( g_flags & M_LOAD) {		/** load **/
	moduleslist_init();
	if (avec_insert(modlist, g_module_path, argv[1], argv+2) < 0)
		if( OK != ErrorLogger( ERR_INTERAL, LOC, NULL))
		    return TCL_ERROR;
    }
#if 0
    if((char *) NULL == (module = CheckModuleVersion( argv[1]))) {
	ErrorLogger( ERR_BADMODNAM, LOC, argv[1], NULL);
	return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
#endif

    /**
     **  Display mode?
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	for( i=1; i<argc; i++)
	    fprintf( stderr, "%s ", argv[ i]);
	fprintf( stderr, "\n");
    }

#if 0
    /**
     **  get the version from the argument
     **/

    if((char *) NULL == (version = strrchr( module, '/'))) {
	if( OK != ErrorLogger( ERR_INTERAL, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
    *version++ = '\0';

    /**
     **  Now we have a module and a version.
     **  Check whether it exists (cond. create them). Check both, the version
     **  and the name queue in order to locate the desired version ...
     **/

    if((ElemModule *) NULL == (modptr = AddModule(g_module_path, module))) {
	ErrorLogger( ERR_BADMODNAM, LOC, argv[1], NULL);
	return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    if((ModName *) NULL == (ptr = FindName( version, modptr->version))) {
	if((ModName *) NULL == (ptr = FindName( version, modptr->name))) 
	    versptr = AddName( version, &modptr->version, modptr);
	else
	    versptr = ptr->version;
    } else
	versptr = ptr;

    /**
     **  Check all symbolic names now and allocate a name record for them
     **/
    
    for( i=2; i<argc; i++) {

	if( FindName( argv[ i], modptr->name)) {
	    if( OK != ErrorLogger( ERR_DUP_SYMVERS, LOC, argv[ i], NULL))
		break;
	    else
		continue;
	}

	if((ModName *) NULL == (nameptr = AddName( argv[ i], &modptr->name,
	    modptr))) {
	    if( OK != ErrorLogger( ERR_INTERAL, LOC, NULL))
		break;
	    else
		continue;
	}

	/**
	 **  Concat the new element at the beginning of the name queue ...
	 **/

	nameptr->ptr = versptr->ptr;
	versptr->ptr = nameptr;
	nameptr->version = versptr;
    }
#endif

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModuleVersion, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdModuleVersion' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ExpandVersions					     **
 ** 									     **
 **   Description:	Callback function for 'version'			     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	char	*name		Name to be expanded	     **
 ** 									     **
 **   Result:		char*	NULL		No symbols found	     **
 **				Otherwise	Pointer to the list string   **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char	*ExpandVersions( char	*name)
{
    char	*version, *module, *s;
    static char	 buffer[ BUFSIZ];
#if 0
    ElemModule	*modptr;
    ModName	*ptr, *tmp2;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModuleVersion, NULL);
#endif

    /**
     **  Parameter check
     **/

    if((char *) NULL == (module = CheckModuleVersion( name))) 
	return((char *) NULL );		/** -------- EXIT (FAILURE) -------> **/

    if((char *) NULL == (version = strrchr( module, '/'))) {
	if( OK != ErrorLogger( ERR_INTERAL, LOC, NULL))
	    return((char *) NULL );	/** -------- EXIT (FAILURE) -------> **/
    }

    *version++ = '\0';

    /**
     **  Now we have a module and a version.
     **  Check whether it exists
     **/

    if((ElemModule *) NULL == (modptr=FindModule(g_module_path, module)))
	return((char *) NULL );		/** -------- EXIT (FAILURE) -------> **/

    if((ModName *) NULL == (ptr = FindName( version, modptr->version))) {
	if((ModName *) NULL == (ptr = FindName( version, modptr->name))) 
	    return((char *) NULL );	/** -------- EXIT (FAILURE) -------> **/
	ptr = ptr->version;
    }

    if( !ptr->ptr)
	return((char *) NULL );		/** -------- EXIT (FAILURE) -------> **/

    /**
     **  Now scan in all the symbolic version names
     **/
 
    *buffer = '\0';
    if( s = scan_versions( buffer, buffer, ptr->ptr, modptr)) 
	*--s = '\0';			/** remove trailing ':'		     **/
#endif

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModuleVersion, NULL);
#endif

    return( buffer);

} /** End of 'ExpandVersions' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		scan_versions					     **
 ** 									     **
 **   Description:	Scan all symbolic versions pointed to be the passed  **
 **			ModName pointer and print them as a list into the    **
 **			passed buffer.					     **
 ** 									     **
 **   First Edition:	95/12/28					     **
 ** 									     **
 **   Parameters:	char		*buffer	Buffer for printing in	     **
 **			const	char	*base				     **
 **			ModName		*ptr	Name structure pointer	     **
 **			ElemModule	*modptr	Assigned module name	     **
 ** 									     **
 **   Result:		char*	NULL		Nothing printed into the     **
 **						buffer			     **
 **				Otherwise	Pointer to the end of the    **
 **						string in the buffer	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

#if 0
static	char	*scan_versions( char		 	*buffer,
				const	char		 *base,
				ModName			 *ptr,
				ElemModule		 *modptr)
{
    ModName     *tmp, *vers;
    char 	*s;

    /**
     **  Recursively print the queue of names
     **/

    if( !ptr)
	return((char *) NULL);			/** ------ EXIT (END) -----> **/

    if( !ptr->name) {
	if( OK != ErrorLogger( ERR_INTERAL, LOC, NULL))
	    return((char *) NULL);		/** ---- EXIT (FAILURE) ---> **/

    } else {

	/**
	 **  Prevent endless loops
	 **/

	if( strstr( base, ptr->name)) {
	    ErrorLogger( ERR_SYMLOOP, LOC, ptr->name, NULL);
	    return((char *) NULL);		/** ---- EXIT (FAILURE) ---> **/
	}

	/**
	 **  Now print ...
	 **/

	/* sprintf( buffer, "%s:", ptr->name); */
	strcpy( buffer, ptr->name);
	strcat( buffer, ":");
	buffer += strlen( buffer);

	/**
	 **  Check whether this is a version name again ...
	 **  This is a recursion, too
	 **/

	if((ModName *) NULL != (vers = FindName( ptr->name, modptr->version))) {
	    if( s = scan_versions( buffer, base, vers->ptr, modptr))
		buffer = s;
	}
    }

    /**
     **  This is the recursion. Preserve the buffer end pointer
     **/

    if( s = scan_versions( buffer, base, ptr->ptr, modptr))
	buffer = s;

    return( buffer);

} /** End of 'scan_versions' **/
#endif

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		CheckModuleVersion				     **
 ** 									     **
 **   Description:	Reduce the passed module name into a <mod>/<vers>    **
 **			string						     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	const	char	*name	name to be checked	     **
 ** 									     **
 **   Result:		char*	NULL		any error		     **
 **				Otherwise	Pointer to a <mod>/<vers>    **
 **						string			     **
 ** 									     **
 **   Attached Globals:	modlist		List containing all version names    **
 **			aliaslist	List containing all alises	     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

#if 0
static	char	*CheckModuleVersion(const char *name)
{
    static char	 buffer[ BUFSIZ];
    char	*s, *t;

    /**
     **  Check the first parameter and extract modulename and version
     **/

    if( '/' == *name) {			/** only the version specified	     **/

	/**
	 **  get the module name from the current module ...
	 **/

	if( !g_current_module)
	    return((char *) NULL);

	strcpy( buffer, g_current_module);
	if((char *) NULL == (t = strrchr( buffer, '/')))
	    t = buffer + strlen( buffer);
	*t++ = '/';
	*t = '\0';

	/**
	 **  The version has been specified as a parameter
	 **/

	if( s = strrchr( name, '/')) {
	    s++;
	} else {
	    ErrorLogger( ERR_INTERAL, LOC, NULL);
	    return((char *) NULL);
	}

	strcpy( t, s);

    } else {				/** Maybe an alias or a module	     **/

	strcpy( buffer, name);
	if( !strrchr( buffer, '/')) {

	    /**
	     **  Check whether this is an alias ...
	     **/

	    if( s = AliasLookup(buffer)) {

		/* sprintf( buffer, "%s/%s", s, t); */
		strcpy( buffer, s);
		strcat( buffer, "/");
		strcat( buffer, t);

	    } else {

		/**
		 **  The default version is being selected
		 **/

		t = buffer + strlen( buffer);
		if( '/' != *t)
		    *t++ = '/';
		strcpy( t, _default);
	    }
	}
    }

    /**
     **  Pass the buffer reference to the caller
     **/

    return( buffer);

} /** End of 'CheckModuleVersion' **/
#endif

/*++++
 ** ** alias-static functions ********************************************** **
 ** 									     **
 ** 									     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
/* alias related static functions */
/* each alias element is actually a stack of strings (uvec)
 * such that when an unload is performed the previous alias (if any)
 * can be returned.
 */
static	int	alias_element_add(void **data, va_list ap) {
	char const *str = va_arg(ap,char *);

	if (!data) return -1;	/* no alias object */
	if (uvec_exists((uvec *) *data)) {
		/* got something already, push on stack */
		if (uvec_push((uvec *) *data, str) < 0) return -2;
		return 1;
	}
	/* allocate and then push onto stack */
	if (!(*data = (void *) uvec_ctor(4))) return -3;
	if (uvec_push((uvec *) *data, str) < 0) return -4;
	return 0;
}

static	int	alias_element_del(void **data, va_list ap) {

	if (!data) return -1;				/* no alias object */
	if (!uvec_exists((uvec *) *data)) return -2;	/* no stack */
	/* pop off stack */
	if (uvec_number((uvec *) *data))		/* stack not empty */
		if (uvec_pop((uvec *) *data) < 0) return -3;
	return 1;
}

static	int	alias_element_rm(void **data, va_list ap) {

	if (!data) return -1;				/* no alias object */
	if (!uvec_exists((uvec *) *data)) return -2;	/* no stack */
	/* destroy stack */
	if (uvec_dtor((uvec **) data) < 0) return -3;	/* destroy stack */
	return 0;
}

static avec_fns alias_fns = {
	AVEC_USER,
	alias_element_add,
	alias_element_del,
	alias_element_rm
};

static	void	alias_init() {
	if (!aliaslist) {
		aliaslist = avec_ctor_(10,alias_fns);
		if (!aliaslist )
			ErrorLogger( ERR_ALLOC, LOC, NULL);
	}
}

static	void	alias_final() {
	if (aliaslist) {
		if (avec_dtor(&aliaslist))
			ErrorLogger( ERR_ALLOC, LOC, NULL);
	}
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleAlias					     **
 ** 									     **
 **   Description:	Callback function for 'alias'			     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successfull completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	aliaslist	List containing all alises	     **
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_module_path	path to the module		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModuleAlias(	ClientData	 client_data,
	      		Tcl_Interp	*interp,
	      		int		 argc,
	      		char		*argv[])
{
    char	*version, *module;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModuleAlias, NULL);
#endif

    /**
     **  Parameter check
     **/
    if( argc != 3) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], " aliasname ",
	    "string", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Whatis mode?
     **/
    if( g_flags & M_WHATIS) 
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t %s %s\n", argv[ 0], argv[ 1], argv[ 2]);
    }
	
    if( g_flags & M_LOAD) {		/** load **/
	alias_init();
	if (avec_insert(aliaslist, argv[1], argv[2]) < 0)
		if( OK != ErrorLogger( ERR_INTERAL, LOC, NULL))
		    return TCL_ERROR;
    } else {				/** unload **/
	if (avec_delete(aliaslist, argv[1], argv[2]) < 0)
		if( OK != ErrorLogger( ERR_INTERAL, LOC, NULL))
		    return TCL_ERROR;
    }

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModuleAlias, NULL);
#endif

    return( TCL_OK);

} /** End of 'cmdModuleAlias' **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		AliasLookup					     **
 ** 									     **
 **   Description:	Resolves a given alias to a  string.		     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	const char *alias	Name of the alias to be re-  **
 **						solved			     **
 ** 									     **
 **   Result:		const char *string	Success, value in the buffer **
 **						is valid		     **
 **				    NULL	Any error, or not found	     **
 ** 									     **
 **   Attached Globals:	aliaslist	List containing all alises	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

const char *	AliasLookup( const char *alias )
{
	char const *retval = (char const *) NULL;
	char const *talias = alias;
	uvec *alias_element;
	int cnt = 0;

	/* check aliases other aliases - let at most 5 levels of indirection */
	while (talias && *talias && cnt < 5
		&& (alias_element = (uvec *) avec_lookup(aliaslist, talias))) {
		cnt++;
	/* got something - return last item in stack */
		if (uvec_exists(alias_element) && uvec_number(alias_element)) {
		    talias = retval = (char const *)
			    uvec_vector(alias_element)[uvec_end(alias_element)];
		} else {
		    talias = retval = (char const *) NULL;
		}

	}

	return retval;

} /** End of 'AliasLookup' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		VersionLookup					     **
 ** 									     **
 **   Description:	Resolves a given alias to a module/version string    **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	char	*alias		Name of the alias to be      **
 **						resolved		     **
 **			char	**module	Buffer for the module name   **
 **			char	**version	Buffer for the module version**
 ** 									     **
 **   Result:		int	1		Success, value in the buffer **
 **						is valid		     **
 **				0		Any error, or not found	     **
 **   Attached Globals:	g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	VersionLookup(	char *name, const char **module, char **version)
{
    ElemModule	*mptr;
    char	*s, *t;
    static char  buffer[ BUFSIZ];
#if 0
    ModName	*vptr, *vtmp;
    ModName	**history;
    int		 histsize = 0, histndx = 0, i;
#endif

    /**
     **  Check whether this is an alias ...
     **  BTW: Alias lookups return the FQMN (full qualifed module name ;-)
     **/

    if( '/' == *name) {
	strcpy( buffer, g_current_module);
	if( s = strrchr( buffer, '/'))
	    *s = '\0';
	*module = buffer;
	*version = name + 1;
    
    } else {

	strcpy( buffer, name);
	*module = buffer;

	if((char *) NULL == (*version = strrchr( buffer, '/'))) {

	    /* see if an alias was passed */
	    if( s = (char *) avec_lookup(aliaslist, buffer)) {
		*module = s; *version = t;

	    } else
		*version = _default;

	} else 
	    *(*version)++ = '\0';
    }

    /**
     **  Look up modulename ...
     **  We call it success, if we do not find a registerd name.
     **  In this case <module>/<version> will be returned as passed.
     **/
    if((ElemModule *) NULL == (mptr = FindModule(g_module_path)))
	return( 1);			/** -------- EXIT (SUCCESS) -------> **/

#if 0
    /**
     **  This is for preventing from endless loops
     **/
    histsize = HISTTAB;
    histndx = 0;

    if((ModName **) NULL == (history = (ModName **) malloc( histsize * 
	sizeof( ModName *)))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	return( 0);			/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Now look up the version name. Check symbolic names first. If some-
     **  thing is found, check if the related version record itself relates
     **  to a name record ...
     **/
    while( 1) {

	/**
	 **  Check the symbolic names ...
	 **/
	if((ModName *) NULL != (vptr = FindName( *version, mptr->name))){
	    if( !vptr->version || !vptr->version->name) {
		if( OK != ErrorLogger( ERR_INTERAL, LOC, NULL)) 
		    *version = (char *) NULL;
		break;
	    }

	    *version = vptr->version->name;

	    /**
	     **  Prevent from looping ...
	     **/
	    for( i=0; i<histndx; i++) {
		if( history[ i] == vptr) {		/** That's the loop  **/
		    ErrorLogger( ERR_SYMLOOP, LOC, *version, NULL);
		    *version = (char *) NULL;
		    break;
		}
	    }

	    if( !*version)
		break;

	    if( histndx >= histsize) {
		histsize += HISTTAB;

		if((ModName **) NULL == (history = (ModName **) realloc(
		    history, histsize * sizeof( ModName *)))) {
		    ErrorLogger( ERR_ALLOC, LOC, NULL);
		    return( 0);		/** -------- EXIT (FAILURE) -------> **/
		}
	    }

	    history[ histndx++] = vptr;

	} else {
	    break;

	} /** if( FindName) **/
    } /** while( 1) **/

    /**
     **  Free the loop preventing list
     **  If version is NULL now, something went wrong in the lookup loop above
     **/
    null_free((void *) &history);
    return((char *) NULL != *version);
#endif

} /** End of 'VersionLookup' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		InitVersion					     **
 ** 									     **
 **   Description:	Initialize the version and alias structure	     **
 ** 									     **
 **   First Edition:	2002/06/23					     **
 ** 									     **
 **   Parameters:	-						     **
 ** 									     **
 **   Result:		int		returns 0 on success		     **
 ** 					else != 0 if failure		     **
 ** 									     **
 **   Attached Globals:	modlist		List containing all version names    **
 **			aliaslist	List containing all alises	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	InitVersion(void)
{
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_InitVersion, NULL);
#endif

} /** End of 'InitVersion' **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		CleanupVersion					     **
 ** 									     **
 **   Description:	Cleanup the version structure			     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	-						     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	modlist		List containing all version names    **
 **			aliaslist	List containing all alises	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

#if 0
void	CleanupVersion(ElemModule *ptr)
{
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_CleanupVersion, NULL);
#endif

    CleanupVersionSub( modlist);
    modlist = (ElemModule *) NULL;

    CleanupName( aliaslist);
    aliaslist = (ModName *) NULL;

} /** End of 'CleanupVersion' **/

static void	CleanupVersionSub( ElemModule *ptr)
{
    /**
     **  Recursion
     **/

    if( !ptr)
	return;

    CleanupVersion( ptr->next);

    /**
     **  Cleanup everything that relates to this record
     **/

    CleanupName( ptr->version);
    CleanupName( ptr->name);
    null_free((void *) &(ptr->module));

} /** End of 'CleanupVersionSub' **/

static void	CleanupName( ModName *ptr)
{
    /**
     **  Recursion
     **/

    if( !ptr)
	return;

    CleanupName( ptr->next);

    /**
     **  Cleanup everything that relates to this record
     **/

    null_free((void *) &(ptr->name));

} /** End of 'CleanupName' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		AddModule					     **
 ** 									     **
 **   Description:	Add a new entry to the modules queue		     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	const	char	*path	path to new module	     **
 **   			const	char	*name	name of the new module	     **
 ** 									     **
 **   Result:		ElemModule*	NULL	Any error                    **
 **					Else	Pointer to the new record    **
 ** 									     **
 **   Attached Globals:	modlist		List containing all version names    **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	ElemModule	*AddModule(	const char	*path,
					const char	*name)
{
    ElemModule	*app_ptr, *ptr;

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_AddModule, NULL);
#endif

    /**
     **  We do not trust in NULL module names
     **/
    if( !name || !*name)
	return((ElemModule *) NULL);

    /**
     **  Check if the module name already exists and save the 'prev' pointer
     **  for appending the new one.
     **/
    if( ptr = FindModule(path))
	return( ptr);

    /**
     **  Allocate a new guy
     **/
    if((ElemModule *) NULL == (ptr = (ElemModule *) malloc( sizeof(ElemModule)))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	return((ElemModule *) NULL);
    }

    /**
     **  Fill the path & name in and put it in the queue
     **/
    if((char *) NULL == (ptr->path = strdup( path))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	null_free((void *) &ptr);
	return((ElemModule *) NULL);
    }
    if((char *) NULL == (ptr->module = strdup( name))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	null_free((void *) &(ptr->path));
	null_free((void *) &ptr);
	return((ElemModule *) NULL);
    }

    if( app_ptr) {
	ptr->next = app_ptr->next;
	app_ptr->next = ptr;
    } else {
	ptr->next = modlist;
	modlist = ptr;
    }

    ptr->version = (ModName *) NULL;
    ptr->name = (ModName *) NULL;

    /**
     **  Pass back the pointer to the new entry
     **/
#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_AddModule, NULL);
#endif

    return( ptr);

} /** End of 'AddModule' **/

#endif
/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		FindModule					     **
 ** 									     **
 **   Description:	Find the modules element for the module fullpath     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	const	char	*pathmodule	path to new module   **
 ** 									     **
 **   Result:		ElemModule*	NULL	Any error or not found       **
 **					Else	Pointer to the record	     **
 ** 									     **
 **   Attached Globals:	modlist		List containing all version names    **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	ElemModule	*FindModule(	const	char	 *pathmodule)
{
    ElemModule *modelem = (ElemModule *) NULL;

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_FindModule, NULL);
#endif

    if (modlist && pathmodule)
	    modelem = avec_lookup(modlist, pathmodule);

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_FindModule, NULL);
#endif

    return modelem;

} /** End of 'FindModule' **/
#if 0

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		AddName						     **
 ** 									     **
 **   Description:	Add a new entry to the name queue		     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	const char *name	Name of the new entry	     **
 **			ModName	**start		Start of the queue	     **
 **			ElemModule *module	Parent module record pointer **
 ** 									     **
 **   Result:		ModName*	NULL	Any error                    **
 **					Else	Pointer to the new record    **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	ModName	*AddName(	const	char	 *name,
				ModName		**start,
				ElemModule	 *module)
{

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_AddName, NULL);
#endif

    /**
     **  Check if the name already exists and save the 'prev' pointer
     **  for appending the new one.
     **/

    if( ptr = FindName( name, *start))
	return( ptr);

    /**
     **  Allocate a new guy
     **/

    if((ModName *) NULL == (ptr = (ModName *) malloc( sizeof(ModName)))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	return((ModName *) NULL);
    }

    /**
     **  Fill the name in and put it in the queue
     **/

    if((char *) NULL == (ptr->name = strdup( name))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	null_free((void *) &ptr);
	return((ModName *) NULL);
    }

    if( app_ptr) {
	ptr->next = app_ptr->next;
	app_ptr->next = ptr;
    } else {
	ptr->next = *start;
	*start = ptr;
    }

    ptr->module = module;
    ptr->version = ptr->ptr = (ModName *) NULL;

    /**
     **  Pass back the pointer to the new entry
     **/

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_AddName, NULL);
#endif

    return( ptr);

} /** End of 'AddName' **/
#endif

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		FindName					     **
 ** 									     **
 **   Description:	Find the version associated with name for the given  **
 ** 			module element.					     **
 ** 									     **
 **   First Edition:	1995/12/28					     **
 ** 									     **
 **   Parameters:	ElemModule  *modelem	module elem with name queue  **
 **   Parameters:	char	    *name	Name of be found	     **
 ** 									     **
 **   Result:		char*		NULL	Any error or not found       **
 **					Else	version that name points to  **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	char		*FindName(	ElemModule	 *modelem,
					const	char	 *name)
{
    char	*ver = (char *) NULL;
    char const	*alias;

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_FindName, NULL);
#endif

    /* assume name is an alias, find the actual name */
    if (modelem) {
        if ((alias = AliasLookup(name)))
		name = alias;
	ver = avec_lookup(modelem->names2vers, name);
    }

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_FindName, NULL);
#endif

    return ver;

} /** End of 'FindName' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		dump_*						     **
 ** 									     **
 **   Description:	dumps the contents of the modules & alias lists	     **
 ** 			only for interactive debugging, e.g.		     **
 ** 			 (gdb) p dump_both()				     **
 ** 									     **
 **   First Edition:	2002/06/20					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void	dump_both() {
	fprintf(stderr,
	"==========================================================\n");
	fprintf(stderr,"modlist:\n");
	fprintf(stderr,
	"----------------------------------------------------------\n");
	dump_modlist();
	fprintf(stderr,
	"==========================================================\n");
	fprintf(stderr,"aliaslist:\n");
	fprintf(stderr,
	"----------------------------------------------------------\n");
	dump_aliaslist();
	fprintf(stderr,
	"==========================================================\n");
}

static void	lindent(int l) {
	char deep[11]="\t\t\t\t\t\t\t\t\t\t";
	l = (l > 10 ? 10 : l);
	fprintf(stderr,"%s", deep + 10 - l);
}

#if 0
static void	dump_version_list(int l, struct _mod_module *m) {
	ModName *vptr;
	int i = 1;
	if (m) {
		vptr = m->version;
		lindent(l++);fprintf(stderr,"Module Versions\n");
		while (vptr) {
			lindent(l);fprintf(stderr,"V[%d]\n", i);
			dump_struct_mod_name(l,vptr);
			i++;
			vptr = vptr->ptr;
		}
	}
}

static void	dump_name_list(int l, struct _mod_module *m) {
	ModName *nptr;
	int i = 1;
	if (m) {
		nptr = m->name;
		lindent(l++);fprintf(stderr,"Module Names\n");
		while (nptr) {
			lindent(l);fprintf(stderr,"N[%d]\n", i);
			dump_struct_mod_name(l,nptr);
			i++;
			nptr = nptr->ptr;
		}
	}
}
#endif

void	dump_modlist() {
	avec *mptr = modlist;
	avec_element **ae = NULL;
	avec_element **aee = NULL;
	ElemModule *em;
	int level = 0;
	int entry = 1;
	if (modlist) {
		while(ae = avec_walk_r(modlist, ae)) {
			em = (ElemModule *) AVEC_DATA(*ae);
			fprintf(stderr,"module: '%s'=>\n",AVEC_KEY(*ae));
			fprintf(stderr,"<%d> '%s'\n",
				uvec_number(em->versions),
				uvec2str(em->versions,":"));

			entry = 1;
			aee = NULL;
			fprintf(stderr,"names->versions =\n");
			while(aee = avec_walk_r(em->names2vers, aee)) {
				fprintf(stderr,"<%d> '%s' => '%s'\n", entry,
					AVEC_KEY(*aee),
					AVEC_DATA(*aee));
				entry++;
			}
		}
	} else {
		fprintf(stderr,"(NULL)\n");
	}
}

void	dump_aliaslist() {
	avec_element **ae = NULL;
	int entry = 1;

	if (aliaslist) {
		while(ae = avec_walk_r(aliaslist, ae)) {
			fprintf(stderr,"<%d> '%s' => '%s'\n", entry,
				AVEC_KEY(*ae),
				uvec2str(AVEC_DATA(*ae),":"));
			entry++;
		}
	} else {
		fprintf(stderr,"(NULL)\n");
	}
}

#if 0
void	dump_struct_mod_module(int l,struct _mod_module *m) {
	lindent(l++); fprintf(stderr,"MODULE:%p: '%s' / '%s' \n",
			m, m->path, m->module);
	lindent(l); fprintf(stderr,"next   :%p",m->next);
	if (m->next)	fprintf(stderr," <- %s / %s\n",
			m->next->path, m->next->module);
	else		fprintf(stderr,"\n");
	lindent(l); fprintf(stderr,"version:%p",m->version);
	if (m->version)	fprintf(stderr," <- %s\n", m->version->name);
	else		fprintf(stderr,"\n");
	lindent(l); fprintf(stderr,"name   :%p\n",m->name);
}

void	dump_struct_mod_name(int l,struct _mod_name *n) {
	lindent(l++); fprintf(stderr,"NAME:%p: '%s' \n",n, n->name);
	lindent(l); fprintf(stderr,"next   :%p",n->next);
	if (n->next)	fprintf(stderr," <- %s\n", n->next->name);
	else		fprintf(stderr,"\n");
	lindent(l); fprintf(stderr,"ptr    :%p",n->ptr);
	if (n->ptr)	fprintf(stderr," <- %s\n", n->ptr->name);
	else		fprintf(stderr,"\n");
	lindent(l); fprintf(stderr,"version:%p",n->version);
	if (n->version)	fprintf(stderr," <- %s\n", n->version->name);
	else		fprintf(stderr,"\n");
	lindent(l); fprintf(stderr,"module :%p",n->module);
	if (n->module)	fprintf(stderr," <- %s / %s\n",
			n->module->path, n->module->module);
	else		fprintf(stderr,"\n");
}
#endif
