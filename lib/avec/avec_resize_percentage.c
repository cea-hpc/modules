static const char RCSID[]="@(#)$Id: avec_resize_percentage.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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

/* avec_resize_percentage - sets/gets the percentage size of number/capacity
 *	that will cause an automatic resize larger.
 *
 *	Set percentage 0 to return the current value.
 *	It will return 0 if no resizing is done.
 *	Set percentage < 0 to disable resizing.
 *	And don't set less than 10 or more than 70.
 *
 *	An automatic resize will approximately double the capacity.
 */

int avec_resize_percentage(avec *av, int percentage) {
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!avec_exists(av)) {
#ifdef RKOERROR
		(void) rkopsterror("avec_resize_percentage : ");
#endif
		return -1;
	}
	if (percentage == 0) {
		return (av->percentage < 0 ? 0 : av->percentage);
	} else if (percentage < 10 || percentage > 70) {
#ifdef RKOERROR
		(void) rkocaterror("avec_resize_percentage : out of range");
		rkoerrno = RKOUSEERR;
#endif
		return -2;
	} else {
		av->percentage = percentage;
	}
	return 0;
}

