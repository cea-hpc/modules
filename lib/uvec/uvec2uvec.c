static const char RCSID[]="@(#)$Id: uvec2uvec.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *   uvec2uvec      copies  a  uvec  to a uvec uses the default
 *                  string functions for creating the uvec.
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

/* uvec2uvec - copies a uvec into a uvec
 *	- outputs a uvec object ptr else NULL
 */
uvec *uvec2uvec(uvec const *uv) {
	uvec *tmp = (uvec *) NULL;

	if ((uvec *) NULL != (tmp = uvec_alloc())) {
		if (uvec_copy(tmp, uv)) {
#ifdef RKOERROR
			if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
			rkopsterror("uvec2uvec : ");
#endif
			(void) uvec_dealloc(&tmp);
			return (uvec *) NULL;
		}
	}
	return tmp;
}

