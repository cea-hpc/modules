/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdTrace.c					     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The Tcl module-trace routine which provides a con-   **
 **			trolling interface to the modulecmd tracing feature  **
 ** 									     **
 **   Exports:		cmdModuleTrace					     **
 **			GetTraceSel					     **
 **			CheckTracing					     **
 **			CheckTracingList				     **
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

static char Id[] = "@(#)$Id: cmdTrace.c,v 1.8.20.1 2010/11/11 18:23:18 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

typedef	struct	_mod_trace	{
    char	 **re_ptr;		/** Pointer to the reqular expr.     **/
					/** which identifies the command     **/
    char const	 *cmd;			/** The spelled command for printing **/
    char	 *tracing;		/** The tracing setup	  	     **/
    char	  alloc;		/** Alloc flag		    	     **/
} ModTrace;

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

#ifdef	WITH_TRACE_LOAD
#   define	MOD_TR_LOAD	WITH_TRACE_LOAD
#else
#   define	MOD_TR_LOAD	_all_off
#endif

#ifdef  WITH_TRACE_UNLOAD
#   define	MOD_TR_UNLOAD	WITH_TRACE_UNLOAD
#else
#   define	MOD_TR_UNLOAD	_all_off
#endif

#ifdef  WITH_TRACE_SWITCH
#   define	MOD_TR_SWITCH	WITH_TRACE_SWITCH
#else
#   define	MOD_TR_SWITCH	_all_off
#endif

#ifdef  WITH_TRACE_DISP
#   define	MOD_TR_DISP	WITH_TRACE_DISP
#else
#   define	MOD_TR_DISP	_all_off
#endif

#ifdef  WITH_TRACE_LIST
#   define	MOD_TR_LIST	WITH_TRACE_LIST
#else
#   define	MOD_TR_LIST	_all_off
#endif

#ifdef  WITH_TRACE_AVAIL
#   define	MOD_TR_AVAIL	WITH_TRACE_AVAIL
#else
#   define	MOD_TR_AVAIL	_all_off
#endif

#ifdef  WITH_TRACE_HELP
#   define	MOD_TR_HELP	WITH_TRACE_HELP
#else
#   define	MOD_TR_HELP	_all_off
#endif

#ifdef  WITH_TRACE_INIT
#   define	MOD_TR_INIT	WITH_TRACE_INIT
#else
#   define	MOD_TR_INIT	_all_off
#endif

#ifdef  WITH_TRACE_USE
#   define	MOD_TR_USE	WITH_TRACE_USE
#else
#   define	MOD_TR_USE	_all_off
#endif

#ifdef  WITH_TRACE_UNUSE
#   define	MOD_TR_UNUSE	WITH_TRACE_UNUSE
#else
#   define	MOD_TR_UNUSE	_all_off
#endif

#ifdef  WITH_TRACE_UPDATE
#   define	MOD_TR_UPDATE	WITH_TRACE_UPDATE
#else
#   define	MOD_TR_UPDATE	_all_off
#endif

#ifdef  WITH_TRACE_PURGE
#   define	MOD_TR_PURGE	WITH_TRACE_PURGE
#else
#   define	MOD_TR_PURGE	_all_off
#endif

#ifdef  WITH_TRACE_CLEAR
#   define	MOD_TR_CLEAR	WITH_TRACE_CLEAR
#else
#   define	MOD_TR_CLEAR	_all_off
#endif

#ifdef  WITH_TRACE_APROPOS
#   define	MOD_TR_APROPOS	WITH_TRACE_APROPOS
#else
#   define	MOD_TR_APROPOS	_all_off
#endif

#ifdef  WITH_TRACE_REFRESH
#   define	MOD_TR_REFRESH	WITH_TRACE_REFRESH
#else
#   define	MOD_TR_REFRESH	_all_off
#endif

#ifdef  WITH_TRACE_WHATIS
#   define	MOD_TR_WHATIS	WITH_TRACE_WHATIS
#else
#   define	MOD_TR_WHATIS	_all_off
#endif

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "cmdTrace.c";	/** File name of this module **/
#if WITH_DEBUGGING_CALLBACK
static	char	_proc_cmdModuleTrace[] = "cmdModuleTrace";
#endif

/**
 **  Tracing strings
 **  contain a colon separated list of 'switch on/switch off' selector for
 **  module files. Each selector beginning with a '+' means 'tracing on', '-'
 **  means 'tracing off'.
 **  The +/- switch is followed by the name of the modulefile to which it
 **  belongs. All valif TCL regexps may be specified here.
 **/

static	char	_all[] = ".*";
static	char	_all_on[] = "+.*";
static	char	_all_off[] = "-.*";

/** 
 **  The tracing selection table
 **/

static	ModTrace	TraceSelect[] = {
    { &addRE,		"load",		MOD_TR_LOAD,	0 },	/**  0 **/
    { &rmRE,		"unload",	MOD_TR_UNLOAD,	0 },	/**  1 **/
    { &swRE,		"switch",	MOD_TR_SWITCH,	0 },	/**  2 **/
    { &dispRE,		"display",	MOD_TR_DISP,	0 },	/**  3 **/
    { &listRE,		"list",		MOD_TR_LIST,	0 },	/**  4 **/
    { &availRE,		"avail",	MOD_TR_AVAIL,	0 },	/**  5 **/
    { &helpRE,		"help",		MOD_TR_HELP,	0 },	/**  6 **/
    { &initRE,		"init",		MOD_TR_INIT,	0 },	/**  7 **/
    { &useRE,		"use",		MOD_TR_USE,	0 },	/**  8 **/
    { &unuseRE,		"unuse",	MOD_TR_UNUSE,	0 },	/**  9 **/
    { &updateRE,	"update",	MOD_TR_UPDATE,	0 },	/** 10 **/
    { &purgeRE,		"purge",	MOD_TR_PURGE,	0 },	/** 11 **/
    { &clearRE,		"clear",	MOD_TR_CLEAR,	0 },	/** 12 **/
    { &whatisRE,	"whatis",	MOD_TR_WHATIS,	0 },	/** 13 **/
    { &aproposRE,	"apropos",	MOD_TR_APROPOS,	0 },	/** 14 **/
    { &refreshRE,	"refresh",	MOD_TR_REFRESH,	0 }	/** 15 **/
};
/** ************************************************************************ **/

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	int	GetTraceTable(	Tcl_Interp *interp,
				char *cmd,
				int num);

static	int	ChangeTraceSel(	Tcl_Interp *interp,
				char	*cmd_tab,
				int	 cmd_tab_size,
				char	 on_off,
				char	*module_pat);

static	int	CheckTracingPat(	Tcl_Interp *interp,
					char	*pattern,
					char	*modulefile);

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleTrace					     **
 ** 									     **
 **   Description:	Callback function for 'trace'			     **
 ** 									     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	TraceSelect	List containing all tracing settings **
 **   			g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	cmdModuleTrace(	ClientData	 client_data,
	      		Tcl_Interp	*interp,
	      		int		 argc,
	      		CONST84 char	*argv[])
{
    char 	  on_off = '+';		/** The first argument		     **/
    char	**args;			/** Argument pointer for scanning    **/
    int 	  i, k;			/** Loop counter		     **/
    int		  cmd_tab_size;
    char	 *cmd_table;		/** Command table		     **/
    int		  ret = TCL_OK;

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_START, LOC, _proc_cmdModuleTrace, NULL);
#endif

    /**
     **  Whatis mode?
     **/
    if( g_flags & (M_WHATIS | M_HELP))
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
	
    /**
     **  Parameter check
     **/
    if( argc < 2) {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "on|off",
	    "[cmd [cmd ...]]", "[ -module module [module ...]]", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  On or off?
     **/
    if( !strcmp( argv[ 1], "on"))
	on_off = '+';
    else if( !strcmp( argv[ 1], "off"))
	on_off = '-';
    else {
	if( OK != ErrorLogger( ERR_USAGE, LOC, argv[0], "on|off",
	    "[cmd [cmd ...]]", "[ -module module [module ...]]", NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
 
  
    /**
     **  Non-persist mode?
     **/
    
    if (g_flags & M_NONPERSIST) {
	return (TCL_OK);
    }

    /**
     **  Display mode?
     **/
    if( g_flags & M_DISPLAY) {
	fprintf( stderr, "%s\t ", argv[ 0]);
	for( i=1; i<argc; i++)
	    fprintf( stderr, "%s ", argv[ i]);
	fprintf( stderr, "\n");
        return( TCL_OK);		/** ------- EXIT PROCEDURE -------> **/
    }

    /**
     **  We need a table of module command involved in this trace selection.
     **  Allocate one and initialize it.
     **/
    cmd_tab_size = sizeof( TraceSelect) / sizeof( TraceSelect[ 0]);
    if((char *) NULL == (cmd_table = (char *) module_malloc( cmd_tab_size)))
	return((OK == ErrorLogger( ERR_ALLOC, LOC, NULL)) ? TCL_OK : TCL_ERROR);

    /**
     **  Scan all commands specified as options. The last option to be scanned
     **  is either the real last one, or the '-module' one
     **/
    args = (char **) argv + 2; i = argc - 2;
    memset( cmd_table, !i, cmd_tab_size);

    while( i--) {
	char *tmp = *args++;

	/**
	 **  Check for '-module'
	 **/
	if( !strncmp( tmp, "-module", strlen( tmp)))
	    break;

	/**
	 **  This should be a module command.
	 **  Check it against the TraceSelect table
	 **/
	if( -1 != (k = GetTraceTable(interp, tmp, cmd_tab_size))) {
	    cmd_table[ k] = 1;
	} else {
	    if( OK != ErrorLogger( ERR_COMMAND, LOC, tmp, NULL)) {
		null_free((void *) &cmd_table);
		return( TCL_ERROR);
	    }
	}

    } /** while( i--) **/

    /**
     **  Now scan all Modulefiles concerned in the current command
     **  If we ran to the end of the argument list (i==0), ALL files are
     **  concerned in this ...
     **/
    if( 0 >= i) {	
	ret = ChangeTraceSel(interp, cmd_table, cmd_tab_size, on_off, _all);
    } else {
	while( i-- && TCL_OK == ret) 
	    ret = ChangeTraceSel(interp, cmd_table, cmd_tab_size,
		on_off, *args++);
    }

    /**
     **  Cleanup finally and return
     **/
    null_free((void *) &cmd_table);

#if WITH_DEBUGGING_CALLBACK
    ErrorLogger( NO_ERR_END, LOC, _proc_cmdModuleTrace, NULL);
#endif

    return( ret);

} /** End of 'cmdModuleTrace' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		GetTraceTable					     **
 ** 									     **
 **   Description:	Returns the TraceSelect index for the passed module  **
 **			subcommand					     **
 ** 									     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Parameters:	char	*cmd		Subcommand to be checked     **
 **			int	 num		Number of commands to be chk **
 ** 									     **
 **   Result:		int	>= 0		Successful completion	     **
 **				-1		Any error		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	GetTraceTable(Tcl_Interp *interp, char *cmd, int num)
{
    int k;

    for( k=0; k < num; k++) {
	if( Tcl_RegExpMatch(interp, cmd, *(TraceSelect[k].re_ptr))) 
	    return( k);			/** ----------- Got it ------------> **/
    } /** for( k) **/

    /**
     **  Not found ..
     **/
    return( -1);

} /** End of 'GetTraceTable' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		GetTraceSel					     **
 ** 									     **
 **   Description:	Retrieve the trace selection pattern for the passed  **
 **			module command					     **
 ** 									     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Parameters:	char	*cmd		Subcommand to be checked     **
 ** 									     **
 **   Result:		char *	NULL		Module subcommand not found  **
 **				Otherwise	Assigned trace pattern	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char	*GetTraceSel(	Tcl_Interp *interp,
			char	*cmd)
{
    int k;

    if( -1 == (k = GetTraceTable(interp, cmd,
	(sizeof( TraceSelect) / sizeof( TraceSelect[ 0]) ))))
	return((char *) NULL);

    return( TraceSelect[ k].tracing);

} /** End of 'GetTraceSel' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ChangeTraceSel					     **
 ** 									     **
 **   Description:	Change the trace selection for all commands speci-   **
 **			fied in the passed 'cmd_table'. The passed module-   **
 **			name has to be changed according 'on_off'	     **
 ** 									     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Parameters:	char	*cmd_table	Boolean array indicating all **
 **						commands in the TraceSelect  **
 **						table to be changed	     **
 **			int	 cmd_tab_size	Size of this array	     **
 **			char	 on_off		'+' switch tracing on	     **
 **						'-' switch tracing off       **
 **			char	*module_pat	Pattern for the affected     **
 **						module files		     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	ChangeTraceSel(	Tcl_Interp *interp,
				char	*cmd_table,
				int	 cmd_tab_size,
				char	 on_off,
				char	*module_pat)
{
    char	*pattern, *s, *t, *tmp;
    int		 len = strlen( module_pat);
    int 	 i;
    int		 ret = TCL_OK;

    /**
     **  Need a buffer for to build the complete pattern
     **/
    if((char *) NULL == (pattern = (char *) module_malloc(len + 2))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	return( TCL_ERROR);
    }

    /**
     **  Check if this is the ALL pattern. If it is, replace all affected
     **  entries with '_all_on' or '_all_off'
     **/
    if( !strcmp( module_pat, _all)) {

	for( i=0; i < cmd_tab_size; i++) {
	    if( cmd_table[ i]) {

		if( TraceSelect[ i].alloc)
		    null_free((void *) &(TraceSelect[ i].tracing));
		TraceSelect[ i].alloc = 0;

		TraceSelect[ i].tracing = ('+' == on_off) ?
		    _all_on : _all_off;
	    }
	}

    } else { /** if( ALL pattern) **/

	/**
	 **  Check the pattern for colons ...
	 **/
	if( strchr( module_pat, ':')) 
	    if( OK != ErrorLogger( ERR_COLON, LOC, module_pat, NULL))
		ret = TCL_ERROR;

	if( TCL_OK == ret) {

	    /**
	     **  Build the complete pattern
	     **/
	    *pattern = on_off;
	    strcpy( pattern + 1, module_pat);
	    len += 1;

	    /**
	     **  Loop for all entries to be changed
	     **/
	    for( i=0; i < cmd_tab_size; i++) {
		if( cmd_table[ i]) {

		    /**
		     **  allocate a buffer for the new pattern
		     **/
		    if((char *) NULL == (tmp = (char *) module_malloc(len + 2 +
			strlen( TraceSelect[ i].tracing)))) {
			if( OK == ErrorLogger( ERR_ALLOC, LOC, NULL)) {
			    continue;
			} else {
			    ret = TCL_ERROR;
			    break;
			}
		    }

		    /**
		     **  Copy the new pattern to the buffer at first
		     **/
		    strcpy( tmp, pattern);
		    t = tmp + len;

		    /**
		     **  Tokenize the old pattern and remove duplicates
		     **/
		    for( s = strtok( TraceSelect[ i].tracing, ":");
			 s;
			 s = strtok( NULL, ":") ) {

			if( strcmp( (s+1), module_pat)) {
			    *t++ = ':';
			    while( *t++ = *s++);
			    t--;
			}

		    } /** for **/

		    /**
		     **  Finally check if we have to dealloc and set up the
		     **  new pattern
		     **/

		    if( TraceSelect[ i].alloc)
			null_free((void *) &(TraceSelect[ i].tracing));

		    TraceSelect[ i].tracing = tmp;
		    TraceSelect[ i].alloc = 1;

		} /** if **/
	    } /** for **/

	} /** if( ret) **/
    } /** if( ALL pattern) **/
	
    /**
     **  Free what has been allocated an return 
     **/
    null_free((void *) &pattern);
    return( ret);

} /** End of 'ChangeTraceSel' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		CheckTracing					     **
 ** 									     **
 **   Description:	Check wheter thracing is turned on for the passed    **
 **			command and modulefile				     **
 ** 									     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Parameters:	char	*cmd		Subcommand to be checked     **
 **			char	*modulefile	Modulefile to be checked     **
 ** 									     **
 **   Result:		int	0		No tracing		     **
 **				1		Tracing enabled		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	CheckTracing(	Tcl_Interp *interp,
			char	*cmd,
			char	*modulefile)
{
    int 	 k;

    /**
     **  Get the TraceSelect table index
     **/

    if( -1 == (k = GetTraceTable(interp, cmd,
	(sizeof( TraceSelect) / sizeof( TraceSelect[0]) )))) {
	ErrorLogger( ERR_COMMAND, LOC, cmd, NULL);
	return( 0);
    }

    /**
     **  Now check this guy ...
     **/

    return( CheckTracingPat(interp, TraceSelect[ k].tracing, modulefile));

} /** End of 'CheckTracing' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		CheckTracingPat					     **
 ** 									     **
 **   Description:	Check the passed pattern if it enables tracing for   **
 **			the passed module file				     **
 ** 									     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Parameters:	char	*pattern	Pattern to be checked	     **
 **			char	*modulefile	Modulefile to be checked     **
 ** 									     **
 **   Result:		int	0		No tracing		     **
 **				1		Tracing enabled		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	CheckTracingPat(	Tcl_Interp *interp,
					char	*pattern,
					char	*modulefile)
{
    char	*s;
    int		 ret;

    /**
     **  Tokenize the pattern and check if it matches ...
     **/

    for( s = strtok( pattern, ":");
	 s;
	 s = strtok( NULL, ":") ) {

	ret = ('+' == *s++);
	if( Tcl_RegExpMatch(interp, modulefile, s))
	    return( ret);

    } /** for **/

    /**
     **  No pattern matched the module file name ...
     **/

    return( 0);

} /** End of 'CheckTracingPat' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		CheckTracingList				     **
 ** 									     **
 **   Description:	Check wheter tracing is turned on for the passed     **
 **			command and at least one of the passed modulefiles   **
 ** 									     **
 **   First Edition:	1995/12/26					     **
 ** 									     **
 **   Parameters:	char	*cmd		Subcommand to be checked     **
 **			int	 count		Number of passed modulefiles **
 **			char	**modules	Modulefiles to be checked    **
 ** 									     **
 **   Result:		int	0		No tracing		     **
 **				1		Tracing enabled		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	CheckTracingList(	Tcl_Interp *interp,
				char	 *cmd,
				int	  count,
				char	**modules)
{
    int 	 k;

    /**
     **  Get the TraceSelect table index
     **/

    if( -1 == (k = GetTraceTable(interp, cmd,
	(sizeof( TraceSelect) / sizeof( TraceSelect[0]) )))) {
	ErrorLogger( ERR_COMMAND, LOC, cmd, NULL);
	return( 0);
    }

    /**
     **  Now check whether one of them requires tracing
     **/

    while( count--)
        if( CheckTracingPat(interp, TraceSelect[ k].tracing, *modules++))
	    return( 1);

    return( 0);

} /** End of 'CheckTracingList' **/

