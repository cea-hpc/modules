static const char RCSID[]="@(#)$Id: uvec_randomize.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_randomize randomizes the vector, will  be  repeatable
 *  if  given  the same random number seed.  If
 *  seed <=0 then will "randomly" choose one.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <time.h>
#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* wrappers for StdC rand functions to conform to librko - urand
 *	if not available
 */
#if !defined(HAVE_URAND)
static double urand (void) {
	return ((double) rand())/(((double) RAND_MAX) + 1);
}
static void setseed(int seed) {
	srand((unsigned int) seed);
}
#endif

/* ---------------------------------------------------------------------- */
/* uvec_randomize - randomizes the vector  - will be deterministic if
 * use the same seed, However, if seed<=0 then will "randomly"
 * choose one.
 */
int uvec_randomize(uvec const *uv, int seed) {
	char *swap;
	char **vec, **ptr1, **ptr2;
	int num, i;

	if (!uvec_exists(uv)) {
#  ifdef RKOERROR
		(void) rkopsterror("uvec_randomize : ");
		rkoerrno = RKOUSEERR;
		return -1;
#  endif
	}
	if (seed <=0) {
		seed = (int) time(NULL);
	}
	if (seed < 0) {
#  ifdef RKOERROR
		(void) rkocpyerror("uvec_randomize : seed making error!");
		rkoerrno = RKOUSEERR;
		return -2;
#  endif
	}
	setseed(seed);
	num = uvec_number(uv);
	vec = uvec_vector(uv);
	/* make 3*number of swaps */
	for (i = 0; i < 3*num; ++i) {
		ptr1 = vec + (int) (num*urand());
		ptr2 = vec + (int) (num*urand());
		swap = *ptr1;
		*ptr1 = *ptr2;
		*ptr2 = swap;
	}
#  ifdef RKOERROR
	rkoerrno = RKO_OK;
#  endif
	return 0;
}

