/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.3						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		modules_def.h					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:							     **
 ** 									     **
 **   Exports:								     **
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

#ifndef _MODULES_DEF_H
#define _MODULES_DEF_H

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include <stdio.h>
#include <stdarg.h>
#include <tcl.h>
#include "config.h"
#include "uvec.h"
#include "ovec.h"

#ifndef CONST84
#  define CONST84	/* const */
#endif

#if (TCL_MAJOR_VERSION < 8)
#  define TCL_RESULT(interp) ((interp)->result)
#else
#  define TCL_RESULT(interp) Tcl_GetStringResult(interp)
#endif

#ifndef HAVE_STDINT_H
/* assume 32 bit - hope for the best */
typedef	int	intptr_h;
#endif

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#if STDC_HEADERS || HAVE_STRING_H
#  include <string.h>
/**
 **  An ANSI string.h and pre-ANSI memory.h might conflict.
 **/
#  if !STDC_HEADERS && HAVE_MEMORY_H
#    include <memory.h>
#  endif /** not STDC_HEADERS and HAVE_MEMORY_H **/
#else /** not STDC_HEADERS and not HAVE_STRING_H **/
#  include <strings.h>
/**
 **  memory.h and strings.h conflict on some systems.
 **/
#endif /** not STDC_HEADERS and not HAVE_STRING_H  **/

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#  include <sys/types.h>
#endif

#ifdef HAVE_CTYPE_H
#  include <ctype.h>
#endif

#ifdef HAVE_SYS_MODE_H
/* #include <sys/mode.h> */
#endif

#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif

#ifdef HAVE_SYS_TERMIOS_H
#  include <sys/termios.h>
#else
#  ifdef HAVE_TERMIO_H
#    include <termio.h>
#  endif
#endif

#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#endif

#if !defined(TIOCGWINSZ) && defined(HAVE_SYS_IOCTL_H)
#  include <sys/ioctl.h>
#endif

#if defined(DIRENT) || defined(_POSIX_VERSION)
#  include <dirent.h>
#  define NLENGTH(dirent) (strlen((dirent)->d_name))
#else /** not (DIRENT or _POSIX_VERSION) **/
#  define dirent direct
#  define NLENGTH(dirent) ((dirent)->d_namlen)
#  ifdef SYSNDIR
#    include <sys/ndir.h>
#  endif /** SYSNDIR **/
#  ifdef SYSDIR
#    include <sys/dir.h>
#  endif /** SYSDIR **/
#  ifdef NDIR
#    include <ndir.h>
#  endif /** NDIR **/
#endif /** not (DIRENT or _POSIX_VERSION) **/

#ifdef HAVE_ERRNO_H
#  include <errno.h>
#else
extern	int	  errno;
#endif

#ifdef HAVE_LOCALE_H
#  include <locale.h>
#endif

#if ENABLE_NLS
#  include <libintl.h>
#  define _(String) gettext(String)
#else
#  define _(String) (String)
#endif
/* gettext_noop() equivalent */
#define N_(String) (String)

#ifdef HAVE_ASSERT_H
#  include <assert.h>
#else
#warning "not able to test code assertions"
#define assert(condition)
#endif

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/**
 **  Module hash type - uses Tcl_*Hash*() underneath
 **/
typedef enum {
	MHashNULL = 0,				/** invalid value	**/
	MHashInt,				/** integer values	**/
	MHashStrings,				/** string storage	**/
	MHashRefCounts,				/** strings &		**/
						/**   reference counts	**/
	MHashFiles				/** files & dirs	**/
} MHashType;

typedef struct _mhash {
	char		tag[6];			/** name tag for this	**/
	MHashType	type;			/** what type is this	**/
	uvec		*keys;			/** vector of keys	**/
	Tcl_HashTable	*hash;			/** hash container	**/
	int		(*add)(void **,va_list);/** fn copy/create data	**/
	int		(*del)(void **,va_list);/** fn delete data	**/
} MHash;

typedef enum {
	GHashNULL = 0,				/** invalid value	**/
	GHashClear,				/** Clear  global hashes**/
	GHashDelete,				/** Delete global hashes**/
	GHashCopy,				/** Copy   global hashes**/
} GHashAction;

/**
 **  Structure to store information about a file.  Includes its name
 **  and the structure to store information from the stat system call.
 **/

typedef struct _file_entry {
    char*        fi_prefix;
    char*        fi_name;
    struct stat  fi_stats;
    int          fi_listcount;
    struct _file_entry* fi_subdir;
} fi_ent;

/**
 **  file stat as given by is_()
 **/
typedef	enum	{
	IS_NOT		= 0,		/** file is not as described	     **/
	IS_SO		= 1,		/** file is as described	     **/
	IS_FILE		= 1,		/** "what" yields a file 	     **/
	IS_DIR		= 2		/** "what" yields a file 	     **/
} is_Result;

/**
 **  Error handling (see error.c for weight and message)
 **/

typedef	enum	{
	NO_ERR = 0,			/** No error			     **/
	NO_ERR_VERBOSE,			/** Verbose messages		     **/
	ERR_PARAM = 10,			/** Parameter error		     **/
	ERR_USAGE,			/** Usage information		     **/
	ERR_ARGSTOLONG,			/** Arguments to long		     **/
	ERR_OPT_AMBIG = 40,		/** Option is ambiguous		     **/
	ERR_OPT_NOARG,			/** Option allows no argument	     **/
	ERR_OPT_REQARG,			/** Option requires an argument	     **/
	ERR_OPT_UNKNOWN,		/** Unrecognized option 	     **/
	ERR_OPT_ILL,			/** Illegal option 		     **/
	ERR_OPT_INV,			/** Invalid option 		     **/
	ERR_USERLVL,			/** Unknown userlevel		     **/
	ERR_GETOPT,			/** getopt() failed		     **/
	ERR_OPEN = 50,			/** Error opening file		     **/
	ERR_POPEN,			/** Error opening pipe	    	     **/
	ERR_OPENDIR,			/** Error opening directory	     **/
	ERR_CHDIR,			/** can not chdir to directory	     **/
	ERR_CLOSE,			/** Error when closing a file	     **/
	ERR_PCLOSE,			/** Error when closing a pipe	     **/
	ERR_CLOSEDIR,			/** Error when closing a directory   **/
	ERR_READ,			/** Error when reading from a file   **/
	ERR_READDIR,			/** Error when reading directory     **/
	ERR_WRITE,			/** Error when writing to a file     **/
	ERR_SEEK,			/** Seek error			     **/
	ERR_FLUSH,			/** Flush error			     **/
	ERR_DUP,			/** File duplication error	     **/
	ERR_DIRNAME,			/** Cannot build directory name	     **/
	ERR_NAMETOLONG,			/** Directory name to long	     **/
	ERR_DIRNOTFOUND,		/** Directory not found		     **/
	ERR_FILEINDIR,			/** File not in directory	     **/
	ERR_NODIR,			/** Not a directory		     **/
	ERR_UNLINK,			/** Cannot unlink file		     **/
	ERR_RENAME,			/** Cannot rename file		     **/
	ERR_ALLOC = 70,			/** Out of memory		     **/
	ERR_UVEC,			/** general UVEC error		     **/
	ERR_MHASH,			/** general MHash error		     **/
	ERR_SOURCE,			/** Error while sourcing ...	     **/
	ERR_UNAME,			/** Uname failed		     **/
	ERR_GETHOSTNAME,		/** gethostname failed		     **/
	ERR_GETDOMAINNAME,		/** getdomainname failed	     **/
	ERR_STRING,			/** string error		     **/
	ERR_DISPLAY = 90,		/** cannot open display	    	     **/
	ERR_IN_MODULEFILE = 100,	/** modulefile related errors	     **/
	ERR_PARSE,			/** Parse error (modulefile)	     **/
	ERR_EXEC,			/** Execution error (modulefile)     **/
	ERR_EXTRACT,			/** Cannot extract X11 ressources    **/
	ERR_COMMAND,			/** Unrecognized command	     **/
	ERR_LOCATE,			/** Module file not found	     **/
	ERR_MAGIC,			/** Bad magic number		     **/
	ERR_MODULE_PATH,		/** Module path not set		     **/
	ERR_HOME,			/** Home not set		     **/
	ERR_SHELL,			/** Unknown shell type		     **/
	ERR_DERELICT,			/** Unknown shell derelict	     **/
	ERR_CONFLICT = 150,		/** Module file conflicts	     **/
	ERR_PREREQ,			/** Module file prerequirements	     **/
	ERR_NOTLOADED,			/** Module file is currently not l.  **/
	ERR_DUP_SYMVERS,		/** Duplicate symbolic version	     **/
	ERR_SYMLOOP,			/** Loop in symbol version def.	     **/
	ERR_BADMODNAM,			/** Bad modulename in version, alias **/
	ERR_DUP_ALIAS,			/** Duplicate alias		     **/
	ERR_BEGINENV,			/** No update if no .modulesbeginenv **/
	ERR_BEGINENVX,			/** No MODULESBEGINENV env.var.      **/
	ERR_INIT_TCL,			/** Cannot initialize TCL	     **/
	ERR_INIT_TCLX,			/** Cannot initialize extended TCL   **/
	ERR_INIT_ALPATH,		/** Cannot set up autoload path      **/
	ERR_INIT_STUP,			/** No 'module load in the shellstups**/
	ERR_SET_VAR,			/** Cannot set TCL variable	     **/
	ERR_INFO_DESCR,			/** Unrecognized module info descr.  **/
	ERR_INVWGHT_WARN,		/** Invalid error weight	     **/
	ERR_INVFAC_WARN,		/** Invalid error facility	     **/
	ERR_INTERNAL = 990,		/** Error logger internal error	     **/
	ERR_INTERAL,			/** Alias module internal error	     **/
	ERR_INTERRL,			/** Error logger internal error	     **/
	ERR_INVAL,			/** Invalid parameter to the error   **/
	ERR_INVWGHT,			/** logger			     **/
	ERR_INVFAC,			/** Invalid error facility	     **/
        ERR_ENVVAR			/** env. variables are inconsistent  **/
} ErrType;

/**
 **  Error return values. Only OK, PROBLEM and ERROR will be returned to the
 **  caller, In case of the remaining, the error handler takes control over the
 **  whole application.
 **/
typedef	enum	{
	OK	= 0,			/** Everything's up and running	     **/
	WARN	= 2,			/** A warning (mapped to OK by the   **/
					/** Errorlogger)		     **/
	PROBLEM	= 5,			/** Problem ... program might cont.  **/
	ERROR	= 7,			/** Error .. try gracefull aborting  **/
	FATAL	= 10,			/** The following will lead to the   **/
	PANIC	= 20			/** program being aborted by the     **/
					/** error logger immediatelly	     **/
} ErrCode;

/**
 **  Internal return value to handle the various ways a module load
 **	could end.
 **/
typedef enum	{
	EM_OK		= 0,		/** normal return	**/
	EM_EXIT,			/** cmd: exit N		**/
					/** (set g_retval = N)	**/
	EM_BREAK,			/** cmd: break		**/
	EM_CONTINUE,			/** cmd: continue	**/
	EM_ERROR			/** abnormal return	**/
} EM_RetVal;

/**
 **  Internal switch for toggling between 'load' and 'unload'
 **/
typedef enum	{
	Mod_Null	=0,		/** Invalid module action	**/
	Mod_Load,			/** load the given module	**/
	Mod_Unload			/** unload the same		**/
} Mod_Act;

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

#define      MODULES_MAGIC_COOKIE         "#%Module"
#define      MODULES_MAGIC_COOKIE_LENGTH  8

/**
 **  User level
 **/

#define	UL_NOVICE	0
#define	UL_ADVANCED	64
#define	UL_EXPERT	128

/**
 **  Default error log facilities
 **/

#ifdef	WITH_LOG_FACILITY
#   define	DEF_FACILITY			WITH_LOG_FACILITY
#else
#   define	DEF_FACILITY			"local7"
#endif

#define	DEF_FACILITY_VERBOSE		"stderr:" DEF_FACILITY ".info"
#define	DEF_FACILITY_INFO		"stderr:" DEF_FACILITY ".notice"
#define	DEF_FACILITY_DEBUG		DEF_FACILITY ".debug"
#define	DEF_FACILITY_WARN		"stderr:" DEF_FACILITY ".warning"
#define	DEF_FACILITY_PROB		"stderr:" DEF_FACILITY ".warning"
#define	DEF_FACILITY_ERROR		"stderr:" DEF_FACILITY ".err"
#define	DEF_FACILITY_FATAL		"stderr:" DEF_FACILITY ".crit"
#define	DEF_FACILITY_PANIC		"stderr:" DEF_FACILITY ".crit"

/**
 **  g_flags values
 **/

#define      M_REMOVE	0x0001
#define      M_DISPLAY	0x0002
#define      M_SWSTATE1	0x0004
#define      M_SWSTATE2	0x0008
#define      M_SWSTATE3	0x0010
#define      M_SWITCH	( M_SWSTATE1 | M_SWSTATE2 | M_SWSTATE3)
#define      M_LOAD	0x0020
#define      M_CLEAR	0x0040
#define      M_PREPEND	0x0080
#define      M_HELP	0x0100
#define      M_WHATIS	0x0200
#define      M_NONPERSIST	0x0400

/**
 **  markers for switching 
 **/

#define      SWMARKER        "--VARMARKER--"  /**  for variables and aliases  **/
#define      APP_SW_MARKER   "--APPMARKER--"  /**  for appending paths  **/
#define      PRE_SW_MARKER   "--PREMARKER--"  /**  for prepending paths  **/

/**
 **  return values 
 **/

#define TCL_LEVEL0_RETURN    11

/**
 **  RC files
 **/

#ifndef	RCFILE
#  define	RCFILE		"modulerc"
#endif

#ifndef	MODULERCFILE
#  define	MODULERCFILE	".modulerc"
#endif

#ifndef	VERSIONFILE
#  define	VERSIONFILE	".version"
#endif

/**
 **  Buffer sizes
 **/

#define  LINELENGTH	8192
#define  MOD_BUFSIZE	1024

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/**
 **  I'm going to remove all of the calls to free( ) since they are not
 **    necessary for Modules.  Since the modulecmd program is only run for
 **    a very short time ( usually <1sec) it's faster to not clutter the heap
 **    by freeing up memory.
 **
 **  If you disagree with this decision, or have some problems with this
 **    behavior on your system, configure with --enable-free
 **
 **  Note that all memory deallocations should go through null_free()
 **/

#ifndef USE_FREE
#  define  free( x)  
#  define  FreeList( x)  
#endif

/** 
 **  Some systems don't define S_ISDIR and S_ISREG
 **/

#ifdef HAVE_SYS_STAT_H
#  ifndef S_ISDIR
#    define S_ISDIR( m) (((m) & S_IFMT) == S_IFDIR)
#  endif
#  ifndef S_ISREG
#    define S_ISREG( m) (((m) & S_IFMT) == S_IFREG)
#  endif
#endif

/**
 **  Error logger
 **/

#define	ErrorLogger	Module_Error
#define	LOC		module_name, __LINE__

/** ************************************************************************ **/
/** 				    COMMON STRINGS			     **/
/** ************************************************************************ **/

extern char	em_reading[];
extern char	em_writing[];
extern char	em_appending[];
extern char	em_read_write[];
extern char	em_unknown[];
extern char	em_default[];

/** ************************************************************************ **/
/** 				    GLOBAL DATA				     **/
/** ************************************************************************ **/

extern	char	**environ;

extern	uvec	 *ModulePathVec;
extern	char	**ModulePath;
extern	char	 *version_string;
extern	char	 *date_string;
extern	char	 *g_current_module;
extern	char	 *g_specified_module;
extern	char	 *shell_name;
extern	char	 *shell_derelict;
extern	char	 *shell_init;
extern	char	 *shell_cmd_separator;
extern	char	 *change_dir;
extern	int	  g_flags;
extern	int	  g_retval;
extern	int	  g_output;
extern	int	  append_flag;
extern	char	 *line;
extern	char	 *error_line;
extern	char	  local_line[];
extern	char	  _colon[];
extern	char	 *psep;
extern	Tcl_Obj	 *cwd;

extern	int	  linenum;

extern	char	 *addRE;
extern	char	 *rmRE;
extern	char	 *swRE;
extern	char	 *dispRE;
extern	char	 *listRE;
extern	char	 *availRE;
extern	char	 *helpRE;
extern	char	 *initRE;
extern	char	 *initxRE;
extern	char	 *useRE;
extern	char	 *unuseRE;
extern	char	 *updateRE;
extern	char	 *purgeRE;
extern	char	 *clearRE;
extern	char	 *whatisRE;
extern	char	 *aproposRE;
extern	char	 *refreshRE;

extern	MHash	*setenvHashTable;
extern	MHash	*unsetenvHashTable;
extern	MHash	*aliasSetHashTable;
extern	MHash	*aliasUnsetHashTable;
extern	MHash	*markVariableHashTable;
extern	MHash	*markAliasHashTable;
extern	MHash	*GlobalHashTables[5];

extern	char    _fil_stdin[];
extern	char    _fil_stdout[];
extern	char    _fil_stderr[];
extern	char    _fil_devnull[];

extern	int	sw_detach;
extern	int	sw_force;
extern	int	sw_format;
#define	SW_SET		0x01
#define	SW_HUMAN	0x02
#define	SW_PARSE	0x04
#define	SW_TERSE	0x08
#define	SW_LONG		0x10
#define	SW_LIST		0x20
extern	int	sw_verbose;
extern	int	sw_userlvl;
extern	int	sw_icase;

extern	char	*instpath;
extern	char	*rc_file;
extern	char	*modulerc_file;
extern	char	*version_file;
extern	char	*change_dir;

extern	char	 long_header[];

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/**  locate_module.c  **/
extern	int	  Locate_ModuleFile( Tcl_Interp*, char*, char*, char*);
extern	int	  SourceVers(Tcl_Interp*,const char*,const char*,const Mod_Act);
extern	int	  SourceRC(Tcl_Interp*,const char*,const char*,const Mod_Act);

/**  main.c  **/
extern  void	  module_usage(FILE *output);

/**  ModuleCmd_Avail.c  **/
extern	int	  ModuleCmd_Avail( Tcl_Interp*, int, char*[]);
extern	void	  print_dirents( char*);
extern	char	 *strip_top( char*);
extern	void	  print_aligned_files(Tcl_Interp*,char*,char*,char**,int,int);
extern	int	  check_dir( char*);
extern	fi_ent	 *get_dir( char*, char*, int*, int*);
extern	void	  dirlst_to_list( char**, fi_ent*, int, int*, char*, char*);
extern	void	  delete_dirlst( fi_ent*, int);
extern	void	  delete_cache_list( char**, int);

/**  ModuleCmd_Clear.c  **/
extern	int	  ModuleCmd_Clear( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Display.c  **/
extern	int	  ModuleCmd_Display( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Help.c  **/
extern	int	  ModuleCmd_Help( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Init.c  **/
extern	int	  ModuleCmd_Init( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_List.c  **/
extern	int	  ModuleCmd_List( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Load.c  **/
extern	int	  ModuleCmd_Load( Tcl_Interp*, int, int, char*[]);

/**  ModuleCmd_Purge.c  **/
extern	int	  ModuleCmd_Purge( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Refresh.c  **/
extern	int	  ModuleCmd_Refresh( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Switch.c  **/
extern	int	  ModuleCmd_Switch( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Update.c  **/
extern	int	  ModuleCmd_Update( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Whatis.c  **/
extern	int	  ModuleCmd_Whatis( Tcl_Interp*, int, char*[]);
extern	int	  ModuleCmd_Apropos( Tcl_Interp*, int, char*[]);

/**  ModuleCmd_Use.c  **/
extern	int	  ModuleCmd_Use( Tcl_Interp*, int, char*[]);
extern	int	  ModuleCmd_UnUse( Tcl_Interp*, int, char*[]);

/**  cmdAlias.c  **/
extern	int	  cmdSetAlias( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdConflict.c  **/
extern	int	  cmdConflict( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	int	  cmdPrereq( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdIsLoaded.c  **/
extern	int	  cmdIsLoaded(ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdVerbose.c  **/
extern	int	  cmdModuleVerbose(ClientData,Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdWhatis.c  **/
extern	char	**whatis;
extern	void	  cmdModuleWhatisInit(void);
extern	void	  cmdModuleWhatisShut(void);
extern	int	  cmdModuleWhatis(ClientData,Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdInfo.c  **/
extern	int	  cmdModuleInfo(ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	char	 *module_command;

/**  cmdMisc.c  **/
extern	int	  cmdSystem( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdModule.c  **/
extern	int	  cmdModule( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	int	  Read_Modulefile( Tcl_Interp*, char*);
extern	int	  Execute_TclFile( Tcl_Interp*, char*);
extern	int	  CallModuleProcedure( Tcl_Interp*, Tcl_DString*, char*, char*,
			int);

/**  cmdPath.c  **/
extern	int	  cmdSetPath( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	int	  cmdRemovePath( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	char	 *chk_nullvars( char*);

/**  cmdSetenv.c  **/
extern	int	  cmdSetEnv( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	int	  moduleSetenv( Tcl_Interp*, char	 *, char*, int);
extern	int	  cmdUnsetEnv( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	int	  moduleUnsetenv( Tcl_Interp*, char	 *);

/**  cmdUname.c  **/
extern	int	  cmdUname( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdXResource.c  **/
extern	int	  cmdXResource( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	void	  xresourceFinish( int);

/**  cmdUlvl.c  **/
extern	int	  cmdModuleUser(ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	int	  cmdModuleUser_sub( char *user_level);

/**  cmdChdir.c **/
extern	int	  cmdChDir(ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdLog.c  **/
extern	int	  cmdModuleLog( ClientData, Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);

/**  cmdVersion.c  **/
extern	int	  cmdModuleVersion(ClientData,Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	int	  cmdModuleAlias(ClientData,Tcl_Interp*,
			int, Tcl_Obj * CONST84 []);
extern	int	  AliasLookup( char*, char**, char**);
extern	int	  VersionLookup( char*, char**, char**);
extern	char	 *ExpandVersions( char*);

/**  init.c  **/
extern	int	  Initialize_Module( Tcl_Interp**, int, char*[], char*[]);
extern	int	  Module_Init( Tcl_Interp*);
extern	int	  Setup_Environment( Tcl_Interp*);
extern	char	**SetStartupFiles( char *shell_name);
extern	int	  TieStdout( void);
extern	int	  UnTieStdout( int);

/**  utility.c  **/
extern	uvec	 *SortedDirList(char*, char*, int*);
extern	uvec	 *SplitIntoList(const char*, int*, const char*);
#ifdef USE_FREE
extern	void	  FreeList( uvec**);
#endif
extern	uvec	 *ModulePathList(void);
extern	int	  store_old_shell_variable( Tcl_HashTable*, const char*,
			const char*);
extern	int	  clear_old_shell_variable( Tcl_HashTable*, const char*);
extern	MHash	**Global_Hash_Tables(GHashAction, MHash **);
extern	int	  Unwind_Modulefile_Changes( Tcl_Interp*, MHash **);
extern	int	  Output_Modulefile_Changes( Tcl_Interp*);
extern	int	  store_env( void);
extern	int	  free_stored_env( void);
extern	Tcl_HashTable*  environ_changes;
extern	Tcl_HashTable*  alias_changes;
extern	char	 *getLMFILES(void);
extern	int	  IsLoaded( char*, char**, char*);
extern	int	  IsLoaded_ExactMatch( char*, char	**, char*);
extern	int	  Update_LoadedList( Tcl_Interp*, char*, char*);
extern	int	  check_magic( char*, char*, int);
extern	char	 *xstrtok_r(char *, const char *, char **);
extern	char	 *xstrtok(char *, const char *);
extern	void	  chk4spch( char*);
extern	void	  cleanse_path( const char*, char*, int);
extern	char	 *xdup(char const *);
extern	char	 *xgetenv(char const *);
extern  int       tmpfile_mod( char**, FILE**);
extern	char	 *stringer(char *, int, ...);
extern	EM_RetVal	ReturnValue( Tcl_Interp*, int);
extern	void	  OutputExit();
extern	int	  module_setenv(const char *var, const char *val);
extern	is_Result	is_(const char *type, const char *path);

/** utilmem.c **/
extern	void	 *module_malloc(size_t);
extern	void	 *module_realloc(void *, size_t);
extern	void	 *module_calloc(size_t,size_t);
extern	void	  null_free(void **);

/** utilobj.c **/
extern	int	  Tcl_ArgvToObjv(int *, Tcl_Obj ***, int, char * const *);
extern	int	  Tcl_ObjvToArgv(int *, char ***, int, Tcl_Obj * CONST84 *);
extern	int	  Tcl_ObjvToUvec(uvec **, int, Tcl_Obj * CONST84 *);
extern	int	  Tcl_FreeObjv(Tcl_Obj ***);

extern	MHash	 *mhash_ctor(MHashType);
extern	int	  mhash_dtor(MHash **);
extern	MHash	 *mhash_copy(MHash *);
extern	int	  mhash_del_(MHash *, const char *key, ...);
extern	int	  mhash_del(MHash *, const char *key, ...);
extern	int	  mhash_add(MHash *, const char *key, ...);
extern	int	  mhash_exists(MHash *);
extern	MHashType mhash_type(MHash *);
extern	int	  mhash_number(MHash *);
extern	uvec	 *mhash_keys_uvec(MHash *);
extern	char	**mhash_keys(MHash *);
extern	void	 *mhash_value(MHash *, const char *key);

/** error.c **/
extern	char	**GetFacilityPtr( char *);
extern	int 	  Module_Error(	ErrType, char*, int, ...);
extern	int	  CheckFacility( char*, int*, int*);
extern	void	  Module_Verbosity( int, char**);

#endif  /**  _MODULES_DEF_H  **/
