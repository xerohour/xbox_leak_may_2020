/* Suite++: The Plum Hall Validation Suite for C++
 * VERSION: 1
 * AND      The Plum Hall Validation Suite for C
 * VERSION: 3
 * Unpublished copyright (c) 1991, Plum Hall Inc (Editor)
 * DATE:    91/07/01
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

/*
 * VALUE - the value routines are used to defeat value propagation in
 * optimizing compilers.  We want to make sure that we are testing what we
 * think we are testing, not what the compiler transformed it to.
 * 1988: Some compilers "open-code" all small functions.  Now we have to hide
 * the constants still further.
 */
#include "defs.h"

static int Zero = 0;	

int ivalue FN_DEF1(i,
	int i)
	{
	return i + Zero;
	}

long lvalue FN_DEF1(i,
	long i)
	{
	return i + Zero;
	}

float fvalue FN_DEF1(i,
	double i)
	{
	return (float)i + Zero;
	}

double dvalue FN_DEF1(i,
	double i)
	{
	return i + Zero;
	}



