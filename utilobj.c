/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		utilobj.c					     **
 **   First Edition:	2009/08/20					     **
 ** 									     **
 **   Authors:	R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:	General Tcl_Obj related routines		     **
 **			which are not necessarily specific to any single     **
 **			block of functionality.				     **
 ** 									     **
 **   Exports:		Tcl_ArgvToObjv					     **
 **			Tcl_ObjvToArgv					     **
 **			mhash_*	(see below)				     **
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

static char Id[] = "@(#)$Id: utilobj.c,v 1.5 2009/09/02 20:37:39 rkowen Exp $";
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
 **   Parameters:	int		*objc		objv element count   **
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
	int *objc,
	Tcl_Obj *** objv,
	int argc,
	char * const *argv
) {
	char * const *aptr = argv;
	Tcl_Obj       **optr;

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
 **   Parameters:	int		*argc		argv element count   **
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
	int *argc,
	char ***argv,
	int objc,
	Tcl_Obj * CONST84 * objv
) {
	Tcl_Obj * CONST84 * optr = objv;
	char    	**aptr;

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

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		mhash_*						     **
 ** 									     **
 **   Description:	A collection of functions for managing a hash	     **
 **			array of objects with the explicit assumption	     **
 **			that the keys are strings.			     **
 ** 									     **
 **   First Edition:	2009/08/28					     **
 ** 									     **
 **   Parameters:	MHashType	  type		of hash contents     **
 **			MHash		 *mh		MHash object ptr     **
 **			MHash		**mhp		MHash object address **
 **			char		*key		hash element key     **
 ** 									     **
 **   Exports:								     **
 **	mhash_ctor		constructs MHash object			     **
 **	mhash_dtor		destructs  Mhash object			     **
 **	mhash_copy		constructs a copy of a Mhash object	     **
 **	mhash_add		add    a key/value pair			     **
 **	mhash_del		delete a key/value pair			     **
 **	mhash_type		type of MHash				     **
 **	mhash_number		number of key/value pairs		     **
 **	mhash_keys_uvec		return uvec object of keys		     **
 **	mhash_keys		return vector of keys			     **
 **	mhash_value		return value for given key		     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

/* MHash data allocation functions for the various types
 * internal use only
 */
/* type: MHashInt */

static int	mhash_int_add(void	**data, va_list ap) {
	intptr_t num = va_arg(ap,intptr_t);
	if (!data) return -1;		/* NULL ptr */
	if ((char *) *data) return 1;		/* already allocated */
	*data = (void *) num;
	return 0;
}

static int	mhash_int_del(void	**data, va_list ap) {
	*data = (void *) NULL;
	return 0;
}

/* type: MHashStrings */

static int	mhash_strings_add(void	**data, va_list ap) {
	char const *str = va_arg(ap,char *);
	if (!data) return -1;		/* NULL ptr */
	if ((char *) *data) return 1;		/* already allocated */
	if ((*data = (void *) stringer(NULL,0, str, NULL)))
		return 0;
	return -2;
}

static int	mhash_strings_del(void	**data, va_list ap) {
	null_free(data);
	return 0;
}

/* -------------------------------------------------------------------------- */
/* mhash_ctor: MHash constructor function
 */
MHash *mhash_ctor(MHashType type) {
	MHash *mhp = (MHash *) NULL;

	/* allocate the hash structure */
	if (!(mhp = (MHash *) module_malloc(sizeof(MHash))))
		if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
			return (MHash *) NULL; 	/** ---- EXIT (FAILURE) ---> **/

	/* set some struct elements */
	mhp->type = type;
	if (!(mhp->hash =(Tcl_HashTable *)module_malloc(sizeof(Tcl_HashTable))))
		if (OK != ErrorLogger(ERR_ALLOC, LOC, NULL))
			return (MHash *) NULL; 	/** ---- EXIT (FAILURE) ---> **/
	Tcl_InitHashTable(mhp->hash,TCL_STRING_KEYS);

	if (!(mhp->keys = uvec_ctor(5)))
		if (OK != ErrorLogger(ERR_UVEC, LOC, NULL))
			return (MHash *) NULL; 	/** ---- EXIT (FAILURE) ---> **/

	/* set the data add/del fns */
	switch (type) {
	case MHashStrings:
		mhp->add = mhash_strings_add;
		mhp->del = mhash_strings_del;
		break;
	case MHashInt:
		mhp->add = mhash_int_add;
		mhp->del = mhash_int_del;
		break;
	default:
		if (OK != ErrorLogger(ERR_MHASH, LOC, NULL))
			return (MHash *) NULL; 	/** ---- EXIT (FAILURE) ---> **/
	}

	return mhp;
}

/* -------------------------------------------------------------------------- */
/* mhash_dtor: MHash destructor function
 */
int mhash_dtor(MHash **mhp) {
	MHash *mh = *mhp;
	char **kvec;

	if (!mhp || !mh)		/* nothing to do */
		return TCL_OK;

	/* remove data elements */
	kvec = uvec_vector(mh->keys);
	while (kvec && *kvec) {
		if (TCL_OK != mhash_del_(mh,*kvec))
			if (OK != ErrorLogger(ERR_MHASH, LOC, NULL))
				return TCL_ERROR;/** --- EXIT (FAILURE) ---> **/
		kvec++;
	}
	/* dealloc struct elements */
	if (uvec_dtor(&(mh->keys)))
		if (OK != ErrorLogger(ERR_UVEC, LOC, NULL))
			return TCL_ERROR; 	/** ---- EXIT (FAILURE) ---> **/

	Tcl_DeleteHashTable(mh->hash);
	null_free((void *) &(mh->hash));

	/* dealloc struct */
	null_free((void *) mhp);

	return TCL_OK;
}

/* -------------------------------------------------------------------------- */
/* mhash_copy: MHash copy constructor function
 */
MHash *mhash_copy(MHash *mh) {
	MHash *mhc = (MHash *) NULL;
	char **keys = mhash_keys(mh);

	/* construct a new MHash object */
	if (!(mhc = mhash_ctor(mh->type)))
		goto unwind0;

	while (keys && *keys) {
		if (TCL_OK != mhash_add(mhc, *keys, mhash_value(mh, *keys)))
			goto unwind1;
		keys++;
	}

	return mhc;

unwind1:
	mhash_dtor(&mhc);
unwind0:
	return (MHash *) NULL;
}

/* -------------------------------------------------------------------------- */
/* mhash_del_ - delete only the data element */
int mhash_del_(MHash *mh, const char *key, ...) {
	va_list vargs;
	Tcl_HashEntry *hep = (Tcl_HashEntry *) NULL;
	void *tmp = (void *) NULL;

	if (!mh && !*key)
		if (OK != ErrorLogger(ERR_MHASH, LOC, NULL))
			return TCL_ERROR; 	/** ---- EXIT (FAILURE) ---> **/

	/* get variable arg pointer */
	va_start(vargs, key);

	/* find and delete element */
	if ((hep = Tcl_FindHashEntry(mh->hash, (char *) key))) {
		tmp = (void *) Tcl_GetHashValue(hep);
		if (tmp)
			(mh->del)(&tmp, vargs);
		Tcl_DeleteHashEntry(hep);
	}

	return TCL_OK;
}

/* -------------------------------------------------------------------------- */
/* mhash_del - delete data element and vector element */
int mhash_del(MHash *mh, const char *key, ...) {
	va_list vargs;
	int	in;

	/* get variable arg pointer */
	va_start(vargs, key);

	if ((in = uvec_find(mh->keys, key, UVEC_ASCEND)) < -1)
		if (OK != ErrorLogger(ERR_UVEC, LOC, NULL))
			return TCL_ERROR; 	/** ---- EXIT (FAILURE) ---> **/
	if (in == -1)	/* not found */
			return TCL_OK;

	if ((in = uvec_delete(mh->keys, in)) < 0)
		if (OK != ErrorLogger(ERR_UVEC, LOC, NULL))
			return TCL_ERROR; 	/** ---- EXIT (FAILURE) ---> **/

	return mhash_del_(mh, key);
}

/* -------------------------------------------------------------------------- */
/* mhash_add - add data element */
int mhash_add(MHash *mh, const char *key, ...) {
	va_list vargs;
	Tcl_HashEntry *hep = (Tcl_HashEntry *) NULL;
	void *tmp = (void *) NULL;
	int new;

	/* see if exists already - don't add to vector */
	if (uvec_find(mh->keys, key, UVEC_ASCEND) < 0) {
		if (uvec_add(mh->keys, key) < 0)
			if (OK != ErrorLogger(ERR_UVEC, LOC, NULL))
				return TCL_ERROR;/** --- EXIT (FAILURE) ---> **/
	}

	/* get variable arg pointer */
	va_start(vargs, key);

	/* change current element */
	hep = Tcl_CreateHashEntry(mh->hash, key, &new);
	if (!new) {
		tmp = (void *) Tcl_GetHashValue(hep);
		if (tmp)
			(mh->del)(&tmp, vargs);
	}

	/* add element */
	if ((mh->add)(&tmp, vargs) < 0)
		if (OK != ErrorLogger(ERR_MHASH, LOC, NULL))
			return TCL_ERROR;/** --- EXIT (FAILURE) ---> **/
	Tcl_SetHashValue(hep, (char *) tmp);

	return TCL_OK;
}

/* -------------------------------------------------------------------------- */
/*
 * accessor methods
 */
/* mhash_type - return the type of MHash this is */
MHashType mhash_type(MHash *mh) {
	return mh->type;
}

/* mhash_number - return the number of key/value pairs */
int mhash_number(MHash *mh) {
	return uvec_number(mh->keys);
}

/* mhash_keys_uvec - return a pointer to internal uvec  */
uvec *mhash_keys_uvec(MHash *mh) {
	return mh->keys;
}

/* mhash_keys - return a pointer to internal uvec vector  */
char **mhash_keys(MHash *mh) {
	return uvec_vector(mh->keys);
}

void *mhash_value(MHash *mh, const char *key) {
	Tcl_HashEntry	*he;		/** result from Tcl hash search	     **/
	
	if(!(he = Tcl_FindHashEntry(mh->hash, key)))
		return (void *) NULL;

	return Tcl_GetHashValue(he);
}
