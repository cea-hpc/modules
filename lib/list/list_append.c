static const char RCSID[]="@(#)$Id: list_append.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
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
/* list_append - add 1 element after current element in list		*/
/* if current element is NULL then append at end of list		*/
/* -------------------------------------------------------------------- */
int list_append_(list *lst, char const *tag, list_elem *here, va_list vargs) {

	int retval = 0;
	list_elem *eptr = (list_elem *) NULL;

	if (!(eptr = list_add_elem_(lst,tag,vargs))) {
#ifdef RKOERROR
		(void) rkopsterror("list_append_ : ");
#endif
		return -1;
	}

	/* diddle with prev & next ptrs */
	/* append at end if no here element */
	if (!here) here = lst->last;

	if (here) {	/* check again - may be an empty list */
		eptr->prev = here;
		eptr->next = here->next;
		if (here->next) {
			here->next->prev = eptr;
		} else {	/* end of list */
			lst->last = eptr;
		}
		here->next = eptr;
	} else {
		lst->last = eptr;
		lst->first = eptr;
		eptr->prev = (list_elem *) NULL;
		eptr->next = (list_elem *) NULL;
	}

	lst->number++;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return retval;
}

int list_append(list *lst, char const *tag, list_elem *here, ...) {

	int retval = 0;
	va_list vargs;

	va_start(vargs, tag);	/* get ready to pass extra args */

	retval = list_append_(lst, tag, here, vargs);

	va_end(vargs);
	return retval;
}
