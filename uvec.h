#ifndef _UVEC_H_
#  define _UVEC_H_
/* 
 * RCSID @(#)$Id: uvec.h,v 1.3 2009/09/02 20:37:39 rkowen Exp $
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001,2009 by R.K.Owen,Ph.D.	                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/
/* define HAVE_STRCASECMP if function exists
 */
#  ifdef __cplusplus
extern "C" {
#  endif

#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/* Unix Vector enums */

enum uvec_order {UVEC_ASCEND, UVEC_DESCEND
#ifdef HAVE_STRCASECMP
	, UVEC_CASE_ASCEND, UVEC_CASE_DESCEND
#endif
	};

enum uvec_def_str_fns {UVEC_DEFAULT, UVEC_USER, UVEC_STDC
#ifdef HAVE_STRMALLOC
	, UVEC_STRMALLOC
#endif
	};

/* Unix Vector package string functions */

typedef struct {
	/* store the string function type - user can ignore this */
	enum uvec_def_str_fns type;
	/* allocate a string of n+1 chars and put str in it */
	char	*(*str_alloc)(char const *str, size_t n);
	/* free up the string storage */
	int	 (*str_free)(char **str);
} uvec_str;

/* Unix Vector package */

typedef struct {
	char	  tag[5];		/* name tag for this type */
	char	**vector;		/* vector of strings */
	int	  number;		/* current number of list */
	int	  capacity;		/* the possible capacity of vector */
	uvec_str  str_fns;		/* which string functions to use */
} uvec;

int       uvec_set_def_strfns(enum uvec_def_str_fns type, uvec_str *strfns);
uvec_str *uvec_get_def_strfns(void);
int       uvec_set_strfns(uvec *uv, uvec_str *strfns);
uvec_str *uvec_get_strfns(uvec *uv);


uvec  *uvec_ctor_(int cap, uvec_str strfns);
uvec  *uvec_ctor(int cap);
int    uvec_dtor(uvec **uv);
uvec  *uvec_alloc_(uvec_str strfns);
uvec  *uvec_alloc(void);
int    uvec_dealloc(uvec **uv);
int    uvec_init_(uvec *uv, int cap, uvec_str strfns);
int    uvec_init(uvec *uv, int cap);
int    uvec_close(uvec *uv);

int    uvec_exists(uvec const *uv);
int    uvec_capacity(uvec const *uv);
int    uvec_number(uvec const *uv);
int    uvec_end(uvec const *uv);
char **uvec_vector(uvec const *uv);

int    uvec_ninsert(uvec *uv, char const *str, size_t n, int place);
int    uvec_insert(uvec *uv, char const *str, int place);
int    uvec_delete(uvec *uv, int place);
int    uvec_nadd(uvec *uv, char const *str, size_t n);
int    uvec_add(uvec *uv, char const *str);
int    uvec_addl(uvec *uv, ...);
int    uvec_push(uvec *uv, char const *str);
int    uvec_pop(uvec *uv);
int    uvec_unshift(uvec *uv, char const *str);
int    uvec_shift(uvec *uv);

int    uvec_count_tok(char const *token, char const *string);
int    uvec_copy_str(uvec *u, char const *token, char const *string);
int    uvec_copy_vec(uvec *u, char const * const *vec, int number);
int    uvec_copy(uvec *u, uvec const *v);
int    uvec_sort(uvec *uv, enum uvec_order type);
int    uvec_qsort(uvec *uv, int (*cmp)(void const *a, void const *b));
int    uvec_find(uvec *uv, char const *str, enum uvec_order type);
int    uvec_uniq(uvec *uv, enum uvec_order type);
int    uvec_reverse(uvec const *uv);
int    uvec_randomize(uvec const *uv, int seed);
uvec  *str2uvec(char const *token, char const *string);
uvec  *vec2uvec(char const * const *vec, int num);
uvec  *uvec2uvec(uvec const *uv);
char  *uvec2str(uvec const *uv, char const *token);
int    uvec_strfree(uvec const *uv, char **str);

#  ifdef __cplusplus
	}
#  endif
#endif /* _UVEC_H_ */
