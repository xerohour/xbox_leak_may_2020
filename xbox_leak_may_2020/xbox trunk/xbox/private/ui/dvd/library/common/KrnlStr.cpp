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
// FILE:      library\common\krnlstr.cpp
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#include "KrnlStr.h"

////////////////////////////////////////////////////////////////////
//
//  Kernel String Buffer Class
//
////////////////////////////////////////////////////////////////////

class KernelStringBuffer
	{
	public:
		int		useCnt;
		int		length;
		TCHAR	*	buffer;
#if NT_KERNEL
		UNICODE_STRING	us;
#endif

		KernelStringBuffer(const TCHAR __far * str);
		KernelStringBuffer(const TCHAR ch);
		KernelStringBuffer(KernelStringBuffer * u, KernelStringBuffer * v);
		KernelStringBuffer(KernelStringBuffer * u, int start, int num);
		KernelStringBuffer(KernelStringBuffer * u, int num);
		KernelStringBuffer(BOOL sign, DWORD value, int digits, int base, TCHAR fill);

		~KernelStringBuffer(void);

		int Compare(KernelStringBuffer * u);

		void Obtain(void) {useCnt++;}
		void Release(void) {if (!--useCnt) delete this;}
	};

#define WS	sizeof(TCHAR)

KernelStringBuffer::KernelStringBuffer(const TCHAR __far * str)
	{
	useCnt = 1;

	const TCHAR __far * p;
	TCHAR * q;

	length = 0;
	p = str;
	while (*p++) length++;

#if   LINUX
	buffer = new TCHAR[length+2];
#else
	buffer = new (PagedPool) TCHAR[length+2];
#endif

	p = str;
	q = buffer;

	while (*q++ = *p++) ;

#if NT_KERNEL
	RtlInitUnicodeString(&us, buffer);
#endif
	}

KernelStringBuffer::KernelStringBuffer(const TCHAR ch)
	{
	useCnt = 1;

   length = 1;

#if   LINUX
	buffer = new TCHAR[2];
#else
	buffer = new (PagedPool) TCHAR[2];
#endif

	buffer[0] = ch;
	buffer[1] = 0;

#if NT_KERNEL
	RtlInitUnicodeString(&us, buffer);
#endif
	}

KernelStringBuffer::~KernelStringBuffer(void)
	{
	if (buffer) delete[] buffer;
	}

KernelStringBuffer::KernelStringBuffer(KernelStringBuffer * u, KernelStringBuffer * v)
	{
	useCnt = 1;

	length = u->length + v->length;
	TCHAR * p, * q;

#if   LINUX
	buffer = new TCHAR[length + 1];
#else
	buffer = new (PagedPool) TCHAR[length + 1];
#endif

	p = buffer;
	q = u->buffer;
	while (*p++ = *q++);
	p--;
	q = v->buffer;
	while (*p++ = *q++);

#if NT_KERNEL
	RtlInitUnicodeString(&us, buffer);
#endif
	}

KernelStringBuffer::KernelStringBuffer(KernelStringBuffer * u, int start, int num)
	{
	int i;

	useCnt = 1;

	length = u->length - start;
	if (length > num) length = num;
	TCHAR * p, * q;

#if   LINUX
	buffer = new TCHAR[length + 1];
#else
	buffer = new (PagedPool) TCHAR[length + 1];
#endif

	p = buffer;
	q = u->buffer + start;

	for(i=0; i<length; i++) *p++ = *q++;

	*p = 0;

#if NT_KERNEL
	RtlInitUnicodeString(&us, buffer);
#endif
	}

KernelStringBuffer::KernelStringBuffer(KernelStringBuffer * u, int num)
	{
	int i;

	useCnt = 1;

	length = u->length * num;
	TCHAR * p, * q;

#if   LINUX
	buffer = new TCHAR[length + 1];
#else
	buffer = new (PagedPool) TCHAR[length + 1];
#endif

	p = buffer;
	for(i=0; i<num; i++)
		{
		q = u->buffer;
		while (*p++ = *q++);
		p--;
		}
	p++;

#if NT_KERNEL
	RtlInitUnicodeString(&us, buffer);
#endif
	}

KernelStringBuffer::KernelStringBuffer(BOOL sign, DWORD value, int digits, int base, TCHAR fill)
	{
	TCHAR lbuffer[12];
	int pos = digits;
	int i;

	useCnt = 1;

	if (!pos) pos = 10;

	do {
		i = (int)(value % base);

		if (i < 10)
			lbuffer[--pos] = __TEXT('0') + i;
		else
			lbuffer[--pos] = __TEXT('A') + i - 10;

		value /= base;
		} while (value && pos);

	if (digits)
		{
#if   LINUX
		buffer = new TCHAR[digits + 1];
#else
		buffer = new (PagedPool) TCHAR[digits + 1];
#endif

		for(i=0; i<pos; i++) buffer[i] = fill;
		if (sign) buffer[0] = __TEXT('-');
		for(i=pos; i<digits; i++) buffer[i] = lbuffer[i];
		buffer[digits] = 0;
		length = digits;
		}
	else
		{
		if (sign)
			{
#if   LINUX
			buffer = new TCHAR[10 - pos + 2];
#else
			buffer = new (PagedPool) TCHAR[10 - pos + 2];
#endif
			buffer[0] = __TEXT('-');
			i = 1;
			length = 10 - pos + 1;
			}
		else
			{
#if   LINUX
			buffer = new TCHAR[10 - pos + 1];
#else
			buffer = new (PagedPool) TCHAR[10 - pos + 1];
#endif
			i = 0;
			length = 10 - pos;
			}

		while (pos < 10)
			{
			buffer[i++] = lbuffer[pos++];
			}
		buffer[i] = 0;
		}
	}


int KernelStringBuffer::Compare(KernelStringBuffer * u)
	{
#if NT_KERNEL
	return RtlCompareUnicodeString(&us, &(u->us), FALSE);
#else
	TCHAR * p, * q;
	TCHAR cp, cq;

	p = buffer;
	q = u->buffer;

	do {
		cp = *p++;
		cq = *q++;
		if (cp < cq)
			return -1;
		else if (cp > cq)
			return 1;
		} while (cp);

	return 0;
#endif
	}

////////////////////////////////////////////////////////////////////
//
//  Kernel String Class
//
////////////////////////////////////////////////////////////////////

KernelString::KernelString(void)
	{
	buffer = NULL;
	}

KernelString::KernelString(const TCHAR __far * str)
	{
	buffer = new (PagedPool) KernelStringBuffer(str);
	}

KernelString::KernelString(const TCHAR ch)
	{
	buffer = new (PagedPool) KernelStringBuffer(ch);
	}

#if NT_KERNEL

KernelString::KernelString(const UNICODE_STRING & str)
	{
	buffer = new (PagedPool) KernelStringBuffer(str.Buffer);
	}

#endif

KernelString::KernelString(int value, int digits, int base, TCHAR fill)
	{
	if (value < 0)
		buffer = new (PagedPool) KernelStringBuffer(TRUE, (DWORD)(-value), digits, base, fill);
	else
		buffer = new (PagedPool) KernelStringBuffer(FALSE, (DWORD)(value), digits, base, fill);
	}



KernelString::KernelString(DWORD value, int digits, int base, TCHAR fill)
	{
	buffer = new (PagedPool) KernelStringBuffer(FALSE, value, digits, base, fill);
	}

KernelString::KernelString(const KernelString & str)
	{
	buffer = str.buffer;
	if (buffer) buffer->Obtain();
	}

KernelString::~KernelString(void)
	{
	if (buffer) buffer->Release();
	}


int KernelString::Length(void) const
	{
	if (buffer)
		return buffer->length;
	else
		return 0;
	}


long KernelString::ToInt(int base)
	{
	long val = 0;
	BOOL sign;
	TCHAR c, * p;

	if (buffer && buffer->length)
		{
		p = buffer->buffer;
		if (*p == __TEXT('-'))
			{
			sign = TRUE;
			p++;
			}
		else
			sign = FALSE;

		while (c = *p++)
			{
			if      (c >= __TEXT('0') && c <= __TEXT('9')) val = val * base + c - __TEXT('0');
			else if (c >= __TEXT('a') && c <= __TEXT('f')) val = val * base + c - __TEXT('a') + 10;
			else if (c >= __TEXT('A') && c <= __TEXT('F')) val = val * base + c - __TEXT('A') + 10;
			else return 0;
			}

		if (sign)
			return -val;
		else
			return val;
		}
	else
		return 0;
	}

DWORD KernelString::ToUnsigned(int base)
	{
	DWORD val = 0;
	TCHAR c, * p;

	if (buffer && buffer->length)
		{
		p = buffer->buffer;

		while (c = *p++)
			{
			if (c>=__TEXT('0') && c<= __TEXT('9')) val = val * base + c - __TEXT('0');
			else if (c>=__TEXT('a') && c<=__TEXT('f')) val = val * base + c - __TEXT('a') + 10;
			else if (c>=__TEXT('A') && c<=__TEXT('F')) val = val * base + c - __TEXT('A') + 10;
			else return 0;
			}

		return val;
		}
	else
		return 0;
	}

BOOL KernelString::Get(TCHAR __far * str, int len)
	{
	TCHAR __far * p, * q;
	if (buffer)
		{
		if (len > buffer->length)
			{
			p = str;
			q = buffer->buffer;

			while (*p++ = * q++);

			return TRUE;
			}
		else
			return FALSE;
		}
	else if (len > 0)
		{
		str[0] = 0;

		return TRUE;
		}
	else
		return FALSE;
	}

#if NT_KERNEL

BOOL KernelString::Get(UNICODE_STRING & us)
	{
	RtlCopyUnicodeString(&us, &(buffer->us));

	return TRUE;
	}

#endif

KernelString & KernelString::operator= (const TCHAR __far * str)
	{
	if (buffer) buffer->Release();
	buffer = new (PagedPool) KernelStringBuffer(str);
	return * this;
	}

#if NT_KERNEL

KernelString & KernelString::operator= (const UNICODE_STRING & str)
	{
	if (buffer) buffer->Release();
	buffer = new (PagedPool) KernelStringBuffer(str.Buffer);
	return * this;
	}

#endif

KernelString & KernelString::operator= (const KernelString str)
	{
	if (buffer) buffer->Release();
	buffer = str.buffer;
	if (buffer) buffer->Obtain();

	return * this;
	}

KernelString operator+ (const KernelString u, const KernelString v)
	{
	KernelString str = u;
	str += v;
	return str;
	}

KernelString & KernelString::operator += (const KernelString u)
	{
	KernelStringBuffer * bp;

	if (buffer)
		{
		if (u.buffer)
			{
			bp = new (PagedPool) KernelStringBuffer(buffer, u.buffer);
			buffer->Release();
			buffer = bp;
			}
		}
	else
		{
		buffer = u.buffer;
		if (buffer) buffer->Obtain();
		}
	return *this;
	}

KernelString operator* (const KernelString u, const int num)
	{
	KernelString str = u;
	str *= num;
	return str;
	}

KernelString & KernelString::operator *= (const int num)
	{
	KernelStringBuffer * bp;

	if (buffer)
		{
		if (num)
			{
			bp = new (PagedPool) KernelStringBuffer(buffer, num);
			buffer->Release();
			buffer = bp;
			}
		else
			{
			buffer->Release();
			buffer = NULL;
			}
		}

	return *this;
	}

int KernelString::Compare(const KernelString str)
	{
	if (buffer == str.buffer)
		return 0;
	else if (!buffer)
		return -1;
	else if (!str.buffer)
		return 1;
	else
		return buffer->Compare(str.buffer);
	}

BOOL operator==(const KernelString u, const KernelString v)
	{
	return ((KernelString)u).Compare(v) == 0;
	}

BOOL operator!=(const KernelString u, const KernelString v)
	{
	return ((KernelString)u).Compare(v) != 0;
	}

BOOL operator<=(const KernelString u, const KernelString v)
	{
	return ((KernelString)u).Compare(v) <= 0;
	}

BOOL operator>=(const KernelString u, const KernelString v)
	{
	return ((KernelString)u).Compare(v) >= 0;
	}

BOOL operator<(const KernelString u, const KernelString v)
	{
	return ((KernelString)u).Compare(v) < 0;
	}

BOOL operator>(const KernelString u, const KernelString v)
	{
	return ((KernelString)u).Compare(v) > 0;
	}

TCHAR & KernelString::operator[] (const int index)
	{
	static TCHAR dummy;

	if (buffer)
		return buffer->buffer[index];
	else
		return dummy;
	}

const TCHAR & KernelString::operator[] (const int index) const
	{
	static TCHAR dummy;

	if (buffer)
		return buffer->buffer[index];
	else
		return dummy;
	}

KernelString operator >> (const KernelString u, int num)
	{
	KernelString str = u;
	str >>= num;

	return str;
	}

KernelString operator << (const KernelString u, int num)
	{
	KernelString str = u;
	str <<= num;

	return str;
	}

KernelString & KernelString::operator <<= (int index)
	{
	KernelStringBuffer * pb;

	if (buffer)
		{
		if (index < buffer->length)
			pb = new (PagedPool) KernelStringBuffer(buffer, index, buffer->length-index);
		else
			pb = NULL;

		buffer->Release();
		buffer = pb;
		}

	return * this;
	}

KernelString & KernelString::operator >>= (int index)
	{
	KernelStringBuffer * pb;

	if (buffer)
		{
		if (index < buffer->length)
			pb = new (PagedPool) KernelStringBuffer(buffer, 0, buffer->length-index);
		else
			pb = NULL;

		buffer->Release();
		buffer = pb;
		}

	return * this;
	}

KernelString KernelString::Seg(int start, int num) const
	{
	KernelString str;

	if (buffer)
		{
		if (start+num > buffer->length) num = buffer->length-start;
		if (num > 0)
			str.buffer = new (PagedPool) KernelStringBuffer(buffer, start, num);
		}

	return str;
	}

//
//  Return first num characters
//

KernelString KernelString::Head(int num) const
	{
	KernelString str;

	if (buffer && num > 0)
		{
		if (num > buffer->length)
			num = buffer->length;

		str.buffer = new (PagedPool) KernelStringBuffer(buffer, 0, num);
		}

	return str;
	}

//
//  Return last num characters
//

KernelString KernelString::Tail(int num) const
	{
	KernelString str;

	if (buffer && num > 0)
		{
		if (num <= buffer->length)
			str.buffer = new (PagedPool) KernelStringBuffer(buffer, buffer->length - num, num);
		else
			str.buffer = new (PagedPool) KernelStringBuffer(buffer, 0, num);
		}

	return str;
	}

//
//  Delete whitespaces at beginning or end of string
//

KernelString KernelString::Trim()
	{
	int i = 0;
	KernelString str = *this;

	// delete leading tabs and spaces...
	i = 0;
	while ((str[i] == ' ') || (str[i] == '\t'))
		i++;
	str <<= i;


	// delete preceding tabs and spaces ...
	i = str.Length()-1;
	while ((str[i] == ' ') || (str[i] == '\t'))
		i--;
	str = str.Seg(0, i+1);

	return str;
	}

//
//  Return upper case version of string
//

KernelString KernelString::Caps(void)
	{
	KernelString str;
	int i;
	TCHAR c;

	str = *this;

	for(i=0; i<str.Length(); i++)
		{
		c = str[i];
		if (c >= __TEXT('a') && c <= __TEXT('z'))
			str[i] = c + __TEXT('A') - __TEXT('a');
		}

	return str;
	}

//
//  Find first occurrence of str
//

int KernelString::First(KernelString str) const
	{
	int i;
	i = 0;

	while (i<= Length()-str.Length() && Seg(i, str.Length()) != str)
		i++;

	return i;
	}

//
//  Find next occurrence of str
//

int KernelString::Next(KernelString str, int pos) const
	{
	int i;
	i = pos+1;

	while (i<= Length()-str.Length() && Seg(i, str.Length()) != str)
		i++;

	return i;
	}

//
//  Find last occurrence of str
//

int KernelString::Last(KernelString str) const
	{
	int i;
	i = Length()-str.Length();

	while (i>=0 && Seg(i, str.Length()) != str)
		i--;

	return i;
	}

//
//  Find previous occurrence of str
//

int KernelString::Prev(KernelString str, int pos) const
	{
	int i;
	i = pos-1;

	while (i>=0 && Seg(i, str.Length()) != str)
		i--;

	return i;
	}

//
//  Find first occurrence of c (-1 if not found)
//

int KernelString::First(TCHAR c) const
	{
	int i = 0;

	if (buffer)
		{
		while (i < Length())
			{
			if (buffer->buffer[i] == c)
				return i;

			i++;
			}
		}

	return -1;
	}

//
//  Find next occurrence of c (length if not found)
//

int KernelString::Next(TCHAR c, int pos) const
	{
	int i = 0;

	if (buffer)
		{
		i = pos + 1;
		while (i < Length())
			{
			if (buffer->buffer[i] == c)
				return i;

			i++;
			}
		}

	return i;
	}

//
//  Find last occurrence of c (length if not found)
//

int KernelString::Last(TCHAR c) const
	{
	int i = 0;

	if (buffer)
		{
		i = Length() - 1;
		while (i >= 0)
			{
			if (buffer->buffer[i] == c)
				return i;

			i--;
			}
		}

	return i;
	}

//
//  Find previous occurrence of c (-1 if not found)
//

int KernelString::Prev(TCHAR c, int pos) const
	{
	int i = 0;

	if (buffer)
		{
		i = pos - 1;
		while (i >= 0)
			{
			if (buffer->buffer[i] == c)
				return i;

			i--;
			}
		}

	return -1;
	}

//
//  Test if string contains c
//

BOOL KernelString::Contains(TCHAR c) const
	{
	int i;

	if (buffer)
		{
		for (i=0; i<Length(); i++)
			{
			if (buffer->buffer[i] == c)
				return TRUE;
			}
		}

	return FALSE;
	}

#if NT_KERNEL

KernelString::operator UNICODE_STRING * (void)
	{
	return &(buffer->us);
	}

KernelString::operator UNICODE_STRING & (void)
	{
	return buffer->us;
	}

#else

KernelString::operator TCHAR * (void)
	{
	if (buffer)
		return buffer->buffer;
	else
		return NULL;
	}

#endif
