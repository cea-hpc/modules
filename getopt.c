/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		getopt.c					     **
 **   First Edition:	1995/12/20					     **
 ** 									     **
 **   Authors:	Jens Hamisch, jens@Strawberry.COM			     **
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
/**   "Keep this file name-space clean" means, talk to roland"@gnu.ai.mit.edu **/
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

static char Id[] = "@(#)$Id: getopt.c,v 1.3 2005/11/29 04:26:30 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				   CONFIGURATION			     **/
/** ************************************************************************ **/

/**
 **  This tells Alpha OSF/1 not to define a getopt prototype in <stdio.h>.
 **  Ditto for AIX 3.2 and <stdlib.h>.
 **/

#ifndef _NO_PROTO
#  define _NO_PROTO
#endif

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/**
 **  This is a separate conditional since some stdc systems
 **  reject `defined( const)'. 
 **/

#if !defined( __STDC__) || !__STDC__
#  ifndef const
#    define const
#  endif
#endif

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include <stdio.h>

/**
 **  Comment out all this code if we are using the GNU C Library, and are not
 **  actually compiling the library itself.  This code is part of the GNU C
 **  Library, but also included in many other GNU distributions.  Compiling
 **  and linking in this code is a waste when using the GNU C library
 ** ( especially if it is a shared library).  Rather than having every GNU
 **  program understand `configure --with-gnu-libc' and omit the object files,
 **  it is simpler to just do this in the source for each such file. 
 **
 ** All this conditional nonsense has been commented out, because this
 ** version of getopt has some module specific error handling that gets
 ** tested during the check.  It's a nice sentiment, but it doesn't buy
 ** you much to not include this code in.
 **/

/**
 **  This needs to come after some library #include
 **  to get __GNU_LIBRARY__ defined. 
 **/

/* #if defined( _LIBC) || !defined (__GNU_LIBRARY__) */

/**
 **  Don't include stdlib.h for non-GNU C libraries because some of them
 **  contain conflicting prototypes for getopt. 
 **/

#  ifdef	__GNU_LIBRARY__
#    include <stdlib.h>
#  endif	/* GNU C library.  */

/**
 **  This is for other GNU distributions with internationalized messages.
 **  When compiling libc, the _ macro is predefined. 
 **/

#  ifndef _
#    ifdef HAVE_LIBINTL_H
#     include <libintl.h>
#     define _(msgid)	gettext( msgid)
#    else
#     define _(msgid)	(msgid)
#    endif
#  endif

/**
 **  This version of `getopt' appears to the caller like standard Unix `getopt'
 **  but it behaves differently for the user, since it allows the user
 **  to intersperse the options with the other arguments.
 **
 **  As `getopt' works, it permutes the elements of ARGV so that,
 **  when it is done, all the options precede everything else.  Thus
 **  all application programs are extended to handle flexible argument order.
 **
 **  Setting the environment variable POSIXLY_CORRECT disables permutation.
 **  Then the behavior is completely standard.
 **
 **  GNU application programs can use a third alternative mode in which
 **  they can distinguish the relative order of options and other arguments. 
 **/

#  include "getopt.h"

/**
 **  We want to avoid inclusion of string.h with non-GNU libraries
 **  because there are many ways it can cause trouble.
 **  On some systems, it contains special magic macros that don't work
 **  in GCC. 
 **/

#  ifdef	__GNU_LIBRARY__
#    include <string.h>
#    define	my_index	strchr
#  else

/**
 **  Avoid depending on library functions or files whose names are
 **  inconsistent. 
 **/

char *getenv();

static char *my_index( const char *str, int chr)
{
    while( *str) {
	if( *str == chr)
	    return( char *) str;
	str++;
    }
    return( 0);
}

/**
 **  If using GCC, we can safely declare strlen this way. 
 **  If not using GCC, it is ok not to declare it. 
 **
 **  Note that Motorola Delta 68k R3V7 comes with GCC but not stddef.h.
 **  That was relevant to code that was here before.
 **/

#    ifdef __GNUC__
#      if !defined( __STDC__) || !__STDC__

/**
 **  gcc with -traditional declares the built-in strlen to return int,
 **  and has done so at least since version 2.4.5. -- rms. 
 **/

extern int strlen( const char *);
#      endif /* not __STDC__ */
#    endif /* __GNUC__ */

#  endif /* not __GNU_LIBRARY__ */

/**
 **  Include the modules header in order to get all error messages
 **/

#include "modules_def.h"

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

/** not applicable **/

/** ************************************************************************ **/
/** 				    GLOBAL DATA				     **/
/** ************************************************************************ **/

/**
 **  The following is required for compiling the TEST environment for the
 **  modules package
 **/

#ifdef	_MODULES_DEF_H
#  ifdef	TEST
char	*g_current_module = "getopt";
int	linenum = 0;
#  endif
#endif /** _MODULES_DEF_H **/

/**
 **  For communication from `getopt' to the caller.  When `getopt' finds an
 **  option that takes an argument, the argument value is returned here.
 **  Also, when `ordering' is RETURN_IN_ORDER, each non-option ARGV-element
 **  is returned here. 
 **/

char *optarg = NULL;

/**
 **  Index in ARGV of the next element to be scanned.  This is used for
 **  communication to and from the caller and for communication between
 **  successive calls to `getopt'.
 **
 **  On entry to `getopt', zero means this is the first call; initialize.
 **
 **  When `getopt' returns EOF, this is the index of the first of the
 **  non-option elements that the caller should itself scan.
 **
 **  Otherwise, `optind' communicates from one call to the next
 **  how much of ARGV has been scanned so far. 
 **/

/**
 **  XXX 1003.2 says this must be 1 before any call. 
 **/

int optind = 0;

/**
 **  Callers store zero here to inhibit the error message for unrecognized
 **  options. 
 **/

int opterr = 1;

/**
 **  Set to an option character which was unrecognized.
 **  This must be initialized on some systems to avoid linking in the
 **  system's own getopt implementation. 
 **/

int optopt = '?';

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

#ifdef	_MODULES_DEF_H

/**
 **  Runtime information for the modules package
 **/

static	char	module_name[] = "getopt.c";	/** File name of this module **/

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
static	char	_proc_exchange[] = "exchange";
static	char	_proc_getopt_initialize[] = "_getopt_initialize";
static	char	_proc_getopt_internal[] = " _getopt_internal";
static	char	_proc_getopt[] = "getopt";
static	char	_proc_getopt_long[] = "getopt_long";
static	char	_proc_getopt_long_only[] = "getopt_long_only";
#  endif
#endif

#  ifdef	TEST
static	char	_proc_main[] = "main";
#  endif

#endif /** _MODULES_DEF_H **/

/**
 **  The next char to be scanned in the option-element in which the last
 **  option character we returned was found.  This allows us to pick up
 **  the scan where we left off.
 **
 **  If this is zero, or a null string, it means resume the scan by advan-
 **  cing to the next ARGV-element. 
 **/

static char *nextchar;

/**
 **  Describe how to deal with options that follow non-option ARGV-elements.
 **
 **  If the caller did not specify anything, the default is REQUIRE_ORDER if
 **  the environment variable POSIXLY_CORRECT is defined, PERMUTE otherwise.
 **
 **  REQUIRE_ORDER means don't recognize them as options;
 **  stop option processing when the first non-option is seen.
 **  This is what Unix does.
 **  This mode of operation is selected by either setting the environment
 **  variable POSIXLY_CORRECT, or using `+' as the first character
 **  of the list of option characters.
 **
 **  PERMUTE is the default.  We permute the contents of ARGV as we scan,
 **  so that eventually all the non-options are at the end.  This allows options
 **  to be given in any order, even with programs that were not written to
 **  expect this.
 **
 **  RETURN_IN_ORDER is an option available to programs that were written
 **  to expect options and other ARGV-elements in any order and that care about
 **  the ordering of the two.  We describe each non-option ARGV-element
 **  as if it were the argument of an option with character code 1.
 **  Using `-' as the first character of the list of option characters
 **  selects this mode of operation.
 **
 **  The special argument `--' forces an end of option-scanning regardless
 **  of the value of `ordering'.  In the case of RETURN_IN_ORDER, only
 **  `--' can cause `getopt' to return EOF with `optind' != ARGC. 
 **/

static enum {
    REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
} ordering;

/**
 **  Value of POSIXLY_CORRECT environment variable. 
 **/

static char *posixly_correct;

/**
 **  Handle permutation of arguments. 
 **/

/**
 **  Describe the part of ARGV that contains non-options that have
 **  been skipped.  `first_nonopt' is the index in ARGV of the first of them;
 **  `last_nonopt' is the index after the last of them. 
 **/

static int first_nonopt;
static int last_nonopt;

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static void exchange( char **argv);
static const char *_getopt_initialize( const char *optstring);
static	int _getopt_internal(	int			 argc,
				char *const	 	*argv,
				const char	 	*optstring,
				const struct option	*longopts,
				int			*longind,
				int			 long_only);

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		exchange					     **
 ** 									     **
 **   Description:	Exchange two adjacent subsequences of ARGV.	     **
 **			One subsequence is elements( first_nonopt,	     **
 **			last_nonopt) which contains all the non-options that **
 **			have been skipped so far. The other is elements	     **
 **			(last_nonopt,optind), which contains all the options **
 **			processed since those non-options were skipped.	     **
 **									     **
 **  			`first_nonopt' and `last_nonopt' are relocated so    **
 **			that they describe the new indices of the non-options**
 **			in ARGV after they are moved. 			     **
 ** 									     **
 **   First Edition:	1995/12/20					     **
 ** 									     **
 **   Parameters:	char	**argv		Command line arguments	     **
 ** 									     **
 **   Result:		argv		ARGV array with exchanged sequences  **
 ** 									     **
 **   Attached Globals:	first_nonopt	first and last non option argument   **
 **			last_nonopt	on the stream before and after the   **
 **					change.( I/O parameter)		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static void exchange( char **argv)
{
    int bottom = first_nonopt;
    int middle = last_nonopt;
    int top = optind;
    char *tem;

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_exchange, NULL);
#  endif
#endif

    /**
     **  Exchange the shorter segment with the far end of the longer segment.
     **  That puts the shorter segment into the right place.
     **  It leaves the longer segment in the right place overall,
     **  but it consists of two parts that need to be swapped next. 
     **/

    while( top > middle && middle > bottom) {
	if( top - middle > middle - bottom) {

	    /**
	     ** Bottom segment is the short one. 
	     **/

	    int len = middle - bottom;
	    register int i;

	    /**
	     **  Swap it with the top part of the top segment. 
	     **/

	    for( i = 0; i < len; i++) {
		tem = argv[bottom + i];
		argv[bottom + i] = argv[top -( middle - bottom) + i];
		argv[top -( middle - bottom) + i] = tem;
	    }

	    /**
	     **  Exclude the moved bottom segment from further swapping. 
	     **/

	    top -= len;

	} else {
	    
	    /**
	     **  Top segment is the short one. 
	     **/

	    int len = top - middle;
	    register int i;

	    /**
	     **  Swap it with the bottom part of the bottom segment. 
	     **/

	    for( i = 0; i < len; i++) {
		tem = argv[bottom + i];
		argv[bottom + i] = argv[middle + i];
		argv[middle + i] = tem;
	    }

	    /**
	     **  Exclude the moved top segment from further swapping. 
	     **/

	    bottom += len;
	}

    } /** while **/

    /**
     **  Update records for the slots the non-options now occupy. 
     **/

    first_nonopt +=( optind - last_nonopt);
    last_nonopt = optind;

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_END, LOC, _proc_exchange, NULL);
#  endif
#endif

} /** end of 'exchange' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		_getopt_initialize				     **
 ** 									     **
 **   Description:	Initialize the internal data when the first call is  **
 **			made						     **
 **			This defines how to proceed if options and non-op-   **
 **			tions are mixed up. See definition of the enum       **
 **			'ordering' for further explanation.		     **
 ** 									     **
 **   First Edition:	1995/12/20					     **
 ** 									     **
 **   Parameters:	char	*optstring	Options string		     **
 ** 									     **
 **   Result:		argv		ARGV array with exchanged sequences  **
 **									     **
 **   Attached globals: first_nonopt,		First an las position of     **
 **			last_nonopt		non-option arguments	     **
 **			optind			Option scan index	     **
 **			nextchar		Next character to scan	     **
 **			posixly_correct		Value of the environment     **
 **						variable 'POSIXLY_CORRECT'   **
 **			ordering		Ordering method ...	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static const char *_getopt_initialize( const char *optstring)
{

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_getopt_initialize, NULL);
#  endif
#endif

    /**
     **  Start processing options with ARGV-element 1( since ARGV-element 0
     **  is the program name); the sequence of previously skipped
     **  non-option ARGV-elements is empty. 
     **/

    first_nonopt = last_nonopt = optind = 1;
    nextchar = NULL;
    posixly_correct = getenv( "POSIXLY_CORRECT");

    /**
     **  Determine how to handle the ordering of options and nonoptions. 
     **/

    if( optstring[0] == '-') {
	ordering = RETURN_IN_ORDER;
	++optstring;

    } else if( optstring[0] == '+') {
	ordering = REQUIRE_ORDER;
	++optstring;

    } else if( posixly_correct != NULL)
	ordering = REQUIRE_ORDER;

    else
	ordering = PERMUTE;

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_END, LOC, _proc_getopt_initialize, NULL);
#  endif
#endif

    return( optstring);

} /** End of '_getopt_initialize' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		_getopt_internal				     **
 ** 									     **
 **   Description:	Scan elements of ARGV( whose length is ARGC) for     **
 **			option characters given in OPTSTRING.	  	     **
 **			or long-options specified in the longopt array	     **
 ** 									     **
 **   First Edition:	1995/12/20					     **
 ** 									     **
 **   Parameters:	int		  argc,		# of arguments	     **
 **			char		**argv,		ARGV array	     **
 **			char	 	 *optstring,	String of valid      **
 **							short options	     **
 **			struct option	 *longopts,	Table of valid long  **
 **							options		     **
 **			int		 *longind,	Returns the index of **
 **							the found long opt.  **
 **			int		  long_only	Search long options  **
 **							only		     **
 ** 									     **
 **   Result:		int	'?'	Parse error			     **
 **				0	Long option w/o argument found	     **
 **				else	short option that has been found     **
 **					or the value of a long option	     **
 **				EOF	no more arguments on ARGV	     **
 **									     **
 **   Attached globals:	optind		Index of the current option in the   **
 **					ARGV array			     **
 **			optarg		Argument of an option with value     **
 **									     **
 ** ************************************************************************ **
 ++++*/
/** 									     **/
/**   If an element of ARGV starts with '-', and is not exactly "-" or "--", **/
/**   then it is an option element.  The characters of this element 	     **/
/**   ( aside from the initial '-') are option characters.  If `getopt'      **/
/**   is called repeatedly, it returns successively each of the option 	     **/
/**   characters from each of the option elements. 			     **/
/** 									     **/
/**   If `getopt' finds another option character, it returns that character, **/
/**   updating `optind' and `nextchar' so that the next call to `getopt' can **/
/**   resume the scan with the following option character or ARGV-element.   **/
/** 									     **/
/**   If there are no more option characters, `getopt' returns `EOF'. 	     **/
/**   Then `optind' is the index in ARGV of the first ARGV-element 	     **/
/**   that is not an option. ( The ARGV-elements have been permuted 	     **/
/**   so that those that are not options now come last.) 		     **/
/** 									     **/
/**   OPTSTRING is a string containing the legitimate option characters.     **/
/**   If an option character is seen that is not listed in OPTSTRING, 	     **/
/**   return '?' after printing an error message.  If you set `opterr' to    **/
/**   zero, the error message is suppressed but we still return '?'. 	     **/
/** 									     **/
/**   If a char in OPTSTRING is followed by a colon, that means it wants an  **/
/**   arg, so the following text in the same ARGV-element, or the text of    **/
/**   the following ARGV-element, is returned in `optarg'.  Two colons mean  **/
/**   an option that wants an optional arg; if there is text in the current  **/
/**   ARGV-element, it is returned in `optarg', otherwise `optarg' is set to **/
/**   zero. 								     **/
/** 									     **/
/**   If OPTSTRING starts with `-' or `+', it requests different methods of  **/
/**   handling the non-option ARGV-elements. 				     **/
/**   See the comments about RETURN_IN_ORDER and REQUIRE_ORDER, above. 	     **/
/** 									     **/
/**   Long-named options begin with `--' instead of `-'. 		     **/
/**   Their names may be abbreviated as long as the abbreviation is unique   **/
/**   or is an exact match for some defined option.  If they have an 	     **/
/**   argument, it follows the option name in the same ARGV-element, 	     **/
/**   separated from the option name by a `=', or else the in next ARGV-     **/
/**   element.  When `getopt' finds a long-named option, it returns 0 if     **/
/**   that option's `flag' field is nonzero, the value of the option's `val' **/
/**   field if the `flag' field is zero. 				     **/
/** 									     **/
/**   The elements of ARGV aren't really const, because we permute them.     **/
/**   But we pretend they're const in the prototype to be compatible 	     **/
/**   with other systems. 						     **/
/** 									     **/
/**   LONGOPTS is a vector of `struct option' terminated by an 	  	     **/
/**   element containing a name which is zero. 				     **/
/** 									     **/
/**   LONGIND returns the index in LONGOPT of the long-named option found.   **/
/**   It is only valid when a long-named option has been found by the most   **/
/**   recent call. 							     **/
/** 									     **/
/**   If LONG_ONLY is nonzero, '-' as well as '--' can introduce 	     **/
/**   long-named options.  						     **/
/** 									     **/
/** ************************************************************************ **/

static	int _getopt_internal(	int			 argc,
				char *const	 	*argv,
				const char	 	*optstring,
				const struct option	*longopts,
				int			*longind,
				int			 long_only)
{
    optarg = NULL;

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_getopt_internal, NULL);
#  endif
#endif

    /**
     **  Initialization
     **/

    if( optind == 0) {
	optstring = _getopt_initialize( optstring);
	optind = 1;		/** Don't scan ARGV[0], the program name.    **/
    }

    if( nextchar == NULL || *nextchar == '\0') {

	/**
	 **  Advance to the next ARGV-element.
	 **/

	if( ordering == PERMUTE) {

	    /**
	     **  If we have just processed some options following some non-
	     **  options, exchange them so that the options come first. 
	     **/

	    if( first_nonopt != last_nonopt && last_nonopt != optind)
		exchange( (char **) argv);
	    else if( last_nonopt != optind)
		first_nonopt = optind;

	    /**
	     **  Skip any additional non-options and extend the range of
	     **  non-options previously skipped. 
	     **/

	    while( optind < argc &&
		  ( argv[optind][0] != '-' || argv[optind][1] == '\0'))
		optind++;

	    last_nonopt = optind;
	}

	/**
	 **  The special ARGV-element `--' means premature end of options.
	 **  Skip it like a null option, then exchange with previous non-
	 **  options **  as if it were an option, then skip everything else
	 **  like a non-option. 
	 **/

	if( optind != argc && !strcmp( argv[optind], "--")) {

	    optind++;

	    if( first_nonopt != last_nonopt && last_nonopt != optind)
		exchange((char **) argv);
	    else if( first_nonopt == last_nonopt)
		first_nonopt = optind;

	    last_nonopt = argc;

	    optind = argc;
	}

	/**
	 **  If we have done all the ARGV-elements, stop the scan and back
	 **  over any non-options that we skipped and permuted. 
	 **/

	if( optind == argc) {

	    /**
	     **  Set the next-arg-index to point at the non-options that we
	     **  previously skipped, so the caller will digest them. 
	     **/

	    if( first_nonopt != last_nonopt)
		optind = first_nonopt;

	    return( EOF);
	}

	/**
	 **  If we have come to a non-option and did not permute it,
	 **  either stop the scan or describe it to the caller and pass it by. 
	 **/

	if( argv[optind][0] != '-' || argv[optind][1] == '\0') {

	    if( ordering == REQUIRE_ORDER)
		return EOF;
	    optarg = argv[optind++];

	    return( 1);
	}

	/**
	 **  We have found another option-ARGV-element. 
	 **  Skip the initial punctuation. 
	 **/

	nextchar =( argv[optind] + 1 +
	   ( longopts != NULL && argv[optind][1] == '-'));
    }

    /**
     **    Decode the current option-ARGV-element. 
     **/

    /**
     **  Check whether the ARGV-element is a long option.
     **
     **  If long_only and the ARGV-element has the form "-f", where f is
     **  a valid short option, don't consider it an abbreviated form of
     **  a long option that starts with f.  Otherwise there would be no
     **  way to give the -f short option.
     **
     **  On the other hand, if there's a long option "fubar" and
     **  the ARGV-element is "-fu", do consider that an abbreviation of
     **  the long option, just like "--fu", and not "-f" with arg "u".
     **
     **  This distinction seems to be the most useful approach. 
     **/

    if( longopts != NULL &&( argv[optind][1] == '-'     ||
       (  long_only &&
	 (  argv[optind][2] || !my_index (optstring, argv[optind][1]))))) {

	char *nameend;
	const struct option *p;
	const struct option *pfound = NULL;
	int exact = 0;
	int ambig = 0;
	int indfound;
	int option_index;

	/**
	 **  Skip the remaining characters of the long option upt to its
	 **  names end( End of the option itsself or the '=' sign)
	 **/

	for( nameend = nextchar; *nameend && *nameend != '='; nameend++);

	/**
	 **  Test all long options for either exact match or abbreviated
	 **  matches.
	 **/

	for( p = longopts, option_index = 0; p->name; p++, option_index++) {
	    if( !strncmp( p->name, nextchar, nameend - nextchar)) {

		if( nameend - nextchar == strlen( p->name)) {

		    /**
		     ** Exact match found. 
		     **/

		    pfound = p;
		    indfound = option_index;
		    exact = 1;
		    break;

		} else if( pfound == NULL) {

		    /**
		     **  First nonexact match found.
		     **/

		    pfound = p;
		    indfound = option_index;

		} else

		    /**
		     **  Second or later nonexact match found. 
		     **/

		    ambig = 1;

	    } /** if( !strncmp) **/
	} /** for **/

	/**
	 **  Print an error message for ambigious abbreviations and exit
	 **  on error
	 **/

	if( ambig && !exact) {

	    if( opterr)
#ifdef	_MODULES_DEF_H
		ErrorLogger( ERR_OPT_AMBIG, LOC, argv[optind], NULL);
#else
		fprintf( stderr, _("%s: option `%s' is ambiguous\n"),
		    argv[0], argv[optind]);
#endif

	    nextchar += strlen( nextchar);
	    optind++;
	    return( '?');
	}

	/**
	 **  Longname found ?
	 **/

	if( pfound != NULL) {

	    option_index = indfound;
	    optind++;

	    /**
	     **  *nameend is != NULL if there is a value specified for
	     **  the option: '--option=value' -> *nameend = '='
	     **/

	    if( *nameend) {

		/**
		 **  Don't test has_arg with >, because some C compilers don't
		 **  allow it to be used on enums. 
		 **/

		if( pfound->has_arg)
		    optarg = nameend + 1;

		else {
		    	  
		    if( opterr)

			/**
			 **  ERROR: --option w/o argument
			 **/

			if( argv[optind - 1][1] == '-')
#ifdef	_MODULES_DEF_H
			    ErrorLogger( ERR_OPT_NOARG, LOC, pfound->name, NULL);
#else
			    fprintf( stderr,
				_("%s: option `--%s' doesn't allow an argument\n"),
				argv[0], pfound->name);
#endif

			/**
			 **  ERROR: +option or -option w/o argument
			 **/

			else {
#ifdef	_MODULES_DEF_H
			    char buffer[ BUFSIZ];
			    sprintf( buffer, "%c%s", argv[optind - 1][0], pfound->name);
			    ErrorLogger( ERR_OPT_NOARG, LOC, buffer, NULL);
#else
			    fprintf( stderr,
				_("%s: option `%c%s' doesn't allow an argument\n"),
				argv[0], argv[optind - 1][0], pfound->name);
#endif
			}

		    nextchar += strlen( nextchar);
		    return( '?');
		}

	    /**
	     **  Options with arguments
	     **/

	    } else if( pfound->has_arg == 1) {

		if( optind < argc)
		    optarg = argv[optind++];

		else {

		    /**
		     **  ERROR: Option without argument where one is required
		     **/

		    if( opterr)
#ifdef	_MODULES_DEF_H
			ErrorLogger( ERR_OPT_REQARG, LOC, argv[optind-1], NULL);
#else
			fprintf( stderr,
			    _("%s: option `%s' requires an argument\n"),
			    argv[0], argv[optind - 1]);
#endif

		    nextchar += strlen( nextchar);
		    return((optstring[0] == ':') ? ':' : '?');
		}
	    }

	    /** 
	     **  Return the indication, that a long vlaue has been found
	     **  and set up pointers for the next option expansion
	     **/

	    nextchar += strlen( nextchar);

	    if( longind != NULL)
		*longind = option_index;

	    if( pfound->flag) {
		*(pfound->flag) = pfound->val;
		return( 0);
	    }
	    return( pfound->val);
	}

	/**
	 **  Can't find it as a long option.  If this is not getopt_long_only,
	 **  or the option starts with '--' or is not a valid short
	 **  option, then it's an error.
	 **  Otherwise interpret it as a short option. 
	 **/

	if( !long_only || argv[optind][1] == '-' ||
	    my_index( optstring, *nextchar) == NULL) {

	    if( opterr) {

		/**
		 ** ERROR: unrecognized --option 
		 **/

		if( argv[optind][1] == '-') {
#ifdef	_MODULES_DEF_H
		    ErrorLogger( ERR_OPT_UNKNOWN, LOC, nextchar, NULL);
#else
		    fprintf( stderr, _("%s: unrecognized option `--%s'\n"),
			argv[0], nextchar);
#endif

		/**
		 ** ERROR: unrecognized +option or -option
		 **/

		} else {
#ifdef	_MODULES_DEF_H
		    char buffer[ BUFSIZ];
		    sprintf( buffer, "%c%s", argv[optind][0], nextchar);
		    ErrorLogger( ERR_OPT_AMBIG, LOC, buffer, NULL);
#else
		    fprintf( stderr, _("%s: unrecognized option `%c%s'\n"),
			argv[0], argv[optind][0], nextchar);
#endif
		}
	    }

	    nextchar =( char *) "";
	    optind++;
	    return( '?');
	}
    } /** if( long option) **/

    /**
     **  Look at and handle the next short option-character. 
     **/

    {
	char c = *nextchar++;
	char *temp = my_index( optstring, c);

	/**
	 **  Increment `optind' when we start to process its last character. 
	 **/

	if( *nextchar == '\0')
	    ++optind;

	/**
	 **  Unrecognized options
	 **/

	if( temp == NULL || c == ':') {
	    if( opterr) {

#ifdef	_MODULES_DEF_H
		    char buffer[ 2];
		    buffer[ 0] = c;
		    buffer[ 1] = '\0';
#endif

		if( posixly_correct) {

		    /**
		     **  1003.2 specifies the format of this message. 
		     **/

#ifdef	_MODULES_DEF_H
		    ErrorLogger( ERR_OPT_ILL, LOC, buffer, NULL);
#else
		    fprintf( stderr, _("%s: illegal option -- %c\n"),
			argv[0], c);
#endif
		} else {
#ifdef	_MODULES_DEF_H
		    ErrorLogger( ERR_OPT_INV, LOC, buffer, NULL);
#else
		    fprintf( stderr, _("%s: invalid option -- %c\n"),
			argv[0], c);
#endif
		}
	    }
	    optopt = c;
	    return( '?');
	}

	/**
	 **
	 **/

	if( temp[1] == ':') {
	    if( temp[2] == ':') {

		/**
		 **  This is an option that accepts an argument optionally.
		 **/

		if( *nextchar != '\0') {
		    optarg = nextchar;
		    optind++;
		} else
		    optarg = NULL;

		nextchar = NULL;

	    } else {	/** optional argument **/

		/**
		 **  This is an option that requires an argument. 
		 **/

		if( *nextchar != '\0') {
		    optarg = nextchar;

		    /**
		     **  If we end this ARGV-element by taking the rest as an arg,
		     **  we must advance to the next element now. 
		     **/

		    optind++;

		} else if( optind == argc) {

		    if( opterr) {

			/**
			 **  1003.2 specifies the format of this message. *
			 **/

#ifdef	_MODULES_DEF_H
			char buffer[ 2];
			buffer[ 0] = c;
			buffer[ 1] = '\0';
			ErrorLogger( ERR_OPT_REQARG, LOC, buffer, NULL);
#else
			fprintf( stderr,
			    _("%s: option requires an argument -- %c\n"),
				argv[0], c);
#endif
		    }

		    optopt = c;
		    if( optstring[0] == ':')
			c = ':';
		    else
			c = '?';

		} else

		    /**
		     **  We already incremented `optind' once;
		     **  increment it again when taking next ARGV-elt as argument.
		     **/

		    optarg = argv[optind++];
		    nextchar = NULL;
	    }
	}

	/**
	 **  Now c contains the found character in case of success of '?' in case
	 **  of failure
	 **/

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
	ErrorLogger( NO_ERR_END, LOC, _proc_getopt_internal, NULL);
#  endif
#endif
	return( c);

    } /** block **/

} /** End of '_getopt_internal' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		getopt, getopt_long, getopt_long_only		     **
 ** 									     **
 **   Description:	Calls _getopt_internal in order to provide a normal  **
 **			getopt call.					     **
 ** 									     **
 **   First Edition:	1995/12/20					     **
 ** 									     **
 **   Parameters:	int	  argc,		# of arguments		     **
 **			char	**argv,		ARGV array		     **
 **			char 	 *optstring,	String of valid short opt.   **
 ** 									     **
 **   Result:		int	'?'	Parse error			     **
 **				0	Long option w/o argument found	     **
 **				else	short option that has been found     **
 **					or the value of a long option	     **
 **				EOF	no more arguments on ARGV	     **
 **									     **
 **   Attached globals:	optind		Index of the current option in the   **
 **					ARGV array			     **
 **			optarg		Argument of an option with value     **
 **			opterr		Set in case of parse errors	     **
 **									     **
 ** ************************************************************************ **
 ++++*/

int getopt( int argc, char *const *argv, const char *optstring)
{

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_getopt, NULL);
#  endif
#endif

    return _getopt_internal( argc, argv, optstring,
			     ( const struct option *) 0,
			     ( int *) 0,
			      0);
} /** End of 'getopt' **/

int getopt_long( int argc, char *const *argv, const char *optstring,
		 const struct option *longopts, int *longind)
{

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_getopt_long, NULL);
#  endif
#endif

    return _getopt_internal( argc, argv, optstring, longopts, longind, 0);

} /** End of 'getopt' **/

int getopt_long_only( int argc, char *const *argv, const char *optstring,
		      const struct option *longopts, int *longind)
{

#ifdef _MODULES_DEF_H
#  if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_getopt_long_only, NULL);
#  endif
#endif

    return _getopt_internal( argc, argv, optstring, longopts, longind, 1);

} /** End of 'getopt' **/

/* #endif	*/ /* _LIBC or not __GNU_LIBRARY__.  */

#ifdef TEST

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		main						     **
 ** 									     **
 **   Description:	Test procedure for getopt			     **
 **			Compile with -DTEST to make an executable for use in **
 **			testing the above definition of `getopt'	     **
 ** 									     **
 **   First Edition:	1995/12/20					     **
 ** 									     **
 **   Parameters:	int	  argc,		# of arguments		     **
 **			char	**argv,		ARGV array		     **
 ** 									     **
 **   Result:		-						     **
 **									     **
 ** ************************************************************************ **
 ++++*/

int main( int argc, char **argv)
{

#  ifdef _MODULES_DEF_H
#    if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_START, LOC, _proc_main, NULL);
#    endif
#  endif

    int c;
    int digit_optind = 0;
    int longind;
    int option, verbose;
    char *value;

    const struct option longopts[] = {
	{ "test", no_argument, NULL, 0 },
	{ "option", optional_argument, &option, 1 },
	{ "verbose", optional_argument, NULL, 'v' },
	{ "value", required_argument, NULL, 'x'},
	{ NULL, no_argument, NULL, 0 }
    };

    /**
     **  Print all options ...
     **/

    while( 1) {

	int this_option_optind = optind ? optind : 1;

	c = getopt_long( argc, argv, "abc:d:0123456789", longopts, &longind);
	if( c == EOF)
	    break;  /** while( 1) **/

	switch( c) {

	    case 0:
		printf( "option --test or --option\n");
		if( optarg)
		    printf( "with argument '%s'", optarg);
		printf( "\n   option is set to %d\n", option);
		break;

	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		if( digit_optind != 0 && digit_optind != this_option_optind)
		    printf( "digits occur in two different argv-elements.\n");
		digit_optind = this_option_optind;
		    printf( "option %c\n", c);
		break;

	    case 'a':
		printf( "option a\n");
		break;

	    case 'b':
		printf( "option b\n");
		break;

	    case 'c':
		printf( "option c with value `%s'\n", optarg);
		break;

	    case 'v':
	    case 'x':
		printf( "option %c ", c);
		if( optarg)
		    printf( "with argument '%s'", optarg);
		printf( "\n");
		break;

	    case '?':
		break;

	    default:
		printf( "?? getopt returned character code 0%o ??\n", c);
	}
    }  /** while( 1) **/

    /**
     **  Finally print all remaining arguments
     **/

    if( optind < argc) {
	printf( "non-option ARGV-elements: ");
	while( optind < argc)
	    printf( "%s ", argv[optind++]);
	printf( "\n");
    }

    /**
     **  Exit on success
     **/

#  ifdef _MODULES_DEF_H
#    if WITH_DEBUGGING_INIT
    ErrorLogger( NO_ERR_END, LOC, _proc_main, NULL);
#    endif
#  endif

    exit( 0);

} /** End of 'main' **/

#endif /* TEST */
