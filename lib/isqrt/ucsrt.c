static const char RCSID[]="@(#)$Id: ucsrt.c,v 1.1 2002/07/18 22:11:15 rkowen Exp $";
static const char AUTHOR[]="@(#)isqrt 1.0 1998/11/26 R.K.Owen,Ph.D.";
/* isqrt.c -
 * finds the the integer sqrt of the given integer.
 * returns 0 if a user error (such as the argument is non-positive)
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include "isqrt/isqrt_.h"

/* must handle the char versions special and can optimize because the
 * LUT is "exact"
 */
unsigned char ucsqrt(unsigned char a) {
	unsigned char r;
	signed char rr;
	if (a == 0) return 0;
	if ((rr = scsqrt(a)) > 0) return rr;
	r = isqrt_lut[a];
	r >>= 4;
	return r;
}

