/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"

#if IS_EMBEDDED
#define ONLY
#define CASE_181T21		Table 25: <cstddef> synopsis
#endif /* IS_EMBEDDED */

/* _181T21 Table 25: <cstddef> synopsis */
#if !defined(SKIP_181T21)&&(!defined(ONLY)||defined(CASE_181T21))
#include <cstddef>
#endif /* CASE_181T21 */

/* _1821T22 numeric_limits::is_specialized true for all specializations */
#if !defined(SKIP_1821T22)&&(!defined(ONLY)||defined(CASE_1821T22))
#include <limits>
#endif /* CASE_1821T22 */

/* _1821T31 non-scalar types shall not have specializations */
#if !defined(SKIP_1821T31)&&(!defined(ONLY)||defined(CASE_1821T31))
#include <complex>
#include <limits>
#endif /* CASE_1821T31 */

/* _1821T41 <limits> summary */
#if !defined(SKIP_1821T41)&&(!defined(ONLY)||defined(CASE_1821T41))
#include <limits>
#endif /* CASE_1821T41 */

/* _18211T01 numeric_limits definition */
#if !defined(SKIP_18211T01)&&(!defined(ONLY)||defined(CASE_18211T01))
#include <limits>
#endif /* CASE_18211T01 */

/* _18211T11 numeric_limits::is_specialized distinguishes specializations */
#if !defined(SKIP_18211T11)&&(!defined(ONLY)||defined(CASE_18211T11))
#include <limits>
class wrapper_18211T11 {
public:
    wrapper_18211T11(int v)
        : val(v) {};
    bool operator==(int v)
        {return (val == v); }
private:
    int val; };
#endif /* CASE_18211T11 */

/* _18211T21 radix, epsilon, and round_error meaningful for floating */
#if !defined(SKIP_18211T21)&&(!defined(ONLY)||defined(CASE_18211T21))
#include <cfloat>
#include <limits>
#endif /* CASE_18211T21 */

/* _18211T41 numeric_limits template shall have all members, with 0 values */
#if !defined(SKIP_18211T41)&&(!defined(ONLY)||defined(CASE_18211T41))
#include <climits>
#include <limits>
class wrapper_18211T41 {
public:
    wrapper_18211T41(int v)
        : val(v) {};
    bool operator==(int v)
        {return (val == v); }
private:
    int val; };
#endif /* CASE_18211T41 */

/* _18212T11 numeric_limits::min() */
#if !defined(SKIP_18212T11)&&(!defined(ONLY)||defined(CASE_18212T11))
#include <climits>
#include <cwchar>
#include <limits>
#endif /* CASE_18212T11 */

/* _18212T21 numeric_limits::denorm_min() */
#if !defined(SKIP_18212T21)&&(!defined(ONLY)||defined(CASE_18212T21))
#include <cfloat>
#include <limits>
#endif /* CASE_18212T21 */

/* _18212T31 numeric_limits::denorm_min() meaningful for is_bounded */
#if !defined(SKIP_18212T31)&&(!defined(ONLY)||defined(CASE_18212T31))
#include <limits>
#endif /* CASE_18212T31 */

/* _18212T41 numeric_limits::max() */
#if !defined(SKIP_18212T41)&&(!defined(ONLY)||defined(CASE_18212T41))
#include <cfloat>
#include <climits>
#include <cwchar>
#include <limits>
#endif /* CASE_18212T41 */

/* _18212T51 numeric_limits::max() meaningful for is_bounded */
#if !defined(SKIP_18212T51)&&(!defined(ONLY)||defined(CASE_18212T51))
#include <limits>
#endif /* CASE_18212T51 */

/* _18212T71 numeric_limits::digits for integer */
#if !defined(SKIP_18212T71)&&(!defined(ONLY)||defined(CASE_18212T71))
#include <climits>
#include <cwchar>
#include <limits>
#endif /* CASE_18212T71 */

/* _18212T81 numeric_limits::digits for floating */
#if !defined(SKIP_18212T81)&&(!defined(ONLY)||defined(CASE_18212T81))
#include <cfloat>
#include <limits>
#endif /* CASE_18212T81 */

/* _18212T91 numeric_limits::digits10 */
#if !defined(SKIP_18212T91)&&(!defined(ONLY)||defined(CASE_18212T91))
#include <cfloat>
#include <limits>
#endif /* CASE_18212T91 */

/* _18212T_101 numeric_limits::digits10 meaningful for is_bounded */
#if !defined(SKIP_18212T_101)&&(!defined(ONLY)||defined(CASE_18212T_101))
#include <limits>
#endif /* CASE_18212T_101 */

/* _18212T_111 numeric_limits::is_signed */
#if !defined(SKIP_18212T_111)&&(!defined(ONLY)||defined(CASE_18212T_111))
#include <climits>
#include <cwchar>
#include <limits>
#endif /* CASE_18212T_111 */

/* _18212T_131 numeric_limits::is_integer */
#if !defined(SKIP_18212T_131)&&(!defined(ONLY)||defined(CASE_18212T_131))
#include <limits>
#endif /* CASE_18212T_131 */

/* _18212T_141 numeric_limits::is_integer meaningful for all specializations */
#if !defined(SKIP_18212T_141)&&(!defined(ONLY)||defined(CASE_18212T_141))
#include <limits>
#endif /* CASE_18212T_141 */

/* _18212T_151 numeric_limits::is_exact */
#if !defined(SKIP_18212T_151)&&(!defined(ONLY)||defined(CASE_18212T_151))
#include <limits>
#endif /* CASE_18212T_151 */

/* _18212T_161 numeric_limits::is_exact meaningful for all specializations */
#if !defined(SKIP_18212T_161)&&(!defined(ONLY)||defined(CASE_18212T_161))
#include <limits>
#endif /* CASE_18212T_161 */

/* _18212T_171 numeric_limits::radix for floating_point types */
#if !defined(SKIP_18212T_171)&&(!defined(ONLY)||defined(CASE_18212T_171))
#include <cfloat>
#include <limits>
#endif /* CASE_18212T_171 */

/* _18212T_181 numeric_limits::radix for integer */
#if !defined(SKIP_18212T_181)&&(!defined(ONLY)||defined(CASE_18212T_181))
#include <limits>
#endif /* CASE_18212T_181 */

/* _18212T_201 numeric_limits::epsilon() */
#if !defined(SKIP_18212T_201)&&(!defined(ONLY)||defined(CASE_18212T_201))
#include <cfloat>
#include <limits>
#endif /* CASE_18212T_201 */

/* _18212T_211 numeric_limits::epsilon() meaningful only for floating */
#if !defined(SKIP_18212T_211)&&(!defined(ONLY)||defined(CASE_18212T_211))
#include <limits>
#endif /* CASE_18212T_211 */

int t180_main(int, char *[])
{
enter_chk("t180.cpp");
/* _181T21 Table 25: <cstddef> synopsis */
#if !defined(SKIP_181T21)&&(!defined(ONLY)||defined(CASE_181T21))
	begin_chk("_181T21");
	{
	struct x {char ch1, ch2; };
	ptrdiff_t pdiff; TOUCH(pdiff);
	size_t st = offsetof(x, ch2); TOUCH(st);
	void *pv = NULL; TOUCH(pv);
	chk(1);


	}
	end_chk("_181T21");
#else
	skip_chk("_181T21");
#endif /* CASE_181T21 */

/* _1821T22 numeric_limits::is_specialized true for all specializations */
#if !defined(SKIP_1821T22)&&(!defined(ONLY)||defined(CASE_1821T22))
	begin_chk("_1821T22");
	{
	chk(STD numeric_limits<bool>::is_specialized);
	chk(STD numeric_limits<char>::is_specialized);
	chk(STD numeric_limits<signed char>::is_specialized);
	chk(STD numeric_limits<unsigned char>::is_specialized);
	chk(STD numeric_limits<wchar_t>::is_specialized);
	chk(STD numeric_limits<short>::is_specialized);
	chk(STD numeric_limits<unsigned short>::is_specialized);
	chk(STD numeric_limits<int>::is_specialized);
	chk(STD numeric_limits<unsigned int>::is_specialized);
	chk(STD numeric_limits<long>::is_specialized);
	chk(STD numeric_limits<unsigned long>::is_specialized);
	chk(STD numeric_limits<float>::is_specialized);
	chk(STD numeric_limits<double>::is_specialized);
	chk(STD numeric_limits<long double>::is_specialized);


	}
	end_chk("_1821T22");
#else
	skip_chk("_1821T22");
#endif /* CASE_1821T22 */

/* _1821T31 non-scalar types shall not have specializations */
#if !defined(SKIP_1821T31)&&(!defined(ONLY)||defined(CASE_1821T31))
	begin_chk("_1821T31");
	{
	chk(!STD numeric_limits<STD complex<float> >::is_specialized);
	chk(!STD numeric_limits<STD complex<double> >::is_specialized);
	chk(!STD numeric_limits<STD complex<long double> >::is_specialized);




	}
	end_chk("_1821T31");
#else
	skip_chk("_1821T31");
#endif /* CASE_1821T31 */

/* _1821T41 <limits> summary */
#if !defined(SKIP_1821T41)&&(!defined(ONLY)||defined(CASE_1821T41))
	begin_chk("_1821T41");
	{
	STD float_round_style *p0; TOUCH(p0);
	STD float_denorm_style *p01; TOUCH(p01);	// ADDED [NOV97]
	STD numeric_limits<bool> *p1; TOUCH(p1);
	STD numeric_limits<char> *p2; TOUCH(p2);
	STD numeric_limits<signed char> *p3; TOUCH(p3);
	STD numeric_limits<unsigned char> *p4; TOUCH(p4);
	STD numeric_limits<wchar_t> *p5; TOUCH(p5);
	STD numeric_limits<short> *p6; TOUCH(p6);
	STD numeric_limits<unsigned short> *p7; TOUCH(p7);
	STD numeric_limits<int> *p8; TOUCH(p8);
	STD numeric_limits<unsigned int> *p9; TOUCH(p9);
	STD numeric_limits<long> *p10; TOUCH(p10);
	STD numeric_limits<unsigned long> *p11; TOUCH(p11);
	STD numeric_limits<float> *p12; TOUCH(p12);
	STD numeric_limits<double> *p13; TOUCH(p13);
	STD numeric_limits<long double> *p14; TOUCH(p14);
	chk(1);


	}
	end_chk("_1821T41");
#else
	skip_chk("_1821T41");
#endif /* CASE_1821T41 */

/* _18211T01 numeric_limits definition */
#if !defined(SKIP_18211T01)&&(!defined(ONLY)||defined(CASE_18211T01))
	begin_chk("_18211T01");
	{
	STD numeric_limits<void *> *px; TOUCH(px);
	chk(1);


	}
	end_chk("_18211T01");
#else
	skip_chk("_18211T01");
#endif /* CASE_18211T01 */

/* _18211T11 numeric_limits::is_specialized distinguishes specializations */
#if !defined(SKIP_18211T11)&&(!defined(ONLY)||defined(CASE_18211T11))
	begin_chk("_18211T11");
	{
	STD numeric_limits<wrapper_18211T11> x; TOUCH(x);
	chk(STD numeric_limits<int>::is_specialized && !x.is_specialized);



	}
	end_chk("_18211T11");
#else
	skip_chk("_18211T11");
#endif /* CASE_18211T11 */

/* _18211T21 radix, epsilon, and round_error meaningful for floating */
#if !defined(SKIP_18211T21)&&(!defined(ONLY)||defined(CASE_18211T21))
	begin_chk("_18211T21");
	{
	ieq(STD numeric_limits<float>::radix, FLT_RADIX);
	ieq(STD numeric_limits<double>::radix, FLT_RADIX);
	ieq(STD numeric_limits<long double>::radix, FLT_RADIX);
	chk(STD numeric_limits<float>::epsilon() == FLT_EPSILON);
	chk(STD numeric_limits<double>::epsilon() == DBL_EPSILON);
	chk(STD numeric_limits<long double>::epsilon() == LDBL_EPSILON);
	chk(0 < STD numeric_limits<float>::round_error());
	chk(0 < STD numeric_limits<double>::round_error());
	chk(0 < STD numeric_limits<long double>::round_error());





	}
	end_chk("_18211T21");
#else
	skip_chk("_18211T21");
#endif /* CASE_18211T21 */

/* _18211T41 numeric_limits template shall have all members, with 0 values */
#if !defined(SKIP_18211T41)&&(!defined(ONLY)||defined(CASE_18211T41))
	begin_chk("_18211T41");
	{
	typedef STD numeric_limits<wrapper_18211T41> myty;
	myty x; TOUCH(x);
	chk(!x.is_specialized);
	chk(x.min() == 0);
	chk(x.max() == 0);
	ieq(x.digits, 0);
	ieq(x.digits10, 0);
	chk(!x.is_signed);
	chk(!x.is_integer);
	chk(!x.is_exact);
	ieq(x.radix, 0);
	chk(x.epsilon() == 0);
	chk(x.round_error() == 0);
	ieq(x.min_exponent, 0);
	ieq(x.min_exponent10, 0);
	ieq(x.max_exponent, 0);
	ieq(x.max_exponent10, 0);
	chk(!x.has_infinity);
	chk(!x.has_quiet_NaN);
	chk(!x.has_signaling_NaN);
	chk(x.has_denorm == STD denorm_absent);	// CHANGED [NOV97]
	chk(!x.has_denorm_loss);
	chk(x.infinity() == 0);
	chk(x.quiet_NaN() == 0);
	chk(x.signaling_NaN() == 0);
	chk(x.denorm_min() == 0);
	chk(!x.is_iec559);
	chk(!x.is_bounded);
	chk(!x.is_modulo);
	chk(!x.traps);
	chk(!x.tinyness_before);
	chk(x.round_style == STD round_toward_zero);
	switch (1) {case myty::is_specialized: break; }	// added [JUL96]
	switch (1) {case myty::digits: break; }	// added [JUL96]
	switch (1) {case myty::digits10: break; }	// added [JUL96]
	switch (1) {case myty::is_signed: break; }	// added [JUL96]
	switch (1) {case myty::is_integer: break; }	// added [JUL96]
	switch (1) {case myty::is_exact: break; }	// added [JUL96]
	switch (1) {case myty::radix: break; }	// added [JUL96]
	switch (1) {case myty::min_exponent: break; }	// added [JUL96]
	switch (1) {case myty::min_exponent10: break; }	// added [JUL96]
	switch (1) {case myty::max_exponent: break; }	// added [JUL96]
	switch (1) {case myty::max_exponent10: break; }	// added [JUL96]
	switch (1) {case myty::has_infinity: break; }	// added [JUL96]
	switch (1) {case myty::has_quiet_NaN: break; }	// added [JUL96]
	switch (1) {case myty::has_signaling_NaN: break; }	// added [JUL96]
	switch (1) {case myty::has_denorm: break; }	// added [JUL96]
	switch (1) {case myty::has_denorm_loss: break; }	// added [JUL96]
	switch (1) {case myty::is_iec559: break; }	// added [JUL96]
	switch (1) {case myty::is_bounded: break; }	// added [JUL96]
	switch (1) {case myty::is_modulo: break; }	// added [JUL96]
	switch (1) {case myty::traps: break; }	// added [JUL96]
	switch (1) {case myty::tinyness_before: break; }	// added [JUL96]
	switch (1) {case myty::round_style: break; }	// added [JUL96]


	}
	end_chk("_18211T41");
#else
	skip_chk("_18211T41");
#endif /* CASE_18211T41 */

/* _18212T11 numeric_limits::min() */
#if !defined(SKIP_18212T11)&&(!defined(ONLY)||defined(CASE_18212T11))
	begin_chk("_18212T11");
	{
	chk(STD numeric_limits<Bool>::min() == false);
	chk(STD numeric_limits<char>::min() == CHAR_MIN);
	chk(STD numeric_limits<signed char>::min() == SCHAR_MIN);
	chk(STD numeric_limits<unsigned char>::min() == 0);
	chk(STD numeric_limits<wchar_t>::min() == WCHAR_MIN);
	chk(STD numeric_limits<short>::min() == SHRT_MIN);
	chk(STD numeric_limits<unsigned short>::min() == 0);
	chk(STD numeric_limits<int>::min() == INT_MIN);
	chk(STD numeric_limits<unsigned int>::min() == 0);
	chk(STD numeric_limits<long>::min() == LONG_MIN);
	chk(STD numeric_limits<unsigned long>::min() == 0);



	}
	end_chk("_18212T11");
#else
	skip_chk("_18212T11");
#endif /* CASE_18212T11 */

/* _18212T21 numeric_limits::denorm_min() */
#if !defined(SKIP_18212T21)&&(!defined(ONLY)||defined(CASE_18212T21))
	begin_chk("_18212T21");
	{
	chk(STD numeric_limits<float>::min() == FLT_MIN);
	chk(STD numeric_limits<double>::min() == DBL_MIN);
	chk(STD numeric_limits<long double>::min() == LDBL_MIN);


	}
	end_chk("_18212T21");
#else
	skip_chk("_18212T21");
#endif /* CASE_18212T21 */

/* _18212T31 numeric_limits::denorm_min() meaningful for is_bounded */
#if !defined(SKIP_18212T31)&&(!defined(ONLY)||defined(CASE_18212T31))
	begin_chk("_18212T31");
	{
	chk((STD numeric_limits<signed char>::is_bounded
	    || !STD numeric_limits<signed char>::is_signed)
	        && STD numeric_limits<signed char>::min() < 0);
	chk((STD numeric_limits<short>::is_bounded
	    || !STD numeric_limits<short>::is_signed)
	        && STD numeric_limits<short>::min() < 0);
	chk((STD numeric_limits<int>::is_bounded
	    || !STD numeric_limits<int>::is_signed)
	        && STD numeric_limits<int>::min() < 0);
	chk((STD numeric_limits<long>::is_bounded
	    || !STD numeric_limits<long>::is_signed)
	        && STD numeric_limits<long>::min() < 0);
	chk((STD numeric_limits<float>::is_bounded
	    || !STD numeric_limits<float>::is_signed)
	        && STD numeric_limits<float>::min() != 0);
	chk((STD numeric_limits<double>::is_bounded
	    || !STD numeric_limits<double>::is_signed)
	        && STD numeric_limits<double>::min() != 0);
	chk((STD numeric_limits<long double>::is_bounded
	    || !STD numeric_limits<long double>::is_signed)
	        && STD numeric_limits<long double>::min() != 0);


	}
	end_chk("_18212T31");
#else
	skip_chk("_18212T31");
#endif /* CASE_18212T31 */

/* _18212T41 numeric_limits::max() */
#if !defined(SKIP_18212T41)&&(!defined(ONLY)||defined(CASE_18212T41))
	begin_chk("_18212T41");
	{
	chk(STD numeric_limits<Bool>::max() == true);
	chk(STD numeric_limits<char>::max() == CHAR_MAX);
	chk(STD numeric_limits<signed char>::max() == SCHAR_MAX);
	chk(STD numeric_limits<unsigned char>::max() == UCHAR_MAX);
	chk(STD numeric_limits<wchar_t>::max() == WCHAR_MAX);
	chk(STD numeric_limits<short>::max() == SHRT_MAX);
	chk(STD numeric_limits<unsigned short>::max() == USHRT_MAX);
	chk(STD numeric_limits<int>::max() == INT_MAX);
	chk(STD numeric_limits<unsigned int>::max() == UINT_MAX);
	chk(STD numeric_limits<long>::max() == LONG_MAX);
	chk(STD numeric_limits<unsigned long>::max() == ULONG_MAX);
	chk(STD numeric_limits<float>::max() == FLT_MAX);
	chk(STD numeric_limits<double>::max() == DBL_MAX);
	chk(STD numeric_limits<long double>::max() == LDBL_MAX);


	}
	end_chk("_18212T41");
#else
	skip_chk("_18212T41");
#endif /* CASE_18212T41 */

/* _18212T51 numeric_limits::max() meaningful for is_bounded */
#if !defined(SKIP_18212T51)&&(!defined(ONLY)||defined(CASE_18212T51))
	begin_chk("_18212T51");
	{
	chk(STD numeric_limits<bool>::is_bounded
	    && STD numeric_limits<bool>::max() != false);
	chk(STD numeric_limits<char>::is_bounded
	    && 0 < STD numeric_limits<char>::max());
	chk(STD numeric_limits<signed char>::is_bounded
	    && 0 < STD numeric_limits<signed char>::max());
	chk(STD numeric_limits<unsigned char>::is_bounded
	    && 0 < STD numeric_limits<unsigned char>::max());
	chk(STD numeric_limits<wchar_t>::is_bounded
	    && 0 < STD numeric_limits<wchar_t>::max());
	chk(STD numeric_limits<short>::is_bounded
	    && 0 < STD numeric_limits<short>::max());
	chk(STD numeric_limits<unsigned short>::is_bounded
	    && 0 < STD numeric_limits<unsigned short>::max());
	chk(STD numeric_limits<int>::is_bounded
	    && 0 < STD numeric_limits<int>::max());
	chk(STD numeric_limits<unsigned int>::is_bounded
	    && 0 < STD numeric_limits<unsigned int>::max());
	chk(STD numeric_limits<long>::is_bounded
	    && 0 < STD numeric_limits<long>::max());
	chk(STD numeric_limits<unsigned long>::is_bounded
	    && 0 < STD numeric_limits<unsigned long>::max());
	chk(STD numeric_limits<float>::is_bounded
	    && 0 < STD numeric_limits<float>::max());
	chk(STD numeric_limits<double>::is_bounded
	    && 0 < STD numeric_limits<double>::max());
	chk(STD numeric_limits<long double>::is_bounded
	    && 0 < STD numeric_limits<long double>::max());


	}
	end_chk("_18212T51");
#else
	skip_chk("_18212T51");
#endif /* CASE_18212T51 */

/* _18212T71 numeric_limits::digits for integer */
#if !defined(SKIP_18212T71)&&(!defined(ONLY)||defined(CASE_18212T71))
	begin_chk("_18212T71");
	{
	ieq(STD numeric_limits<Bool>::digits, 1);
	ieq(STD numeric_limits<char>::digits, CHAR_BIT - (CHAR_MIN < 0 ? 1 :
	0));
	ieq(STD numeric_limits<signed char>::digits, CHAR_BIT - 1);
	ieq(STD numeric_limits<unsigned char>::digits, CHAR_BIT);
	ieq(STD numeric_limits<wchar_t>::digits,
	    CHAR_BIT * sizeof (wchar_t) - (WCHAR_MIN < 0 ? 1 : 0));
	ieq(STD numeric_limits<short>::digits, CHAR_BIT * sizeof (short) -
	1);
	ieq(STD numeric_limits<unsigned short>::digits,
	    STD numeric_limits<short>::digits + 1);
	ieq(STD numeric_limits<int>::digits, CHAR_BIT * sizeof (int) - 1);
	ieq(STD numeric_limits<unsigned int>::digits,
	    STD numeric_limits<int>::digits + 1);
	ieq(STD numeric_limits<long>::digits, CHAR_BIT * sizeof (long) - 1);
	ieq(STD numeric_limits<unsigned long>::digits,
	    STD numeric_limits<long>::digits + 1);


	}
	end_chk("_18212T71");
#else
	skip_chk("_18212T71");
#endif /* CASE_18212T71 */

/* _18212T81 numeric_limits::digits for floating */
#if !defined(SKIP_18212T81)&&(!defined(ONLY)||defined(CASE_18212T81))
	begin_chk("_18212T81");
	{
	ieq(STD numeric_limits<float>::digits, FLT_MANT_DIG);
	ieq(STD numeric_limits<double>::digits, DBL_MANT_DIG);
	ieq(STD numeric_limits<long double>::digits, LDBL_MANT_DIG);


	}
	end_chk("_18212T81");
#else
	skip_chk("_18212T81");
#endif /* CASE_18212T81 */

/* _18212T91 numeric_limits::digits10 */
#if !defined(SKIP_18212T91)&&(!defined(ONLY)||defined(CASE_18212T91))
	begin_chk("_18212T91");
	{
	const float LOG2 = (float)0.30103;
	ieq((int)(LOG2 * (int)STD numeric_limits<bool>::digits),
	    STD numeric_limits<bool>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<char>::digits),
	    STD numeric_limits<char>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<signed char>::digits),
	    STD numeric_limits<signed char>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<unsigned char>::digits),
	    STD numeric_limits<unsigned char>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<wchar_t>::digits),
	    STD numeric_limits<wchar_t>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<short>::digits),
	    STD numeric_limits<short>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<unsigned short>::digits),
	    STD numeric_limits<unsigned short>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<int>::digits),
	    STD numeric_limits<int>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<unsigned int>::digits),
	    STD numeric_limits<unsigned int>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<long>::digits),
	    STD numeric_limits<long>::digits10);
	ieq((int)(LOG2 * (int)STD numeric_limits<unsigned long>::digits),
	    STD numeric_limits<unsigned long>::digits10);
	ieq(STD numeric_limits<float>::digits10, FLT_DIG);
	ieq(STD numeric_limits<double>::digits10, DBL_DIG);
	ieq(STD numeric_limits<long double>::digits10, LDBL_DIG);


	}
	end_chk("_18212T91");
#else
	skip_chk("_18212T91");
#endif /* CASE_18212T91 */

/* _18212T_101 numeric_limits::digits10 meaningful for is_bounded */
#if !defined(SKIP_18212T_101)&&(!defined(ONLY)||defined(CASE_18212T_101))
	begin_chk("_18212T_101");
	{
	chk(STD numeric_limits<char>::is_bounded
	    && 0 < STD numeric_limits<char>::digits10);
	chk(STD numeric_limits<signed char>::is_bounded
	    && 0 < STD numeric_limits<signed char>::digits10);
	chk(STD numeric_limits<unsigned char>::is_bounded
	    && 0 < STD numeric_limits<unsigned char>::digits10);
	chk(STD numeric_limits<wchar_t>::is_bounded
	    && 0 < STD numeric_limits<wchar_t>::digits10);
	chk(STD numeric_limits<short>::is_bounded
	    && 0 < STD numeric_limits<short>::digits10);
	chk(STD numeric_limits<unsigned short>::is_bounded
	    && 0 < STD numeric_limits<unsigned short>::digits10);
	chk(STD numeric_limits<int>::is_bounded
	    && 0 < STD numeric_limits<int>::digits10);
	chk(STD numeric_limits<unsigned int>::is_bounded
	    && 0 < STD numeric_limits<unsigned int>::digits10);
	chk(STD numeric_limits<long>::is_bounded
	    && 0 < STD numeric_limits<long>::digits10);
	chk(STD numeric_limits<unsigned long>::is_bounded
	    && 0 < STD numeric_limits<unsigned long>::digits10);
	chk(STD numeric_limits<float>::is_bounded
	    && 0 < STD numeric_limits<float>::digits10);
	chk(STD numeric_limits<double>::is_bounded
	    && 0 < STD numeric_limits<double>::digits10);
	chk(STD numeric_limits<long double>::is_bounded
	    && 0 < STD numeric_limits<long double>::digits10);


	}
	end_chk("_18212T_101");
#else
	skip_chk("_18212T_101");
#endif /* CASE_18212T_101 */

/* _18212T_111 numeric_limits::is_signed */
#if !defined(SKIP_18212T_111)&&(!defined(ONLY)||defined(CASE_18212T_111))
	begin_chk("_18212T_111");
	{
	chk(!STD numeric_limits<Bool>::is_signed);
	chk(STD numeric_limits<char>::is_signed && CHAR_MIN < 0
	    || !STD numeric_limits<char>::is_signed && CHAR_MIN == 0);
	chk(STD numeric_limits<signed char>::is_signed);
	chk(!STD numeric_limits<unsigned char>::is_signed);
	chk(STD numeric_limits<wchar_t>::is_signed && WCHAR_MIN < 0
	    || !STD numeric_limits<wchar_t>::is_signed && WCHAR_MIN == 0);
	chk(STD numeric_limits<short>::is_signed);
	chk(!STD numeric_limits<unsigned short>::is_signed);
	chk(STD numeric_limits<int>::is_signed);
	chk(!STD numeric_limits<unsigned int>::is_signed);
	chk(STD numeric_limits<long>::is_signed);
	chk(!STD numeric_limits<unsigned long>::is_signed);
	chk(STD numeric_limits<float>::is_signed);
	chk(STD numeric_limits<double>::is_signed);
	chk(STD numeric_limits<long double>::is_signed);


	}
	end_chk("_18212T_111");
#else
	skip_chk("_18212T_111");
#endif /* CASE_18212T_111 */

/* _18212T_131 numeric_limits::is_integer */
#if !defined(SKIP_18212T_131)&&(!defined(ONLY)||defined(CASE_18212T_131))
	begin_chk("_18212T_131");
	{
	chk(STD numeric_limits<bool>::is_integer);
	chk(STD numeric_limits<char>::is_integer);
	chk(STD numeric_limits<signed char>::is_integer);
	chk(STD numeric_limits<unsigned char>::is_integer);
	chk(STD numeric_limits<wchar_t>::is_integer);
	chk(STD numeric_limits<short>::is_integer);
	chk(STD numeric_limits<unsigned short>::is_integer);
	chk(STD numeric_limits<int>::is_integer);
	chk(STD numeric_limits<unsigned int>::is_integer);
	chk(STD numeric_limits<long>::is_integer);
	chk(STD numeric_limits<unsigned long>::is_integer);


	}
	end_chk("_18212T_131");
#else
	skip_chk("_18212T_131");
#endif /* CASE_18212T_131 */

/* _18212T_141 numeric_limits::is_integer meaningful for all specializations */
#if !defined(SKIP_18212T_141)&&(!defined(ONLY)||defined(CASE_18212T_141))
	begin_chk("_18212T_141");
	{
	chk(!STD numeric_limits<float>::is_integer);
	chk(!STD numeric_limits<double>::is_integer);
	chk(!STD numeric_limits<long double>::is_integer);


	}
	end_chk("_18212T_141");
#else
	skip_chk("_18212T_141");
#endif /* CASE_18212T_141 */

/* _18212T_151 numeric_limits::is_exact */
#if !defined(SKIP_18212T_151)&&(!defined(ONLY)||defined(CASE_18212T_151))
	begin_chk("_18212T_151");
	{
	chk(STD numeric_limits<bool>::is_exact);
	chk(STD numeric_limits<char>::is_exact);
	chk(STD numeric_limits<signed char>::is_exact);
	chk(STD numeric_limits<unsigned char>::is_exact);
	chk(STD numeric_limits<wchar_t>::is_exact);
	chk(STD numeric_limits<short>::is_exact);
	chk(STD numeric_limits<unsigned short>::is_exact);
	chk(STD numeric_limits<int>::is_exact);
	chk(STD numeric_limits<unsigned int>::is_exact);
	chk(STD numeric_limits<long>::is_exact);
	chk(STD numeric_limits<unsigned long>::is_exact);


	}
	end_chk("_18212T_151");
#else
	skip_chk("_18212T_151");
#endif /* CASE_18212T_151 */

/* _18212T_161 numeric_limits::is_exact meaningful for all specializations */
#if !defined(SKIP_18212T_161)&&(!defined(ONLY)||defined(CASE_18212T_161))
	begin_chk("_18212T_161");
	{
	chk(!STD numeric_limits<float>::is_exact);
	chk(!STD numeric_limits<double>::is_exact);
	chk(!STD numeric_limits<long double>::is_exact);

	//

	}
	end_chk("_18212T_161");
#else
	skip_chk("_18212T_161");
#endif /* CASE_18212T_161 */

/* _18212T_171 numeric_limits::radix for floating_point types */
#if !defined(SKIP_18212T_171)&&(!defined(ONLY)||defined(CASE_18212T_171))
	begin_chk("_18212T_171");
	{
	ieq(STD numeric_limits<float>::radix, FLT_RADIX);
	ieq(STD numeric_limits<double>::radix, FLT_RADIX);
	ieq(STD numeric_limits<long double>::radix, FLT_RADIX);


	}
	end_chk("_18212T_171");
#else
	skip_chk("_18212T_171");
#endif /* CASE_18212T_171 */

/* _18212T_181 numeric_limits::radix for integer */
#if !defined(SKIP_18212T_181)&&(!defined(ONLY)||defined(CASE_18212T_181))
	begin_chk("_18212T_181");
	{
	ieq(STD numeric_limits<bool>::radix, 2);
	ieq(STD numeric_limits<char>::radix, 2);
	ieq(STD numeric_limits<signed char>::radix, 2);
	ieq(STD numeric_limits<unsigned char>::radix, 2);
	ieq(STD numeric_limits<wchar_t>::radix, 2);
	ieq(STD numeric_limits<short>::radix, 2);
	ieq(STD numeric_limits<unsigned short>::radix, 2);
	ieq(STD numeric_limits<int>::radix, 2);
	ieq(STD numeric_limits<unsigned int>::radix, 2);
	ieq(STD numeric_limits<long>::radix, 2);
	ieq(STD numeric_limits<unsigned long>::radix, 2);


	}
	end_chk("_18212T_181");
#else
	skip_chk("_18212T_181");
#endif /* CASE_18212T_181 */

/* _18212T_201 numeric_limits::epsilon() */
#if !defined(SKIP_18212T_201)&&(!defined(ONLY)||defined(CASE_18212T_201))
	begin_chk("_18212T_201");
	{
	chk(STD numeric_limits<float>::epsilon() == FLT_EPSILON);
	chk(STD numeric_limits<double>::epsilon() == DBL_EPSILON);
	chk(STD numeric_limits<long double>::epsilon() == LDBL_EPSILON);


	}
	end_chk("_18212T_201");
#else
	skip_chk("_18212T_201");
#endif /* CASE_18212T_201 */

/* _18212T_211 numeric_limits::epsilon() meaningful only for floating */
#if !defined(SKIP_18212T_211)&&(!defined(ONLY)||defined(CASE_18212T_211))
	begin_chk("_18212T_211");
	{
	chk(STD numeric_limits<bool>::epsilon() == 0);
	chk(STD numeric_limits<char>::epsilon() == 0);
	chk(STD numeric_limits<signed char>::epsilon() == 0);
	chk(STD numeric_limits<unsigned char>::epsilon() == 0);
	chk(STD numeric_limits<wchar_t>::epsilon() == 0);
	chk(STD numeric_limits<short>::epsilon() == 0);
	chk(STD numeric_limits<unsigned short>::epsilon() == 0);
	chk(STD numeric_limits<int>::epsilon() == 0);
	chk(STD numeric_limits<unsigned int>::epsilon() == 0);
	chk(STD numeric_limits<long>::epsilon() == 0);
	chk(STD numeric_limits<unsigned long>::epsilon() == 0);
	}
	end_chk("_18212T_211");
#else
	skip_chk("_18212T_211");
#endif /* CASE_18212T_211 */

return leave_chk("t180");
}
/* V3.10:0009 */
