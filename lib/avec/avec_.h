/* 
 * RCSID @(#)$Id: avec_.h,v 1.1 2002/07/18 22:03:39 rkowen Exp $
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "avec.h"

/* ---------------------------------------------------------------------- */
/* type of key search */
enum avec_search {AVEC_MATCH, AVEC_NEXT, AVEC_INSERT};

/* avec_hash_search - finds and returns the address in the av->hash
 * if successful, else returns NULL
 * type=AVEC_MATCH  - return the matching hash element else NULL
 * type=AVEC_NEXT   - return the next empty hash element else NULL
 * type=AVEC_INSERT - return the matching or empty hash element else NULL
 */
avec_element **avec_hash_search(enum avec_search type,
		avec *av, char const *key);

/* ---------------------------------------------------------------------- */
/* avec_alloc_hash - does the memory allocation of the hash array
 * returns NULL if an error
 */
avec_element **avec_alloc_hash(int cap);

/* ---------------------------------------------------------------------- */
/* whether delete or remove */
enum avec_delrm {AVEC_DELETE, AVEC_REMOVE};

/* avec_data_delrm - delete or remove an element of the key and returns 0
 * if successful
 * else returns < 0 if an error
 * and only AVEC_DELETE should ever return > 0 to signify the key is not gone
 */
int avec_data_delrm(enum avec_delrm type,
	avec *av, char const *key, va_list vargs);

extern char avec_TAG[5];
extern const avec_element REMOVED;
extern avec_fns default_fns;

#ifdef __cplusplus
	}
#endif
