#ifndef _IPRIME_H_
#  define _IPRIME_H_
/* 
 * RCSID @(#)$Id: iprime.h,v 1.1 2002/07/18 22:07:50 rkowen Exp $
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 2002 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/
#  ifdef __cplusplus
extern "C" {
#  endif

/* integer prime */
#define __IPRIME(NM, TYPE) TYPE NM(TYPE a);
__IPRIME(chprime,char)
__IPRIME(scprime,signed char)
__IPRIME(ucprime,unsigned char)
__IPRIME(hprime,short)
__IPRIME(uhprime,unsigned short)
__IPRIME(iprime,int)
__IPRIME(uiprime,unsigned int)
__IPRIME(lprime,long)
__IPRIME(ulprime,unsigned long)
#undef __IPRIME

#  ifdef __cplusplus
	}
#  endif
#endif /* _IPRIME_H_ */
