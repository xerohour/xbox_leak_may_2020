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
// FILE:      library\common\krnlstr.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//

#ifndef KRNLSTR
#define KRNLSTR

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"

class KernelStringBuffer;

class __far KernelString
	{
	private:
		KernelStringBuffer	*	buffer;
	public:
		KernelString(void);
		KernelString(const TCHAR __far * str);
		KernelString(const TCHAR ch);
#if NT_KERNEL
		KernelString(const UNICODE_STRING & org);
#endif
		KernelString(const KernelString & str);
		KernelString(DWORD value, int digits = 0, int base = 10, TCHAR fill = __TEXT('0'));
		KernelString(int value, int digits = 0, int base = 10, TCHAR fill = __TEXT('0'));
		~KernelString();

		// returns the length of the string EXCLUDING the succeeding zero...
		int Length() const;

		long ToInt(int base = 10);
		DWORD ToUnsigned(int base = 10);


		BOOL Get(TCHAR __far * str, int len);
#if NT_KERNEL
		BOOL Get(UNICODE_STRING & us);
#endif

		KernelString & operator = (const TCHAR __far * str);
#if NT_KERNEL
		KernelString & operator = (const UNICODE_STRING & str);
#endif
		KernelString & operator = (const KernelString str);

		friend KernelString operator+ (const KernelString u, const KernelString v);
		KernelString & operator+= (const KernelString u);
		friend KernelString operator* (const KernelString u, const int num);
		KernelString & operator*= (const int num);

		int Compare(const KernelString str);

		friend BOOL operator==(const KernelString u, const KernelString v);
		friend BOOL operator!=(const KernelString u, const KernelString v);
		friend BOOL operator<(const KernelString u, const KernelString v);
		friend BOOL operator>(const KernelString u, const KernelString v);
		friend BOOL operator<=(const KernelString u, const KernelString v);
		friend BOOL operator>=(const KernelString u, const KernelString v);

		friend KernelString operator << (const KernelString u, int index);
		friend KernelString operator >> (const KernelString u, int index);
		KernelString & operator <<= (int index);
		KernelString & operator >>= (int index);

		KernelString Seg(int start, int num) const;	// Extract seqment of string
		KernelString Caps(void);
		KernelString Head(int num) const;				// Return the first num characters
		KernelString Tail(int num) const;				// Return the last num characters

		int First(KernelString str) const;				// Find first occurrence of str
		int Next(KernelString str, int pos) const;	// Find next occurrence of str
		int Last(KernelString str) const;				// Find last occurrence of str
		int Prev(KernelString str, int pos) const;	// Find previous occurrence of str

		int First(TCHAR c) const;							// Find first occurrence of c (-1 if not found)
		int Next(TCHAR c, int pos) const;				// Find next occurrence of c (-1 if not found)
		int Last(TCHAR c) const;							// Find last occurrence of c (-1 if not found)
		int Prev(TCHAR c, int pos) const;				// Find prevoius occurrence of c (-1 if not found)
		BOOL Contains(TCHAR c) const;						// Test if character occurs in string

		// deletes spaces (and tabs) at beginning or end of string...
		KernelString Trim();

		TCHAR& operator[] (const int index);
		const TCHAR& operator[] (const int index) const;

//
// Unsafe functions
//
#if NT_KERNEL
		operator UNICODE_STRING * (void);
		operator UNICODE_STRING & (void);
#else
		operator TCHAR * (void);
#endif
	};

#endif
