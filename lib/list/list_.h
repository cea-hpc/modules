/* 
 * RCSID @(#)$Id: list_.h,v 1.1 2002/07/29 20:10:32 rkowen Exp $
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "list.h"

#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* internal routines */

list_elem *list_add_elem_(list const *lst, char const *tag, va_list vargs);
int list_del_elem_(list const *lst, char const *tag,
			list_elem *here,va_list vargs);
int list_append_(list *lst, char const *tag, list_elem *here, va_list vargs);
int list_insert_(list *lst, char const *tag, list_elem *here, va_list vargs);
int list_delete_(list *lst, char const *tag, list_elem *here, va_list vargs);

#ifdef __cplusplus
	}
#endif
