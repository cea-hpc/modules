static const char RCSID[]="@(#)$Id: avec_alloc.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
#include "config.h"
#include "avec.h"
#include "avec_.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_alloc_ - allocate an unitialized avec object and use the
 * given data functions.
 * returns NULL if an error, else the memory location  if OK.
 */
avec *avec_alloc_(avec_fns fns) {
	avec *av = (avec *) NULL;

	if (!(av = (avec *) malloc(sizeof(avec)))) {
#ifdef RKOERROR
		(void) rkocpyerror("avec_alloc_ : avec malloc error!");
		rkoerrno = RKOMEMERR;
#endif
		return av;
	}
	av->fns.data_add = fns.data_add;
	av->fns.data_del = fns.data_del;
	av->fns.data_rm = fns.data_rm;
	*(av->tag) = '\0';
	av->number = 0;
	av->capacity = 0;
	av->percentage = -1;
	return av;
}
/* avec_alloc - allocate an unitialized avec object
 * set to use the default data functions
 * avec_alloc will call avec_alloc_() to set things up.
 */
avec *avec_alloc(void) {
	return avec_alloc_(default_fns);
}

