/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Bootstrap.c				     **
 **   First Edition:	2002/04/22					     **
 ** 									     **
 **   Authors:	Mark Lakata, lakata@att.net				     **
 ** 									     **
 **   Description:	Creates the necessary shell commands to install      **
 **			the 'module' alias/shell function and the dependent  **
 **			shell variables. Replaces the old $MODULESHOME/init  **
 ** 									     **
 **   Exports:		ModuleCmd_Bootstrap				     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Bootstrap.c,v 1.7 2002/06/12 20:07:57 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"
#include <unistd.h>

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

/** File name of this module **/
static	char	module_name[] = "ModuleCmd_Bootstrap.c";

#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Bootstrap[] = "ModuleCmd_Bootstrap";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

int      tmpfile_mod(char** filename, FILE** file);

/** ************************************************************************ **/
/**				    STATIC FUNCTIONS			     **/
/** ************************************************************************ **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Bootstrap				     **
 ** 									     **
 **   Description:	Execution of the module-command 'Bootstrap'	     **
 ** 									     **
 **   First Edition:	2002/04/15					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successfull operation	     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			shell_name	shell properties		     **
 **			shell_derelict					     **
 **			shell_init					     **
 **			shell_cmd_separator				     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	ModuleCmd_Bootstrap(	Tcl_Interp	*interp,
	       		int            	 argc,
	       		char		*argv[])
{
  int         result;
  int         return_val;
  Tcl_Interp *tmp_interp;		/** Tcl interpreter to be    **/

  char       *command;
  char       *initfile;

  FILE       *aliasfile=0;
  char       *aliasfilename;
  const char *execname;
  char        binary_path[1024];

  /*
   * generate the commands to source the 'module' init script.
    */
  if( !strcmp( shell_derelict, "csh")) {
	  command = "source %s/%s%s";
  } else if( !strcmp( shell_derelict, "sh")) {
	  command = ". %s/%s%s";
  } else if( !strcmp( shell_derelict, "perl")) {
	  command = "require \"%s/%s%s\"";
  } else if( !strcmp( shell_derelict, "python")) {
	  command = "execfile('%s/%s')%s";
  } else {
    ErrorLogger( ERR_SHELL, LOC, shell_name, NULL);
    return(TCL_ERROR);
  }
  fprintf( stdout, command, MODULES_INIT_DIR, shell_init, shell_cmd_separator);

#if 0
  g_flags |= M_LOAD; /* this is necessary so that the setenv do something */
  
  tmp_interp = Tcl_CreateInterp();
  if( TCL_OK != (return_val = InitializeModuleCommands( tmp_interp)))
    return( return_val);	/** -------- EXIT (FAILURE) -------> **/

  if (Tcl_Eval(tmp_interp,command) != TCL_OK) {
    ErrorLogger( ERR_COMMAND, LOC, command, NULL);
  }

  Tcl_DeleteInterp(tmp_interp);

  g_flags &= ~M_LOAD;
#endif

  return( TCL_OK);
} /** end of 'ModuleCmd_Bootstrap' **/
