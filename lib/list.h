#ifndef _LIST_H_
#  define _LIST_H_
/* 
 * RCSID @(#)$Id: list.h,v 1.1 2002/07/29 20:10:32 rkowen Exp $
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#  ifdef __cplusplus
extern "C" {
#  endif

/* generic list ``object'' */

/* warning sizeof(list) does not necessarily give the correct memory size
 * since we make it "expandable" to handle a variable size tag
 */
typedef struct list list;		/* forward declaration */
typedef struct list_elem list_elem;

struct list {
	list_elem *first;		/* head of list */
	list_elem *last;		/* last in list */
	int number;			/* number in list */
	int (*addfn)(void **, va_list);	/* user fn to add user data */
	int (*delfn)(void **, va_list);	/* user fn to del user data */
	char tag[1];			/* name tag for list */
/* followed by expanded memory allocation to contain rest of tag with
 * terminating NULL.  A list object must only be defined as "list *"
 * and set = to list_ctor(TAG,ADDFN,DELFN);
 */
};

struct list_elem {
	list_elem *prev;		/* previous one in list */
	list_elem *next;		/* next one in list */
	void *object;			/* pointer to object of interest */
};

/* shield users from certain internal details */
#define LIST_OBJECT(le)	((le)->object)
#define LIST_NEXT(le)	((le)->next)
#define LIST_PREV(le)	((le)->prev)

list *list_ctor(const char *tag,
	int (addfn)(void **, va_list), int (delfn)(void **, va_list));
int list_dtor(list **lst, char const *tag, ...);

int list_exists(list const *lst, const char *tag);
int list_sizeof(list const *lst, const char *tag);
int list_number(list const *lst, const char *tag);
list_elem *list_first(list const *lst, const char *tag);
list_elem *list_last(list const *lst, const char *tag);

int list_append(list *lst, char const *tag, list_elem *here, ...);
int list_insert(list *lst, char const *tag, list_elem *here, ...);
int list_delete(list *lst, char const *tag, list_elem *here, ...);

int list_unshift(list *lst, char const *tag, ...);
int list_shift(list *lst, char const *tag, ...);
int list_push(list *lst, char const *tag, ...);
int list_pop(list *lst, char const *tag, ...);

int list_swap(list *lst, char const *tag, list_elem *here, list_elem *there);
int list_qsort(list *lst, char const *tag,
		int (*cmp)(const void *, const void *));

#  ifdef __cplusplus
	}
#  endif
#endif /* _LIST_H_ */
