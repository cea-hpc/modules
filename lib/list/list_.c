static const char RCSID[]="@(#)$Id: list_.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
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

/* -------------------------------------------------------------------- */
/* list_add_elem_ - create a add element calling the user addfn		*/
/*	returns NULL if an error					*/
/* -------------------------------------------------------------------- */
list_elem *list_add_elem_(const list *lst, char const *tag, va_list vargs) {

	int retval;
	void *ptr = (void *) NULL;
	list_elem *eptr = (list_elem *) NULL;

	if (!list_exists(lst, tag)) {
#ifdef RKOERROR
		(void) rkopsterror("list_add_elem_ : ");
#endif
		return eptr;
	}

	if ((retval = (lst->addfn)(&ptr, vargs)) != 0) {
#ifdef RKOERROR
		(void) rkocpyerror("list_add_elem_ : user function error!");
		rkoerrno = RKOMEMERR;
#endif
		goto unwind;
	}
	if ((eptr = (list_elem *)malloc(sizeof(list_elem)))
	== (list_elem *) NULL) {
#ifdef RKOERROR
		(void) rkocpyerror("list_add_elem_ : malloc error!");
		rkoerrno = RKOMEMERR;
#endif
		(void) (lst->delfn)(&ptr, vargs);
		goto unwind;
	}
	/* diddle with structure values */
	eptr->object = ptr;
	eptr->prev = (list_elem *) NULL;
	eptr->next = (list_elem *) NULL;
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
unwind:
	return eptr;
}

/* -------------------------------------------------------------------- */
/* list_del_elem_ - remove an element calling the user delfn		*/
/*	returns non-zero if an error					*/
/* -------------------------------------------------------------------- */
int list_del_elem_(const list *lst, char const *tag,
			list_elem *here, va_list vargs) {

	int retval;

	if (!list_exists(lst, tag)) {
#ifdef RKOERROR
		(void) rkopsterror("list_del_elem_ : ");
#endif
		return -1;
	}

	if ((retval = (lst->delfn)(&(here->object), vargs)) != 0) {
#ifdef RKOERROR
		(void) rkocpyerror("list_del_elem_ : user function error!");
		rkoerrno = RKOMEMERR;
#endif
		goto unwind;
	}
	here->object = (void *) NULL;
	here->prev = (list_elem *) NULL;
	here->next = (list_elem *) NULL;
	free(here);
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
unwind:
	return retval;
}

