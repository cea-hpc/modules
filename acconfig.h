/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		acconfig.h					     **
 **   Revision:		1.6						     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		Leif Hedstrom, hedstrom@inf.ethz.ch			     **
 ** 									     **
 **   Description:	Config file, used to create config.h.in (with	     **
 **			autoheader)					     **
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

/* HAS_X11LIBS:
 *     This symbol, if defined, indicates that the C program can use X11
 *     directly.
 */
#undef HAS_X11LIBS


/* TCL_MINOR_VERSION:
 * TCL_MAJOR_VERSION:
 *     This symbol, if defined, contains the version of TCL (and TCLX)
 */
#undef TCL_MINOR_VERSION
#undef TCL_MAJOR_VERSION


/* HAS_TCLXLIBS:
 *     This symbol, if defined, indicates that Modules should support
 *     extended TCL (tclX) commands.
 */
#undef HAS_TCLXLIBS


/* CACHE_UMASK:
 *     This symbol, if defined, is used as the umask for creating
 *     cachefiles for the module avail command.
 */
#undef CACHE_UMASK


/* CACHE_AVAIL:
 *     This symbol, if defined, indicates that Modules should cache the
 *     directories in MODULEPATH when doing an avail.
 */
#undef CACHE_AVAIL


/* AUTOLOADPATH:
 *     This symbol, if defined, is the "default" path of directories
 *     Tcl will look for functions to autoload.  It is space separated!
 */
#undef AUTOLOADPATH


/* MODULE_INIT_DIR:
 *     This symbol declares where the module init scripts are located
 *     for the bootstrap code.
 */
#undef MODULE_INIT_DIR


/* EVAL_ALIAS:
 *     This symbol, if defined, indicates that Modules should ask the shell
 *     to evaluate aliases via stdout like other environment changes instead
 *     of opening a file and then asking the shell to source the file and
 *     then remove it.
 */
#undef EVAL_ALIAS


/* HAS_BOURNE_FUNCS:
 *     This symbol, if defined, indicates that Modules should ask the shell
 *     to evaluate aliases via stdout like other environment changes instead
 *     of opening a file and then asking the shell to source the file and
 *     then remove it.
 */
#undef HAS_BOURNE_FUNCS


/* LMSPLIT_SIZE:
 *     This symbol, if defined, will force the environment variable
 *     _LM_FILES to be split into smaller "chunks".
 */
#undef LMSPLIT_SIZE


/* FORCE_PATH:
 *     This symbol, if defined, is the "default" path built-in to the
 *     modulecmd app. Note: There is no longer a FORCE_PATH_BASE!
 */
#undef FORCE_PATH
#define FORCE_PATH_SACRED  NULL
#define FORCE_LD_LIBRARY_PATH_SACRED NULL


/* PHOSTNAME:
 *      Last resort to get the hostname, if uname() and gethostname()
 *      are not available.
 */
#undef PHOSTNAME


/* CPPSTDIN:
 *	This symbol contains the first part of the string which will invoke
 *	the C preprocessor on the standard input and produce to standard
 *	output.	 Typical value of "cc -E" or "/lib/cpp".
 */
#undef CPPSTDIN


/* CPPMINUS:
 *	This symbol contains the second part of the string which will invoke
 *	the C preprocessor on the standard input and produce to standard
 *	output.  This symbol will have the value "-" if CPPSTDIN needs a minus
 *	to specify standard input, otherwise the value is "".
 */
#define CPPMINUS ""


/* USE_FREE:
 *	By default, we will not call free() for any dynamic data. This
 *	define enables these calls again.
 */
#undef USE_FREE

/* WANTS_VERSIONING
 *	Does the system want to use modules versions
 */
#undef WANTS_VERSIONING

/* BASEPREFIX
 * Equivalent to /usr/local
 */
#undef BASEPREFIX

/* PREFIX
 * Equivalent to /usr/local/Modules[/version_number]
 */
#undef PREFIX

/* MODULEPATH:
 *	Directory that contains all the module files
 */
#undef MODULEPATH

/* VERSIONPATH:
 *	Directory that contains the env.modules version files.
 */
#undef VERSIONPATH

/* MODULES_INIT_DIR:
 *	Directory that contains the modules init files.
 */
#undef MODULES_INIT_DIR

/* DOT_EXT:
 *	Set this to some extension value (e.g. .ext) to be append onto
 *	each shell dot-file name in the list for each shell for processing
 *	the "module init*" commands.  For example - C-shell: .cshrc.ext,
 *	.login.ext.
 */
#define DOT_EXT		""

/* TMP_DIR:
 *  * tmp directory name
 *   */
#define TMP_DIR "/tmp"

/* VERSION_MAGIC:
 *	By default, .version files will require to have the modulefile
 *	magic cookie to be valid, else an error message will be printed out.
 *	Set to 0 to disable.
 */
#define VERSION_MAGIC 1


/* WITH_DEBUG_INFO:
 *      Debug info level if defined
 */
#undef WITH_DEBUG_INFO

/* WITH_LOGGING:
 *      Syslog logging and facilities
 */
#undef WITH_LOGGING
#undef WITH_LOG_DIR
#undef WITH_LOG_FACILITY_VERBOSE
#undef WITH_LOG_FACILITY_INFO
#undef WITH_LOG_FACILITY_DEBUG
#undef WITH_LOG_FACILITY_TRACE
#undef WITH_LOG_FACILITY_WARN
#undef WITH_LOG_FACILITY_PROB
#undef WITH_LOG_FACILITY_ERROR
#undef WITH_LOG_FACILITY_FATAL
#undef WITH_LOG_FACILITY_PANIC

/* Tracing */
#undef WITH_TRACE_LOAD
#undef WITH_TRACE_UNLOAD
#undef WITH_TRACE_SWITCH
#undef WITH_TRACE_DISP
#undef WITH_TRACE_LIST
#undef WITH_TRACE_AVAIL
#undef WITH_TRACE_HELP
#undef WITH_TRACE_INIT
#undef WITH_TRACE_USE
#undef WITH_TRACE_UNUSE
#undef WITH_TRACE_UPDATE
#undef WITH_TRACE_PURGE
#undef WITH_TRACE_CLEAR
#undef WITH_TRACE_WHATIS
#undef WITH_TRACE_APROPOS

