static const char RCSID[]="@(#)$Id: uvec_copy_str.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 * uvec_copy_str create a uvec from a token delimited string.
 * Tokens escaped with a '\' are not counted.
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

/* uvec_copy_str - create a uvec from a token delimited string
 * 	'\' before the token escapes it
 * 	... but it's still possible to confuse the count
 */
int uvec_copy_str(uvec *u, char const *token, char const *string) {
	int num = uvec_count_tok(token,string);
	const char *ptr = string;
	const char *tokptr;
	int toklen = strlen(token);
	int tokfound = 0;
	int rstat;

	if (num < 0) {
#ifdef RKOERROR
		if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
		rkocpyerror("uvec_copy_str : token count error!");
#endif
		return -1;
	}

#if 0
	if (! string) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		rkocpyerror("uvec_copy_str : NULL string!");
#endif
		return -2;
	}
#endif

	if ((rstat = uvec_init(u, num + 1))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_copy_str : ");
#endif
		return rstat - 128;
	}

	while (ptr && *ptr) {
		tokptr = ptr;
		while (! tokfound) {
			tokptr = strstr(tokptr, token);
			if (tokptr && (tokptr > string)) {
				if (*(tokptr-1) != '\\') {
					tokfound = 1;
				} else {
					tokptr++;
					continue;
				}
			}
			if ((tokptr == string) || !tokptr) tokfound = 1;
		}
		tokfound = 0;
		if (! tokptr ) {
			if ((rstat = uvec_add(u, ptr))) {
#ifdef RKOERROR
				(void) rkopsterror("uvec_copy_str : ");
#endif
				return rstat - 128;
			}
			break;
		} else {
			uvec_nadd(u, ptr, tokptr - ptr);
		}
		ptr = tokptr + toklen;
	}

	return 0;
}

