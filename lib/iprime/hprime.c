static const char RCSID[]="@(#)$Id: hprime.c,v 1.1 2002/07/18 22:07:50 rkowen Exp $";
static const char AUTHOR[]="@(#)iprime 1.0 1999/09/09 R.K.Owen,Ph.D.";
/* iprime.c -
 * a simple minded routine to find out whether a positive number is prime.
 * returns 1 if prime, else it returns the smallest factor > 1 if not.
 * An invalid value causes the function to return 0;
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1999 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include "iprime/iprime_.h"

/* just like templates - almost */
_IPRIME(hprime, short, hsqrt)

