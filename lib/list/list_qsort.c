static const char RCSID[]="@(#)$Id: list_qsort.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
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
#include "list_.h"
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif

/* ---------------------------------------------------------------------- */
/* list_qsort - sorts the list according to the comparison function
 * see qsort for details.
 */
int list_qsort(list *lst, char const *tag,
		int (*cmp)(const void *, const void *)) {

	list_elem *eptr, *nptr;
	void **obj;
	void **obj_arr;

	if (!list_exists(lst, tag)) {
#ifdef RKOERROR
		(void) rkopsterror("list_qsort : ");
#endif
		return -1;
	}

	if (lst->number < 1) {	/* nothing to sort */
		return 0;
	}
	/* allocate array for objects */
	if (!(obj_arr = (void **) malloc(sizeof(void *) * (lst->number)))) {
#ifdef RKOERROR
		(void) rkocpyerror("list_qsort : object array malloc error");
#endif
		return -2;
	}
	/* fill array with object ptrs */
	eptr = lst->first;
	obj = obj_arr;
	while ((eptr != (list_elem *) NULL)) {
		nptr = eptr->next;
		*obj++ = eptr->object;
		eptr = nptr;
	}
	/* use StdC::qsort */
	qsort((void *) obj_arr, lst->number, sizeof(void *), cmp);

	/* put objects from obj_arr back into list */
	eptr = lst->first;
	obj = obj_arr;
	while ((eptr != (list_elem *) NULL)) {
		nptr = eptr->next;
		eptr->object = *obj++;
		eptr = nptr;
	}
	
	/* free up obj_arr */
	free (obj_arr);
#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

