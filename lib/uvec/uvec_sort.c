static const char RCSID[]="@(#)$Id: uvec_sort.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_sort sort the vector, given the following types:
 *  UVEC_ASCEND, UVEC_DESCEND,
 *  UVEC_CASE_ASCEND, UVEC_CASE_DESCEND,  where
 *  the last two are only available if the str­
 *  casecmp function is available  for  ``case­
 *  less'' string comparisons.
 *
 *  uvec_qsort sort the vector, according to the passed comparison function,
 *  which is of the same type as needed by qsort()
 *  namely int cmp(void const *a, void const *b)
 *
 *  uvec_find find the first or last element that matches
 *  the string str depending on the  sort  type
 *  and  returns  the  element  number.  If the
 *  string is not found  then  returns  -1.   A
 *  value  less  than  -1  indicates  an error,
 *  which probably can be ignored.  type = uvec
 *  sorting type.
 *
 *  uvec_uniq remove  all  adjacent  duplicate  elements,
 *  where type =  uvec  sorting  type  ...  the
 *  important  information  is whether to use a
 *  caseless comparison or not, but  there  may
 *  be subtle side effects depending on whether
 *  the sort type is ascending or descending.
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

/* ---------------------------------------------------------------------- */
/* comparison functions */

static int uvec_sort_cmp_ascend(void const *a, void const *b) {
	return strcmp(*(char **) a, *(char **) b);
}

static int uvec_sort_cmp_descend(void const *a, void const *b) {
	return strcmp(*(char **) b, *(char **) a);
}

#ifdef HAVE_STRCASECMP
static int uvec_sort_cmp_case_ascend(void const *a, void const *b) {
	return strcasecmp(*(char **) a, *(char **) b);
}

static int uvec_sort_cmp_case_descend(void const *a, void const *b) {
	return strcasecmp(*(char **) b, *(char **) a);
}
#endif /* HAVE_STRCASECMP */

/* ---------------------------------------------------------------------- */
/* uvec_sort - sort the vector */

int uvec_sort(uvec *uv, enum uvec_order type) {
	int (*cmp)(void const *, void const *);
	if (!uvec_exists(uv)) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_sort : ");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	if (uvec_number(uv) > 0) {
		switch (type) {
		case UVEC_ASCEND:
			cmp = uvec_sort_cmp_ascend;
			break;
		case UVEC_DESCEND:
			cmp = uvec_sort_cmp_descend;
			break;
#ifdef HAVE_STRCASECMP
		case UVEC_CASE_ASCEND:
			cmp = uvec_sort_cmp_case_ascend;
			break;
		case UVEC_CASE_DESCEND:
			cmp = uvec_sort_cmp_case_descend;
			break;
#endif /* HAVE_STRCASECMP */
		default:
#ifdef RKOERROR
			(void) rkocpyerror("uvec_sort : invalid ordering type!");
			rkoerrno = RKOUSEERR;
#endif
			return -2;
		}
	}
	uvec_qsort(uv, cmp);
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

/* ---------------------------------------------------------------------- */
/* uvec_qsort - sort the vector */

int uvec_qsort(uvec *uv,  int (*cmp)(void const *a, void const *b)) {
	int retval = -1;
	if (uvec_exists(uv) && cmp) {
		qsort((void *) uvec_vector(uv), (size_t) uvec_number(uv),
			sizeof(char *), cmp);
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return 0;
	} else {
#ifdef RKOERROR
		if (rkoerrno != RKO_OK) (void) rkopsterror("uvec_sort : ");
		rkoerrno = RKOUSEERR;
#endif
		return retval;
	}
}

/* ---------------------------------------------------------------------- */
/* uvec_find - finds the first or last entry in a vector that matches
 * 	the string given the uvec sorting type.
 */

int uvec_find(uvec *uv, char const *str, enum uvec_order type) {
	int i = 0;
	char **vec;
	int (*cmp)(void const *, void const *);

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif

	if (!uvec_exists(uv)) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_find : ");
		rkoerrno = RKOUSEERR;
#endif
		return -2;
	}
	if (uvec_number(uv) > 0) {
		switch (type) {
		case UVEC_ASCEND:
			cmp = uvec_sort_cmp_descend;
			break;
		case UVEC_DESCEND:
			cmp = uvec_sort_cmp_ascend;
			break;
#ifdef HAVE_STRCASECMP
		case UVEC_CASE_ASCEND:
			cmp = uvec_sort_cmp_case_descend;
			break;
		case UVEC_CASE_DESCEND:
			cmp = uvec_sort_cmp_case_ascend;
			break;
#endif /* HAVE_STRCASECMP */
		default:	/* assume case sensitive */
			cmp = uvec_sort_cmp_ascend;
		}
	}
	while (*(vec = (uvec_vector(uv) + i))) {
		if (!(*cmp)(vec,  &str)) return i;
		i++;
	}
	return -1;	/* string not found */
}

/* ---------------------------------------------------------------------- */
/* uvec_uniq - remove all adjacent duplicate elements
 *	type = uvec sorting type ... the import information is whether
 *	to use a caseless comparison or not.
 */
int uvec_uniq(uvec *uv, enum uvec_order type) {
	int i = 1;
	char **vec;
	int (*cmp)(void const *, void const *);

	if (!uvec_exists(uv)) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_uniq : ");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	if (uvec_number(uv) > 1) {
		switch (type) {
		case UVEC_ASCEND:
			cmp = uvec_sort_cmp_ascend;
			break;
		case UVEC_DESCEND:
			cmp = uvec_sort_cmp_descend;
			break;
#ifdef HAVE_STRCASECMP
		case UVEC_CASE_ASCEND:
			cmp = uvec_sort_cmp_case_ascend;
			break;
		case UVEC_CASE_DESCEND:
			cmp = uvec_sort_cmp_case_descend;
			break;
#endif /* HAVE_STRCASECMP */
		default:
#ifdef RKOERROR
			(void) rkocpyerror("uvec_uniq : invalid ordering type!");
			rkoerrno = RKOUSEERR;
#endif
			return -2;
		}
		while (*(vec = (uvec_vector(uv) + i))) {
			if ((*cmp)(vec, (vec - 1))) {
				i++;			/* go to next */
			} else {
				uvec_delete(uv,i);	/* found match */
			}
		}
	}
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
