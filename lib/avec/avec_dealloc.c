static const char RCSID[]="@(#)$Id: avec_dealloc.c,v 1.1 2002/07/18 22:03:39 rkowen Exp $";
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
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* ---------------------------------------------------------------------- */
/* avec_dealloc - 
 * Deallocate the avec object (hash array data elements are not touched)
 * but the avec_element's are deallocated hence losing the keys and
 * pointers to the data.  The user should have collected these pointers
 * prior to this call, else there will be a massive memory leak.
 */
int avec_dealloc(avec **av) {
	int retval = 0;
	avec_element **aeptr = (avec_element **) NULL;

	/* walk through hash and free elements (not data) */
	while ((aeptr = avec_walk_r(*av,aeptr))) {
		if ((retval = avec_dealloc_element(aeptr))) {
#ifdef RKOERROR
			(void) rkopsterror("avec_dealloc : ");
			rkoerrno = RKOMEMERR;
#endif
			return retval;
		}
	}
	/* free hash */
	free ((*av)->hash);
	(*av)->hash = (avec_element **) NULL;

	/* clear tag to prevent accidental re-aquisition */
	*((*av)->tag) = '\0';

	/* free avec object */
	free (*av);
	*av = (avec *) NULL;
	return retval;
}

