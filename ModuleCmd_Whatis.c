/*****
 ** ** Module Header ******************************************************* **
 **									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 **									     **
 **   File:		ModuleCmd_Whatis.c				     **
 **   First Edition:	1995/12/31					     **
 **									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 **									     **
 **			ModuleCmd_Apropos				     **
 **			ModuleCmd_Whatis				     **
 **									     **
 **   Notes:								     **
 **									     **
 ** ************************************************************************ **
 ****/

/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1991-1994 by John L. Furlan.                      	     **
 ** see LICENSE.GPL, which must be provided, for details		     **
 ** 									     ** 
 ** ************************************************************************ **/

static char Id[] = "@(#)$Id$";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/**				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

/** ************************************************************************ **/
/**				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/**				     CONSTANTS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

#define WHATIS_SOME	0
#define WHATIS_ALL	1

/** ************************************************************************ **/
/**				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = __FILE__;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	int	 whatis_dir( char*, int, char**, int);

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		ModuleCmd_Whatis				     **
 **									     **
 **   Description:	Display the passed modules 'whatis' information	     **
 **									     **
 **   First Edition:	1995/12/31					     **
 **									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument list	     **
 **									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 **									     **
 ** ************************************************************************ **
 ++++*/

int ModuleCmd_Whatis(
	Tcl_Interp * interp,
	int argc,
	char *argv[]
) {
	Tcl_Interp     *whatis_interp;
	Tcl_DString     cmdbuf;			/** dynamic string buffer    **/
	int             i,			/** loop index		     **/
	                result = TCL_OK;	/** result (default OK	     **/
	char            modulefile[MOD_BUFSIZE],/** buffer for modulefile    **/
	                modulename[MOD_BUFSIZE],/** buffer for modulename    **/
	              **wptr,		/** whatis text line		     **/
	              **dirname;	/** modulepath dir		     **/
    /**
     **	 Initialize the command buffer and set up the modules flag to
     **	 'whatisonly'
     **/
	Tcl_DStringInit(&cmdbuf);
	g_flags |= M_WHATIS;

    /**
     **	 Handle each passed module file. Create a Tcl interpreter for each
     **	 module file to be handled and initialize it with custom module commands
     **/

	if (argc) {
    /**
     **	 User provided a list of modules for ``whatis'' info
     **/
		for (i = 0; i < argc && argv[i]; i++) {
			whatis_interp = EM_CreateInterp();
			if (TCL_OK != (result = Module_Init(whatis_interp))) {
				EM_DeleteInterp(whatis_interp);
				result = TCL_ERROR;
				break;
			}
	    /**
	     **	 locate the filename related to the passed module
	     **/
			if (TCL_ERROR ==
			    Locate_ModuleFile(whatis_interp, argv[i],
					      modulename, modulefile)) {
				EM_DeleteInterp(whatis_interp);
				if (OK !=
				    ErrorLogger(ERR_LOCATE, LOC, argv[i], NULL))
					break;
				else
					continue;
			}
	    /**
	     **	 Print out everything that would happen if the module file were
	     **	 executed ...
	     **/
			g_current_module = modulename;

			cmdModuleWhatisInit();
			result = CallModuleProcedure(whatis_interp, &cmdbuf,
						     modulefile,
						     "ModulesWhatis", 0);
	    /**
	     **	 Print the result ...
	     **/
			if (whatis) {
				wptr = whatis;
				while (*wptr)
					fprintf(stderr, "%-21s: %s\n", argv[i],
						*wptr++);
			}
	    /**
	     **	 Remove the Tcl interpreter that has been used for printing ...
	     **/
			EM_DeleteInterp(whatis_interp);
			cmdModuleWhatisShut();
		}
	  /** for **/
	} else {
       /**
	**  User wants all module ``whatis'' info
	**/
	/**
	 **  Load the MODULEPATH and split it into a list of paths.
	 **  Assume success if no list is to be displayed ...
	 **/
		if (!uvec_number(ModulePathVec))
			goto success0;
	/**
	 **  Scan all the files
	 **/
		dirname = ModulePath;
		while (dirname && *dirname) {
			if (!check_dir(*dirname))
				continue;
			whatis_dir(*dirname, argc, argv, WHATIS_ALL);
			dirname++;
		}
	}
    /**
     **	 Leave the 'whatis only mode', free up what has been used and return
     **/
	g_flags &= ~M_WHATIS;
	fprintf(stderr, "\n");

	Tcl_DStringFree(&cmdbuf);
    /**
     **	 Return on success
     **/

success0:
	return (result);		/** --- EXIT PROCEDURE (result)  --> **/

unwind0:
	return (TCL_ERROR);		/** --- EXIT PROCEDURE (FAILURE) --> **/

} /** End of 'ModuleCmd_Whatis' **/

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		ModuleCmd_Apropos				     **
 **									     **
 **   Description:	Scan the whatis database in order to find something  **
 **			matching the passed strings			     **
 **									     **
 **   First Edition:	1995/12/31					     **
 **									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char		*argv[]		Argument list	     **
 **									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successful operation	     **
 **									     **
 **   Attached Globals:							     **
 **									     **
 ** ************************************************************************ **
 ++++*/

int ModuleCmd_Apropos(
	Tcl_Interp * interp,
	int argc,
	char *argv[]
) {
	char          **dirname,	/** modulepath dir		     **/
	               *c;
	int             i;
    /**
     **	 Ignore case ... convert all arguments to lower case
     **/
	if (sw_icase)
		for (i = 0; i < argc; i++)
			for (c = argv[i]; c && *c; c++)
				*c = tolower(*c);

	/**
	 **  Load the MODULEPATH and split it into a list of paths.
	 **  Assume success if no list is to be displayed ...
	 **/
	if (!uvec_number(ModulePathVec))
		goto success0;

	dirname = ModulePath;
	while (dirname && *dirname) {
		if (!check_dir(*dirname))
			continue;

		whatis_dir(*dirname++, argc, argv, WHATIS_SOME);
	}

success0:
	return (TCL_OK);

unwind0:
	return (TCL_ERROR);			/** ---- EXIT (FAILURE) ---> **/

} /** End of 'ModuleCmd_Apropos' **/

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		whatis_dir					     **
 **									     **
 **   Description:	Print all files beyond the passed directory	     **
 **									     **
 **   First Edition:	1991/10/23					     **
 **									     **
 **   Parameters:	char	*dir		Directory to be scanned	     **
 **			int	 argc		Number of tokens	     **
 **			char   **argv		List of tokens to check	     **
 **									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 **									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 **			g_current_module	The module which is handled  **
 **						by the current command	     **
 **									     **
 ** ************************************************************************ **
 ++++*/

static	int	whatis_dir( char *dir, int argc, char **argv,
			    int whatis_list)
{
    fi_ent	 *dirlst_head = NULL;	/** Directory list base pointer	     **/
    int		  count = 0;		/** Number of elements in the top    **/
					/** level directory list	     **/
    int		  tcount = 0;		/** Total number of files to print   **/
    char	**list;			/** flat list of module files	     **/
    int		  start = 0, i, k;
    int		  result = TCL_OK;
    Tcl_Interp	 *whatis_interp;
    Tcl_DString	  cmdbuf;
    char	  modulefile[ MOD_BUFSIZE];
    char	**wptr, *c;

    /**
     **	 Normal reading of the files
     **/

    if(!(dirlst_head = get_dir( dir, NULL, &count, &tcount)))
	if( OK != ErrorLogger( ERR_READDIR, LOC, dir, NULL))
	    goto unwind0;

    if(!(list = (char**) module_malloc( tcount * sizeof( char**))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind1;

    dirlst_to_list( list, dirlst_head, count, &start, NULL, NULL);

    /**
     **	 Initialize the command buffer and set up the modules flag to 'whatislay
     **	 only'
     **/

    Tcl_DStringInit( &cmdbuf);
    g_flags |= M_WHATIS;

    /**
     **	 Check all the files in the flat list for the passed tokens
     **/

    for( i=0; i<tcount; i++) {

	whatis_interp = EM_CreateInterp();
	if( TCL_OK != (result = Module_Init( whatis_interp))) {
	    EM_DeleteInterp( whatis_interp);
	    result = TCL_ERROR;
	    break; /** for( i) **/
	}

	/**
	 **  locate the filename related to the passed module
	 **/

	if(!stringer(modulefile,MOD_BUFSIZE, dir,psep,list[i],NULL)) {
	    result = TCL_ERROR;
	    break; /** for( i) **/
	}
	g_current_module = list[ i];

	if(is_("dir", modulefile))
	    continue;

	cmdModuleWhatisInit();
	result = CallModuleProcedure( whatis_interp, &cmdbuf, modulefile,
	    "ModulesApropos", 0);

	/**
	 **  Check if at least one of the passed tokens is found in the
	 **  retrieved whatis strings. If yes, print the string.
	 **/

	if( whatis) {
	    wptr = whatis;
	    while( *wptr) {

		/**
		 **  Ignore case?
		 **/

		if( sw_icase) {
		    strncpy( modulefile, *wptr, MOD_BUFSIZE);
		    for( c = modulefile; c && *c; c++)
			*c = tolower( *c);
		    c = modulefile;
		} else
		    c = *wptr;

		/**
		 **  Seek for the passed tokens
		 **/

		if( whatis_list)
		    fprintf( stderr, "%-21s: %s\n", list[i], *wptr);
		else
		    for( k=0; k<argc; k++) {
			if( strstr( c, argv[ k]))
		    	    fprintf( stderr, "%-21s: %s\n", list[i], *wptr);
		    }
		wptr++;
	    }
	}

	/**
	 **  Remove the Tcl interpreter that has been used for printing ...
	 **/

	EM_DeleteInterp( whatis_interp);
	cmdModuleWhatisShut();

    } /** for( i) **/

    /**
     **	 Cleanup
     **/
    g_flags &= ~M_WHATIS;
    delete_dirlst( dirlst_head, count);
    delete_cache_list( list, start);

    return( result);			/** ------- EXIT (result) --------> **/

/* unwind2:
    delete_cache_list( list, start); */
unwind1:
    delete_dirlst( dirlst_head, count);
unwind0:
    return( TCL_ERROR);			/** ------- EXIT (FAILURE) --------> **/

} /** End of 'whatis_dir' **/
