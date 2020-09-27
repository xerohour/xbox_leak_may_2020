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
#define CASE_2627T71     cos  (const complex<T>& x)
#define CASE_2627T81     cosh (const complex<T>& x)
#define CASE_2627T91     exp  (const complex<T>& x)
#define CASE_2627T_101   log  (const complex<T>& x)
#define CASE_2627T_111   log10(const complex<T>& x)
#define CASE_2627T_121   pow(const complex<T>& x, const complex<T>& y)
#define CASE_2627T_131   pow  (const complex<T>& x, T y)
#define CASE_2627T_141   pow  (T x, const complex<T>& y)
#define CASE_2627T_151   pow  (const complex<T>& x, int y)
#define CASE_2627T_161   sin  (const complex<T>& x)
#define CASE_2627T_171   sinh (const complex<T>& x)
#define CASE_2627T_181   sqrt (const complex<T>& x)
#define CASE_2627T_191   tan  (const complex<T>& x)
#define CASE_2627T_201   tanh (const complex<T>& x)
//#define CASE_263T11  <valarray> synopsis
//#define CASE_2631T11  class valarray definition
#endif /* IS_EMBEDDED */

/* _2627T71     cos  (const complex<T>& x) */
#if !defined(SKIP_2627T71)&&(!defined(ONLY)||defined(CASE_2627T71))
#include <complex>
#endif /* CASE_2627T71 */

/* _2627T81     cosh (const complex<T>& x) */
#if !defined(SKIP_2627T81)&&(!defined(ONLY)||defined(CASE_2627T81))
#include <complex>
#endif /* CASE_2627T81 */

/* _2627T91     exp  (const complex<T>& x) */
#if !defined(SKIP_2627T91)&&(!defined(ONLY)||defined(CASE_2627T91))
#include <complex>
#endif /* CASE_2627T91 */

/* _2627T_101   log  (const complex<T>& x) */
#if !defined(SKIP_2627T_101)&&(!defined(ONLY)||defined(CASE_2627T_101))
#include <complex>
#endif /* CASE_2627T_101 */

/* _2627T_111   log10(const complex<T>& x) */
#if !defined(SKIP_2627T_111)&&(!defined(ONLY)||defined(CASE_2627T_111))
#include <complex>
#endif /* CASE_2627T_111 */

/* _2627T_121   pow(const complex<T>& x, const complex<T>& y) */
#if !defined(SKIP_2627T_121)&&(!defined(ONLY)||defined(CASE_2627T_121))
#include <complex>
#endif /* CASE_2627T_121 */

/* _2627T_131   pow  (const complex<T>& x, T y) */
#if !defined(SKIP_2627T_131)&&(!defined(ONLY)||defined(CASE_2627T_131))
#include <complex>
#endif /* CASE_2627T_131 */

/* _2627T_141   pow  (T x, const complex<T>& y) */
#if !defined(SKIP_2627T_141)&&(!defined(ONLY)||defined(CASE_2627T_141))
#include <complex>
#endif /* CASE_2627T_141 */

/* _2627T_151   pow  (const complex<T>& x, int y) */
#if !defined(SKIP_2627T_151)&&(!defined(ONLY)||defined(CASE_2627T_151))
#include <complex>
#endif /* CASE_2627T_151 */

/* _2627T_161   sin  (const complex<T>& x) */
#if !defined(SKIP_2627T_161)&&(!defined(ONLY)||defined(CASE_2627T_161))
#include <complex>
#endif /* CASE_2627T_161 */

/* _2627T_171   sinh (const complex<T>& x) */
#if !defined(SKIP_2627T_171)&&(!defined(ONLY)||defined(CASE_2627T_171))
#include <complex>
#endif /* CASE_2627T_171 */

/* _2627T_181   sqrt (const complex<T>& x) */
#if !defined(SKIP_2627T_181)&&(!defined(ONLY)||defined(CASE_2627T_181))
#include <complex>
#endif /* CASE_2627T_181 */

/* _2627T_191   tan (const complex<T>& x) */
#if !defined(SKIP_2627T_191)&&(!defined(ONLY)||defined(CASE_2627T_191))
#include <complex>
#endif /* CASE_2627T_191 */

/* _2627T_201   tanh (const complex<T>& x) */
#if !defined(SKIP_2627T_201)&&(!defined(ONLY)||defined(CASE_2627T_201))
#include <complex>
#endif /* CASE_2627T_201 */

/* _263T11  <valarray> synopsis */
#if !defined(SKIP_263T11)&&(!defined(ONLY)||defined(CASE_263T11))
#include <valarray>
#endif /* CASE_263T11 */

/* _2631T11  class valarray definition */
#if !defined(SKIP_2631T11)&&(!defined(ONLY)||defined(CASE_2631T11))
#include <valarray>
#endif /* CASE_2631T11 */

int t262_main(int, char *[])
{
enter_chk("t262.cpp");
/* _2627T71     cos  (const complex<T>& x) */
#if !defined(SKIP_2627T71)&&(!defined(ONLY)||defined(CASE_2627T71))
	begin_chk("_2627T71");
	{
	CPXfloat cf (1., 0.);
	feq(cos(cf).real(), 0.5403023058681398F);
	feq(cos(cf).imag(), 0.F);

	CPXdouble cd (1., 1.);
	deq(cos(cd).real(), 8.3373002513114904887E-01);
	deq(cos(cd).imag(), -9.8889770576286509626E-01);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (piby4, -1.);
	ldeq(cos(cld).real(), cos1);
	ldeq(cos(cld).imag(), sin1);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T71");
#else
	skip_chk("_2627T71");
#endif /* CASE_2627T71 */

/* _2627T81     cosh (const complex<T>& x) */
#if !defined(SKIP_2627T81)&&(!defined(ONLY)||defined(CASE_2627T81))
	begin_chk("_2627T81");
	{
	CPXfloat cf (1., 0.);
	feq(cosh(cf).real(), 1.543080634815244F);
	feq(cosh(cf).imag(), 0.F);

	CPXdouble cd (1., 1.);
	deq(cosh(cd).real(), 8.3373002513114904887E-01);
	deq(cosh(cd).imag(), 9.8889770576286509626E-01);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (-1, piby4);
	ldeq(cosh(cld).real(), cos1);
	ldeq(cosh(cld).imag(), -sin1);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T81");
#else
	skip_chk("_2627T81");
#endif /* CASE_2627T81 */

/* _2627T91     exp  (const complex<T>& x) */
#if !defined(SKIP_2627T91)&&(!defined(ONLY)||defined(CASE_2627T91))
	begin_chk("_2627T91");
	{
	CPXfloat cf (1., 0.);
	feq(exp(cf).real(), (float)e);
	feq(exp(cf).imag(), 0.F);

	CPXdouble cd (1., 1.);
	deq(exp(cd).real(), 1.4686939399158851571E+00);
	deq(exp(cd).imag(), 2.2873552871788423911E+00);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (1., -piby4);
	ldeq(exp(cld).real(), e * rthalf);
	ldeq(exp(cld).imag(), -e * rthalf);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T91");
#else
	skip_chk("_2627T91");
#endif /* CASE_2627T91 */

/* _2627T_101   log  (const complex<T>& x) */
#if !defined(SKIP_2627T_101)&&(!defined(ONLY)||defined(CASE_2627T_101))
	begin_chk("_2627T_101");
	{
	CPXfloat cf (2., 0.);
	feq(log(cf).real(), 6.9314718055994530940E-01F);
	feq(log(cf).imag(), 0.F);

	CPXdouble cd (1., 1.);
	deq(log(cd).real(), 3.4657359027997265470E-01);
	deq(log(cd).imag(), 7.8539816339744830964E-01);

	#if IS_EMBEDDED
	TOUCH(ln2);
	#else /* IS_EMBEDDED */
	CPXldouble cld (1., -1.);
	ldeq(log(cld).real(), ln2 / 2);
	ldeq(log(cld).imag(), -piby4);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_101");
#else
	skip_chk("_2627T_101");
#endif /* CASE_2627T_101 */

/* _2627T_111   log10(const complex<T>& x) */
#if !defined(SKIP_2627T_111)&&(!defined(ONLY)||defined(CASE_2627T_111))
	begin_chk("_2627T_111");
	{
	CPXfloat cf (1., 0.);
	feq(log10(cf).real(), 0.F);
	feq(log10(cf).imag(), 0.F);

	CPXdouble cd (10., 0.);
	deq(log10(cd).real(), 1.);
	deq(log10(cd).imag(), 0.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (100., 0.);
	ldeq(log10(cld).real(), 2.);
	ldeq(log10(cld).imag(), 0.);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_111");
#else
	skip_chk("_2627T_111");
#endif /* CASE_2627T_111 */

/* _2627T_121   pow(const complex<T>& x, const complex<T>& y) */
#if !defined(SKIP_2627T_121)&&(!defined(ONLY)||defined(CASE_2627T_121))
	begin_chk("_2627T_121");
	{
	CPXfloat cf (1., 0.);
	CPXfloat cf2 (1., 0.);
	feq(pow(cf, cf2).real(), 1.F);
	feq(pow(cf, cf2).imag(), 0.F);

	CPXdouble cd (1., 1.);
	CPXdouble cd2 (1., 1.);
	deq(pow(cd, cd2).real(), 2.7395725383012107108E-01);
	deq(pow(cd, cd2).imag(), 5.8370075875861462750E-01);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (0., 1.);
	CPXldouble cld2 (1., 0.);
	ldeq(pow(cld, cld2).real(), 0.);
	ldeq(pow(cld, cld2).imag(), 1.);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_121");
#else
	skip_chk("_2627T_121");
#endif /* CASE_2627T_121 */

/* _2627T_131   pow  (const complex<T>& x, T y) */
#if !defined(SKIP_2627T_131)&&(!defined(ONLY)||defined(CASE_2627T_131))
	begin_chk("_2627T_131");
	{
	CPXfloat cf (1., 0.);
	float f2 = 0.;
	feq(pow(cf, f2).real(), 1.F);
	feq(pow(cf, f2).imag(), 0.F);

	CPXdouble cd (1., 1.);
	double d2 = 1.;
	deq(pow(cd, d2).real(), 1);
	deq(pow(cd, d2).imag(), 1);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (2., 2.);
	long double ld2 = 5.;
	ldeq(pow(cld, ld2).real(), -128.);
	ldeq(pow(cld, ld2).imag(), -128.);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_131");
#else
	skip_chk("_2627T_131");
#endif /* CASE_2627T_131 */

/* _2627T_141   pow  (T x, const complex<T>& y) */
#if !defined(SKIP_2627T_141)&&(!defined(ONLY)||defined(CASE_2627T_141))
	begin_chk("_2627T_141");
	{
	float f = 1.;
	CPXfloat cf2 (0., 1.);
	feq(pow(f, cf2).real(), 1.F);
	feq(pow(f, cf2).imag(), 0.F);

	double d = 0.;
	CPXdouble cd2 (1., 0.);
	deq(pow(d, cd2).real(), 0);
	deq(pow(d, cd2).imag(), 0);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	long double ld = e;
	CPXldouble cld2 (1., -piby4);
	ldeq(pow(ld, cld2).real(), e * rthalf);
	ldeq(pow(ld, cld2).imag(), -e * rthalf);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_141");
#else
	skip_chk("_2627T_141");
#endif /* CASE_2627T_141 */

/* _2627T_151   pow  (const complex<T>& x, int y) */
#if !defined(SKIP_2627T_151)&&(!defined(ONLY)||defined(CASE_2627T_151))
	begin_chk("_2627T_151");
	{
	CPXfloat cf (0., 1.);
	int i2a = 1;
	feq(pow(cf, i2a).real(), 0.F);
	feq(pow(cf, i2a).imag(), 1.F);

	CPXdouble cd (1., 0.);
	int i2b = 0;
	deq(pow(cd, i2b).real(), 1.);
	deq(pow(cd, i2b).imag(), 0.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (2., 2.);
	int i2c = 5;
	ldeq(pow(cld, i2c).real(), -128.);
	ldeq(pow(cld, i2c).imag(), -128.);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_151");
#else
	skip_chk("_2627T_151");
#endif /* CASE_2627T_151 */

/* _2627T_161   sin  (const complex<T>& x) */
#if !defined(SKIP_2627T_161)&&(!defined(ONLY)||defined(CASE_2627T_161))
	begin_chk("_2627T_161");
	{
	CPXfloat cf (1., 0.);
	feq(sin(cf).real(), 8.4147098480789650665E-01F);
	feq(sin(cf).imag(), 0.F);

	CPXdouble cd (1., 1.);
	deq(sin(cd).real(), 1.2984575814159772947E+00);
	deq(sin(cd).imag(), 6.3496391478473610820E-01);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (piby4, -1.);
	ldeq(sin(cld).real(), cos1);
	ldeq(sin(cld).imag(), -sin1);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_161");
#else
	skip_chk("_2627T_161");
#endif /* CASE_2627T_161 */

/* _2627T_171   sinh (const complex<T>& x) */
#if !defined(SKIP_2627T_171)&&(!defined(ONLY)||defined(CASE_2627T_171))
	begin_chk("_2627T_171");
	{
	CPXfloat cf (1., 0.);
	feq(sinh(cf).real(), 1.1752011936438014568E+00F);
	feq(sinh(cf).imag(), 0.F);

	CPXdouble cd (1., 1.);
	deq(sinh(cd).real(), 6.3496391478473610820E-01);
	deq(sinh(cd).imag(), 1.2984575814159772947E+00);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (-1., piby4);
	ldeq(sinh(cld).real(), -sin1);
	ldeq(sinh(cld).imag(), cos1);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_171");
#else
	skip_chk("_2627T_171");
#endif /* CASE_2627T_171 */

/* _2627T_181   sqrt (const complex<T>& x) */
#if !defined(SKIP_2627T_181)&&(!defined(ONLY)||defined(CASE_2627T_181))
	begin_chk("_2627T_181");
	{
	CPXfloat cf (1., 0.);
	feq(sqrt(cf).real(), 1.F);
	feq(sqrt(cf).imag(), 0.F);

	CPXdouble cd (1., 1.);
	deq(sqrt(cd).real(), 1.0986841134678099660E+00);
	deq(sqrt(cd).imag(), 4.5508986056222734126E-01);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (0., -1.);
	ldeq(sqrt(cld).real(), rthalf);
	ldeq(sqrt(cld).imag(), -rthalf);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2627T_181");
#else
	skip_chk("_2627T_181");
#endif /* CASE_2627T_181 */

/* _2627T_191   tan (const complex<T>& x) */
#if !defined(SKIP_2627T_191)&&(!defined(ONLY)||defined(CASE_2627T_191))
	begin_chk("_2627T_191");
	{
	CPXfloat cf ((float)piby4, 0.);
	feq(tan(cf).real(), 1.F);
	feq(tan(cf).imag(), 0.F);

	CPXdouble cd (piby4, 0.);
	deq(tan(cd).real(), 1.);
	deq(tan(cd).imag(), 0.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (piby4, 0.);
	ldeq(tan(cld).real(), 1.);
	ldeq(tan(cld).imag(), 0.);
	#endif /* IS_EMBEDDED */

	}
	end_chk("_2627T_191");
#else
	skip_chk("_2627T_191");
#endif /* CASE_2627T_191 */

/* _2627T_201   tanh (const complex<T>& x) */
#if !defined(SKIP_2627T_201)&&(!defined(ONLY)||defined(CASE_2627T_201))
	begin_chk("_2627T_201");
	{
	CPXfloat cf (0., 0.);
	feq(tanh(cf).real(), 0.F);
	feq(tanh(cf).imag(), 0.F);

	CPXdouble cd (0., 0.);
	deq(tanh(cd).real(), 0.);
	deq(tanh(cd).imag(), 0.);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	CPXldouble cld (0., 0.);
	ldeq(tanh(cld).real(), 0.);
	ldeq(tanh(cld).imag(), 0.);
	#endif /* IS_EMBEDDED */






	}
	end_chk("_2627T_201");
#else
	skip_chk("_2627T_201");
#endif /* CASE_2627T_201 */

/* _263T11  <valarray> synopsis */
#if !defined(SKIP_263T11)&&(!defined(ONLY)||defined(CASE_263T11))
	begin_chk("_263T11");
	{
	slice *p0 = 0; TOUCH(p0);
	gslice *p1 = 0; TOUCH(p1);
	valarray<Ty> *p2 = 0; TOUCH(p2);
	Ty *pv2 = (valarray<Ty>::value_type *)0; TOUCH(pv2);	// ADDED [MAR96]

	gslice_array<Ty> *p3 = 0; TOUCH(p3);
	Ty *pv3 = (gslice_array<Ty>::value_type *)0; TOUCH(pv3);	// ADDED [MAR96]

	indirect_array<Ty> *p4 = 0; TOUCH(p4);
	Ty *pv4 = (indirect_array<Ty>::value_type *)0; TOUCH(pv4);	// ADDED [MAR96]

	mask_array<Ty> *p5 = 0; TOUCH(p5);
	Ty *pv5 = (mask_array<Ty>::value_type *)0; TOUCH(pv5);	// ADDED [MAR96]

	slice_array<Ty> *p6 = 0; TOUCH(p6);
	Ty *pv6 = (slice_array<Ty>::value_type *)0; TOUCH(pv6);	// ADDED [MAR96]
	chk(1);




	}
	end_chk("_263T11");
#else
	skip_chk("_263T11");
#endif /* CASE_263T11 */

/* _2631T11  class valarray definition */
#if !defined(SKIP_2631T11)&&(!defined(ONLY)||defined(CASE_2631T11))
	begin_chk("_2631T11");
	{
	valarray<Ty> x, y(4, 10);
	ieq(y[9], 4);
	x = -y;
	ieq(x[9], -4);
	x /= y;
	ieq(x.sum(), -10);



	}
	end_chk("_2631T11");
#else
	skip_chk("_2631T11");
#endif /* CASE_2631T11 */

return leave_chk("t262");
}
/* V3.10:0009 */
