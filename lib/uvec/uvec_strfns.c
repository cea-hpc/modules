static const char RCSID[]="@(#)$Id: uvec_strfns.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_set_strfns
 *  register the set of string functions to use
 *  by default.
 *
 *  uvec_get_strfns
 *  query  as  to which set of string functions
 *  are being used, only the  values  given  by
 *  enum uvec_def_str_fns will be returned.
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
/* wrappers for the StdC string functions
 */
static char *stdc_nmalloc (char const *str, size_t n) {
	char *out;
	/* fprintf(stderr, "stdc malloc\n"); */
	if((out = calloc((n+1),sizeof(char))))
		(void) strncpy(out, str, n);
	return out;
}
static int stdc_free (char **str) {
	/* fprintf(stderr, "stdc free\n"); */
	free(*str);
	*str = (char *) NULL;
	return 0;
}

uvec_str stdc_str_fns = {
	UVEC_STDC,
	stdc_nmalloc,
	stdc_free
};

/* ---------------------------------------------------------------------- */
#ifdef HAVE_STRMALLOC
uvec_str strmalloc_str_fns = {
	UVEC_STRMALLOC,
	strnmalloc,
	strfree
};

uvec_str default_str_fns = {
	UVEC_STRMALLOC,
	strnmalloc,
	strfree
};
#else
uvec_str default_str_fns = {
	UVEC_STDC,
	stdc_nmalloc,
	stdc_free
};
#endif
/* set the default set of string functions to use
 */
int uvec_set_strfns(enum uvec_def_str_fns type, uvec_str *strfns) {
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (type == UVEC_DEFAULT) {
#ifdef HAVE_STRMALLOC
		type = UVEC_STRMALLOC;
#else
		type = UVEC_STDC;
#endif
	}
	if (type == UVEC_STDC) {
		default_str_fns.str_alloc = stdc_str_fns.str_alloc;
		default_str_fns.str_free = stdc_str_fns.str_free;
#ifdef HAVE_STRMALLOC
	} else if (type == UVEC_STRMALLOC) {
		default_str_fns.str_alloc = strmalloc_str_fns.str_alloc;
		default_str_fns.str_free = strmalloc_str_fns.str_free;
#endif
	} else if (type == UVEC_USER) {
		if (strfns == (uvec_str*) NULL
		|| strfns->str_alloc == NULL
		|| strfns->str_free == NULL) {
#ifdef RKOERROR
			(void) rkocpyerror(
				"uvec_set_strfns : null string functions!");
			rkoerrno = RKOUSEERR;
#endif
			return -1;
		}
		default_str_fns.str_alloc = strfns->str_alloc;
		default_str_fns.str_free = strfns->str_free;
	}
	default_str_fns.type = type;
	return 0;
}

/* return what type of string functions are currently default
 */
enum uvec_def_str_fns uvec_get_strfns(void) {
	return default_str_fns.type;
}

