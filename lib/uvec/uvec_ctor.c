static const char RCSID[]="@(#)$Id: uvec_ctor.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_ctor_     construct  Unix  vector to capacity ``cap''
 *  using the given set of string functions.
 *
 *  uvec_ctor      construct Unix vector to  capacity  ``cap''
 *  using  the default set of string functions.
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

extern uvec_str default_str_fns;

/* ---------------------------------------------------------------------- */
/* uvec_ctor_ - construct Unix vector to capacity cap and use the
 * given string functions.
 * returns NULL if an error, else the memory location  if OK.
 * uvec_ctor_ will call uvec_init_() to set things up.
 */
uvec *uvec_ctor_(int cap, uvec_str str_fns) {
	uvec *uv = (uvec *) NULL;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif

	if ((uvec *) NULL == (uv = uvec_alloc_(str_fns))) {
#ifdef RKOERROR
		if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
		(void) rkopsterror("uvec_ctor_ : ");
#endif
		return uv;
	}
	if (uvec_init_(uv,cap,str_fns)) {
#ifdef RKOERROR
		if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
		(void) rkopsterror("uvec_ctor_ : ");
#endif
		(void) uvec_dealloc(&uv);
		uv = (uvec *) NULL;
	}
	return uv;
}

/* uvec_ctor - construct Unix vector to capacity cap
 * uvec_ctor will call uvec_ctor_() to set things up.
 */
uvec *uvec_ctor(int cap) {
	return uvec_ctor_(cap, default_str_fns);
}

