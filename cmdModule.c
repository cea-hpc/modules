/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdModule.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The actual module command from the Tcl level. This   **
 **			routines calls other ModuleCmd routines to carry out **
 **			the subcommand requested. 			     **
 ** 									     **
 **   Exports:		cmdModule					     **
 **			Read_Modulefile					     **
 **			Execute_TclFile					     **
 **			CallModuleProcedure				     **
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

static char Id[] = "@(#)$Id: cmdModule.c,v 1.11.6.1 2010/11/11 18:23:18 rkowen Exp $";
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

char	_fil_stdin[] = "stdin";
char	_fil_stdout[] = "stdout";
char	_fil_stderr[] = "stderr";
char	_fil_devnull[] = "/dev/null";

int	linenum = 0;

static	char	module_name[] = "cmdModule.c";	/** File name of this module **/

#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdModule[] = "cmdModule";
#endif
#if WITH_DEBUGGING_UTIL
static	char	_proc_Read_Modulefile[] = "Read_Modulefile";
#endif
#if WITH_DEBUGGING_UTIL_1
static	char	_proc_Execute_TclFile[] = "Execute_TclFile";
static	char	_proc_CallModuleProcedure[] = "CallModuleProcedure";
#endif

char	 *module_command;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModule					     **
 ** 									     **
 **   Description:	Evaluation of the module command line and callup of  **
 **			the according subroutine			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModule(	ClientData	 client_data,
	       		Tcl_Interp	*interp,
	       		int		 argc,
	       		CONST84 char	*argv[])
{
    int		  return_val = -1, i;
    int		  store_flags = g_flags;
    char	 *store_curmodule = NULL;
    char	 *save_module_command = NULL;
    int 	  match = 0;

    /**
     **  These skip the arguments past the shell and command.
     **/

    int		  num_modulefiles = argc - 2;
    char	**modulefile_list = (char **) argv + 2;

#if 0
	int x=0;
#  define _XD	fprintf(stderr,":%d:",++x),
#else
#  define _XD
#endif

#define	_MTCH	_XD match =
#define	_ISERR	((match == -1) && (*TCL_RESULT(interp)))
#define _TCLCHK(a) {if(_ISERR) ErrorLogger(ERR_EXEC,LOC,TCL_RESULT(a),NULL);}

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModule, NULL);
#endif

    /**
     **  Help or whatis mode?
     **/

    if( g_flags & (M_HELP | M_WHATIS))
	return( TCL_OK);

    /**
     **  Parameter check
     **/

    if( argc < 2) {
	(void) ErrorLogger( ERR_USAGE, LOC, "module", " command ",
	    " [arguments ...] ", NULL);
	(void) ModuleCmd_Help( interp, 0, modulefile_list);
	return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Non-persist mode?
     **/
    
    if (g_flags & M_NONPERSIST) {
	return (TCL_OK);
    }

    /**
     **  Display whatis mode?
     **/

    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t\t ", argv[ 0]);
	for( i=1; i<argc; i++)
	    fprintf( stderr, "%s ", argv[ i]);
	fprintf( stderr, "\n");
	return( TCL_OK);
    }
    
    /**
     **  For recursion.  This can be called multiple times.
     **/

    save_module_command = module_command;
    module_command  = strdup( argv[1]);

    if( g_current_module)
	store_curmodule = g_current_module;
    
    /**
     **  If the command is '-', we want to just start 
     **    interpreting Tcl from stdin.
     **/

    if(_XD !strcmp( module_command, "-")) { 
	return_val = Execute_TclFile( interp, _fil_stdin);

    /**
     **  Evaluate the module command and call the according subroutine
     **  --- module LOAD|ADD
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, addRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Load( interp, 1,num_modulefiles,modulefile_list);

       /**
        **  We always say the load succeeded.  ModuleCmd_Load will
        **  output any necessary error messages.
        **/

        return_val = TCL_OK;

    /**
     **  --- module UNLOAD
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, rmRE)) {
	_TCLCHK(interp);
        ModuleCmd_Load( interp, 0, num_modulefiles, modulefile_list);
	return_val = TCL_OK;

    /**
     **  --- module SWITCH
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, swRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Switch( interp, num_modulefiles,modulefile_list);

    /**
     **  --- module DISPLAY
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, dispRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Display( interp,num_modulefiles,modulefile_list);

    /**
     **  --- module LIST
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, listRE)) {
	_TCLCHK(interp);
	if (! (sw_format & SW_SET) ) {	/* default format options */
		sw_format |= (SW_HUMAN | SW_TERSE );
		sw_format &= ~(SW_PARSE | SW_LONG );
	}
	/* use SW_LIST to indicate LIST & not AVAIL */
	sw_format |= SW_LIST;
	return_val = ModuleCmd_List( interp, num_modulefiles, modulefile_list);

    /**
     **  --- module AVAIL
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command,availRE)) {
	_TCLCHK(interp);
	if (! (sw_format & SW_SET) ) {	/* default format options */
		sw_format |= (SW_HUMAN | SW_TERSE);
		sw_format &= ~(SW_PARSE | SW_LONG );
	}
	/* use SW_LIST to indicate LIST & not AVAIL */
	sw_format &= ~SW_LIST;
	return_val = ModuleCmd_Avail( interp, num_modulefiles, modulefile_list);

    /**
     **  --- module WHATIS and APROPOS
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command,whatisRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Whatis(interp, num_modulefiles, modulefile_list);

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command,aproposRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Apropos(interp, num_modulefiles,modulefile_list);

    /**
     **  --- module CLEAR
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command,clearRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Clear( interp, num_modulefiles, modulefile_list);

    /**
     **  --- module UPDATE
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command,updateRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Update(interp, num_modulefiles, modulefile_list);

    /**
     **  --- module PURGE
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command,purgeRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Purge( interp, num_modulefiles, modulefile_list);

    /**
     **  --- module INIT
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command,initRE)) {
	_TCLCHK(interp);
	
        if( Tcl_RegExpMatch(interp,module_command, "^inita|^ia")){/* initadd */
	    _TCLCHK(interp);
	    g_flags |= M_LOAD;
	    return_val = ModuleCmd_Init(interp,num_modulefiles,modulefile_list);
	    g_flags &= ~M_LOAD;
	}
	
        if( Tcl_RegExpMatch(interp,module_command, "^initr|^iw")){ /* initrm */
	    _TCLCHK(interp);
	    g_flags |= M_REMOVE;
	    return_val = ModuleCmd_Init(interp,num_modulefiles,modulefile_list);
	    g_flags &= ~M_REMOVE;
	}
	
        if( Tcl_RegExpMatch(interp,module_command, "^initl|^il")){/* initlist*/
	    _TCLCHK(interp);
	    g_flags |= M_DISPLAY;
	    return_val = ModuleCmd_Init(interp,num_modulefiles,modulefile_list);
	    g_flags &= ~M_DISPLAY;
	}
	
        if(Tcl_RegExpMatch(interp,module_command, "^inits|^is")){/* initswitch*/
	    _TCLCHK(interp);
	    g_flags |= M_SWITCH;
	    return_val = ModuleCmd_Init(interp,num_modulefiles,modulefile_list);
	    g_flags &= ~M_SWITCH;
	}
	
        if(Tcl_RegExpMatch(interp,module_command, "^initc|^ic")){/* initclear*/
	    _TCLCHK(interp);
	    g_flags |= M_CLEAR;
	    return_val = ModuleCmd_Init(interp,num_modulefiles,modulefile_list);
	    g_flags &= ~M_CLEAR;
	}
	
        if(Tcl_RegExpMatch(interp,module_command,"^initp|^ip")){/*initprepend*/
	    _TCLCHK(interp);
	    g_flags |= (M_PREPEND | M_LOAD);
	    return_val = ModuleCmd_Init(interp,num_modulefiles,modulefile_list);
	    g_flags &= ~(M_PREPEND | M_LOAD);
	}

    /**
     **  --- module USE
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, useRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Use( interp, num_modulefiles, modulefile_list);

    /**
     **  --- module UNUSE
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, unuseRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_UnUse( interp, num_modulefiles, modulefile_list);

    /**
     **  --- module REFRESH 
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, refreshRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Refresh( interp, num_modulefiles, modulefile_list);

    /**
     **  --- module HELP
     **/

    } else if(_MTCH Tcl_RegExpMatch(interp,module_command, helpRE)) {
	_TCLCHK(interp);
	return_val = ModuleCmd_Help( interp, num_modulefiles, modulefile_list);
    }
    
    /**
     **  Tracing
     **/

    if( CheckTracingList(interp,  module_command,
	num_modulefiles, modulefile_list))
	Module_Tracing( return_val, argc, (char **) argv);

    /**
     **  Evaluate the subcommands return value in order to get rid of unrecog-
     **  nized commands
     **/   

    if( return_val < 0)
	if( OK != ErrorLogger( ERR_COMMAND, LOC, module_command, NULL))
          return (TCL_ERROR);
    
    /**
     **  Clean up from recursion
     **/

    g_flags = store_flags;
    if( store_curmodule)
	g_current_module = store_curmodule;

    module_command = save_module_command;
 
    /**
     **  Return on success
     **/

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModule, NULL);
#endif

    return( return_val);

} /** End of 'cmdModule' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Read_Modulefile					     **
 ** 									     **
 **   Description:	Check the passed filename for to be a valid module   **
 **			and execute the according command file		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **		 	char		*filename			     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_BREAK	break command		     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int   Read_Modulefile( Tcl_Interp	*interp, 
		       char		*filename)
{
    int    result;
    char   *startp, *endp;

#if WITH_DEBUGGING_UTIL
    ErrorLogger( NO_ERR_START, LOC, _proc_Read_Modulefile, NULL);
#endif

    /**
     **  Parameter check. A valid filename is to be given.
     **/

    if( !filename) {
	if( OK != ErrorLogger( ERR_PARAM, LOC, "filename", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Check for the module 'magic cookie'
     **  Trust stdin as a valid module file ...
     **/
    
    if( !strcmp( filename, _fil_stdin) && !check_magic( filename,
    	MODULES_MAGIC_COOKIE, MODULES_MAGIC_COOKIE_LENGTH)) {
	if( OK != ErrorLogger( ERR_MAGIC, LOC, filename, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Now do execute that module file and evaluate the result of the
     **  latest executed command
     **/

    result = Execute_TclFile(interp, filename);

#if WITH_DEBUGGING_UTIL
    if(EM_ERROR == ReturnValue(interp, result))
	ErrorLogger( NO_ERR_DEBUG, LOC, "Execution of '",
		filename, "' failed", NULL);
#endif

    /**
     **  Return the result as derivered from the module file execution
     **/
#if WITH_DEBUGGING_UTIL
    ErrorLogger( NO_ERR_END, LOC, _proc_Read_Modulefile, NULL);
#endif

    return( result);

} /** End of 'Read_Modulefile' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Execute_TclFile					     **
 ** 									     **
 **   Description:	Read in and execute all commands concerning the Tcl  **
 **			file passed as parameter			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **		 	char		*filename			     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	line		Input read buffer		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	 Execute_TclFile(	Tcl_Interp	*interp,
		     		char		*filename)
{
    FILE	*infile;
    int		 gotPartial = 0;
    int		 result = 0;
    EM_RetVal	 em_result = EM_OK;
    char	*cmd;
    Tcl_DString	 cmdbuf;

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_Execute_TclFile, NULL);
#endif

    /**
     **  If there isn't a line buffer allocated so far, do it now
     **/

    if( line == NULL) {
        if( NULL == (line = (char*) module_malloc(LINELENGTH * sizeof(char)))) {
	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
        }
    }

    /**
     **  If we're supposed to be interpreting from stdin, set infile 
     **  equal to stdin, otherwise, open the file and interpret
     **/

    if( !strcmp( filename, _fil_stdin)) {
	infile = stdin;
    } else {
	if( NULL == (infile = fopen( filename, "r"))) {
	    if( OK != ErrorLogger( ERR_OPEN, LOC, filename, "reading", NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	}
    }
    
    /**
     **  Allow access to which file is being loaded.
     **/

    linenum = 0;
    Tcl_SetVar( interp, "ModulesCurrentModulefile", filename, 0);
    Tcl_DStringInit( &cmdbuf);
    
    while( 1) {

        linenum++;
	if( fgets(line, LINELENGTH, infile) == NULL) {
	    if( !gotPartial) {
		break;	/** while **/
	    }
	    line[0] = '\0';
	}
	
	/**
	 **  Put the whole command on the command buffer
	 **/

	cmd = Tcl_DStringAppend( &cmdbuf, line, (-1));
	
	if( line[0] != 0  && !Tcl_CommandComplete(cmd)) {
	    gotPartial++;
	    continue;
	}
	
	/**
	 **  Now evaluate the command and react on its result
	 **  Reinitialize the command buffer
	 **/

#if WITH_DEBUGGING_UTIL_1
	ErrorLogger( NO_ERR_DEBUG, LOC, " Evaluating '", cmd, "'", NULL);
#endif

        result = Tcl_Eval( interp, cmd);

	if( EM_ERROR == (em_result = ReturnValue(interp, result))) {
	    ErrorLogger( ERR_EXEC, LOC, cmd, NULL);
	}

	Tcl_DStringTrunc( &cmdbuf, 0);

#if WITH_DEBUGGING_UTIL_1
	{
	    char buffer[ 80];

	    switch( result) {
		case TCL_OK:	    strcpy( buffer, "TCL_OK");
				    break;
		
		case TCL_ERROR:	    strcpy( buffer, "TCL_ERROR");
				    break;

		case TCL_LEVEL0_RETURN:
				    strcpy( buffer, "TCL_LEVEL0_RETURN");
				    break;
	    }

	    ErrorLogger( NO_ERR_DEBUG, LOC, " Result: '", buffer, "'", NULL);
	}
#endif

        switch( result) {

            case TCL_OK:	gotPartial = 0;
			        continue;	/** while **/
	    
            case TCL_ERROR:	interp->errorLine = ((linenum-1)-gotPartial) +
				    interp->errorLine;
	    			/* FALLTHROUGH */

            case TCL_LEVEL0_RETURN:
	    			break;	/** switch **/
	}

	/**
	 **  If the while loop hasn't been continued so far, it is to be broken
	 **  now
	 **/

	break;	/** while **/

    } /** while **/

    /**
     **  Free up what has been used, close the input file and return the result
     **  of the last command to the caller
     **/

    Tcl_DStringFree( &cmdbuf);
    if( EOF == fclose( infile))
	if( OK != ErrorLogger( ERR_CLOSE, LOC, filename, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_Execute_TclFile, NULL);
#endif

    return( result);

} /** End of 'Execute_TclFile' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		CallModuleProcedure				     **
 ** 									     **
 **   Description:	Call a Tcl Procedure				     **
 **			Executes the passed modulefile (conditionally hidden)**
 **			and then evaluates the passed Tcl procedure	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **			Tcl_DString	*cmdptr		Buffer fot the Tcl   **
 **							command		     **
 **			char		*modulefile	According module file**
 **			char		*procname	Name of the Tcl Proc.**
 **			int		 suppress_output  Controlls redirect.**
 **							of stdout and stderr **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int  CallModuleProcedure(	Tcl_Interp	*interp,
			 	Tcl_DString	*cmdptr,
			 	char		*modulefile,
			 	char		*procname,
			 	int		 suppress_output)
{
    char 	 cmdline[ LINELENGTH];
    char	*cmd;
    int          result;
    int          saved_stdout = 0, saved_stderr = 0, devnull;

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_START, LOC, _proc_CallModuleProcedure, NULL);
#endif

    /**
     **  Must send stdout and stderr to /dev/null until the 
     **  ModulesHelp procedure is called.
     **/

    if( suppress_output) {
	if( 0 > (devnull = open( _fil_devnull, O_RDWR))) {
	    if( OK != ErrorLogger( ERR_OPEN, LOC, _fil_devnull, "changing", NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	}
	
	/**
	 **  Close STDOUT and reopen it as /dev/null
	 **/

	if( -1 == ( saved_stdout = dup( 1)))
	    if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stdout, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
	
	if( -1 == close( 1))
	    if( OK != ErrorLogger( ERR_CLOSE, LOC, _fil_stdout, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/

	if( -1 == dup( devnull))
	    if( OK != ErrorLogger( ERR_DUP, LOC, _fil_devnull, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
	
	/**
	 **  Close STDERR and reopen it as /dev/null
	 **/

	if( -1 == ( saved_stdout = dup( 2)))
	    if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stderr, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
	
	if( -1 == close( 2))
	    if( OK != ErrorLogger( ERR_CLOSE, LOC, _fil_stderr, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/

	if( -1 == dup( devnull))
	    if( OK != ErrorLogger( ERR_DUP, LOC, _fil_devnull, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
    }

    /**
     **  Read the passed module file
     **/

    Read_Modulefile( interp, modulefile);

    /**
     **  Reinstall stdout and stderr
     **/

    if( suppress_output) {

	/**
	 **  Reinstall STDOUT
	 **/

	if( EOF == fflush( stdout))
	    if( OK != ErrorLogger( ERR_FLUSH, LOC, _fil_stdout, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/

	if( EOF == fflush( stderr))
	    if( OK != ErrorLogger( ERR_FLUSH, LOC, _fil_stderr, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
	
	if( -1 == close( 1))
	    if( OK != ErrorLogger( ERR_CLOSE, LOC, _fil_stdout, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/

	/**
	 **  Reinstall STDERR
	 **/

	if( -1 == dup( saved_stdout))
	    if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stdout, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
	
	if( -1 == close( 2))
	    if( OK != ErrorLogger( ERR_CLOSE, LOC, _fil_stderr, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
	
	if( -1 == dup( saved_stderr))
	    if( OK != ErrorLogger( ERR_DUP, LOC, _fil_stderr, NULL))
		return( TCL_ERROR);	/** ------- EXIT (FAILURE) --------> **/
    }	

    /**
     **  Now evaluate the Tcl Procedure
     **/

    /* sprintf( cmdline, "%s\n", procname); */
    strcpy( cmdline, procname);
    strcat( cmdline, "\n");
    cmd = Tcl_DStringAppend( cmdptr, cmdline, (-1));

    result = Tcl_Eval( interp, cmd);
    Tcl_DStringTrunc( cmdptr, 0);

#if WITH_DEBUGGING_UTIL_1
    ErrorLogger( NO_ERR_END, LOC, _proc_CallModuleProcedure, NULL);
#endif

    return( result);

} /** End of 'CallModuleProcedure' **/
