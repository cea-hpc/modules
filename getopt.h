/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		getopt.h					     **
 **   Revision:		1.1						     **
 **   First Edition:	95/12/20					     **
 ** 									     **
 **   Authors:	Jens Hamisch, Jens.Hamisch@Strawberry.COM		     **
 ** 									     **
 **   Description:	getopt procedure for the Modules package	     **
 ** 									     **
 **   Exports:		getopt		Recognition of commadn line options  **
 ** 									     **
 **   Notes: This is based on the 'Getopt for GNU' from the gcc-2.7.2        **
 **          compiler. It is preferred to the libc version, because it       **
 **	     provides 'long-options'.					     **
 **									     **
 ** ************************************************************************ **
 ****/
/**									     **/
/**   Getopt for GNU.							     **/
/**   NOTE: getopt is now part of the C library, so if you don't know what   **/
/**   "Keep this file name-space clean" means, talk to roland@gnu.ai.mit.edu **/
/**   before changing it!						     **/
/**									     **/
/**   Copyright( C) 1987, 88, 89, 90, 91, 92, 93, 94, 95		     **/
/**   	Free Software Foundation, Inc.					     **/
/**									     **/
/**   This program is free software; you can redistribute it and/or modify   **/
/**   it under the terms of the GNU General Public License as published by   **/
/**   the Free Software Foundation; either version 2, or( at your option)    **/
/**   any later version.						     **/
/**									     **/
/**   This program is distributed in the hope that it will be useful,	     **/
/**   but WITHOUT ANY WARRANTY; without even the implied warranty of	     **/
/**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	     **/
/**   GNU General Public License for more details.			     **/
/**									     **/
/**   You should have received a copy of the GNU General Public License	     **/
/**   along with this program; if not, write to the Free Software 	     **/
/**   Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.		     **/
/** 									     **/
/** ************************************************************************ **/

/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1991-1994 by John L. Furlan.                      	     **
 ** see LICENSE.GPL, which must be provided, for details		     **
 ** 									     ** 
 ** ************************************************************************ **/

#ifndef _GETOPT_H
#define _GETOPT_H 1

#ifdef	__cplusplus
extern "C" {
#endif

/**
 **  For communication from `getopt' to the caller. When `getopt' finds an
 **  option that takes an argument, the argument value is returned here.
 **  Also, when `ordering' is RETURN_IN_ORDER, each non-option ARGV-element
 **  is returned here. 
 **/

extern char *optarg;

/**
 **  Index in ARGV of the next element to be scanned.
 **  This is used for communication to and from the caller
 **  and for communication between successive calls to `getopt'.
 **
 **  On entry to `getopt', zero means this is the first call; initialize.
 **
 **  When `getopt' returns EOF, this is the index of the first of the
 **  non-option elements that the caller should itself scan.
 **
 **  Otherwise, `optind' communicates from one call to the next
 **  how much of ARGV has been scanned so far.  
 **/

extern int optind;

/**
 **  Callers store zero here to inhibit the error message `getopt' prints
 **  for unrecognized options. 
 **/

extern int opterr;

/**
 **  Set to an option character which was unrecognized. 
 **/

extern int optopt;

/**
 **  Describe the long-named options requested by the application.
 **  The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
 **  of `struct option' terminated by an element containing a name which is
 **  zero.
 **
 **  The field `has_arg' is:
 **  no_argument		(or 0) if the option does not take an argument,
 **  required_argument	(or 1) if the option requires an argument,
 **  optional_argument 	(or 2) if the option takes an optional argument.
 **
 **  If the field `flag' is not NULL, it points to a variable that is set
 **  to the value given in the field `val' when the option is found, but
 **  left unchanged if the option is not found.
 **
 **  To have a long-named option do something other than set an `int' to
 **  a compiled-in constant, such as set a value from `optarg', set the
 **  option's `flag' field to zero and its `val' field to a nonzero
 **  value( the equivalent single-letter option character, if there is
 **  one).  For long options that have a zero `flag' field, `getopt'
 **  returns the contents of the `val' field. 
 **/

struct option {

#if defined( __STDC__) && __STDC__
    const char *name;
#else
    char *name;
#endif

    /**
     **  has_arg can't be an enum because some compilers complain about
     **  type mismatches in all the code that assumes it is an int. 
     **/

    int has_arg;
    int *flag;
    int val;
};

/**
 **  Names for the values of the `has_arg' field of `struct option'. 
 **/

#define	no_argument		0
#define required_argument	1
#define optional_argument	2

/**
 **  Many other libraries have conflicting prototypes for getopt, with
 **  differences in the consts, in stdlib.h.  To avoid compilation
 **  errors, only prototype getopt for the GNU C library. 
 **/

#if defined( __STDC__) && __STDC__

extern int getopt( int argc, char *const *argv, const char *shortopts);

extern int getopt_long( int argc, char *const *argv, const char *shortopts,
		        const struct option *longopts, int *longind);
extern int getopt_long_only( int argc, char *const *argv,
			     const char *shortopts,
		             const struct option *longopts, int *longind);

#else /* not __STDC__ */

extern int getopt();
extern int getopt_long();
extern int getopt_long_only();

#endif /* __STDC__ */

#ifdef	__cplusplus
}
#endif

#endif /* _GETOPT_H */
