static const char RCSID[]="@(#)$Id: avec_set_fns.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
static const char AUTHOR[]="@(#)avec 1.0 2002/02/08 R.K.Owen,Ph.D.";
/* avec.c -
 * This could have easily been made a C++ class, but is
 * confined to C so that it can be maximally portable.
 *
 * This is an associative vector package (a hash array)
 * that uses quadratic hashing.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2002 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "config.h"
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif
#include "avec.h"
#include "iprime.h"
#ifdef HAVE_STRMALLOC
#  include "strmalloc.h"
#endif
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* Implement AVEC_COUNT
 * put the count into the (void *) ptr
 * but limited to the unsigned integer size that fits
 */
static int avec_count_insert (void **data, va_list ap) {
	int retval = 1;
	unsigned short us;
	unsigned int ui;
	unsigned long ul;
	if (!data) return -1;

	if (*data == (void *) NULL) retval = 0;

	if (sizeof(void *) == sizeof(unsigned int)) {
		ui = (unsigned int) *data;
		*data = (void *) ++ui;
	} else if (sizeof(void *) == sizeof(unsigned short)) {
		us = (unsigned short) *data;
		*data = (void *) ++us;
	} else if (sizeof(void *) == sizeof(unsigned long)) {
		ul = (unsigned long) *data;
		*data = (void *) ++ul;
	} else {	/* hope for the best */
		ui = (unsigned int) *data;
		*data = (void *) ++ui;
	}
	return retval;
}

static int avec_count_delete (void **data, va_list ap) {
	int retval = 1;
	unsigned short us;
	unsigned int ui;
	unsigned long ul;
	if (!data) return -1;

	if (*data == (void *) NULL) retval = 0;

	if (sizeof(void *) == sizeof(unsigned int)) {
		ui = (unsigned int) *data;
		*data = (void *) --ui;
	} else if (sizeof(void *) == sizeof(unsigned short)) {
		us = (unsigned short) *data;
		*data = (void *) --us;
	} else if (sizeof(void *) == sizeof(unsigned long)) {
		ul = (unsigned long) *data;
		*data = (void *) --ul;
	} else {	/* hope for the best */
		ui = (unsigned int) *data;
		*data = (void *) --ui;
	}

	if (*data == (void *) NULL) retval = 0;

	return retval;
}

static int avec_count_rm (void **data, va_list ap) {
	if (!data) return -1;
	*data = (void *) NULL;
	return 0;
}

static avec_fns count_fns = {
	AVEC_COUNT,
	avec_count_insert,
	avec_count_delete,
	avec_count_rm
};

/* ---------------------------------------------------------------------- */
/* wrappers for the StdC string functions
 */
static int stdc_malloc (void **data, va_list ap) {
	char const *str = va_arg(ap,char *);
	if (!data) return -1;
	if (*data) return 1;
	if((*data = calloc((strlen(str)+1),sizeof(char)))) {
		(void) strcpy((char *) *data, str);
		return 0;
	}
	return -2;
}
static int stdc_free (void **data, va_list ap) {
	free(*data);
	*data = (char *) NULL;
	return 0;
}

static avec_fns stdc_fns = {
	AVEC_STDC,
	stdc_malloc,
	stdc_free,
	stdc_free
};

/* ---------------------------------------------------------------------- */
#ifdef HAVE_STRMALLOC
/* ---------------------------------------------------------------------- */
/* wrappers for the strmalloc string functions
 */
static int str_malloc (void **data, va_list ap) {
	char const *str = va_arg(ap,char *);
	if (!data) return -1;
	if (*data) return 1;
	if((*data = (void *) strmalloc(str))) return 0;
	return -2;
}

static int str_free (void **data, va_list ap) {
	strfree((char **) data);
	return 0;
}

static avec_fns strmalloc_fns = {
	AVEC_STRMALLOC,
	str_malloc,
	str_free,
	str_free
};

avec_fns default_fns = {
	AVEC_STRMALLOC,
	str_malloc,
	str_free,
	str_free
};
#else
avec_fns default_fns = {
	AVEC_STDC,
	stdc_malloc,
	stdc_free,
	stdc_free
};
#endif
/* set the default set of data functions to use
 */
int avec_set_fns(enum avec_def_fns type, avec_fns *fns) {
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	if (type == AVEC_DEFAULT) {
#ifdef HAVE_STRMALLOC
		type = AVEC_STRMALLOC;
#else
		type = AVEC_STDC;
#endif
	}
	if (type == AVEC_STDC) {
		default_fns.data_add = stdc_fns.data_add;
		default_fns.data_del = stdc_fns.data_del;
		default_fns.data_rm = stdc_fns.data_rm;
#ifdef HAVE_STRMALLOC
	} else if (type == AVEC_STRMALLOC) {
		default_fns.data_add = stdc_fns.data_add;
		default_fns.data_del = stdc_fns.data_del;
		default_fns.data_rm = stdc_fns.data_rm;
#endif
	} else if (type == AVEC_COUNT) {
		default_fns.data_add = count_fns.data_add;
		default_fns.data_del = count_fns.data_del;
		default_fns.data_rm = count_fns.data_rm;
	} else if (type == AVEC_USER) {
		if (fns == (avec_fns*) NULL
		|| fns->data_add == NULL
		|| fns->data_del == NULL
		|| fns->data_rm == NULL) {
#ifdef RKOERROR
			(void) rkocpyerror(
				"avec_set_fns : null data functions!");
			rkoerrno = RKOUSEERR;
#endif
			return -1;
		}
		default_fns.data_add = fns->data_add;
		default_fns.data_del = fns->data_del;
		default_fns.data_rm = fns->data_rm;
	}
	default_fns.type = type;
	return 0;
}
/* return what type of data functions are currently default
 */
enum avec_def_fns avec_get_fns(void) {
	return default_fns.type;
}

