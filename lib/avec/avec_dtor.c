static const char RCSID[]="@(#)$Id: avec_dtor.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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

#include <stdarg.h>
#include "config.h"
#include "avec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_dtor - destroy the avec (calls avec_close also) */
int avec_dtor(avec **av, ...) {
	int retval = 0;
	if ((retval = avec_close(*av))) {
#ifdef RKOERROR
		(void) rkopsterror("avec_dtor : ");
#endif
		return retval;
	}
	if ((retval = avec_dealloc(av))) {
#ifdef RKOERROR
		(void) rkopsterror("avec_dtor : ");
#endif
		return retval;
	}
	return retval;
}

