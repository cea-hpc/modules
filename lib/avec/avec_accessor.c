static const char RCSID[]="@(#)$Id: avec_accessor.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
/* accessor functions */
int avec_exists(avec const *av) {
	int retval = 0;
	if (av == (avec *) NULL) {
#ifdef RKOERROR
		(void) rkocpyerror("avec_exists : null pointer!");
		rkoerrno = RKOUSEERR;
#endif
		retval = 0;
	} else {
		if (strncmp(av->tag,avec_TAG, 5)) {
#ifdef RKOERROR
			(void) rkocpyerror("avec_exists : avec doesn't exist!");
			rkoerrno = RKOUSEERR;
#endif
			retval = 0;
		} else {
#ifdef RKOERROR
			rkoerrno = RKO_OK;
#endif
			retval = 1;
		}
	}
	return retval;
}

int avec_capacity(avec const *av) {
	if (avec_exists(av)) {
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return av->capacity;
	} else {
#ifdef RKOERROR
		(void) rkopsterror("avec_capacity : ");
#endif
		return -1;
	}
}

int avec_number(avec const *av) {
	if (avec_exists(av)) {
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return av->number;
	} else {
#ifdef RKOERROR
		(void) rkopsterror("avec_number : ");
#endif
		return -1;
	}
}

/* ---------------------------------------------------------------------- */
/* avec_hash - returns a restrictive pointer to the hash
 * else returns NULL
 */
avec_element const * const *avec_hash(avec *av) {
	if (!avec_exists(av)) {
#ifdef RKOERROR
		(void) rkopsterror("avec_hash : ");
#endif
	}
	return (avec_element const * const *) av->hash;
}
