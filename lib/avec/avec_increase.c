static const char RCSID[]="@(#)$Id: avec_increase.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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

#include "config.h"
#include "avec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_increase - increase the size of the hash
 * if newcap <= 0 then increase by default size else compute  new capacity
 * returns 0 if OK, else < 0
 */
int avec_increase(avec *av, int newcap) {

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!avec_exists(av)) {
#ifdef RKOERROR
		(void) rkopsterror("avec_increase : ");
#endif
		return -1;
	}

	if (0 < newcap && newcap <= av->capacity) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		(void) rkocpyerror("avec_increase : can not decrease capacity");
#endif
		return -2;
	}
	return avec_resize(av, newcap);
}

