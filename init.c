/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		init.c						     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The initialization routines for Tcl Modules.	     **
 **			Primarily the setup of the different Tcl module	     **
 **			commands and the global hash tables are initialized  **
 **			here. The initial storage of the begining	     **
 **			environment is here as well.			     **
 ** 									     **
 **   Exports:		Initialize_Tcl					     **
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

static char Id[] = "@(#)$Id: init.c,v 1.1 2000/06/28 00:17:32 rk Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

#ifdef	HAS_TCLXLIBS
#include "tclExtend.h"
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

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Module_Tcl_ExitCmd				     **
 ** 									     **
 **   Description:	Error (???) exit routine			     **
 ** 									     **
 **   First Edition:	91/10/23					     **
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
		   	char	  	 *argv[])
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

    if((argc != 1) && (argc != 2)) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "?returnCode?", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  If the exit command comes with an paramter, set up the TCL result.
     **  Otherwise the result is 0.
     **/

    if( argc == 1) {
	value = 0;
    } else if( Tcl_GetInt( interp, argv[1], &value) != TCL_OK) {
	if( OK != ErrorLogger( ERR_PARAM, LOC, argv[1], NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Allocate memory
     **/

    if( NULL == (buffer = (char *) malloc( 25)))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

    sprintf( buffer, "EXIT %d", value);
    Tcl_SetResult( interp, buffer, NULL);

    /**
     **  Exit from this module command.
     **  ??? Why hardcoded on error ???
     **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_Module_Tcl_ExitCmd, NULL);
#endif

    return( TCL_ERROR);

} /** End of 'Module_Tcl_ExitCmd' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Initialize_Tcl					     **
 ** 									     **
 **   Description:	This procedure is called from 'main' in order to ini-**
 **			tialize the whole thing. The arguments specified on  **
 **			the invoking command line are passed to here.	     **
 ** 									     **
 **   First Edition:	91/10/23					     **
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
    int 	Result;

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
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

    strcpy( shell_name, argv[1]);

    /**
     **  Check the first parameter to modulcmd for a known shell type
     **/

    if( !set_derelict( argv[1])) {
	if( OK != ErrorLogger( ERR_SHELL, LOC, argv[1], NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Now create a Tcl interpreter in order to proceed the command. Initialize
     **  this interpreter and set up pointers to all Tcl Module commands
     **  (InitializeModuleCommands)
     **/

    *interp = Tcl_CreateInterp();
    if( TCL_OK != (Result = InitializeModuleCommands( *interp)))
	return( Result);		/** -------- EXIT (FAILURE) -------> **/

    /**
     **  Now set up the hash-tables for shell environment modifications.
     **  For a description of these tables have a look at main.c, where
     **  they're defined.  The tables have to be allocated and thereafter
     **  initialized. Exit from the whole program in case allocation fails.
     **/

    if( ( ! ( setenvHashTable = 
	    (Tcl_HashTable*) malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( unsetenvHashTable = 
	    (Tcl_HashTable*) malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( aliasSetHashTable = 
	    (Tcl_HashTable*) malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( aliasUnsetHashTable = 
	    (Tcl_HashTable*) malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( markVariableHashTable = 
	    (Tcl_HashTable*) malloc( sizeof(Tcl_HashTable))) ) ||
        ( ! ( markAliasHashTable = 
	    (Tcl_HashTable*) malloc( sizeof(Tcl_HashTable))) ) ) {

	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    Tcl_InitHashTable( setenvHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( unsetenvHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( aliasSetHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( aliasUnsetHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( markVariableHashTable, TCL_STRING_KEYS);
    Tcl_InitHashTable( markAliasHashTable, TCL_STRING_KEYS);

    /**
     **  What I'm going to do here is check for the existance of the
     **  environment variable "_MODULESBEGINENV_".  If it is set, then I will do
     **  nothing, otherwise, I will store every environment variable into
     **  ~/.modulesbeginenv.  This will be used to store the environment
     **  variables exactly as it was when Modules saw it for the very first
     **  time.
     **
     **  The update command will use this information to reinitialize the
     **  environment and then reload every modulefile that has been loaded
     **  since as stored in the LOADEDMODULES environment variable in order.
     **/

    if( !getenv( "_MODULESBEGINENV_") ) {

        FILE*  file;
	
        char savefile[] = "/.modulesbeginenv";
	char *ptr, *buffer;

	ptr = getenv("HOME");
	if( ptr == (char *) NULL)
	    if( OK != ErrorLogger( ERR_HOME, LOC, NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/

	if( NULL == (buffer = (char *) malloc(strlen(ptr)+strlen(savefile)+1)))
	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/

	    strcpy(buffer, ptr);
	    strcat(buffer, savefile);
        
            if( file = fopen(buffer, "w+")) {
                int i=0;
                while( environ[i]) {
                    fprintf( file, "%s\n", environ[i++]);
                }
                moduleSetenv( *interp, "_MODULESBEGINENV_", buffer, 1);
                fclose( file);
            } else
		if( OK != ErrorLogger( ERR_OPEN, LOC, (*interp)->result,
		    "append", NULL))
		    return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/

	    free(buffer);
    }

    /**
     **  Exit to the main program
     **/

    return( TCL_OK);

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
 **   First Edition:	91/10/23					     **
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

    if( Tcl_Init( interp) == TCL_ERROR) {
	if( OK != ErrorLogger( ERR_INIT_TCL, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#ifdef  HAS_TCLXLIBS

    /**
     **  Extended Tcl initialization if configured so ...
     **/

#if (TCL_MAJOR_VERSION > 7 || TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION > 5)
    if( Tclxcmd_Init( interp) == TCL_ERROR) {
#else
    if( TclXCmd_Init( interp) == TCL_ERROR) {
#endif
	if( OK != ErrorLogger( ERR_INIT_TCLX, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#endif  /* HAS_TCLXLIBS */

#ifdef	AUTOLOADPATH

    /**
     ** Extend autoload path
     **/

    if( TCL_OK != Tcl_VarEval( interp, "set auto_path [linsert $auto_path 0 ",
	AUTOLOADPATH, "]", (char *) NULL)) {
	if( OK != ErrorLogger( ERR_INIT_ALPATH, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

#endif	/* AUTOLOADPATH */

    /**
     **   Now for each module command a callback routine has to be specified
     **/

    Tcl_CreateCommand( interp, "exit", Module_Tcl_ExitCmd,
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "setenv", cmdSetEnv, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "unsetenv", cmdUnsetEnv, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
  
    Tcl_CreateCommand( interp, "prepend-path", cmdSetPath, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "append-path", cmdSetPath, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "remove-path", cmdRemovePath, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "module-info", cmdModuleInfo, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module", cmdModule, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "module-whatis", cmdModuleWhatis, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-verbosity", cmdModuleVerbose, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-user", cmdModuleUser, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-log", cmdModuleLog, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-trace", cmdModuleTrace, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "module-alias", cmdModuleAlias, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "module-version", cmdModuleVersion, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
  
    Tcl_CreateCommand( interp, "set-alias", cmdSetAlias, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "unset-alias", cmdSetAlias, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "conflict", cmdConflict, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "prereq", cmdPrereq, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "is-loaded", cmdIsLoaded, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "system", cmdSystem, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);
    Tcl_CreateCommand( interp, "uname", cmdUname, 
		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);

    Tcl_CreateCommand( interp, "x-resource", cmdXResource,
 		       (ClientData) shell_derelict, (void (*)(ClientData)) NULL);

    /**
     **  Success
     **/

    return( TCL_OK);

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
 **   First Edition:	91/10/23					     **
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

	if( Tcl_SetVar( interp, environ[i], eq, 0) == NULL) {
	    if( OK != ErrorLogger( ERR_SET_VAR, LOC, environ[i], NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	}

	/**
	 **  Reinstall the changed environment
	 **/

	*(eq - 1) = '=';

    } /** for **/

    return( TCL_OK);

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
 **   First Edition:	91/10/23					     **
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

    if( -1 == (save = dup(1)))
	if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stdout, NULL))
	    return( -1);		/** ------- EXIT (FAILURE) --------> **/

    if( -1 == close( 1))
	if( OK != ErrorLogger( ERR_CLOSE, LOC, _fil_stdout, NULL))
	    return( -1);		/** ------- EXIT (FAILURE) --------> **/

    /**
     ** dup used the very first closed handle for duplication. Since stdout
     ** has just been closed, this will be reopened as stderr here.
     **/

    if( -1 == (dup(2)))
	if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stderr, NULL))
	    return( -1);		/** ------- EXIT (FAILURE) --------> **/

    return save;
}

int UnTieStdout( int saved_stdout) {

    int		retval;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_UnTieStdout, NULL);
#endif

    if( -1 == close( 1))
	if( OK != ErrorLogger( ERR_CLOSE, LOC, _fil_stdout, NULL))
	    return( -1);		/** ------- EXIT (FAILURE) --------> **/

    if( -1 == (retval = dup( saved_stdout)))
	if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stdout, NULL))
	    return( -1);		/** ------- EXIT (FAILURE) --------> **/

    return( retval);
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		SetStartupFiles					     **
 ** 									     **
 **   Description:	Collects all startupfiles used by the various shells **
 **			in the array 'shell_startups'. This function does not**
 **			take care, if the startup file do exist!	     **
 ** 									     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Parameters:	-						     **
 **   Result:		-						     **
 **   Attached Globals:	shell_startups	will be set up with a list of all    **
 **					startup files used by the shell      **
 **					specified in 'shell_name'	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int SetStartupFiles(void)
{

#if WITH_DEBUGGING_UTIL
    ErrorLogger( NO_ERR_START, LOC, _proc_SetStartupFiles, NULL);
#endif

    /**
     ** CSH
     **/

    if( (strcmp( "csh", shell_name) == 0)) {

       shell_startups = cshStartUps;

    /**
     ** TCSH
     **/

    } else if((strcmp("tcsh", shell_name) == 0)) {

       shell_startups = tcshStartUps;

    /**
     ** SH and KSH
     ** ??? What's about .environ ???
     **/

    } else if((strcmp("sh", shell_name) == 0) ||
	      (strcmp("ksh", shell_name) == 0)) {

       shell_startups = shStartUps;

    /**
     ** BASH
     ** ??? doesn't this guy use the SH startups, too ???
     **/

    } else if((strcmp("bash", shell_name) == 0)) { 

       shell_startups = bashStartUps;

    /**
     ** ZSH
     **/

    } else if((strcmp("zsh", shell_name) == 0)) { 

       shell_startups = zshStartUps;
       
    /**
     **  All of the remainig "shells" are not supposed to used startup
     **  files
     **/

    } else {

       shell_startups = genericStartUps;
    }

    /**
     **  Success
     **/

    return( TCL_OK);

} /** End of 'SetStartupFiles' **/
