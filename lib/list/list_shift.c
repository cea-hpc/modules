static const char RCSID[]="@(#)$Id: list_shift.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
static const char AUTHOR[]="@(#)list 1.0 1998/10/31 R.K.Owen,Ph.D.";
/* list.c -
 * This could have easily been made a C++ class, but is
 * confined to C for easy portability.
 *
 * a "list" is similar to the C++ STL deque, but implemented
 * using (void *), and where the user needs to create & pass
 * certain helper functions.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1998 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <stdlib.h>
#include <stdarg.h>
#include "list_.h"

/* -------------------------------------------------------------------- */
/* list_shift - delete off 1 element at the beginning of list 		*/
/* -------------------------------------------------------------------- */
int list_shift(list *lst, char const *tag, ...) {
	int retval = 0;
	va_list args;

	if (!list_exists(lst, tag)) {
#ifdef RKOERROR
		(void) rkopsterror("list_shift : ");
#endif
		return -1;
	}

	if (lst->last == (list_elem *) NULL) {		/* already empty */
		return 1;				/* not an error */
	}

	va_start(args, tag);	/* get ready to pass extra args */

	if ((retval = list_delete_(lst,tag, lst->first, args))) {
#ifdef RKOERROR
		(void) rkopsterror("list_shift : ");
#endif
		return retval;
	}

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	va_end(args);
	return retval;
}
