/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		init.c						     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@strawberry.com			     **
 **		R.K. Owen, rk@owen.sj.ca.us				     **
 ** 									     **
 **   Description:	The initialization routines for Tcl Modules.	     **
 **			Primarily the setup of the different Tcl module	     **
 **			commands and the global hash tables are initialized  **
 **			here. The initial storage of the begining	     **
 **			environment is here as well.			     **
 ** 									     **
 **   Exports:		EM_CreateInterp					     **
 **   			Initialize_Tcl					     **
 **			Module_Tcl_ExitCmd				     **
 **			InitializeModuleCommands			     **
 **			Setup_Environment				     **
 **			TieStdout					     **
 **			UnTieStdout					     **
 **			SetStartupFiles					     **
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

static char Id[] = "@(#)$Id: init.c,v 1.9.4.4 2011/11/11 15:04:03 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

#ifdef	HAS_TCLXLIBS
#  include "tclExtend.h"
#endif	/* HAS_TCLXLIBS */

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

static	char	module_name[] = "init.c";	/** File name of this module **/

#if WITH_DEBUGGING_CALLBACK
static	char	_proc_Module_Tcl_ExitCmd[] = "Module_Tcl_ExitCmd";
#endif
#if WITH_DEBUGGING_INIT
static	char	_proc_InitializeModuleCommands[] = "InitializeModuleCommands";
static	char	_proc_EM_CreateInterp[] = "EM_CreateInterp";
static	char	_proc_EM_DeleteInterp[] = "EM_DeleteInterp";
static	char	_proc_Initialize_Tcl[] = "Initialize_Tcl";
static	char	_proc_Setup_Environment[] = "Setup_Environment";
#endif
#if WITH_DEBUGGING_UTIL_2
static	char	_proc_TieStdout[] = "TieStdout";
static	char	_proc_UnTieStdout[] = "UnTieStdout";
#endif
#if WITH_DEBUGGING_UTIL
static	char	_proc_SetStartupFiles[] = "SetStartupFiles";
#endif
#if WITH_DEBUGGING_UTIL_3
static	char	_proc_set_shell_properties[] = "set_shell_properties";
#endif

/** These are the recognized startup files that the given shells
 ** use.  If your site uses a different set, make the modifications here.
 ** Give the names and the order they should be searched. The lists
 ** must be null terminated.
 **/

/** CSH **/
static char *cshStartUps[] = {
    ".modules", ".cshrc" DOT_EXT, ".csh_variables", ".login" DOT_EXT, NULL
};
/** TCSH **/

static char *tcshStartUps[] = {
    ".modules", ".tcshrc", ".cshrc" DOT_EXT, ".csh_variables",
    ".login" DOT_EXT, NULL
};

/** SH and KSH **/
/** KSH uses whatever is pointed to by $ENV, which is usually named .kshenv
 ** (TODO) have it read $ENV and use the value
 **/

static char *shStartUps[] = {
    ".modules", ".profile" DOT_EXT, ".kshenv" DOT_EXT, NULL
};

/** BASH **/
/** BASH uses whatever is pointed to by $ENV, for non-interactive shells
 ** and for POSIX compliance
 ** (TODO) have it read $ENV and use the value
 **/

static char *bashStartUps[] = {
    ".modules", ".bash_profile", ".bash_login",
    ".profile" DOT_EXT, ".bashrc" DOT_EXT, NULL
};

/** ZSH **/

static char *zshStartUps[] = {
    ".modules", ".zshrc" DOT_EXT, ".zshenv" DOT_EXT, ".zlogin" DOT_EXT, NULL
};

/** All the remaining "shells"  are not supposed to use startup files **/

static char *genericStartUps[] = {
    NULL
};

/** The shell properties matrix - global pointers are set to elements of
 ** this array
 **/
static char *shellprops [][4] = {
/*	shell		derelict	init		cmd sep		*/
	{"csh",		"csh",		"csh",		";"},
	{"tcsh",	"csh",		"csh",		";"},
	{"sh",		"sh",		"sh",		";"},
	{"ksh",		"sh",		"ksh",		";"},
	{"bash",	"sh",		"bash",		";"},
	{"zsh",		"sh",		"zsh",		";"},
	{"perl",	"perl",		"perl",		";"},
	{"python",	"python",	"python",	"\n"},
	{"ruby",	"ruby", 	"ruby", 	"\n"},
	{"scm",		"scm",		NULL,		"\n"},
	{"scheme",	"scm",		NULL,		"\n"},
	{"guile",	"scm",		NULL,		"\n"},
	{"mel",		"mel",		NULL,		";"},
	{"cmake",	"cmake",	"cmake",	"\n"},
	{NULL,		NULL,		NULL,		NULL},
};

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static char	*set_shell_properties(	const char	*name);


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Module_Tcl_ExitCmd				     **
 ** 									     **
 **   Description:	Error (???) exit routine			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	ClientData	client_data			     **
 **			Tcl_Interp*	interp		The attached Tcl     **
 **							interpreter	     **
 **			int		argc		Number of arguments  **
 **			char		*argv[]		Array of arguments   **
 **							to the module command**
 ** 									     **
 **   Result:		int	TCL_ERROR		Exit on error	     **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Module_Tcl_ExitCmd(	ClientData	  client_data,
		   	Tcl_Interp	 *interp,
		   	int 		  argc,
		   	CONST84 char 	 *argv[])
{
    char *buffer;			/** Buffer for sprintf		     **/
    int  value;				/** Return value from exit command   **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_Module_Tcl_ExitCmd, NULL);
#endif

    /**
     **  Check the number of arguments. The exit command may take no or one
     **  parameter. So the following is legal:
     **     exit;
     **     exit value;
     **/
    if((argc != 1) && (argc != 2))
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "?returnCode?", NULL))
	    goto unwind0;

    /**
     **  If the exit command comes with an paramter, set up the TCL result.
     **  Otherwise the result is 0.
     **/
    if( argc == 1) {
	value = 0;
    } else if( Tcl_GetInt( interp, argv[1], &value) != TCL_OK) {
	if( OK != ErrorLogger( ERR_PARAM, LOC, argv[1], NULL))
	    goto unwind0;
    }

    /**
     **  Allocate memory
     **/
    if((char *) NULL == (buffer = stringer(NULL,25,NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;

    sprintf( buffer, "EXIT %d", value);
    Tcl_SetResult( interp, buffer, NULL);

    /**
     **  Exit from this module command.
     **  ??? Why hardcoded on error ???
     **/
#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_Module_Tcl_ExitCmd, NULL);
#endif

unwind0:
    return( TCL_ERROR);

} /** End of 'Module_Tcl_ExitCmd' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		EM_CreateInterp					     **
 ** 									     **
 **   Description:	Create a Tcl interpreter and set some default	     **
 **			attributes for each interpreter.		     **
 ** 									     **
 **   First Edition:	2011/09/26					     **
 ** 									     **
 **   Parameters:	-						     **
 ** 									     **
 **   Result:		Tcl_Interp	**interp	New Tcl interpr.     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

Tcl_Interp *EM_CreateInterp(void) {
	Tcl_Interp	*interp;

#if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_EM_CreateInterp, NULL);
#endif

    interp = Tcl_CreateInterp();
    /*
     * avoid freeing storage when in use
     */
    Tcl_Preserve(interp);

    return interp;

} /** End of 'EM_CreateInterp' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		EM_DeleteInterp					     **
 ** 									     **
 **   Description:	Delete a Tcl interpreter and set some default	     **
 **			attributes for each interpreter.		     **
 ** 									     **
 **   First Edition:	2011/09/26					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	**interp	Tcl interpr to	     **
 ** 							delete		     **
 ** 									     **
 **   Result	:	-						     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void EM_DeleteInterp(Tcl_Interp *interp) {

#if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_EM_DeleteInterp, NULL);
#endif

    /*
     * avoid freeing storage when in use, now release
     */
    Tcl_Release(interp);

    Tcl_DeleteInterp(interp);

} /** End of 'EM_DeleteInterp' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Initialize_Tcl					     **
 ** 									     **
 **   Description:	This procedure is called from 'main' in order to ini-**
 **			tialize the whole thing. The arguments specified on  **
 **			the invoking command line are passed to here.	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	**interp	Buffer to store the  **
 **							Tcl interpr. handle  **
 **			int		  argc		Number od args and   **
 **			char		 *argv[]	arg. array from the  **
 **							shell command line   **
 **			char		 *environ[]	Process environment  **
 ** 									     **
 **   Result:		int						     **
 ** 									     **
 **   Attached Globals:	*Ptr		will be initialized		     **
 **			*HashTable	will be allocated and initialized    **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Initialize_Tcl(	Tcl_Interp	**interp,
	       		int         	  argc,
	       		char		 *argv[],
               		char		 *environ[])
{
    int 	Result = TCL_ERROR;
    char *	tmp;

#if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_Initialize_Tcl, NULL);
#endif

    /**
     **  Check the command syntax. Since this is already done
     **  Less than 3 parameters isn't valid. Invocation should be
     **   'modulecmd <shell> <command>'
     **/
    if(argc < 2) 
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], " shellname", NULL))
	    goto unwind0;

    /**
     **  Check the first parameter to modulcmd for a known shell type
     **  and set the shell properties
     **/
    if( !set_shell_properties( argv[1]))
	if( OK != ErrorLogger( ERR_SHELL, LOC, argv[1], NULL))
	    goto unwind0;

    /**
     **  Create a Tcl interpreter in order to proceed the command. Initialize
     **  this interpreter and set up pointers to all Tcl Module commands
     **  (InitializeModuleCommands)
     **/
 
#ifdef __CYGWIN__
    /* ABr, 12/10/01: from Cygwin stuff */
    Tcl_FindExecutable( argv[0] ) ;
#endif

    *interp = EM_CreateInterp();

    if( TCL_OK != (Result = InitializeModuleCommands( *interp)))
	goto unwind0;

    /**
     **  Now set up the hash-tables for shell environment modifications.
     **  For a description of these tables have a look at main.c, where
     **  they're defined.  The tables have to be allocated and thereafter
     **  initialized. Exit from the whole program in case allocation fails.
     **/
    if( ( ! ( setenvHashTable = 
	    (Tcl_HashTable*) module_malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( unsetenvHashTable = 
	    (Tcl_HashTable*) module_malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( aliasSetHashTable = 
	    (Tcl_HashTable*) module_malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( aliasUnsetHashTable = 
	    (Tcl_HashTable*) module_malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( markVariableHashTable = 
	    (Tcl_HashTable*) module_malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( markAliasHashTable = 
	    (Tcl_HashTable*) module_malloc( sizeof(Tcl_HashTable))) ) ) {

	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind0;
    }

    Tcl_InitHashTable( setenvHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( unsetenvHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( aliasSetHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( aliasUnsetHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( markVariableHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( markAliasHashTable, TCL_STRING_KEYS);

#ifdef BEGINENV
#  if BEGINENV == 99
    /**
     **  Check for the existence of the environment variable
     **  "MODULESBEGINENV".  This signals that for this
     **  configuration that the user wants to record the initial
     **  environment as seen for the first time by the module
     **  command into the filename given in the MODULESBEGINENV
     **  environment variable (which can have one level of
     **  variable expansion).  Whether it's the first time or not
     **  is moderated by the existence of environment variable
     **  _MODULESBEGINENV_ or not.
     **
     **  The update command will use this information to reinitialize the
     **  environment and then reload every modulefile that has been loaded
     **  since as stored in the LOADEDMODULES environment variable in order.
     **/
    if( (tmp = xgetenv( "MODULESBEGINENV")) ) {
	/* MODULESBEGINENV is set ... use it */

	if( !getenv( "_MODULESBEGINENV_") ) {
		FILE*  file;
		if( (file = fopen(tmp, "w+")) ) {
			int i=0;
			while( environ[i]) {
				fprintf( file, "%s\n", environ[i++]);
			}
			moduleSetenv( *interp, "_MODULESBEGINENV_", tmp, 1);
			fclose( file);
		} else
			if( OK != ErrorLogger( ERR_OPEN, LOC,
			    TCL_RESULT(*interp), "append", NULL))
			    goto unwind0;

		null_free((void *) &tmp);
	}
    }
#  else
    /**
     **  Check for the existence of the
     **  environment variable "_MODULESBEGINENV_".  If it is set, then
     **  do nothing, otherwise, Store every environment variable into
     **  ~/.modulesbeginenv.  This will be used to store the environment
     **  variables exactly as it was when Modules saw it for the very first
     **  time.
     **
     **  The update command will use this information to reinitialize the
     **  environment and then reload every modulefile that has been loaded
     **  since as stored in the LOADEDMODULES environment variable in order.
     **/
    if( !getenv( "_MODULESBEGINENV_") ) {
	/* use .modulesbeginenv */

        FILE*  file;
	
        char savefile[] = "/.modulesbeginenv";
	char *buffer;

	tmp = getenv("HOME");
	if((char *) NULL == (tmp = getenv("HOME")))
	    if( OK != ErrorLogger( ERR_HOME, LOC, NULL))
		goto unwind0;

	if((char *) NULL == (buffer = stringer(NULL,0,tmp,savefile,NULL)))
	    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		goto unwind0;

            if( file = fopen(buffer, "w+")) {
                int i=0;
                while( environ[i]) {
                    fprintf( file, "%s\n", environ[i++]);
                }
                moduleSetenv( *interp, "_MODULESBEGINENV_", buffer, 1);
                fclose( file);
            } else
		if( OK != ErrorLogger( ERR_OPEN, LOC,
		    TCL_RESULT(*interp), "append", NULL))
		    goto unwind0;

	    null_free((void *) &buffer);
    }
#  endif
#endif

    /**
     **  Exit to the main program
     **/
    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind0:
    return( Result);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'Initialize_Tcl' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		InitializeModuleCommands			     **
 ** 									     **
 **   Description:	Initialization of the passed Tcl interpreter. At     **
 **			first the standard Tcl and (if required) TclX initi- **
 **			alization is called. Thereafter all module commands  **
 **			callback function are defined.			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	 *interp	The Tcl Interpreter  **
 **							to be initilized     **
 ** 									     **
 **   Result:		int	TCL_OK		All done, Success	     **
 **				TCL_ERROR	Failure anywhere	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int InitializeModuleCommands( Tcl_Interp* interp)
{

#if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_InitializeModuleCommands, NULL);
#endif

    /**
     **  General initialization of the Tcl interpreter
     **/
    if( Tcl_Init( interp) == TCL_ERROR)
	if( OK != ErrorLogger( ERR_INIT_TCL, LOC, NULL))
	    goto unwind0;

#ifdef  HAS_TCLXLIBS

    /**
     **  Extended Tcl initialization if configured so ...
     **/

#if (TCL_MAJOR_VERSION > 8 || TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION > 3)
    if( Tclx_Init( interp) == TCL_ERROR)
#elif (TCL_MAJOR_VERSION > 7 || TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION > 5)
    if( Tclxcmd_Init( interp) == TCL_ERROR)
#else
    if( TclXCmd_Init( interp) == TCL_ERROR)
#endif
    {
	if( OK != ErrorLogger( ERR_INIT_TCLX, LOC, NULL))
	    goto unwind0;
    }

#endif  /* HAS_TCLXLIBS */

#ifdef	AUTOLOADPATH

    /**
     ** Extend autoload path
     **/
    if( TCL_OK != Tcl_Eval( interp,
	"if [info exists auto_path] { "
		"set auto_path [linsert $auto_path 0 " AUTOLOADPATH
	"]} else {"
		"set auto_path \"" AUTOLOADPATH "\" }"))
	if( OK != ErrorLogger( ERR_INIT_ALPATH, LOC, NULL))
	    goto unwind0;

#endif	/* AUTOLOADPATH */

    /**
     **   Now for each module command a callback routine has to be specified
     **/
    Tcl_CreateCommand( interp, "exit", Module_Tcl_ExitCmd,
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "setenv", cmdSetEnv, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "unsetenv", cmdUnsetEnv, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
  
    Tcl_CreateCommand( interp, "prepend-path", cmdSetPath, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "append-path", cmdSetPath, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "remove-path", cmdRemovePath, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "module-info", cmdModuleInfo, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module", cmdModule, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "module-whatis", cmdModuleWhatis, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-verbosity", cmdModuleVerbose, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-user", cmdModuleUser, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-log", cmdModuleLog, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-trace", cmdModuleTrace, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "module-alias", cmdModuleAlias, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-version", cmdModuleVersion, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
  
    Tcl_CreateCommand( interp, "set-alias", cmdSetAlias, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "unset-alias", cmdSetAlias, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "conflict", cmdConflict, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "prereq", cmdPrereq, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "is-loaded", cmdIsLoaded, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "chdir", cmdChDir,
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "system", cmdSystem, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "uname", cmdUname, 
		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "x-resource", cmdXResource,
 		       (ClientData) shell_derelict,(void (*)(ClientData)) NULL);

    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind0:
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'InitializeModuleCommands' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Setup_Environment				     **
 ** 									     **
 **   Description:Define all variables to be found in the current	     **
 **			shell environment as Tcl variables in the passed     **
 **			Tcl interpreter.				     **
 **			Assign as value 0 to all of them. ??? Why ???	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	 *interp	Attched Tcl interpr. **
 ** 									     **
 **   Result:		int	TCL_ERROR	Variable could not be set up **
 **				0		Success ??? TCL_OK ???	     **
 ** 									     **
 **   Attached Globals:	environ						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Setup_Environment( Tcl_Interp*	interp)
{

    int   	 i, 			/** loop counter		     **/
		 envsize = 0;		/** Total size of the environment    **/
    char	*eq;			/** Temp. val. used for location the **/
					/** Equal sign.			     **/
    char	*loaded;		/** The currently loaded modules     **/
 
#if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_Setup_Environment, NULL);
#endif

    /** 
     **  Scan the whole environment value by value.
     **  Count its size
     **/
    for( i = 0; environ[i]; i++) {

	envsize += strlen( environ[i]) + 1;

	/**
	 **  Locate the equal sign and terminate the string at its position.
	 **/
	eq = environ[i];
	while( *eq++ != '=' && *eq);
	*(eq - 1) = '\0';

	/**
	 **  Now set up a Tcl variable of the same name and value as the
	 **  environment variable
	 **/
	if( Tcl_SetVar( interp, environ[i], eq, 0) == (char *) NULL)
	    if( OK != ErrorLogger( ERR_SET_VAR, LOC, environ[i], NULL))
		goto unwind0;

	/**
	 **  Reinstall the changed environment
	 **/
	*(eq - 1) = '=';

    } /** for **/

    /**
     ** Reconstruct the _LMFILES_ environment variable
     **/
    loaded = getLMFILES( interp);
    if( loaded)
	if( !(EMSetEnv( interp, "_LMFILES_", loaded)))
	    if( OK != ErrorLogger( ERR_SET_VAR, LOC, environ[i], NULL))
		goto unwind0;

    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind0:
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** end of 'Setup_Environment' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		TieStdout, UnTieStdout				     **
 ** 									     **
 **   Description:	TieStdout closes the 'stdout' handle and reopens it  **
 **			as 'stderr'. The original 'stdout' handle is passed  **
 **			back to the caller.				     **
 **			UnTieStdout reverts this by reopening 'stdout' as the**
 **			handle passed as parameter			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	int	saved_stdout	Handle to be used for rein-  **
 **						stalling stdout		     **
 ** 									     **
 **   Result:		int	The (just reinstalled or saved) stdout handle**
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int TieStdout( void) {
    int save;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_TieStdout, NULL);
#endif

    if( 0 > (save = dup(1)))
	if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stdout, NULL))
	    goto unwind0;

    if( 0 > close( 1))
	if( OK != ErrorLogger( ERR_CLOSE, LOC, _fil_stdout, NULL))
	    goto unwind0;

    /**
     ** dup used the very first closed handle for duplication. Since stdout
     ** has just been closed, this will be reopened as stderr here.
     **/
    if( 0 > (dup(2)))
	if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stderr, NULL))
	    goto unwind0;

    return( save);			/** ------- EXIT (RESULT)  --------> **/

unwind0:
    return( -1);			/** ------- EXIT (FAILURE) --------> **/
}

int UnTieStdout( int saved_stdout) {

    int		retval;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_UnTieStdout, NULL);
#endif

    if( 0 > close( 1))
	if( OK != ErrorLogger( ERR_CLOSE, LOC, _fil_stdout, NULL))
	    goto unwind0;

    if( 0 > (retval = dup( saved_stdout)))
	if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stdout, NULL))
	    goto unwind0;

    return( retval);

unwind0:
    return( -1);			/** ------- EXIT (FAILURE) --------> **/
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		SetStartupFiles					     **
 ** 									     **
 **   Description:	Collects all startupfiles used by the various shells **
 **			in the array 'shell_startups'. This function does not**
 **			care if the startup file do not exist!		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	shell_name	the shell being used		     **
 **   Result:		shell_startups	NULL terminated list of startup files**
 **					for the shell			     **
 **					returns NULL if an error	     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char **SetStartupFiles(char *shell_name)
{

#if WITH_DEBUGGING_UTIL
    ErrorLogger( NO_ERR_START, LOC, _proc_SetStartupFiles, NULL);
#endif

    /**
     ** CSH
     **/
    if( (strcmp( "csh", shell_name) == 0)) {

       return cshStartUps;

    /**
     ** TCSH
     **/
    } else if((strcmp("tcsh", shell_name) == 0)) {

       return tcshStartUps;

    /**
     ** SH and KSH
     ** ??? What's about .environ ???
     **/
    } else if((strcmp("sh", shell_name) == 0) ||
	      (strcmp("ksh", shell_name) == 0)) {

       return shStartUps;

    /**
     ** BASH
     ** ??? doesn't this guy use the SH startups, too ???
     **/
    } else if((strcmp("bash", shell_name) == 0)) { 

       return bashStartUps;

    /**
     ** ZSH
     **/
    } else if((strcmp("zsh", shell_name) == 0)) { 

       return zshStartUps;
       
    /**
     **  All of the remainig "shells" are not supposed to used startup
     **  files
     **/
    } else {

       return genericStartUps;
    }

} /** End of 'SetStartupFiles' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		set_shell_properties				     **
 ** 									     **
 **   Description:	Normalize the current calling shell to one of the    **
 **			basic shells defining the variable and alias syntax  **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*name	Invoking shell name	     **
 ** 									     **
 **   Result:		char*			Shell derelict name	     **
 ** 									     **
 **   Attached Globals:	shell_derelict					     **
 ** 			shell_cmd_separator				     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static char	*set_shell_properties(	const char	*name) 
{

#if WITH_DEBUGGING_UTIL_3
    ErrorLogger( NO_ERR_START, LOC, _proc_set_shell_properties, NULL);
#endif

    /**
     ** Loop through the shell properties matrix until a match is found
     **/
    int i = 0;

    while (shellprops[i][0]) {
	if( !strcmp(name,shellprops[i][0])) {	/* found match */
	    shell_name		= shellprops[i][0];
	    shell_derelict	= shellprops[i][1];
	    shell_init		= shellprops[i][2];
	    shell_cmd_separator	= shellprops[i][3];
	    return ((char *) name);
	}
	i++;
    }

    shell_name		= NULL;
    shell_derelict	= NULL;
    shell_init		= NULL;
    shell_cmd_separator	= NULL;
    /**
     **  Oops! Undefined shell name ...
     **/
    return( NULL);

} /** End of 'set_shell_properties' **/
