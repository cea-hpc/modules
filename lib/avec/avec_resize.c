static const char RCSID[]="@(#)$Id: avec_resize.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "avec.h"
#include "avec_.h"
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_resize - expands the hash array to the new capacity.
 * If the capacity is given as 0, then will automatically set it about
 * twice the current capacity.
 * Returns 0 if successful else returns < 0 if an error
 */
int avec_resize(avec *av, int newcap) {
	int		  oldcap;	/* old capacity */
	avec_element	**hash,		/* hash to store elements */
			**ptrptr,	/* marching ptr */
			**elem;		/* the new place for the element */

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!avec_exists(av)) {
#ifdef RKOERROR
		(void) rkopsterror("avec_resize : ");
#endif
		return -1;
	}

	/* keep the old hash here */
	hash = av->hash;
	if (!hash) {
#ifdef RKOERROR
		rkoerrno = RKOGENERR;
		(void) rkocpyerror("avec_resize : null hash!");
		return -2;
#endif
	}
	oldcap = av->capacity;

	/* determine the new capacity */
	if (newcap <= 0) {
		newcap = avec_hash_cap(2*av->capacity);
	} else if (newcap < av->number) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		(void) rkocpyerror("avec_resize : "
			"new capacity can not handle current data");
#endif
		return -3;
	}
	newcap = avec_hash_cap(newcap);

	/* set up new hash */
	if (!(av->hash = avec_alloc_hash(newcap))) {
#ifdef RKOERROR
		(void) rkopsterror("avec_resize : ");
#endif
		return -4;
	}
	av->capacity = newcap;

	/* start walk through old hash and reassign to new hash */
	ptrptr = hash;
	ptrptr--;	/* backup one */
	while ((++ptrptr - hash) < oldcap) {
		if (*ptrptr && *ptrptr != &REMOVED) {
			/* found an element */
			if (!(elem = avec_hash_search(AVEC_NEXT, av,
				(*ptrptr)->key))) {
#ifdef RKOERROR
				rkopsterror("avec_resize : ");
#endif
				return -4;
			} else {		/* got a place */
				*elem = *ptrptr;
			}
		}
	}
	/* free old hash */
	free(hash);

	return 0;
}

