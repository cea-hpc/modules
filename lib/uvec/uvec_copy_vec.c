static const char RCSID[]="@(#)$Id: uvec_copy_vec.c,v 1.1 2002/07/18 22:20:42 rkowen Exp $";
static const char AUTHOR[]="@(#)uvec 1.1 10/31/2001 R.K.Owen,Ph.D.";
/* uvec.c -
 *  uvec_copy_vec  copy   an   existing   char  vector  to  an
 *  unitialized uvec.  Set num to a value  less
 *  than  or equal to 0 to grab the entire vector.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include "uvec.h"
#ifdef RKOERROR
#  include "rkoerror.h"
#endif

/* uvec_copy_vec - copy an existing vector to an unitialized uvec */
int uvec_copy_vec(uvec *u, char const * const *vec, int number) {
	int rstat;
	char  const * const *ptr = vec;
	int num = 0;
	if (number <= 0) {
/* count number in vector (add 1) */
		while (*ptr++) ++num;
		number = ++num;
	}

	if ((rstat = uvec_init(u, number))) {
#ifdef RKOERROR
		(void) rkopsterror("uvec_copy_vec : ");
#endif
		return rstat - 128;
	}
	for (ptr = vec; *ptr != (char *)NULL; ++ptr) {
		if ((rstat = uvec_add(u, *ptr))) {
#ifdef RKOERROR
			(void) rkopsterror("uvec_copy_vec : ");
#endif
			return rstat - 128;
		}
	}
	return 0;
}

