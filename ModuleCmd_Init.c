/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Init.c				     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	Routines that act on a user's "dot" startup files to **
 **			add, remove, and list modulefiles to/from/in their   **
 **			startup files.					     **
 ** 									     **
 **   Exports:		ModuleCmd_Init					     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Init.c,v 1.7.20.1 2010/11/11 18:23:18 rkowen Exp $";
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

static	char	module_name[] = "ModuleCmd_Init.c";	/** File name of this module **/

#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Init[] = "ModuleCmd_Init";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/**				    STATIC FUNCTIONS			     **/
/** ************************************************************************ **/

/* Handles the output of a substring where the start & ending positions
 * are given - if either is NULL then just do nothing and return -1
 * all other cases it returns 0
 */
static int out_substr(FILE *stream, char *start, char *end) {
	char save;

	if (!start || !end) return -1;

	save = *end;
	*end = '\0';
	fputs(start, stream);
	*end = save;
	return 0;
}


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Init					     **
 ** 									     **
 **   Description:	Execution of the module-command 'init'		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	ModuleCmd_Init(	Tcl_Interp	*interp,
	       		int            	 argc,
	       		char		*argv[])
{
    char	 *home_pathname,
		 *home_pathname2,
		 **shell_startups;	/** A list off all startup files our **/                                        /** invoking shell will source       **/
    int		  max_home_path = MOD_BUFSIZE + 40;
    Tcl_RegExp	 modcmdPtr = Tcl_RegExpCompile(interp,
	"^([ \t]*module[ \t]+)(load|add)[ \t]+([^#\n]*)([#.\n]*)");
    char	**modlist,
		 *home,
		 *buffer,
		  ch,
		 *startp, *endp;
    FILE	 *fileptr, *newfileptr;
    int		  i, j,
		  found_module_command = 0,
		  found_modload_flag = 0,
		  shell_num = 0,
		  final_list_num = 0,
		  nummods, bufsiz = 8192,
		  new_file,
		  homelen, home_end, path_end;

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger(NO_ERR_START, LOC, _proc_ModuleCmd_Init, NULL);
#endif

    /**
     **  If called with no arguments and the flags don't say that there's some-
     **  thing to do - exit now!
     **/
    if (argc < 1 && !(g_flags & (M_DISPLAY | M_CLEAR)))
	goto success0;

    /**
     **  Parameter check for the initswitch command
     **/
    if (g_flags & M_SWITCH) {
	argc--;
	if (argc != 1)
	    if (OK != ErrorLogger(ERR_USAGE, LOC,
				  "initswitch oldmodule newmodule", NULL))
		goto unwind0;
    }

    /**
     **  Where's my HOME?
     **/
    if ((char *) NULL == (home = (char *) getenv("HOME")))
	if (OK != ErrorLogger(ERR_HOME, LOC, NULL))
	    goto unwind1;

    /**
     **  Put HOME into a buffer and store a slash where the end of HOME is
     **  for quick concatination of the shell startup files.
     **/
    homelen = strlen(home) + 40;
    if ((char *) NULL ==
	(home_pathname = stringer(NULL, homelen, home, "/", NULL)))
	if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
	    goto unwind0;

    if ((char *) NULL == (home_pathname2 = stringer(NULL, homelen, NULL)))
	if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
	    goto unwind1;

    home_end = strlen(home_pathname);

    /**
     **  Allocate a buffer for fgets ...
     **/
    if (NULL == (buffer = stringer(NULL, bufsiz, NULL)))
	if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
	    goto unwind2;

    /**
     **  Scan all startup files related to the current invoking shell
     **/
    if ((char **) NULL == (shell_startups = SetStartupFiles(shell_name)))
	goto unwind3;

    while (shell_startups[shell_num]) {
	new_file = 1;
	found_modload_flag = 0;

	if ((char *) NULL == stringer(home_pathname + home_end, 40,
				      shell_startups[shell_num], NULL))
	    if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
		goto unwind3;

	if (NULL == (fileptr = fopen(home_pathname, "r")))
	    goto unwhile0; 	/** while( shell_startups) ...	     **/

	/**
	 **  ... when the startup file exists ...
	 **  open a new startupfile with the extension -NEW for output
	 **/
	path_end = strlen(home_pathname);
	if ((char *) NULL == stringer(home_pathname + path_end,
				      homelen - path_end, "-NEW", NULL))
	    if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
		goto unwind3;

	if (!(g_flags & M_DISPLAY) &&
	    ((FILE *) NULL == (newfileptr = fopen(home_pathname, "w")))) {
	    (void) ErrorLogger(ERR_OPEN, LOC, home_pathname, "init", NULL);
	    goto unwhile0; 	/** while( shell_startups) ...	     **/
	}

	/**
	 **  Seek for a modules load|add command within the shell startup file
	 **  Copy the shell input file to the new one until the magic cookie
	 **  is found.
	 **/
	while (fgets(buffer, bufsiz, fileptr)) {
	    if (Tcl_RegExpExec(interp, modcmdPtr, buffer, buffer)) {
		found_modload_flag = 1;
		/**
		 **  ... module load|add found ...
		 **/
		found_module_command = 1;

		/* print out the "module" part */
		(void) Tcl_RegExpRange(modcmdPtr, 1,
				       (CONST84 char **) &startp,
				       (CONST84 char **) &endp);
		if (!(g_flags & M_DISPLAY))
		    (void) out_substr(newfileptr, startp, endp);

		/* print out the "add/load" part */
		(void) Tcl_RegExpRange(modcmdPtr, 2,
				       (CONST84 char **) &startp,
				       (CONST84 char **) &endp);
		if (!(g_flags & M_DISPLAY))
		    (void) out_substr(newfileptr, startp, endp);

		if (!(g_flags & M_CLEAR)) {
		    /* look at the "module list" part */
		    (void) Tcl_RegExpRange(modcmdPtr, 3,
					   (CONST84 char **) &startp,
					   (CONST84 char **) &endp);
		    /* save the end character & set to 0 */
		    if (endp) {
			ch = *endp;
			*endp = '\0';
		    }

		    if ((char **) NULL ==
		    (modlist = SplitIntoList(interp, startp, &nummods," \t")))
			continue; /** while(fgets) **/

		    /* restore the list end character */
		    if (endp)
			*endp = ch;

		    if (g_flags & M_DISPLAY) {
			if (modlist[0] == NULL) {
			    fprintf(stderr,
	    "\nNo modules are loaded in %s's initialization file "
				    "$HOME/%s\n", shell_name,
				    shell_startups[shell_num]);
			} else {
			    if (new_file) {
				fprintf(stderr,
	    "\n%s initialization file $HOME/%s loads modules:\n\t",
				    shell_name, shell_startups[shell_num]);
		    		(void) out_substr(stderr, startp, endp);
				fputs("\n",stderr);
				new_file = 0;
			    } else {
				fputs("\t",stderr);
		    		(void) out_substr(stderr, startp, endp);
				fputs("\n",stderr);
			    }
			}

			FreeList(modlist, nummods);
			continue; /** while(fgets) **/
		    }

		    for (i = 0; i < argc; i++) {
		    /**
		     ** Search through the modlist of modules that are currently
		     ** in the ~/.startup.  If one is found, it handles removing
		     ** it, switching it, etc.
		     **/
			for (j = 0; j < nummods; j++) {
			    if (modlist[j] && !strcmp(modlist[j], argv[i])) {
				if (g_flags & (M_LOAD | M_REMOVE)) {
				/**
				 **  If removing, adding, prepending it,
				 **  NULL it off the list.
				 **/
				    if (g_flags & M_REMOVE)
					fprintf(stderr, "Removed %s\n",
						modlist[j]);
				    else if ((g_flags & M_LOAD)
					     && !(g_flags & M_PREPEND))
					fprintf(stderr, "Moving %s to end\n",
						modlist[j]);
				    else if (g_flags & M_PREPEND)
					fprintf(stderr,
						"Moving %s to beginning\n",
						modlist[j]);
				    null_free((void *) (modlist + j));

				} else if (g_flags & M_SWITCH) {
				/**
				 **  If switching it, swap the old string with
				 **  the new string in the list.
				 **/
				    fprintf(stderr, "Switching %s to %s\n",
					    modlist[j], argv[i + 1]);
				    null_free((void *) (modlist + j));
				    modlist[j] = strdup(argv[i + 1]);
				}
			    } /** if **/
			} /** for(j) **/
		    } /** for(i) **/
		/**
		 **  Ok, if we're removing it, prepending it, or switching it, 
		 **  the modlist contains what needs to be put where...
		 **/
		    if ((new_file) && (g_flags & M_PREPEND)) {
		    /**
		     **  PREPENDING
		     **/
			for (i = 0; i < argc; i++) {
			    fprintf(newfileptr, " %s", argv[i]);
			    final_list_num++;
			}
		    }

		    if ((g_flags & (M_LOAD | M_REMOVE | M_SWITCH))) {
		    /**
		     **  DUMP LIST
		     **/
			for (j = 0; j < nummods; j++) {
			    if (modlist[j]) {
				fprintf(newfileptr, " %s", modlist[j]);
				final_list_num++;
			    }
			}
		    }
		    if ((new_file) && (g_flags & M_LOAD)
		    && !(g_flags & M_PREPEND)) {
		    /**
		     **  ADDING
		     **/
			for (i = 0; i < argc; i++) {
			    fprintf(newfileptr, " %s", argv[i]);
			    final_list_num++;
			}
		    }
		    /* always place a null if an empty list */
		    if (!final_list_num)
			fprintf(newfileptr, " %s", "null");

		    FreeList(modlist, nummods);

		} else { /** if( M_CLEAR) **/
		/**
		 ** Clear out the list, but leave a "null"
		 **/
		    fprintf(newfileptr, " %s", "null");
		}
	/**
	 **  Restore any comments at the end of the line...
	 **/
		(void) Tcl_RegExpRange(modcmdPtr, 4,
				       (CONST84 char **) &startp,
				       (CONST84 char **) &endp);
		(void) out_substr(newfileptr, startp, endp);
		new_file = 0;
	    } else {		/* not module load line */
		if (!(g_flags & M_DISPLAY))
		    fputs(buffer, newfileptr);
	    }
	} /** while (fgets) **/
	if (g_flags & M_DISPLAY) {
	    fputs("\n",stderr);
	}

	if (!found_modload_flag) {
	    /**
	     **  If not found...
	     **/
	    if (EOF == fclose(fileptr))
		if (OK != ErrorLogger(ERR_CLOSE, LOC, home_pathname, NULL))
		    goto unwind3;

	    if (!(g_flags & M_DISPLAY)) {
		if (EOF == fclose(newfileptr))
		    if (OK != ErrorLogger(ERR_CLOSE, LOC, home_pathname, NULL))
			goto unwind3;

		if (0 > unlink(home_pathname))
		    if (OK != ErrorLogger(ERR_UNLINK, LOC, home_pathname, NULL))
			goto unwind3;
	    }
	} else {		/* found_modload_flag */
	    /**
	     **  Don't need these any more
	     **/
	    if (EOF == fclose(fileptr))
		if (OK != ErrorLogger(ERR_CLOSE, LOC, home_pathname, NULL))
		    goto unwind3;

	    if (g_flags & M_DISPLAY)
	        goto unwhile0; 	/** while( shell_startups) ...	     **/

	    if (EOF == fclose(newfileptr))
		if (OK != ErrorLogger(ERR_CLOSE, LOC, home_pathname, NULL))
		    goto unwind3;

	    /**
	     **  Truncate -NEW from home_pathname and Create a -OLD name
	     **  Move ~/.startup to ~/.startup-OLD
	     **/
	    home_pathname[path_end] = '\0';

	    if ((char *) NULL == stringer(home_pathname2, homelen,
					  home_pathname, "-OLD", NULL))
		if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
		    goto unwind3;

	    if (0 > rename(home_pathname, home_pathname2))
		if (OK !=
		    ErrorLogger(ERR_RENAME, LOC, home_pathname, home_pathname2,
				NULL))
		    goto unwind3;

	    /**
	     **  Create a -NEW name
	     **  Move ~/.startup-NEW to ~/.startup
	     **/
	    if ((char *) NULL == stringer(home_pathname2, homelen,
					  home_pathname, "-NEW", NULL))
		if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
		    goto unwind3;

	    if (0 > rename(home_pathname2, home_pathname)) {
		if (OK !=
		    ErrorLogger(ERR_RENAME, LOC, home_pathname2, home_pathname,
				NULL)) {
		    /**
		     **  Put the -OLD one back if I can't rename it
		     **/
		    if ((char *) NULL == stringer(home_pathname2, homelen,
						  home_pathname, "-OLD", NULL))
			if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
			    goto unwind3;

		    if (0 > rename(home_pathname2, home_pathname))
			ErrorLogger(ERR_RENAME, LOC, home_pathname2,
				    home_pathname, NULL);

		    goto unwind3;
		}
	    }

	    /**
	     **  So far we're successful so
	     **  Create a -OLD name
	     **  Unlink ~/.startup-OLD
	     **/
	    if ((char *) NULL == stringer(home_pathname2, homelen,
					  home_pathname, "-OLD", NULL))
		if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
		    goto unwind3;

	    if ((g_flags & (M_CLEAR | M_LOAD | M_REMOVE | M_SWITCH)))
		if (0 > unlink(home_pathname2)) {
		    ErrorLogger(ERR_UNLINK, LOC, home_pathname2, NULL);
		    goto unwind3;
		}
	}
    unwhile0:
	shell_num++;
    } /** while( shell_startups) **/

    /**
     **  Free up internal I/O buffers
     **/
    null_free((void *) &buffer);

    if (!found_module_command)
	if (OK != ErrorLogger(ERR_INIT_STUP, LOC, shell_name, NULL))
	    goto unwind2;

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger(NO_ERR_END, LOC, _proc_ModuleCmd_Init, NULL);
#endif

    /**
     **  Free up memory
     **/
    null_free((void *) &home_pathname2);
    null_free((void *) &home_pathname);

success0:
    return (TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind3:
    null_free((void *) &buffer);
unwind2:
    null_free((void *) &home_pathname2);
unwind1:
    null_free((void *) &home_pathname);
unwind0:
    return (TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** end of 'ModuleCmd_Init' **/
