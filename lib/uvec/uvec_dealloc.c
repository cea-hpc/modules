static const char RCSID[]="@(#)$Id: uvec_dealloc.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_dealloc   Deallocate  the  uvec object (vector is not touched)
 *
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <stdlib.h>
#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif

/* ---------------------------------------------------------------------- */
/* uvec_dealloc - deallocate the unitialized uvec  */
int uvec_dealloc(uvec **uv) {
	int retval = 0;
	free (*uv);
	*uv = (uvec *) NULL;
	return retval;
}
