static const char RCSID[]="@(#)$Id: uvec2str.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec2str       copies a uvec to a string with the elements
 *  separated  by the given delimeter, uses the
 *  default string functions for  creating  the uvec.
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

/* uvec2str - copies a uvec into a strmalloc object
 *	- outputs a (char *) ptr else NULL
 */
char *uvec2str(uvec const *uv, char const *token) {
	char **argv;
	char *tmp = (char *) NULL;
	int i,len = 0, toklen = strlen(token);

	if (! token) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		rkocpyerror("str2uvec : NULL token!");
#endif
		return tmp;
	}
	
	if (uvec_exists(uv)) {
		argv = uvec_vector(uv);
		/* count the total length */
		for (i = 0; *argv != (char *) NULL; ++i, ++argv) {
			if (i) len += toklen;
			len += strlen(*argv);
		}
		/* alloc space */
		if ((char *) NULL==(tmp=(uv->str_fns.str_alloc)("", len + 1))) {
#ifdef RKOERROR
			if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
			rkopsterror("uvec2str : ");
#endif
			return tmp;
		}
		/* copy stuff over */
		argv = uvec_vector(uv);
		for (i = 0; *argv != (char *) NULL; ++i, ++argv) {
			if (i) strcat(tmp, token);
			strcat(tmp, *argv);
		}
	}
	return tmp;
}

