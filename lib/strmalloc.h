#ifndef _STRMALLOC_H_
#  define _STRMALLOC_H_
/* 
 * RCSID @(#)$Id: strmalloc.h,v 1.1 2002/07/18 22:23:57 rkowen Exp $
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2001 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <string.h>

#  ifdef __cplusplus
extern "C" {
#  endif

/* old interface */
char	*strnmalloc(	char const *in, size_t n);
char	*strmalloc(	char const *in);
int	 strfree(	char **str);

/* new interface */
char	*str_nmalloc(	char const *in, size_t n);
char	*str_malloc(	char const *in);
int	 str_free(	char **str);
size_t	 str_sizeof(	char const *str);
char	*str_ncpy(	char **str, char *in, size_t n);
char	*str_cpy(	char **str, char *in);
char	*str_ncat(	char **str, char *in, size_t n);
char	*str_cat(	char **str, char *in);

#  ifdef __cplusplus
	}
#  endif
#endif /* _STRMALLOC_H_ */
