/* rudimentary test code */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef MEMDEBUG
#  include "memdebug.h"
#endif
#ifdef RKOERROR
#  include "rkoerror.h"
#endif
#ifdef HAVE_STRMALLOC
#  include "strmalloc.h"
#else
#  define strmalloc(a)  strdup(a)
#  define strnmalloc(a,b)  strcpy((char *) malloc((b)),(a))
#  define strfree(a)  free(*a)
#endif
#include "uvec.h"
#include "avec.h"

char testbuf[512];

int printout(avec *av, char const *head, int err, char const *ans) {
	char const * const * vec;
	char **vptr;
	char *sptr;
	uvec *uk;
	sprintf(testbuf,"e:%d c:%d n:%d",
		err, avec_capacity(av), avec_number(av));
	if (avec_exists(av)) {
		strcat(testbuf," k:");
		vec = avec_keys(av);
		uk = uvec_alloc();
		uvec_copy_vec(uk, vec, 0);
		uvec_sort(uk, UVEC_ASCEND);
		strcat(testbuf, sptr = uvec2str(uk, "|"));
		strfree(&sptr);
		free((void *)vec);

		vptr = uvec_vector(uk);
		strcat(testbuf," v:");
		while (*vptr) {
			strcat(testbuf,avec_lookup(av,*vptr));
			vptr++;
			if (*vptr) strcat(testbuf,"|");
		}

		uvec_dtor(&uk);
	}
	if (strcmp(testbuf, ans)) {
		printf("FAIL:%-20s=\n    <\t%s\n    >\t%s\n",head,testbuf,ans);
		return 1;
	} else {
		printf("OK  :%-20s=\n\t%s\n",head,testbuf);
		return 0;
	}
# ifdef RKOERROR
	if (rkoerrno != RKO_OK) {
		printf("%s\n", rkostrerror());
	}
# endif
}

int printcount(avec *av, char const *head, int err, char const *ans) {
	char const * const * vec;
	char **vptr;
	char *sptr;
	uvec *uk;
	char ibuf[64];

	sprintf(testbuf,"e:%d c:%d n:%d",
		err, avec_capacity(av), avec_number(av));
	if (avec_exists(av)) {
		strcat(testbuf," k:");
		vec = avec_keys(av);
		uk = uvec_alloc();
		uvec_copy_vec(uk, vec, 0);
		uvec_sort(uk, UVEC_ASCEND);
		strcat(testbuf, sptr = uvec2str(uk, "|"));
		strfree(&sptr);
		free((void *)vec);

		vptr = uvec_vector(uk);
		strcat(testbuf," v:");
		while (*vptr) {
			sprintf(ibuf, " %d", (int) avec_lookup(av,*vptr));
			strcat(testbuf,ibuf);
			vptr++;
		}

		uvec_dtor(&uk);
	}
	if (strcmp(testbuf, ans)) {
		printf("FAIL:%-20s=\n    <\t%s\n    >\t%s\n",head,testbuf,ans);
		return 1;
	} else {
		printf("OK  :%-20s=\n\t%s\n",head,testbuf);
		return 0;
	}
# ifdef RKOERROR
	if (rkoerrno != RKO_OK) {
		printf("%s\n", rkostrerror());
	}
# endif
}

int verify(avec *av, char const *head) {
	int    retval = 0;
	avec_element **aeptr;
	char **ptr;
	int    index;
	char  *keys[] = {
		"first",
		"second",
		"third",
		"fourth",
		"fifth",
		"sixth",
		"seventh",
		(char *) NULL
	};
	char  *values[] = {
		"=1=",
		(char *) NULL,
		"=3=",
		"=4=",
		"=5=",
		"=6=",
		(char *) NULL,
		(char *) NULL
	};
	char *key, *val;

	printf("------------------------\n%s\n------------------------\n",
		head);
	aeptr = avec_walk(av);
	do {
		key = (char *)(*aeptr)->key;
		val = (char *)(*aeptr)->data;
		ptr = keys;
		index = 0;
		while (*ptr) {	/* find the associatiate value */
			if (!strcmp(*ptr, key)) break;
			ptr++;
			index++;
		}
		if (!strcmp(values[index], val)) {
			printf("OK  >");
		} else {
			printf("FAIL>");
			retval = 1;
		}
		printf("\tkey: %s val: %s\n", key, val);
	} while ((aeptr = avec_walk(NULL)));

	ptr = keys;
	index = 0;
	while (*ptr) {
		key = *ptr;
		val = avec_lookup(av, *ptr);
		if (values[index] == (char *)NULL
		||  val == (char *) NULL) {
			if (values[index] == (char *)NULL
			&&  val == (char *) NULL) {
				printf("OK  >");
				val = "(NULL)";
			} else {
				printf("FAIL>");
				retval = 1;
			}
		} else if (!strcmp(values[index], val)) {
			printf("OK  >");
		} else {
			printf("FAIL>");
			retval = 1;
		}
		printf("\t%s -> %s\n", *ptr, val);
		ptr++;
		index++;
	}
	printf("------------------------\n");
#ifdef RKOERROR
	if (retval) {
		rkoperror("test");
	}
#endif
	return retval;
}

#define _CHECK(c, v, a) \
	count++; estat = c; \
	results += printout(&(v), #c, estat, a);

#define _COUNT(c, v, a) \
	count++; estat = c; \
	results += printcount(&(v), #c, estat, a);

int str_Xmalloc (void **data, va_list ap) {
	char *newstr;
	char const *str = va_arg(ap,char *);
	if (!data) return -1;
	if (*data) {
		newstr = strnmalloc((char const *)*data,
			strlen((char const *)*data) + strlen(str) + 1);
		strcat(newstr, str);
		strfree((char **)data);
		*data = (void *) newstr;
		return 1;
	}
	if((*data = (void *) strmalloc(str))) return 0;
	return -2;
}

int str_Xfree (void **data, va_list ap) {
	strfree((char **) data);
	return 0;
}

avec_fns user_fns = {
	AVEC_USER,
	str_Xmalloc,
	str_Xfree,
	str_Xfree
};

int main () {
	avec *x, *y, *z;
	int count=0, results=0;
	int estat;
	void **values, **vptr;

#ifdef MEMDEBUG
	memdebug_output(stdout);
#endif

	x = avec_ctor(10);
	_CHECK(avec_number(x),*x,
		"e:0 c:11 n:0 k: v:");
	_CHECK(avec_capacity(x),*x,
		"e:11 c:11 n:0 k: v:");
	_CHECK(avec_resize_percentage(x,0),*x,
		"e:0 c:11 n:0 k: v:");
	_CHECK(avec_resize_percentage(x,40),*x,
		"e:0 c:11 n:0 k: v:");
	_CHECK(avec_resize_percentage(x,0),*x,
		"e:40 c:11 n:0 k: v:");
	_CHECK(avec_insert(x,"first","=1="),*x,
		"e:0 c:11 n:1 k:first v:=1=");
	_CHECK(avec_insert(x,"second","=2="),*x,
		"e:0 c:11 n:2 k:first|second v:=1=|=2=");
	_CHECK(avec_insert(x,"foo","=bar="),*x,
		"e:0 c:11 n:3 k:first|foo|second v:=1=|=bar=|=2=");
	_CHECK(avec_remove(x,"foo","=bar="),*x,
		"e:0 c:11 n:2 k:first|second v:=1=|=2=");
	_CHECK(avec_insert(x,"second","#2#"),*x,
		"e:1 c:11 n:2 k:first|second v:=1=|=2=");
	_CHECK(avec_insert(x,"third","=3="),*x,
		"e:0 c:11 n:3 k:first|second|third v:=1=|=2=|=3=");
	_CHECK(avec_delete(x,"second"),*x,
		"e:0 c:11 n:2 k:first|third v:=1=|=3=");
	_CHECK(avec_insert(x,"second","=2="),*x,
		"e:0 c:11 n:3 k:first|second|third v:=1=|=2=|=3=");
	_CHECK(avec_insert(x,"fourth","=4="),*x,
	"e:0 c:11 n:4 k:first|fourth|second|third v:=1=|=4=|=2=|=3=");
	_CHECK(avec_insert(x,"fifth","=5="),*x,
	"e:0 c:23 n:5 k:fifth|first|fourth|second|third v:=5=|=1=|=4=|=2=|=3=");
	_CHECK(avec_insert(x,"sixth","=6="),*x,
	"e:0 c:23 n:6 k:fifth|first|fourth|second|sixth|third v:=5=|=1=|=4=|=2=|=6=|=3=");
	_CHECK(avec_delete(x,"forth"),*x,
	"e:-1 c:23 n:6 k:fifth|first|fourth|second|sixth|third v:=5=|=1=|=4=|=2=|=6=|=3=");
	_CHECK(avec_delete(x,"second"),*x,
	"e:0 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_resize(x,4),*x,
	"e:-3 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_increase(x,4),*x,
	"e:-2 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_decrease(x,4),*x,
	"e:-3 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_decrease(x,25),*x,
	"e:-2 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_resize(x,14),*x,
	"e:0 c:17 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_resize(x,6),*x,
	"e:0 c:7 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");

	count++;
	results += verify(x, "Default avec_fns");

	_CHECK(avec_close(x),*x,
		"e:0 c:7 n:0 k: v:");
	_CHECK(avec_dtor(&x),*x,
		"e:0 c:-1 n:-1");
	printf("========================\n");

	y = avec_ctor_(10, user_fns);
	_CHECK(avec_number(y),*y,
		"e:0 c:11 n:0 k: v:");
	_CHECK(avec_capacity(y),*y,
		"e:11 c:11 n:0 k: v:");
	_CHECK(avec_resize_percentage(y,0),*y,
		"e:0 c:11 n:0 k: v:");
	_CHECK(avec_resize_percentage(y,40),*y,
		"e:0 c:11 n:0 k: v:");
	_CHECK(avec_resize_percentage(y,0),*y,
		"e:40 c:11 n:0 k: v:");
	_CHECK(avec_insert(y,"first","=1="),*y,
		"e:0 c:11 n:1 k:first v:=1=");
	_CHECK(avec_insert(y,"second","=2="),*y,
		"e:0 c:11 n:2 k:first|second v:=1=|=2=");
	_CHECK(avec_insert(y,"foo","=bar="),*y,
		"e:0 c:11 n:3 k:first|foo|second v:=1=|=bar=|=2=");
	_CHECK(avec_remove(y,"foo","=bar="),*y,
		"e:0 c:11 n:2 k:first|second v:=1=|=2=");
	_CHECK(avec_insert(y,"second","#2#"),*y,
		"e:1 c:11 n:2 k:first|second v:=1=|=2=#2#");
	_CHECK(avec_insert(y,"third","=3="),*y,
		"e:0 c:11 n:3 k:first|second|third v:=1=|=2=#2#|=3=");
	_CHECK(avec_delete(y,"second"),*y,
		"e:0 c:11 n:2 k:first|third v:=1=|=3=");
	_CHECK(avec_insert(y,"second","=2="),*y,
		"e:0 c:11 n:3 k:first|second|third v:=1=|=2=|=3=");
	_CHECK(avec_insert(y,"fourth","=4="),*y,
	"e:0 c:11 n:4 k:first|fourth|second|third v:=1=|=4=|=2=|=3=");
	_CHECK(avec_insert(y,"fifth","=5="),*y,
	"e:0 c:23 n:5 k:fifth|first|fourth|second|third v:=5=|=1=|=4=|=2=|=3=");
	_CHECK(avec_insert(y,"sixth","=6="),*y,
	"e:0 c:23 n:6 k:fifth|first|fourth|second|sixth|third v:=5=|=1=|=4=|=2=|=6=|=3=");
	_CHECK(avec_delete(y,"forth"),*y,
	"e:-1 c:23 n:6 k:fifth|first|fourth|second|sixth|third v:=5=|=1=|=4=|=2=|=6=|=3=");
	_CHECK(avec_delete(y,"second"),*y,
	"e:0 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_resize(y,4),*y,
	"e:-3 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_increase(y,4),*y,
	"e:-2 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_decrease(y,4),*y,
	"e:-3 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_decrease(y,25),*y,
	"e:-2 c:23 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_resize(y,14),*y,
	"e:0 c:17 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");
	_CHECK(avec_resize(y,6),*y,
	"e:0 c:7 n:5 k:fifth|first|fourth|sixth|third v:=5=|=1=|=4=|=6=|=3=");

	count++;
	results += verify(y, "User avec_fns");

	vptr = values = avec_values(y);
	_CHECK(avec_dealloc(&y),*y,
		"e:0 c:-1 n:-1");
	printf("should have different values and will clear memory\n");
	while(*vptr) {
		printf("\tv:%s\n", (char *) *vptr);
		strfree((char **)vptr);
		vptr++;
	}
	free((void *) values);
	printf("========================\n");

	avec_set_fns(AVEC_COUNT, NULL);
	z = avec_ctor(10);
	_COUNT(avec_number(z),*z,
		"e:0 c:11 n:0 k: v:");
	_COUNT(avec_capacity(z),*z,
		"e:11 c:11 n:0 k: v:");
	_COUNT(avec_resize_percentage(z,0),*z,
		"e:0 c:11 n:0 k: v:");
	_COUNT(avec_resize_percentage(z,40),*z,
		"e:0 c:11 n:0 k: v:");
	_COUNT(avec_resize_percentage(z,0),*z,
		"e:40 c:11 n:0 k: v:");
	_COUNT(avec_insert(z,"first"),*z,
		"e:0 c:11 n:1 k:first v: 1");
	_COUNT(avec_insert(z,"first"),*z,
		"e:1 c:11 n:1 k:first v: 2");
	_COUNT(avec_insert(z,"second"),*z,
		"e:0 c:11 n:2 k:first|second v: 2 1");
	_COUNT(avec_insert(z,"foo"),*z,
		"e:0 c:11 n:3 k:first|foo|second v: 2 1 1");
	_COUNT(avec_remove(z,"foo"),*z,
		"e:0 c:11 n:2 k:first|second v: 2 1");
	_COUNT(avec_insert(z,"first"),*z,
		"e:1 c:11 n:2 k:first|second v: 3 1");
	_COUNT(avec_delete(z,"second"),*z,
		"e:0 c:11 n:1 k:first v: 3");
	_COUNT(avec_delete(z,"first"),*z,
		"e:1 c:11 n:1 k:first v: 2");
	_COUNT(avec_insert(z,"third"),*z,
		"e:0 c:11 n:2 k:first|third v: 2 1");
	_COUNT(avec_insert(z,"second"),*z,
		"e:0 c:11 n:3 k:first|second|third v: 2 1 1");
	_COUNT(avec_insert(z,"third"),*z,
		"e:1 c:11 n:3 k:first|second|third v: 2 1 2");
	_COUNT(avec_delete(z,"third"),*z,
		"e:1 c:11 n:3 k:first|second|third v: 2 1 1");
	_CHECK(avec_dtor(&z),*z,
		"e:0 c:-1 n:-1");
	printf("========================\n");

	if (results) {
		printf("There were %d failures in %d tests\n", results, count);
	} else {
		printf("There were no failures in %d tests\n", count);
	}
	return results;
}
