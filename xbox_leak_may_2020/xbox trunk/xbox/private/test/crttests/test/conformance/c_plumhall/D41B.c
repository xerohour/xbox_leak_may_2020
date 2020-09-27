/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1990, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 2.00
 * DATE: 90/04/05
 * The "ANSI" mode of this suite corresponds to official ANSI C, X3.159-1989.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#define LIB_TEST 1
#include "defs.h"
#if !ANSI || !HAS_PROTOTYPES
#define SKIP41 1
#endif
#ifndef SKIP41
#include <limits.h>
/*
 * 4.1.4 - Limits <limits.h>
 */
void d41_4a()
	{
	Filename = "d41b.c";

	/* All of these must be restricted constant expresions. */
if (CHAR_BIT < 8)
	complain(__LINE__);
if (SCHAR_MIN > -127)
	complain(__LINE__);
if (SCHAR_MAX < 127)
	complain(__LINE__);
if (UCHAR_MAX < 255U)
	complain(__LINE__);
if (CHAR_MIN != SCHAR_MIN && CHAR_MIN != 0)
	complain(__LINE__);
if (CHAR_MAX != SCHAR_MAX && CHAR_MAX != UCHAR_MAX)
	complain(__LINE__);
if (SHRT_MIN > -32767)
	complain(__LINE__);
if (SHRT_MAX < 32767)
	complain(__LINE__);
if (USHRT_MAX < 65535U)
	complain(__LINE__);
if (INT_MIN > -32767)
	complain(__LINE__);
if (INT_MAX < 32767)
	complain(__LINE__);
if (UINT_MAX < 65535U)
	complain(__LINE__);

if (LONG_MIN > -2147483647)
	complain(__LINE__);
if (LONG_MAX <  2147483647)
	complain(__LINE__);
if (ULONG_MAX < 0xffffffff)
	complain(__LINE__);


#if !defined(CHAR_BIT) ||  !defined(SCHAR_MIN) ||  !defined(SCHAR_MAX) ||  !defined(UCHAR_MAX) ||  !defined(CHAR_MIN)
	complain(__LINE__);
#endif
#if !defined(CHAR_MAX) ||  !defined(SHRT_MIN) ||  !defined(SHRT_MAX) ||  !defined(USHRT_MAX) ||  !defined(INT_MIN)
	complain(__LINE__);
#endif
#if !defined(INT_MAX) ||  !defined(UINT_MAX) ||  !defined(LONG_MIN) ||  !defined(LONG_MAX) ||  !defined(ULONG_MAX)
	complain(__LINE__);
#endif

/* ANSI8809 - Each limit must be properly represented to equal its value stored in appropriate object */
	#if ANSI8809
	{
	char cmin = CHAR_MIN;
	char c = CHAR_MAX;
	signed char scmin = SCHAR_MIN;
	signed char sc = SCHAR_MAX;
	unsigned char uc = UCHAR_MAX;
	short smin = SHRT_MIN;
	short s = SHRT_MAX;
	unsigned short us = USHRT_MAX;
	int imin = INT_MIN;
	int i = INT_MAX;
	unsigned int ui = UINT_MAX;
	long Lmin = LONG_MIN;
	long L = LONG_MAX;
	unsigned long uL = ULONG_MAX;
	


	if ((char)CHAR_MIN != cmin)
		complain(__LINE__);
	if (CHAR_MAX != c)
		complain(__LINE__);
	if (SCHAR_MIN != scmin)
		complain(__LINE__);
	if (SCHAR_MAX != sc)
		complain(__LINE__);
	if (UCHAR_MAX != uc)
		complain(__LINE__);
	if (SHRT_MIN != smin)
		complain(__LINE__);
	if (SHRT_MAX != s)
		complain(__LINE__);
	if (USHRT_MAX != us)
		complain(__LINE__);
	if (INT_MIN != imin)
		complain(__LINE__);
	if (INT_MAX != i)
		complain(__LINE__);

	if (UINT_MAX != ui)
		complain(__LINE__);
	if (LONG_MIN != Lmin)
		complain(__LINE__);
	if (LONG_MAX != L)
		complain(__LINE__);
	if (ULONG_MAX != uL)
		complain(__LINE__);
	}
	#endif	/* ANSI8809 */
	}
#else	/* SKIP41 */
int skp41b = 1;	/* must have one def */
#endif	/* SKIP41 */
