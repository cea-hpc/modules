/* ovec.c -
 *  Collected Tcl objv Vector routines
 *  Why the Tcl/Tk developers don't provide this? ... 
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2009 by R.K.Owen,Ph.D.					     **
 ** last known email: rk@owen.sj.ca.us or rkowen@nersc.gov		     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

static char Id[]="@(#)$Id$";
static void *UseId[] = { &UseId, Id };

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "modules_def.h"

char ovec_TAG[5] = "OVEC";

/* ovec internal macros */
#define         OVECNEXTLENGTH(x)       (((x)*3)/2)
#define         OVECPREVLENGTH(x)       (((x)*2)/3)

/* ====================================================================== */
/* === ovec_ctor.c === */
/* ovec.c -
 *  ovec_ctor      construct Tcl_Obj vector to  capacity  ``cap''
 *  using  the default set of string functions.
 * returns NULL if an error, else the memory location  if OK.
 * ovec_ctor will call ovec_init() to set things up.
 */
ovec *ovec_ctor(int cap) {
	ovec *uv = (ovec *) NULL;

	if (cap < 0)
		goto unwind0;
	if (!cap)
		cap = 11;	/* some default value */

	/* create vector object */
	if ((uv = (ovec *) module_malloc(sizeof(ovec)))) {
		/* fill it out */
		(void) strcpy(uv->tag, ovec_TAG);
		uv->capacity = cap;
		uv->objc = 0;
		if (!(uv->objv = (Tcl_Obj **) module_calloc(cap,
					sizeof(Tcl_Obj *))))
			goto unwind1;
	}

	return uv;

unwind1:
	null_free((void *) &uv);
unwind0:
	return (ovec *) NULL;
}

/* ====================================================================== */
/* ovec.c -
 *  ovec_dtor      destroy the Unix vector and it's  contents.
 */
/* ovec_dtor - destroy the ovec (calls ovec_close also) */
int ovec_dtor(ovec **uv) {
	int retval = 0;
	if ((retval = ovec_close(*uv))) return retval;
	if ((retval = ovec_dealloc(uv))) return retval;
	return retval;
}


/* ====================================================================== */
/* === ovec_dealloc.c === */
/* ovec.c -
 *  ovec_dealloc   Deallocate  the  ovec object (vector is not touched)
 *
 */
/* ovec_dealloc - deallocate the unitialized ovec  */
int ovec_dealloc(ovec **uv) {
	int retval = 0;
	null_free ((void **) uv);
	/* *uv = (ovec *) NULL; */
	return retval;
}

/* ====================================================================== */
/* ovec.c -
 * ovec_close     decrement the Tcl_Obj vector contents.
 */
int ovec_close(ovec *uv) {
	int i;

	if (uv == (ovec *) NULL) {
		return -1;
	}
	*(uv->tag) = '\0';
	/* decrement the reference count for each */
	for (i = 0; i < uv->objc; ++i) {
		Tcl_DecrRefCount(uv->objv[i]);
		uv->objv[i] = (Tcl_Obj *) NULL;
	}
	null_free((void **) &(uv->objv));
	/* uv->objv = (Tcl_Obj **) NULL; */
	uv->capacity = 0;
	uv->objc = 0;
	return 0;
}

/* ====================================================================== */
/* ovec.c -
 */
/* accessor functions */
int ovec_exists(ovec const *uv) {
	int retval = 0;
	if (uv == (ovec *) NULL) {
		retval = 0;
	} else {
		if (strncmp(uv->tag,ovec_TAG, 5)) {
			retval = 0;
		} else {
			retval = 1;
		}
	}
	return retval;
}

int ovec_capacity(ovec const *uv) {
	if (ovec_exists(uv)) {
		return uv->capacity;
	} else {
		return -1;
	}
}

int ovec_objc(ovec const *uv) {
	if (ovec_exists(uv)) {
		return uv->objc;
	} else {
		return -1;
	}
}

int ovec_end(ovec const *uv) {
	if (ovec_exists(uv)) {
		return uv->objc - 1;
	} else {
		return -1;
	}
}

Tcl_Obj ** ovec_objv(ovec const *uv) {
	if (ovec_exists(uv)) {
		return uv->objv;
	} else {
		return (Tcl_Obj **) NULL;
	}
}

/* ====================================================================== */
/* ovec.c -
 *  ovec_insert    insert an element before element ``place''.
 *
 *  ovec_delete    delete an element at element ``place''.
 *
 */
/* ovec_increase - internal function to increase the size of vector
 * if newcap <= 0 then increase by default size else compute  new capacity
 */
static int ovec_increase(ovec *uv, int newcap) {
	int i;

	newcap = (newcap <= 0 ? OVECNEXTLENGTH(uv->capacity) : newcap);
	if (!(uv->objv = (Tcl_Obj **) module_realloc(uv->objv,
		newcap*sizeof(Tcl_Obj *)))) {
		return -1;
	}
	uv->capacity = newcap;
	/* zero out extra capacity */
	for (i = uv->objc; i < uv->capacity; ++i)
		uv->objv[i] = (Tcl_Obj *) NULL;
	return 0;
}
/* ---------------------------------------------------------------------- */
/* ovec_decrease - internal function to decrease the size of vector
 * if newcap <= 0 then decrease by default size else compute  new capacity
 */
static int ovec_decrease(ovec *uv, int newcap) {

	newcap = (newcap <= 0 ? OVECPREVLENGTH(uv->capacity) : newcap);
	if (!(uv->objv = (Tcl_Obj **) module_realloc(uv->objv,
		newcap*sizeof(Tcl_Obj *)))) {
		return -1;
	}
	uv->capacity = newcap;
	return 0;
}

/* ovec_malloc - copy a Tcl_Obj ptr to element "place"
 * assume no element exists there yet and you have the capacity.
 * increment RefCount for Tcl_Obj (since this is another reference)
 */
static int ovec_malloc(ovec *uv, Tcl_Obj *str, int place) {
	if (place < 0 || place >= uv->capacity)
		return -1;
	if (!(uv->objv[place] = str))
		return -2;

	Tcl_IncrRefCount(uv->objv[place]);
	return 0;
}

/* ovec_shift_vec - internal function to shift part of the vector */
/* if start <= 0, then set start=0  (start of vector)
 * if end   <= 0, then set end=objc (end of vector)
 * newstart = where to put the range of vector elements
 * everything between start & newstart is zeroed and assumed to be removable
 *
 * note 'end' = first element after the last element to move
 */
static int ovec_shift_vec(ovec *uv, int start, int end, int newstart) {
	register int i;
	int n;
	int rstat;

	if (start <= 0) start = 0;
	if (end <= 0) end = uv->objc;
	if (newstart < 0)
		return -1;
	if (start > uv->objc)
		return -2;
	if (end > uv->objc || end < start)
		return -3;
	if (newstart == start)			/* do nothing */
		return 0;

	if (newstart < start) {			/* shift downwards */
		for (i = newstart; i < start; ++i) {
			/* zero elements */
			if (uv->objv[i])
				Tcl_DecrRefCount(uv->objv[i]);
			uv->objv[i] = (Tcl_Obj *) NULL;
		}
		for (i = start; i < end; ++i) {
			uv->objv[newstart++] = uv->objv[i];
			uv->objv[i] = (Tcl_Obj *) NULL;
		}
	} else {				/* shift upwards */
		n = newstart + end - start;
		if (n >= uv->capacity) {
			/* increase capacity */
			if ((rstat = ovec_increase(uv,
				(n>OVECNEXTLENGTH(uv->capacity)
				? n :  OVECNEXTLENGTH(uv->capacity))))) {
				return rstat - 128;
			}
		}
		for (i = end; i < n; ++i) {
			/* zero elements */
			if (uv->objv[i])
				Tcl_DecrRefCount(uv->objv[i]);
			uv->objv[i] = (Tcl_Obj *) NULL;
		}
		for (i = end - 1; i >= start; --i) {
			uv->objv[--n] = uv->objv[i];
			uv->objv[i] = (Tcl_Obj *) NULL;
		}
	}
	return 0;
}
/* ---------------------------------------------------------------------- */
/* ovec_insert - insert a Tcl_Obj before element "place"
 */
int ovec_insert(ovec *uv, Tcl_Obj *str, int place) {
	int rstat;

	if (place < 0 || place > uv->objc)
		return -1;

	if ((rstat = ovec_shift_vec(uv, place, 0, place + 1)))
		return rstat - 128;

	if ((rstat = ovec_malloc(uv, str, place)))
		return rstat - 128;

	++(uv->objc);
	return 0;
}
/* ---------------------------------------------------------------------- */
/* ovec_delete - delete an element at element "place"
 */
int ovec_delete(ovec *uv, int place) {
	int rstat;

	if (uv->objc <= 0)
		return -1;

	if (place < 0 || place > uv->objc - 1)
		return -2;

	if ((rstat = ovec_shift_vec(uv, place + 1, 0, place)))
		return rstat - 128;

	--(uv->objc);
	if(uv->objv[uv->objc])
		Tcl_DecrRefCount(uv->objv[uv->objc]);
	uv->objv[uv->objc] = (Tcl_Obj *) NULL;

	if (uv->objc < uv->capacity/2) {
		if ((rstat = ovec_decrease(uv, 0))) {
			return rstat - 128;
		}
	}
	return 0;
}

/* ====================================================================== */
/* === ovec_add.c === */
/* ovec.c -
 *  ovec_add       add one element to end of vector.
 *
 *  ovec_addl      add a NULL terminated list of  elements  to
 *                 end of vector.
 */
/* ---------------------------------------------------------------------- */
/* ovec_add - add 1 element to end of vector */
int ovec_add(ovec *uv, Tcl_Obj *str) {
	int rstat;
	if ((rstat = ovec_insert(uv, str, ovec_objc(uv))))
		return rstat - 128;

	return 0;
} 

/* ---------------------------------------------------------------------- */
/* ovec_addl - add a variable number of elements (terminated by a NULL
 *	argument) to end of vector
 */
int ovec_addl(ovec *uv, ...) {
	int rstat;
	Tcl_Obj *str;
	va_list ap;

	va_start(ap, uv);
	while ((str = va_arg(ap, Tcl_Obj *))) {
		if ((rstat = ovec_add(uv, str))) {
			return rstat - 128;
		}
	}
	va_end(ap);
	return 0;
}


/* ====================================================================== */
/* === ovec_push.c === */
/* ovec.c -
 *  ovec_push      same as ovec_add
 *
 *  ovec_pop       pop off one element at end of vector.
 */
/* ovec_push - add 1 element to end of vector (same as ovec_add) */
int ovec_push(ovec *uv, Tcl_Obj *str) {
	return ovec_add(uv,str);
}
/* ---------------------------------------------------------------------- */
/* ovec_pop - pop off 1 element at end of vector
 *	and delete it
 */ 	
int ovec_pop(ovec *uv) {
	int rstat;
	if ((rstat = ovec_delete(uv, uv->objc - 1)))
		return rstat - 128;

	return 0;
}

/* ====================================================================== */
/* === ovec_shift.c === */
/* ovec.c -
 *  ovec_unshift   add one element to start of vector.
 *
 *  ovec_shift     shift one element from start of vector.
 */
/* ovec_unshift - add 1 element to the beginning of vector */
int ovec_unshift(ovec *uv, Tcl_Obj *str) {
	int rstat;

	if (! str )
		return -1;

	if ((rstat = ovec_insert(uv, str, 0)))
		return rstat - 128;

	return 0;
}
/* ---------------------------------------------------------------------- */
/* ovec_shift - shift off 1 element at the beginning of vector
 *	and delete it
 */ 	
int ovec_shift(ovec *uv) {
	int rstat;
	if ((rstat = ovec_delete(uv, 0)))
		return rstat - 128;

	return 0;
}

/* ====================================================================== */
/* ovec.c -
 *  ovec_sort sort the vector, given the following types:
 *  OVEC_ASCEND, OVEC_DESCEND,
 *  OVEC_CASE_ASCEND, OVEC_CASE_DESCEND,  where
 *  the last two are only available if the strcasecmp
 *  function is available  for  ``caseless'' string comparisons.
 *
 *  ovec_qsort sort the vector, according to the passed comparison function,
 *  which is of the same type as needed by qsort()
 *  namely int cmp(void const *a, void const *b)
 */
/* comparison functions */

static int ovec_sort_cmp_ascend(void const *a, void const *b) {
	return strcmp(  Tcl_GetString(*(Tcl_Obj **)a),
			Tcl_GetString(*(Tcl_Obj **)b));
}

static int ovec_sort_cmp_descend(void const *a, void const *b) {
	return strcmp(  Tcl_GetString(*(Tcl_Obj **)b),
			Tcl_GetString(*(Tcl_Obj **)a));
}

#ifdef HAVE_STRCASECMP
static int ovec_sort_cmp_case_ascend(void const *a, void const *b) {
	return strcasecmp(Tcl_GetString(*(Tcl_Obj **)a),
			  Tcl_GetString(*(Tcl_Obj **)b));
}

static int ovec_sort_cmp_case_descend(void const *a, void const *b) {
	return strcasecmp(Tcl_GetString(*(Tcl_Obj **)b),
			  Tcl_GetString(*(Tcl_Obj **)a));
}
#endif /* HAVE_STRCASECMP */

/* ---------------------------------------------------------------------- */
/* ovec_sort - sort the vector */

int ovec_sort(ovec *uv, enum ovec_order type) {
	int (*cmp)(void const *, void const *);
	if (!ovec_exists(uv)) {
		return -1;
	}
	if (ovec_objc(uv) > 1) {
		switch (type) {
		case OVEC_ASCEND:
			cmp = ovec_sort_cmp_ascend;
			break;
		case OVEC_DESCEND:
			cmp = ovec_sort_cmp_descend;
			break;
#ifdef HAVE_STRCASECMP
		case OVEC_CASE_ASCEND:
			cmp = ovec_sort_cmp_case_ascend;
			break;
		case OVEC_CASE_DESCEND:
			cmp = ovec_sort_cmp_case_descend;
			break;
#endif /* HAVE_STRCASECMP */
		default:
			return -2;
		}
	}
	ovec_qsort(uv, cmp);
	return 0;
}

/* ---------------------------------------------------------------------- */
/* ovec_qsort - sort the vector */

int ovec_qsort(ovec *uv,  int (*cmp)(void const *a, void const *b)) {
	int retval = -1;
	if (ovec_exists(uv) && cmp) {
		qsort((void *) ovec_objv(uv), (size_t) ovec_objc(uv),
			sizeof(Tcl_Obj *), cmp);
		return 0;
	} else {
		return retval;
	}
}

/* ====================================================================== */
/* ovec.c -
 * ovec2ovec - copies a ovec into a ovec
 *	- outputs a ovec object ptr else NULL
 */
ovec *ovec2ovec(ovec const *uv) {
	ovec *tmp = (ovec *) NULL;
	Tcl_Obj	**ptrin  = ovec_objv(uv),
		**ptrout;

	if ((tmp = ovec_ctor(ovec_objc(uv)+1))) {
		ptrout = ovec_objv(tmp);
		while (ptrin && *ptrin) {
			*ptrout = Tcl_DuplicateObj(*ptrin++);
			Tcl_IncrRefCount(*ptrout);
			ptrout++;
		}
		*ptrout = (Tcl_Obj *) NULL;
	}
	tmp->objc = uv->objc;

	return tmp;
}

