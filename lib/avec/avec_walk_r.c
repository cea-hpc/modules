static const char RCSID[]="@(#)$Id: avec_walk_r.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
#include "avec.h"
#include "avec_.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_walk_r - returns all the keys in a NULL terminated vector
 * else returns NULL
 * Must pass a NULL ptrptr on first call.
 */
avec_element **avec_walk_r(avec *av, avec_element **ptrptr) {
	avec_element **retval = (avec_element **) NULL;

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!avec_exists(av)) {
#ifdef RKOERROR
		(void) rkopsterror("avec_walk_r : ");
#endif
		return retval;
	}

	/* make sure have a value in ptrptr */
	if (ptrptr == (avec_element **) NULL) {
		ptrptr = av->hash;
		ptrptr--;	/* backup one if at start */
	}

	while ((++ptrptr - av->hash) < av->capacity) {
		if (ptrptr && *ptrptr && *ptrptr != &REMOVED) {
			/* found an element */
			return ptrptr;
		}
	}
	
	return retval;
}

