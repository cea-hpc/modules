/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdConflict.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:	The Tcl conflict and prereq commands.		     **
 ** 									     **
 **   Exports:		cmdConflict					     **
 **			cmdPrereq					     **
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

static char Id[] = "@(#)$Id: cmdConflict.c,v 1.24 2010/10/08 19:52:09 rkowen Exp $";
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

static char error_module[ MOD_BUFSIZE];
static	char	module_name[] = __FILE__;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		checkConflict					     **
 ** 									     **
 **   Description:	Check whether the 'g_current_module' is in the list  **
 **			of passed modules				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **			char		*path		Modulepath to be chk.**
 **			char		**modulelist	List of loaded mod.  **
 **			int		 nummodules	Number of loaded mod.**
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **									     **
 **		  	g_current_module	Module to check for	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static int checkConflict(
	Tcl_Interp * interp,
	char *path,
	char **modulelist,
	unsigned int nummodules
) {
	uvec           *new_modulelist;
	int             new_nummodules, k;
	is_Result       fstate;
	char           *buffer;

	memset(error_module, '\0', MOD_BUFSIZE);

    /**
     **  Check all modules passed to me as parameter
     **  At first clarify if they really do exist ...
     **/
	for (k = 0; k < nummodules; k++) {
		if ((char *)NULL == (buffer = stringer(NULL, 0,
		path, psep, modulelist[k], NULL)))
			if (OK != ErrorLogger(ERR_STRING, LOC, NULL))
				goto unwind0;
		if (!(fstate = is_("what", buffer))) {
			if (OK != ErrorLogger(ERR_FILEINDIR, LOC,
				modulelist[k], path, NULL))
				if ((char *)NULL ==
				    stringer(error_module, MOD_BUFSIZE,
					     modulelist[k], NULL))
					if (OK !=
					    ErrorLogger(ERR_STRING, LOC, NULL))
						goto unwind1;
			goto unwind1;
		}
	/**
	 **  Is it a directory what has been passed? If it is, list the
	 **  according directory and call myself recursively in order to
	 **/
		if (fstate == IS_DIR) {
			if (NULL == (new_modulelist = SortedDirList(path,
			modulelist[k], &new_nummodules)))
				continue;

			if (TCL_ERROR == checkConflict(interp, path,
			uvec_vector(new_modulelist), new_nummodules)) {
				FreeList(&new_modulelist);
				goto unwind1;
			}
			FreeList(&new_modulelist);
	/**
	 **  If it isn't a directory, check the current one for to be the
	 **  required module file
	 **/
		} else {
			if (IsLoaded_ExactMatch (modulelist[k], NULL, NULL)
			    && strcmp(g_current_module, modulelist[k])) {

		/**
                 **  Save the name of the offending module in a buffer
                 **  for reporting purposes when we get back to the top.
                 **/

				if ((char *)NULL ==
				    stringer(error_module, MOD_BUFSIZE,
					     modulelist[k], NULL))
					if (OK !=
					    ErrorLogger(ERR_STRING, LOC, NULL))
						goto unwind1;
				goto unwind1;
			}

		} /** if( directory) **/
	} /** for **/
    /**
     ** free resources
     **/
	null_free((void *)&buffer);

	return (TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/

unwind1:
	null_free((void *)&buffer);
unwind0:
	return (TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

} /** End of 'checkConflict' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdConflict					     **
 ** 									     **
 **   Description:	Callback function for 'conflict'		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 objc		Number of arguments  **
 **			Tcl_Obj		*objv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			ModulePathVec	MODULEPATHS uvec		     **
 **			ModulePath	Vector of MODULEPATHS		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int cmdConflict(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	uvec           *modulelist;	/** List of modules		     **/
	int             i, j,		/** Loop counters		     **/
	                numpaths, nummodules;
					/** Size of the according arrays     **/
    /**
     **  Whatis mode
     **/
	if (g_flags & (M_WHATIS | M_HELP))
		goto success0;
    /**
     **  Check the parameters. Usage is 'conflict <module> [<module> ...]'
     **/
	if (objc < 2)
		if (OK != ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				      "conflicting-modulefiles", NULL))
			goto unwind0;
    /**
     **  There will be no conflicts in case of switch or unload
     **/
	if (g_flags & (M_REMOVE | M_SWITCH))
		goto success0;
    /**
     **  Load the MODULEPATH and split it into a list of paths. Assume success
     **  if no list to be built...
     **/
	if (!ModulePathVec || !(numpaths = uvec_number(ModulePathVec)))
		goto success0;
    /**
     **  Non-persist mode?
     **/
	if (g_flags & M_NONPERSIST) {
		return (TCL_OK);
	}
    /**
     **  Display?
     **/
	if (g_flags & M_DISPLAY) {
		fprintf(stderr, "%s\t ", Tcl_GetString(*objv++));
		while (--objc)
			fprintf(stderr, "%s ", Tcl_GetString(*objv++));
		fprintf(stderr, "\n");
		goto success0;
	}
    /**
     **  Now check/display all passed modules ...
     **/
	for (i = 1; i < objc && Tcl_GetString(objv[i]); i++) {
		for (j = 0; j < numpaths; j++) {
			modulelist = SortedDirList(ModulePath[j],
				   Tcl_GetString(objv[i]), &nummodules);
			if (!modulelist)
				continue; /** no list to browse		     **/
			if (modulelist && !uvec_number(modulelist)) {
				FreeList(&modulelist);
				continue; /** not browseable		     **/
			}
	    /**
	     **  Actually checking for conflicts is done here
	     **/
			if (TCL_ERROR ==
			    checkConflict(interp, ModulePath[j],
					  uvec_vector(modulelist), nummodules))
				if (OK != ErrorLogger(ERR_CONFLICT, LOC,
				g_current_module, error_module, NULL)) {
					FreeList(&modulelist);
					goto unwind0;
				}
	    /**
	     **  Free the list of modules used in the loops body above.
	     **/
			FreeList(&modulelist);
		} /** for( j) **/
	} /** for( i) **/
    /**
     ** free resources
     **/
success0:
	return (TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
unwind0:
	return (TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
} /** End of 'cmdConflict' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdPrereq					     **
 ** 									     **
 **   Description:	Callback function for 'prereq'			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 objc		Number of arguments  **
 **			Tcl_Obj		*objv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			ModulePathVec	MODULEPATHS uvec		     **
 **			ModulePath	Vector of MODULEPATHS		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int cmdPrereq(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	uvec          **savedlists = (uvec **) NULL;
	int            *savedlens = (int *)NULL;
	uvec           *modulelist;		/** sorted module list **/
	char           *notloaded_flag;		/** missing module **/
	int             i, j, k, numpaths, nummodules, listcnt = 0,
	    Result = TCL_OK;
	char            buffer[MOD_BUFSIZE];

    /** 
     **  Parameter check. Usage is 'prereq <module> [<module> ...]'
     **/
	if (objc < 2)
		if (OK != ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				      "prerequisite-modules", NULL))
			goto unwind0;
    /**
     **  There's no prerequisite check in case of removal
     **/
	if (g_flags & (M_REMOVE | M_WHATIS))
		goto success0;
    /**
     **  Non-persist mode?
     **/
	if (g_flags & M_NONPERSIST) {
		return (TCL_OK);
	}
    /**
     **  Display mode
     **/
	if (g_flags & M_DISPLAY) {
		fprintf(stderr, "%s\t ", Tcl_GetString(*objv++));
		while (--objc)
			fprintf(stderr, "%s ", Tcl_GetString(*objv++));
		fprintf(stderr, "\n");
		goto success0;
	}
    /**
     **  Load the MODULEPATH and split it into a list of paths. Assume success
     **  if no list to be built...
     **/
	if (!(numpaths = uvec_number(ModulePathVec)))
		goto success0;
    /**
     **  Allocate memory for the lists of conflict modules
     **/
	if (!(savedlists
	= (uvec **) module_malloc(numpaths * (objc - 1) * sizeof(uvec *))))
		if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
			goto unwind0;
	if (!(savedlens
	= (int *)module_malloc(numpaths * (objc - 1) * sizeof(int))))
		if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
			goto unwind1;
    /**
     **  Check/Display all passed modules
     **/
	notloaded_flag = Tcl_GetString(objv[1]);
	for (i = 1; i < objc && Tcl_GetString(objv[i]) && notloaded_flag; i++) {
		for (j = 0; j < numpaths && notloaded_flag; j++) {
			modulelist = SortedDirList(ModulePath[j],
				Tcl_GetString(objv[i]), &nummodules);
			if (modulelist && !uvec_number(modulelist)) {
				FreeList(&modulelist);
				continue;
			}
	    /**
	     **  save the list of files to be printed in case of missing pre-
	     **  requisites or 
	     **/
			savedlens[listcnt] = nummodules;
			savedlists[listcnt++] = modulelist;
	    /**
	     **  Now actually check if the prerequisites are fullfilled
	     **  The notloaded_flag controls the exit from both loops in case
	     **  a prerequisite is missing.
	     **/
			for (k = 0; k < nummodules && notloaded_flag; k++) {
				if (!IsLoaded(uvec_vector(modulelist)[k], NULL,
				     NULL)) {
					notloaded_flag = Tcl_GetString(objv[i]);
				} else {
					notloaded_flag = NULL;
				}
			}
		} /** for( j) **/
	} /** for( i) **/
    /**
     **  Display an error message if this was *NOT* display mode and a
     **  missing prerequisite has been found
     **/
	if (notloaded_flag) {
	/**
	 **  Add the whole list of prerequired module files to the Tcl result
	 **  string
	 **/
		for (k = 0; k < listcnt; k++) {
			uvec           *listptr = savedlists[k];

			*buffer = '\0';
			for (i = 0; listptr && i < savedlens[k];i++) {
				if (!stringer(buffer + strlen(buffer),
					MOD_BUFSIZE - strlen(buffer),
					uvec_vector(listptr)[i], " ", NULL)) {
					if (OK != ErrorLogger(ERR_STRING, LOC,
							      NULL)) {
						/* not completely clean */
						goto unwind2;
					}
				}
			}
			FreeList(savedlists + k);
		}

		if (strlen(buffer) > 0)			/* remove last blank */
			buffer[strlen(buffer) - 1] = '\0';

		if (OK != ErrorLogger(ERR_PREREQ, LOC, g_current_module, buffer,
				      NULL))
			Result = TCL_ERROR;
	} else {
	/**
	 **  We have to free the saved module names again
	 **/
		for (k = 0; k < listcnt; k++)
			FreeList(savedlists + k);
	}
    /**
     **  Free up the list of prerequisites and return ...
     **/
	null_free((void *)&savedlens);
	null_free((void *)&savedlists);

success0:
	return (Result);		/** -------- EXIT (Result)  -------> **/

unwind2:
	null_free((void *)&savedlens);
unwind1:
	null_free((void *)&savedlists);
unwind0:
	return (TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
} /** End of 'cmdPrereq' **/
