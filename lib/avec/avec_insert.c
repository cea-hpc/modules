static const char RCSID[]="@(#)$Id: avec_insert.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
#include <stdarg.h>
#include "config.h"
#include "avec.h"
#include "avec_.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_insert - insert an element into the hash table,
 * returns 0 if the insert was successful.
 * Else it returns 1 if the key already exists and the default data fns
 * are used.
 * and < 0 if an error occured.
 */
int avec_insert(avec *av, char const *key, ...) {
	va_list vargs;
	int		  retval;	/* user data_add return value */
	avec_element	**elem;		/* place to insert value */

	if (!(elem = avec_hash_search(AVEC_INSERT, av,key))) {
#ifdef RKOERROR
		rkopsterror("avec_insert : ");
#endif
		return -1;
	}

	/* get variable arg pointer */
	va_start(vargs,key);

	/* NULL out if removed */
	if (*elem == &REMOVED) *elem = (avec_element *) NULL;

	if (*elem == (avec_element *) NULL) {	/* add element */
		/* alloc element */
		if (avec_alloc_element(key, elem)) {
#ifdef RKOERROR
			rkopsterror("avec_insert : ");
#endif
			return -2;
		}
		retval = (av->fns.data_add)(&((*elem)->data), vargs);
		if (retval) {
#ifdef RKOERROR
			if (! rkostrerror() ) {
				rkocpyerror("unspecified user data_add error");
				rkoerrno = RKOUSEERR;
			}
			rkopsterror("avec_insert : ");
#endif
			return retval;
		}
		av->number++;
		if (avec_do_resize(av)) {
			if ((retval = avec_resize(av,0)) < 0) {
#ifdef RKOERROR
				rkopsterror("avec_insert : ");
#endif
				return retval;
			}
		}
		return 0;
	} else { /* value already exists - pass off to data_add anyways */
		retval = (av->fns.data_add)(&((*elem)->data), vargs);
#ifdef RKOERROR
		if (retval < 0) {
			if (! rkostrerror() ) {
				rkocpyerror("unspecified user data_add error2");
				rkoerrno = RKOUSEERR;
			}
			rkopsterror("avec_insert : ");
		}
#endif
		return retval;
	}
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 1;
}

