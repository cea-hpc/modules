/* iprime.c -
 * a simple minded routine to find out whether a positive number is prime.
 * returns 1 if prime, else it returns the smallest factor > 1 if not.
 * An invalid value causes the function to return 0;
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1999 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "isqrt.h"

/* a look-up table of the first few primes */
extern unsigned char iprime_lut[54];

/* The procedure just tries the table of primes, then every odd value
 * after that upto the integer sqrt of the value
 *
 * If the number is a prime then iprime returns 1,
 * else it returns the lowest factor found.
 *
 * This last property can be used to factorize an integer
 * into it's prime factors.
 */

/* this assumes that the type is larger than unsigned char */
#define _IPRIME(NM, TYPE, _SQRT) \
TYPE NM(TYPE in) { TYPE i; \
	if (in < 1) return 0; if (in == 1) return 1; \
	for (i=0; i < sizeof(iprime_lut)/sizeof(*iprime_lut); ++i) { \
		if (in == ((TYPE) iprime_lut[i])) return 1; \
		if (!(in%((TYPE) iprime_lut[i]))) return (TYPE) iprime_lut[i]; }  \
	for (i = iprime_lut[sizeof(iprime_lut)/sizeof(*iprime_lut)-1]+2; i<=_SQRT(in); i+= 2) { \
		if (!(in%i)) return i; } return 1; }

