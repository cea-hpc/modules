/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		error.c						     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	The modules error logger			     **
 ** 									     **
 **   Exports:		Module_Error					     **
 **			GetFacilityPtr					     **
 **			CheckFacility					     **
 **			Enable_Error					     **
 **			Disable_Error					     **
 **			Restore_Error					     **
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

static char Id[] = "@(#)$Id: error.c,v 1.8.4.2 2011/10/03 20:25:43 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"
#if HAVE_STDARG_H
#  include <stdarg.h>
#else
#  error "You need an ANSI C compiler"
#endif

#if defined(HAVE_SYSLOG) && defined(WITH_LOGGING)
#  include <syslog.h>
#endif

#include <pwd.h>
#include <grp.h>

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/**
 **  Error weights
 **/

typedef enum _err_weights	{
    WGHT_NONE=0,			/** Dummy value: No error	     **/
    WGHT_VERBOSE,			/** Verbose messages		     **/
    WGHT_INFO=3,			/** Informal message	 	     **/
    WGHT_DEBUG=5,			/** Debugger output		     **/
    WGHT_TRACE,				/** Tracing output		     **/
    WGHT_WARN=10,			/** Warning: Prog. flow not affected **/
    WGHT_PROB=20,			/** Problem: Prog. flow cond. aff.   **/
    WGHT_ERROR=25,			/** Error and Fatal: Prog. flow aff. **/
    WGHT_FATAL=27,			/**   Return to the caller	     **/
    WGHT_PANIC=29			/** Panic: Exit program immediatelly **/
} ErrWeights;

/**
 **  Log facilities
 **/

typedef	struct	_err_facility	{
    ErrWeights	 Weight;		/** Error weight 		     **/
    char	*facility;		/** Log facility		     **/
    char	*def_facility;		/** Default facility (facility undef)**/
} ErrFacilities;

typedef struct	_facil_names	{
    char 	*name;			/** Name of a facility		     **/
    int		 token;			/** Assigned token		     **/
} FacilityNames;

/**
 **  Error measurement table
 **/

typedef	struct	{
    ErrWeights	 error_weight;		/** The weight itsself		     **/
    char	*message;		/** Message to be printed	     **/
    ErrCode	 ret_nov,		/** Return code			     **/
		 ret_adv,
		 ret_exp;
} ErrMeasr;

/**
 **  Error code translation table
 **/

typedef struct  { 
    ErrType	  error_type;		/** The error type as specified by   **/
					/** the caller			     **/
    ErrWeights    error_weight;		/** The weight of this error         **/
    char	 *messages;		/** List of messages to be printed   **/
} ErrTransTab;

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

#define	ARGLIST_SIZE	10
#define	ERR_LINELEN	80		/** internal buffer size	     **/
#define ERR_BUFSIZE	4096		/** buffer for the whole error msg.  **/

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "error.c";	/** File name of this module **/

/**
 **  Local flags
 **/
static	int	quiet_on_error = 0;

/**
 **  Local strings
 **/

static	char	unknown[] = "unknown";		/** If something's unknown   **/

static	char	 buffer[ ERR_LINELEN];		/** Internal string buffer   **/
	char	*error_line = NULL;
static	int	 strsize = 0;

/**
 **  Log facility table
 **/

static	char	_stderr[] = "stderr";
static	char	_stdout[] = "stdout";
static	char	_null[] = "null";
static	char	_none[] = "none";
static	char	_unknown[] = "unknown";

static	ErrFacilities	Facilities[] = {
    { WGHT_NONE,	NULL,	NULL },
    { WGHT_VERBOSE,	NULL,	DEF_FACILITY_VERBOSE },
    { WGHT_INFO,	NULL,	DEF_FACILITY_INFO },
    { WGHT_DEBUG,	NULL,	DEF_FACILITY_DEBUG },
    { WGHT_TRACE,	NULL,	DEF_FACILITY_TRACE },
    { WGHT_WARN,	NULL,	DEF_FACILITY_WARN },
    { WGHT_PROB,	NULL,	DEF_FACILITY_PROB },
    { WGHT_ERROR,	NULL,	DEF_FACILITY_ERROR },
    { WGHT_FATAL,	NULL,	DEF_FACILITY_FATAL },
    { WGHT_PANIC,	NULL,	DEF_FACILITY_PANIC }
};

/**
 **  Syslog facility names
 **/

static	FacilityNames	facility_names[] = {
#if defined(HAVE_SYSLOG) && defined(WITH_LOGGING)
    { "auth", LOG_AUTH },
    { "cron", LOG_CRON },
    { "daemon", LOG_DAEMON },
    { "kern", LOG_KERN },
    { "local0", LOG_LOCAL0 },
    { "local1", LOG_LOCAL1 },
    { "local2", LOG_LOCAL2 },
    { "local3", LOG_LOCAL3 },
    { "local4", LOG_LOCAL4 },
    { "local5", LOG_LOCAL5 },
    { "local6", LOG_LOCAL6 },
    { "local7", LOG_LOCAL7 },
    { "lpr", LOG_LPR },
    { "mail", LOG_MAIL },
    { "news", LOG_NEWS },
    { "user", LOG_USER },
    { "uucp", LOG_UUCP }
#else
    { "none", 0 }
#endif
};

/**
 **  Syslog level names
 **/

static	FacilityNames	level_names[] = {
#if defined(HAVE_SYSLOG) && defined(WITH_LOGGING)
    { "alert", LOG_ALERT },
    { "crit", LOG_CRIT },
    { "debug", LOG_DEBUG },
    { "emerg", LOG_EMERG },
    { "err", LOG_ERR },
    { "info", LOG_INFO },
    { "notice", LOG_NOTICE },
    { "warning", LOG_WARNING }
#else
    { "none", 0 }
#endif
};

/**
 **  Error measurement table
 **  Take care that this list is sorted in ascending order concerning the error
 **  weights
 **/

#define	MEAS_VERB_NDX	1	/** Index of the 'VERBOSE' entry	    **/

static	ErrMeasr	Measurements[] = {
    { WGHT_NONE,   ""	  , OK,      OK,      OK },	
    { WGHT_VERBOSE,"VERB" , OK,      OK,      OK },
    { WGHT_INFO,   "INFO" , OK,      OK,      OK },
    { WGHT_DEBUG,  "DEBUG", OK,      OK,      OK },
    { WGHT_TRACE,  "TRACE", OK,      OK,      OK },
    { WGHT_WARN,   "WARN" , PROBLEM, WARN,    OK },
    { WGHT_PROB,   "PROB" , ERROR,   PROBLEM, OK },
    { WGHT_ERROR,  "ERROR", ERROR,   ERROR,   ERROR },
    { WGHT_FATAL,  "FATAL", FATAL,   FATAL,   FATAL },
    { WGHT_PANIC,  "PANIC", PANIC,   PANIC,   PANIC },
};

/**
 **  Error code translation table
 **  Take care that this list is sorted in ascending order concerning the error
 **  types
 **/

static	ErrTransTab	TransTab[] = {
    { NO_ERR,		WGHT_NONE,  NULL },
    { NO_ERR_DEBUG,	WGHT_DEBUG, "$*" },
    { NO_ERR_START,	WGHT_DEBUG, "Starting $*" },
    { NO_ERR_END,	WGHT_DEBUG, "Exit $*" },
    { NO_ERR_VERBOSE,	WGHT_VERBOSE,  NULL },
    { ERR_PARAM,	WGHT_ERROR, "Parameter error concerning '$1'" },
    { ERR_USAGE,	WGHT_ERROR, "Usage is '$*'" },
    { ERR_ARGSTOLONG,	WGHT_ERROR, "'$1': Arguments too long. Max. is '$2'" },
    { ERR_OPT_AMBIG,	WGHT_ERROR, "Option '$1' is ambiguous" },
    { ERR_OPT_NOARG,	WGHT_ERROR, "Option '$1' allows no argument" },
    { ERR_OPT_REQARG,	WGHT_ERROR, "Option '$1' requires an argument" },
    { ERR_OPT_UNKNOWN,	WGHT_ERROR, "Unrecognized option '$1'" },
    { ERR_OPT_ILL,	WGHT_ERROR, "Illegal option '$1'" },
    { ERR_OPT_INV,	WGHT_ERROR, "Invalid option '$1'" },
    { ERR_USERLVL,	WGHT_ERROR, "Undefined userlevel '$1'" },
    { ERR_GETOPT,	WGHT_FATAL, "getopt() failed" },
    { ERR_OPEN,		WGHT_ERROR, "Cannot open file '$1' for '$2'" },
    { ERR_POPEN,	WGHT_ERROR, "Cannot open pipe '$1' for '$2'" },
    { ERR_OPENDIR,	WGHT_ERROR, "Cannot open directory '$1' for reading" },
    { ERR_CLOSE,	WGHT_WARN,  "Cannot close file '$1'" },
    { ERR_PCLOSE,	WGHT_WARN,  "Cannot close pipe '$1'" },
    { ERR_CLOSEDIR,	WGHT_WARN,  "Cannot close directory '$1'" },
    { ERR_READ,		WGHT_ERROR, "Error while reading file '$1'" },
    { ERR_READDIR,	WGHT_ERROR, "Error while reading directory '$1'" },
    { ERR_WRITE,	WGHT_ERROR, "Error while writing file '$1'" },
    { ERR_SEEK,		WGHT_ERROR, "Seek error on file '$1'" },
    { ERR_FLUSH,	WGHT_WARN,  "Flush error on file '$1'" },
    { ERR_DUP,		WGHT_WARN,  "Cannot duplicate handle of file '$1'" },
    { ERR_DIRNAME,	WGHT_ERROR, "Cannot build directory name" },
    { ERR_NAMETOLONG,	WGHT_ERROR, "Requested directory name to long: "
				    "dir='$1',file='$2'" },
    { ERR_DIRNOTFOUND,	WGHT_ERROR, "Directory '$1' not found" },
    { ERR_FILEINDIR,	WGHT_ERROR, "File '$1' not found in directory '$2'" },
    { ERR_NODIR,	WGHT_ERROR, "'$1' is not a directory" },
    { ERR_UNLINK,	WGHT_WARN,  "Cannot unlink '$1'" },
    { ERR_RENAME,	WGHT_PROB,  "Cannot rename '$1' to '$2'" },
    { ERR_ALLOC,	WGHT_FATAL, "Out of memory." },
    { ERR_SOURCE,	WGHT_WARN,  "Error sourcing file '$1'" },
    { ERR_UNAME,	WGHT_FATAL, "'uname (2)' failed." },
    { ERR_GETHOSTNAME,	WGHT_FATAL, "'gethostname (2)' failed." },
    { ERR_GETDOMAINNAME,WGHT_FATAL, "'getdomainname (2)' failed." },
    { ERR_STRING,	WGHT_FATAL, "string error" },
    { ERR_DISPLAY,	WGHT_ERROR, "Cannot open display" },
    { ERR_PARSE,	WGHT_ERROR, "Parse error" },
    { ERR_EXEC,		WGHT_ERROR, "Tcl command execution failed: $1" },
    { ERR_EXTRACT,	WGHT_ERROR, "Cannot extract X11 resources" },
    { ERR_COMMAND,	WGHT_ERROR, "'$1' is an unrecognized subcommand" },
    { ERR_LOCATE,	WGHT_ERROR, "Unable to locate a modulefile for '$1'" },
    { ERR_MAGIC,	WGHT_ERROR, "Magic cookie '#%Module' missing in '$1'" },
    { ERR_MODULE_PATH,	WGHT_ERROR, "'MODULEPATH' not set" },
    { ERR_HOME,		WGHT_ERROR, "'HOME' not set" },
    { ERR_SHELL,	WGHT_ERROR, "Unknown shell type '$1'" },
    { ERR_DERELICT,	WGHT_ERROR, "Unknown shell derelict '$1'" },
    { ERR_CONFLICT,	WGHT_ERROR, "Module '$1' conflicts with the currently "
				    "loaded module(s) '$2*'" },
    { ERR_PREREQ,	WGHT_ERROR, "Module '$1' depends on one of the "
				    "module(s) '$2*'" },
    { ERR_NOTLOADED,	WGHT_ERROR, "Module '$1' is currently not loaded" },
    { ERR_DUP_SYMVERS,	WGHT_PROB,  "Duplicate version symbol '$1' found" },
    { ERR_SYMLOOP,	WGHT_ERROR, "Version symbol '$1' loops" },
    { ERR_BADMODNAM,	WGHT_PROB,  "Invalid modulename '$1' found" },
    { ERR_DUP_ALIAS,	WGHT_WARN,  "Duplicate alias '$1' found" },
    { ERR_CACHE_INVAL,	WGHT_ERROR, "Invalid cache version '$1' found" },
    { ERR_CACHE_LOAD,	WGHT_WARN,  "Couldn't load the cache properly" },
    { ERR_BEGINENV,	WGHT_WARN, "Invalid update subcommand - "
#ifdef BEGINENV
#  if BEGINENV == 99
	"No _MODULESBEGINENV_ file"
#  else
	"No .modulesbeginenv file"
#  endif
#else
	".modulesbeginenv not supported"
#endif
	},
    { ERR_BEGINENVX,	WGHT_WARN,
	"Invalid update subcommand - No MODULESBEGINENV - hence not supported"},
    { ERR_INIT_TCL,	WGHT_ERROR, "Cannot initialize TCL" },
    { ERR_INIT_TCLX,	WGHT_WARN,  "Cannot initialize TCLX modules using "
				    "extended commands might fail" },
    { ERR_INIT_ALPATH,	WGHT_WARN,  "Could not extend auto_path variable. "
				    "Module using autoloadable functions might "
				    "fail" },
    { ERR_INIT_STUP,	WGHT_WARN,  "Cannot find a 'module load' command in "
				    "any of the '$1' startup files" },
    { ERR_SET_VAR,	WGHT_WARN,  "Cannot set TCL variable '$1'" },
    { ERR_INFO_DESCR,	WGHT_ERROR, "Unrecognized module info descriptor '$1'" },
    { ERR_INVWGHT_WARN,	WGHT_WARN,  "Invalid error weight '$1' found" },
    { ERR_INVFAC_WARN,	WGHT_WARN,  "Invalid log facility '$1'" },
    { ERR_COLON,	WGHT_WARN,  "Spurious colon in pattern '$1'" },
    { ERR_INTERAL,	WGHT_PANIC, "Internal error in the alias handler" },
    { ERR_INTERRL,	WGHT_PANIC, "Internal error in the error logger" },
    { ERR_INVAL,	WGHT_PANIC, "Invalid error type '$1' found" },
    { ERR_INVWGHT,	WGHT_PANIC, "Invalid error weight '$1' found" },
    { ERR_INVFAC,	WGHT_PANIC, "Invalid log facility '$1'" },
    { ERR_ENVVAR,       WGHT_FATAL, "The environment variables LOADEDMODULES and _LMFILES_ have inconsistent lengths." }
};

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	ErrTransTab	*ErrorLookup(	ErrType error_type );

static	ErrMeasr	*MeasLookup(	ErrWeights weigth );

static	int	FlushError(	ErrType		  Type,
				char		 *module,
				int		  lineno,
				ErrWeights	  Weight,
				char		 *WeightMsg,
				char		 *ErrMsgs,
				int		  argc,
				char		**argv);

static	int	PrintError(	char		 *buffer,
				ErrType		  Type,
				char		 *module,
				int		  lineno,
				ErrWeights	  Weight,
				char		 *WeightMsg,
				char		 *ErrMsgs,
				int		  argc,
				char		**argv);

static	char	*ErrorString(	char		 *ErrMsgs,
				int		  argc,
				char		**argv);

static	void	add_param(	char		**Control,
				char		**Target,
				int		 *Length,
				int		  argc,
				char		**argv);

static	int	scan_facility(	char		 *s,
				FacilityNames	 *table,
				int		  size);

static	char	*GetFacility(	ErrWeights	  Weight);
static	ErrFacilities	*GetFacility_sub( ErrWeights Weight);

static	void Print_Tracing(	char	 *buffer,
				int	  result,
			   	int	  argc,
			   	char	**argv);

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Module_Tracing					     **
 **			Print_Tracing					     **
 **			Module_Verbosity				     **
 ** 									     **
 **   Description:	Display a tracing or verbose message		     **
 ** 									     **
 **   First Edition:	1995/12/27					     **
 ** 									     **
 **   Parameters:	int	result	Result code of th module command     **
 **			int	argc	Number od arguments to the module    **
 **					command				     **
 **			char	**argv	Argument array			     **
 **			char	*buffer	Print buffer			     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void Module_Tracing(	int	result,
		   	int	argc,
		   	char	**argv)
{
    if( !FlushError( NO_ERR, (char *) NULL, result, WGHT_TRACE, (char *) NULL,
	(char *) NULL, argc, argv)) {
	ErrorLogger( ERR_INTERRL, LOC, NULL);
    }

} /** End of 'Module_Tracing' **/

void Module_Verbosity(	int	argc,
		   	char	**argv)
{
    if( sw_verbose && argc && *argv)
	if( !FlushError( NO_ERR_VERBOSE, g_current_module,linenum,WGHT_VERBOSE,
	    Measurements[ MEAS_VERB_NDX].message, *argv, argc, argv)) {
	    ErrorLogger( ERR_INTERRL, LOC, NULL);
	}

} /** End of 'Module_Verbosity' **/

static	void Print_Tracing(	char	 *buffer,
				int	  result,
			   	int	  argc,
			   	char	**argv)
{
    char 		*s = buffer;
    struct passwd 	*pwent;
    struct group 	*grpent;
    uid_t 		 uid;
    gid_t 		 gid;

    /**
     **  Print the arguments
     **/

    while( argc--) {
	/* sprintf( s, "%s ", *argv++); */
	strcpy( s, *argv++);
	strcat( s, " ");
	s += strlen( s);
    }

    /**
     **  Add the real and effective user- and group-id
     **/

    pwent = getpwuid( uid = getuid());
    sprintf( s, " [%s(%d)", (pwent ? pwent->pw_name : _unknown), uid);
    s += strlen( s);
    grpent = getgrgid( gid = getgid());
    sprintf( s, ".%s(%d)]", (grpent ? grpent->gr_name : _unknown), gid);
    s += strlen( s);

    pwent = getpwuid( uid = geteuid());
    sprintf( s, " [%s(%d)", (pwent ? pwent->pw_name : _unknown), uid);
    s += strlen( s);
    grpent = getgrgid( gid = getegid());
    sprintf( s, ".%s(%d)] = ", (grpent ? grpent->gr_name : _unknown), gid);
    s += strlen( s);

    /**
     **  Add the commands result
     **/

    switch( result) {
	case TCL_OK:
	    strcpy( s, "TCL_OK");
	    break;
	case TCL_ERROR:
	    strcpy( s, "TCL_ERROR");
	    break;
	case TCL_RETURN:
	    strcpy( s, "TCL_RETURN");
	    break;
	case TCL_BREAK:
	    strcpy( s, "TCL_BREAK");
	    break;
	case TCL_CONTINUE:
	    strcpy( s, "TCL_CONTINUE");
	    break;
	default:
	    strcpy( s, "UNKNOWN");
	    break;
    }

    s += strlen( s);
    sprintf( s, "(%d)", result);

} /** End of 'Module_Tracing' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Enable_Error, Disable_Error, Restore_Error	     **
 ** 									     **
 **   Description:	Enables, disables, or restores error logging	     **
 ** 			Sometimes an error isn't really an error	     **
 ** 									     **
 **   First Edition:	1999/11/11					     **
 ** 									     **
 **   Parameters:	none						     **
 ** 									     **
 **   Result:		none						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static void save_error_state(int reset) {
	static int in_effect = 0;
	static int saved_state = 0;

	if (reset && in_effect ) {
		quiet_on_error = saved_state;
		in_effect = 0;
	} else if (!reset && !in_effect ) {
		saved_state = quiet_on_error;
		in_effect = 1;
	}
}


void Enable_Error(void) {
	save_error_state(0);
	quiet_on_error = 0;
}

void Disable_Error(void) {
	save_error_state(0);
	quiet_on_error = 1;
}

void Restore_Error(void) {
	quiet_on_error = 0;	/* the default is to output errors */
	save_error_state(1);
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Module_Error					     **
 ** 									     **
 **   Description:	Error handling for the modules package		     **
 ** 									     **
 **   First Edition:	1995/08/06					     **
 ** 									     **
 **   Parameters:	ErrType		 error_type	Type of the error    **
 **			char		*module		Affected module	     **
 **			int		 lineo		Line number	     **
 **			...				Argument list	     **
 ** 									     **
 **   Result:		ErrCode		OK		No error	     **
 **					PROBLEM		Problem. Program may **
 **							continue running     **
 **					ERROR		Caller should try to **
 **							exit gracefully      **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Module_Error(	ErrType		  error_type,
		   	char		 *module,
		   	int		  lineno,
		   	... )
{
    int		  listsize = ARGLIST_SIZE,	/** Initial size of the argu-**/
						/** ment list		     **/
		  argc = 0;			/** Actual number of args    **/
    char	**argv,				/** Argument array	     **/
		 *arg;				/** A single argument	     **/
    va_list	  parptr;			/** Varargs scan pointer     **/
    ErrTransTab	 *TransPtr;			/** Error transtab entry     **/
    ErrMeasr	 *MeasPtr;			/** Measurement pointer      **/
    ErrCode	  ret_code;
    int		  NoArgs = (error_type == ERR_ALLOC);

    if( quiet_on_error ) return OK;		/* do nothing - just OK */

    /**
     **  Argument check
     **/
    if( NO_ERR_VERBOSE == error_type && !sw_verbose)
	return( OK);

    if( !module)
	module = unknown;

    /**
     **  Build the argument array at first
     **/

    if( NULL == (argv = (char **) module_malloc(listsize * sizeof( char *)))) {
	module = module_name;
	error_type = ERR_ALLOC;
	NoArgs = 1;
    }

    va_start( parptr, lineno);
    while( !NoArgs && (NULL != (arg = va_arg( parptr, char *)))) {

	/**
	 **  Conditionally realloc
	 **/
	while( argc >= listsize) {
	    listsize += ARGLIST_SIZE;
	    if(!(argv = (char **) module_realloc( argv,
		listsize * sizeof(char *)))) {
		module = module_name;
		error_type = ERR_ALLOC;
		NoArgs = 1;
		break;
	    }
	}

	argv[ argc++] = arg;

    } /** while **/
    
    if( NO_ERR_VERBOSE == error_type && !argc)
	return( OK);

    /**
     **  Locate the error translation table entry according to the 
     **  passed error type
     **/
    if( NULL == (TransPtr = ErrorLookup( error_type))) {
	if( argv)
	    null_free((void *) &argv);
	return( ErrorLogger( ERR_INVAL, LOC, (sprintf( buffer, "%d",
	    error_type), buffer), NULL));
    }
    
    /**
     **  Now locate the assigned error weight ...
     **/

    if( NULL == (MeasPtr = MeasLookup( TransPtr->error_weight))) {
	if( argv)
	    null_free((void *) &argv);
	argc = 0;
	return( ErrorLogger( ERR_INVWGHT, LOC, (sprintf( buffer, "%d",
	    TransPtr->error_weight), buffer), NULL));
    }

    /**
     **  Use the return code as defined for the current user level
     **/
    switch( sw_userlvl) {
	case UL_NOVICE:
	    ret_code = MeasPtr->ret_nov;
	    break;
	case UL_ADVANCED:
	    ret_code = MeasPtr->ret_adv;
	    break;
	case UL_EXPERT:
	    ret_code = MeasPtr->ret_exp;
	    break;
    }

    /**
     **  Print the error message
     **/
    if( TransPtr->error_weight <= WGHT_TRACE || ret_code != OK)
	if( !FlushError( error_type, module, lineno, TransPtr->error_weight,
	    MeasPtr->message, TransPtr->messages, argc, argv)) {
	    if( argv)
		null_free((void *) &argv);
	    argc = 0;
	    return( ErrorLogger( ERR_INTERRL, LOC, NULL));
	}

    /**
     **  Return to the caller ... conditionally ...
     **/
    if( WARN == ret_code)
	ret_code = OK;

    if( argv)
	null_free((void *) &argv);
    argc = 0;

    if( ret_code > ERROR)
	exit( ret_code);

    return( ret_code);
} /** End of 'Module_Error' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ErrorLookup					     **
 ** 									     **
 **   Description:	Look up the passed error type in the translation tab.**
 ** 									     **
 **   First Edition:	1995/08/06					     **
 ** 									     **
 **   Parameters:	ErrType		 error_type	Type of the error    **
 ** 									     **
 **   Result:		ErrTransTab*	NULL	Not found		     **
 **					else	Pointer to the acc. entry    **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	ErrTransTab	*ErrorLookup(	ErrType error_type )
{
    ErrTransTab	*low, *mid, *high, *save;	/** Search pointers	     **/

    /**
     **  Init serach pointers
     **/

    low = TransTab;
    high = TransTab + (sizeof( TransTab) / sizeof( TransTab[0]) -1);
    mid = (ErrTransTab *) NULL;

    /**
     **  Search loop
     **/

    while( low < high) {

	save = mid;
	mid = low + ((high - low) / 2);
	if( save == mid)
	    low = mid = high;			/** Just for safety ...	     **/

	if( mid->error_type < error_type ) {
	    low = mid;
	} else if( mid->error_type > error_type ) {
	    high = mid;
	} else
	    return( mid);		/** Yep! Got it!		     **/

    } /** while **/

    /**
     **  Maybe the loop has been finished before the comparison took place
     **  (low == high) and hit!
     **/

    if( mid->error_type == error_type )
	return( mid);               	/** Yep! Got it!                     **/

    /**
     **  If this point is reached, nothing has been found ...
     **/

    return( NULL);

} /** End of 'ErrorLookup' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		MeasLookup					     **
 ** 									     **
 **   Description:	Look up the passed error weight in the measurement   **
 **			table						     **
 ** 									     **
 **   First Edition:	1995/08/06					     **
 ** 									     **
 **   Parameters:	ErrWeights	weigth	Weight of the error	     **
 ** 									     **
 **   Result:		ErrMeasr*	NULL	Not found		     **
 **					else	Pointer to the acc. entry    **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	ErrMeasr	*MeasLookup(	ErrWeights weigth )
{
    ErrMeasr	*low, *mid, *high, *save;	/** Search pointers	     **/

    /**
     **  Init serach pointers
     **/

    low = Measurements;
    high = Measurements + (sizeof( Measurements) / sizeof( Measurements[0]) -1);
    save = (ErrMeasr *) NULL;
    mid = (ErrMeasr *) NULL;

    /**
     **  Search loop
     **/

    while( low < high) {

	save = mid;
	mid = low + ((high - low) / 2);
	if( save == mid)
	    low = mid = high;			/** Just to be sure ...      **/

	if( mid->error_weight < weigth ) {
	    low = mid;
	} else if( mid->error_weight > weigth ) {
	    high = mid;
	} else
	    return( mid);		/** Yep! Got it!		     **/

    } /** while **/

    /**
     **  Maybe the loop has been finished before the comparison took place
     **  (low == high) and hit!
     **/

    if( mid->error_weight == weigth )
	return( mid);               	/** Yep! Got it!                     **/

    /**
     **  If this point is reached, nothing has been found ...
     **/

    return( NULL);

} /** End of 'MeasLookup' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		FlushError					     **
 ** 									     **
 **   Description:	Print the error message. Decide which facility to    **
 **			use and schedule the according logger routine	     **
 ** 									     **
 **   First Edition:	1995/12/21					     **
 ** 									     **
 **   Parameters:	ErrType		  Type		Error type as passed **
 **			char		 *module	Module name	     **
 **			int		  lineno	Line number	     **
 **		  	ErrWeights	  Weight	Error Weight	     **
 **			char		 *WeightMsg	Printable Weight     **
 **			char		 *ErrMsgs	Error message	     **
 **			int		  argc		Number of arguments  **
 **			char		**argv		Argument array	     **
 ** 									     **
 **   Result:		int	1		Everything OK		     **
 **				0		Error occured while printing **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	FlushError(	ErrType		  Type,
				char		 *module,
				int		  lineno,
				ErrWeights	  Weight,
				char		 *WeightMsg,
				char		 *ErrMsgs,
				int		  argc,
				char		**argv)
{
    char	*facilities, *buffer;
    char 	*fac;
    FILE	*facfp;
    char	*errmsg_buffer;
    int		 fac_alloc = 0;

    /**
     **  get the error facilities at first. If there isn't any, we may
     **  return on success immediatelly.
     **/
    if((char *) NULL == (facilities = GetFacility( Weight)))
	goto success0;

    /**
     **  PANIC and FATAL error messages ought to be on stderr at least!
     **/
    if( WGHT_FATAL == Weight || WGHT_PANIC == Weight)
	if( !strstr( facilities, _stderr)) {

	    if((char *) NULL == (buffer = stringer(NULL,0,
		    _stderr,":", facilities, NULL))) {
		ErrorLogger( ERR_STRING, LOC, NULL);
		goto unwind0;
	    }

	    facilities = buffer;
	    fac_alloc = 1;
	}

    /**
     **  Print the error message into the buffer
     **/
    if((char *) NULL == (errmsg_buffer = stringer(NULL, ERR_BUFSIZE, NULL))) {
	ErrorLogger( ERR_ALLOC, LOC, NULL);
	goto unwind1;
    }

    /**
     **  In case of verbosity, the first argument is the ErrMsgs format string
     **/
    if( WGHT_VERBOSE == Weight) {
	ErrMsgs = *argv++;
	--argc;
    }

    if( WGHT_TRACE == Weight)
	Print_Tracing( errmsg_buffer, lineno, argc, argv);
    else if( !PrintError( errmsg_buffer, Type, module, lineno, Weight,
	WeightMsg, ErrMsgs, argc, argv))
	    goto unwind2;

    /**
     **  Now tokenize the facilities string and schedule the error messge
     **  for every single facility
     **/
    for( fac = xstrtok( facilities, ":");
	 fac;
	 fac = xstrtok( (char *) NULL, ":") ) {

	/**
	 **  Check for filenames. Two specials are defined: stderr and stdout
	 **  Otherwise filenames are expected to begin on '.' or '/'.
	 **  Everthing not recognized as a filename is assumed to be a
	 **  syslog facility
	 **  'null' and 'none' are known as 'no logging'
	 **/
	if( !strcmp( fac, _null) || !strcmp( fac, _none))
	    continue;

	else if( !strcmp( fac, _stderr))
	    fprintf( stderr, "%s", errmsg_buffer);
	
	else if( !strcmp( fac, _stdout)) 
	    fprintf( stdout, "%s", errmsg_buffer);

	/**
	 **  Syslog
	 **/
	else if( '.' != *fac && '/' != *fac) {
#if defined(HAVE_SYSLOG) && defined(WITH_LOGGING)
	    int syslog_fac, syslog_lvl;

	/* error output to stderr too if an error or verbose */
	    if (Type >= NO_ERR_VERBOSE)
		fprintf( stderr, "%s", errmsg_buffer);

	/* now send to syslog */
	    if( CheckFacility( fac, &syslog_fac, &syslog_lvl)) {
		openlog( "modulecmd", LOG_PID, syslog_fac);
		setlogmask( LOG_UPTO( syslog_lvl));
		syslog( (syslog_fac | syslog_lvl), "%s", errmsg_buffer);
		closelog();

	    /**
	     **  Invalid facilities ... take care not to end up in 
	     **  infinite loops
	     **/
	    } else if( Type == ERR_INVFAC ||
	               OK == ErrorLogger( ERR_INVFAC, LOC, fac, NULL)) 
		continue;

#else
#  ifdef	SYSLOG_DIR
	    /* this is an intentional memory leak */
	    buffer = stringer(NULL,0, SYSLOG_DIR, "/", fac);
	    fac = buffer;
#  endif
#endif
	}

	/**
	 **  Custom files ...
	 **  This may result from the syslog part above
	 **/
	if( '.' == *fac || '/' == *fac) {
	    if((FILE *) NULL == (facfp = fopen( fac, "a"))) {

		if( WGHT_PANIC == Weight)	/** Avoid endless loops!     **/
		    goto unwind2;

		/**
		 **  Invalid facilities ... take care not to end up in 
		 **  infinite loops
		 **/
		if( Type == ERR_INVFAC ||
		    OK == ErrorLogger( ERR_INVFAC, LOC, fac, NULL))
		    continue;
		else
		    goto unwind2;
	    }

	    fprintf( facfp, "%s", errmsg_buffer);

	    if( EOF == fclose( facfp))
		if( OK != ErrorLogger( ERR_CLOSE, LOC, fac, NULL))
		    goto unwind2;

	}
    } /** for **/

    /**
     **  Return on success
     **/
    null_free((void *) &errmsg_buffer);
    if( fac_alloc)
	null_free((void *) &facilities);
success0:
    return( 1);				/** -------- EXIT (SUCCESS) -------> **/

unwind2:
    null_free((void *) &errmsg_buffer);
unwind1:
    if( fac_alloc)
	null_free((void *) &facilities);
unwind0:
    return( 0);				/** -------- EXIT (FAILURE) -------> **/

} /** End of 'FlushError' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		GetFacility					     **
 ** 									     **
 **   Description:	Get the log facility according to the passed error   **
 **			weight						     **
 ** 									     **
 **   First Edition:	1995/12/21					     **
 ** 									     **
 **   Parameters:	ErrWeights	  Weight	Error Weight	     **
 ** 									     **
 **   Result:		char*	NULL		No facility found	     **
 **				Otherwise	Pointer to the colon separa- **
 **						ted facility string	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	char	*GetFacility(	ErrWeights	  Weight)
{
    ErrFacilities	*facility;

    /**
     **  Get the facility table entry at first
     **/
    if( !(facility = GetFacility_sub( Weight)))
	return((char *) NULL);

    /**
     **  Now we've got two possibilities:
     **      First of all there may be a custom facilitiy defined
     **	     Otherwise the default facility is to be returned now
     **/
    return( facility->facility ? facility->facility : facility->def_facility);

} /** End of 'GetFacility' **/

static	ErrFacilities	*GetFacility_sub(	ErrWeights	  Weight)
{
    ErrFacilities	*low, *mid, *high, *save;
    char		buffer[ 20];

    /**
     **  Binary search for the passed facility in the Facilities table.
     **  This requires the table to be sorted on ascending error weight
     **/
    low = Facilities;
    high = Facilities + (sizeof( Facilities) / sizeof( Facilities[0]));
    save = (ErrFacilities *) NULL;
    mid = (ErrFacilities *) NULL;

    while( low < high) {
	save = mid;
	mid = low + ((high - low) / 2);
	if( save == mid)
	    low = mid = high;			/** Just to be sure ...	     **/

	if( mid->Weight > Weight) 
	    high = mid;
	else if( mid->Weight < Weight)
	    low = mid;
	else
	    break;	/** found! **/
    }

    /**
     **  We have to check, if we've found something or if there's an internal
     **  error (wrong weight)
     **/
    if( mid->Weight != Weight) {
	if( OK == ErrorLogger( ERR_INVWGHT, LOC, (sprintf( buffer, "%d",
	    Weight), buffer), NULL))
	    return( NULL);
    }

    return( mid);

} /** End of 'GetFacility_sub' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		CheckFacility					     **
 ** 									     **
 **   Description:	Check the passwd string to be a valid combination    **
 **			of       <syslog_facility>.<syslog_level>	     **
 ** 									     **
 **   First Edition:	1995/12/21					     **
 ** 									     **
 **   Parameters:	char	*string		Input facility string	     **
 **			int	*facility	Buffer for the real facility **
 **			int	*level		Buffer for the real level    **
 ** 									     **
 **   Result:		int	1		Success			     **
 **				0		Failure. String not valid    **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	CheckFacility(	char *string, int *facility, int *level)
{
    char *s, *buf;
    int x;

    /**
     **  We do not want to change the strings ... so allocate a buffer here
     **/
    if((char *) NULL == (buf = stringer(NULL,0, string,NULL)))
	if( OK == ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;

    /** 
     **  We cannot use strtok here, because there's one initialized in an
     **  outer loop!
     **/
    for( s=buf; s && *s && *s != '.'; s++);
    if( !s || !*s)
	goto unwind1;
    *s = '\0';
	
    /**
     **  This should be the facility
     **/
    if( -1 == (x = scan_facility( buf, facility_names,
	    (sizeof( facility_names) / sizeof( facility_names[0])) )))
	goto unwind1;
    *facility = x;

    /**
     **  This should be the level
     **/
    if( -1 == (x = scan_facility( ++s, level_names, 
	    (sizeof( level_names) / sizeof( level_names[0])) )))
	goto unwind1;
    *level = x;

    /**
     **  Success
     **/
    null_free((void *) &buf);
    return( 1);				/** -------- EXIT (SUCCESS) -------> **/

unwind1:
    null_free((void *) &buf);
unwind0:
    return( 0);				/** -------- EXIT (FAILURE) -------> **/

} /** End of 'CheckFacility' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		scan_facility					     **
 ** 									     **
 **   Description:	Scan the passed facility names table for the given   **
 **			string and pass back the assigned token		     **
 ** 									     **
 **   First Edition:	1995/12/21					     **
 ** 									     **
 **   Parameters:	char		*s	String to be checked	     **
 **			FacilityNames	*table	Table of valid names and     **
 **						tokens			     **
 **			int		 size	Size of the table	     **
 ** 									     **
 **   Result:		int	-1		name not found in the table  **
 **				Otherwise	Assigned token		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	scan_facility( char *s, FacilityNames *table, int size)
{
    FacilityNames *low, *mid, *high, *save;

    low = table;
    high = table + size;
    save = (FacilityNames *) NULL;

    while( low < high) {
	int x;			/** Have to use this, because strcmp will    **/
				/** not return -1 and 1 on Solaris 2.x	     **/
	save = mid;
	mid = low + ((high - low) / 2);
	if( save == mid)
	    low = mid = high;			/** To prevent endless loops **/

	if (mid == high)
		return -1;
	x = strcmp( mid->name, s);

	if( x < 0 )
	    low = mid;
	else if( x > 0)
	    high = mid;
	else
	    return( mid->token);

    } /** while **/

    return( !strcmp( mid->name, s) ? mid->token : -1 );

} /** End of 'scan_facility' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		GetFacilityPtr					     **
 ** 									     **
 **   Description:	Scan the passed facility names table for the given   **
 **			string and pass back the assigned token		     **
 ** 									     **
 **   First Edition:	1995/12/21					     **
 ** 									     **
 **   Parameters:	char	*facility	Name of the facility	     **
 ** 									     **
 **   Result:		char**	NULL		Invalid facility name	     **
 **				Otherwise	Pointer to the facilty string**
 **						reference		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char	**GetFacilityPtr( char *facility)
{
    int		 	 i, len;
    ErrMeasr		*measptr;
    char		*buf, *s, *t;
    ErrFacilities	*facptr;

    /**
     **  Try to figure out the error weight at first
     **  Need the given weight in upper case for this
     **/
    len = strlen( facility);

    if((char *) NULL == (buf = stringer(NULL, 0, facility, NULL)))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind0;

    for( t = buf; *t; ++t) *t = toupper(*t);
    
    /**
     **  Now look up the measurements table for the uppercase weight
     **/
    i = sizeof( Measurements) / sizeof( Measurements[ 0]);
    measptr = Measurements;

    while( i) {
	if( !strncmp( measptr->message, buf, len))
	    break;
	i--; measptr++;
    }

    null_free((void *) &buf);

    if( !i) 					/** not found		     **/
	goto unwind0;
    
    /**
     **  Now get the facility table entry
     **/

    if((ErrFacilities *) NULL == (facptr = GetFacility_sub(
	measptr->error_weight))) {
	ErrorLogger( ERR_INVWGHT_WARN, LOC, facility, NULL); 
	goto unwind0;
    }

    /**
     **  Got it ... return the desired pointer
     **/
    return( &facptr->facility);		/** -------- EXIT (RESULT)  -------> **/

unwind0:
    return((char **) NULL);		/** -------- EXIT (FAILURE) -------> **/

} /** End of 'GetFacilityPtr' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		PrintError					     **
 ** 									     **
 **   Description:	Print the error message				     **
 ** 									     **
 **   First Edition:	1995/08/06					     **
 ** 									     **
 **   Parameters:	char             *errbuffer	Buffer to hold the   **
 **							error messge	     **
 **			ErrType		  Type		Error type as passed **
 **			char		 *module	Module name	     **
 **			int		  lineno	Line number	     **
 **		  	ErrWeights	  Weight	Error Weight	     **
 **			char		 *WeightMsg	Printable Weight     **
 **			char		 *ErrMsgs	Error message	     **
 **			int		  argc		Number of arguments  **
 **			char		**argv		Argument array	     **
 ** 									     **
 **   Result:		int	1		Everything OK		     **
 **				0		Error occured while printing **
 ** 									     **
 **   Notes:	According to the error type, the passed module and line num- **
 **		ber will be handled as a module-file related one or depending**
 **		on the packages source code:				     **
 **									     **
 **		src -> ERR_IN_MODULEFILE -> modulefile -> ERR_INTERNAL -> src**
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	PrintError(	char 		 *errbuffer,
				ErrType		  Type,
				char		 *module,
				int		  lineno,
				ErrWeights	  Weight,
				char		 *WeightMsg,
				char		 *ErrMsgs,
				int		  argc,
				char		**argv)
{
    char	*error_string;

    /**
     **  Build the error string at first. Note - we cannot alloc memory any
     **  more!
     **/
    if( ERR_ALLOC == Type) 
	error_string = ErrMsgs;
    else
	if( NULL == (error_string = ErrorString( ErrMsgs, argc, argv)))
	    return( 0);

    /**
     **  Print
     **/

    if( ERR_INTERNAL > Type && ERR_IN_MODULEFILE < Type &&
	linenum && g_current_module )

	sprintf( errbuffer, "%s(%s):%s:%d: %s\n", g_current_module,
	    (sprintf( buffer, "%d", linenum), buffer),
	    WeightMsg, Type, (error_string ? error_string : "") );
    else

	sprintf( errbuffer, "%s(%s):%s:%d: %s\n", (module ? module : "??"),
	    ( lineno ? (sprintf( buffer, "%d", lineno), buffer) : "??" ),
	    WeightMsg, Type, (error_string ? error_string : "") );

    /**
     **  Success
     **/
    return( 1);

} /** End of 'PrintError' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ErrorString					     **
 ** 									     **
 **   Description:	Print the error message				     **
 ** 									     **
 **   First Edition:	1995/08/06					     **
 ** 									     **
 **   Parameters:	char		 *ErrMsgs	Error message	     **
 **			int		  argc		Number of arguments  **
 **			char		**argv		Argument array	     **
 ** 									     **
 **   Result:		char*	NULL		Parse or alloc error	     **
 **				else		Pointer to the error string  **
 ** 									     **
 **   Attached Globals:	-						     **
 **									     **
 ** ************************************************************************ **
 ++++*/

static	char	*ErrorString(	char		 *ErrMsgs,
				int		  argc,
				char		**argv)
{
    char	*s;			/** Insertion pointer		     **/
    int		len = 0;		/** Current length		     **/
    int		backslash = 0;		/** backslash found ?		     **/

    if( !ErrMsgs)
	return( NULL);

    /**
     **  Allocate memory if neccessary
     **/
    if( !error_line)
	if((char *) NULL ==(error_line = stringer(NULL,strsize = ERR_LINELEN,
		NULL))){
	    ErrorLogger( ERR_STRING, LOC, NULL);
	    return( NULL);
	}

    s = error_line;

    /**
     **  Scan the error strings to be printed
     **/
    while( *ErrMsgs) {

	/**
	 **  Check for special characters
	 **/
	switch( *ErrMsgs) {
	    case '\\':	if( !backslash) {
				backslash = 1;
				ErrMsgs++;
				continue;	/** while( *ErrMsgs) **/
			    }
			    break;	/** switch **/

	    case '$':	if( !backslash) {
				ErrMsgs++;
				add_param( &ErrMsgs, &s, &len, argc, argv);
				error_line = s - len;
				continue;	/** while( *ErrMsgs) **/
			    }
			    break;	/** switch **/
	} /** switch **/

	/**
	 **  Add a single character to the error string
	 **/
	if( ++len >= strsize - 5) {	/** 5 Bytes for safety		     **/
	    if(!(error_line = (char *) module_realloc( error_line,
		strsize += ERR_LINELEN))) {
		ErrorLogger( ERR_ALLOC, LOC, NULL);
		return( NULL);
	    }
	    s = error_line + len - 1;
	}

	*s++ = *ErrMsgs++;
	backslash = 0;
				
    } /** while( *ErrMsgs) **/

    /**
     **  Success. Return a pointer to the newly created string
     **/
    *s++ = '\0';
    return( error_line);

} /** End of 'ErrorString' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		add_param					     **
 ** 									     **
 **   Description:	Put an argument to the error string		     **
 ** 									     **
 **   First Edition:	1995/08/06					     **
 ** 									     **
 **   Parameters:	char		**Control	Parameter control    **
 **			char		**Target	Target to print to   **
 **			int		 *Length	Current length of the**
 **							output string	     **
 **			int		  argc		Number of arguments  **
 **			char		**argv		Argument array	     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	-						     **
 **									     **
 ** ************************************************************************ **
 ++++*/

static	void	add_param(	char		**Control,
				char		**Target,
				int		 *Length,
				int		  argc,
				char		**argv)
{
    char	*s;			/** Scan pointer fot the ctrl field  **/
    int		index, last = 0;	/** parameter index		     **/
    int		len = 0;		/** Parameter string length	     **/

    /**
     **  Copy the current control field into the buffer
     **/

    for( s = buffer; **Control; (*Control)++ ) {

	if( **Control == '*') {
	    last = argc;
	    continue;
	} else if( **Control < '0' || **Control > '9')
	    break;

	*s++ = **Control;
    }

    *s = '\0';

    /**
     **  Has something been found ? If not, print a '$'
     **/
    if( s == buffer && !last) {

	if( ++(*Length) >= strsize) {
	    if(!(error_line = (char*) module_realloc( error_line,
		strsize += ERR_LINELEN))) {
		ErrorLogger( ERR_ALLOC, LOC, NULL);
		return;
	    }
	    *Target = error_line + *Length - 1;
	}

	*(*Target++) = '$';

    } else {

	/**
	 **  Something has been found. Form the parameter index at first
	 **/
	if( s == buffer)
	    index = 0;
	else
	    index = atoi( buffer) - 1;

	if( !last)
	    last = index + 1;
	if( last > argc)
	    last = argc;

	/**
	 **  Spool them all out
	 **/
	while( index < last) {

	    len = strlen( argv[ index]);

	    while(( *Length + len + 1) >= strsize - 5) {
		if(!(error_line = (char*) module_realloc( error_line,
		    strsize += ERR_LINELEN))) {
		    ErrorLogger( ERR_ALLOC, LOC, NULL);
		    return;
		}
		*Target = error_line + *Length;
	    }

	    strcpy( *Target, argv[ index]);
	    *Target += len;
	    *Length += len;

	    index++;

	} /** while **/
    } /** if **/

} /** End of 'add_param' **/
