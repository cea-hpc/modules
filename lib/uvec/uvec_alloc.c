static const char RCSID[]="@(#)$Id: uvec_alloc.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_alloc_    allocate  the uninitialized uvec object (no
 *  vector is allocated) using the given set of
 *  string functions.
 *
 *  uvec_alloc     allocate  the uninitialized uvec object (no
 *  vector is allocated) using the default  set
 *  of string functions.
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

extern uvec_str default_str_fns;

/* ---------------------------------------------------------------------- */
/* uvec_alloc_ - allocate an unitialized uvec object and use the
 * given string functions.
 * returns NULL if an error, else the memory location  if OK.
 */
uvec *uvec_alloc_(uvec_str strfns) {
	uvec *uv = (uvec *) NULL;

	if (!(uv = (uvec *) malloc(sizeof(uvec)))) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_alloc_ : uvec malloc error!");
		rkoerrno = RKOMEMERR;
#endif
		return uv;
	}
	uv->str_fns.str_alloc = strfns.str_alloc;
	uv->str_fns.str_free = strfns.str_free;
	return uv;
}

/* uvec_alloc - allocate an unitialized uvec object
 * set to use the default string functions
 * uvec_alloc will call uvec_alloc_() to set things up.
 */
uvec *uvec_alloc(void) {
	return uvec_alloc_(default_str_fns);
}

