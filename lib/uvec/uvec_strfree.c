static const char RCSID[]="@(#)$Id: uvec_strfree.c,v 1.1 2002/09/13 04:36:21 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_strfree       free a uvec2str allocated string
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2002 by R.K.Owen,Ph.D.		                      	     **
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

/* uvec_strfree - free a uvec2str allocated string
 *	- returns 0 if OK
 */
int uvec_strfree(uvec const *uv, char **str) {
	int retval = 0;

	/* no string to dealloc */
	if (! str) return retval;
	
	if (uvec_exists(uv)) {
		/* free string */
		if (retval=(uv->str_fns.str_free)(str)) {
#ifdef RKOERROR
			if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
			rkopsterror("uvec_strfree : ");
#endif
		}
	} else {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		rkocpyerror("uvec_strfree : invalid uvec!");
#endif
		retval = -1;
	}
	return retval;
}

