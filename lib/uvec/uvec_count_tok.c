static const char RCSID[]="@(#)$Id: uvec_count_tok.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_copy_str *  create a uvec from a token delimited string.
 *  Tokens escaped with a '\' are not counted.
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
#ifdef HAVE_STRMALLOC
#  include "strmalloc.h"
#endif
#ifdef RKOERROR
#  include "rkoerror.h"
#endif
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif

/* ---------------------------------------------------------------------- */
/* some useful functions ...
 * note that all these use no direct access to the structure
 * and form an example set of how to use uvec */
/* ---------------------------------------------------------------------- */

/* uvec_count_tok - returns the number of token delimited elements
 * 	in a string (trailing following token is optional)
 * 	returns -1 if an error
 * 	'\' before the token escapes it
 * 	... but it's still possible to confuse the count
 */
int uvec_count_tok(char const *token, char const *string) {
	int num = 0;
	int toklen = 0;
	const char *ptr = string;
	const char *tokptr;

	if (! string)	return 0;
	if (! *string)	return 0;
	if (! token)	return -1;	/* gotta supply a token */
	if (! *token)	return -1;
	toklen = strlen(token);
	if (! toklen)	return -1;
	while (*ptr) {
		num++;
		tokptr = strstr(ptr, token);
		if (! tokptr ) break;
		if ( tokptr > string) {
			if (*(tokptr-1) == '\\') num--;
		}
		ptr = tokptr + toklen;
	}

	return num;
}

