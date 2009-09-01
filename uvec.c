/* uvec.c -
 *  Collected Unix Vector routines
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001,2009 by R.K.Owen,Ph.D.				     **
 ** last known email: rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

static char Id[]="@(#)$Id: uvec.c,v 1.3 2009/09/02 20:37:39 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "modules_def.h"
#ifdef HAVE_STRMALLOC
#  include "strmalloc.h"
#endif

char uvec_TAG[5] = "UVEC";

/* uvec internal macros */
#define         UVECNEXTLENGTH(x)       (((x)*3)/2)
#define         UVECPREVLENGTH(x)       (((x)*2)/3)


/* ====================================================================== */
/* === uvec_strfns.c === */
/* uvec.c -
 *  uvec_set_def_strfns
 *  register the set of string functions to use by default.
 *
 *  uvec_get_def_strfns
 *  query  as  to which set of string functions are being used
 *  returns a str_fns *
 *
 *  uvec_set_strfns
 *  register the set of string functions to use for the object
 *  overrides the current set ... don't do this if the object is already
 *  fully defined.
 *
 *  uvec_get_strfns
 *  query  as  to which set of string functions
 *  are being used for the object.
 *  returns a str_fns *
 */
/* wrappers for the StdC string functions
 */
static char *stdc_nmalloc (char const *str, size_t n) {
	char *out;
	/* fprintf(stderr, "stdc malloc\n"); */
	if((out = module_calloc((n+1),sizeof(char))))
		(void) strncpy(out, str, n);
	return out;
}
static int stdc_free (char **str) {
	/* fprintf(stderr, "stdc free\n"); */
	null_free((void **) str);
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
int uvec_set_def_strfns(enum uvec_def_str_fns type, uvec_str *strfns) {
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
uvec_str *uvec_get_def_strfns(void) {
	return &default_str_fns;
}

/* set the string functions to use
 */
int uvec_set_strfns(uvec *uv, uvec_str *strfns) {
	if (!strfns) {
		return -1;
	}
	if (uv) {
		uv->str_fns.type = strfns->type;
		uv->str_fns.str_alloc = strfns->str_alloc;
		uv->str_fns.str_free = strfns->str_free;
	} else {
		return -2;
	}
	return 0;
}

/* return what type of string functions are currently default
 */
uvec_str *uvec_get_strfns(uvec *uv) {
	if (uvec_exists(uv)) {
		return &(uv->str_fns);
	} else {
		return (uvec_str *) NULL;
	}
}

/* ====================================================================== */
/* === uvec_ctor.c === */
/* uvec.c -
 *  uvec_ctor_     construct  Unix  vector to capacity ``cap''
 *  using the given set of string functions.
 *
 *  uvec_ctor      construct Unix vector to  capacity  ``cap''
 *  using  the default set of string functions.
 */
/* uvec_ctor_ - construct Unix vector to capacity cap and use the
 * given string functions.
 * returns NULL if an error, else the memory location  if OK.
 * uvec_ctor_ will call uvec_init_() to set things up.
 */
uvec *uvec_ctor_(int cap, uvec_str str_fns) {
	uvec *uv = (uvec *) NULL;

	if ((uvec *) NULL == (uv = uvec_alloc_(str_fns))) {
		return uv;
	}
	if (uvec_init_(uv,cap,str_fns)) {
		(void) uvec_dealloc(&uv);
		uv = (uvec *) NULL;
	}
	return uv;
}

/* uvec_ctor - construct Unix vector to capacity cap
 * uvec_ctor will call uvec_ctor_() to set things up.
 */
uvec *uvec_ctor(int cap) {
	return uvec_ctor_(cap, default_str_fns);
}


/* ====================================================================== */
/* === uvec_dtor.c === */
/* uvec.c -
 *  uvec_dtor      destroy the Unix vector and it's  contents.
 */
/* uvec_dtor - destroy the uvec (calls uvec_close also) */
int uvec_dtor(uvec **uv) {
	int retval = 0;
	if ((retval = uvec_close(*uv))) return retval;
	if ((retval = uvec_dealloc(uv))) return retval;
	return retval;
}


/* ====================================================================== */
/* === uvec_alloc.c === */
/* uvec.c -
 *  uvec_alloc_    allocate  the uninitialized uvec object (no
 *  vector is allocated) using the given set of
 *  string functions.
 *
 *  uvec_alloc     allocate  the uninitialized uvec object (no
 *  vector is allocated) using the default  set
 *  of string functions.
 */
/* uvec_alloc_ - allocate an unitialized uvec object and use the
 * given string functions.
 * returns NULL if an error, else the memory location  if OK.
 */
uvec *uvec_alloc_(uvec_str strfns) {
	uvec *uv = (uvec *) NULL;

	if (!(uv = (uvec *) module_malloc(sizeof(uvec)))) {
		return uv;
	}
	uv->str_fns.str_alloc = strfns.str_alloc;
	uv->str_fns.str_free = strfns.str_free;
	return uv;
}

/* uvec_alloc - allocate an unitialized uvec object
 * set to use the default string functions
 * uvec_alloc will call uvec_alloc_() to set things up.
 */
uvec *uvec_alloc(void) {
	return uvec_alloc_(default_str_fns);
}


/* ====================================================================== */
/* === uvec_dealloc.c === */
/* uvec.c -
 *  uvec_dealloc   Deallocate  the  uvec object (vector is not touched)
 *
 */
/* uvec_dealloc - deallocate the unitialized uvec  */
int uvec_dealloc(uvec **uv) {
	int retval = 0;
	null_free ((void **) uv);
	/* *uv = (uvec *) NULL; */
	return retval;
}

/* ====================================================================== */
/* === uvec_init.c === */
/* uvec.c -
 *  uvec_init_     initialize Unix vector to capacity  ``cap''
 *  using the given set of string functions.
 *
 *  uvec_init      initialize  Unix vector to capacity ``cap''
 *  using the default set of string  functions.
 */
/* uvec_init_ - construct Unix vector to capacity cap use the
 * given string functions.
 * returns <0 if an error, else 0 if OK as well as all the other functions
 */
int uvec_init_(uvec *uv, int cap, uvec_str strfns) {
	if (uv == (uvec *) NULL) {
		return -1;
	}
/* can't guarantee that struct will be initialized to 0 hence use "tag" */
	if (!strncmp(uv->tag,uvec_TAG, 5)) {
		return -2;
	}
	if (cap < 1) {
		return -3;
	}
	if (!(uv->vector = (char **) module_calloc(cap, sizeof(char *)))) {
		return -4;
	}
	(void) strcpy(uv->tag, uvec_TAG);
	uv->capacity = cap;
	uv->number = 0;
	uv->str_fns.type = strfns.type;
	uv->str_fns.str_alloc = strfns.str_alloc;
	uv->str_fns.str_free = strfns.str_free;
	return 0;
}
/* uvec_init_ - construct Unix vector to capacity cap
 * use the default string functions
 * uvec_init will call uvec_init_() to set things up.
 */
int uvec_init(uvec *uv, int cap) {
	return uvec_init_(uv, cap, default_str_fns);
}

/* ====================================================================== */
/* === uvec_close.c === */
/* uvec.c -
 * uvec_close     destroy the Unix vector contents.
 */
/* uvec_close - destroy the uvec contents */
int uvec_close(uvec *uv) {
	int i;

	if (uv == (uvec *) NULL) {
		return -1;
	}
	*(uv->tag) = '\0';
	for (i = 0; i < uv->number; ++i) {
		(uv->str_fns.str_free)(&(uv->vector[i]));
		uv->vector[i] = (char *) NULL;
	}
	null_free((void **) &(uv->vector));
	/* uv->vector = (char **) NULL; */
	uv->capacity = 0;
	uv->number = 0;
	return 0;
}

/* ====================================================================== */
/* === uvec_accessor.c === */
/* uvec.c -
 */
/* accessor functions */
int uvec_exists(uvec const *uv) {
	int retval = 0;
	if (uv == (uvec *) NULL) {
		retval = 0;
	} else {
		if (strncmp(uv->tag,uvec_TAG, 5)) {
			retval = 0;
		} else {
			retval = 1;
		}
	}
	return retval;
}

int uvec_capacity(uvec const *uv) {
	if (uvec_exists(uv)) {
		return uv->capacity;
	} else {
		return -1;
	}
}

int uvec_number(uvec const *uv) {
	if (uvec_exists(uv)) {
		return uv->number;
	} else {
		return -1;
	}
}

int uvec_end(uvec const *uv) {
	if (uvec_exists(uv)) {
		return uv->number - 1;
	} else {
		return -1;
	}
}

char ** uvec_vector(uvec const *uv) {
	if (uvec_exists(uv)) {
		return uv->vector;
	} else {
		return (char **) NULL;
	}
}


/* ====================================================================== */
/* === uvec_insert.c === */
/* uvec.c -
 *  uvec_insert    insert an element before element ``place''.
 *
 *  uvec_ninsert   insert an element of size n before  element ``place''.
 *
 *  uvec_delete    delete an element at element ``place''.
 *
 */
/* uvec_increase - internal function to increase the size of vector
 * if newcap <= 0 then increase by default size else compute  new capacity
 */
static int uvec_increase(uvec *uv, int newcap) {
	int i;

	newcap = (newcap <= 0 ? UVECNEXTLENGTH(uv->capacity) : newcap);
	if (!(uv->vector = (char **) module_realloc(uv->vector,
		newcap*sizeof(char *)))) {
		return -1;
	}
	uv->capacity = newcap;
	/* zero out extra capacity */
	for (i = uv->number; i < uv->capacity; ++i)
		uv->vector[i] = (char *) NULL;
	return 0;
}
/* ---------------------------------------------------------------------- */
/* uvec_decrease - internal function to decrease the size of vector
 * if newcap <= 0 then decrease by default size else compute  new capacity
 */
static int uvec_decrease(uvec *uv, int newcap) {

	newcap = (newcap <= 0 ? UVECPREVLENGTH(uv->capacity) : newcap);
	if (!(uv->vector = (char **) module_realloc(uv->vector,
		newcap*sizeof(char *)))) {
		return -1;
	}
	uv->capacity = newcap;
	return 0;
}

/* uvec_nmalloc - str_alloc & copy a string to element "place"
 * assume no element exists there yet and you have the capacity.
 */
static int uvec_nmalloc(uvec *uv, char const *str, size_t n, int place) {
	if (place < 0 || place >= uv->capacity) {
		return -1;
	}
	if (!(uv->vector[place] = (uv->str_fns.str_alloc)(str,n))) {
		return -2;
	}
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
		return -1;
	}
	if (start > uv->number) {
		return -2;
	}
	if (end > uv->number || end < start) {
		return -3;
	}
	if (newstart == start) {	/* do nothing */
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
	return 0;
}
/* ---------------------------------------------------------------------- */
/* uvec_ninsert - insert an element of length n before element "place"
 */
int uvec_ninsert(uvec *uv, char const *str, size_t n, int place) {
	int rstat;

	if (place < 0 || place > uv->number) {
		return -1;
	}
	if ((rstat = uvec_shift_vec(uv, place, 0, place + 1))) {
		return rstat - 128;
	}
	if ((rstat = uvec_nmalloc(uv, str, n, place))) {
		return rstat - 128;
	}
	++(uv->number);
	return 0;
}
/* ---------------------------------------------------------------------- */
/* uvec_insert - insert an element before element "place"
 */
int uvec_insert(uvec *uv, char const *str, int place) {

	if (! str ) {
		return -1;
	}
	return uvec_ninsert(uv, str, strlen(str)+1, place);
}

/* ---------------------------------------------------------------------- */
/* uvec_delete - delete an element at element "place"
 */
int uvec_delete(uvec *uv, int place) {
	int rstat;

	if (uv->number <= 0) {
		return -1;
	}
	if (place < 0 || place > uv->number - 1) {
		return -2;
	}
	if ((rstat = uvec_shift_vec(uv, place + 1, 0, place))) {
		return rstat - 128;
	}
	--(uv->number);
	(uv->str_fns.str_free)(&(uv->vector[uv->number]));
	uv->vector[uv->number] = (char *) NULL;

	if (uv->number < uv->capacity/2) {
		if ((rstat = uvec_decrease(uv, 0))) {
			return rstat - 128;
		}
	}
	return 0;
}

/* ====================================================================== */
/* === uvec_add.c === */
/* uvec.c -
 *  uvec_add       add one element to end of vector.
 *
 *  uvec_nadd      add one element of size n to end of vector.
 *
 *  uvec_addl      add a NULL terminated list of  elements  to
 *                 end of vector.
 */
/* uvec_add - add 1 element to end of vector */
int uvec_add(uvec *uv, char const *str) {

	if (! str ) {
		return -1;
	}
	return uvec_nadd(uv, str, strlen(str)+1);
}

/* ---------------------------------------------------------------------- */
/* uvec_nadd - add 1 element of size n to end of vector */
int uvec_nadd(uvec *uv, char const *str, size_t n) {
	int rstat;
	if ((rstat = uvec_ninsert(uv, str, n, uv->number))) {
		return rstat - 128;
	}

	return 0;
} 

/* ---------------------------------------------------------------------- */
/* uvec_addl - add a variable number of elements (terminated by a NULL
 *	argument) to end of vector
 */
int uvec_addl(uvec *uv, ...) {
	int rstat;
	const char *str;
	va_list ap;

	va_start(ap, uv);
	while ((str = va_arg(ap, const char *))) {
		if ((rstat = uvec_add(uv, str))) {
			return rstat - 128;
		}
	}
	va_end(ap);
	return 0;
}


/* ====================================================================== */
/* === uvec_push.c === */
/* uvec.c -
 *  uvec_push      same as uvec_add
 *
 *  uvec_pop       pop off one element at end of vector.
 */
/* uvec_push - add 1 element to end of vector (same as uvec_add) */
int uvec_push(uvec *uv, char const *str) {
	return uvec_add(uv,str);
}
/* ---------------------------------------------------------------------- */
/* uvec_pop - pop off 1 element at end of vector
 *	and delete it
 */ 	
int uvec_pop(uvec *uv) {
	int rstat;
	if ((rstat = uvec_delete(uv, uv->number - 1))) {
		return rstat - 128;
	}

	return 0;
}

/* ====================================================================== */
/* === uvec_shift.c === */
/* uvec.c -
 *  uvec_unshift   add one element to start of vector.
 *
 *  uvec_shift     shift one element from start of vector.
 */
/* uvec_unshift - add 1 element to the beginning of vector */
int uvec_unshift(uvec *uv, char const *str) {
	int rstat;

	if (! str ) {
		return -1;
	}
	if ((rstat = uvec_ninsert(uv, str, strlen(str)+1, 0))) {
		return rstat - 128;
	}

	return 0;
}
/* ---------------------------------------------------------------------- */
/* uvec_shift - shift off 1 element at the beginning of vector
 *	and delete it
 */ 	
int uvec_shift(uvec *uv) {
	int rstat;
	if ((rstat = uvec_delete(uv, 0))) {
		return rstat - 128;
	}

	return 0;
}

/* ====================================================================== */
/* === uvec_count_tok.c === */
/* uvec.c -
 *  uvec_copy_str *  create a uvec from a token delimited string.
 *  Tokens escaped with a '\' are not counted.
 */
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


/* ====================================================================== */
/* === uvec_copy_str.c === */
/* uvec.c -
 * uvec_copy_str create a uvec from a token delimited string.
 * Tokens escaped with a '\' are not counted.
 */
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
	size_t fraglen;

	if (num < 0) {
		return -1;
	}

#if 0
	if (! string) {
		return -2;
	}
#endif

	if ((rstat = uvec_init(u, num + 1))) {
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
				return rstat - 128;
			}
			break;
		} else {
			/* add 1 to length and set last char to \0 */
			fraglen = tokptr - ptr;
			uvec_nadd(u, ptr, fraglen + 1);
			ptr = uvec_vector(u)[uvec_end(u)];
			*((char *) ptr + fraglen) = '\0';
		}
		ptr = tokptr + toklen;
	}

	return 0;
}


/* ====================================================================== */
/* === uvec_copy_vec.c === */
/* uvec.c -
 *  uvec_copy_vec  copy   an   existing   char  vector  to  an
 *  unitialized uvec.  Set num to a value  less
 *  than  or equal to 0 to grab the entire vector.
 */
/* uvec_copy_vec - copy an existing vector to an unitialized uvec */
int uvec_copy_vec(uvec *u, char const * const *vec, int number) {
	int rstat;
	char  const * const *ptr = vec;
	int num = 0;
	if (number <= 0) {
/* count number in vector (add 1) */
		while (*ptr++) ++num;
		number = ++num;
	}

	if ((rstat = uvec_init_(u, number,u->str_fns))) {
		return rstat - 128;
	}
	for (ptr = vec; *ptr != (char *)NULL; ++ptr) {
		if ((rstat = uvec_add(u, *ptr))) {
			return rstat - 128;
		}
	}
	return 0;
}


/* ====================================================================== */
/* === uvec_copy.c === */
/* uvec.c -
 *  uvec_copy      copy one uvec to another unitialized one.
 */
/* uvec_copy - copy one uvec to another unitialized one */
int uvec_copy(uvec *u, uvec const *v) {
	int rstat;

	if ((rstat = uvec_copy_vec(u, (char const * const *) uvec_vector(v),
					uvec_capacity(v)))) {
		return rstat - 128;
	}
	return 0;
}


/* ====================================================================== */
/* === uvec_sort.c === */
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
			return -2;
		}
	}
	uvec_qsort(uv, cmp);
	return 0;
}

/* ---------------------------------------------------------------------- */
/* uvec_qsort - sort the vector */

int uvec_qsort(uvec *uv,  int (*cmp)(void const *a, void const *b)) {
	int retval = -1;
	if (uvec_exists(uv) && cmp) {
		qsort((void *) uvec_vector(uv), (size_t) uvec_number(uv),
			sizeof(char *), cmp);
		return 0;
	} else {
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


	if (!uvec_exists(uv)) {
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
	return 0;
}

/* ====================================================================== */
/* === uvec_reverse.c === */
/* uvec.c -
 * uvec_reverse   reverses the element order of the vector.
 */
/* uvec_reverse - reverses the element order of the vector */
int uvec_reverse(uvec const *uv) {
	int num;
	char **ptrstart, **ptrend;
	char *swap;
	if (!uvec_exists(uv)) {
	}
	num = uvec_number(uv);
	if (num > 1) {		/* do reversal */
		ptrstart = uvec_vector(uv);
		ptrend = ptrstart + uvec_end(uv);
		while (ptrstart < ptrend) {
			swap = *ptrend;
			*ptrend-- = *ptrstart;
			*ptrstart++ = swap;
		}
	}
	return 0;
}

/* ====================================================================== */
/* === uvec_randomize.c === */
/* uvec.c -
 *  uvec_randomize randomizes the vector, will  be  repeatable
 *  if  given  the same random number seed.  If
 *  seed <=0 then will "randomly" choose one.
 */
/* wrappers for StdC rand functions to conform to librko - urand
 *	if not available
 */
#if !defined(HAVE_URAND)
static double urand (void) {
	return ((double) rand())/(((double) RAND_MAX) + 1);
}
static void setseed(int seed) {
	srand((unsigned int) seed);
}
#endif

/* ---------------------------------------------------------------------- */
/* uvec_randomize - randomizes the vector  - will be deterministic if
 * use the same seed, However, if seed<=0 then will "randomly"
 * choose one.
 */
int uvec_randomize(uvec const *uv, int seed) {
	char *swap;
	char **vec, **ptr1, **ptr2;
	int num, i;

	if (!uvec_exists(uv)) {
		return -1;
	}
	if (seed <=0) {
		seed = (int) time(NULL);
	}
	if (seed < 0) {
		return -2;
	}
	setseed(seed);
	num = uvec_number(uv);
	vec = uvec_vector(uv);
	/* make 3*number of swaps */
	for (i = 0; i < 3*num; ++i) {
		ptr1 = vec + (int) (num*urand());
		ptr2 = vec + (int) (num*urand());
		swap = *ptr1;
		*ptr1 = *ptr2;
		*ptr2 = swap;
	}
	return 0;
}

/* ====================================================================== */
/* === str2uvec.c === */
/* uvec.c -
 *  str2uvec copies a string to  a  uvec,  breaking  the
 *  string  at  the  given  delimiter, uses the
 *  default string functions for  creating  the uvec.
 *
 */
/* A bunch of conversion routines
 */
/* str2uvec - converts a tokenized string to a uvec
 *	- outputs a uvec object ptr else NULL
 */
uvec *str2uvec(char const *token, char const *string) {
	int num = uvec_count_tok(token,string);
	uvec *tmp = (uvec *) NULL;

	if (num < 0) {
		return tmp;
	}

#if 0
	if (! string) {
		return tmp;
	}
#endif
	if (! token) {
		return tmp;
	}
	if ((uvec *) NULL != (tmp = uvec_alloc())) {
		if (uvec_copy_str(tmp, token, string)) {
			(void) uvec_dealloc(&tmp);
			return (uvec *) NULL;
		}
	}
	return tmp;
}


/* ====================================================================== */
/* === vec2uvec.c === */
/* uvec.c -
 *  vec2uvec copies  a  vector (or part of a vector upto
 *  num elements) to a uvec, uses  the  default
 *  string  functions  for  creating  the uvec.
 *  Set num to a value less than or equal to  0
 *  to grab the entire vector.
 */
/* vec2uvec - converts a vector into a uvec
 *	- outputs a uvec object ptr else NULL
 */
uvec *vec2uvec(char const * const *vec, int num) {
	uvec *tmp = (uvec *) NULL;

	if ((uvec *) NULL != (tmp = uvec_alloc())) {
		if (uvec_copy_vec(tmp, vec, num)) {
			(void) uvec_dealloc(&tmp);
			return (uvec *) NULL;
		}
	}
	return tmp;
}

/* ====================================================================== */
/* === uvec2uvec.c === */
/* uvec.c -
 *   uvec2uvec      copies  a  uvec  to a uvec uses the default
 *                  string functions for creating the uvec.
 */
/* uvec2uvec - copies a uvec into a uvec
 *	- outputs a uvec object ptr else NULL
 */
uvec *uvec2uvec(uvec const *uv) {
	uvec *tmp = (uvec *) NULL;

	if ((uvec *) NULL != (tmp = uvec_alloc())) {
		if (uvec_copy(tmp, uv)) {
			(void) uvec_dealloc(&tmp);
			return (uvec *) NULL;
		}
	}
	return tmp;
}

/* ====================================================================== */
/* === uvec2str.c === */
/* uvec.c -
 *  uvec2str       copies a uvec to a string with the elements
 *  separated  by the given delimeter, uses the
 *  default string functions for  creating  the uvec.
 */
/* uvec2str - copies a uvec into a strmalloc object
 *	- outputs a (char *) ptr else NULL
 */
char *uvec2str(uvec const *uv, char const *token) {
	char **argv;
	char *tmp = (char *) NULL;
	int i,len = 0, toklen = strlen(token);

	if (! token) {
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

/* ====================================================================== */
/* === uvec_strfree.c === */
/* uvec.c -
 *  uvec_strfree       free a uvec2str allocated string
 */
/* uvec_strfree - free a uvec2str allocated string
 *	- returns 0 if OK
 */
int uvec_strfree(uvec const *uv, char **str) {
	int retval = 0;

	/* no string to dealloc */
	if (! str) return retval;
	
	if (uvec_exists(uv)) {
		/* free string */
		if ((retval=(uv->str_fns.str_free)(str))) {
		}
	} else {
		retval = -1;
	}
	return retval;
}

