static const char RCSID[]="@(#)$Id: uvec_shift.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_unshift   add one element to start of vector.
 *
 *  uvec_shift     shift one element from start of vector.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <string.h>
#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* uvec_unshift - add 1 element to the beginning of vector */
int uvec_unshift(uvec *uv, char const *str) {
	int rstat;

	if (! str ) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_unshift : NULL string!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	if ((rstat = uvec_ninsert(uv, str, strlen(str), 0))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_unshift : ");
#endif
		return rstat - 128;
	}

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
/* ---------------------------------------------------------------------- */
/* uvec_shift - shift off 1 element at the beginning of vector
 *	and delete it
 */ 	
int uvec_shift(uvec *uv) {
	int rstat;
	if ((rstat = uvec_delete(uv, 0))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_shift : ");
#endif
		return rstat - 128;
	}

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
