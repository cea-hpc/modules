static const char RCSID[]="@(#)$Id: memdebug.c,v 1.1 2002/08/27 21:07:41 rkowen Exp $";
static const char AUTHOR[]="@(#)memdebug 1.0 02/10/95 R.K.Owen,PhD";

/* memory - provides a front-end for the memory allocation routines to
 *	help find memory leaks.  Supports only the ANSI-C routines:
 *		  calloc,   free,   malloc,   realloc
 *	call with the following names:
 *		m_calloc, m_free, m_malloc, m_realloc
 *	the output goes to stderr.
 */
/** ** Copyright *********************************************************** **
 ** 									     **
 ** Copyright 1997 by R.K.Owen,Ph.D.		                      	     **
 ** last known email: librko@kooz.sj.ca.us				     **
 **                   rk@owen.sj.ca.us					     **
 ** see LICENSE.LGPL, which must be provided, for details		     **
 ** 									     **
 ** ************************************************************************ **/

#include <stdlib.h>
#include <stdio.h>

FILE *m_output;

FILE *memdebug_output(FILE *output) {

	if (!m_output) m_output = stderr;

	if (output)
		m_output = output;

	return m_output;
}

void *m_calloc(size_t nelem, size_t size, char *file, int line) {
	void *ptr;
	char *null = "(null)";

	if (!m_output) m_output = stderr;

	ptr = calloc(nelem, size);
	if (ptr != NULL)
		(void) fprintf(m_output,"RKOMEM:         calloc : %s %d :"
			" %p %d %d\n",
			file, line, ptr, nelem, size);
	else
		(void) fprintf(m_output,"RKOMEM:         calloc : %s %d :"
			" %s %d %d\n",
			file, line, null, nelem, size);
	return ptr;
}

void m_free(void *ptr, char *file, int line) {
	char *null = "(null)";

	if (!m_output) m_output = stderr;

	if (ptr != NULL)
		(void) fprintf(m_output,"RKOMEM:         free   : %s %d : %p\n",
			file, line, ptr);
	else
		(void) fprintf(m_output,"RKOMEM:         free   : %s %d : %s\n",
			file, line, null);
	free(ptr);
}

void *m_malloc(size_t size, char *file, int line) {
	void *ptr;
	char *null = "(null)";

	if (!m_output) m_output = stderr;

	ptr = malloc(size);
	if (ptr != NULL)
		(void) fprintf(m_output,"RKOMEM:         malloc : %s %d :"
			" %p %d\n",
			file, line, ptr, size);
	else
		(void)fprintf(m_output,"RKOMEM:         malloc : %s %d :"
		" %s %d\n",
			file, line, null, size);
	return ptr;
}

void *m_realloc(void *rptr, size_t size, char *file, int line) {
	void *ptr;
	char *null = "(null)";

	if (!m_output) m_output = stderr;

	ptr = realloc(rptr, size);
	if (rptr != NULL)
		(void) fprintf(m_output,"RKOMEM: realloc_free   : %s %d : %p\n",
		file, line, rptr);

	if (size) {
		(void) fprintf(m_output,"RKOMEM: realloc_malloc : %s %d :",
			file, line);
		if (ptr != NULL)
			(void) fprintf(m_output," %p %d\n", ptr, size);
		else
			(void) fprintf(m_output," %s %d\n", null, size);
	}
	return ptr;
}
