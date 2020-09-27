/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _262T11  <complex> synopsis */
#if !defined(SKIP_262T11)&&(!defined(ONLY)||defined(CASE_262T11))
#if DISALLOW_USER_NAME_PROTECTION
 #include <complex>
#else
	#define re	"re"	/* looking for trouble */
	#define im	"im"
 #include <complex>
	#undef im
	#undef re
#endif
#endif /* CASE_262T11 */

/* _2621T11  complex definition */
#if !defined(SKIP_2621T11)&&(!defined(ONLY)||defined(CASE_2621T11))
#include <complex>
#endif /* CASE_2621T11 */

/* _2622T11  complex<float, double, long double> definitions */
#if !defined(SKIP_2622T11)&&(!defined(ONLY)||defined(CASE_2622T11))
#include <complex>
#endif /* CASE_2622T11 */

/* _2623T11 complex(T re = T(), T im = T())    effects, postcondition */
#if !defined(SKIP_2623T11)&&(!defined(ONLY)||defined(CASE_2623T11))
#include <complex>
#endif /* CASE_2623T11 */

/* _2624T11 operator+=   effects, returns */
#if !defined(SKIP_2624T11)&&(!defined(ONLY)||defined(CASE_2624T11))
#include <complex>
#endif /* CASE_2624T11 */

/* _2624T21 operator-=   effects, returns */
#if !defined(SKIP_2624T21)&&(!defined(ONLY)||defined(CASE_2624T21))
#include <complex>
#endif /* CASE_2624T21 */

/* _2624T31 operator*=   effects, returns */
#if !defined(SKIP_2624T31)&&(!defined(ONLY)||defined(CASE_2624T31))
#include <complex>
#endif /* CASE_2624T31 */

/* _2624T41 operator/=   effects, returns */
#if !defined(SKIP_2624T41)&&(!defined(ONLY)||defined(CASE_2624T41))
#include <complex>
#endif /* CASE_2624T41 */

/* _2625T11 unary  op+(complx)  effects, returns */
#if !defined(SKIP_2625T11)&&(!defined(ONLY)||defined(CASE_2625T11))
#include <complex>
#endif /* CASE_2625T11 */

/* _2625T21 op+(cmplx, cmplx)  effects, returns */
#if !defined(SKIP_2625T21)&&(!defined(ONLY)||defined(CASE_2625T21))
#include <complex>
#endif /* CASE_2625T21 */

/* _2625T22 op+(cmplx, T)  effects, returns */
#if !defined(SKIP_2625T22)&&(!defined(ONLY)||defined(CASE_2625T22))
#include <complex>
#endif /* CASE_2625T22 */

/* _2625T23 op+(T, cmplx)  effects, returns */
#if !defined(SKIP_2625T23)&&(!defined(ONLY)||defined(CASE_2625T23))
#include <complex>
#endif /* CASE_2625T23 */

/* _2625T31 unary op-(cmplx) */
#if !defined(SKIP_2625T31)&&(!defined(ONLY)||defined(CASE_2625T31))
#include <complex>
#endif /* CASE_2625T31 */

/* _2625T41 op-(cmplx, cmplx)  effects, returns */
#if !defined(SKIP_2625T41)&&(!defined(ONLY)||defined(CASE_2625T41))
#include <complex>
#endif /* CASE_2625T41 */

/* _2625T42 op-(cmplx, T)  effects, returns */
#if !defined(SKIP_2625T42)&&(!defined(ONLY)||defined(CASE_2625T42))
#include <complex>
#endif /* CASE_2625T42 */

/* _2625T43 op-(T, cmplx)  effects, returns */
#if !defined(SKIP_2625T43)&&(!defined(ONLY)||defined(CASE_2625T43))
#include <complex>
#endif /* CASE_2625T43 */

/* _2625T51 op*(cmplx, cmplx)  effects, returns */
#if !defined(SKIP_2625T51)&&(!defined(ONLY)||defined(CASE_2625T51))
#include <complex>
#endif /* CASE_2625T51 */

/* _2625T52 op*(cmplx, T)  effects, returns */
#if !defined(SKIP_2625T52)&&(!defined(ONLY)||defined(CASE_2625T52))
#include <complex>
#endif /* CASE_2625T52 */

/* _2625T53 op*(T, cmplx)  effects, returns */
#if !defined(SKIP_2625T53)&&(!defined(ONLY)||defined(CASE_2625T53))
#include <complex>
#endif /* CASE_2625T53 */

int t260_main(int, char *[])
{
enter_chk("t260.cpp");
/* _262T11  <complex> synopsis */
#if !defined(SKIP_262T11)&&(!defined(ONLY)||defined(CASE_262T11))
	begin_chk("_262T11");
	{
	CPXfloat *p0 = 0; TOUCH(p0);
	CPXdouble *p1 = 0; TOUCH(p1);
	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble *p2 = 0; TOUCH(p2);
	#endif /* IS_EMBEDDED */
	chk(1);





	}
	end_chk("_262T11");
#else
	skip_chk("_262T11");
#endif /* CASE_262T11 */

/* _2621T11  complex definition */
#if !defined(SKIP_2621T11)&&(!defined(ONLY)||defined(CASE_2621T11))
	begin_chk("_2621T11");
	{
	#if IS_EMBEDDED
	typedef float_complex Mycplx;
	float *p0 = (Mycplx::value_type *)0; TOUCH(p0);	// ADDED [MAR96]
	#else /* IS_EMBEDDED */
	typedef complex<Myfloat> Mycplx;
	Myfloat *p0 = (Mycplx::value_type *)0; TOUCH(p0);	// ADDED [MAR96]
	#endif /* IS_EMBEDDED */
	Mycplx c0(5, -3);

	chk(c0.real() == 5);
	chk(c0.imag() == -3);


	}
	end_chk("_2621T11");
#else
	skip_chk("_2621T11");
#endif /* CASE_2621T11 */

/* _2622T11  complex<float, double, long double> definitions */
#if !defined(SKIP_2622T11)&&(!defined(ONLY)||defined(CASE_2622T11))
	begin_chk("_2622T11");
	{
	typedef CPXfloat Mycplx0;
	float *p0 = (Mycplx0::value_type *)0; TOUCH(p0);	// ADDED [MAR96]
	Mycplx0 c0;
	chk(c0.real() == 0 && c0.imag() == 0);

	typedef CPXdouble Mycplx1;
	double *p1 = (Mycplx1::value_type *)0; TOUCH(p1);	// ADDED [MAR96]
	Mycplx1 c1(5);
	chk(c1.real() == 5 && c1.imag() == 0);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	typedef CPXldouble Mycplx2;
	long double *p2 = (Mycplx2::value_type *)0; TOUCH(p2);	// ADDED [MAR96]
	Mycplx2 c2(5, -3);
	chk(c2.real() == 5 && c2.imag() == -3);
	#endif /* IS_EMBEDDED */





	}
	end_chk("_2622T11");
#else
	skip_chk("_2622T11");
#endif /* CASE_2622T11 */

/* _2623T11 complex(T re = T(), T im = T())    effects, postcondition */
#if !defined(SKIP_2623T11)&&(!defined(ONLY)||defined(CASE_2623T11))
	begin_chk("_2623T11");
	{
	CPXfloat cf;
	feq(cf.real(), 0.F);
	feq(cf.imag(), 0.F);
	feq((cf = (float)3.0).real(), 3.F);	// ADDED [MAR96]
	CPXfloat cf2 (9.);
	feq(cf2.real(), 9.F);
	feq(cf2.imag(), 0.F);
	CPXfloat cf3 (8., 7.F);
	feq(cf3.real(), 8.F);
	feq(cf3.imag(), 7.F);

	CPXdouble cd;
	deq(cd.real(), 0.);
	deq(cd.imag(), 0.);
	deq((cd = (double)3.0).real(), 3);	// ADDED [MAR96]
	CPXdouble cd2 (9.);
	deq(cd2.real(), 9.);
	deq(cd2.imag(), 0.);
	CPXdouble cd3 (8., 7.);
	deq(cd3.real(), 8.);
	deq(cd3.imag(), 7.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld;
	ldeq(cld.real(), 0.);
	ldeq(cld.imag(), 0.);
	ldeq((cld = (long double)3.0).real(), 3);	// ADDED [MAR96]
	CPXldouble cld2 (9.);
	ldeq(cld2.real(), 9.);
	ldeq(cld2.imag(), 0.);
	CPXldouble cld3 (8., 7.);
	ldeq(cld3.real(), 8.);
	ldeq(cld3.imag(), 7.);

	complex<Myfloat> ci;
	chk(ci.real() == 0);
	chk(ci.imag() == 0);
	ci = (Myfloat)3.0;
	chk(ci.real() == 3);	// ADDED [MAR96]
	complex<Myfloat> ci2 (9);
	chk(ci2.real() == 9);
	chk(ci2.imag() == 0);
	complex<Myfloat> ci3 (8, 7);
	chk(ci3.real() == 8);
	chk(ci3.imag() == 7);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2623T11");
#else
	skip_chk("_2623T11");
#endif /* CASE_2623T11 */

/* _2624T11 operator+=   effects, returns */
#if !defined(SKIP_2624T11)&&(!defined(ONLY)||defined(CASE_2624T11))
	begin_chk("_2624T11");
	{
	CPXfloat cf (9., 8.);
	CPXfloat cf2 (7., 5.);
	feq((cf += cf2).real(), 16.F);
	feq(cf.real(), 16.F);
	feq(cf.imag(), 13.F);
	feq((cf += (float)3.0).real(), 19.F);	// ADDED [MAR96]

	CPXdouble cd (9., 8.);
	CPXdouble cd2 (7., 5.);
	deq((cd += cd2).real(), 16);
	deq(cd.real(), 16.);
	deq(cd.imag(), 13.);
	deq((cd += (double)3.0).real(), 19);	// ADDED [MAR96]

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	CPXldouble cld2 (7., 5.);
	ldeq((cld += cld2).real(), 16);
	ldeq(cld.real(), 16.);
	ldeq(cld.imag(), 13.);
	ldeq((cld += (long double)3.0).real(), 19);	// ADDED [MAR96]

	complex<Myfloat> ci (9, 8);
	complex<Myfloat> ci2 (7, 5);
	chk((ci += ci2).real() == 16);
	chk(ci.real() == 16);
	chk(ci.imag() == 13);
	chk((ci += (Myfloat)3.0).real() == 19);	// ADDED [MAR96]
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2624T11");
#else
	skip_chk("_2624T11");
#endif /* CASE_2624T11 */

/* _2624T21 operator-=   effects, returns */
#if !defined(SKIP_2624T21)&&(!defined(ONLY)||defined(CASE_2624T21))
	begin_chk("_2624T21");
	{
	CPXfloat cf (9., 8.);
	CPXfloat cf2 (7., 5.);
	feq((cf -= cf2).real(), 2.F);
	feq(cf.real(), 2.F);
	feq(cf.imag(), 3.F);
	feq((cf -= (float)3.0).real(), -1.F);	// ADDED [MAR96]

	CPXdouble cd (9., 8.);
	CPXdouble cd2 (7., 5.);
	deq((cd -= cd2).real(), 2.);
	deq(cd.real(), 2.);
	deq(cd.imag(), 3.);
	deq((cd -= (double)3.0).real(), -1);	// ADDED [MAR96]

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	CPXldouble cld2 (7., 5.);
	ldeq((cld -= cld2).real(), 2.);
	ldeq(cld.real(), 2.);
	ldeq(cld.imag(), 3.);
	ldeq((cld -= (long double)3.0).real(), -1);	// ADDED [MAR96]

	complex<Myfloat> ci (9, 8);
	complex<Myfloat> ci2 (7, 5);
	chk((ci -= ci2).real() == 2);
	chk(ci.real() == 2);
	chk(ci.imag() == 3);
	chk((ci -= (float)3.0).real() == -1);	// ADDED [MAR96]
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2624T21");
#else
	skip_chk("_2624T21");
#endif /* CASE_2624T21 */

/* _2624T31 operator*=   effects, returns */
#if !defined(SKIP_2624T31)&&(!defined(ONLY)||defined(CASE_2624T31))
	begin_chk("_2624T31");
	{
	CPXfloat cf (9., 8.);
	CPXfloat cf2 (7., 5.);
	feq((cf *= cf2).real(), 23.F);
	feq(cf.real(), 23.F);
	feq(cf.imag(), 101.F);
	feq((cf *= (float)3.0).real(), 69.F);	// ADDED [MAR96]

	CPXdouble cd (9., 8.);
	CPXdouble cd2 (7., 5.);
	deq((cd *= cd2).real(), 23.);
	deq(cd.real(), 23.);
	deq(cd.imag(), 101.);
	deq((cd *= (double)3.0).real(), 69);	// ADDED [MAR96]

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	CPXldouble cld2 (7., 5.);
	ldeq((cld *= cld2).real(), 23.);
	ldeq(cld.real(), 23.);
	ldeq(cld.imag(), 101.);
	ldeq((cld *= (long double)3.0).real(), 69);	// ADDED [MAR96]

	complex<Myfloat> ci (9, 8);
	complex<Myfloat> ci2 (7, 5);
	chk((ci *= ci2).real() == 23);
	chk(ci.real() == 23);
	chk(ci.imag() == 101);
	chk((ci *= (Myfloat)3.0).real() == 69);	// ADDED [MAR96]
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2624T31");
#else
	skip_chk("_2624T31");
#endif /* CASE_2624T31 */

/* _2624T41 operator/=   effects, returns */
#if !defined(SKIP_2624T41)&&(!defined(ONLY)||defined(CASE_2624T41))
	begin_chk("_2624T41");
	{
	CPXfloat cf (23., 101.);
	CPXfloat cf2 (9., 8.);
	feq((cf /= cf2).real(), 7.F);
	feq(cf.real(), 7.F);
	feq(cf.imag(), 5.F);
	feq((cf /= (float)7.0).real(), 1.F);	// ADDED [MAR96]

	CPXdouble cd (23., 101.);
	CPXdouble cd2 (9., 8.);
	deq((cd /= cd2).real(), 7.);
	deq(cd.real(), 7.);
	deq(cd.imag(), 5.);
	deq((cd /= (double)7.0).real(), 1);	// ADDED [MAR96]

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (23., 101.);
	CPXldouble cld2 (9., 8.);
	ldeq((cld /= cld2).real(), 7.);
	ldeq(cld.real(), 7.);
	ldeq(cld.imag(), 5.);
	ldeq((cld /= (long double)7.0).real(), 1);	// ADDED [MAR96]

	complex<Myfloat> ci (23, 101);
	complex<Myfloat> ci2 (9, 8);
	deq((ci /= ci2).real(), 7.);
	deq(ci.real(), 7.);
	deq(ci.imag(), 5.);
	deq((ci /= (Myfloat)7.0).real(), 1.);	// ADDED [MAR96]
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2624T41");
#else
	skip_chk("_2624T41");
#endif /* CASE_2624T41 */

/* _2625T11 unary  op+(complx)  effects, returns */
#if !defined(SKIP_2625T11)&&(!defined(ONLY)||defined(CASE_2625T11))
	begin_chk("_2625T11");
	{
	CPXfloat cf (9., 8.);
	feq((+cf).real(), 9.F);
	feq((+cf).imag(), 8.F);

	CPXdouble cd (9., 8.);
	deq((+cd).real(), 9.);
	deq((+cd).imag(), 8.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	ldeq((+cld).real(), 9.);
	ldeq((+cld).imag(), 8.);

	complex<Myfloat> ci (9, 8);
	chk((+ci).real() == 9);
	chk((+ci).imag() == 8);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2625T11");
#else
	skip_chk("_2625T11");
#endif /* CASE_2625T11 */

/* _2625T21 op+(cmplx, cmplx)  effects, returns */
#if !defined(SKIP_2625T21)&&(!defined(ONLY)||defined(CASE_2625T21))
	begin_chk("_2625T21");
	{
	CPXfloat cf (9., 8.);
	CPXfloat cf2 (7., 5.);
	feq((cf + cf2).real(), 16.F);
	feq((cf + cf2).imag(), 13.F);

	CPXdouble cd (9., 8.);
	CPXdouble cd2 (7., 5.);
	deq((cd + cd2).real(), 16.);
	deq((cd + cd2).imag(), 13.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	CPXldouble cld2 (7., 5.);
	ldeq((cld + cld2).real(), 16.);
	ldeq((cld + cld2).imag(), 13.);

	complex<Myfloat> ci (9, 8);
	complex<Myfloat> ci2 (7, 5);
	chk((ci + ci2).real() == 16);
	chk((ci + ci2).imag() == 13);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2625T21");
#else
	skip_chk("_2625T21");
#endif /* CASE_2625T21 */

/* _2625T22 op+(cmplx, T)  effects, returns */
#if !defined(SKIP_2625T22)&&(!defined(ONLY)||defined(CASE_2625T22))
	begin_chk("_2625T22");
	{
	CPXfloat cf (9., 8.);
	float f2 = 7.;
	feq((cf + f2).real(), 16.F);
	feq((cf + f2).imag(), 8.F);

	CPXdouble cd (9., 8.);
	double d2 = 7.;
	deq((cd + d2).real(), 16.);
	deq((cd + d2).imag(), 8.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	long double ld2 = 7.;
	ldeq((cld + ld2).real(), 16.);
	ldeq((cld + ld2).imag(), 8.);

	complex<Myfloat> ci (9, 8);
	Myfloat i2 = 7;
	chk((ci + i2).real() == 16);
	chk((ci + i2).imag() == 8);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2625T22");
#else
	skip_chk("_2625T22");
#endif /* CASE_2625T22 */

/* _2625T23 op+(T, cmplx)  effects, returns */
#if !defined(SKIP_2625T23)&&(!defined(ONLY)||defined(CASE_2625T23))
	begin_chk("_2625T23");
	{
	CPXfloat cf (9., 8.);
	float f2 = 7.;
	feq((f2 + cf).real(), 16.F);
	feq((f2 + cf).imag(), 8.F);

	CPXdouble cd (9., 8.);
	double d2 = 7.;
	deq((d2 + cd).real(), 16.);
	deq((d2 + cd).imag(), 8.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	long double ld2 = 7.;
	ldeq((ld2 + cld).real(), 16.);
	ldeq((ld2 + cld).imag(), 8.);

	complex<Myfloat> ci (9, 8);
	Myfloat i2 = 7;
	chk((i2 + ci).real() == 16);
	chk((i2 + ci).imag() == 8);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2625T23");
#else
	skip_chk("_2625T23");
#endif /* CASE_2625T23 */

/* _2625T31 unary op-(cmplx) */
#if !defined(SKIP_2625T31)&&(!defined(ONLY)||defined(CASE_2625T31))
	begin_chk("_2625T31");
	{
	CPXfloat cf (9., 8.);
	feq((-cf).real(), -9.F);
	feq((-cf).imag(), -8.F);

	CPXdouble cd (9., 8.);
	deq((-cd).real(), -9.);
	deq((-cd).imag(), -8.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	ldeq((-cld).real(), -9.);
	ldeq((-cld).imag(), -8.);

	complex<Myfloat> ci (9, 8);
	chk((-ci).real() == -9);
	chk((-ci).imag() == -8);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2625T31");
#else
	skip_chk("_2625T31");
#endif /* CASE_2625T31 */

/* _2625T41 op-(cmplx, cmplx)  effects, returns */
#if !defined(SKIP_2625T41)&&(!defined(ONLY)||defined(CASE_2625T41))
	begin_chk("_2625T41");
	{
	CPXfloat cf (9., 8.);
	CPXfloat cf2 (7., 5.);
	feq((cf - cf2).real(), 2.F);
	feq((cf - cf2).imag(), 3.F);

	CPXdouble cd (9., 8.);
	CPXdouble cd2 (7., 5.);
	deq((cd - cd2).real(), 2.);
	deq((cd - cd2).imag(), 3.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	CPXldouble cld2 (7., 5.);
	ldeq((cld - cld2).real(), 2.);
	ldeq((cld - cld2).imag(), 3.);

	complex<Myfloat> ci (9, 8);
	complex<Myfloat> ci2 (7, 5);
	chk((ci - ci2).real() == 2);
	chk((ci - ci2).imag() == 3);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2625T41");
#else
	skip_chk("_2625T41");
#endif /* CASE_2625T41 */

/* _2625T42 op-(cmplx, T)  effects, returns */
#if !defined(SKIP_2625T42)&&(!defined(ONLY)||defined(CASE_2625T42))
	begin_chk("_2625T42");
	{
	CPXfloat cf (9., 8.);
	float f2 = 7.;
	feq((cf - f2).real(), 2.F);
	feq((cf - f2).imag(), 8.F);

	CPXdouble cd (9., 8.);
	double d2 = 7.;
	deq((cd - d2).real(), 2.);
	deq((cd - d2).imag(), 8.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	long double ld2 = 7.;
	ldeq((cld - ld2).real(), 2.);
	ldeq((cld - ld2).imag(), 8.);

	complex<Myfloat> ci (9, 8);
	Myfloat i2 = 7;
	chk((ci - i2).real() == 2);
	chk((ci - i2).imag() == 8);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2625T42");
#else
	skip_chk("_2625T42");
#endif /* CASE_2625T42 */

/* _2625T43 op-(T, cmplx)  effects, returns */
#if !defined(SKIP_2625T43)&&(!defined(ONLY)||defined(CASE_2625T43))
	begin_chk("_2625T43");
	{
	CPXfloat cf (9., 8.);
	float f2 = 7.;
	feq((f2 - cf).real(), -2.F);
	feq((f2 - cf).imag(), -8.F);

	CPXdouble cd (9., 8.);
	double d2 = 7.;
	deq((d2 - cd).real(), -2.);
	deq((d2 - cd).imag(), -8.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	long double ld2 = 7.;
	ldeq((ld2 - cld).real(), -2.);
	ldeq((ld2 - cld).imag(), -8.);

	complex<Myfloat> ci (9, 8);
	Myfloat i2 = 7;
	chk((i2 - ci).real() == -2);
	chk((i2 - ci).imag() == -8);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2625T43");
#else
	skip_chk("_2625T43");
#endif /* CASE_2625T43 */

/* _2625T51 op*(cmplx, cmplx)  effects, returns */
#if !defined(SKIP_2625T51)&&(!defined(ONLY)||defined(CASE_2625T51))
	begin_chk("_2625T51");
	{
	CPXfloat cf (9., 8.);
	CPXfloat cf2 (7., 5.);
	feq((cf * cf2).real(), 23.F);
	feq((cf * cf2).imag(), 101.F);

	CPXdouble cd (9., 8.);
	CPXdouble cd2 (7., 5.);
	deq((cd * cd2).real(), 23.);
	deq((cd * cd2).imag(), 101.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	CPXldouble cld2 (7., 5.);
	ldeq((cld * cld2).real(), 23.);
	ldeq((cld * cld2).imag(), 101.);

	complex<Myfloat> ci (9, 8);
	complex<Myfloat> ci2 (7, 5);
	chk((ci * ci2).real() == 23);
	chk((ci * ci2).imag() == 101);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2625T51");
#else
	skip_chk("_2625T51");
#endif /* CASE_2625T51 */

/* _2625T52 op*(cmplx, T)  effects, returns */
#if !defined(SKIP_2625T52)&&(!defined(ONLY)||defined(CASE_2625T52))
	begin_chk("_2625T52");
	{
	CPXfloat cf (9., 8.);
	float f2 = 7.;
	feq((cf * f2).real(), 63.F);
	feq((cf * f2).imag(), 56.F);

	CPXdouble cd (9., 8.);
	double d2 = 7.;
	deq((cd * d2).real(), 63.);
	deq((cd * d2).imag(), 56.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	long double ld2 = 7.;
	ldeq((cld * ld2).real(), 63.);
	ldeq((cld * ld2).imag(), 56.);

	complex<Myfloat> ci (9, 8);
	Myfloat i2 = 7;
	chk((ci * i2).real() == 63);
	chk((ci * i2).imag() == 56);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2625T52");
#else
	skip_chk("_2625T52");
#endif /* CASE_2625T52 */

/* _2625T53 op*(T, cmplx)  effects, returns */
#if !defined(SKIP_2625T53)&&(!defined(ONLY)||defined(CASE_2625T53))
	begin_chk("_2625T53");
	{
	CPXfloat cf (9., 8.);
	float f2 = 7.;
	feq((f2 * cf).real(), 63.F);
	feq((f2 * cf).imag(), 56.F);

	CPXdouble cd (9., 8.);
	double d2 = 7.;
	deq((d2 * cd).real(), 63.);
	deq((d2 * cd).imag(), 56.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (9., 8.);
	long double ld2 = 7.;
	ldeq((ld2 * cld).real(), 63.);
	ldeq((ld2 * cld).imag(), 56.);

	complex<Myfloat> ci (9, 8);
	Myfloat i2 = 7;
	chk((i2 * ci).real()== 63);
	chk((i2 * ci).imag() == 56);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2625T53");
#else
	skip_chk("_2625T53");
#endif /* CASE_2625T53 */

return leave_chk("t260");
}
/* V3.10:0009 */
