/* tiprime.c - tests the iprime() fns, tries to test the low end
 *	and high end.
 */

#include <stdio.h>
#include <limits.h>
#include "isqrt.h"	/* ulsqrt */
#include "iprime.h"

#define TESTOUT(NM, TYPE, arg) \
tot++; if((res1 = (long) NM((TYPE) arg)) != (res2 = testprime((unsigned long) arg))) {\
	printf("FAIL:" #NM "(%lu)\t= %lu != %lu\n", \
		(unsigned long) arg, res1, res2);\
	err++; } else \
	printf("OK  :" #NM "(%lu)\t= %lu\n", (unsigned long) arg, res1);

#define STESTOUT(NM, TYPE, arg) \
tot++; if((res1 = (long) NM((TYPE) arg)) != (long) 0) { \
	printf("FAIL:" #NM "(%lu)\t= %lu != %lu\n", (long) arg, res1, (long) 0);\
	err++; } else \
	printf("OK  :" #NM "(%lu)\t= %lu\n", (long) arg, res1);

#define TESTRANGE(NM, TYPE, lo, hi) \
for (errsum = 0, i = (unsigned long) lo; i < (unsigned long) hi; ++i, ++tot) { \
	if(((long) NM((TYPE) i)) != (testprime((unsigned long) i))) {errsum++;\
	printf("%lu != %lu for %lu\n", (unsigned long) NM((TYPE) i), \
	testprime((unsigned long) i), (unsigned long) i);} }\
	if (errsum) {\
	printf("FAIL:" #NM "(%lu:%lu)\t=> %d errs\n", \
		(unsigned long) lo, (unsigned long) hi, errsum);\
	err+=errsum; } else \
	printf("OK  :" #NM "(%lu:%lu)\t=> no errs\n", \
		(unsigned long) lo, (unsigned long) hi);

unsigned long testprime(unsigned long in) {
	unsigned long i;
	if (in == 0 ) return 0;
	if (in == 1 || in == 2) return 1;
	if (!(in%2)) return 2;
	for (i = 3; i <= ulsqrt(in); i += 2) {
		if (!(in%i)) return i;
	}
	return 1;
}

int main() {
	int err=0, tot = 0, errsum=0;
	long res1, res2;
	unsigned long i;

	(void) setvbuf(stdout, (char *)NULL, _IOLBF, 0);

	TESTRANGE(chprime, char, 0, 127)
	TESTOUT(chprime, char, CHAR_MAX)
	STESTOUT(scprime, signed char, -64)
	TESTRANGE(scprime, signed char, 0, 127)
	TESTOUT(scprime, signed char, SCHAR_MAX)
	TESTRANGE(ucprime, unsigned char, 0, 255)
	TESTOUT(ucprime, unsigned char, UCHAR_MAX)

	STESTOUT(hprime, short, -64)
	TESTRANGE(hprime, short, 0, 32767)
	TESTRANGE(hprime, short, SHRT_MAX-256, SHRT_MAX)
	TESTOUT(hprime, short, SHRT_MAX)
	TESTRANGE(uhprime, unsigned short, 0, 65535)
	TESTRANGE(uhprime, unsigned short, USHRT_MAX-256, USHRT_MAX)
	TESTOUT(uhprime, unsigned short, USHRT_MAX)

	STESTOUT(iprime, int, -64)
	TESTRANGE(iprime, int, 0, 32767)
	TESTRANGE(iprime, int, INT_MAX-256, INT_MAX)
	TESTOUT(iprime, int, INT_MAX)
	TESTRANGE(uiprime, unsigned int, 0, 65535)
	TESTRANGE(uiprime, unsigned int, UINT_MAX-256, UINT_MAX)
	TESTOUT(uiprime, unsigned int, UINT_MAX)

	STESTOUT(lprime, long, -64)
	TESTRANGE(lprime, long, 0, 32767)
	TESTRANGE(lprime, long, LONG_MAX-256, LONG_MAX)
	TESTOUT(lprime, long, LONG_MAX)
	TESTRANGE(ulprime, unsigned long, 0, 65535)
	TESTRANGE(ulprime, unsigned long, ULONG_MAX-256, ULONG_MAX)
	TESTOUT(ulprime, unsigned long, ULONG_MAX)

	if (err)
		printf("%d test failures out of %d\n", err, tot);
	else
		printf("no test failures out of %d\n", tot);

	return 0;
}
