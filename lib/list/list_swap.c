static const char RCSID[]="@(#)$Id: list_swap.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
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
/* list_swap - swap any two elements (objects)				*/
/* -------------------------------------------------------------------- */
int list_swap(list *lst, char const *tag,
			list_elem *here, list_elem *there) {

	void *tobject;

	if (!list_exists(lst, tag)) {
#ifdef RKOERROR
		(void) rkopsterror("list_swap : ");
#endif
		return -1;
	}

	if (!here || !there) {
#ifdef RKOERROR
		rkocpyerror("list_swap : NULL swap elements");
		rkoerrno = RKOUSEERR;
#endif
		return -2;
	}

	/* swap objects - not elements!
	 * this avoids overlap problems in the prev/next/first/last ptrs
	 */
	tobject = here->object;
	here->object = there->object;
	there->object = tobject;

#ifdef RKOERROR
	rkoerrno = RKO_OK;
#endif
	return 0;
}

