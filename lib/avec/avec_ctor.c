static const char RCSID[]="@(#)$Id: avec_ctor.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
static const char AUTHOR[]="@(#)avec 1.0 2002/02/08 R.K.Owen,Ph.D.";
/* avec.c -
 * This could have easily been made a C++ class, but is
 * confined to C so that it can be maximally portable.
 *
 * This is an associative vector package (a hash array)
 * that uses quadratic hashing.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2002 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <stdlib.h>
#include "config.h"
#include "avec.h"
#include "avec_.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_ctor_ - construct associative vector to capacity cap and use the
 * given data functions.
 * returns NULL if an error, else the memory location  if OK.
 * avec_ctor_ will call avec_init_() to set things up.
 */
avec *avec_ctor_(int cap, avec_fns fns) {
	avec *av = (avec *) NULL;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif

	if ((avec *) NULL == (av = avec_alloc_(fns))) {
#ifdef RKOERROR
		if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
		(void) rkopsterror("avec_ctor_ : ");
#endif
		return av;
	}
	if (avec_init_(av,cap,fns)) {
#ifdef RKOERROR
		if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
		(void) rkopsterror("avec_ctor_ : ");
#endif
		(void) avec_dealloc(&av);
		av = (avec *) NULL;
	}
	return av;
}

/* avec_ctor - construct associative vector to capacity cap
 * avec_ctor will call avec_ctor_() to set things up.
 */
avec *avec_ctor(int cap) {
	return avec_ctor_(cap, default_fns);
}

