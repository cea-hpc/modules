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
 **   Exports:		ModuleCmd_Bootstrap					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Bootstrap.c,v 1.3 2002/04/29 19:58:06 lakata Exp $";
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

static	char	module_name[] = "ModuleCmd_Bootstrap.c";	/** File name of this module **/

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
 **   Function:		ModuleCmd_Bootstrap					     **
 ** 									     **
 **   Description:	Execution of the module-command 'Bootstrap'		     **
 ** 									     **
 **   First Edition:	91/10/23					     **
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

  char        command[8192];
  char       *modcmd;
  FILE       *aliasfile=0;
  char       *aliasfilename;
  char        binary_path[1024];


  /*
   * First get an absolute path to this modulecmd binary *
   */
  if (binary_name[0] != '/') { 
    char cwd[1024];
    getcwd(cwd,1024);
    sprintf(binary_path,"%s/%s",cwd,binary_name);
  } else {
    strcat(binary_path,binary_name);
  }

  /*
   * now generate the commands to create the 'module' command.
   * For sh and csh types, it is easiest to write commands to
   * a file and source it. Otherwise, it is too hard to do inside
   * an eval expression.
    */
  if( !strcmp( shell_derelict, "csh")) {
    if(tmpfile_mod(&aliasfilename,&aliasfile)) {
      ErrorLogger( ERR_OPEN, LOC, aliasfilename, NULL);
      return(TCL_ERROR);
    }
    /*
     * as you can see, the csh/tcsh is particularly bothersome
     * because of conflicts with history characters and
     * glob (*,?,{,}) characters, and also the prompt. So
     * we unset the histchars, prompt and glob operation
     * while the command is running.
     */
    modcmd = "
set prefix=\"\"
set postfix=\"\"

if ( $?histchars ) then
  set histchar = `echo $histchars | cut -c1`
  set _histchars = $histchars

  set prefix  = 'unset histchars;'
  set postfix = 'set histchars = $_histchars;'
else
  set histchar = \\!
endif

if ($?prompt) then
  set prefix  = \"$prefix\"'set _prompt=\"$prompt\";set prompt=\"\";'
  set postfix = \"$postfix\"'set prompt=\"$_prompt\";unset _prompt;'
endif

if ($?noglob) then
  set prefix  = \"$prefix\"\"set noglob;\"
  set postfix = \"$postfix\"\"unset noglob;\"
endif

alias module $prefix'eval `%s %s '$histchar'*`; '$postfix
unset exec_prefix
unset prefix
unset postfix
";
  } else if( !strcmp( shell_derelict, "sh")) {
    if(tmpfile_mod(&aliasfilename,&aliasfile)) {
      ErrorLogger( ERR_OPEN, LOC, aliasfilename, NULL);
      return(TCL_ERROR);
    }
    modcmd = "module() {\n eval `%s %s $*`;\n}\n";
  } else if( !strcmp( shell_derelict, "perl")) {
    modcmd = "sub module { eval `%s %s @_`; }\n";
  } else if( !strcmp( shell_derelict, "python")) {
    modcmd = "
import os, string

def module(command, *arguments):
        commands = os.popen('%s %s %%s %%s' %% (command, string.join(arguments))).read()
        exec commands
}
";
  } else {
    ErrorLogger( ERR_SHELL, LOC, shell_derelict, NULL);
    return(TCL_ERROR);
  }

  /*
   * now we either spit out the commands to stderr, or
   * spit them to a file, and source them from stderr
   */
  if (aliasfile) {
    fprintf(aliasfile,modcmd,binary_path,shell_name);
    fclose(aliasfile);
    free(aliasfilename);
    if( !strcmp( shell_derelict, "csh")) {
      printf("source %s ; /bin/rm -f %s ; ",aliasfilename,aliasfilename);
    } else if (!strcmp( shell_derelict, "sh")) {
      printf(". %s ; /bin/rm -f %s; ",aliasfilename,aliasfilename);
    }
  } else {
    printf(modcmd,binary_path,shell_name);
  }


  /*
   * now that we got the hard shell-dependent stuff out of the way,
   * the rest of the initializations are done by the module commands
   * themselves!
   * We construct a little TCL script and then execute it.
   */
  
  sprintf(command,"
setenv MODULE_VERSION %s
setenv MODULESHOME %s
if { ! [info exists env(MODULEPATH)    ] } { setenv MODULEPATH    \"%s\" }
if { ! [info exists env(LOADEDMODULES) ] } { setenv LOADEDMODULES \"\"}
",
	  version_string,PREFIX,MODULEPATH);
  
  g_flags |= M_LOAD; /* this is necessary so that the setenv do something */
  
  tmp_interp = Tcl_CreateInterp();
  if( TCL_OK != (return_val = InitializeModuleCommands( tmp_interp)))
    return( return_val);	/** -------- EXIT (FAILURE) -------> **/

  if (Tcl_Eval(tmp_interp,command) != TCL_OK) {
    ErrorLogger( ERR_COMMAND, LOC, command, NULL);
  }

  Tcl_DeleteInterp(tmp_interp);

  g_flags &= ~M_LOAD;

  return( TCL_OK);
} /** end of 'ModuleCmd_Bootstrap' **/



