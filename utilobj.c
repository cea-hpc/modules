/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		utilobj.c					     **
 **   First Edition:	2009/08/20					     **
 ** 									     **
 **   Authors:	R.K. Owen, rk@owen.sj.ca.us				     **
 ** 									     **
 **   Description:	General Tcl_Obj related routines		     **
 **			which are not necessarily specific to any single     **
 **			block of functionality.				     **
 ** 									     **
 **   Exports:		Tcl_ArgvToObjv					     **
 **			Tcl_ObjvToArgv					     **
 **									     **
 **   Notes:								     **
 ** 									     **
 ** ************************************************************************ **
 ****/

/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2009 by R.K. Owen			                      	     **
 ** see LICENSE.GPL, which must be provided, for details		     **
 ** 									     ** 
 ** ************************************************************************ **/

static char Id[] = "@(#)$Id: utilobj.c,v 1.2 2009/08/23 06:57:17 rkowen Exp $";
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

static	char	module_name[] = __FILE__;

#if WITH_DEBUGGING_UTIL_2
static	char	_proc_Tcl_ArgvToObjv[] = "Tcl_ArgvToObjv";
static	char	_proc_Tcl_ObjvToArgv[] = "Tcl_ObjvToArgv";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Tcl_ArgvToObjv					     **
 ** 									     **
 **   Description:	Take a Unix NULL terminated vector of char strings   **
 **			and create a Tcl_Obj vector.			     **
 **			If argc < 0 then count the elements		     **
 ** 									     **
 **   First Edition:	2009/08/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Tcl Interpreter	     **
 **			int		*objc		objv element count   **
 **			Tcl_Obj 	**objv[]	objv vector	     **
 **			int		argc		argv element count   **
 **			const char	*argv[]		argv vector	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Tcl_ArgvToObjv(
	Tcl_Interp * interp,
	int *objc,
	Tcl_Obj *** objv,
	int argc,
	const char **argv
) {
	const char    **aptr = argv;
	Tcl_Obj       **optr;

#if WITH_DEBUGGING_UTIL_2
	ErrorLogger(NO_ERR_START, LOC, _proc_Tcl_ArgvToObjv, NULL);
#endif

	/** if argc < 0 then count the number of elements **/
	if (argc < 0) {
		argc = 0;
		while (aptr && *aptr) {
			aptr++;
			argc++;
		}
	}

	*objc = argc;

	/** allocate the necessary memory **/
	if (!(optr = *objv =
	     (Tcl_Obj **) module_malloc((argc + 1) * sizeof(Tcl_Obj *))))
		if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
			return (TCL_ERROR);   /** ----- EXIT (FAILURE) ----> **/

	/** create the list of TCL objects **/
	while (argc--) {
		*optr = Tcl_NewStringObj(*argv++, -1);
		/* must increment the reference count */
		Tcl_IncrRefCount(*optr++);
	}
	*optr = (Tcl_Obj *) NULL;

	return (TCL_OK);		      /** ----- EXIT (SUCCESS) ----> **/

} /** End of 'Tcl_ArgvToObjv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Tcl_ObjvToArgv					     **
 ** 									     **
 **   Description:	Take a Unix NULL terminated vector of char strings   **
 **			and create a Tcl_Obj vector.			     **
 ** 									     **
 **   First Edition:	2009/08/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Tcl Interpreter	     **
 **			int		*argc		argv element count   **
 **			const char	**argv[]	argv vector	     **
 **			int		objc		objv element count   **
 **			Tcl_Obj 	*objv[]		objv vector	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Tcl_ObjvToArgv(
	Tcl_Interp * interp,
	int *argc,
	char ***argv,
	int objc,
	Tcl_Obj * CONST84 * objv
) {
	Tcl_Obj		**optr = objv;
	char    	**aptr;

#if WITH_DEBUGGING_UTIL_2
	ErrorLogger(NO_ERR_START, LOC, _proc_Tcl_ObjvToArgv, NULL);
#endif

	/** if objc < 0 then count the number of elements **/
	if (objc < 0) {
		objc = 0;
		while (optr && *optr) {
			optr++;
			objc++;
		}
	}

	*argc = objc;

	/** allocate the necessary memory **/
	if (!(aptr = *argv =
	     (char **) module_malloc((objc + 1) * sizeof(char *))))
		if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
			return (TCL_ERROR);   /** ----- EXIT (FAILURE) ----> **/

	/** create the list of strings **/
	while (objc--) {
		*aptr++ = stringer(NULL,0, Tcl_GetString(*optr++), NULL);
	}
	*aptr = (char *) NULL;

	return (TCL_OK);		      /** ----- EXIT (SUCCESS) ----> **/

} /** End of 'Tcl_ObjvToArgv' **/

