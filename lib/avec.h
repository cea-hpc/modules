#ifndef _AVEC_H_
#  define _AVEC_H_
/* 
 * RCSID @(#)$Id: avec.h,v 1.1 2002/07/18 22:03:39 rkowen Exp $
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/
#  ifdef __cplusplus
extern "C" {
#  endif

#  include <stdarg.h>	/* va_list */

/* Associative Vector (hash array) package */

enum avec_def_fns {AVEC_DEFAULT, AVEC_USER, AVEC_COUNT, AVEC_STDC
#  ifdef HAVE_STRMALLOC
	, AVEC_STRMALLOC
#endif
	};


typedef struct {
	/* store the data type - user can ignore this */
	enum avec_def_fns	type;
	int  (*data_add)(void **,va_list); /* allocate data element for insert*/
	int  (*data_del)(void **,va_list); /* dealloc  data element for delete*/
	int  (*data_rm)(void **,va_list);  /* remove   data element for close*/
} avec_fns;
	
typedef struct {
	char const	 *key;		/* key for associative array */
	void		 *data;		/* container for datum */
} avec_element;

typedef struct {
	char		  tag[5];	/* name tag for this type */
	avec_element	**hash;		/* container for keys & data */
	int		  number;	/* current number of hash */
	int		  capacity;	/* the possible capacity of hash */
	int		  percentage;	/* percentage of capacity that
					   forces a resize larger 
					   (< 0 = no resizeing )*/
	avec_fns	  fns;		/* which alloc fns to use */
} avec;

/* shield users from certain internal details */
#define AVEC_KEY(elemptr)	((elemptr)->key)
#define AVEC_DATA(elemptr)	((elemptr)->data)


int                avec_set_fns(enum avec_def_fns type, avec_fns *fns);
enum avec_def_fns  avec_get_fns(void);

avec  *avec_ctor_(int cap, avec_fns fns);
avec  *avec_ctor(int cap);
int    avec_dtor(avec **av, ...);
avec  *avec_alloc_(avec_fns strfns);
avec  *avec_alloc(void);
int    avec_dealloc(avec **av);
int    avec_init_(avec *av, int cap, avec_fns fns);
int    avec_init(avec *av, int cap);
int    avec_close(avec *av, ...);

int    avec_exists(avec const *av);
int    avec_capacity(avec const *av);
int    avec_number(avec const *av);
int    avec_resize_percentage(avec *av, int percentage);
 
int    avec_resize(avec *av, int newcap);
int    avec_increase(avec *av, int newcap);
int    avec_decrease(avec *av, int newcap);

int    avec_insert(avec *av, char const *key, ...);
int    avec_delete(avec *av, char const *key, ...);
int    avec_remove(avec *av, char const *key, ...);
void  *avec_lookup(avec *av, char const *key);

avec_element             **avec_walk_r(avec *av, avec_element **ptrptr);
avec_element             **avec_walk(avec *av);
char const * const        *avec_keys(avec *av);
void                     **avec_values(avec *av);
avec_element const *const *avec_hash(avec *av);

#  ifdef __cplusplus
	}
#  endif
#endif /* _AVEC_H_ */
