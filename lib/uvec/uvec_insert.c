static const char RCSID[]="@(#)$Id: uvec_insert.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_insert    insert an element before element ``place''.
 *
 *  uvec_ninsert   insert an element of size n before  element ``place''.
 *
 *  uvec_delete    delete an element at element ``place''.
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

#include <string.h>
#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* uvec internal macros */
#define 	UVECNEXTLENGTH(x)	(((x)*3)/2)
#define 	UVECPREVLENGTH(x)	(((x)*2)/3)

/* ---------------------------------------------------------------------- */
/* uvec_increase - internal function to increase the size of vector
 * if newcap <= 0 then increase by default size else compute  new capacity
 */
static int uvec_increase(uvec *uv, int newcap) {
	int i;

	newcap = (newcap <= 0 ? UVECNEXTLENGTH(uv->capacity) : newcap);
	if (!(uv->vector = (char **) realloc(uv->vector,
		newcap*sizeof(char *)))) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_increase : realloc error!");
		rkoerrno = RKOMEMERR;
#endif
		return -1;
	}
	uv->capacity = newcap;
	/* zero out extra capacity */
	for (i = uv->number; i < uv->capacity; ++i)
		uv->vector[i] = (char *) NULL;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
/* ---------------------------------------------------------------------- */
/* uvec_decrease - internal function to decrease the size of vector
 * if newcap <= 0 then decrease by default size else compute  new capacity
 */
static int uvec_decrease(uvec *uv, int newcap) {

	newcap = (newcap <= 0 ? UVECPREVLENGTH(uv->capacity) : newcap);
	if (!(uv->vector = (char **) realloc(uv->vector,
		newcap*sizeof(char *)))) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_decrease : realloc error!");
		rkoerrno = RKOMEMERR;
#endif
		return -1;
	}
	uv->capacity = newcap;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

/* uvec_nmalloc - str_alloc & copy a string to element "place"
 * assume no element exists there yet and you have the capacity.
 */
static int uvec_nmalloc(uvec *uv, char const *str, size_t n, int place) {
	if (place < 0 || place >= uv->capacity) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_nmalloc : invalid place!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	if (!(uv->vector[place] = (uv->str_fns.str_alloc)(str,n))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_nmalloc : ");
#endif
		return -2;
	}
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

/* uvec_shift_vec - internal function to shift part of the vector */
/* if start <= 0, then set start=0  (start of vector)
 * if end   <= 0, then set end=number (end of vector)
 * newstart = where to put the range of vector
 * everything between start & newstart is zeroed and assumed to be
 *
 * note 'end' = first element after the last element to move
 */
static int uvec_shift_vec(uvec *uv, int start, int end, int newstart) {
	register int i;
	int n;
	int rstat;

	if (start <= 0) start = 0;
	if (end <= 0) end = uv->number;
	if (newstart < 0) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_shift_vec : invalid newstart!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	if (start > uv->number) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_shift_vec : invalid start!");
		rkoerrno = RKOUSEERR;
#endif
		return -2;
	}
	if (end > uv->number || end < start) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_shift_vec : invalid end!");
		rkoerrno = RKOUSEERR;
#endif
		return -3;
	}
	if (newstart == start) {	/* do nothing */
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return 0;
	}
	if (newstart < start) {			/* shift upwards */
		for (i = newstart; i < start; ++i) {
			/* dealloc elements */
			(uv->str_fns.str_free)(&(uv->vector[i]));
		}
		for (i = start; i < end; ++i) {
			uv->vector[newstart++] = uv->vector[i];
			uv->vector[i] = (char *) NULL;
		}
	} else {				/* shift downwards */
		n = newstart + end - start;
		if (n >= uv->capacity) {
			/* increase capacity */
			if ((rstat = uvec_increase(uv,
				(n>UVECNEXTLENGTH(uv->capacity)
				? n :  UVECNEXTLENGTH(uv->capacity))))) {
#ifdef RKOERROR
				(void) rkopsterror("uvec_shift_vec : ");
#endif
				return rstat - 128;
			}
		}
		for (i = end; i < n; ++i) {
			/* dealloc elements */
			(uv->str_fns.str_free)(&(uv->vector[i]));
			uv->vector[i] = (char *) NULL;
		}
		for (i = end - 1; i >= start; --i) {
			uv->vector[--n] = uv->vector[i];
			uv->vector[i] = (char *) NULL;
		}
	}
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
/* ---------------------------------------------------------------------- */
/* uvec_ninsert - insert an element of length n before element "place"
 */
int uvec_ninsert(uvec *uv, char const *str, size_t n, int place) {
	int rstat;

	if (place < 0 || place > uv->number) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_ninsert : invalid place!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	if ((rstat = uvec_shift_vec(uv, place, 0, place + 1))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_ninsert : ");
#endif
		return rstat - 128;
	}
	if ((rstat = uvec_nmalloc(uv, str, n, place))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_ninsert : ");
#endif
		return rstat - 128;
	}
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	++(uv->number);
	return 0;
}
/* ---------------------------------------------------------------------- */
/* uvec_insert - insert an element before element "place"
 */
int uvec_insert(uvec *uv, char const *str, int place) {

	if (! str ) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_insert : NULL string!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	return uvec_ninsert(uv, str, strlen(str), place);
}

/* ---------------------------------------------------------------------- */
/* uvec_delete - delete an element at element "place"
 */
int uvec_delete(uvec *uv, int place) {
	int rstat;

	if (uv->number <= 0) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_delete : empty vector!");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}
	if (place < 0 || place > uv->number - 1) {
#ifdef RKOERROR
		(void) rkocpyerror("uvec_delete : invalid place!");
		rkoerrno = RKOUSEERR;
#endif
		return -2;
	}
	if ((rstat = uvec_shift_vec(uv, place + 1, 0, place))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_delete : ");
#endif
		return rstat - 128;
	}
	--(uv->number);
	(uv->str_fns.str_free)(&(uv->vector[uv->number]));
	uv->vector[uv->number] = (char *) NULL;

	if (uv->number < uv->capacity/2) {
		if ((rstat = uvec_decrease(uv, 0))) {
#ifdef RKOERROR
			(void) rkopsterror("uvec_delete : ");
#endif
			return rstat - 128;
		}
	}
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}
