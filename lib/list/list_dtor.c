static const char RCSID[]="@(#)$Id: list_dtor.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
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
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif

/* ---------------------------------------------------------------------- */
/* list_dtor - destroy the list object (needs user delfn if not empty).
 */
int list_dtor(list **lst, char const *tag, ...) {
	int retval = 0;
	list_elem *eptr = (list_elem *) NULL;
	list_elem *nptr = (list_elem *) NULL;
	void *ptr = (void *) NULL;
	va_list args;

	if (*lst == (void *) NULL) return 0;	/* already nulled */
	if (!list_exists(*lst, tag)) {
#ifdef RKOERROR
		(void) rkopsterror("list_dtor : ");
#endif
		return -1;
	}

	if ((*lst)->first != (void *) NULL && (*lst)->last != (void *) NULL) {
		eptr = (*lst)->first;
		va_start(args, tag);	/* get ready to pass extra args */
		while ((eptr != (list_elem *) NULL)) {
			nptr = eptr->next;
			if (list_del_elem_(*lst,tag,eptr, args)) {
#ifdef RKOERROR
				(void) rkopsterror(
					"list_dtor : ");
#endif
				goto unwind;
			}
			eptr = nptr;
		}
	}
	(*lst)->first = (*lst)->last = (list_elem *) NULL;
	(*lst)->number = -1;
	(*lst)->tag[0] = '\0';
	free (*lst);
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
unwind: va_end(args);
	return retval;
}

