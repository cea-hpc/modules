/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Update.c				     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description: 	Uses the beginning environment stored the first time **
 **			each login session to reload all of the currently    **
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

static char Id[] = "@(#)$Id: ModuleCmd_Update.c,v 1.6.18.5 2011/11/28 21:13:15 rkowen Exp $";
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

#define	UPD_BUFSIZE	1024

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = "ModuleCmd_Update.c";	/** File name of this module **/
#if WITH_DEBUGGING_MODULECMD
static	char	_proc_ModuleCmd_Update[] = "ModuleCmd_Update";
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

/** not applicable **/


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ModuleCmd_Update				     **
 ** 									     **
 **   Description:	Execution of the module-command 'update'	     **
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
 **   Attached Globals:	flags		Controllig the callback functions    **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	ModuleCmd_Update(	Tcl_Interp	*interp,
                 		int		 count,
                 		char		*module_list[])
{
#ifdef  BEGINENV
    char	 *buf,			/** Read buffer			     **/
		 *var_ptr,		/** Pointer to a variables name	     **/
		 *val_ptr,		/** Pointer to a variables value     **/
		**load_list,		/** List of loaded modules	     **/
		 *tmpload,		/** LOADEDMODULES contents	     **/
		 *loaded,		/** Buffer for tokenization	     **/
		 *filename;		/** The name of the file, where the  **/
					/** beginning environment resides    **/
    FILE	 *file;			/** Handle to read in a file	     **/
    int		  list_count = 0,
		  maxlist = 16,		/** Max. number of list entries	     **/
		  buffer_size = UPD_BUFSIZE;	
    					/** Current size of the input buffer **/
    char	 *ptr, c;		/** Read pointers and char buffer    **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Update, NULL);
#endif

#if BEGINENV == 99
    	if (!Tcl_GetVar2( interp,"env","MODULESBEGINENV", TCL_GLOBAL_ONLY)) {
		ErrorLogger( ERR_BEGINENVX, LOC, NULL);
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	}
#endif
    /**
     **  Nothing loaded so far - we're ready!
     **/

    if( !( tmpload = (char *) getenv("LOADEDMODULES"))) {
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    goto unwind0;
	else
	    goto success0;
    }

    /**
     **  First I'll update the environment with what's in _MODULESBEGINENV_
     **/
    filename = EMGetEnv( interp,"_MODULESBEGINENV_");
    if(filename && *filename) {

	/**
	 **  Read the begining environment
	 **/
	if( NULL != (file = fopen( filename, "r"))) {

	    if((char *) NULL == (buf = stringer(NULL, buffer_size, NULL )))
		if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		    goto unwind0;

	    while( !feof( file)) {

		/**
		 **  Trigger to entries of the type
		 **    <variable> = <value>
		 **/

		ptr = buf;
		while( !feof( file)) {
		    
		    if((ptr-buf) >= buffer_size-10) {	/** 10 bytes safety  **/
			null_free((void *) &buf);
			if((char *) NULL == (buf = stringer(NULL,
				buffer_size += UPD_BUFSIZE, NULL )))
			    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
				goto unwind0;
		    }

		    /**
		     **  Read a character and put it into the read buffer. Check
		     **  for the lines (CR) or a terminator character ...
		     **/
		    if( '\n' == (*ptr++ = c = fgetc( file))) {
			*ptr++ = c = '\0';
			break;
		    }

		    if( !c)
			break;

		} /** while **/

		/**
		 **  If there hasn't been a terminator so far, put it at the
		 **  end of the line. Therefor we've left a safety space at the
		 **  buffers end ;-)
		 **/
		if( c)
		    *ptr++ = '\0';
		 
		/**
		 **  Now let's evaluate the read line
		 **/
		if( var_ptr = strchr( buf, '=')) {
		
		    *var_ptr = '\0';
		    val_ptr = var_ptr+1;
		    var_ptr = buf;

		    /**
		     **  Reset the environment to the values derivered from the
		     **  _MODULESBEGINENV_.
		     **  Do not change the LOADEDMODULES variable ;-)
		     **  Do not change the TCL_LIBRARY and TK_LIBRARY also.
		     **/
		    if( strncmp( var_ptr, "LOADEDMODULES", 12) &&
			strncmp( var_ptr, "TCL_LIBRARY", 10 ) &&
			strncmp( var_ptr, "TK_LIBRARY", 9 ))
			if( !strncmp( var_ptr, "MODULEPATH", 10))
			    moduleSetenv( interp, var_ptr, val_ptr, 1);
			else
			    EMSetEnv( interp, var_ptr, val_ptr);
		} /** if( var_ptr) **/
	    } /** while **/

	    /**
	     **  Close the _MODULESBEGINENV_ file anf free up the read buffer.
	     **/
	    null_free((void *) &buf);

	    if( EOF == fclose( file))
		if( OK != ErrorLogger( ERR_CLOSE, LOC, filename, NULL))
		    goto unwind0;

	} else { /** if( fopen) **/

	    if( OK != ErrorLogger( ERR_OPEN, LOC, filename, "reading", NULL))
		goto unwind0;

	} /** if( fopen) **/
    } /** if( filename) **/

    /**
     **  Allocate memory for a buffer to tokenize the list of loaded modules
     **  and a list buffer
     **/
    if( NULL == (load_list = (char**) module_malloc( maxlist*sizeof(char**))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind0;
    
    if( NULL == (loaded = stringer(NULL, 0, tmpload, NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind1;
    
    /**
     **  Tokenize and build the list
     **/
    if( *loaded) {

	for( load_list[ list_count++] = xstrtok( loaded, ":");
	     load_list[ list_count-1];
             load_list[ list_count++] = xstrtok( NULL, ":") ) {

	    /**
	     **  Conditionally we have to double the space, we've allocated for
	     **  the list
	     **/

            if( list_count >= maxlist) {
                maxlist = maxlist<<1;
            
                if(!(load_list = (char**) module_realloc((char *) load_list,
		    maxlist*sizeof(char**))))
		    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
			goto unwind1;

            } /** if( maxlist) **/
	} /** for **/

	/**
	 **  Load all the modules in the list
	 **/
        ModuleCmd_Load( interp, 1, list_count, load_list);
    }

    /**
     **  Free up what has been allocated and return on success
     **/
    null_free((void *) &loaded);
    null_free((void *) &load_list);

success0:
#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Update, NULL);
#endif

    return( TCL_OK);			/** -------- EXIT (SUCCESS) -------> **/

unwind1:
    null_free((void *) &load_list);
unwind0:
    null_free((void *) &filename);
#else	/* BEGINENV */
	ErrorLogger( ERR_BEGINENV, LOC, NULL);
#endif	/* BEGINENV */
    return( TCL_ERROR);			/** -------- EXIT (FAILURE) -------> **/

} /** End of 'ModuleCmd_Update' **/
