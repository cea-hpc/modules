/*****
 ** ** Module Header ******************************************************* **
 ** 									     **
 **   Modules Revision 3.0						     **
 **   Providing a flexible user environment				     **
 ** 									     **
 **   File:		cmdXResource.c					     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Authors:	John Furlan, jlf@behere.com				     **
 **		Leif Hedstrom <hedstrom@boot.org>			     **
 **		Jens Hamisch, jens@Strawberry.COM			     **
 **		R.K. Owen, <rk@owen.sj.ca.us> or <rkowen@nersc.gov>	     **
 ** 									     **
 **   Description:	Module command to merge/remove resources from the X11**
 **			resource manager. The database is update internally, **
 **			ie. its not done at evaluation of string modulecmd   **
 **			returns. It will do something like "xrdb -merge"     **
 **			using the default display ($DISPLAY).		     **
 ** 									     **
 **   Exports:		xresourceFinish					     **
 **			cmdXResource					     **
 ** 									     **
 **   Notes:		Fragments of this code are from the original xrdb    **
 **			source, Copyright 1987 & 1991 by DIGITAL EQUIPMENT   **
 **			CORPORATION. Xrdb was written and modified by:	     **
 ** 									     **
 **				Jim Gettys, August 28, 1987		     **
 **				Phil Karlton, January 5, 1987		     **
 **				Bob Scheifler, February, 1991		     **
 ** 									     **
 **   ToDo/Bugs:	+ The command only handles screen independant re-    **
 **                       sources.                                           **
 ** ************************************************************************ **
 ****/

/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1991-1994 by John L. Furlan.                      	     **
 ** see LICENSE.GPL, which must be provided, for details		     **
 ** 									     ** 
 ** ************************************************************************ **/

static char Id[] = "@(#)$Id: cmdXResource.c,v 1.17 2010/10/08 21:40:19 rkowen Exp $";
static void *UseId[] = { &UseId, Id };

/** ************************************************************************ **/
/** 				      HEADERS				     **/
/** ************************************************************************ **/

#include "modules_def.h"

#ifdef HAS_X11LIBS
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

/** ************************************************************************ **/
/** 				  LOCAL DATATYPES			     **/
/** ************************************************************************ **/

#ifdef HAS_X11LIBS
typedef struct _ResourceDB {
    Tcl_HashTable *data;
    Window root;
    Atom prop;
} ResourceDB;
#endif

/** ************************************************************************ **/
/** 				     CONSTANTS				     **/
/** ************************************************************************ **/

#ifndef R_OK
#define F_OK		0		/** does file exist		     **/
#define X_OK		1		/** is it executable by caller	     **/
#define W_OK		2		/** is it writable by caller	     **/
#define R_OK		4		/** is it readable by caller	     **/
#endif

/** ************************************************************************ **/
/**				      MACROS				     **/
/** ************************************************************************ **/

#ifdef HAS_X11LIBS
#define MAXHOSTNAME	 255
#define Resolution( pixels, mm)	(((pixels * 100000 / mm) + 50) / 100)
#endif

/** ************************************************************************ **/
/** 				    LOCAL DATA				     **/
/** ************************************************************************ **/

static	char	module_name[] = __FILE__;

#ifdef HAS_X11LIBS
static Display		*dpy		= (Display *) NULL;
static char		*defines	= (char *) NULL;
static int		def_base	= 0;
static Tcl_DString	*buffer		= (Tcl_DString *) NULL;
static ResourceDB	resDB		= { NULL, (Window) 0, (Atom) 0 };
#endif

/** ************************************************************************ **/
/**				    PROTOTYPES				     **/
/** ************************************************************************ **/

static	void	addDef(	char*, char*);
static	void	addNum(	char*, int);
static	void	doDisplayDefines(void);
static	void	doScreenDefines( int);
static	int	readFile( register FILE	*, int);
static	ErrType getEntries(Tcl_Interp*,  Tcl_HashTable*, register char*, int);
#ifdef HAS_X11LIBS
static	void	storeResProp( register ResourceDB* );
#endif
static	ErrType getOld( register char**);
static	ErrType	initBuffers(Tcl_Interp*, register int );

#ifdef HAS_X11LIBS

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		addDef, addNum					     **
 ** 									     **
 **   Description:	Adds DEFINES to the define buffer. This code is main-**
 **			ly the same as in the original xrdb.c		     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	char	*title		Name of the resource	     **
 **			char	*value		and its value		     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	defines		Buffer for all DEFINES which will be **
 **					written here in command lien syntax: **
 **					   -D <titel>=<value>		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	void	addDef(	char	*title,
			char	*value)
{
    register int quote;

    /**
     **  Add '-D title' at first
     **/

    if( title && *title) {

	strcat( defines, " -D");
	strcat( defines, title);

	/**
	 **  Add the value if there is one
	 **/

	if( value && *value) {

	    quote = (value && strchr( value, ' '));
	    strcat( defines, (quote ? "=\"" : "="));
	    strcat( defines, value);
	    if( quote)
		strcat( defines,"\"");

	} /** if( value) **/
    } /** if( title) **/

} /** End of 'addDef' **/

static	void	addNum(	char	*title,
			int	 value)
{
    char num[ 20];

    sprintf( num, "%d", value);
    addDef( title, num);

} /** End of 'addNum' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		doDisplayDefines				     **
 ** 									     **
 **   Description:	Put the client and server specific defines on the    **
 **			define buffer					     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	-						     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	dpy		For seeking the name of the display  **
 **			defines		(via addDef and addNum)	  	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	void	doDisplayDefines()
{
    char	 client[ MAXHOSTNAME],		/** X client name buffer     **/
		 server[ MAXHOSTNAME],		/** X server name buffer     **/
		*colon;				/** Pointer for seeking the  **/
						/** colon in the server name **/
    /**
     **  Get client and server hostname. Remove everything after the ':' from
     **  the server name. If there's no server name available, the server de-
     **  faults to the client.
     **/

    gethostname(client,MAXHOSTNAME);
    strcpy( server, XDisplayName( NULL));

    if( (colon = strchr( server, ':')) )
	*colon = '\0';
    if( !*server)
	strcpy( server, client);

    /**
     **  Add the standard defines now ...
     **/

    addDef( "HOST", server);
    addDef( "SERVERHOST", server);
    addDef( "CLIENTHOST", client);
    addNum( "VERSION", ProtocolVersion( dpy));
    addNum( "REVISION", ProtocolRevision( dpy));
    addDef( "VENDOR", ServerVendor( dpy));
    addNum( "RELEASE", VendorRelease( dpy));

} /** End of 'doDisplayDefines' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		doScreenDefines					     **
 ** 									     **
 **   Description:	Put the screen specific defines on the define buffer **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	int	scrno	Screen number			     **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	dpy		For seeking the name of the display  **
 **			defines		(via addDef and addNum)	  	     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	void	 doScreenDefines( int	scrno)
{
    register Screen	*screen;
    register Visual	*visual;
    
    /**
     **  Get screen data at first
     **/

    screen = ScreenOfDisplay( dpy, scrno);
    visual = DefaultVisualOfScreen( screen);

    /**
     **  Put screen data on the defines buffer
     **/

    addNum( "WIDTH", screen->width);
    addNum( "HEIGHT", screen->height);
    addNum( "X_RESOLUTION", Resolution( screen->width, screen->mwidth));
    addNum( "Y_RESOLUTION", Resolution( screen->height, screen->mheight));
    addNum( "PLANES", DisplayPlanes( dpy, scrno));
    addNum( "BITS_PER_RGB", visual->bits_per_rgb);

    /**
     **  The CLASS and COLOR do depend on the screen class
     **/

    switch(visual->class) {

	case StaticGray:	addDef( "CLASS", "StaticGray");
				break;

	case GrayScale:		addDef( "CLASS", "GrayScale");
				break;

	case StaticColor:	addDef( "CLASS", "StaticColor");
				addDef( "COLOR", NULL);
				break;

	case PseudoColor:	addDef( "CLASS", "PseudoColor");
				addDef( "COLOR", NULL);
				break;

	case TrueColor:		addDef( "CLASS", "TrueColor");
				addDef( "COLOR", NULL);
				break;

	case DirectColor:	addDef( "CLASS", "DirectColor");
				addDef( "COLOR", NULL);
				break;
    } /** switch **/

} /** End of 'doScreenDefines' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		readFile					     **
 ** 									     **
 **   Description:	Read resource from a file, which normally is a pipe  **
 **			opened with popen.				     **
 **			The file will be closed, when reading is finished    **
 **									     **
 **   Note:		This routine uses the global variable 'line', declar-**
 **			ed in another file!!!				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	register FILE	*input	The stream to be read from   **
 **			int		 do_cpp	Differs between a pipe or a  **
 **						file being assigned to input **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	line		Buffer for a line to be read	     **
 **			buffer		Buffer for the whole resource file  **
 **					image				     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	int	readFile(	register FILE	*input,
				int		 do_cpp)
{
    register int bytes;

    while( !feof( input) && (bytes = fread( line, 1, LINELENGTH, input)))
	Tcl_DStringAppend( buffer, line, bytes);

    if( do_cpp) {
	if( -1 == pclose( input))
	    if( OK != ErrorLogger( ERR_PCLOSE, LOC, NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
    } else {
	if( EOF == fclose( input))
	    if( OK != ErrorLogger( ERR_CLOSE, LOC, NULL))
		return( TCL_ERROR);	/** -------- EXIT (FAILURE) -------> **/
    }

    return( TCL_OK);

} /**  End of 'readFile' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		getEntries					     **
 ** 									     **
 **   Description:	Updates the resources database (which is a Tcl hash **
 **			table) with the resources passed in the buffer. The **
 **			buffer contains a X resource lookalike text image.  **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **			Tcl_HashTable	*data	The hash tables holding the  **
 **						resource data		     **
 **			register char	*buf	The buffer containing the    **
 **						resources to be modified in **
 **						X resource syntax	     **
 **			int		 remove	Remove or add resources     **
 ** 									     **
 **   Result:		ErrType	NO_ERR		Success			     **
 **				ERR_PARSE	Parse error		     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	ErrType getEntries(	Tcl_Interp	*interp,
				Tcl_HashTable	*data,
				register char	*buf,
				int		 remove)
{
    Tcl_RegExp		res_exp = (Tcl_RegExp) NULL;
    register Tcl_HashEntry	*entry;
    char			*end;
    int				 new_res;

    /**
     **  The following regular expression matches pattern like
     **
     **       <resource>:	<value>
     **
     **  The resource will be returned as \1 and the value as \2
     **  Set the regexp pointer only, if it hasn't already been set. This 
     **  is a constant regexp!
     **/

    if( !res_exp)
	res_exp  = Tcl_RegExpCompile(interp,
		 "^[ \t]*([^ \t]*)[ \t]*:[ \t]*(.*)[ \t]*$");

    /**
     **  Seek for the lines (buffers) end. Put a terminator there. Take care of
     **  escaped newlines!
     **/

    for( end = buf; *end; end++)
	if( *end == '\\' && *(end+1) == '\n')
	    end++;
	else if( *end == '\n')
	    *end = '\0';

    /**
     **  Now read the whole buffer.
     **  At first, we have to ignore comments
     **/

    while( buf <= end) {

	if( *buf == '#' || *buf == '!' || !*buf) {
	    while( *buf++) ;

	/**
	 **  Otherwise we're seeking for a syntacticl correct X resource entry
	 **/

	} else if( !Tcl_RegExpExec(interp, res_exp, buf, buf)) {
	    if( OK != ErrorLogger( ERR_PARSE, LOC, NULL)) {
		return( ERR_PARSE);	/** ------ EXIT (PARSE ERROR) -----> **/
	    }

	} else {

	    /**
	     **  Valid entry found. Set up buf pointing behind the pattern
	     **  that has matched and put a terminator at the end of either the
	     **  resource name and its value.
	     **/

	    char *startp, *endp;
	    Tcl_RegExpRange(res_exp, 0,
		(CONST84 char **) &startp, (CONST84 char **) &endp);
	    buf = endp + 1;
	    Tcl_RegExpRange(res_exp, 1,
		(CONST84 char **) &startp, (CONST84 char **) &endp);
	    *endp = '\0';
	    Tcl_RegExpRange(res_exp, 2,
		(CONST84 char **) &startp, (CONST84 char **) &endp);
	    *endp = '\0';

	    /**
	     **  Now create (or remove) a hash entry for the parsed resource
	     **/

	    if( remove) {
	        Tcl_RegExpRange(res_exp, 1,
			(CONST84 char **) &startp, (CONST84 char **) &endp);
		if( (entry = Tcl_FindHashEntry( data, startp)) ) {
		    null_free((void *) &( Tcl_GetHashValue( entry)));
		    Tcl_DeleteHashEntry( entry);
		}
	    } else {
	        Tcl_RegExpRange(res_exp, 1,
			(CONST84 char **) &startp, (CONST84 char **) &endp);
		entry = Tcl_CreateHashEntry( data, startp, &new_res);
		if( !new_res)
		    null_free((void *) &( Tcl_GetHashValue( entry)));
	        Tcl_RegExpRange(res_exp, 2,
			(CONST84 char **) &startp, (CONST84 char **) &endp);
		Tcl_SetHashValue( entry, stringer(NULL,0, startp, NULL));
	    }

	} /** if( reg exp matched) **/
    } /** while **/

    /**
     **  Return on success
     **/

    return( NO_ERR);

} /** end of 'getEntries' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		storeResProp					     **
 ** 									     **
 **   Description:	Update the X11 resource property, adding new resour- **
 **			ces.						     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	register ResourceDB *rdb	Resource database   **
 ** 									     **
 **   Result:		-						     **
 ** 									     **
 **   Attached Globals:	-						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

#ifdef HAS_X11LIBS
static	void	storeResProp(	register ResourceDB *rdb)
{
    Tcl_HashSearch	search;
    register int	mode = PropModeReplace;
    register int	max = (XMaxRequestSize( dpy) << 2) - 28;
    register int	left;
    register Tcl_HashEntry *entry = Tcl_FirstHashEntry( rdb->data, &search);
    unsigned char	*buf;

    /**
     **	 Write all attached resources into the buffer. Follow the X 
     **  resource syntax:
     **
     **        <resource>:	<value>
     **/

    Tcl_DStringTrunc( buffer, 0);
    while( entry) {
	Tcl_DStringAppend( buffer, Tcl_GetHashKey(rdb->data, entry), -1);
	Tcl_DStringAppend( buffer, ":\t", 2);
	Tcl_DStringAppend( buffer, (char *)Tcl_GetHashValue( entry), -1);
	Tcl_DStringAppend( buffer, "\n", 1);
	entry = Tcl_NextHashEntry( &search);
    }

    /**
     **  In case of the request being larger than the largest request the X
     **  server may handle, spool it block by block until the final one.
     **/

    buf = (unsigned char *) Tcl_DStringValue( buffer);
    if( max < (left = Tcl_DStringLength( buffer))) {
	XGrabServer( dpy);
	mode = PropModeAppend;
	do {
	    XChangeProperty( dpy, rdb->root, rdb->prop, XA_STRING, 8, mode, buf,
		max);
	    buf += max;
	} while( max < (left -= max));
    }

    /**
     **  Put the final request block (which may be the only one, if the if-
     **  statement above doesn't match) to the X server
     **/

    XChangeProperty( dpy, rdb->root, rdb->prop, XA_STRING, 8, mode, buf, left);

    if( mode != PropModeReplace)
	XUngrabServer( dpy);

} /** End of 'storeResProp' **/
#endif

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		getOld						     **
 ** 									     **
 **   Description:	First, we have to find the resources already loaded  **
 **			into the X11 resource property. This routine current-**
 **			ly only handles one screen, the default screen for   **
 **			the DISPLAY. This routine should only be called if   **
 **			resDB.data is NULL.				     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	register char	**buf	Buffer for the old resource **
 **						database		     **
 ** 									     **
 **   Result:		ErrType	ERR_PARAM	resDB.data != NULL	     **
 **				ERR_ALLOC	out of memory		     **
 **				NO_ERR		Success			     **
 ** 									     **
 **   Attached Globals:	resDB		The data area will be installed as a **
 **					Tcl hash table			     **
 **			dpy		The current display		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	ErrType getOld( register char **buf)
{

    /**
     **  Allocate memory for the hash table
     **/

    if( resDB.data)
	if( OK != ErrorLogger( ERR_PARAM, LOC, "Resource database", NULL))
	    return( ERR_PARAM);		/** ------- EXIT (PARAMETER) -----> **/

    if( !(resDB.data = (Tcl_HashTable *) module_malloc(sizeof(Tcl_HashTable))))
	if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
	    return( ERR_ALLOC);		/** ----- EXIT (OUT OF MEMORY) ----> **/

    /**
     **  Initialize the hash table and read in the old resources
     **/

    Tcl_InitHashTable( resDB.data, TCL_STRING_KEYS);
    resDB.root = RootWindow( dpy, 0);
    resDB.prop = XA_RESOURCE_MANAGER;
    *buf = XResourceManagerString( dpy);

    /**
     **  Success
     **/

    return( NO_ERR);

} /** End of 'getOld' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		initBuffers					     **
 ** 									     **
 **   Description:	Initilize buffers if not already done, or reinitia-  **
 **			lize some variables if buffers already exists.	     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	Tcl_Interp	*interp		According Tcl interp.**
 **   			register int is_file	Differs between a single X   **
 **						resource to be modified or  **
 **						a resource file to be merged**
 ** 									     **
 **   Result:		ErrType	ERR_DISPLAY	Cannot open DISPLAY	     **
 **				ERR_ALLOC	ALLOC failure		     **
 **				ERR_EXTRACT				     **
 **				NO_ERR		Success			     **
 ** 									     **
 **   Attached Globals:	dpy		Display will be openend		     **
 **			resDB		Resource database will be filled up **
 **					with the current setup		     **
 **			defines						     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

static	ErrType	initBuffers(	Tcl_Interp *interp,
				register int is_file)
{
    char *tmpbuf;

    /**
     **  Open the display
     **/

    if( !dpy && !(dpy = XOpenDisplay( NULL)))
	if( OK != ErrorLogger( ERR_DISPLAY, LOC, NULL))
	    return( ERR_DISPLAY);	/** -------- EXIT (FAILURE) -------> **/

    /**
     **  Read in the old setup at first and put it into the resource database
     **/

    if( !resDB.data) {

	if( getOld( &tmpbuf) != NO_ERR)	/** NULL if no resources exists      **/
	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		return( ERR_ALLOC);	/** ----- EXIT (OUT OF MEMORY) ----> **/

	if( tmpbuf && (getEntries(interp, resDB.data, tmpbuf, 0) != NO_ERR))
	    if( OK != ErrorLogger( ERR_EXTRACT, LOC, NULL))
		return( ERR_EXTRACT);
    }

    /**
     **  Conditionally allocate a buffer and initialize this guy
     **/

    if( !buffer) {
	if( !(buffer = (Tcl_DString *) module_malloc(sizeof(Tcl_DString)))) {
	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		return( ERR_ALLOC);	/** ----- EXIT (OUT OF MEMORY) ----> **/
	} else 
	    Tcl_DStringInit( buffer);
    } else
	Tcl_DStringTrunc( buffer, 0);

    /**
     **  Set up all the defines according display and screen
     **/

    if( defines)
	defines[ def_base] = '\0';
    else if( is_file) {

	if( !(defines = (char *) module_malloc(BUFSIZ * sizeof( char))))
	    if( OK != ErrorLogger( ERR_ALLOC, LOC, NULL))
		return( ERR_ALLOC);	/** ----- EXIT (OUT OF MEMORY) ----> **/

	/* sprintf( defines, "%s %s ", CPPSTDIN, CPPMINUS); */
	strcpy( defines, CPPSTDIN);
	strcat( defines, " ");
	strcat( defines, CPPMINUS);
	strcat( defines, " ");
	doDisplayDefines();
	doScreenDefines( DefaultScreen( dpy));
	def_base = strlen( strcat( defines, " "));
    }

    /**
     **  Return on success
     **/

    return( NO_ERR);

} /** End of 'initBuffers' **/
#endif

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:		xresourceFinish					     **
 ** 									     **
 **   Description:	Update the resource property if everything is ok.    **
 **			This routine should be called when all properies have**
 **			been defines or updated. Remember that this routine  **
 **			always will be called, even if there was no 	     **
 **			"x-resource" command in the module!		     **
 ** 									     **
 **   First Edition:    1991/10/23                                             **
 **                                                                          **
 **   Parameters:                                                            **
 **                                                                          **
 **   Result:                                                                **
 **                                                                          **
 **   Attached Globals: -                                                    **
 **                                                                          **
 ** ************************************************************************ **
 ++++*/

void xresourceFinish(register int no_errors)
{

#ifdef HAS_X11LIBS

    /**
     **  If there is data stored in the resource database, spool it to the
     **  according X server
     **/

    if( resDB.data && no_errors)
	storeResProp( &resDB);

    /**
     **  Close the display and free what has been used
     **/

    if( dpy)
	XCloseDisplay( dpy);
    if( buffer)
	Tcl_DStringFree( buffer);

#endif

} /** End of 'xresourceFinish' **/

/*++++
 ** ** Function-Header ***************************************************** **
 ** 									     **
 **   Function:	 	cmdXResource					     **
 ** 									     **
 **   Description:	Callback function for 'x-resource'. The function    **
 **			sets up a hash table containing all resources to be **
 **			passed to the X server. This hash table will be	     **
 **			flushed whenever the function xresourceFinish is cal-**
 **			led.						     **
 ** 									     **
 **   First Edition:	1991/10/23					     **
 ** 									     **
 **   Parameters:	ClientData	 client_data			     **
 **			Tcl_Interp	*interp		According Tcl interp.**
 **			int		 objc		Number of arguments  **
 **			Tcl_Obj		*objv[]		Argument array	     **
 ** 									     **
 **   Result:		int	TCL_OK		Successful completion	     **
 **				TCL_ERROR	Any error		     **
 ** 									     **
 **   Attached Globals:	g_flags		These are set up accordingly before  **
 **					this function is called in order to  **
 **					control everything		     **
 ** 									     **
 ** ************************************************************************ **
 ++++*/

int cmdXResource(
	ClientData client_data,
	Tcl_Interp * interp,
	int objc,
	Tcl_Obj * CONST84 objv[]
) {
	register FILE  *inp;			/** input file or pipe	     **/
	int             is_file,		/** file or not		     **/
	                i,			/** loop counter	     **/
	                do_cpp = 1,		/** pipe file thru cpp	     **/
	    opt_ind = 1;			/** option index	     **/
    /**
     **  Whatis mode?
     **/
	if (g_flags & (M_WHATIS | M_HELP))
		return (TCL_OK);	/** ------- EXIT PROCEDURE -------> **/

	if (!getenv("DISPLAY")) {
		/* don't bother trying to set display variables, */
		/* if there is no display */
		return (TCL_OK);
	}
    /**
     **  Parameter check
     **/
	if (objc > 1 && !strcmp(Tcl_GetString(objv[1]), "-nocpp")) {
		do_cpp = 0;
		opt_ind++;
	}
	if (objc <= opt_ind) {
		if (OK !=
		    ErrorLogger(ERR_USAGE, LOC, Tcl_GetString(objv[0]),
				"[ -nocpp ] strings", NULL))
			return (TCL_ERROR); /** ------ EXIT (FAILURE) -----> **/
	}
    /**
     **  Ok, now let's treat all remaining arguments as X resources or
     **  X resource files. At first let's check if it is a file ...
     **/
	while (opt_ind < objc) {
		is_file =
		    (access(Tcl_GetString(objv[opt_ind]), R_OK & F_OK) == 0);
#ifdef HAS_X11LIBS
		if (g_flags & M_DISPLAY) {
			fprintf(stderr, "xrdb -merge\t ");
			for (i = 1; i < objc; i++)
				fprintf(stderr, "%s ", Tcl_GetString(objv[i]));
			fprintf(stderr, "\n");
		} else {
	    /**
	     **  Initialize read buffers
	     **/
			if (NO_ERR != initBuffers(interp, is_file))
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
	    /**
	     **  This puts all required resources into a text image buffer ...
	     **/
			if (!is_file) {
				Tcl_DStringAppend(buffer,
					  Tcl_GetString(objv[opt_ind]), -1);
			} else {
				if (!(inp = (do_cpp ? popen(strcat(defines,
					Tcl_GetString(objv[opt_ind])), "r")
					:fopen(Tcl_GetString(objv[opt_ind]),"r")
					)))
					if (OK != ErrorLogger((do_cpp
					? ERR_POPEN : ERR_OPEN), LOC,
					"argv[ opt_ind]", _(em_reading)))
						return (TCL_ERROR);
						/** ---- EXIT (FAILURE) ---> **/
				if (TCL_ERROR == readFile(inp, do_cpp))
					return (TCL_ERROR);
					/** -------- EXIT (FAILURE) -------> **/
			}
	    /**
	     **  ... and this transforms the text image buffer into a Tcl hash
	     **  table
	     **/
			if (NO_ERR != getEntries(interp, resDB.data,
						 Tcl_DStringValue(buffer),
						 g_flags & M_REMOVE)) {
				return (TCL_ERROR); /** -- EXIT (FAILURE) -> **/
			}
		}
#else
		if (g_flags & M_DISPLAY) {
			fprintf(stderr, "xrdb -merge\t ");
			for (i = 1; i < objc; i++)
				fprintf(stderr, "%s ", Tcl_GetString(objv[i]));
			fprintf(stderr, "*** ignored ***\n");
		}
#endif
		opt_ind++;
	} /** while **/

	return (TCL_OK);

} /** End of 'cmdXResource' **/
