static const char RCSID[]="@(#)$Id: vec2uvec.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  vec2uvec copies  a  vector (or part of a vector upto
 *  num elements) to a uvec, uses  the  default
 *  string  functions  for  creating  the uvec.
 *  Set num to a value less than or equal to  0
 *  to grab the entire vector.
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

/* vec2uvec - converts a vector into a uvec
 *	- outputs a uvec object ptr else NULL
 */
uvec *vec2uvec(char const * const *vec, int num) {
	uvec *tmp = (uvec *) NULL;

	if ((uvec *) NULL != (tmp = uvec_alloc())) {
		if (uvec_copy_vec(tmp, vec, num)) {
#ifdef RKOERROR
			if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
			rkopsterror("vec2uvec : ");
#endif
			(void) uvec_dealloc(&tmp);
			return (uvec *) NULL;
		}
	}
	return tmp;
}

