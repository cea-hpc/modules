static const char RCSID[]="@(#)$Id: strmalloc.c,v 1.1 2002/07/18 22:23:57 rkowen Exp $";
static const char AUTHOR[]="@(#)strmalloc 1.0 09/01/1999 R.K.Owen,Ph.D.";
/* str_malloc	- adds a strdup-like routine for copying strings.
 * str_nmalloc	- same as above with over allocation.
 * str_free	- frees the string memory storage.
 * str_sizeof	- returns the maximum string size.
 * str_cpy	- safely copies the input string to the str_malloc object
 * str_cat	- safely cats the input string onto the str_malloc object
 *
 * by R.K. Owen,Ph.D.   1999/09/01
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1999 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <strings.h>
#include <stdlib.h>
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "strmalloc.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif

static char id[3] = "STR";

char *str_nmalloc(char const *in, size_t n) {
	char *out = (char *) NULL;
	size_t sizeit;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (in) {
		sizeit = n;

		if(!(out = (char *) calloc(sizeit * sizeof(char)
		+ sizeof(size_t) + sizeof(id),1))) {
#ifdef RKOERROR
			(void) rkocpyerror("str_nmalloc : malloc error!");
			rkoerrno = RKOMEMERR;
#endif
		}
		(void) strncpy(out, id, sizeof(id));
		out += sizeof(id);
		(void) memcpy(out, &sizeit, sizeof(size_t));
		out += sizeof(size_t);
		(void) strncpy(out, in, n);
#ifdef RKOERROR
	} else {
			(void) rkocpyerror("str_nmalloc : NULL string!");
			rkoerrno = RKOUSEERR;
#endif
	}
	return out;
}

char *strnmalloc(char const *in, size_t n) {
	return str_nmalloc(in, n);
}

char *str_malloc(char const *in) {
	char *out = (char *) NULL;
	size_t sizeit;

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!in) {
#ifdef RKOERROR
		(void) rkocpyerror("str_malloc : NULL string!");
		rkoerrno = RKOUSEERR;
#endif
		return out;
	}

	sizeit = strlen(in)+1;
	out = str_nmalloc(in, sizeit);
	if (!out) {	/* test if input is non-zero */
#ifdef RKOERROR
		(void) rkopsterror("str_malloc : ");
#endif
	}
	return out;
}

char *strmalloc(char const *in) {
	return str_malloc(in);
}

/* str_free_ - only use this if sure that a str object is passed
 */
static int str_free_(char **str, size_t len) {
	char *ptr;

	ptr = *str - sizeof(size_t) - sizeof(id);

	(void) memset((void *) ptr, 0, sizeof(id)+sizeof(size_t)+len);
	free((void *) ptr);

	*str = (char *) NULL;

	return 0;
}

int str_free(char **str) {
	size_t sizeit;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (str && *str) {
		if ((size_t) -1 == (sizeit = str_sizeof(*str))) {
#ifdef RKOERROR
			(void) rkopsterror("str_free : ");
#endif
			return -1;
		}
		return str_free_(str, sizeit);
	}
	return 0;
}

int strfree(char **str) {
	return str_free(str);
}

size_t str_sizeof(const char *str) {
	size_t sizeit;
	char *ptr;

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (str && *str) {
		ptr = (char *) str - sizeof(size_t) - sizeof(id);
		if (strncmp(ptr, id, sizeof(id))) {
#ifdef RKOERROR
			rkoerrno = RKOUSEERR;
		(void) rkocpyerror("str_sizeof : invalid string object!");
#endif
			return (size_t) -1;
		}
		(void) memcpy(&sizeit, (void *) (ptr + sizeof(id)),
			sizeof(size_t));
		return sizeit;
	}
	return 0;
}

/* str_ncpy_ - use this if sure if no size fudge factor is needed */
static char *str_ncpy_(char **str, char *in, size_t n) {
	char *out = (char *) NULL;
	size_t sizeit;

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!in) return out;

	if ((size_t) -1 == (sizeit = str_sizeof(*str))) {
#ifdef RKOERROR
		(void) rkopsterror("str_ncpy_ : ");
#endif
		return out;
	}

	if (sizeit < n) {	/* insufficient memory */
		str_free_(str, sizeit);
		out = str_nmalloc(in,n);
		return *str = out;
	} else {
		return strncpy(*str, in, n);
	}
}

char *str_ncpy(char **str, char *in, size_t n) {
	return str_ncpy_(str, in, n);
}

char *str_cpy(char **str, char *in) {
	char *retval = (char *) NULL;

	if (!*str || !in)
		return retval;

	retval = str_ncpy_(str, in, strlen(in)+1);
#ifdef RKOERROR
	if (!retval && rkoerrno != RKO_OK)
		rkopsterror("str_cpy : ");
#endif
	return retval;
}

char *str_ncat(char **str, char *in, size_t n) {
	char *out = (char *) NULL;
	size_t sizeit;

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (!in) return out;

	if ((size_t) -1 == (sizeit = str_sizeof(*str))) {
#ifdef RKOERROR
		(void) rkopsterror("str_cat : ");
#endif
		return out;
	}

	if (sizeit <= n + strlen(*str)) {   /* insufficient memory */
		out = str_nmalloc(*str, n + strlen(*str));
		str_free_(str,sizeit);
		strncat(out, in, n);
		return *str = out;
	} else {
		return strcat(*str, in);
	}
}

char *str_cat(char **str, char *in) {
	char *retval = (char *) NULL;

	if (!*str || !in)
		return retval;

	retval = str_ncat(str, in, strlen(in)+1);
#ifdef RKOERROR
	if (!retval && rkoerrno != RKO_OK)
		rkopsterror("str_cat : ");
#endif
	return retval;
}

