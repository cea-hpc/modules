static const char RCSID[]="@(#)$Id: list_number.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
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

#include "list_.h"

int list_number(list const *lst, const char *tag) {
	if (list_exists(lst, tag)) {
#ifdef RKOERROR
		rkoerrno = RKO_OK;
#endif
		return lst->number;
	} else {
#ifdef RKOERROR
		(void) rkopsterror("list_number : ");
#endif
		return -1;
	}
}

