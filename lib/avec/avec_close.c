static const char RCSID[]="@(#)$Id: avec_close.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_close - destroy the avec contents - data and keys */
int avec_close(avec *av, ...) {
	int retval = 0;
	va_list vargs;
	avec_element **aeptr = (avec_element **) NULL;

	if (av == (avec *) NULL) {
#ifdef RKOERROR
		(void) rkocpyerror("avec_close : null pointer!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}

	/* get variable arg pointer */
	va_start(vargs,av);

	/* walk through hash and free elements, first destroy data */
	while ((aeptr = avec_walk_r(av,aeptr))) {
		/* destroy data with data_rm */
		if ((retval = (av->fns.data_rm)(&((*aeptr)->data), vargs))) {
#ifdef RKOERROR
			if (rkostrerror()) {
				(void) rkopsterror("avec_close : ");
			} else {
				(void) rkocpyerror("avec_close : "
				" unspecified data_rm error");
				rkoerrno = RKOMEMERR;
			}
#endif
			return retval;
		}
		/* destroy the hash elements */
		if ((retval = avec_dealloc_element(aeptr))) {
#ifdef RKOERROR
			(void) rkopsterror("avec_close : ");
			rkoerrno = RKOMEMERR;
#endif
			return retval;
		}
	}
	av->number = 0;

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

