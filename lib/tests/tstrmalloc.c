
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#ifdef RKOERROR
#  include "rkoerror.h"
#endif
#include "strmalloc.h"

int comparestr(const char *malstr, const char *teststr, size_t n) {
	while (*teststr != '\0' && n--) {
		if (*teststr == '_' && *malstr != '\0') return 1;
		if (*teststr != '_' && *teststr != *malstr) return -1;
		teststr++;
		malstr++;
	}
	return 0;
}

int trystr(const char *test, const char *malstr, const char *teststr,
	size_t len) {
	int errval = 0;
	if ((errval = comparestr(malstr, teststr, len))) {
		printf("FAIL: %s: %d : `%s' != `%s'\n",
			test, errval, malstr, teststr);
		return 1;
	} else if (str_sizeof(malstr) != len) {
		printf("FAIL: %s: %d-`%s' != %d-%s'\n",
			test, (int) str_sizeof(malstr), malstr,
			(int) strlen(teststr), teststr);
		return 1;
	} else {
		printf("OK  : %s: `%s'\n", test, malstr);
		return 0;
	}
}

int tryfree(int errvalue, char *errmsg) {
#ifdef RKOERROR
	if (rkoerrno != errvalue) {
		printf("FAIL: str_free: incorrect rkoerrno = %d != %d\n",
			rkoerrno, errvalue);
		return 1;
	} else
		if (strcmp(errmsg, rkostrerror())) {
		printf("FAIL: str_free: incorrect error message : %s\n",
			rkostrerror());
		return 1;
	} else
#endif
	{
		printf("OK  : str_free: correct rkoerrno = %d\n",
			rkoerrno);
		return 0;
	}
}

int main() {
	char *test[7];
	char *teststr[9] = {
		"",
		"This is a Test String",
		"This is a Test_String",
		"This is another Test String",
		"This is another Test_String",
		"This",
		"is",
		"ThisThis",
		"ThisThisThis is another Test String",
	};

	int results = 0;

	test[0] = (char *) NULL;
	test[1] = str_malloc(teststr[1]);
	results += trystr("str_malloc",test[1],teststr[1],strlen(teststr[1])+1);
	test[2] = str_malloc("This is another Test String");
	results += trystr("str_malloc",test[2],teststr[3],strlen(teststr[3])+1);
	test[3] = test[1];
	results += trystr("str_malloc",test[3],teststr[1],strlen(teststr[1])+1);
	test[1][14] = '\0';
	results += trystr("str_malloc",test[1],teststr[2],strlen(teststr[2])+1);
	test[2][20] = '\0';
	results += trystr("str_malloc",test[2],teststr[4],strlen(teststr[4])+1);
	test[4] = str_nmalloc(test[3],4);
	results +=trystr("str_nmalloc",test[4],teststr[5],strlen(teststr[5]));
	test[5] = str_nmalloc(teststr[3]+2,2);
	results +=trystr("str_nmalloc",test[5],teststr[6],strlen(teststr[6]));
	test[6] = str_nmalloc(teststr[5],10);
	results += trystr("str_nmalloc",test[6], teststr[5],10);

	str_cpy(&test[6],teststr[1]);
	results += trystr("str_cpy", test[6], teststr[1],strlen(teststr[1])+1);
	str_cpy(&test[6],teststr[5]);
	results += trystr("str_cpy", test[6], teststr[5],strlen(teststr[1])+1);
	str_cat(&test[6],teststr[5]);
	results += trystr("str_cat", test[6], teststr[7],strlen(teststr[1])+1);
	str_cat(&test[6],teststr[3]);
	results += trystr("str_cat", test[6], teststr[8],strlen(teststr[8])+1);

	str_ncpy(&test[5],teststr[1], strlen(teststr[1])+1);
	results += trystr("str_ncpy", test[5], teststr[1],strlen(teststr[1])+1);
	str_ncpy(&test[5],teststr[5], strlen(teststr[5])+1);
	results += trystr("str_ncpy", test[5], teststr[5],strlen(teststr[1])+1);
	str_ncat(&test[5],teststr[5], strlen(teststr[5])+1);
	results += trystr("str_ncat", test[5], teststr[7],strlen(teststr[1])+1);
	str_ncat(&test[5],teststr[3], strlen(teststr[3])+1);
	results += trystr("str_ncat", test[5], teststr[8],strlen(teststr[8])+1);

	str_free(&test[0]);
	results += tryfree(RKO_OK, "");
	str_free(&test[1]);
	results += tryfree(RKO_OK, "");
	str_free(&test[2]);
	results += tryfree(RKO_OK, "");
	str_free(&test[3]);
	results += tryfree(RKOUSEERR, "str_free : str_sizeof : invalid string object!");
	str_free(&test[4]);
	results += tryfree(RKO_OK, "");
	str_free(&test[5]);
	results += tryfree(RKO_OK, "");
	str_free(&test[6]);
	results += tryfree(RKO_OK, "");
	/* bunch of error stuff */
	test[1] = str_malloc((char *) NULL);
	results += tryfree(RKOUSEERR, "str_malloc : NULL string!");
	test[1] = str_nmalloc((char *) NULL, 3);
	results += tryfree(RKOUSEERR, "str_nmalloc : NULL string!");

	if (results) {
		printf("There were %d test failures\n", results);
	} else {
		printf("There were no test failures\n");
	}
	return results;
}
