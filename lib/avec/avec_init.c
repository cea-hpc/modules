static const char RCSID[]="@(#)$Id: avec_init.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
/* avec_init_ - construct associative vector to capacity cap use the
 * given data functions.
 * returns <0 if an error, else 0 if OK as well as all the other functions
 */
int avec_init_(avec *av, int cap, avec_fns fns) {
	int newcap;
	if (av == (avec *) NULL) {
#ifdef RKOERROR
		(void) rkocpyerror("avec_init : null pointer!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
/* can't guarantee that struct will be initialized to 0 hence use "tag" */
	if (!strncmp(av->tag,avec_TAG, 5)) {
#ifdef RKOERROR
		(void) rkocpyerror("avec_init : already initialized!");
		rkoerrno = RKOUSEERR;
#endif
		return -2;
	}
	if (cap < 1) {
#ifdef RKOERROR
		(void) rkocpyerror("avec_init : invalid capacity!");
		rkoerrno = RKOUSEERR;
#endif
		return -3;
	}

	newcap = avec_hash_cap(cap);

	if (!(av->hash = avec_alloc_hash(newcap))) {
#ifdef RKOERROR
		(void) rkopsterror("avec_init : ");
#endif
		return -4;
	}
	(void) strcpy(av->tag, avec_TAG);
	av->capacity = newcap;
	av->number = 0;
	av->fns.type = fns.type;
	av->fns.data_add = fns.data_add;
	av->fns.data_del = fns.data_del;
	av->fns.data_rm = fns.data_rm;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
/* avec_init_ - construct associative vector to capacity cap
 * use the default data functions
 * avec_init will call avec_init_() to set things up.
 */
int avec_init(avec *av, int cap) {
	return avec_init_(av, cap, default_fns);
}

