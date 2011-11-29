/*****
 ** ** Module Header ******************************************************* **
 **									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 **									     **
 **   File:		ModuleCmd_Whatis.c				     **
 **   First Edition:	1995/12/31					     **
 **									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
 **									     **
 **			ModuleCmd_Apropos				     **
 **									     **
 **   Notes:								     **
 **									     **
 ** ************************************************************************ **
 ****/

/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1991-1994 by John L. Furlan.                      	     **
 ** see LICENSE.GPL, which must be provided, for details		     **
 ** 									     ** 
 ** ************************************************************************ **/

static char Id[] = "@(#)$Id: ModuleCmd_Whatis.c,v 1.5.18.3 2011/11/29 15:48:58 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/**				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

/** ************************************************************************ **/
/**				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/**				     CONSTANTS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

#define WHATIS_SOME	0
#define WHATIS_ALL	1

/** ************************************************************************ **/
/**				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "ModuleCmd_Whatis.c";	/** File name of this module **/

#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Whatis[] = "ModuleCmd_Whatis";
static	char	_proc_ModuleCmd_Apropos[] = "ModuleCmd_Apropos";
#endif
#if WITH_DEBUGGING_UTIL_1
static	char	_proc_whatis_dir[] = "whatis_dir";
static	char	_proc_read_cache[] = "read_cache";
static	char	_proc_apropos_cache[] = "apropos_cache";
#endif

static	char	cache_name[] = APR_CACHE;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	int	 read_cache( int, char**, FILE*, int);
static	int	 whatis_dir( char*, int, char**, FILE*, int);
static	char	*apropos_cache(void);

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		ModuleCmd_Whatis				     **
 **									     **
 **   Description:	Display the passed modules 'whatis' information	     **
 **									     **
 **   First Edition:	1995/12/31					     **
 **									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument list	     **
 **									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 **									     **
 ** ************************************************************************ **
 ++++*/

int ModuleCmd_Whatis(	Tcl_Interp	*interp,
			int		 argc,
			char		*argv[])
{
    struct stat	 stats;
    Tcl_Interp	*whatis_interp;
    Tcl_DString	 cmdbuf;
    int		 i, result = TCL_OK, done = 0;
    char	 modulefile[ MOD_BUFSIZE],
		 modulename[ MOD_BUFSIZE],
		*modpath = (char *)NULL,/** Buffer for the contents of the   **/
					/** environment variable MODULEPATH  **/
		**wptr,
		*dirname = (char *)NULL,
		*cache_file = (char *) NULL;	/** Name of the cache file   **/
    FILE	*cachefp = (FILE *) NULL;	/** Cache file pointer	     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Whatis, NULL);
#endif

    /**
     **	 Initialize the command buffer and set up the modules flag to
     **	 'whatisonly'
     **/
    Tcl_DStringInit( &cmdbuf);
    g_flags |= M_WHATIS;

    /**
     **	 Handle each passed module file. Create a Tcl interpreter for each
     **	 module file to be handled and initialize it with custom module commands
     **/

    if ( argc) {
    /**
     **	 User provided a list of modules for ``whatis'' info
     **/
	for(i=0; i<argc && argv[i]; i++) {

	    whatis_interp = EM_CreateInterp();
	    if( TCL_OK != (result = InitializeModuleCommands( whatis_interp))) {
		EM_DeleteInterp( whatis_interp);
		result = TCL_ERROR;
		break;
	    }

	    /**
	     **	 locate the filename related to the passed module
	     **/
	    if( TCL_ERROR ==
		Locate_ModuleFile(whatis_interp,argv[i],modulename,modulefile)){
		EM_DeleteInterp( whatis_interp);
		if( OK != ErrorLogger( ERR_LOCATE, LOC, argv[i], NULL))
		    break;
		else
		    continue;
	    }

	    /**
	     **	 Print out everything that would happen if the module file were
	     **	 executed ...
	     **/
	    g_current_module = modulename;

	    cmdModuleWhatisInit();
	    result = CallModuleProcedure( whatis_interp, &cmdbuf, modulefile,
		"ModulesWhatis", 0);

	    /**
	     **	 Print the result ...
	     **/
	    if( whatis) {
		wptr = whatis;
		while( *wptr)
		    fprintf( stderr, "%-21s: %s\n", argv[i], *wptr++);
	    }

	    /**
	     **	 Remove the Tcl interpreter that has been used for printing ...
	     **/
	    EM_DeleteInterp( whatis_interp);
	    cmdModuleWhatisShut();

	} /** for **/
    } else {
       /**
	**  User wants all module ``whatis'' info
	**/

	/**
	 **  If no MODULEPATH defined, we can not output anything
	 **/
	if( !(modpath = (char *) xgetenv( "MODULEPATH")))
	    if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
		goto unwind0;

	/**
	 **  Check whether a cache file exists then list all the ``whatis'' info
	 **  Otherwise read all module files ...
	 **/
	cache_file = apropos_cache();
	if( !sw_create && cache_file && !stat( cache_file, &stats)) {

	    /**
	     **	 Open the cache file
	     **/
	    if((FILE *) NULL == (cachefp = fopen( cache_file, "r"))) {
		if( OK != ErrorLogger( ERR_OPEN, LOC, cache_file, NULL))
		    goto unwind1;
	
	    } else {
		
		/**
		 **  Read the cache and close the file
		 **/
		result = read_cache( argc, argv, cachefp, WHATIS_ALL);

		if( EOF == fclose( cachefp))
		    if( OK != ErrorLogger( ERR_CLOSE, LOC, cache_file, NULL))
			goto unwind1;

		done = 1;
	    }
	}

	/**
	 **  If we're not done now, we have to scan the files
	 **/
	 if( !done) {

	     /**
	      **  Open the cache file if neccessary
	      **/
	     if( sw_create && cache_file)
		 if((FILE *) NULL == (cachefp = fopen( cache_file, "w")))
		     if( OK != ErrorLogger( ERR_OPEN, LOC, cache_file, NULL))
			 goto unwind1;
		
	     /**
	      **  Tokenize the module path string and check all dirs
	      **/
	     for( dirname = xstrtok( modpath, ":");
		  dirname;
		  dirname = xstrtok( NULL, ":") ) {
	
		 if( !check_dir( dirname))
		     continue;
	
		 whatis_dir( dirname, argc, argv, cachefp, WHATIS_ALL);

	     } /** for **/

	     /**
	      **  Close the cache file
	      **/
	     if( cachefp)
		 if( EOF == fclose( cachefp))
		     if( OK != ErrorLogger( ERR_CLOSE, LOC, cache_file, NULL))
			 goto unwind1;
	 }
    }

    /**
     **	 Leave the 'whatis only mode', free up what has been used and return
     **/
    g_flags &= ~M_WHATIS;
    fprintf( stderr, "\n");

    Tcl_DStringFree( &cmdbuf);

    /**
     **	 Free up allocated resources
     **/
    null_free((void *) &cache_file);
    null_free((void *) &modpath);

    /**
     **	 Return on success
     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Whatis, NULL);
#endif

    return( result);			/** --- EXIT PROCEDURE (result)  --> **/

unwind1:
    null_free((void *) &cache_file);
    null_free((void *) &modpath);
unwind0:
    return( TCL_ERROR);			/** --- EXIT PROCEDURE (FAILURE) --> **/

} /** End of 'ModuleCmd_Whatis' **/

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		ModuleCmd_Apropos				     **
 **									     **
 **   Description:	Scan the whatis database in order to find something  **
 **			matching the passed strings			     **
 **									     **
 **   First Edition:	1995/12/31					     **
 **									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument list	     **
 **									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 **									     **
 **   Attached Globals:							     **
 **									     **
 ** ************************************************************************ **
 ++++*/

int ModuleCmd_Apropos(	Tcl_Interp	*interp,
			int		 argc,
			char		*argv[])
{
    struct stat	 stats;
    char	*dirname;
    char	*modpath;		/** Buffer for the contents of the   **/
					/** environment variable MODULEPATH  **/
    int		 i, done = 0;
    char	*c;
    char	*cache_file;		/** Name of the cache file	     **/
    FILE	*cachefp = (FILE *) NULL;	/** Cache file pointer	     **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Apropos, NULL);
#endif

    /**
     **	 Ignore case ... convert all arguments to lower case
     **/
    if( sw_icase)
	for( i=0; i<argc; i++)
	    for( c=argv[ i]; c && *c; c++)
		*c = tolower( *c);

    /**
     **	 If there's no MODULEPATH defined, we cannot output anything
     **/
    if((char *) NULL == (modpath =  xgetenv( "MODULEPATH")))
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    goto unwind0;

    /**
     **	 Check whether	there's a cache file. If it is, grep for the tokens
     **	 in this file. Otherwise read all module files ...
     **/
    cache_file = apropos_cache();
    if( !sw_create && cache_file && !stat( cache_file, &stats)) {

	/**
	 **  Open the cache file
	 **/
	if((FILE *) NULL == (cachefp = fopen( cache_file, "r"))) {
	    if( OK != ErrorLogger( ERR_OPEN, LOC, cache_file, NULL))
		goto unwind1;
	
	} else {
		
	    /**
	     **	 Read the cache and close the file
	     **/
	    read_cache( argc, argv, cachefp, WHATIS_SOME);

	    if( EOF == fclose( cachefp))
		if( OK != ErrorLogger( ERR_CLOSE, LOC, cache_file, NULL))
		    goto unwind1;

	    done = 1;
	}
    }

    /**
     **	 If we're not done now, we have to scan the files
     **/
    if( !done) {

	/**
	 **  Open the cache file if neccessary
	 **/
	if( sw_create && cache_file)
	    if((FILE *) NULL == (cachefp = fopen( cache_file, "w")))
		if( OK != ErrorLogger( ERR_OPEN, LOC, cache_file, NULL))
		    goto unwind1;

	/**
	 **  Tokenize the module path string and check all dirs
	 **/
	for( dirname = xstrtok( modpath, ":");
	     dirname;
	     dirname = xstrtok( NULL, ":") ) {
	
	    if( !check_dir( dirname))
		continue;
	
	    whatis_dir( dirname, argc, argv, cachefp, WHATIS_SOME);

	} /** for **/

	/**
	 **  Close the cache file
	 **/
	if( cachefp)
	    if( EOF == fclose( cachefp))
		if( OK != ErrorLogger( ERR_CLOSE, LOC, cache_file, NULL))
		    goto unwind1;
    }

    /**
     **	 Free up what has been allocated and exit from this procedure
     **/
    null_free((void *) &modpath);

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Apropos, NULL);
#endif

    return( TCL_OK);

unwind1:
    null_free((void *) &modpath);
unwind0:
    return( TCL_ERROR);				/** ---- EXIT (FAILURE) ---> **/

} /** End of 'ModuleCmd_Apropos' **/

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		whatis_dir					     **
 **									     **
 **   Description:	Print all files beyond the passed directory	     **
 **									     **
 **   First Edition:	1991/10/23					     **
 **									     **
 **   Parameters:	char	*dir		Directory to be scanned	     **
 **			int	 argc		Number of tokens	     **
 **			char   **argv		List of tokens to check	     **
 **			FILE	*cfp		Cache file pointer	     **
 **									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 **									     **
 ** ************************************************************************ **
 ++++*/

static	int	whatis_dir( char *dir, int argc, char **argv, FILE *cfp,
			    int whatis_list)
{
    fi_ent	 *dirlst_head = NULL;	/** Directory list base pointer	     **/
    int		  count = 0;		/** Number of elements in the top    **/
					/** level directory list	     **/
    int		  tcount = 0;		/** Total number of files to print   **/
    char	**list;			/** flat list of module files	     **/
    int		  start = 0, i, k;
    int		  result = TCL_OK;
    Tcl_Interp	 *whatis_interp;
    Tcl_DString	  cmdbuf;
    char	  modulefile[ MOD_BUFSIZE];
    char	**wptr, *c;
    struct stat	 stats;

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_whatis_dir, "dir='", dir, NULL);
#endif

    /**
     **	 Normal reading of the files
     **/

    if( NULL == (dirlst_head = get_dir( dir, NULL, &count, &tcount)))
	if( OK != ErrorLogger( ERR_READDIR, LOC, dir, NULL))
	    goto unwind0;

    if( NULL == (list = (char**) module_malloc( tcount * sizeof( char**))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind1;

    dirlst_to_list( list, dirlst_head, count, &start, NULL, NULL);

    /**
     **	 Initialize the command buffer and set up the modules flag to 'whatislay
     **	 only'
     **/

    Tcl_DStringInit( &cmdbuf);
    g_flags |= M_WHATIS;

    /**
     **	 Check all the files in the flat list for the passed tokens
     **/

    for( i=0; i<tcount; i++) {

	whatis_interp = EM_CreateInterp();
	if( TCL_OK != (result = InitializeModuleCommands( whatis_interp))) {
	    EM_DeleteInterp( whatis_interp);
	    result = TCL_ERROR;
	    break; /** for( i) **/
	}

	/**
	 **  locate the filename related to the passed module
	 **/

	if( (char *) NULL == stringer(modulefile,MOD_BUFSIZE,
		dir,"/",list[i],NULL)) {
	    result = TCL_ERROR;
	    break; /** for( i) **/
	}
	g_current_module = list[ i];

	if( stat( modulefile, &stats) || S_ISDIR( stats.st_mode))
	    continue;

	cmdModuleWhatisInit();
	result = CallModuleProcedure( whatis_interp, &cmdbuf, modulefile,
	    "ModulesApropos", 0);

	/**
	 **  Check if at least one of the passed tokens is found in the
	 **  retrieved whatis strings. If yes, print the string.
	 **/

	if( whatis) {
	    wptr = whatis;
	    while( *wptr) {

		/**
		 **  Cache output enabled?
		 **/

		if( cfp)
		    fprintf( cfp, "%-21s: %s\n", list[i], *wptr);

		/**
		 **  Ignore case?
		 **/

		if( sw_icase) {
		    strncpy( modulefile, *wptr, MOD_BUFSIZE);
		    for( c = modulefile; c && *c; c++)
			*c = tolower( *c);
		    c = modulefile;
		} else
		    c = *wptr;

		/**
		 **  Seek for the passed tokens
		 **/

		if( whatis_list)
		    fprintf( stderr, "%-21s: %s\n", list[i], *wptr);
		else
		    for( k=0; k<argc; k++) {
			if( strstr( c, argv[ k]))
		    	    fprintf( stderr, "%-21s: %s\n", list[i], *wptr);
		    }
		wptr++;
	    }
	}

	/**
	 **  Remove the Tcl interpreter that has been used for printing ...
	 **/

	EM_DeleteInterp( whatis_interp);
	cmdModuleWhatisShut();

    } /** for( i) **/

    /**
     **	 Cleanup
     **/
    g_flags &= ~M_WHATIS;
    delete_dirlst( dirlst_head, count);
    delete_cache_list( list, start);

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_whatis_dir, NULL);
#endif

    return( result);			/** ------- EXIT (result) --------> **/

unwind2:
    delete_cache_list( list, start);
unwind1:
    delete_dirlst( dirlst_head, count);
unwind0:
    return( TCL_ERROR);			/** ------- EXIT (FAILURE) --------> **/

} /** End of 'whatis_dir' **/

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		read_cache					     **
 **									     **
 **   Description:	Grep in the cache file for the passed tokens         **
 **									     **
 **   First Edition:	1991/10/23					     **
 **									     **
 **   Parameters:	int	 argc		Number of tokens	     **
 **			char   **argv		List of tokens to check	     **
 **			FILE	*cfp		Cache file pointer	     **
 **									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 **									     **
 **   Attached Globals: -						     **
 **									     **
 ** ************************************************************************ **
 ++++*/

static	int	read_cache( int argc, char **argv, FILE *cfp, int whatis_list)
{
    char	*wptr, *c;
    char	 modulefile[ MOD_BUFSIZE];
    char	 buffer[ MOD_BUFSIZE];
    int		 k;

#if WITH_DEBUGGING_UTIL_1
/*
    ErrorLogger( NO_ERR_START, LOC, _proc_read_cache, "dir='", dir, NULL);
*/
#endif

    while( wptr = fgets( buffer, MOD_BUFSIZE, cfp)) {

	/**
	 **  Ingnore case?
	 **/

	if( sw_icase) {
	    strncpy( modulefile, wptr, MOD_BUFSIZE);
	    for( c = modulefile; c && *c; c++)
		*c = tolower( *c);
	    c = modulefile;
	} else
	    c = wptr;

	c = strchr( c, ':');
	c++;

	/**
	 **  Seek for the passed tokens
	 **/

	if ( whatis_list)
	    fprintf( stderr, "%s", wptr);
	else
	    for( k=0; k<argc; k++)
		if( strstr( c, argv[ k]))
		    fprintf( stderr, "%s", wptr);
    }

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_read_cache, NULL);
#endif

    return( TCL_OK);

} /** End of 'read_cache' **/

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		apropos_cache					     **
 **									     **
 **   Description:	Figure out, what's the name of the apropos cache file**
 **									     **
 **   First Edition:	1991/10/23					     **
 **									     **
 **   Parameters:	-						     **
 **									     **
 **   Result:		char*	NULL		No cache file configured     **
 **				Otherwise	Pointer to the file name     **
 **									     **
 **   Attached Globals: -						     **
 **									     **
 ** ************************************************************************ **
 ++++*/

static	char	*apropos_cache()
{
    char	*buffer, *env, *env_file, *env_path;

#if WITH_DEBUGGING_UTIL_1
/*
    ErrorLogger( NO_ERR_START, LOC, _proc_apropos_cache, "dir='", dir, NULL);
*/
#endif

    /**
     **	 Figure out, what out global RC file is. This depends on the environ-
     **	 ment variable 'MODULEWHATISCACHE', which can be set to one of the
     **	 following:
     **
     **		<filename>	-->	PREFIX/etc/<filename>
     **		<dir>/		-->	<dir>/RC_FILE
     **		<dir>/<file>	-->	<dir>/<file>
     **/

    if( env = getenv( "MODULEWHATISCACHE")) {

	if((char *) NULL == (env_file = strrchr( env, '/'))) {
	    env_file = env;
	    env_path = instpath;

	} else {
	    *env_file++ = '\0';
	    env_path = env;
	}
	
	if( !*env_file)
	    env_file = cache_name;

    } else {
	env_path = instpath;
	env_file = cache_name;
    }

    /**
     **	 Finaly we have to change PREFIX -> PREFIX/etc
     **/
    if( env_path == instpath) {
	if((char *) NULL == (buffer = stringer(NULL,0,
		env_path,"/etc/",env_file,NULL)))
	    goto unwind0;
    } else {
	if((char *) NULL == (buffer = stringer(NULL,0,
		env_path,"/",env_file,NULL)))
	    goto unwind0;
    }

    /**
     **	 Return the name of the cache file
     **/
#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_apropos_cache, NULL);
#endif

    return( buffer);

unwind0:
    return (char *) NULL;
} /** End of 'apropos_cache' **/
