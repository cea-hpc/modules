static const char RCSID[]="@(#)$Id: avec_decrease.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
/* avec_decrease - decrease the size of the hash
 * returns 0 if OK, else < 0
 */
int avec_decrease(avec *av, int newcap) {

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!avec_exists(av)) {
#ifdef RKOERROR
		(void) rkopsterror("avec_decrease : ");
#endif
		return -1;
	}

	if (newcap >= av->capacity) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		(void) rkocpyerror("avec_decrease : can not decrease capacity");
#endif
		return -2;
	} else if (newcap <= 0) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		(void) rkocpyerror("avec_decrease : no default capacity");
#endif
		return -2;
	}
	return avec_resize(av, newcap);
}

