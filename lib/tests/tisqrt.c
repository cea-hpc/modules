/* tisqrt.c - tests the isqrt() fns, tries to test the low end
 *	and high end (since for the unsigned versions they use
 *	the signed versions at the low end).
 */

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "isqrt.h"

#define TESTOUT(NM, TYPE, arg) \
tot++; if((res1 = (long) NM((TYPE) arg)) != (res2 = tsqrt((unsigned long) arg))) {\
	printf("FAIL:" #NM "(%lu)\t= %ld != %ld\n", \
		(unsigned long) arg, res1, res2);\
	err++; } else \
	printf("OK  :" #NM "(%lu)\t= %ld\n", (unsigned long) arg, res1);

#define STESTOUT(NM, TYPE, arg) \
tot++; if((res1 = (long) NM((TYPE) arg)) != (long) 0) { \
	printf("FAIL:" #NM "(%ld)\t= %ld != %ld\n", (long) arg, res1, (long) 0);\
	err++; } else \
	printf("OK  :" #NM "(%ld)\t= %ld\n", (long) arg, res1);

#define TESTRANGE(NM, TYPE, lo, hi) \
for (errsum = 0, i = (unsigned long) lo; i < (unsigned long) hi; ++i, ++tot) { \
	if(((long) NM((TYPE) i)) != (tsqrt((unsigned long) i))) errsum++; }\
	if (errsum) {\
	printf("FAIL:" #NM "(%lu:%lu)\t=> %d errs\n", \
		(unsigned long) lo, (unsigned long) hi, errsum);\
	err+=errsum; } else \
	printf("OK  :" #NM "(%lu:%lu)\t=> no errs\n", \
		(unsigned long) lo, (unsigned long) hi);

long tsqrt(unsigned long a) {
	unsigned long lt;
	if (a == 0) return 0;
	lt = sqrt((double) a);
	if (lt*lt <= a && lt*lt > 0) return lt;
	/* watch for round-up errors */
	while ((--lt, (lt*lt) > a));
	return (long) lt;
}

int main() {
	int err=0, tot = 0, errsum=0;
	long res1, res2;
	unsigned long i;

	(void) setvbuf(stdout, (char *)NULL, _IOLBF, 0);

	TESTRANGE(chsqrt, char, 0, 127)
	TESTOUT(chsqrt, char, CHAR_MAX)
	STESTOUT(scsqrt, signed char, -64)
	TESTRANGE(scsqrt, signed char, 0, 127)
	TESTOUT(scsqrt, signed char, SCHAR_MAX)
	TESTRANGE(ucsqrt, unsigned char, 0, 255)
	TESTOUT(ucsqrt, unsigned char, UCHAR_MAX)

	STESTOUT(hsqrt, short, -64)
	TESTRANGE(hsqrt, short, 0, 32767)
	TESTRANGE(hsqrt, short, SHRT_MAX-4096, SHRT_MAX)
	TESTOUT(hsqrt, short, SHRT_MAX)
	TESTRANGE(uhsqrt, unsigned short, 0, 65535)
	TESTRANGE(uhsqrt, unsigned short, USHRT_MAX-4096, USHRT_MAX)
	TESTOUT(uhsqrt, unsigned short, USHRT_MAX)

	STESTOUT(isqrt, int, -64)
	TESTRANGE(isqrt, int, 0, 32767)
	TESTRANGE(isqrt, int, INT_MAX-32767, INT_MAX)
	TESTOUT(isqrt, int, INT_MAX)
	TESTRANGE(uisqrt, unsigned int, 0, 65535)
	TESTRANGE(uisqrt, unsigned int, UINT_MAX-32767, UINT_MAX)
	TESTOUT(uisqrt, unsigned int, UINT_MAX)

	STESTOUT(lsqrt, long, -64)
	TESTRANGE(lsqrt, long, 0, 32767)
	TESTRANGE(lsqrt, long, LONG_MAX - 32767, LONG_MAX)
	TESTOUT(lsqrt, long, LONG_MAX)
	TESTRANGE(ulsqrt, unsigned long, 0, 65535)
	TESTRANGE(ulsqrt, unsigned long, ULONG_MAX - 32767, ULONG_MAX)
	TESTOUT(ulsqrt, unsigned long, ULONG_MAX)

	if (err)
		printf("%d test failures out of %d\n", err, tot);
	else
		printf("no test failures out of %d\n", tot);

	return 0;
}
