/* Suite++: The Plum Hall Validation Suite for C++
 * LVS++:   The Plum Hall Validation Suite for the C++ Library
 * AND      The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1994, Plum Hall Inc
 * DATE: 1994-07-20
 * As per your license agreement, your distribution is not
 * to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#ifndef SDUTIL_H
#define SDUTIL_H

#ifndef FREESTANDING
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#endif /* FREESTANDING */

#include "defs.h"

extern char last_char_printed;

#ifndef FREESTANDING

/* If your environment is FREESTANDING, you need to ...
 *  For the C suite, modify  pr_ok  and  setzero ; you don't need
 *      the  ph_alloc  and  ph_free  functions.
 *  For the C++ suite, modify all of the remaining functions.
 */

/*
 * PH_ALLOC - allocate dynamic memory
 */
void *ph_alloc FN_DEF1(n,
	int n)
	{
	return malloc(n != 0 ? n : 1);
	}

/*
 * PH_FREE - free dynamic memory
 */
void ph_free FN_DEF1(p,
	void *p)
	{
	free((char *)p);
	}

/*
 * PR_OK - printf a text line in ok situation (no error, just text)
 */
void pr_ok FN_DEF1(s,
	const char *s)
	{
	char c;
#ifndef _XBOX
	fputs(s, stdout);
#else
	DbgPrint(s);
#endif
	while (*s != '\0')
		c = *s++;
	last_char_printed = c;
	fflush(stdout);
	}

/*
 * SETZERO - appear, to a compiler, as though Zero is unpredictably set
 */
extern int Zero;

void setzero()
	{
	FILE *fp;
	int save;

	save = errno;
	if ((fp = fopen("nonexistent", "r")) != 0)
		{
		fscanf(fp, "ignore format", &Zero);
		fclose(fp);
		}
	errno = save;
	}

#endif /* FREESTANDING */

#endif /* SDUTIL_H */
