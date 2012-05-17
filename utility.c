/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		utility.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 ** 									     **
 **   Description:	General routines that are called throughout Modules  **
 **			which are not necessarily specific to any single     **
 **			block of functionality.				     **
 ** 									     **
 **   Exports:		store_hash_value				     **
 **			clear_hash_value				     **
 **			Delete_Global_Hash_Tables			     **
 **			Delete_Hash_Tables				     **
 **			Copy_Hash_Tables				     **
 **			Unwind_Modulefile_Changes			     **
 **			Output_Modulefile_Changes			     **
 **			IsLoaded_ExactMatch				     **
 **			IsLoaded					     **
 **			chk_marked_entry				     **
 **			set_marked_entry				     **
 **			Update_LoadedList				     **
 **			check_magic					     **
 **			regex_quote					     **
 **			xstrtok						     **
 **			xstrtok_r					     **
 **			chk4spch					     **
 **			module_malloc					     **
 **			xdup						     **
 **			xgetenv						     **
 **			stringer					     **
 **			null_free					     **
 **			countTclHash					     **
 **									     **
 **			strdup		if not defined by the system libs.   **
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

static char Id[] = "@(#)$Id: utility.c,v 1.19.6.9 2011/11/28 21:27:13 rkowen Exp $";
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

static	char	module_name[] = "utility.c";	/** File name of this module **/

#if WITH_DEBUGGING_UTIL_2
static	char	_proc_store_hash_value[] = "store_hash_value";
static	char	_proc_clear_hash_value[] = "clear_hash_value";
static	char	_proc_Clear_Global_Hash_Tables[] = "Clear_Global_Hash_Tables";
static	char	_proc_Delete_Global_Hash_Tables[] = "Delete_Global_Hash_Tables";
static	char	_proc_Delete_Hash_Tables[] = "Delete_Hash_Tables";
static	char	_proc_Copy_Hash_Tables[] = "Copy_Hash_Tables";
static	char	_proc_Unwind_Modulefile_Changes[] = "Unwind_Modulefile_Changes";
static	char	_proc_Output_Modulefile_Changes[] = "Output_Modulefile_Changes";
static	char	_proc_Output_Modulefile_Aliases[] = "Output_Modulefile_Aliases";
static	char	_proc_Output_Directory_Change[] = "Output_Directory_Change";
static	char	_proc_output_set_variable[] = "output_set_variable";
static	char	_proc_output_unset_variable[] = "output_unset_variable";
static	char	_proc_output_function[] = "output_function";
static	char	_proc_output_set_alias[] = "output_set_alias";
static	char	_proc_output_unset_alias[] = "output_unset_alias";
static	char	_proc_getLMFILES[] = "getLMFILES";
static	char	_proc___IsLoaded[] = "__IsLoaded";
static	char	_proc_chk_marked_entry[] = "chk_marked_entry";
static	char	_proc_set_marked_entry[] = "set_marked_entry";
static	char	_proc_get_module_basename[] = "get_module_basename";
static	char	_proc_Update_LoadedList[] = "Update_LoadedList";
static	char	_proc_check_magic[] = "check_magic";
static	char	_proc_cleanse_path[] = "cleanse_path";
static	char	_proc_chop[] = "chop";
#endif

static	FILE *aliasfile;		/** Temporary file to write aliases  **/
static	char *aliasfilename;		/** Temporary file name		     **/
static	char  alias_separator = ';';	/** Alias command separator	     **/
static	const int   eval_alias = 	/** EVAL_ALIAS macro		     **/
#ifdef EVAL_ALIAS
	1
#else
	0
#endif
;
static	const int   bourne_funcs = 	/** HAS_BOURNE_FUNCS macro	     **/
#ifdef HAS_BOURNE_FUNCS
	1
#else
	0
#endif
;
static	const int   bourne_alias = 	/** HAS_BOURNE_FUNCS macro	     **/
#ifdef HAS_BOURNE_ALIAS
	1
#else
	0
#endif
;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	void	 Clear_Global_Hash_Tables( void);
static	int	 Output_Modulefile_Aliases( Tcl_Interp *interp);
static	int	 Output_Directory_Change(Tcl_Interp *interp);
static	int	 output_set_variable( Tcl_Interp *interp, const char*,
				      const char*);
static	int	 output_unset_variable( const char* var);
static	void	 output_function( const char*, const char*);
static	int	 output_set_alias( const char*, const char*);
static	int	 output_unset_alias( const char*, const char*);
static	int	 __IsLoaded( Tcl_Interp*, char*, char**, char*, int);
static	char	*get_module_basename( char*);
static	char	*chop( const char*);
static  void     EscapeCshString(const char* in,
				 char* out);
static  void     EscapeShString(const char* in,
				 char* out);
static  void     EscapePerlString(const char* in,
				 char* out);
static  void     EscapeCmakeString(const char* in,
				 char* out);


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		store_hash_value				     **
 ** 									     **
 **   Description:	Keeps the old value of the variable around if it is  **
 **			touched in the modulefile to enable undoing a	     **
 **			modulefile by resetting the evironment to it started.**
 ** 									     **
 **			This is the same for unset_shell_variable()	     **
 ** 									     **
 **   First Edition:	1992/10/14					     **
 ** 									     **
 **   Parameters:	Tcl_HashTable	*htable		Hash table to be used**
 **			const char	*key		Attached key	     **
 **			const char	*value		Alias value	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int store_hash_value(	Tcl_HashTable* htable,
        		const char*    key,
        		const char*    value)
{
    int   		 new;		/** Return from Tcl_CreateHashEntry  **/
					/** which indicates creation or ref- **/
					/** ference to an existing entry     **/
    char		*tmp;		/** Temp pointer used for disalloc.  **/
    Tcl_HashEntry	*hentry;	/** Hash entry reference	     **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_store_hash_value, NULL);
#endif

    /**
     **  Create a hash entry for the key to be stored. If there exists one
     **  so far, its value has to be unlinked.
     **  All values in this hash are pointers to allocated memory areas.
     **/

    hentry = Tcl_CreateHashEntry( htable, (char*) key, &new);
    if( !new) {
	tmp = (char *) Tcl_GetHashValue( hentry);
    	if( tmp)
	    null_free((void *) &tmp);
    }

    /**
     **  Set up the new value. strdup allocates!
     **/

    if( value)
        Tcl_SetHashValue( hentry, (char*) stringer(NULL,0, (char *)value,NULL));
    else
        Tcl_SetHashValue( hentry, (char*) NULL);
    
    return( TCL_OK);

} /** End of 'store_hash_value' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		clear_hash_value				     **
 ** 									     **
 **   Description:	Remove the specified shell variable from the passed  **
 **			hash table					     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_HashTable	*htable		Hash table to be used**
 **			const char	*key		Attached key	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int clear_hash_value(	Tcl_HashTable	*htable,
                       	const char	*key)
{
    char		*tmp;		/** Temp pointer used for dealloc.   **/
    Tcl_HashEntry	*hentry;	/** Hash entry reference	     **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_clear_hash_value, NULL);
#endif

    /**
     **  If I haven't already created an entry for keeping this environment
     **  variable's value, then just leave.
     **  Otherwise, remove this entry from the hash table.
     **/

    if( hentry = Tcl_FindHashEntry( htable, (char*) key) ) {

        tmp = (char*) Tcl_GetHashValue( hentry);
        if( tmp)
	    null_free((void *) &tmp);

        Tcl_DeleteHashEntry( hentry);
    }
    
    return( TCL_OK);

} /** End of 'clear_hash_value' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Clear_Global_Hash_Tables			     **
 ** 									     **
 **   Description: 	Deletes and reinitializes our env. hash tables.	     **
 ** 									     **
 **   First Edition:	1992/10/14					     **
 ** 									     **
 **   Parameters:	-						     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	setenvHashTable,				     **
 **			unsetenvHashTable,				     **
 **			aliasSetHashTable,				     **
 **			aliasUnsetHashTable				     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	void	Clear_Global_Hash_Tables( void)
{
    Tcl_HashSearch	 searchPtr;	/** Tcl hash search handle	     **/
    Tcl_HashEntry	*hashEntry;	/** Result from Tcl hash search      **/
    char		*val = NULL;	/** Stored value (is a pointer!)     **/

    /**
     **  The following hash tables are to be initialized
     **/

    Tcl_HashTable	*table[5],
			**table_ptr = table;

    table[0] = setenvHashTable;
    table[1] = unsetenvHashTable;
    table[2] = aliasSetHashTable;
    table[3] = aliasUnsetHashTable;
    table[4] = NULL;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_Clear_Global_Hash_Tables, NULL);
#endif

    /**
     **  Loop for all the hash tables named above. If there's no value stored
     **  in a hash table, skip to the next one. 
     **/

    for( ; *table_ptr; table_ptr++) {

	if( ( hashEntry = Tcl_FirstHashEntry( *table_ptr, &searchPtr)) == NULL) 
	    continue;
	
	/**
	 **  Otherwise remove all values stored in the table
	 **/

	do {
	    val = (char*) Tcl_GetHashValue( hashEntry);
	    if( val)
		null_free((void *) &val);
	} while( hashEntry = Tcl_NextHashEntry( &searchPtr));

	/**
	 **  Reinitialize the hash table by unlocking it from memory and 
	 **  thereafter initializing it again.
	 **/

	Tcl_DeleteHashTable( *table_ptr);
	Tcl_InitHashTable( *table_ptr, TCL_STRING_KEYS);

    } /** for **/

} /** End of 'Clear_Global_Hash_Tables' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Delete_Global_Hash_Tables			     **
 **			Delete_Hash_Tables				     **
 ** 									     **
 **   Description: 	Deletes our environment hash tables.		     **
 ** 									     **
 **   First Edition:	1992/10/14					     **
 ** 									     **
 **   Parameters:	Tcl_HashTable	**table_ptr	NULL-Terminated list **
 **							of hash tables to be **
 **							deleted		     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	setenvHashTable,				     **
 **			unsetenvHashTable,				     **
 **			aliasSetHashTable,				     **
 **			aliasUnsetHashTable				     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void Delete_Global_Hash_Tables( void) {

    /**
     **  The following hash tables are to be initialized
     **/

    Tcl_HashTable	*table[5];

    table[0] = setenvHashTable;
    table[1] = unsetenvHashTable;
    table[2] = aliasSetHashTable;
    table[3] = aliasUnsetHashTable;
    table[4] = NULL;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_Delete_Global_Hash_Tables, NULL);
#endif

    Delete_Hash_Tables( table);

} /** End of 'Delete_Global_Hash_Tables' **/

void Delete_Hash_Tables( Tcl_HashTable	**table_ptr)
{

    Tcl_HashSearch	 searchPtr;	/** Tcl hash search handle	     **/
    Tcl_HashEntry	*hashEntry;	/** Result from Tcl hash search      **/
    char		*val = NULL;	/** Stored value (is a pointer!)     **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_Delete_Hash_Tables, NULL);
#endif

    /**
     **  Loop for all the hash tables named above. Remove all values stored in
     **  the table and then free up the whole table
     **/
    for( ; *table_ptr; table_ptr++) {

        if( ( hashEntry = Tcl_FirstHashEntry( *table_ptr, &searchPtr))) {

	    /**
	     **  Remove all values stored in the table
	     **/
	    do {
		val = (char*) Tcl_GetHashValue( hashEntry);
		if( val)
		    null_free((void *) &val);
	    } while( hashEntry = Tcl_NextHashEntry( &searchPtr));

	    /**
	     **  Remove internal hash control structures
	     **/
	    Tcl_DeleteHashTable( *table_ptr);
	}

	null_free((void *) table_ptr);

    } /** for **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_END, LOC, _proc_Delete_Hash_Tables, NULL);
#endif

} /** End of 'Delete_Hash_Tables' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Copy_Hash_Tables				     **
 ** 									     **
 **   Description:	Allocate new hash tables for the global environment, **
 **			initialize them and copy the contents of the current **
 **			tables into them.				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	-						     **
 **   Result:		Tcl_HashTable**		Pointer to the new list of   **
 **						hash tables		     **
 **   Attached Globals:	setenvHashTable,				     **
 **			unsetenvHashTable,				     **
 **			aliasSetHashTable,				     **
 **			aliasUnsetHashTable				     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

Tcl_HashTable	**Copy_Hash_Tables( void)
{
    Tcl_HashSearch	  searchPtr;	/** Tcl hash search handle	     **/
    Tcl_HashEntry	 *oldHashEntry,	/** Hash entries to be copied	     **/
			 *newHashEntry;
    char		 *val = NULL,	/** Stored value (is a pointer!)     **/
    			 *key = NULL;	/** Hash key			     **/
    int			  new;		/** Tcl inidicator, if the new hash  **/
					/** entry has been created or ref.   **/

    Tcl_HashTable	 *oldTable[5],
			**o_ptr, **n_ptr,
			**newTable;	/** Destination hash table	     **/

    oldTable[0] = setenvHashTable;
    oldTable[1] = unsetenvHashTable;
    oldTable[2] = aliasSetHashTable;
    oldTable[3] = aliasUnsetHashTable;
    oldTable[4] = NULL;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_Copy_Hash_Tables, NULL);
#endif

    /**
     **  Allocate storage for the new list of hash tables
     **/
    if( !(newTable = (Tcl_HashTable**) module_malloc( sizeof( oldTable))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    goto unwind0;

    /**
     **  Now copy each hashtable out of the list
     **/
    for( o_ptr = oldTable, n_ptr = newTable; *o_ptr; o_ptr++, n_ptr++) {

	/**
	 **  Allocate memory for a single hash table
	 **/
	if( !(*n_ptr = (Tcl_HashTable*) module_malloc( sizeof( Tcl_HashTable))))
	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		goto unwind1;

	/**
	 **  Initialize that guy and copy it from the old table
	 **/
	Tcl_InitHashTable( *n_ptr, TCL_STRING_KEYS);
        if( oldHashEntry = Tcl_FirstHashEntry( *o_ptr, &searchPtr)) {

	    /**
	     **  Copy all entries if there are any
	     **/
	    do {

		key = (char*) Tcl_GetHashKey( *o_ptr, oldHashEntry);
		val = (char*) Tcl_GetHashValue( oldHashEntry);

		newHashEntry = Tcl_CreateHashEntry( *n_ptr, key, &new);

		if(val)
		    Tcl_SetHashValue(newHashEntry, stringer(NULL,0, val, NULL));
		else
		    Tcl_SetHashValue(newHashEntry, (char *) NULL);

	    } while( oldHashEntry = Tcl_NextHashEntry( &searchPtr));

	} /** if **/
    } /** for **/

    /**
     **  Put a terminator at the end of the new table
     **/
    *n_ptr = NULL;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_END, LOC, _proc_Copy_Hash_Tables, NULL);
#endif

    return( newTable);

unwind1:
    null_free((void *) &newTable);
unwind0:
    return( NULL);			/** -------- EXIT (FAILURE) -------> **/
} /** End of 'Copy_Hash_Tables' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:								     **
 ** 									     **
 **   Description:	Once a the loading or unloading of a modulefile	     **
 **			fails, any changes it has made to the environment    **
 **			must be undone and reset to its previous state. This **
 **			function is responsible for unwinding any changes a  **
 **			modulefile has made.				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	 *interp	According TCL interp.**
 **			Tcl_HashTable	**oldTables	Hash tables storing  **
 **							the former environm. **
 **   Result:								     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Unwind_Modulefile_Changes(	Tcl_Interp	 *interp, 
				Tcl_HashTable	**oldTables )
{
    Tcl_HashSearch	 searchPtr;	/** Tcl hash search handle	     **/
    Tcl_HashEntry	*hashEntry;	/** Result from Tcl hash search      **/
    char		*val = NULL,	/** Stored value (is a pointer!)     **/
			*key;		/** Tcl hash key		     **/
    int			 i;		/** Loop counter		     **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_Unwind_Modulefile_Changes, NULL);
#endif

    if( oldTables) {

	/**
	 **  Use only entries 0 and 1 which do contain all changes to the 
	 **  shell varibles (setenv and unsetenv)
	 **/

	/** ??? What about the aliases (table 2 and 3) ??? **/

	for( i = 0; i < 2; i++) {
	    if( hashEntry = Tcl_FirstHashEntry( oldTables[i], &searchPtr)) {

		do {
		    key = (char*) Tcl_GetHashKey( oldTables[i], hashEntry);

		    /**
		     **  The hashEntry will contain the appropriate value for the
		     **  specified 'key' because it will have been aquired depending
		     **  upon whether the unset or set table was used.
		     **/

		    val = (char*) Tcl_GetHashValue( hashEntry);
		    if( val)
			EMSetEnv( interp, key, val);

		} while( hashEntry = Tcl_NextHashEntry( &searchPtr) );

	    } /** if **/
	} /** for **/

	/**
	 **  Delete and reset the hash tables now that the current contents have been
	 **  flushed.
	 **/

	Delete_Global_Hash_Tables();

	setenvHashTable     = oldTables[0];
	unsetenvHashTable   = oldTables[1];
	aliasSetHashTable   = oldTables[2];
	aliasUnsetHashTable = oldTables[3];

    } else {

	Clear_Global_Hash_Tables();

    }

    return( TCL_OK);

} /** End of 'Unwind_Modulefile_Changes' **/

static int keycmp(const void *a, const void *b) {
	return strcmp(*(const char **) a, *(const char **) b);
}
                                                                                
/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Output_Modulefile_Changes			     **
 ** 									     **
 **   Description:	Is used to flush out the changes of the current	     **
 **			modulefile in a manner depending upon whether the    **
 **			modulefile was successful or unsuccessful.	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		The attached Tcl in- **
 **							terpreter	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals:	setenvHashTable,				     **
 **			unsetenvHashTable,				     **
 **			aliasSetHashTable,	via Output_Modulefile_Aliases**
 **			aliasUnsetHashTable	via Output_Modulefile_Aliases**
 **			change_dir		for the chdir command	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Output_Modulefile_Changes(	Tcl_Interp	*interp)
{
    Tcl_HashSearch	  searchPtr;	/** Tcl hash search handle	     **/
    Tcl_HashEntry	 *hashEntry;	/** Result from Tcl hash search      **/
    char		 *val = NULL,	/** Stored value (is a pointer!)     **/
			 *key,		/** Tcl hash key		     **/
			**list;		/** list of keys		     **/
    int			  i,k;		/** Loop counter		     **/
    size_t		  hcnt;		/** count of hash entries	     **/

    /**
     **  The following hash tables do contain all changes to be made on
     **  shell variables
     **/

    Tcl_HashTable	*table[2];

    table[0] = setenvHashTable;
    table[1] = unsetenvHashTable;
  
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_Output_Modulefile_Changes, NULL);
#endif

    aliasfile = stdout;

    /**
     **  Scan both tables that are of interest for shell variables
     **/

    for(i = 0; i < 2; i++) {
	/* count hash */
	hcnt = countTclHash(table[i]);

	/* allocate array for keys */
	if( !(list = (char **) module_malloc(hcnt * sizeof(char *)))) {
		if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    		return(TCL_ERROR);/** ------- EXIT (FAILURE) ------> **/
	}

	/* collect keys */
	k = 0;
	if( hashEntry = Tcl_FirstHashEntry( table[i], &searchPtr))
		do {
			key = (char*) Tcl_GetHashKey( table[i], hashEntry);
			list[k++] = stringer(NULL,0, key, NULL);
		} while( hashEntry = Tcl_NextHashEntry( &searchPtr));
	/* sort hash */
	if (hcnt > 1)
		qsort((void *) list, hcnt, sizeof(char *), keycmp);

	/* output key/values */
	for (k = 0; k < hcnt; ++k) {
		key = list[k];
    		hashEntry = Tcl_FindHashEntry( table[i], key);
		/**
		 **  The table list indicator is used in order to differ
		 **  between the setenv and unsetenv operation
		 **/
		if( i == 1) {
			output_unset_variable( (char*) key);
		} else {
			val = EMGetEnv(interp, key);
			if(val && *val)
				output_set_variable(interp, (char*) key, val);
			null_free((void *)&val);
		}
	} /** for **/
	/* delloc list */
	for (k = 0; k < hcnt; ++k)
		free(list[k]);
	free(list);
    } /** for **/

    if( EOF == fflush( stdout))
	if( OK != ErrorLogger( ERR_FLUSH, LOC, _fil_stdout, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/

    Output_Modulefile_Aliases( interp);
    Output_Directory_Change( interp);

    /**
     **  Delete and reset the hash tables since the current contents have been
     **  flushed.
     **/

    Clear_Global_Hash_Tables();
    return( TCL_OK);

} /* End of 'Output_Modulefile_Changes' */ 

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Open_Aliasfile					     **
 ** 									     **
 **   Description:	Creates/opens or closes temporary file for sourcing  **
 **			or aliases.					     **
 **			Passes back the filehandle and filename in global    **
 ** 			variables.					     **
 ** 									     **
 **   First Edition:	2005/09/26	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	int	action		if != 0 to open else close   **
 ** 									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals: aliasfile					     **
 **			aliasfilename					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int Open_Aliasfile(int action)
{
    if (action) {
	/**
	 **  Open the file ...
	 **/
	if( tmpfile_mod(&aliasfilename,&aliasfile))
	    if(OK != ErrorLogger( ERR_OPEN, LOC, aliasfilename, "append", NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
    } else {
	if( EOF == fclose( aliasfile))
	    if( OK != ErrorLogger( ERR_CLOSE, LOC, aliasfilename, NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
    }

    return( TCL_OK);

} /** End of 'Open_Aliasfile' **/
/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Output_Modulefile_Aliases			     **
 ** 									     **
 **   Description:	Is used to flush out the changes to the aliases of   **
 **			the current modulefile. But, some shells don't work  **
 **			well with having their alias information set via the **
 **			'eval' command.  So, what we'll do now is output the **
 **			aliases into a /tmp dotfile, have the shell source   **
 **			the /tmp dotfile and then have the shell remove the  **
 **			/tmp dotfile.					     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		The attached Tcl in- **
 **							terpreter	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 ** 									     **
 **   Attached Globals: aliasSetHashTable,	via Output_Modulefile_Aliases**
 **			aliasUnsetHashTable	via Output_Modulefile_Aliases**
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int Output_Modulefile_Aliases( Tcl_Interp *interp)
{
    Tcl_HashSearch	 searchPtr;	/** Tcl hash search handle	     **/
    Tcl_HashEntry	*hashEntry;	/** Result from Tcl hash search      **/
    char		*val = NULL,	/** Stored value (is a pointer!)     **/
			*key;		/** Tcl hash key		     **/
    int			 i,		/** Loop counter		     **/
			 openfile = 0;	/** whether using a file or not	     **/
    char		*sourceCommand; /** Command used to source the alias **/

    /**
     **  The following hash tables do contain all changes to be made on
     **  shell aliases
     **/
    Tcl_HashTable	*table[2];

    table[0] = aliasSetHashTable;
    table[1] = aliasUnsetHashTable;

    /**
     **  If configured so, all changes to aliases are written into a temporary
     **  file which is sourced by the invoking shell ...
     **  In this case a temporary filename has to be assigned for the alias
     **  source file. The file has to be opened as 'aliasfile'.
     **  The default for aliasfile, if no shell sourcing is used, is stdout.
     **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_Output_Modulefile_Aliases, NULL);
#endif

    /**
     **  We only need to output stuff into a temporary file if we're setting
     **  stuff.  We can unset variables and aliases by just using eval.
     **/
    if( hashEntry = Tcl_FirstHashEntry( aliasSetHashTable, &searchPtr)) {

	/**
	 **  We must use an aliasfile if EVAL_ALIAS is not defined
	 **  or the sh shell does not do aliases (HAS_BOURNE_ALIAS)
	 **  and that the sh shell does do functions (HAS_BOURNE_FUNCS)
	 **/
	if (!eval_alias
	|| (!strcmp(shell_name,"sh") && !bourne_alias && bourne_funcs)) {
	    if (OK != Open_Aliasfile(1))
		if(OK != ErrorLogger(ERR_OPEN,LOC,aliasfilename,"append",NULL))
		    return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	    openfile = 1;
	}
	/**
	 **  We only support sh and csh variants for aliases.  If not either
	 **  sh or csh print warning message and return
	 **/
	assert(shell_derelict != NULL);
	if( !strcmp( shell_derelict, "csh")) {
	    sourceCommand = "source %s%s";
	} else if( !strcmp( shell_derelict, "sh")) {
	    sourceCommand = ". %s%s";
	} else {
	    return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
	}

	if (openfile) {
	    /**
	     **  Only the source command has to be flushed to stdout. After
	     **  sourcing the alias definition (temporary) file, the source
	     **  file is to be removed.
	     **/
	    alias_separator = '\n';

	    fprintf( stdout, sourceCommand, aliasfilename, shell_cmd_separator);
	    fprintf( stdout, "/bin/rm -f %s%s",
		aliasfilename, shell_cmd_separator);
	} /** openfile **/
    } /** if( alias to set) **/

    /**
     **  Scan the hash tables involved in changing aliases
     **/

    for( i=0; i<2; i++) {
    
	if( hashEntry = Tcl_FirstHashEntry( table[i], &searchPtr)) {

	    do {
		key = (char*) Tcl_GetHashKey( table[i], hashEntry);
		val = (char*) Tcl_GetHashValue( hashEntry);

		/**
		 **  The hashtable list index is used to differ between aliases
		 **  to be set and aliases to be reset
		 **/
		if(i == 1) {
		    output_unset_alias( key, val);
		} else {
		    output_set_alias( key, val);
		}

	    } while( hashEntry = Tcl_NextHashEntry( &searchPtr));

	} /** if **/
    } /** for **/


    if(openfile) {
	if( OK != Open_Aliasfile(0))
	    if( OK != ErrorLogger( ERR_CLOSE, LOC, aliasfilename, NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/

	null_free((void *) &aliasfilename);
    }

    return( TCL_OK);

} /** End of 'Output_Modulefile_Aliases' **/

/*++++
 ** ** Function-Header ***************************************************** **
 **									     **
 **   Function:		Output_Directory_Change				     **
 **									     **
 **   Description:	Changes the current working directory.               **
 **									     **
 **   Parameters:	Tcl_Interp *interp	The attached Tcl interpreter **
 **									     **
 **   Result:		int	TCL_OK		Successful operation	     **
 **				TCL_ERROR	When not applicable	     **
 **									     **
 **   Attached Global:  change_dir					     **
 **									     **
 ** ************************************************************************ **
 ++++*/
static	int Output_Directory_Change(Tcl_Interp *interp)
{
	int rc = TCL_OK;

	if (change_dir == NULL)
		return rc;

	assert(shell_derelict != NULL);
	if(!strcmp(shell_derelict, "csh") || !strcmp(shell_derelict, "sh")) {
		fprintf(stdout, "cd '%s'%s", change_dir, shell_cmd_separator);
	} else if(!strcmp( shell_derelict, "perl")) {
		fprintf(stdout, "chdir '%s'%s", change_dir, shell_cmd_separator);
	} else if( !strcmp( shell_derelict, "python")) {
		fprintf(stdout, "os.chdir('%s')\n", change_dir);
	} else if( !strcmp( shell_derelict, "ruby")) {
		fprintf(stdout, "Dir.chdir('%s')\n", change_dir);
	} else {
		rc = TCL_ERROR;
	}

	free(change_dir);
	change_dir = NULL;

	return rc;
}


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_set_variable				     **
 ** 									     **
 **   Description:	Outputs the command required to set a shell variable **
 **			according to the current shell			     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp	The attached Tcl interpreter **
 **   			const char	*var	Name of the variable to be   **
 **						set			     **
 **			const char	*val	Value to be assigned	     **
 **									     **
 **   Result:		int	TCL_OK		Finished successful	     **
 **				TCL_ERROR	Unknown shell type	     **
 **									     **
 **   Attached Globals:	shell_derelict					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	output_set_variable(	Tcl_Interp	*interp,
					const char	*var,
          	          		const char	*val)
{

    /**
     **  Differ between the different kinds od shells at first
     **
     **  CSH
     **/
    chop( val);
    chop( var);

    assert(shell_derelict != NULL);
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_output_set_variable, " var='", var,
	"' val= '", val, "'", NULL);
#endif

    if( !strcmp((char*) shell_derelict, "csh")) {

#ifdef LMSPLIT_SIZE

	/**
	 **  Many C Shells (specifically the Sun one) has a hard limit on
	 **  the size of the environment variables around 1k.  The
	 **  _LMFILES_ variable can grow beyond 1000 characters.  So, I'm
	 **  going to break it up here since I can put it back together
	 **  again when I use it.
	 **
	 **  You can set the split size using --with-split-size=<number>
	 **  it should probably be <1000.  I don't count the size of
	 **  "setenv _LMFILES_xxx" so subtract this from your limit.
	 **/
	if( !strcmp( var, "_LMFILES_")) {
	    char formatted[ MOD_BUFSIZE];
	    char *cptr = NULL;
	    int	lmfiles_len;
	    int	count = 0;
	    char* escaped = stringer(NULL,strlen(val)*2+1,NULL);
	    EscapeCshString(val,escaped);

	    if(( lmfiles_len = strlen(escaped)) > LMSPLIT_SIZE) {

	    char buffer[ LMSPLIT_SIZE + 1];

	    /**
	     **  Break up the _LMFILES_ variable...
	     **/
	    while( lmfiles_len > LMSPLIT_SIZE) {

		    strncpy( buffer, ( escaped + count*LMSPLIT_SIZE ),
			     LMSPLIT_SIZE);
		buffer[ LMSPLIT_SIZE] = '\0';

		fprintf( stdout, "setenv %s%03d %s %s", var, count, buffer,
		    shell_cmd_separator);

		lmfiles_len -= LMSPLIT_SIZE;
		count++;
	    }

		if( lmfiles_len) {
		fprintf( stdout, "setenv %s%03d %s %s", var, count,
		    (escaped + count*LMSPLIT_SIZE), shell_cmd_separator);
		    count++;
		}

	    /**
		 ** Unset _LMFILES_ as indicator to use the multi-variable
		 ** _LMFILES_
	     **/
	    fprintf(stdout, "unsetenv %s %s", var, shell_cmd_separator);

	    } else {	/** if ( lmfiles_len = strlen(val)) > LMSPLIT_SIZE) **/

		fprintf(stdout, "setenv %s %s %s", var, escaped, shell_cmd_separator);
	    }

	    /**
	     ** Unset the extra _LMFILES_%03d variables that may be set
	     **/
	    do {
		if (cptr) null_free((void *) &cptr);
		sprintf( formatted, "_LMFILES_%03d", count++);
		cptr = EMGetEnv( interp, formatted);
		if(cptr && *cptr) {
		    fprintf(stdout, "unsetenv %s %s", formatted,
			shell_cmd_separator);
		}
	    } while( cptr && *cptr);

	    null_free((void *) &cptr);
	    null_free((void *) &escaped);

	} else {	/** if( var == "_LMFILES_") **/

#endif /* not LMSPLIT_SIZE */

		char* escaped = stringer(NULL,strlen(val)*2+1,NULL);
		EscapeCshString(val,escaped);
		fprintf(stdout, "setenv %s %s %s", var, escaped,
			shell_cmd_separator);
		null_free((void *) &escaped);
#ifdef LMSPLIT_SIZE
	}
#endif /* not LMSPLIT_SIZE */

    /**
     **  SH
     **/
    } else if( !strcmp((char*) shell_derelict, "sh")) {

      char* escaped = (char*)module_malloc(strlen(val)*2+1);
      EscapeShString(val,escaped);

      fprintf( stdout, "%s=%s %sexport %s%s", var, escaped, shell_cmd_separator,
	       var, shell_cmd_separator);
      free(escaped);
      
    /**
     **  EMACS
     **/
    } else if( !strcmp((char*) shell_derelict, "emacs")) {
	fprintf( stdout, "(setenv \"%s\" \'%s\')\n", var, val);

    /**
     **  PERL
     **/
    } else if( !strcmp((char*) shell_derelict, "perl")) {
		char* escaped = stringer(NULL,strlen(val)*2+1,NULL);
		EscapePerlString(val,escaped);
		fprintf(stdout, "$ENV{'%s'} = '%s'%s", var, escaped,
			shell_cmd_separator);  
		null_free((void *) &escaped);

    /**
     **  PYTHON
     **/
    } else if( !strcmp((char*) shell_derelict, "python")) {
	fprintf( stdout, "os.environ['%s'] = '%s'\n", var, val);

    /**
     **  RUBY
     **/
    } else if( !strcmp((char*) shell_derelict, "ruby")) {
	fprintf( stdout, "ENV['%s'] = '%s'\n", var, val);

    /**
     **  CMAKE
     **/
    } else if( !strcmp((char*) shell_derelict, "cmake")) {
		char* escaped = stringer(NULL, strlen(val)*2+1,NULL);
		EscapeCmakeString(val,escaped);
		fprintf(stdout, "set(ENV{%s} \"%s\")%s", var, escaped,
			shell_cmd_separator);
		null_free((void *) &escaped);

    /**
     ** SCM
     **/
    } else if ( !strcmp((char*) shell_derelict, "scm")) {
	fprintf( stdout, "(putenv \"%s=%s\")\n", var, val);

    /**
     ** MEL (Maya Extension Language)
     **/
    } else if ( !strcmp((char*) shell_derelict, "mel")) {
        fprintf( stdout, "putenv \"%s\" \"%s\";", var, val);

    /**
     **  Unknown shell type - print an error message and 
     **  return on error
     **/
    } else {
	if( OK != ErrorLogger( ERR_DERELICT, LOC, shell_derelict, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Return and acknowldge success
     **/
    g_output = 1;
    return( TCL_OK);

} /** End of 'output_set_variable' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_unset_variable				     **
 ** 									     **
 **   Description:	Outputs the command required to unset a shell        **
 **			variable according to the current shell		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*var	Name of the variable to be   **
 **						unset			     **
 **									     **
 **   Result:		int	TCL_OK		Finished successful	     **
 **				TCL_ERROR	Unknown shell type	     **
 **									     **
 **   Attached Globals:	shell_derelict					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	output_unset_variable( const char* var)
{
    chop( var);

    assert(shell_derelict != NULL);
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_output_unset_variable, NULL);
#endif

    /**
     **  Display the 'unsetenv' command according to the current invoking shell.
     **/
    if( !strcmp( shell_derelict, "csh")) {
	fprintf( stdout, "unsetenv %s%s", var, shell_cmd_separator);
    } else if( !strcmp( shell_derelict, "sh")) {
	fprintf( stdout, "unset %s%s", var, shell_cmd_separator);
    } else if( !strcmp( shell_derelict, "emacs")) {
	fprintf( stdout, "(setenv \"%s\" nil)\n", var);
    } else if( !strcmp( shell_derelict, "perl")) {
	fprintf( stdout, "delete $ENV{'%s'}%s", var, shell_cmd_separator);  
    } else if( !strcmp( shell_derelict, "cmake")) {
	fprintf( stdout, "unset(ENV{%s})%s", var, shell_cmd_separator);  
    } else if( !strcmp( shell_derelict, "python")) {
      fprintf( stdout, "os.environ['%s'] = ''\ndel os.environ['%s']\n",var,var);
    } else if( !strcmp( shell_derelict, "ruby")) {
      fprintf( stdout, "ENV['%s'] = nil\n",var);
    } else if( !strcmp( shell_derelict, "scm")) {
	fprintf( stdout, "(putenv \"%s\")\n", var);
    } else if( !strcmp( shell_derelict, "mel")) {
	fprintf( stdout, "putenv \"%s\" \"\";", var);
    } else {
	if( OK != ErrorLogger( ERR_DERELICT, LOC, shell_derelict, NULL))
	    return( TCL_ERROR);		/** -------- EXIT (FAILURE) -------> **/
    }

    /**
     **  Return and acknowldge success
     **/
    g_output = 1;
    return( TCL_OK);

} /** End of 'output_unset_variable' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_function					     **
 ** 									     **
 **   Description:	Actually turns the Modules set-alias information     **
 **			into a string that a shell can source.  Previously,  **
 **			this routine just output the alias information to be **
 **			eval'd by the shell.				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*var	Name of the alias to be set  **
 **			const char	*val	Value to be assigned	     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	aliasfile,	The output file for alias commands.  **
 **					see 'Output_Modulefile_Aliases'      **
 **			alias_separator					     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	void	output_function(	const char	*var,
					const char	*val)
{
    const char *cptr = val;
    int nobackslash = 1;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger(NO_ERR_START, LOC, _proc_output_function, NULL);
#endif

    /**
     **  This opens a function ...
     **/
    fprintf(aliasfile, "%s() { ", var);

    /**
     **  ... now print the value. Print it as a single line and remove any
     **  backslashes, and substitute a safe $*
     **/
    while (*cptr) {

	if (*cptr == '\\') {
	    if (!nobackslash)
		putc(*cptr, aliasfile);
	    else
		nobackslash = 0;
	    cptr++;
	    continue;
	} else if (*cptr == '$' && (cptr + 1) && (*(cptr + 1) == '*')) {
	    /* found $* */
	    fputs("${1+\"$@\"}", aliasfile);
	    cptr++;
	    cptr++;
	    continue;
	} else
	    nobackslash = 1;

	putc(*cptr++, aliasfile);

    } /** while **/

    /**
     **  Finally close the function
     **/
    fprintf(aliasfile, "%c}%c", alias_separator, alias_separator);

} /** End of 'output_function' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_set_alias				     **
 ** 									     **
 **   Description:	Flush the commands required to set shell aliases de- **
 **			pending on the current invoking shell		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*alias		Name of the alias    **
 **			const char	*val		Value to be assigned **
 ** 									     **
 **   Result:		int	TCL_OK		Operation successful	     **
 ** 									     **
 **   Attached Globals:	aliasfile, 	The alias command is written out to  **
 **			alias_separator Defined the command separator	     **
 **			shell_derelict	to determine the shell family	     **
 **			shell_name	to determine the real shell type     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	output_set_alias(	const char	*alias,
               			  	const char	*val)
{
    int nobackslash = 1;		/** Controls whether backslashes are **/
					/** to be print			     **/
    const char *cptr = val;		/** Scan the value char by char	     **/

    assert(shell_derelict != NULL);
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_output_set_alias, NULL);
#endif

    /**
     **  Check for the shell family
     **  CSHs need to switch $* to \!* and $n to \!\!:n unless the $ has a
     **  backslash before it
     **/
    if( !strcmp( shell_derelict, "csh")) {

	/**
	 **  On CSHs the command is 'alias <name> <value>'. Print the beginning
	 **  of the command and then print the value char by char.
	 **/
        fprintf( aliasfile, "alias %s '", alias);

        while( *cptr) {

	    /**
	     **  Convert $n to \!\!:n
	     **/
            if( *cptr == '$' && nobackslash) {
                cptr++;
                if( *cptr == '*')
                    fprintf( aliasfile, "\\!");
                else
                    fprintf( aliasfile, "\\!\\!:");
            }

	    /**
	     **  Recognize backslashes
	     **/
            if( *cptr == '\\') {
                if( !nobackslash)
		    putc( *cptr, aliasfile);
                else
		    nobackslash = 0;
                cptr++;
                continue;
            } else
                nobackslash = 1;

	    /**
	     **  print the read character
	     **/
            putc( *cptr++, aliasfile);

        } /** while **/
 
	/**
	 **  Now close up the command using the alias command terminator as
	 **  defined in the global variable
	 **/
        fprintf( aliasfile, "'%c", alias_separator);

    /**
     **  Bourne shell family: The alias has to be  translated into a
     **  function using the function call 'output_function'
     **/
    } else if( !strcmp(shell_derelict, "sh")) {
	/**
	 **  Shells supporting extended bourne shell syntax ....
	 **/
	if( (!strcmp( shell_name, "sh") && bourne_alias)
		||  !strcmp( shell_name, "bash")
                ||  !strcmp( shell_name, "zsh" )
                ||  !strcmp( shell_name, "ksh")) {
	    /**
	     **  in this case we only have to write a function if the alias
	     **  takes arguments. This is the case if the value has '$'
	     **  somewhere in it without a '\' in front.
	     **/
	    while( *cptr) {
		if( *cptr == '\\') {
		    if( nobackslash) {
			nobackslash = 0;
		    }
		} else {
	    	   if( *cptr == '$') {
			if( nobackslash) {
				output_function( alias, val);
				return TCL_OK;
			}
		    }
		    nobackslash = 1;
		}
		cptr++;
	    }

            /**
             **  So, we can just output an alias with '\$' translated to '$'...
             **/
	    fprintf( aliasfile, "alias %s='", alias);

	    nobackslash = 1;
	    cptr = val;

	    while( *cptr) {
		if( *cptr == '\\') {
		    if( nobackslash) {
			nobackslash = 0;
			cptr++;
			continue;
		    }
		}
		nobackslash = 1;

		putc(*cptr++, aliasfile);

	    } /** while **/

	    fprintf( aliasfile, "'%c", alias_separator);

        } else if( !strcmp( shell_name, "sh")
		&&   bourne_funcs) {
	/**
	 **  The bourne shell itself
         **  need to write a function unless this sh doesn't support
	 **  functions (then just punt)
	 **/
            output_function(alias, val);
        }
	/** ??? Unknown derelict ??? **/

    } /** if( sh ) **/

    return( TCL_OK);

} /** End of 'output_set_alias' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		output_unset_alias				     **
 ** 									     **
 **   Description:	Flush the commands required to reset shell aliases   **
 **			depending on the current invoking shell		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char	*alias		Name of the alias    **
 **			const char	*val		Value which has been **
 **							assigned	     **
 ** 									     **
 **   Result:		int	TCL_OK		Operation successful	     **
 ** 									     **
 **   Attached Globals:	aliasfile, 	The alias command is writte out to   **
 **			alias_separator Defined the command separator	     **
 **			shell_derelict	to determine the shell family	     **
 **			shell_name	to determine the real shell type     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	output_unset_alias(	const char	*alias,
					const char	*val)
{
    int nobackslash = 1;		/** Controls wether backslashes are  **/
					/** to be print			     **/
    const char *cptr = val;	/** Need to read the value char by char      **/

    assert(shell_derelict != NULL);
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_output_unset_alias, NULL);
#endif

    /**
     **  Check for the shell family at first
     **  Ahh! CSHs ... ;-)
     **/
    if( !strcmp( shell_derelict, "csh")) {
        fprintf( aliasfile, "unalias %s%c", alias, alias_separator);

    /**
     **  Hmmm ... bourne shell types ;-(
     **  Need to unset a function in case of sh or if the alias took parameters
     **/
    } else if( !strcmp( shell_derelict, "sh")) {

        if( !strcmp( shell_name, "sh")) {
	    if (bourne_alias) {
		fprintf(aliasfile, "unalias %s%c", alias, alias_separator);
	    } else if (bourne_funcs) {
        	fprintf(aliasfile,"unset -f %s%c", alias, alias_separator);
	    } /* else do nothing */
	/**
	 **  BASH
	 **/
        } else if( !strcmp( shell_name, "bash")) {

            /**
             **  If we have what the old value should have been, then look to
             **  see if it was a function or an alias because bash spits out an
             **  error if you try to unalias a non-existent alias.
             **/
            if(val) {

                /**
                 **  Was it a function?
                 **  Yes, if it has arguments...
                 **/
		while( *cptr) {
		    if( *cptr == '\\') {
			if( nobackslash) {
			    nobackslash = 0;
			}
		    } else {
		if(*cptr == '$') {
			    if( nobackslash) {
				fprintf(aliasfile, "unset -f %s%c", alias,
				        alias_separator);
		    return TCL_OK;
			    }
			}
			nobackslash = 1;
		    }
		    cptr++;
		}

                /**
                 **  Well, it wasn't a function, so we'll put out an unalias...
                 **/
		    fprintf( aliasfile, "unalias %s%c", alias, alias_separator);

            } else {	/** No value known (any more?) **/

                /**
                 **  We'll assume it was a function because the unalias command
                 **  in bash produces an error.  It's possible that the alias
                 **  will not be cleared properly here because it was an
                 **  unset-alias command.
                 **/
                fprintf( aliasfile, "unset -f %s%c", alias, alias_separator);
            }

	/**
	 **  ZSH or KSH
	 **  Put out both because we it could be either a function or an
	 **  alias.  This will catch both.
	 **/

        } else if( !strcmp( shell_name, "zsh")){

            fprintf(aliasfile, "unalias %s%c", alias, alias_separator);

        } else if( !strcmp( shell_name, "ksh")) {

            fprintf(aliasfile, "unalias %s%c", alias, alias_separator);
            fprintf(aliasfile, "unset -f %s%c", alias, alias_separator);

        } /** if( bash, zsh, ksh) **/

	/** ??? Unknown derelict ??? **/

    } /** if( sh-family) **/
    
    return( TCL_OK);

} /** End of 'output_unset_alias' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		getLMFILES					     **
 ** 									     **
 **   Description:	Read in the _LMFILES_ environment variable. This one **
 **			may be split into several variables cause by limited **
 **			variable space of some shells (esp. the SUN csh)     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp    *interp	Attached Tcl interpreter     **
 ** 									     **
 **   Result:		char*	Value of the environment varibale _LMFILES_  **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char	*getLMFILES( Tcl_Interp	*interp)
{
    static char	*lmfiles = NULL;	/** Buffer pointer for the value     **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_getLMFILES, NULL);
#endif

    /**
     **  Try to read the variable _LMFILES_. If the according buffer pointer
     **  contains a value, disallocate it before.
     **/
    if( lmfiles)
        null_free((void *) &lmfiles);

    lmfiles = EMGetEnv( interp, "_LMFILES_");

    /**
     **  Now the pointer is NULL in case of the variable has not been defined.
     **  In this case try to read in the splitted variable from _LMFILES_xxx
     **/
    if( !lmfiles || !*lmfiles) {

        char	buffer[ MOD_BUFSIZE];	/** Used to set up the split variab- **/
					/** les name			     **/
        int	count = 0;		/** Split part count		     **/
        int	lmsize = 0;		/** Total size of _LMFILES_ content  **/
        int	old_lmsize;		/** Size save buffer		     **/
        int	cptr_len;		/** Size of the current split part   **/
        char	*cptr;			/** Split part read pointer	     **/

	/**
	 **  Set up the split part environment variable name and try to read it
	 **  in
	 **/
        sprintf( buffer, "_LMFILES_%03d", count++);
        cptr = EMGetEnv( interp, buffer);

	while(cptr && *cptr) {		/** Something available		     **/

	    /**
	     **  Count up the variables length
	     **/
            cptr_len = strlen( cptr);	
            old_lmsize = lmsize;
            lmsize += cptr_len;
 
	    /**
	     **  Reallocate the value's buffer and copy the current split
	     **  part at its end
	     **/
            if(!(lmfiles =
		(char*) module_realloc( lmfiles, lmsize * sizeof(char) + 1))) {
		    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
			return( NULL);		/** ---- EXIT (FAILURE) ---> **/
	    }
            
            strncpy( lmfiles + old_lmsize, cptr, cptr_len);
            *(lmfiles + old_lmsize + cptr_len) = '\0';
 
	    /**
	     **  Read the next split part variable
	     **/
            sprintf( buffer, "_LMFILES_%03d", count++);
            cptr = EMGetEnv( interp,buffer);
        }

    } else {  /** if( lmfiles) **/

	/**
	 **  If the environvariable _LMFILES_ has been set, copy the contents
	 **  of the returned buffer into a free allocated one in order to
	 **  avoid side effects.
	 **/
	char	*tmp = stringer(NULL,0, lmfiles, NULL);

	if( !tmp)
	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		return( NULL);		/** -------- EXIT (FAILURE) -------> **/

	/** 
	 **  Set up lmfiles pointing to the new buffer in order to be able to
	 **  disallocate when invoked next time.
	 **/
        lmfiles = tmp;

    } /** if( lmfiles) **/

    /**
     **  Return the received value to the caller
     **/
    return( lmfiles);

} /** end of 'getLMFILES' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		IsLoaded					     **
 ** 									     **
 **   Description:	Check whether the passed modulefile is currently     **
 **			loaded						     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp       *interp	According Tcl interp.**
 **			char             *modulename	Name of the module to**
 **							be searched for	     **
 **			char            **realname	Buffer for the name  **
 **							and version of the   **
 **							module that has mat- **
 **							ched the query	     **
 **			char             *filename	Buffer to store the  **
 **							whole filename of a  **
 **							found loaded module  **
 **									     **
 **   Result:		int	0	Requested module not loaded	     **
 **				1	module is loaded		     **
 **									     **
 **			realname	points to the name of the module that**
 **					has matched the query. If this poin- **
 **					differs form 'modulename' after this **
 **					function has finished, the buffer for**
 **					to store the module name in has been **
 **					allocated here.			     **
 **					if (char **) NULL is passed, no buf- **
 **					fer will be allocated		     **
 **				??? Is this freed correctly by the caller ???**
 **									     **
 **			filename	will be filled with the full module  **
 **					file path of the module that has     **
 **					matched the query		     **
 **									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

/**
 **  Check all possibilities of module-versions
 **/

int IsLoaded(	Tcl_Interp	 *interp,
		char		 *modulename,
		char		**realname,
		char		 *filename )
{
    return( __IsLoaded( interp, modulename, realname, filename, 0));
}

/**
 **  Check only an exact match of the passed module and version
 **/
int IsLoaded_ExactMatch(	Tcl_Interp	 *interp,
				char		 *modulename,
				char		**realname,
				char		 *filename )
{
    return( __IsLoaded( interp, modulename, realname, filename, 1));
}

/**
 **  The subroutine __IsLoaded finally checks for the requested module being
 **  loaded or not.
 **/
static int __IsLoaded(	Tcl_Interp	 *interp,
			char		 *modulename,
			char		**realname,
			char		 *filename,
			int		  exact)
{
    char *l_modules = NULL;		/** Internal module list buffer	     **/
    char *l_modulefiles = NULL;		/** Internal module file list buffer **/
    char *loaded = NULL;		/** Buffer for the module            **/
    char *basename = NULL;		/** Pointer to module basename       **/
    char *loadedmodule_path = NULL;	/** Pointer to one loaded module out **/
					/** of the loaded modules list	     **/
    int   count = 0;

    /**
     **  Get a list of loaded modules (environment variable 'LOADEDMODULES')
     **  and the list of loaded module-files (env. var. __LMFILES__)
     **/
    char	*loaded_modules = EMGetEnv( interp, "LOADEDMODULES");
    char	*loaded_modulefiles = getLMFILES( interp);
    
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc___IsLoaded, NULL);
#endif

    /**
     **  If no module is currently loaded ... the requested module is surely
     **  not loaded, too ;-)
     **/
    if( !loaded_modules || !*loaded_modules) {
	goto unwind0;
    }
    
    /**
     **  Copy the list of currently loaded modules into a new allocated array
     **  for further handling. If this fails it will be assumed, that the 
     **  module is *NOT* loaded.
     **/
    if((char *) NULL == (l_modules = stringer(NULL,0,loaded_modules,NULL)))
	if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	    goto unwind0;

    /**
     **  Copy the list of currently loaded modulefiles into a new allocated
     **  array for further handling. If this failes it will be assumed, that
     **  the module is *NOT* loaded.
     **/
    if(loaded_modulefiles)
	if((char *) NULL == (l_modulefiles = stringer(NULL,0,
		loaded_modulefiles,NULL)))
	    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		goto unwind1;

    /**
     **  Assume the modulename given was an exact match so there is no
     **  difference to return -- this will change in the case it wasn't an
     **  exact match below
     **/
    if( realname)
        *realname = modulename;

    if( *l_modules) {

	/**
	 **  Get each single module which is loaded by splitting up at colons
	 **  The variable LOADEDMODULES contains a list of modulefile like the
	 **  following:
	 **                gnu/2.0:openwin/3.0
	 **/
	loadedmodule_path = xstrtok( l_modules, ":");
	while( loadedmodule_path) {

	    if((char *) NULL == (loaded = stringer(NULL,0,
		    loadedmodule_path,NULL)))
		if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		    goto unwind2;

	    /**
	     **  Get a modulefile without a version and check if this is the
	     **  requested one.
	     **/
	    if( !strcmp( loaded, modulename)) {	/** FOUND    **/

		null_free ((void *) &loaded);
		break;			/** leave the while loop	     **/

	    } else if( !exact) {		/** NOT FOUND	     **/

		/**
		 **  Try to more and more simplify the modulename by removing
		 **  all detail (version) information
		 **/
		basename = get_module_basename( loaded);
		while( basename && strcmp( basename, modulename)) {
		    basename = get_module_basename( basename);
                }

		/**
		 **  Something left after splitting again? If yes the requested
		 **  module is found!
                 **  Since the name given was a basename, return the fully
		 **  loaded path
		 **/
                if( basename) {
		    null_free ((void *) &loaded);
                    if( realname)
			if((char *) NULL == (*realname = stringer(NULL,0,
				loadedmodule_path,NULL)))
			    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
				goto unwind2;

		    break;		/** leave the while loop	     **/

		} /** if( basename) **/
	    } /** if not found with single basename **/

	    /**
	     **  Get the next entry from the loaded modules list
	     **/
	    loadedmodule_path = xstrtok( NULL, ":");
            count++;

	    null_free ((void *) &loaded); /** Free what has been alloc. **/

	} /** while **/
    } /** if( *l_modules) **/

    /**
     **  If we found something locate it's associated modulefile
     **/
    if( loadedmodule_path) {
        if( filename && l_modulefiles && *l_modulefiles) {

	    /**
	     **  The position of the loaded module within the list of loaded
	     **  modules has been counted in 'count'. The position of the 
	     **  associated modulefile should be the same. So tokenize the
	     **  list of modulefiles by the colon until the wanted position
	     **  is reached.
	     **/
            char* modulefile_path = xstrtok(l_modulefiles, ":");
	
            while( count) {
                if( !( modulefile_path = xstrtok( NULL, ":"))) {

		    /**
		     **  Oops! Fewer entries in the list of loaded modulefiles
		     **  than in the list of loaded modules. This will
		     **  generally suggest that _LMFILES_ has become corrupted,
		     **  but it may just mean we're working intermittantly with
		     **  an old version.  So, I'll just not touch filename which
		     **  means the search will continue using the old method of
		     **  looking through MODULEPATH.  
                     */
		    goto success0;
                }
                count--;

            } /** while **/

	    /**
	     **  Copy the result into the buffer passed from the caller
	     **/
            strcpy( filename, modulefile_path);
        }

	/**
	 **  FOUND.
	 **  free up everything which has been allocated and return on success
	 **/
	goto success0;
    }

    /**
     **  NOT FOUND. Free up everything which has been alloc'd and return on
     **  failure
     **/

unwind2:
    if( l_modulefiles)
	null_free((void *) &l_modulefiles);
unwind1:
    null_free((void *) &l_modules);
unwind0:
    null_free((void *)&loaded_modules);
    return( 0);				/** -------- EXIT (FAILURE) -------> **/

success0:
    if( l_modulefiles)
	null_free((void *) &l_modulefiles);
    null_free((void *) &l_modules);
    return( 1);				/** -------- EXIT (SUCCESS) -------> **/

} /** End of '__IsLoaded' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		chk_marked_entry, set_marked_entry		     **
 ** 									     **
 **   Description:	When switching, the variables are marked with a mar- **
 **			ker that is tested to see if the variable was changed**
 **			in the second modulefile. If it was not, then the    **
 **			variable is unset.				     **
 ** 									     **
 **   First Edition:	1992/10/25					     **
 ** 									     **
 **   Parameters:	Tcl_HashTable   *table	Attached hash table	     **
 **			char            *var	According variable name	     **
 **			int              val	Value to be set.	     **
 **									     **
 **   Result:		int	0	Mark not set (or the value of the    **
 **					mark was 0 ;-)			     **
 **				else	Value of the mark that has been set  **
 **					with set_marked_entry.		     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

intptr_t chk_marked_entry(	Tcl_HashTable	*table,
				char		*var)
{
    Tcl_HashEntry 	*hentry;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_chk_marked_entry, NULL);
#endif

    if( hentry = Tcl_FindHashEntry( table, var))
        return((intptr_t) Tcl_GetHashValue( hentry));
    else
        return 0;
}

void set_marked_entry(	Tcl_HashTable	*table,
			char		*var,
			intptr_t 	 val)
{
    Tcl_HashEntry	*hentry;
    int    		 new;

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_set_marked_entry, NULL);
#endif

    if( hentry = Tcl_CreateHashEntry( table, var, &new)) {
        if( val)
            Tcl_SetHashValue( hentry, val);
    }

    /**  ??? Shouldn't there be an error return in case of hash creation
	     failing ??? **/
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		get_module_basename				     **
 ** 									     **
 **   Description:	Get the name of a module without its version.	     **
 **			This function modifies the string passed in.	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char	*modulename		Full module name     **
 ** 									     **
 **   Result:		char*		Module name without version	     **
 ** 									     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	char	*get_module_basename(	char	*modulename)
{
    char *version;			/** Used to locate the version sep.  **/
    
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_get_module_basename, NULL);
#endif

    /**
     **  Use strrchr to locate the very last version string on the module
     **  name.
     **/
    if((version = strrchr( modulename, '/'))) {
	*version = '\0';
    } else {
	modulename = NULL;
    }
 
    /**
     **  Return the *COPIED* string
     **/
    return( modulename);

} /** End of 'get_module_basename' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		Update_LoadedList				     **
 ** 									     **
 **   Description:	Add or remove the passed modulename and filename to/ **
 **			from LOADEDMODULES and _LMFILES_		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp      *interp		Attached Tcl Interp. **
 **			char            *modulename	Name of the module   **
 **			char            *filename	Full path name of the**
 **							related modulefile   **
 **									     **
 **   Result:		int	1	Successful operation		     **
 **									     **
 **   Attached Globals:	g_flags		Controls whether the modulename      **
 **					should be added (M_XXXX) or removed  **
 **					(M_REMOVE) from the list of loaded   **
 **					modules				     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int Update_LoadedList(	Tcl_Interp	*interp,
			char		*modulename,
			char		*filename)
{
    char 	*argv[4];
    char	*basename;
    char	*module;
 
#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_Update_LoadedList, NULL);
#endif

    /**
     **  Apply changes to LOADEDMODULES first
     **/
    argv[1] = "LOADEDMODULES";
    argv[2] = modulename;
    argv[3] = NULL;
    
    if(g_flags & M_REMOVE) {
	argv[0] = "remove-path";
	cmdRemovePath( 0, interp, 3, (CONST84 char **) argv);
    } else {
	argv[0] = "append-path";
	cmdSetPath( 0, interp, 3, (CONST84 char **) argv);
    }
 
    /**
     **  Apply changes to _LMFILES_ now
     **/
    argv[1] = "_LMFILES_";
    argv[2] = filename;
    argv[3] = NULL;

    if(g_flags & M_REMOVE) {
	argv[0] = "remove-path";
	cmdRemovePath( 0, interp, 3, (CONST84 char **) argv);
    } else {
	argv[0] = "append-path";
	cmdSetPath( 0, interp, 3, (CONST84 char **) argv);
    }

    /**
     **  A module with just the basename might have been added and now we're
     **  removing one of its versions. We'll want to look for the basename in
     **  the path too.
     **/
    if( g_flags & M_REMOVE) {
	module = stringer(NULL,0, modulename, NULL);
	basename = module;
	if( basename = get_module_basename( basename)) {
	argv[2] = basename;
	argv[0] = "remove-path";
	cmdRemovePath( 0, interp, 3, (CONST84 char **) argv);
	}
	null_free((void *) &module);
    }

    /**
     **  Return on success
     **/
    return( 1);

} /** End of 'Update_LoadedList' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		check_magic					     **
 ** 									     **
 **   Description:	Check the magic cookie of the file passed as para-   **
 **			meter if it is a valid module file		     **
 **			Based on check_magic in Richard Elling's	     **
 **			find_by_magic <Richard.Elling"@eng.auburn.edu>	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char   *filename	Name of the file to check    **
 **			char   *magic_name	Magic cookie		     **
 **			int     magic_len	Length of the magic cookie   **
 ** 									     **
 **   Result:		int	0	Magic cookie doesn't match or any    **
 **					I/O error			     **
 **				1	Success - Magic cookie has matched   **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int check_magic( char	*filename,
		 char	*magic_name,
		 int	 magic_len)
{
    int  fd;				/** File descriptor for reading in   **/
    int  read_len;			/** Number of bytes read	     **/
    char buf[BUFSIZ];			/** Read buffer			     **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_check_magic, NULL);
#endif

    /**
     **  Parameter check. The length of the magic cookie shouldn't exceed the
     **  length of out read buffer
     **/
    if( magic_len > BUFSIZ)
	return 0;

    /**
     **  Open the file and read in as many bytes as required for checking the
     **  magic cookie. If there's an I/O error (Unable to open the file or
     **  less than magic_len have been read) return on failure.
     **/
    if( 0 > (fd = open( filename, O_RDONLY)))
	if( OK != ErrorLogger( ERR_OPEN, LOC, filename, "reading", NULL))
	    return( 0);			/** -------- EXIT (FAILURE) -------> **/

    read_len = read( fd, buf, magic_len);
    
    if( 0 > close(fd))
	if( OK != ErrorLogger( ERR_CLOSE, LOC, filename, NULL))
	    return( 0);			/** -------- EXIT (FAILURE) -------> **/

    if( read_len < magic_len)
	return( 0);

    /**
     **  Check the magic cookie now
     **/
    return( !strncmp( buf, magic_name, magic_len));

} /** end of 'check_magic' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		regex_quote					     **
 ** 									     **
 **   Description:	Copy the passed path into the new path buffer and    **
 **			devalue '.' and '+'				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	const char   *path	Original path		     **
 **			      char   *newpath	Buffer for to copy the new   **
 **						path in			     **
 **			      int     len	Max length of the new path   **
 **									     **
 **   Result:		newpath		will be filled up with the new, de-  **
 **					valuated path			     **
 **									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void regex_quote( const char	*path,
		         char	*newpath,
			 int	 len)
{
    unsigned int path_len = strlen( path);	/** Length of the orig. path **/
    int 	 i,				/** Read index		     **/
    		 j;				/** Write index		     **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_cleanse_path, NULL);
#endif

    /**
     **  Stopping at (len - 1) ensures that the newpath string can be
     **  null-terminated below.
     **/
    for( i=0, j=0; i<path_len && j<(len - 1); i++, j++) {

        switch(*path) {
            case '.':
            case '+':
            case '$':
		*newpath++ = '\\';		/** devalue '.' and '+'	    **/
		j++;
		break;
        }

	/**
	 **  Flush the current character into the newpath buffer
	 **/
        *newpath++ = *path++;

    } /** for **/

    /**
     **  Put a string terminator at the newpaths end
     **/
    *newpath = '\0';

} /** End of 'cleanse_path' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		chop						     **
 ** 									     **
 **   Description:	Remove '\n' characters from the passed string	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char   *string	String to be chopped		     **
 **									     **
 **   Result:		string		The chopped string		     **
 **									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	char *chop( const char	*string)
{
    char	*s, *t;			/** source and target pointers       **/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_chop, NULL);
#endif

    /**
     **  Remove '\n'
     **/

    s = t = (char *) string;
    while( *s) {
	if( '\n' == *s)
	    s++;
	else
	    *t++ = *s++;
    }

    /**
     **  Copy the trailing terminator and return
     **/
    *t++ = '\0';
    return( (char *) string);

} /** End of 'chop' **/

#ifndef HAVE_STRDUP

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		strdup						     **
 ** 									     **
 **   Description:	Makes new space to put a copy of the given string    **
 **			into and then copies the string into the new space.  **
 **			Just like the "standard" strdup(3).		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:							     **
 **   Result:								     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char	*strdup( char *str)
{
    char* new;
    if ((char *) NULL) == (new = stringer(NULL,0, str, NULL))
	if( OK != ErrorLogger( ERR_STRING, LOC, filename, NULL))
	    return( (char*) NULL);	/** -------- EXIT (FAILURE) -------> **/
    return( new);			/** -------- EXIT (SUCCESS) -------> **/
}
#endif /* HAVE_STRDUP  */


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		xstrtok_r,xstrtok				     **
 ** 									     **
 **   Description: 	Considers the string s to consist of a sequence of   **
 **			zero or more text tokens separated by spans of one   **
 **			or more characters from the separator string  delim. **
 **			Just like the "standard" strtok(3), and the
 **			reentrant version strtok_r(3).			     **
 **			Except this tokenizer will return "empty" tokens too.**
 **			Providing our own strtok() eliminates the various    **
 **			nuances of different implementations.		     **
 **									     **
 **   First Edition:	2006/04/17	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:							     **
 **   Result:								     **
 **   Attached Globals:							     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char *xstrtok_r(char *s, const char *delim, char **ptrptr) {
	register char	*ptr, *lptr;
	register int	 c,sc;
	const char	*tok;

	/* return NULL if NULL string and at end of the line */
	if (s == NULL && *ptrptr == NULL)
		return (NULL);

	/* do not skip leading (or trailing) delimiters ... */

	/* if non-NULL string then at beginning of token parsing */
	if (s != NULL)	*ptrptr = s;

	ptr = *ptrptr;
	while (ptr && *ptr) {
		/* cycle through delimiters */
		tok = delim;
		while (tok && *tok) {
			if (*ptr == *tok) {	/* match */
				*ptr = '\0';	/* null terminate */
				lptr = *ptrptr;
				*ptrptr = ++ptr;	/* set for next time */
				return  lptr;
			}
			tok++;
		}
		ptr++;
	}
	/* didn't find delimiter */
	lptr = *ptrptr;
	*ptrptr = NULL;
	return lptr;
} /** End of 'xstrtok_r' **/

char *xstrtok(char *s, const char *delim) {
	static char	*last;
	return	xstrtok_r(s,delim,&last);
} /** End of 'xstrtok' **/

#if 0
void tryxstrtok (char *string, char *delim) {
	char *start, *str;
	char *token;
	int n = 1;

	start = str = strdup(string);
	printf("string: %s\n", str);
	printf("delim : %s\n", delim);
	
	token = xstrtok(str,delim);
	printf("\t%d = %s\n", n, token);
	while (token = xstrtok(NULL, delim)) {
		printf("\t%d = %s\n", ++n, token);
	}
	free(start);
}
int main () {
	tryxstrtok("abc:def;ghi,jkl", ":;,");
	tryxstrtok(":abc:def;ghi,jkl", ":;,");
	tryxstrtok("::abc:def;ghi,jkl", ":;,");
	tryxstrtok("abc:def;ghi,jkl:", ":;,");
	tryxstrtok("abc:def;ghi,jkl::", ":;,");
	tryxstrtok("abc", ":;,");
	tryxstrtok("", ":;,");
}
#endif

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		chk4spch					     **
 ** 									     **
 **   Description:	goes through the given string and changes any non-   **
 **			printable characters to question marks.		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char	*s		String to be checke	     **
 ** 									     **
 **   Result:		*s			Will be changed accordingly  **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void chk4spch(char* s)
{
    for( ; *s; s++)
	if( !isgraph( *s)) *s = '?';

} /** End of 'chk4spch' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		module_malloc					     **
 **   			module_realloc					     **
 ** 									     **
 **   Description:	A wrapper for the system malloc(),realloc() function **
 ** 			so the argument can be tested and set to a positive  **
 ** 			value.						     **
 ** 									     **
 **   First Edition:	2007/02/14	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	size_t	size		Number of bytes to allocate  **
 ** 									     **
 **   Result:		void    *		An allocated memory pointer  **
 ** 									     **
 ** ************************************************************************ **
 ++++*/


void *module_malloc(size_t size) {
	void *ret;

	if (size == 0)	size = 1;
#ifdef TCL_MEM_DEBUG
	ret = ckalloc(size);
#else
	ret = malloc(size);
#endif
	ret = memset(ret,'\0',size);

	return ret;
} /** End of 'module_malloc' **/

void *module_realloc(void * ptr, size_t size) {
	void *ret;

#ifdef TCL_MEM_DEBUG
	ret = ckrealloc(ptr, size);
#else
	ret = realloc(ptr, size);
#endif

	return ret;
} /** End of 'module_realloc' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		xdup						     **
 ** 									     **
 **   Description:	will return a string with 1 level of environment     **
 ** 			variables expanded. The limit is MOD_BUFSIZE.	     **
 ** 			An env.var. is denoted with either $name or ${name}  **
 **			\$ escapes the expansion and substitutes a '$' in    **
 **			its place.					     **
 ** 									     **
 **   First Edition:	2000/01/21	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	char	*string		Environment variable	     **
 ** 									     **
 **   Result:		char    *		An allocated string	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/


char *xdup(char const *string) {
	char *result = NULL;
	char *dollarptr;

	if (string == (char *)NULL) return result;

	/** need to work from copy of string **/
	if (((char *) NULL) == (result = stringer(NULL,0, string, NULL)))
	    if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
		return( (char*) NULL);	/** -------- EXIT (FAILURE) -------> **/

	/** check for '$' else just pass strdup of it **/
	if ((dollarptr = strchr(result, '$')) == (char *) NULL) {
		return result;
	} else {
	/** found something **/
		char const *envvar;
		char  buffer[MOD_BUFSIZE];
		char  oldbuffer[MOD_BUFSIZE];
		size_t blen = 0;	/** running buffer length	**/
		char *slashptr = result;/** where to continue parsing	**/
		char  slashchr;		/** store slash char		**/
		int   brace;		/** flag if ${name}		**/
		pid_t pid;		/** the process id		**/

		/** zero out buffers */
		memset(   buffer, '\0', MOD_BUFSIZE);
		memset(oldbuffer, '\0', MOD_BUFSIZE);

		/** copy everything upto $ into old buffer **/
		*dollarptr = '\0';
		strncpy(oldbuffer, slashptr, MOD_BUFSIZE);
		*dollarptr = '$';

		while (dollarptr) {
			if (*oldbuffer) strncpy(buffer, oldbuffer, MOD_BUFSIZE);
			blen = strlen(buffer);

			/** get the env.var. name **/
			if (*(dollarptr + 1) == '{') {
				brace = 1;
				slashptr = strchr(dollarptr + 1, '}');
			} else if (*(dollarptr + 1) == '$') {
				slashptr = dollarptr + 2;
			} else {
				slashptr = dollarptr + 1
					+ strcspn(dollarptr + 1,"/:$\\");
				brace = 0;
			}
			if (*slashptr) {
				slashchr = *slashptr;
				*slashptr = '\0';
			} else slashptr = (char *)NULL;

			/** see if escaped **/
			if ((result != dollarptr) && *(dollarptr - 1) == '\\') {
				/** replace \ with 0 and copy rest of name **/
				buffer[blen - 1] = '\0';
				strncat(buffer, dollarptr, MOD_BUFSIZE-blen);
				blen = strlen(buffer);
				if(brace)
					strncat(buffer,"}",MOD_BUFSIZE-blen-1);
			} else {
				if (! strcmp(dollarptr + 1 + brace, "$")) {
					/** put in the process pid **/
					pid = getpid();
					sprintf(buffer + blen,"%ld",(long)pid);
				} else {
					/** get env.var. value **/
					envvar = getenv(dollarptr + 1 + brace);

					/** cat value to rest of string **/
					if (envvar) strncat(buffer,envvar,
						MOD_BUFSIZE-blen-1);
				}
			}
			blen = strlen(buffer);

			/** start at slashptr and find next $ **/
			if (slashptr) {
				*slashptr = slashchr;
				dollarptr = strchr(slashptr, '$');
				/** copy everything upto $ **/
				if (dollarptr) *dollarptr = '\0';
				strncat(buffer, slashptr + brace,
					MOD_BUFSIZE -blen -1);
				if (dollarptr) {
					*dollarptr = '$';
					strncpy(oldbuffer, buffer, MOD_BUFSIZE);
				}
			} else {		/** no more to show **/
				dollarptr = (char *)NULL;
			}
		}
		null_free((void *) &result);
		return stringer(NULL,0, buffer, NULL);
	}

} /** End of 'xdup' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		xgetenv						     **
 ** 									     **
 **   Description:	will return an expanded environment variable.	     **
 ** 			However, it will only expand 1 level.		     **
 ** 			See xdup() for details.				     **
 ** 									     **
 **   First Edition:	2000/01/18	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	char	*var		Environment variable	     **
 ** 									     **
 **   Result:		char    *		An allocated string	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char *xgetenv(char const * var) {
	char *result = NULL;

	if (var == (char *)NULL) return result;

	return xdup(getenv(var));

} /** End of 'xgetenv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		EscapeCshString(char* in,char* out)                  **
 ** 									     **
 **   Description:	will translate input string to escaped output string **
 **                     out must be allocated first                          **
 ** 									     **
 **   First Edition:	2002/04/10					     **
 ** 									     **
 **   Parameters:	char	*in		input            	     **
 **              	char	*out		output               	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void EscapeCshString(const char* in,
		     char* out) {
  
  for(;*in;in++) {
    if (*in == ' ' ||
	*in == '\t'||
	*in == '\\'||
	*in == '{' ||
	*in == '}' ||
	*in == '|' ||
	*in == '<' ||
	*in == '>' ||
	*in == '!' ||
	*in == ';' ||
	*in == '#' ||
	*in == '$' ||
	*in == '^' ||
	*in == '&' ||
	*in == '*' ||
	*in == '\''||
	*in == '"' ||
	*in == '(' ||
	*in == ')') {
      *out++ = '\\';
    }
    *out++ = *in;
  }
  *out = 0;
}

void EscapeShString(const char* in,
		     char* out) {
  
  for(;*in;in++) {
    if (*in == ' ' ||
	*in == '\t'||
	*in == '\\'||
	*in == '{' ||
	*in == '}' ||
	*in == '|' ||
	*in == '<' ||
	*in == '>' ||
	*in == '!' ||
	*in == ';' ||
	*in == '#' ||
	*in == '$' ||
	*in == '^' ||
	*in == '&' ||
	*in == '*' ||
	*in == '\''||
	*in == '"' ||
	*in == '(' ||
	*in == ')') {
      *out++ = '\\';
    }
    *out++ = *in;
  }
  *out = 0;
}

void EscapePerlString(const char* in,
		     char* out) {
  
  for(;*in;in++) {
    if (*in == '\\'||
	*in == ';' ||
	*in == '\'') {
      *out++ = '\\';
    }
    *out++ = *in;
  }
  *out = 0;
}

/* I think this needs a bunch of work --NJW */
void EscapeCmakeString(const char* in,
		     char* out) {
  for(;*in;in++) {
    if (*in == '\\'||
	*in == '"') {
      *out++ = '\\';
    }
    *out++ = *in;
  }
  *out = 0;
}

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		tmpfile_mod                                          **
 ** 									     **
 **   Description:	emulates tempnam  and tmpnam  and mktemp             **
 **                     Atomically creates a unique temp file and opens it   **
 **                     for writing. returns 0 on success, 1 on failure      **
 **                     Filename and file handle are returned through        **
 **                     argument pointers                                    **
 ** 									     **
 **   First Edition:	2002/04/22					     **
 ** 									     **
 **   Parameters:	char	**filename	pointer to char* 	     **
 **              	char	**file          pointer to FILE*     	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int tmpfile_mod(char** filename, FILE** file) {
  char* filename2;
  FILE* f = NULL;
  int trial = 0;

  if ((char *) NULL == (filename2 =
	 stringer(NULL, strlen(TMP_DIR)+strlen("modulesource")+20, NULL)))
     if( OK != ErrorLogger( ERR_STRING, LOC, NULL))
	 return 1;
  
  do {
    int fildes;

    sprintf(filename2,"%s/modulesource_%d",TMP_DIR,trial++);
    fildes = open(filename2,O_WRONLY | O_CREAT | O_EXCL | O_TRUNC,0755);
#if 0
    fprintf(stderr,"DEBUG: filename=%s fildes=%d\n",
	   filename2,fildes);
#endif
    if (fildes >=0) {
      *file = fdopen(fildes,"w");
      *filename = filename2;
      return 0;
    }
  } while (trial < 1000);

  null_free((void *) &filename2);
  fprintf(stderr,
	"FATAL: could not get a temp file! at %s(%d)",__FILE__,__LINE__);
  
  return 1;
}


/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		stringer					     **
 ** 									     **
 **   Description:	Safely copies and concats series of strings	     **
 **			until it hits a NULL argument.			     **
 **			Either a buffer & length are given or if the buffer  **
 **			pointer is NULL then it will allocate memory to the  **
 **			given length. If the length is 0 then get the length **
 **			from the series of strings.			     **
 **			The resultant buffer is returned unless there	     **
 **			is an error then NULL is returned.		     **
 **			(Therefore, one of the main uses of stringer is to   **
 **			 allocate string memory.)			     **
 ** 									     **
 ** 									     **
 **   First Edition:	2001/08/08	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	char		*buffer	string buffer (if not NULL)  **
 **			int		 len	maximum length of buffer     **
 **			const char	*str1	1st string to copy to buffer **
 **			const char	*str2	2nd string to cat  to buffer **
 ** 			...						     **
 **			const char	*strN	Nth string to cat  to buffer **
 **			const char	*NULL	end of arguments	     **
 ** 									     **
 **   Result:		char		*buffer	if successful completion    **
 ** 					else NULL			     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

char *stringer(	char *		buffer,
		int		len,
		... )
{
	va_list	 argptr;	/** stdarg argument ptr			**/
	char	*ptr;		/** argument string ptr			**/
	char	*tbuf = buffer;	/** tempory buffer  ptr			**/
	int	 sumlen = 0;	/** length of all the concat strings	**/
	char	*(*strfn)(char*,const char*) = strcpy;
				/** ptr to 1st string function		**/

#if WITH_DEBUGGING_UTIL_2
    ErrorLogger( NO_ERR_START, LOC, _proc_stringer, NULL);
#endif

	/* get start of optional arguments and sum string lengths */
	va_start(argptr, len);
	while ((ptr = va_arg(argptr, char *))) {
		sumlen += strlen(ptr);
	}
	va_end(argptr);

	/* can we even proceed? */
	if (tbuf && (sumlen >= len || len < 0)) {
		return (char *) NULL;
	}

	/* do we need to allocate memory? */
	if (tbuf == (char *) NULL) {
		if (len == 0) {
			len = sumlen + 1;
		}
		if ((char *) NULL == (tbuf = (char*) module_malloc(len))) {
			if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
				return (char *) NULL;
		}
	}

	/* concat all the strings to buffer */
	va_start(argptr, len);
	while ((ptr = va_arg(argptr, char *))) {
		strfn(tbuf, ptr);
		strfn = strcat;
	}
	va_end(argptr);

	/* got here successfully - return buffer */
	return tbuf;

} /** End of 'stringer' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		null_free					     **
 ** 									     **
 **   Description:	does a free and then nulls the pointer.		     **
 ** 									     **
 **   first edition:	2000/08/24	r.k.owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   parameters:	void	**var		allocated memory	     **
 ** 									     **
 **   result:		void    		(nothing)		     **
 ** 									     **
 **   attached globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

void null_free(void ** var) {

	if (! *var) return;	/* passed in a NULL ptr */

#ifdef USE_FREE
#  ifdef TCL_MEM_DEBUG
	ckfree( *var);
#  else
	free( *var);
#  endif
#endif
	*var = NULL;

} /** End of 'null_free' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		countTclHash					     **
 ** 									     **
 **   Description:	returns the number of hash entries in a TclHash	     **
 ** 									     **
 **   first edition:	2005/09/01	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	Tcl_HashTable	*table	Hash to count		     **
 ** 									     **
 **   Result:		size_t			Count of Hash Entries	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/


size_t countTclHash(Tcl_HashTable *table) {
	size_t result = 0;
	Tcl_HashSearch	 searchPtr;	/** Tcl hash search handle	     **/

	if(Tcl_FirstHashEntry(table, &searchPtr)) {

	    do {
		result++;
	    } while(Tcl_NextHashEntry( &searchPtr));

	} /** if **/

	return result;
} /** End of 'countHashTable' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		ReturnValue					     **
 ** 									     **
 **   Description:	Handles the various possible return values	     **
 ** 									     **
 **   first edition:	2006/02/13	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	Tcl_Interp      *interp		Attached Tcl Interp. **
 **			int		retval		Return value to check**
 ** 									     **
 **   Result:		EM_RetVal		Limited set		     **
 ** 									     **
 **   Attached Globals:	g_retval	set to N if "exit N"		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/


EM_RetVal ReturnValue(Tcl_Interp *interp, int retval) {
	EM_RetVal	 em_result;
	char		*startp		= (char *) NULL,
			*endp		= (char *) NULL;
	const char 	*tstr;
	int		 result;
	static Tcl_RegExp	exit__expPtr,
				break_expPtr,
				continue_expPtr;

	tstr = (const char *) TCL_RESULT(interp);

	/* compile regular expression the first time through */
	if (!exit__expPtr)
		exit__expPtr = Tcl_RegExpCompile(interp, "^EXIT ([0-9]*)");

	/*  result = "invoked \"break\" outside of a loop" */
	if (!break_expPtr)
		break_expPtr = Tcl_RegExpCompile(interp, ".*\"break\".*");

	/*  result = "invoked \"continue\" outside of a loop" */
	if (!continue_expPtr)
		continue_expPtr = Tcl_RegExpCompile(interp, ".*\"continue\".*");

	/* intercept any "EXIT N" first */
	if(tstr && *tstr && 0 < Tcl_RegExpExec(interp, exit__expPtr,
		(CONST84 char *) tstr, (CONST84 char *) tstr)){
		/* found 'EXIT' */
		Tcl_RegExpRange(exit__expPtr, 1,
			(CONST84 char **) &startp, (CONST84 char **) &endp);
		if( startp != '\0')
			result = atoi((const char *) startp);

		g_retval = result;
		em_result = EM_EXIT;

	/* check for a break not within loop */
	} else if(tstr && *tstr && 0 < Tcl_RegExpExec(interp, break_expPtr,
		(CONST84 char *) tstr, (CONST84 char *) tstr)){
		em_result = EM_BREAK;

	/* check for a continue not within loop */
	} else if(tstr && *tstr && 0 < Tcl_RegExpExec(interp, continue_expPtr,
		(CONST84 char *) tstr, (CONST84 char *) tstr)){
		em_result = EM_CONTINUE;

	} else {
		switch (retval) {
		case TCL_OK:
			em_result = EM_OK;
			break;
		case TCL_BREAK:
			em_result = EM_BREAK;
			break;
		case TCL_CONTINUE:
			em_result = EM_CONTINUE;
			break;
		case TCL_ERROR:
		default:
			em_result = EM_ERROR;
			break;
		}
	}
	return em_result;
} /** End of 'ReturnValue' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		OutputExit					     **
 ** 									     **
 **   Description:	Outputs a 'test 0 = 1' line so command will eval     **
 ** 			with a non-zero	exit code			     **
 ** 									     **
 **   first edition:	2006/03/07	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	void			none			     **
 ** 									     **
 **   result:		void    		(nothing)		     **
 ** 									     **
 **   Attached Globals:	g_retval	if non-zero			     **
 **   			g_output	if non-zero			     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
void OutputExit() {

	if (shell_derelict == NULL) {
		return;
	} else if( !strcmp( shell_derelict, "csh")) {
		/* OK shell derelict */
	} else if( !strcmp( shell_derelict, "sh")) {
		/* OK shell derelict */
	} else {
		return;
	}
	if (g_retval) {
		fprintf( stdout, " test 0 = 1;");
	}
} /** End of 'OutputExit' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		EMGetEnv					     **
 ** 									     **
 **   Description:	Wrap the Tcl_GetVar2() call and return an allocated  **
 ** 			string						     **
 ** 									     **
 **   first edition:	2011/08/15	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	Tcl_Interp	 *interp	 TCL interp.	     **
 **   			char	*var		Environment variable	     **
 ** 									     **
 **   Result:		char    *		An allocated string	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
char * EMGetEnv(	Tcl_Interp	 *interp, 
			char const	 *var) {

	char *value, *string;

	Tcl_Preserve(interp);
	value = (char *) Tcl_GetVar2( interp, "env", var, TCL_GLOBAL_ONLY);
	Tcl_Release(interp);
	string = stringer(NULL, 0, value, NULL);

	if(!string)
		if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
			return (NULL);		/** ---- EXIT (FAILURE) ---> **/

	return string;

} /** End of 'EMGetEnv' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		EMSetEnv					     **
 ** 									     **
 **   Description:	Wrap the Tcl_SetVar2() call and return an allocated  **
 ** 			string						     **
 ** 									     **
 **   first edition:	2011/09/26	R.K.Owen <rk@owen.sj.ca.us>	     **
 ** 									     **
 **   Parameters:	Tcl_Interp	 *interp	 TCL interp.	     **
 **   			char	*var		Environment variable	     **
 **   			char	*val		New value		     **
 ** 									     **
 **   Result:		char    *		current value string	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/
char * EMSetEnv(	Tcl_Interp	 *interp, 
			char const	 *var,
			char const	 *val) {

	char *value;

	Tcl_Preserve(interp);
	value = (char *) Tcl_SetVar2( interp, "env", var, val, TCL_GLOBAL_ONLY);
	Tcl_Release(interp);

	return value;

} /** End of 'EMSetEnv' **/
