static const char RCSID[]="@(#)$Id: uvec_close.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 * uvec_close     destroy the Unix vector contents.
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
/* uvec_close - destroy the uvec contents */
int uvec_close(uvec *uv) {
	int i;

	if (uv == (uvec *) NULL) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_dtor : null pointer!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	*(uv->tag) = '\0';
	for (i = 0; i < uv->number; ++i) {
		(uv->str_fns.str_free)(&(uv->vector[i]));
		uv->vector[i] = (char *) NULL;
	}
	free(uv->vector);
	uv->vector = (char **) NULL;
	uv->capacity = 0;
	uv->number = 0;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
