static const char RCSID[]="@(#)$Id: uvec_accessor.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <string.h>
#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

extern char uvec_TAG[5];

/* ---------------------------------------------------------------------- */
/* accessor functions */
int uvec_exists(uvec const *uv) {
	int retval = 0;
	if (uv == (uvec *) NULL) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_exists : null pointer!");
		rkoerrno = RKOUSEERR;
#endif
		retval = 0;
	} else {
		if (strncmp(uv->tag,uvec_TAG, 5)) {
#ifdef RKOERROR
			(void) rkocpyerror("uvec_exists : uvec doesn't exist!");
			rkoerrno = RKOUSEERR;
#endif
			retval = 0;
		} else {
#ifdef RKOERROR
			rkoerrno = RKO_OK;
#endif
			retval = 1;
		}
	}
	return retval;
}

int uvec_capacity(uvec const *uv) {
	if (uvec_exists(uv)) {
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return uv->capacity;
	} else {
#ifdef RKOERROR
		(void) rkopsterror("uvec_capacity : ");
#endif
		return -1;
	}
}

int uvec_number(uvec const *uv) {
	if (uvec_exists(uv)) {
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return uv->number;
	} else {
#ifdef RKOERROR
		(void) rkopsterror("uvec_number : ");
#endif
		return -1;
	}
}

int uvec_end(uvec const *uv) {
	if (uvec_exists(uv)) {
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return uv->number - 1;
	} else {
#ifdef RKOERROR
		(void) rkopsterror("uvec_end : ");
#endif
		return -1;
	}
}

char ** uvec_vector(uvec const *uv) {
	if (uvec_exists(uv)) {
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return uv->vector;
	} else {
#ifdef RKOERROR
		(void) rkopsterror("uvec_vector : ");
#endif
		return (char **) NULL;
	}
}

