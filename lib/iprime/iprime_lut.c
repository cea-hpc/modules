static const char RCSID[]="@(#)$Id: iprime_lut.c,v 1.1 2002/07/18 22:07:50 rkowen Exp $";
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

/* a look-up table of the first few primes */
unsigned char iprime_lut[54] = {
  2,     3,     5,     7,    11,    13,    17,    19,    23,
 29,    31,    37,    41,    43,    47,    53,    59,    61,
 67,    71,    73,    79,    83,    89,    97,   101,   103,
107,   109,   113,   127,   131,   137,   139,   149,   151,
157,   163,   167,   173,   179,   181,   191,   193,   197,
199,   211,   223,   227,   229,   233,   239,   241,   251};

