#ifndef _OVEC_H_
#  define _OVEC_H_
/* 
 * RCSID @(#)$Id: ovec.h,v 1.2 2009/10/15 19:09:35 rkowen Exp $
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2009 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: rk@owen.sj.ca.us	or rkowen@nersc.gov		     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/
#  ifdef __cplusplus
extern "C" {
#  endif

#include <stdlib.h>
#include "tcl.h"

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/* Tcl_Obj Vector enums */

enum ovec_order {OVEC_ASCEND, OVEC_DESCEND
#ifdef HAVE_STRCASECMP
	, OVEC_CASE_ASCEND, OVEC_CASE_DESCEND
#endif
	};

/* Tcl_Obj Vector package */

typedef struct {
	char	  tag[5];		/* name tag for this type */
	Tcl_Obj	**objv;			/* vector of Tcl_Obj's */
	int	  objc;			/* current number of list */
	int	  capacity;		/* the possible capacity of vector */
} ovec;

ovec	 *ovec_ctor(int cap);
int	  ovec_dtor(ovec **uv);
int	  ovec_dealloc(ovec **uv);
int	  ovec_close(ovec *uv);

int	  ovec_exists(ovec const *uv);
int	  ovec_capacity(ovec const *uv);
int	  ovec_objc(ovec const *uv);
int	  ovec_end(ovec const *uv);
Tcl_Obj	**ovec_objv(ovec const *uv);

int	  ovec_insert(ovec *uv, Tcl_Obj *str, int place);
int	  ovec_delete(ovec *uv, int place);
int	  ovec_add(ovec *uv, Tcl_Obj *str);
int	  ovec_addl(ovec *uv, ...);
int	  ovec_push(ovec *uv, Tcl_Obj *str);
int	  ovec_pop(ovec *uv);
int	  ovec_unshift(ovec *uv, Tcl_Obj *str);
int	  ovec_shift(ovec *uv);

int	  ovec_sort(ovec *uv, enum ovec_order type);
int	  ovec_qsort(ovec *uv, int (*cmp)(void const *a, void const *b));
ovec	 *ovec2ovec(ovec const *uv);

#  ifdef __cplusplus
	}
#  endif
#endif /* _OVEC_H_ */
