static const char RCSID[]="@(#)$Id: uvec_copy.c,v 1.2 2002/08/02 21:57:51 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_copy      copy one uvec to another unitialized one.
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
/* uvec_copy - copy one uvec to another unitialized one */
int uvec_copy(uvec *u, uvec const *v) {
	int rstat;

	if ((rstat = uvec_copy_vec(u, (char const * const *) uvec_vector(v),
					uvec_capacity(v)))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_copy : ");
#endif
		return rstat - 128;
	}
	return 0;
}

