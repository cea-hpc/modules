
#include <stdio.h>
#include <string.h>
#include "uvec.h"
#ifdef HAVE_STRMALLOC
#  include "strmalloc.h"
#else
#  define strfree(x) 0
#endif
#ifdef RKOERROR
#  include "rkoerror.h"
#else
int rkoerrno = 0;
#endif
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif

FILE *output;

char testbuf[256];

int printout(uvec *uv, char const *head, int err, char const *ans) {
	int i;
	char **argv;

	if (uv) argv = uv->vector;
	sprintf(testbuf,"e:%d c:%d n:%d r:",
		err, uvec_capacity(uv), uvec_number(uv));
	if (uvec_exists(uv)) {
		for (i = 0; *argv != (char *) NULL; ++i, ++argv) {
			strcat(testbuf, *argv);
		}
	}
	if (
#ifdef HAVE_STRCASECMP
	strcasecmp(testbuf, ans)
#else
	strcmp(testbuf, ans)
#endif
	) {
		fprintf(output,"FAIL:%-20s=\n    <\t%s\n    >\t%s\n",
			head,testbuf,ans);
		return 1;
	} else {
		fprintf(output,"OK  :%-20s=\n\t%s\n",head,testbuf);
		return 0;
	}
}

int printret(uvec *uv, char const *head, int result, int ans) {
	int i;
	char **argv = uv->vector;
	sprintf(testbuf,"e:%d c:%d n:%d r:",
		result, uvec_capacity(uv), uvec_number(uv));
	if (uvec_exists(uv)) {
		for (i = 0; *argv != (char *) NULL; ++i, ++argv) {
			strcat(testbuf, *argv);
		}
	}
	if (result != ans) {
		fprintf(output,"FAIL:%-20s=\n    <\t%s\n    >\te: %d != %d\n",
			head, testbuf, result, ans);
		return 1;
	} else {
		fprintf(output,"OK  :%-20s=\n\t%s\n",head,testbuf);
		return 0;
	}
}

int printval(char const *head, int result, int ans) {
	sprintf(testbuf,"e:%d r:", result );
	if (result != ans) {
		fprintf(output,"FAIL:%-20s=\n    <\t%s\n    >\te: %d != %d\n",
			head, testbuf, result, ans);
		return 1;
	} else {
		fprintf(output,"OK  :%-20s=\t%s\n",head,testbuf);
		return 0;
	}
}

#define _CHECK(c,v,a) \
	count++; estat = c; \
	results += printout(&(v), #c , estat, a);

#define _CHECKRET(c,v,a) \
	count++; estat = c; \
	results += printret(&(v), #c , estat, a);

#define _CHECKVAL(c,a) \
	count++; estat = c; \
	results += printval(#c , estat, a);

int main() {
	uvec u,v, *x, *y;
	char *vec;
	int estat = 0;
	int results = 0;
	int i;
	int count = 0;
	char buffer[128];
	char const * const list[] = {
	":xyz",":ABC",":aaa",":bbb",":XYZ",
	":AAA",":bb" ,":abc",":ABC", (char *) NULL};

	output = stdout;

#ifdef MEMDEBUG
	memdebug_output(output);
#endif

	_CHECK(uvec_init(&u,30), u,
	"e:0 c:30 n:0 r:")
	_CHECK(uvec_init(&u,20), u,
	"e:-2 c:30 n:0 r:")
	_CHECK(uvec_close(&u), u,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_init(&u,-30), v,
	"e:-3 c:-1 n:-1 r:")
	_CHECK(uvec_init(&u,10), u,
	"e:0 c:10 n:0 r:")
	_CHECK(((x = uvec_ctor(25))==(uvec *)NULL), *x,
	"e:0 c:25 n:0 r:")
	_CHECK(uvec_dtor(&x), *x,
	"e:0 c:-1 n:-1 r:")

	_CHECK(uvec_add(&u, ";00"), u,
	"e:0 c:10 n:1 r:;00")
	_CHECK(uvec_addl(&u, ":01", ":02", NULL), u,
	"e:0 c:10 n:3 r:;00:01:02")
	for (i = 3; i < 12; ++i) {
		(void) sprintf(buffer, ":%.2d", i);
		if ((estat = uvec_add(&u, buffer)))
#ifdef RKOERROR
			rkoperror("main");
#else
			fprintf(output,"error = %d",estat);
#endif
	}
	*(uvec_vector(&u)[3]) = 'x';
	*(uvec_vector(&u)[8]) = 'x';

	_CHECK(uvec_insert(&u, "=90", 0), u,
	"e:0 c:15 n:13 r:=90;00:01:02x03:04:05:06:07x08:09:10:11")
	_CHECK(uvec_insert(&u, "=91", uvec_number(&u)), u,
	"e:0 c:15 n:14 r:=90;00:01:02x03:04:05:06:07x08:09:10:11=91")
	_CHECK(uvec_insert(&u, "=92", 3), u,
	"e:0 c:22 n:15 r:=90;00:01=92:02x03:04:05:06:07x08:09:10:11=91")
	_CHECK(uvec_insert(&u, "=93", uvec_end(&u)), u,
	"e:0 c:22 n:16 r:=90;00:01=92:02x03:04:05:06:07x08:09:10:11=93=91")
	_CHECK(uvec_insert(&u, "=94", 1), u,
	"e:0 c:22 n:17 r:=90=94;00:01=92:02x03:04:05:06:07x08:09:10:11=93=91")

	_CHECK(uvec_pop(&u), u,
	"e:0 c:22 n:16 r:=90=94;00:01=92:02x03:04:05:06:07x08:09:10:11=93")
	_CHECK(uvec_delete(&u, uvec_end(&u)), u,
	"e:0 c:22 n:15 r:=90=94;00:01=92:02x03:04:05:06:07x08:09:10:11")
	_CHECK(uvec_delete(&u, 1), u,
	"e:0 c:22 n:14 r:=90;00:01=92:02x03:04:05:06:07x08:09:10:11")
	_CHECK(uvec_delete(&u, 0), u,
	"e:0 c:22 n:13 r:;00:01=92:02x03:04:05:06:07x08:09:10:11")
	_CHECK(uvec_delete(&u, 2), u,
	"e:0 c:22 n:12 r:;00:01:02x03:04:05:06:07x08:09:10:11")
	_CHECK(uvec_pop(&u), u,
	"e:0 c:22 n:11 r:;00:01:02x03:04:05:06:07x08:09:10")
	_CHECK(uvec_pop(&u), u,
	"e:0 c:14 n:10 r:;00:01:02x03:04:05:06:07x08:09")
	_CHECK(uvec_insert(&u, "=95=", uvec_end(&u) + 2), u,
	"e:-1 c:14 n:10 r:;00:01:02x03:04:05:06:07x08:09")
	_CHECK(uvec_delete(&u, uvec_end(&u) + 2), u,
	"e:-2 c:14 n:10 r:;00:01:02x03:04:05:06:07x08:09")
	_CHECK(uvec_close(&u), u,
	"e:0 c:-1 n:-1 r:")

/* next batch */
	_CHECK(uvec_copy_vec(&u, list, 0), u,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	_CHECK(uvec_copy(&v, &u), v,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	_CHECK(uvec_sort(&v,UVEC_ASCEND), v,
	"e:0 c:10 n:9 r::AAA:ABC:ABC:XYZ:aaa:abc:bb:bbb:xyz")
	_CHECK(uvec_uniq(&v,UVEC_ASCEND), v,
	"e:0 c:10 n:8 r::AAA:ABC:XYZ:aaa:abc:bb:bbb:xyz")
#if 0
	_CHECK(uvec_dup(&w,UVEC_ASCEND), w,
	"e:0 c:5 n:1 r::ABC")
#endif
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy(&v,&u), v,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	_CHECKRET(uvec_find(&v,":abc", UVEC_ASCEND), v, 7)
	_CHECKRET(uvec_find(&v,":abc", UVEC_DESCEND), v, 7)
	_CHECKRET(uvec_find(&v,":abcdef", UVEC_ASCEND), v, -1)
#ifdef HAVE_STRCASECMP
	_CHECKRET(uvec_find(&v,":abc", UVEC_CASE_ASCEND), v, 1)
	_CHECKRET(uvec_find(&v,":abc", UVEC_CASE_DESCEND), v, 1)
	_CHECKRET(uvec_find(&v,":abcdef", UVEC_CASE_ASCEND), v, -1)
#endif
	_CHECK(uvec_sort(&v,UVEC_DESCEND), v,
	"e:0 c:10 n:9 r::xyz:bbb:bb:abc:aaa:XYZ:ABC:ABC:AAA")
	_CHECK(uvec_uniq(&v,UVEC_DESCEND), v,
	"e:0 c:10 n:8 r::xyz:bbb:bb:abc:aaa:XYZ:ABC:AAA")
#if 0
	_CHECK(uvec_dup(&w,UVEC_DESCEND), w,
	"e:0 c:5 n:1 r::ABC")
#endif
#ifdef HAVE_STRCASECMP
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy(&v,&u), v,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	_CHECK(uvec_sort(&v,UVEC_CASE_ASCEND), v,
	"e:0 c:10 n:9 r::aaa:AAA:ABC:abc:ABC:bb:bbb:xyz:XYZ")
	_CHECK(uvec_uniq(&v,UVEC_CASE_ASCEND), v,
	"e:0 c:10 n:5 r::aaa:ABC:bb:bbb:xyz")
#if 0
	_CHECK(uvec_dup(&w,UVEC_CASE_ASCEND), w,
	"e:0 c:10 n:7 r::aaa:AAA:ABC:abc:ABC:xyz:XYZ")
#endif
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy(&v,&u), v,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	_CHECK(uvec_sort(&v,UVEC_CASE_DESCEND), v,
	"e:0 c:10 n:9 r::xyz:XYZ:bbb:bb:ABC:abc:ABC:aaa:AAA")
	_CHECK(uvec_uniq(&v,UVEC_CASE_DESCEND), v,
	"e:0 c:10 n:5 r::xyz:bbb:bb:ABC:aaa")
#if 0
	_CHECK(uvec_dup(&w,UVEC_CASE_DESCEND), w,
	"e:0 c:10 n:7 r::xyz:XYZ:ABC:abc:ABC:aaa:AAA")
#endif
#endif
	_CHECKVAL(uvec_set_strfns(UVEC_STDC, NULL), 0)
	_CHECKVAL(uvec_get_strfns(), 2)
	_CHECKVAL(uvec_set_strfns(UVEC_DEFAULT, NULL), 0)

	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy(&v,&u), v,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	_CHECK(uvec_reverse(&v), v,
	"e:0 c:10 n:9 r::ABC:abc:bb:AAA:XYZ:bbb:aaa:ABC:xyz")
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy(&v,&u), v,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	fprintf(output,
	"\nthe following may fail if different random number generator\n");
	_CHECK(uvec_randomize(&v, 51258), v,
	"e:0 c:10 n:9 r::abc:ABC:ABC:bb:aaa:XYZ:bbb:AAA:xyz")
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy(&v,&u), v,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	fprintf(output,
	"\nthe following may fail if different random number generator\n");
	_CHECK(uvec_randomize(&v, 51258), v,
	"e:0 c:10 n:9 r::abc:ABC:ABC:bb:aaa:XYZ:bbb:AAA:xyz")
	_CHECKVAL(uvec_count_tok(":","a:b:c:d"), 4)
	_CHECKVAL(uvec_count_tok(":","a:b:c:d:"), 4)
	_CHECKVAL(uvec_count_tok("()","a()b()c()d"), 4)
	_CHECKVAL(uvec_count_tok(":","a\\:b:c:d"), 3)
	_CHECKVAL(uvec_count_tok("()","a()b\\()c()d"), 3)
	_CHECKVAL(uvec_count_tok(":","a()b()c()d"), 1)
	_CHECKVAL(uvec_count_tok(":","a"), 1)
	_CHECKVAL(uvec_count_tok(":",":a"), 2)
	_CHECKVAL(uvec_count_tok(":","a:"), 1)
	_CHECKVAL(uvec_count_tok(NULL,"a"), -1)
	_CHECKVAL(uvec_count_tok(":",NULL), 0)
	_CHECKVAL(uvec_count_tok("","a"), -1)
	_CHECKVAL(uvec_count_tok(":",""), 0)

	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy_str(&v, ":", "a:ab:abc"), v,
	"e:0 c:4 n:3 r:aababc")
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy_str(&v, ":", ":a:ab:abc"), v,
	"e:0 c:5 n:4 r:aababc")
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy_str(&v, ":", "a:ab:abc:"), v,
	"e:0 c:4 n:3 r:aababc")
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_copy_str(&v, ":", "a\\:ab:abc"), v,
	"e:0 c:3 n:2 r:a\\:ababc")
	_CHECK(uvec_close(&v), v,
	"e:0 c:-1 n:-1 r:")
	_CHECK(uvec_close(&u), u,
	"e:0 c:-1 n:-1 r:")

	_CHECK((x = str2uvec(":", "a:ab:abc"),rkoerrno), *x,
	"e:0 c:4 n:3 r:aababc")
	_CHECK(uvec_dtor(&x), *x, "e:0 c:-1 n:-1 r:")
	_CHECK((x = vec2uvec(list, 0),rkoerrno), *x,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	_CHECK((y = uvec2uvec(x),rkoerrno), *y,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	_CHECK(uvec_dtor(&x), *x, "e:0 c:-1 n:-1 r:")
	_CHECK((x = str2uvec(";", vec=uvec2str(y,";")),rkoerrno), *x,
	"e:0 c:10 n:9 r::xyz:ABC:aaa:bbb:XYZ:AAA:bb:abc:ABC")
	if(strfree(&vec)) fprintf(output,">>> strfree error\n");;
	_CHECK(uvec_dtor(&x), *x, "e:0 c:-1 n:-1 r:")
	_CHECK((x = str2uvec(":", vec=uvec2str(y,";")),rkoerrno), *x,
	"e:0 c:11 n:10 r:xyz;ABC;aaa;bbb;XYZ;AAA;bb;abc;ABC")
	if(strfree(&vec)) fprintf(output,">>> strfree error\n");;
	_CHECK(uvec_dtor(&x), *x, "e:0 c:-1 n:-1 r:")
	_CHECK(uvec_dtor(&y), *x, "e:0 c:-1 n:-1 r:")

	if (results) {
		fprintf(output,"There were %d failures in %d tests\n", results, count);
	} else {
		fprintf(output,"There were no failures in %d tests\n", count);
	}
	fprintf(output,
	"Expect 2 failures due to different random number generators\n");
	return results;
}
