/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdInfo.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:	The Tcl module-info routine which provides informa-  **
 **			tion about the state of Modules as the modulefile    **
 **			is being parsed. 				     **
 ** 									     **
 **   Exports:		cmdModuleInfo					     **
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

static char Id[] = "@(#)$Id: cmdInfo.c,v 1.17 2010/10/08 21:40:19 rkowen Exp $";
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

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		cmdModuleInfo					     **
 ** 									     **
 **   Description:	Callback function for 'module-info'		     **
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
 **   Attached Globals:	g_flags			These are set up accordingly **
 **						before this function is	     **
 **						called in order to control   **
 **						everything.		     **
 **			g_specified_module	The module name from the     **
 **						command line.		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int cmdModuleInfo(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	char           *arg0, *arg1, *arg2;
	char           *s, *t, buf[BUFSIZ];

    /**
     **  Parameter check
     **/
	arg0 = Tcl_GetString(objv[0]);
	if (objc < 2) {
		if (OK != ErrorLogger(ERR_USAGE, LOC, arg0, "info-descriptor ",
				      "descriptor-args", NULL))
			return (TCL_ERROR);	/** ---- EXIT (FAILURE) ---> **/
	}
    /**
     **  'module-info flags' ?
     **/
	arg1 = Tcl_GetString(objv[1]);
	if (objc > 2)
		arg2 = Tcl_GetString(objv[2]);
	if (!strcmp(arg1, "flags")) {
		char            tmpbuf[6];
		sprintf(tmpbuf, "%d", g_flags);
		Tcl_SetResult(interp, tmpbuf, TCL_VOLATILE);
    /**
     **  'module-info mode'
     **  without suggestion this will return the name of the state we're in.
     **/
	} else if (!strcmp(arg1, "mode")) {

		if (objc < 3) {
			if (g_flags & M_SWSTATE1)
				Tcl_SetResult(interp, "switch1", TCL_STATIC);
			else if (g_flags & M_SWSTATE2)
				Tcl_SetResult(interp, "switch2", TCL_STATIC);
			else if (g_flags & M_SWSTATE3)
				Tcl_SetResult(interp, "switch3", TCL_STATIC);
			else if (g_flags & M_LOAD)
				Tcl_SetResult(interp, "load", TCL_STATIC);
			else if (g_flags & M_REMOVE)
				Tcl_SetResult(interp, "remove", TCL_STATIC);
			else if (g_flags & M_DISPLAY)
				Tcl_SetResult(interp, "display", TCL_STATIC);
			else if (g_flags & M_HELP)
				Tcl_SetResult(interp, "help", TCL_STATIC);
			else if (g_flags & M_WHATIS)
				Tcl_SetResult(interp, "whatis", TCL_STATIC);
			else if (g_flags & M_NONPERSIST)
				Tcl_SetResult(interp, "nonpersist", TCL_STATIC);
			else
				Tcl_SetResult(interp, "unknown", TCL_STATIC);
		} else {
	    /**
	     **  'module-info mode <my_suggestion>'
	     **  results in a boolean return indicating whether this is the
	     **  correct state ...
	     **/
			Tcl_SetResult(interp, "0", TCL_STATIC);

			if (!strcmp(arg2, "load")) {
				if (g_flags & M_LOAD)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "remove")) {
				if (g_flags & M_REMOVE)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "display")) {
				if (g_flags & M_DISPLAY)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "nonpersist")) {
				if (g_flags & M_NONPERSIST)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "help")) {
				if (g_flags & M_HELP)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "whatis")) {
				if (g_flags & M_WHATIS)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "switch1")) {
				if (g_flags & M_SWSTATE1)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "switch2")) {
				if (g_flags & M_SWSTATE2)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "switch3")) {
				if (g_flags & M_SWSTATE3)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "switch")) {
				if (g_flags & M_SWITCH)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else {
				ErrorLogger(ERR_USAGE, LOC, arg0, "mode ",
					    "[load|remove|display|help|whatis|switch1|switch2|switch3|switch|nonpersist]",
					    NULL);
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
			}
		}
    /**
     **  'module-info user'
     **  without suggestion this will return the current user level
     **/
	} else if (!strcmp(arg1, "user")) {
		if (objc < 3) {
			if (UL_NOVICE == sw_userlvl)
				Tcl_SetResult(interp, "novice", TCL_STATIC);
			else if (UL_ADVANCED == sw_userlvl)
				Tcl_SetResult(interp, "advanced", TCL_STATIC);
			else if (UL_EXPERT == sw_userlvl)
				Tcl_SetResult(interp, "expert", TCL_STATIC);
			else
				Tcl_SetResult(interp, "unknown", TCL_STATIC);
		} else {
	    /**
	     **  'module-info user <my_suggestion>'
	     **  results in a boolean return indicating whether this is the
	     **  correct user level ...
	     **/
			Tcl_SetResult(interp, "0", TCL_STATIC);

			if (!strcmp(arg2, "novice")) {
				if (UL_NOVICE == sw_userlvl)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "advanced")) {
				if (UL_ADVANCED == sw_userlvl)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else if (!strcmp(arg2, "expert")) {
				if (UL_EXPERT == sw_userlvl)
					Tcl_SetResult(interp, "1", TCL_STATIC);
			} else {
				ErrorLogger(ERR_USAGE, LOC, arg0, "user ",
					    "[novice|advanced|expert]", NULL);
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
			}
		}
    /**
     **  'module-info name'
     **  returns the name of the current module
     **/
	} else if (!strcmp(arg1, "name")) {
		Tcl_SetResult(interp, g_current_module, TCL_VOLATILE);
    /**
     **  'module-info shell'
     **  returns the name of the current user shell
     **/
	} else if (!strcmp(arg1, "shell")) {
		if (objc < 3) {
			Tcl_SetResult(interp, shell_name, TCL_VOLATILE);
		} else {
			if (!strcmp(arg2, shell_name))
				Tcl_SetResult(interp, "1", TCL_STATIC);
			else
				Tcl_SetResult(interp, "0", TCL_STATIC);
		}
	} else if (!strcmp(arg1, "shelltype")) {
		if (objc < 3) {
			Tcl_SetResult(interp, shell_derelict, TCL_VOLATILE);
		} else {
			if (!strcmp(arg2, shell_derelict))
				Tcl_SetResult(interp, "1", TCL_STATIC);
			else
				Tcl_SetResult(interp, "0", TCL_STATIC);
		}
	} else if (!strcmp(arg1, "alias")) {
		if (objc < 3) {
			if (OK != ErrorLogger(ERR_USAGE, LOC, arg0, "alias ",
					      "name", NULL))
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
		}

		if (AliasLookup((char *)arg2, &s, &t)) {
			stringer(buf, BUFSIZ, s, psep, t, NULL);
			Tcl_SetResult(interp, buf, TCL_VOLATILE);
		} else {
			Tcl_SetResult(interp, "*undef*", TCL_STATIC);
		}
    /**
     **  'module-info symbols'
     **  List all symbolic names of the passed or current module file
     **/
	} else if (!strcmp(arg1, "symbols")) {
		char           *name;

		name = (objc < 3) ? g_current_module : (char *)arg2;

		if (!(s = ExpandVersions(name)))
			Tcl_SetResult(interp, "*undef*", TCL_STATIC);
		else
			Tcl_SetResult(interp, (char *)s, TCL_VOLATILE);
    /**
     **  'module-info version'
     **  Returns the full qualified module name and version of the passed
     **  symbolic version specifier
     **/
	} else if (!strcmp(arg1, "version")) {
		if (VersionLookup((char *)arg2, &s, &t)) {
			if (t) {
				stringer(buf, BUFSIZ, s, psep, t, NULL);
			} else {
				stringer(buf, BUFSIZ, s, NULL);
			}
			Tcl_SetResult(interp, buf, TCL_VOLATILE);
		} else {
			Tcl_SetResult(interp, "*undef*", TCL_STATIC);
		}
    /**
     **  'module-info specified'
     **   gives the module name as specified on the command line
     **/
	} else if (!strcmp(arg1, "specified")) {
		if (g_specified_module) {
			/* TCL_STATIC because it comes from the command line */
			Tcl_SetResult(interp, g_specified_module, TCL_STATIC);
		} else {
			Tcl_SetResult(interp, "*undef*", TCL_STATIC);
		}
    /**
     **  unknown command ....
     **/
	} else {
		if (OK != ErrorLogger(ERR_INFO_DESCR, LOC, arg1, NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}

	return (TCL_OK);

} /** End of 'cmdModuleInfo' **/
