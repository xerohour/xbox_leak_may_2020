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
// FILE:      library\common\krnlint.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#ifndef KRNLINT_H
#define KRNLINT_H

#include "prelude.h"
#include "KrnlStr.h"


//
// REMARK:
// This class is passed through DLLs.
// So please:
//        - insert member variables only at the end of the class declaration
//			 - do not append virutal methods
//
// occures in krnlint.h


// this defines a new data type as a class, the KernelInt64 type,
// a 64-Bit-Integer data type for all integer arithmetics
class __far KernelInt64
	{
	private:
      // upper 32 bit
		unsigned long lower;
      // lower 32 bit
		signed long upper;
	public:

      // empty constructor sets value to ZERO
		KernelInt64(void) {lower = 0; upper = 0;}

      // DWORD constructor, generates a 64-bit value from an 32 DWORD
      KernelInt64(DWORD val) {lower = val; upper = 0;}

      // long constructor (32 bit with sign)
		// upper 32 bit part carries sign
      KernelInt64(long val) {lower = val; upper = val < 0 ? -1 : 0;}

      // int constructor, same effect as for long value
      // i.e. upper 32 bit carry also sign
		KernelInt64(int val) {lower = val; upper = val < 0 ? -1 : 0;}
      // same for positive values, upper 32 bit are ZERO
		KernelInt64(unsigned int val) {lower = val; upper =  0;}
      // copy constructor
      // dublicates value
      KernelInt64(const KernelInt64 & val) {lower = val.lower; upper = val.upper;}

      // assignment operator
      KernelInt64 & operator= (const KernelInt64 val) {lower = val.lower; upper = val.upper; return *this;}

      // constructor:  a 64 bit value out of 2 32 bit int parts
		KernelInt64(unsigned long lower, int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned int lower, int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(int lower, unsigned int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(int lower, int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(int lower, long upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned int lower, unsigned int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned long lower, unsigned int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned long lower, unsigned long upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned long lower, signed long upper) {this->lower = lower; this->upper = upper;}

		KernelInt64(KernelString str, int base = 10);
		KernelString ToString(int digits = 0, int base = 10, TCHAR fill = __TEXT('0'));

      // convert 64bit value to 32 bit long/DWORD/Int value with saturation
      // i.e in case of an overflow a predefined value is retruned

		long ToLong(void)
			{
			if      (upper == 0x00000000 && !(lower & 0x80000000)) return lower;
			else if (upper == 0xffffffff &&  (lower & 0x80000000)) return lower;
			else if (upper < 0) return 0x8000000;
			else return 0x7fffffff;
			}

		DWORD ToDWORD(void)
			{
			if (upper < 0) return 0;
			else if (upper > 0) return 0xffffffff;
			else return lower;
			}

		int ToInt(void)
			{
			if (*this < -32768) return -32768;
			else if (*this > 32767) return 32767;
			else return (int)lower;
			}

#if NT_KERNEL
		LONGLONG ToLongLong(void)
			{
			LARGE_INTEGER li;

			li.LowPart = lower;
			li.HighPart = upper;

			return li.QuadPart;
			}
#endif

      // return upper or lower part of 64 bit values
		DWORD Lower(void) {return lower;}
		long Upper(void) {return upper;}

      // The logical-negation (logical-NOT) operator produces the value 0 if its operand
      // is true (nonzero) and the value 1 if its operand is false (0). The result has int type.
      // The operand must be an integral, floating, or pointer value.
		inline int operator! (void) const {return !lower && !upper;}

      // minus operator
		inline KernelInt64 operator- (void) const;

      // The one's complement operator, sometimes called the "bitwise complement" or "bitwise NOT"
      // operator, produces the bitwise one's complement of its operand. The operand must be of
      // integral type. This operator performs usual arithmetic conversions; the result has the
      // type of the operand after conversion.
		inline KernelInt64 operator~ (void) const {return KernelInt64(~lower, ~upper);}

      // FRIEND
      // The friend keyword allows a function or class to gain access to the private
      // and protected members of a class. In some circumstances, it is more convenient to grant
      // member-level access to functions that are not members of a class or to all functions in
      // a separate class. With the friend keyword, programmers can designate either the specific
      // functions or the classes whose functions can access not only public members but also protected
      // and private members


      // integer arithmetic operators for different input types/vaiations
		inline friend KernelInt64 operator+ (const KernelInt64 u, const KernelInt64 v);
		inline friend KernelInt64 operator- (const KernelInt64 u, const KernelInt64 v);
		inline friend KernelInt64 operator+ (const long u, const KernelInt64 v);
		inline friend KernelInt64 operator- (const long u, const KernelInt64 v);
		inline friend KernelInt64 operator+ (const KernelInt64 u, const long v);
		inline friend KernelInt64 operator- (const KernelInt64 u, const long v);
		friend KernelInt64 operator* (const KernelInt64 u, const KernelInt64 v);
		friend KernelInt64 operator/ (const KernelInt64 u, const KernelInt64 v);
		inline friend KernelInt64 operator% (const KernelInt64 u, const KernelInt64 v);

		inline KernelInt64 & operator+= (const KernelInt64 u);
		inline KernelInt64 & operator-= (const KernelInt64 u);
		inline KernelInt64 & operator+= (const long u);
		inline KernelInt64 & operator-= (const long u);
		inline KernelInt64 & operator*= (const KernelInt64 u);
		inline KernelInt64 & operator/= (const KernelInt64 u);
		inline KernelInt64 & operator%= (const KernelInt64 u);

		inline KernelInt64 & operator++ (void);
		inline KernelInt64 & operator-- (void);

      // shift operators
		inline friend KernelInt64 operator << (const KernelInt64 u, const int shl);
		inline friend KernelInt64 operator >> (const KernelInt64 u, const int shl);

		inline KernelInt64 & operator <<= (const int shl);
		inline KernelInt64 & operator >>= (const int shl);

		inline int Compare(const KernelInt64 u) const;

		friend BOOL operator==(const KernelInt64 u, const KernelInt64 v) {return u.Compare(v) == 0;}
		friend BOOL operator!=(const KernelInt64 u, const KernelInt64 v) {return u.Compare(v) != 0;}
		friend BOOL operator<(const KernelInt64 u, const KernelInt64 v)  {return u.Compare(v) < 0;}
		friend BOOL operator>(const KernelInt64 u, const KernelInt64 v)  {return u.Compare(v) > 0;}
		friend BOOL operator<=(const KernelInt64 u, const KernelInt64 v) {return u.Compare(v) <= 0;}
		friend BOOL operator>=(const KernelInt64 u, const KernelInt64 v) {return u.Compare(v) >= 0;}

		friend KernelInt64 operator& (const KernelInt64 u, const KernelInt64 v) {return KernelInt64(u.lower & v.lower, u.upper & v.upper);}
		friend KernelInt64 operator| (const KernelInt64 u, const KernelInt64 v) {return KernelInt64(u.lower | v.lower, u.upper | v.upper);}

		KernelInt64 & operator&= (const KernelInt64 u) {lower &= u.lower; upper &= u.upper; return *this;}
		KernelInt64 & operator|= (const KernelInt64 u) {lower |= u.lower; upper |= u.upper; return *this;}
	};

inline KernelInt64 & KernelInt64::operator+= (const KernelInt64 u)
	{
	lower += u.lower;
   // check and propagare overflow from lower part up to upper part
	if (lower < u.lower)
      // overflow
		upper += u.upper+1;
	else
      // no overflow
		upper += u.upper;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator-= (const KernelInt64 u)
	{
	unsigned long sum = lower - u.lower;

   // check and propagare overflow from lower part up to upper part
	if (sum > lower)
      // overflow...
		upper -= u.upper+1;
	else
      // none...
		upper -= u.upper;
	lower = sum;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator+= (const long u)
	{
   // check sign of u
	if (u < 0)
      // already use new defined -= operator (see above) and negation operator
      // to invert and subtract u from this (type KernelInt64)
		*this -= -u;
	else
		{
		lower += u;
      // check overflow and eventually propagate it to upper part
		if (lower < (DWORD)u)
         // yes, overflow
			upper += 1;
		}
	return *this;
	}

inline KernelInt64 & KernelInt64::operator++ (void)
	{
	lower ++;
	if (!lower)
      // pass on overflow upwards...
		upper ++;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator-- (void)
	{
   // check overflow
	if (!lower)
      // .. and paas on
		upper --;
	lower --;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator-= (const long u)
	{
	if (u < 0)
		*this += -u;
	else
		{
      // check and handle overflow
		unsigned long sum = lower - u;
		if (sum > lower)
         // propagate overflow
			upper -= 1;
		lower = sum;
		}
	return *this;
	}


// the following additions and subtractions is rearranged from the add/sub of two KernelInto64 objects
// with each having an upper and lower part to a separate addition of the lower part
// then passing the result to another add/sub  of the two added/subtracted lower parts with the
// added/subtracted upper parts


// add two KernelInt64 objects
inline KernelInt64 operator+ (const KernelInt64 u, const KernelInt64 v)
	{
   // add into temporary var
	unsigned long sum = u.lower + v.lower;
   // check overflow from lower to upper
	if (sum < u.lower)
      // overflow, so add one to pass this on
		return KernelInt64(sum, u.upper + v.upper + 1);
	else
      // no overflow
		return KernelInt64(sum, u.upper + v.upper);
	}

inline KernelInt64 operator- (const KernelInt64 u, const KernelInt64 v)
	{
	unsigned long sum = u.lower - v.lower;
	if (sum > u.lower)
      // overflow
		return KernelInt64(sum, u.upper - v.upper - 1);
	else
      // no overflow
		return KernelInt64(sum, u.upper - v.upper);
	}

inline KernelInt64 operator+ (const KernelInt64 u, const long v)
	{
	if (v < 0)
		return u - -v;
	else
		{
		unsigned long sum = u.lower + v;
		if (sum < u.lower)
			return KernelInt64(sum, u.upper + 1);
		else
			return KernelInt64(sum, u.upper);
		}
	}

inline KernelInt64 operator- (const KernelInt64 u, const long v)
	{
	if (v < 0)
		return u + -v;
	else
		{
		unsigned long sum = u.lower - v;
		if (sum > u.lower)
			return KernelInt64(sum, u.upper - 1);
		else
			return KernelInt64(sum, u.upper);
		}
	}


inline KernelInt64 operator+ (const long u, const KernelInt64 v)
	{
	if (u < 0)
		return v - -u;
	else
		{
		unsigned long sum = u + v.lower;
		if (sum < v.lower)
			return KernelInt64(sum, v.upper + 1);
		else
			return KernelInt64(sum, v.upper);
		}
	}

inline KernelInt64 operator- (const long u, const KernelInt64 v)
	{
   // use negation operation and addition opeartor to define minus operator
	return u + -v;
	}

inline KernelInt64 KernelInt64::operator- (void) const
	{
	if (lower == 0)
		return KernelInt64(0, -upper);
	else
		return KernelInt64((DWORD)-(long)lower, ~upper);
	}

inline int KernelInt64::Compare(const KernelInt64 u) const
	{
	if (upper < u.upper) return -1;
	else if (upper > u.upper) return 1;
	else if (lower < u.lower) return -1;
	else if (lower > u.lower) return 1;
	else return 0;
	}


// define shift operators

// left shift of KernelInt64 object for shl bits
inline KernelInt64 operator<< (const KernelInt64 u, const int shl)
	{
   // use definition of <<= operator to do this

   // first make a copy of u to do the shift with
	KernelInt64 v = u;
	v <<= shl;
	return v;
	}

// right shift of KernelInt64 object for shr bits
inline KernelInt64 operator>> (const KernelInt64 u, const int shr)
	{
   // use definition of >>= operator to do this

   // do shift with copy
	KernelInt64 v = u;
	v >>= shr;
	return v;
	}

inline KernelInt64 & KernelInt64::operator*= (const KernelInt64 u)
	{
   // use "normal" mult to do *=
	*this = *this * u;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator/= (const KernelInt64 u)
	{
   // use normal division to do /=
	*this = *this / u;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator%= (const KernelInt64 u)
	{
   // use normal modulo op to do %=
	*this = *this % u;
	return *this;
	}

inline KernelInt64 operator % (const KernelInt64 u, const KernelInt64 v)
	{
   // normal modulo op is done with / and *
	return u - (u / v) * v;
	}

inline KernelInt64 & KernelInt64::operator<<= (const int shl)
	{
	int s = shl;

   // shift data shl times left by 1
	while (s > 0)
		{
      // upper part left 1 bit, lowest bit becomes always 0
		upper <<= 1;
      // check lower part before shift: if highest bit is set, carry this to upper part
      // i.e. set lowest bit of upper part
		if (lower & 0x80000000) upper |= 1;
		lower <<= 1;
		s--;
		}

	return *this;
	}

inline KernelInt64 & KernelInt64::operator>>= (const int shl)
	{
	int s = shl;

   // lower part right by 1, highest bit becomes always 0
	while (s > 0)
		{
		lower >>= 1;
      // check lowest bit of upper part, and take carry to highest bit of lower part
		if (upper & 0x00000001) lower |= 0x80000000;
		upper >>= 1;
		s--;
		}

	return *this;
	}

#endif
