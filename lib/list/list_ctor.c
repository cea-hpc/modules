static const char RCSID[]="@(#)$Id: list_ctor.c,v 1.1 2002/07/29 20:10:32 rkowen Exp $";
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

/* warning sizeof(list) does not necessarily give the correct memory size
 * since we make it "expandable" to handle a variable size tag
 */
/* ---------------------------------------------------------------------- */
/* list_ctor - construct a "list" object
 * returns NULL if an error, else the memory location  if OK.
 */
list *list_ctor(const char *tag,
int (addfn)(void **, va_list), int (delfn)(void **, va_list)) {

	list *lst = (list *) NULL;

	if (!(lst = (list *) malloc(sizeof(list)+sizeof(tag)))) {
#ifdef RKOERROR
		(void) rkocpyerror("list_ctor : list malloc error!");
		rkoerrno = RKOMEMERR;
#endif
		return lst;
	}
	/* copy in tag to structure and initialize pointers */
	lst->first = (void *) NULL;
	lst->last = (void *) NULL;
	lst->number = 0;
	lst->addfn = addfn;
	lst->delfn = delfn;
	(void) strcpy(lst->tag, tag);

	return lst;
}
