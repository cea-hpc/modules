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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/* a look-up table lut[i] = 256*sqrt(i/256) */
extern unsigned char isqrt_lut[256];

/* All the procedures shown here use the Newton-Cotes method to
 * iteratively refine the result.
 *
 * The LUT is used to give a sufficiently good start to
 * guarentee convergence.
 */

/* this assumes that the type is larger than unsigned char */
#define _ISQRT(NM, TYPE) \
TYPE NM(TYPE a) { \
	TYPE t = a, i=0,r; if (a <= 0) return 0; \
	while (t & ~((TYPE) 0xFF)) { t >>=2; i++; } \
	r = isqrt_lut[t]; if (i < 4) r >>= (4-i); else r <<= (i-4); \
	while ((t = r*r, (t > a) || ((a - 2*r - 1) >= t))) { \
	r+=(a/r); r>>=1; } return r; }

/* need to use the sign bit when near 0, hence the call to the signed
 * version; however if near unsigned maximum value, the signed version
 * sees it as negative and returns 0, then the unsigned version can
 * safely carry on. */

#define _USQRT(NM, TYPE, SNM, STYPE) \
TYPE NM(TYPE a) { \
	TYPE t = a, i=0,r; STYPE rr; \
	if (a == 0) return 0; \
	if ((rr = SNM(a)) > 0) return rr; \
	while (t & ~((TYPE) 0xFF)) { t >>=2; i++; } \
	r = isqrt_lut[t]; if (i < 4) r >>= (4-i); else r <<= (i-4); \
	while ((t = r*r, (t > a) || ((a - 2*r - 1) >= t))) { \
	r+=(a/r); r>>=1; } return r; }

