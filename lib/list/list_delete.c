static const char RCSID[]="@(#)$Id: list_delete.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
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
/* list_delete - remove the given element element from list		*/
/* -------------------------------------------------------------------- */
int list_delete_(list *lst, char const *tag, list_elem *here, va_list vargs) {

	int retval = 0;
	list_elem *eptr = (list_elem *) NULL;
	list_elem *nptr = (list_elem *) NULL;

	if (!here) {
#ifdef RKOERROR
		(void) rkocpyerror("list_delete_ : NULL element");
		rkoerrno = RKOUSEERR;
#endif
		return -1;
	}

	/* get current pointers */
	eptr = here->prev;
	nptr = here->next;

	/* delete element */
	if ((retval = list_del_elem_(lst,tag,here,vargs))) {
#ifdef RKOERROR
		(void) rkopsterror("list_delete_ : ");
#endif
		return retval;
	}

	/* fix up pointers */
	if (eptr)	eptr->next = nptr;
	else		lst->first = nptr;
	if (nptr)	nptr->prev = eptr;
	else		lst->last  = eptr;

	lst->number--;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
unwind:
	return retval;
}

int list_delete(list *lst, char const *tag, list_elem *here, ...) {

	int retval = 0;
	va_list vargs;

	va_start(vargs, tag);	/* get ready to pass extra args */

	retval = list_delete_(lst, tag, here, vargs);

	va_end(vargs);
	return retval;
}
