static const char RCSID[]="@(#)$Id: uvec_add.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_add       add one element to end of vector.
 *
 *  uvec_nadd      add one element of size n to end of vector.
 *
 *  uvec_addl      add a NULL terminated list of  elements  to
 *                 end of vector.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <stdarg.h>
#include <string.h>
#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* uvec_add - add 1 element to end of vector */
int uvec_add(uvec *uv, char const *str) {

	if (! str ) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_add : NULL string!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	return uvec_nadd(uv, str, strlen(str));
}

/* ---------------------------------------------------------------------- */
/* uvec_nadd - add 1 element of size n to end of vector */
int uvec_nadd(uvec *uv, char const *str, size_t n) {
	int rstat;
	if ((rstat = uvec_ninsert(uv, str, n, uv->number))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_nadd : ");
#endif
		return rstat - 128;
	}

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
} 

/* ---------------------------------------------------------------------- */
/* uvec_addl - add a variable number of elements (terminated by a NULL
 *	argument) to end of vector
 */
int uvec_addl(uvec *uv, ...) {
	int rstat;
	const char *str;
	va_list ap;

	va_start(ap, uv);
	while ((str = va_arg(ap, const char *))) {
		if ((rstat = uvec_add(uv, str))) {
#ifdef RKOERROR
			(void) rkopsterror("uvec_addl : ");
#endif
			return rstat - 128;
		}
	}
	va_end(ap);
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

