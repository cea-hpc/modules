static const char RCSID[]="@(#)$Id: uvec_dtor.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_dtor      destroy the Unix vector and it's  contents.
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
/* uvec_dtor - destroy the uvec (calls uvec_close also) */
int uvec_dtor(uvec **uv) {
	int retval = 0;
	if ((retval = uvec_close(*uv))) return retval;
	if ((retval = uvec_dealloc(uv))) return retval;
	return retval;
}

