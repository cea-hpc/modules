/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		ModuleCmd_Update.c				     **
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description: 	Uses the beginning environment stored the first time **
 **			each login session to reload all of the currently    **
 ** 									     **
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

static char Id[] = "@(#)$Id: ModuleCmd_Update.c,v 1.1 2000/06/28 00:17:32 rk Exp $";
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
 **   First Edition:	91/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		Attached Tcl Interp. **
 **			int		 argc		Number of arguments  **
 **			char 		*argv[]		Argument list	     **
 ** 									     **
 **   Result:		int	TCL_ERROR	Failure			     **
 **				TCL_OK		Successfull operation	     **
 ** 									     **
 **   Attached Globals:	flags		Controllig the callback functions    **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int	ModuleCmd_Update(	Tcl_Interp	*interp,
                 		int		 count,
                 		char		*module_list[])
{
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
		  maxlist = 16;		/** Max. number of list entries	     **/
    int		  buffer_size;		/** Cirrent size of the input buffer **/
    char	 *ptr, c;		/** Read pointers and char buffer    **/

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_START, LOC, _proc_ModuleCmd_Update, NULL);
#endif

    /**
     **  Nothing loaded so far - we're ready!
     **/

    if( !( tmpload = (char *) getenv("LOADEDMODULES"))) {
	if( OK != ErrorLogger( ERR_MODULE_PATH, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
	else
	    return( TCL_OK);		/** -------- EXIT (SUCCESS) -------> **/
    }

    /**
     **  First I'll update the environment with what's in _MODULESBEGINENV_
     **/

    filename = Tcl_GetVar2( interp, "env", "_MODULESBEGINENV_", TCL_GLOBAL_ONLY);
    if( filename) {

	/**
	 **  Read the beginning environment
	 **/

	if( NULL != (file = fopen( filename, "r"))) {

	    if( NULL == (buf = (char*) malloc( buffer_size = UPD_BUFSIZE))) {
		if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		    return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	    }

	    while( !feof( file)) {

		/**
		 **  Trigger to entries of the type
		 **    <variable> = <value>
		 **/

		ptr = buf;
		while( !feof( file)) {
		    
		    if((ptr-buf) >= buffer_size-10) {	/** 10 bytes safety  **/
			if( NULL == (buf = (char*) malloc(
			    buffer_size += UPD_BUFSIZE))) {
			    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
				return( TCL_ERROR);	/**  EXIT (FAILURE)  **/
			}
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
			    Tcl_SetVar2( interp, "env", var_ptr, val_ptr,
					 TCL_GLOBAL_ONLY);
		} /** if( var_ptr) **/
	    } /** while **/

	    /**
	     **  Close the _MODULESBEGINENV_ file anf free up the read buffer.
	     **/

	    free(buf);

	    if( EOF == fclose( file))
		if( OK != ErrorLogger( ERR_CLOSE, LOC, filename, NULL))
		    return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/

	} else { /** if( fopen) **/

	    if( OK != ErrorLogger( ERR_OPEN, LOC, filename, "reading", NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/

	} /** if( fopen) **/
    } /** if( filename) **/

    /**
     **  Allocate memory for a buffer to tokenize the list of loaded modules
     **  and a list buffer
     **/

    if( NULL == (load_list = (char**) malloc( maxlist*sizeof(char**)))) {
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }
    
    if( NULL == (loaded = (char*) malloc( strlen( tmpload) + 1))) {
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL)) {
	    free( load_list);
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
	}
    }
    
    strcpy( loaded, tmpload);
    
    /**
     **  Tokenize and build the list
     **/

    if( *loaded) {

	for( load_list[ list_count++] = strtok( loaded, ":");
	     load_list[ list_count-1];
             load_list[ list_count++] = strtok( NULL, ":") ) {

	    /**
	     **  Conditionally we have to double the space, we've allocated for
	     **  the list
	     **/

            if( list_count >= maxlist) {
                maxlist = maxlist<<1;
            
                if( NULL == (load_list = (char**) realloc((char *) load_list,
		    maxlist*sizeof(char**)))) {
		    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL)) {
			free( load_list);
			return( TCL_ERROR);	/** ---- EXIT (FAILURE) ---> **/
		    }
                }

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

    free( loaded);
    free( load_list);

#if WITH_DEBUGGING_MODULECMD
    ErrorLogger( NO_ERR_END, LOC, _proc_ModuleCmd_Update, NULL);
#endif

    return( TCL_OK);

} /** End of 'ModuleCmd_Update' **/
