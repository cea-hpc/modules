static const char RCSID[]="@(#)$Id: avec_lookup.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
static const char AUTHOR[]="@(#)avec 1.0 2002/02/08 R.K.Owen,Ph.D.";
/* avec.c -
 * This could have easily been made a C++ class, but is
 * confined to C so that it can be maximally portable.
 *
 * This is an associative vector package (a hash array)
 * that uses quadratic hashing.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2002 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <stdlib.h>
#include "config.h"
#include "avec.h"
#include "avec_.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_lookup - returns a pointer to the data associated with the key
 * else returns NULL
 */
void *avec_lookup(avec *av, char const *key) {
	avec_element	**elem;		/* place to insert value */
	void *retval = (void *) NULL;

	if (!(elem = avec_hash_search(AVEC_MATCH, av,key))) {
#ifdef RKOERROR
		rkopsterror("avec_lookup : ");
#endif
		return retval;
	}
	return (*elem)->data;
}

