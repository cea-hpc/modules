static const char RCSID[]="@(#)$Id: uvec_push.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_push      same as uvec_add
 *
 *  uvec_pop       pop off one element at end of vector.
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
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif

/* ---------------------------------------------------------------------- */
/* uvec_push - add 1 element to end of vector (same as uvec_add) */
int uvec_push(uvec *uv, char const *str) {
	return uvec_add(uv,str);
}
/* ---------------------------------------------------------------------- */
/* uvec_pop - pop off 1 element at end of vector
 *	and delete it
 */ 	
int uvec_pop(uvec *uv) {
	int rstat;
	if ((rstat = uvec_delete(uv, uv->number - 1))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_pop : ");
#endif
		return rstat - 128;
	}

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
