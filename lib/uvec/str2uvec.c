static const char RCSID[]="@(#)$Id: str2uvec.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  str2uvec copies a string to  a  uvec,  breaking  the
 *  string  at  the  given  delimiter, uses the
 *  default string functions for  creating  the uvec.
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

#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* A bunch of conversion routines
 */
/* str2uvec - converts a tokenized string to a uvec
 *	- outputs a uvec object ptr else NULL
 */
uvec *str2uvec(char const *token, char const *string) {
	int num = uvec_count_tok(token,string);
	uvec *tmp = (uvec *) NULL;

	if (num < 0) {
#ifdef RKOERROR
		if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
		rkocpyerror("str2uvec : token count error!");
#endif
		return tmp;
	}

#if 0
	if (! string) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		rkocpyerror("str2uvec : NULL string!");
#endif
		return tmp;
	}
#endif
	if (! token) {
#ifdef RKOERROR
		rkoerrno = RKOUSEERR;
		rkocpyerror("str2uvec : NULL token!");
#endif
		return tmp;
	}
	if ((uvec *) NULL != (tmp = uvec_alloc())) {
		if (uvec_copy_str(tmp, token, string)) {
#ifdef RKOERROR
			if (rkoerrno == RKO_OK) rkoerrno = RKOGENERR;
			rkopsterror("str2uvec : ");
#endif
			(void) uvec_dealloc(&tmp);
			return (uvec *) NULL;
		}
	}
	return tmp;
}

