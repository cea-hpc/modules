static const char RCSID[]="@(#)$Id: uvec_init.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_init_     initialize Unix vector to capacity  ``cap''
 *  using the given set of string functions.
 *
 *  uvec_init      initialize  Unix vector to capacity ``cap''
 *  using the default set of string  functions.
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
#include <string.h>
#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif

char uvec_TAG[5] = "UVEC";
extern uvec_str default_str_fns;

/* ---------------------------------------------------------------------- */
/* uvec_init_ - construct Unix vector to capacity cap use the
 * given string functions.
 * returns <0 if an error, else 0 if OK as well as all the other functions
 */
int uvec_init_(uvec *uv, int cap, uvec_str strfns) {
	if (uv == (uvec *) NULL) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_init : null pointer!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
/* can't guarantee that struct will be initialized to 0 hence use "tag" */
	if (!strncmp(uv->tag,uvec_TAG, 5)) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_init : already initialized!");
		rkoerrno = RKOUSEERR;
#endif
		return -2;
	}
	if (cap < 1) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_init : invalid capacity!");
		rkoerrno = RKOUSEERR;
#endif
		return -3;
	}
	if (!(uv->vector = (char **) calloc(cap, sizeof(char *)))) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_init : malloc error!");
		rkoerrno = RKOMEMERR;
#endif
		return -4;
	}
	(void) strcpy(uv->tag, uvec_TAG);
	uv->capacity = cap;
	uv->number = 0;
	uv->str_fns.type = strfns.type;
	uv->str_fns.str_alloc = strfns.str_alloc;
	uv->str_fns.str_free = strfns.str_free;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
/* uvec_init_ - construct Unix vector to capacity cap
 * use the default string functions
 * uvec_init will call uvec_init_() to set things up.
 */
int uvec_init(uvec *uv, int cap) {
	return uvec_init_(uv, cap, default_str_fns);
}
