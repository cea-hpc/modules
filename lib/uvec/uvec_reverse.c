static const char RCSID[]="@(#)$Id: uvec_reverse.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 * uvec_reverse   reverses the element order of the vector.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* uvec_reverse - reverses the element order of the vector */
int uvec_reverse(uvec const *uv) {
	int num;
	char **ptrstart, **ptrend;
	char *swap;
	if (!uvec_exists(uv)) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_reverse : ");
		rkoerrno = RKOUSEERR;
		return -1;
#endif
	}
	num = uvec_number(uv);
	if (num > 1) {		/* do reversal */
		ptrstart = uvec_vector(uv);
		ptrend = ptrstart + uvec_end(uv);
		while (ptrstart < ptrend) {
			swap = *ptrend;
			*ptrend-- = *ptrstart;
			*ptrstart++ = swap;
		}
	}
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
