static const char RCSID[]="@(#)$Id: avec_.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
static const char AUTHOR[]="@(#)avec 1.0 2002/02/08 R.K.Owen,Ph.D.";
/* avec.c -
 * This could have easily been made a C++ class, but is
 * confined to C so that it can be maximally portable.
 *
 * This is an associative vector package (a hash array)
 * that uses quadratic hashing.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2002 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <stdlib.h>
#include "config.h"
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif
#include "avec.h"
#include "avec_.h"
#include "iprime.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* make these as macros so they can be "inlined" when used */
/* must use the SETUP macro to define the need variables
 *  ... don't use _i,_s,_t otherwise.
 */
#define SETUP	\
unsigned int _i; \
char const *_s, *_t

#define HASH(ss,v,Size) \
	{for(_s=ss,_i=0; *_s; _s++) _i=131 * _i + *_s;v=_i % Size;}

#define STRCMP(ss,tt,v) {if (*ss) v=0;else v=1; _s=ss; _t=tt; while(*_s & *_t){\
		if(*_s==*_t){_s++;_t++;} else {v = 1; break;}}}


/* global variables */

char avec_TAG[5] = "AVEC";
/* place holder for removed entries */
const avec_element REMOVED = {"", NULL};

/* ---------------------------------------------------------------------- */
/* - internal routines only                                             - */
/* ---------------------------------------------------------------------- */
/* avec_hash_cap - returns the next largest prime to the given value
 */
int avec_hash_cap(int cap) {
/* next larger prime */
	while (iprime(cap) != 1) ++cap;
	return cap;
}

/* avec_do_resize - returns 1 if a avec_resize should be done,
 * else 0;
 */
int avec_do_resize(avec const *av) {
	if (av->percentage < 0) return 0;
	if (av->percentage * av->capacity < 100 * av->number) return 1;
	return 0;
}


/* avec_alloc_hash - does the memory allocation of the hash array
 * returns NULL if an error
 */
avec_element **avec_alloc_hash(int cap) {
	avec_element **hash = (avec_element **) NULL;

	if (cap < 0) return hash;

	if (!(hash = (avec_element **)
		calloc(cap, sizeof(avec_element)))) {
#ifdef RKOERROR
		(void) rkocpyerror("avec_alloc_hash : malloc error!");
		rkoerrno = RKOMEMERR;
#endif
		return hash;
	}
	return hash;
}

/* avec_dealloc_element - deallocates the memory for an element and returns 0
 * if successful, else < 0;
 */
int avec_dealloc_element(avec_element **element) {
	if ((! element) || (! *element)) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		rkocpyerror("avec_dealloc_element : already NULL");
#endif
		return -1;
	}
	if ((*element)->key) {
		free((void *)(*element)->key);
		(*element)->key = '\0';
	}
	free((void *) *element);
	*element = (avec_element *) NULL;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

/* avec_alloc_element - allocates memory for an element and returns 0
 * if successful, else < 0;
 */
int avec_alloc_element(char const *key, avec_element **element) {
	int retval = 0;
/* check if key is OK */
	if ((!key) || (!*key)) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		rkocpyerror("avec_alloc_element : NULL key");
#endif
		retval = -1;
		goto unwind1;
	}
/* allocate an element array */
	if (!(*element = (avec_element *) calloc(sizeof(avec_element), 1))) {
#ifdef RKOERROR
		rkoerrno = RKOMEMERR;
		rkocpyerror("avec_alloc_element : element memory error");
#endif
		retval = -2;
		goto unwind1;
	}
/* allocate the key string space */
	if (!((*element)->key = malloc(strlen(key)+1))) {
#ifdef RKOERROR
		rkoerrno = RKOMEMERR;
		rkocpyerror("avec_alloc_element : key memory error");
#endif
		retval = -3;
		goto unwind2;
	}
/* copy key over */
	strcpy((char *)(*element)->key, key);
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return retval;
unwind2:
	free(element);
unwind1:
	return retval;
}

/* avec_hash_search - finds and returns the address in the av->hash
 * if successful, else returns NULL
 * type=AVEC_MATCH  - return the matching hash element else NULL
 * type=AVEC_NEXT   - return the next empty hash element else NULL
 * type=AVEC_INSERT - return the matching or empty hash element else NULL
 */
avec_element **avec_hash_search(enum avec_search type,
		avec *av, char const *key) {
	avec_element **retval = (avec_element **) NULL;
	SETUP;
	unsigned int	hv,		/* hash value */
			inc = 0,	/* increment for quadratic hashing */
			tv;		/* boolean test result */
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!avec_exists(av)) {
#ifdef RKOERROR
		(void) rkopsterror("avec_hash_search : ");
#endif
		return retval;
	}
/* check if key is OK */
	if ((!key) || (!*key)) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		rkocpyerror("avec_hash_search : NULL key");
#endif
		return retval;
	}

/* get the HASH of the key */
	HASH(key,hv, av->capacity);
/* start search */
	while (inc < av->capacity && av->hash[hv]) {
	/* there is an entry here ... see if it matches */

		/* this entry has been removed before ... return if OK */
		if ((av->hash[hv] == &REMOVED) && (type == AVEC_NEXT))
			return &(av->hash[hv]);

		STRCMP(av->hash[hv]->key, key, tv);

		if (!tv) {	/* found match */
			if (type == AVEC_MATCH || type == AVEC_INSERT)
				return &(av->hash[hv]);
		}
		hv += ++inc;
		hv %= av->capacity;
		inc++;
	}
	/* found nothing but this empty slot */
	if (type == AVEC_NEXT || type == AVEC_INSERT)
		return &(av->hash[hv]);

	/* either no match or no room left */
	return retval;
}

/* ---------------------------------------------------------------------- */
/* avec_data_delrm - delete or remove an element of the key and returns 0
 * if successful
 * else returns < 0 if an error
 * and only AVEC_DELETE should ever return > 0 to signify the key is not gone
 */
int avec_data_delrm(enum avec_delrm type,
	avec *av, char const *key, va_list vargs) {

	int retval;
	avec_element	**elem;		/* place to data_delrm value */

	if (!(elem = avec_hash_search(AVEC_MATCH, av,key))) {
#ifdef RKOERROR
		rkopsterror("avec_data_delrm : ");
#endif
		return -1;
	}
	if (type == AVEC_DELETE) {
		retval = (av->fns.data_del)(&((*elem)->data),vargs);
	} else {
		retval = (av->fns.data_rm)(&((*elem)->data),vargs);
	}
	if (retval) {
#ifdef RKOERROR
		if (retval < 0) {
			if (! rkostrerror() ) {
				rkocpyerror("unspecified user data_del error");
				rkoerrno = RKOUSEERR;
			}
			rkopsterror("avec_data_delrm : ");
		}
#endif
		return retval;
	}
	(*elem)->data = (char *) NULL;
	(void) avec_dealloc_element(elem);
	*elem = (avec_element *) &REMOVED;
	av->number--;

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

