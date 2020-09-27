////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//
// FILE:      library\common\krnlint.cpp
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#include "KrnlInt.h"



KernelInt64 operator* (const KernelInt64 u, const KernelInt64 v)
	{
	BOOL sign;
	DWORD ll0, ll1, lh, hl, dummy;
	KernelInt64 u1, v1;

	if (u < 0)
		{
		u1 = -u;
		sign = TRUE;
		}
	else
		{
		u1 = u;
		sign = FALSE;
		}

	if (v < 0)
		{
		v1 = -v;
		sign = !sign;
		}
	else
		{
		v1 = v;
		}

	MUL32x32(v1.lower, u1.lower, ll1, ll0);
	MUL32x32(v1.lower, u1.upper, dummy, lh);
	MUL32x32(v1.upper, u1.lower, dummy, hl);

	if (sign)
		return -KernelInt64(ll0, lh+hl+ll1);
	else
		return KernelInt64(ll0, lh+hl+ll1);
	}

KernelInt64 operator/ (const KernelInt64 u, const KernelInt64 v)
	{
	BOOL sign;
	KernelInt64 u1, v1, acc;
	DWORD a0, a1, a2;

	if (u < 0)
		{
		u1 = -u;
		sign = TRUE;
		}
	else
		{
		u1 = u;
		sign = FALSE;
		}

	if (v < 0)
		{
		v1 = -v;
		sign = !sign;
		}
	else
		{
		v1 = v;
		}

	if (v1 != 0)
		{
		if (v1.upper)
			{
			if (u1.upper <= v1.upper)
				{
				a0 = u1.upper / v1.upper;
				MUL32x32(a0, v1.lower, a1, a2);
				a2 += a0 * v1.upper;
				acc = KernelInt64(a1, a2);
				if (acc > u1) a0--;

				acc = KernelInt64(a0, 0L);
				}
			else
				acc = 0;
			}
		else
			{
			if ((unsigned long)u1.upper < v1.lower)
				{
				a0 = DIV64x32(u1.upper, u1.lower, v1.lower);
				acc = KernelInt64(a0, 0L);
				}
			else
				{
				a0 = u1.upper / v1.lower;
				a1 = DIV64x32(u1.upper % v1.lower, u1.lower, v1.lower);
				acc = KernelInt64(a1, a0);
				}
			}
		if (sign)
			return -acc;
		else
			return acc;
		}
	else
		{
		if (sign)
			return KernelInt64(0x00000000, 0x80000000);
		else
			return KernelInt64(0xffffffff, 0x7fffffff);
		}
	}

KernelInt64::KernelInt64(KernelString str, int base)
	{
	int i = 0;
	BOOL sign = FALSE;
	TCHAR c;

	*this = 0;

	if (str[0] == '-')
		{
		sign = TRUE;
		i++;
		}

	while (c = str[i++])
		{
		*this *= base;

		if (c >= __TEXT('a') && c <= __TEXT('f'))
			*this += c - __TEXT('a') + 10;
		else if (c >= __TEXT('A') && c <= __TEXT('F'))
			*this += c - __TEXT('A') + 10;
		else if (c >= __TEXT('0') && c <= __TEXT('9'))
			*this += c - __TEXT('0');
		}

	if (sign)
		*this = -*this;
	}

KernelString KernelInt64::ToString(int digits, int base, TCHAR fill)
	{
	KernelString s;
	KernelInt64 a;
	BOOL sign;

	if (*this < 0)
		{
		a = -*this;
		sign = TRUE;
		}
	else
		{
		a = *this;
		sign = FALSE;
		}

	if (a == 0)
		{
		s = __TEXT("0");
		}
	else
		{
		while (a > 0)
			{
			int val = (a % base).ToLong();

			if (val < 10)
				s = KernelString((TCHAR)(__TEXT('0') + val)) + s;
			else
				s = KernelString((TCHAR)(__TEXT('A') + val - 10)) + s;

			a = a / base;
			}
		}

	if (sign)
		s = KernelString(__TEXT('-')) + s;

	if (digits)
		{
		while (s.Length() < digits) s = KernelString(fill) + s;
		}

	return s;
	}

