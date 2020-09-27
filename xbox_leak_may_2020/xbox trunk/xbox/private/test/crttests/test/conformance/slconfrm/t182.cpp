/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _1822T21 Table 26: <climits> synopsis */
#if !defined(SKIP_1822T21)&&(!defined(ONLY)||defined(CASE_1822T21))
#include <climits>
#endif /* CASE_1822T21 */

/* _1822T41 Table 27: <cfloat> synopsis */
#if !defined(SKIP_1822T41)&&(!defined(ONLY)||defined(CASE_1822T41))
#include <cfloat>
#endif /* CASE_1822T41 */

/* _183T21 Table 28: <cstdlib> synopsis */
#if !defined(SKIP_183T21)&&(!defined(ONLY)||defined(CASE_183T21))
#include <cstdlib>
#endif /* CASE_183T21 */

/* _183T48 exit(EXIT_FAILURE) reports unsuccessful termination */
#if !defined(SKIP_183T48)&&(!defined(ONLY)||defined(CASE_183T48))
#include <cstdlib>
#endif /* CASE_183T48 */

/* _184T31 <new> synopsis */
#if !defined(SKIP_184T31)&&(!defined(ONLY)||defined(CASE_184T31))
#include <cstdlib>
#if DISALLOW_USER_NAME_PROTECTION
 #include <new>
#else
	#define xalloc	"xalloc"	/* looking for trouble */
 #include <new>
	#undef xalloc
#endif
static void newh_184T31()
	{}
#endif /* CASE_184T31 */

/* _18411T11 new(size_t) effects */
#if !defined(SKIP_18411T11)&&(!defined(ONLY)||defined(CASE_18411T11))
#include <new>
#endif /* CASE_18411T11 */

/* _18411T44 new(size_t) returns pointer on success */
#if !defined(SKIP_18411T44)&&(!defined(ONLY)||defined(CASE_18411T44))
#include <new>
#endif /* CASE_18411T44 */

/* _18411T51 new(size_t, const nothrow&) returns null, doesn't throw */
#if !defined(SKIP_18411T51)&&(!defined(ONLY)||defined(CASE_18411T51))
#include <new>
#endif /* CASE_18411T51 */

/* _18411T91 delete(void *) effects */
#if !defined(SKIP_18411T91)&&(!defined(ONLY)||defined(CASE_18411T91))
#include <new>
#endif /* CASE_18411T91 */

/* _18411T_121 delete(void *) does nothing with null argument */
#if !defined(SKIP_18411T_121)&&(!defined(ONLY)||defined(CASE_18411T_121))
#include <new>
#endif /* CASE_18411T_121 */

/* _18411T_131 delete(void *) reclaims storage from earlier new call */
#if !defined(SKIP_18411T_131)&&(!defined(ONLY)||defined(CASE_18411T_131))
#include <new>
#endif /* CASE_18411T_131 */

/* _18412T11 new[](size_t) called by array new expression */
#if !defined(SKIP_18412T11)&&(!defined(ONLY)||defined(CASE_18412T11))
#include <new>
#endif /* CASE_18412T11 */

/* _18412T61 new[](size_t, const nothrow&) effects */
#if !defined(SKIP_18412T61)&&(!defined(ONLY)||defined(CASE_18412T61))
#include <new>
#endif /* CASE_18412T61 */

/* _18412T_111 delete[](void *) effects */
#if !defined(SKIP_18412T_111)&&(!defined(ONLY)||defined(CASE_18412T_111))
#include <new>
#endif /* CASE_18412T_111 */

/* _18412T_141 delete[](void *) does nothing with null argument */
#if !defined(SKIP_18412T_141)&&(!defined(ONLY)||defined(CASE_18412T_141))
#include <new>
#endif /* CASE_18412T_141 */

/* _18412T_152 delete[](void *) reclaims storage from new[] */
#if !defined(SKIP_18412T_152)&&(!defined(ONLY)||defined(CASE_18412T_152))
#include <new>
#endif /* CASE_18412T_152 */

/* _18413T13 new(size_t, void *) returns */
#if !defined(SKIP_18413T13)&&(!defined(ONLY)||defined(CASE_18413T13))
#include <new>
#endif /* CASE_18413T13 */

/* _18413T51 new[](size_t, void *) returns */
#if !defined(SKIP_18413T51)&&(!defined(ONLY)||defined(CASE_18413T51))
#include <new>
#endif /* CASE_18413T51 */

/* _18413T81 delete(void *, void *) effects */
#if !defined(SKIP_18413T81)&&(!defined(ONLY)||defined(CASE_18413T81))
#include <new>
#endif /* CASE_18413T81 */

/* _18413T_111 delete[](void *, void *) effects */
#if !defined(SKIP_18413T_111)&&(!defined(ONLY)||defined(CASE_18413T_111))
#include <new>
#endif /* CASE_18413T_111 */

int t182_main(int, char *[])
{
enter_chk("t182.cpp");
/* _1822T21 Table 26: <climits> synopsis */
#if !defined(SKIP_1822T21)&&(!defined(ONLY)||defined(CASE_1822T21))
	begin_chk("_1822T21");
	{
	static int char_bit = CHAR_BIT;
	static char char_max = CHAR_MAX;
	static char char_min = CHAR_MIN;
	static int int_max = INT_MAX;
	static int int_min = INT_MIN;
	static long long_max = LONG_MAX;
	static long long_min = LONG_MIN;
	static int mb_len_max = MB_LEN_MAX;
	static signed char schar_max = SCHAR_MAX;
	static signed char schar_min = SCHAR_MIN;
	static short shrt_max = SHRT_MAX;
	static short shrt_min = SHRT_MIN;
	static unsigned char uchar_max = UCHAR_MAX;
	static unsigned int uint_max = UINT_MAX;
	static unsigned long ulong_max = ULONG_MAX;
	static unsigned short ushrt_max = USHRT_MAX;
	ieq(char_bit, CHAR_BIT);
	chk(char_max == CHAR_MAX);
	chk(char_min == CHAR_MIN);
	ieq(int_max, INT_MAX);
	ieq(int_min, INT_MIN);
	leq(long_max, LONG_MAX);
	leq(long_min, LONG_MIN);
	ieq(mb_len_max, MB_LEN_MAX);
	ieq(schar_max, SCHAR_MAX);
	ieq(schar_min, SCHAR_MIN);
	ieq(shrt_max, SHRT_MAX);
	ieq(shrt_min, SHRT_MIN);
	chk(uchar_max == UCHAR_MAX);
	chk(uint_max == UINT_MAX);
	chk(ulong_max == ULONG_MAX);
	chk(ushrt_max == USHRT_MAX);


	}
	end_chk("_1822T21");
#else
	skip_chk("_1822T21");
#endif /* CASE_1822T21 */

/* _1822T41 Table 27: <cfloat> synopsis */
#if !defined(SKIP_1822T41)&&(!defined(ONLY)||defined(CASE_1822T41))
	begin_chk("_1822T41");
	{
	int flt_radix = FLT_RADIX;
	ieq(flt_radix, FLT_RADIX);
	//	double properties
	int dbl_dig = DBL_DIG;
	double dbl_epsilon = DBL_EPSILON;
	int dbl_mant_dig = DBL_MANT_DIG;
	double dbl_max = DBL_MAX;
	int dbl_max_10_exp = DBL_MAX_10_EXP;
	int dbl_max_exp = DBL_MAX_EXP;
	double dbl_min = DBL_MIN;
	int dbl_min_10_exp = DBL_MIN_10_EXP;
	int dbl_min_exp = DBL_MIN_EXP;
	ieq(dbl_dig, DBL_DIG);
	chk(dbl_epsilon == DBL_EPSILON);
	ieq(dbl_mant_dig, DBL_MANT_DIG);
	chk(dbl_max == DBL_MAX);
	ieq(dbl_max_10_exp, DBL_MAX_10_EXP);
	ieq(dbl_max_exp, DBL_MAX_EXP);
	chk(dbl_min == DBL_MIN);
	ieq(dbl_min_10_exp, DBL_MIN_10_EXP);
	ieq(dbl_min_exp, DBL_MIN_EXP);
	// float properties
	int flt_dig = FLT_DIG;
	float flt_epsilon = FLT_EPSILON;
	int flt_mant_dig = FLT_MANT_DIG;
	float flt_max = FLT_MAX;
	int flt_max_10_exp = FLT_MAX_10_EXP;
	int flt_max_exp = FLT_MAX_EXP;
	float flt_min = FLT_MIN;
	int flt_min_10_exp = FLT_MIN_10_EXP;
	int flt_min_exp = FLT_MIN_EXP;
	ieq(flt_dig, FLT_DIG);
	chk(flt_epsilon == FLT_EPSILON);
	ieq(flt_mant_dig, FLT_MANT_DIG);
	chk(flt_max == FLT_MAX);
	ieq(flt_max_10_exp, FLT_MAX_10_EXP);
	ieq(flt_max_exp, FLT_MAX_EXP);
	chk(flt_min == FLT_MIN);
	ieq(flt_min_10_exp, FLT_MIN_10_EXP);
	ieq(flt_min_exp, FLT_MIN_EXP);
	// long double properties
	int ldbl_dig = LDBL_DIG;
	long double ldbl_epsilon = LDBL_EPSILON;
	int ldbl_mant_dig = LDBL_MANT_DIG;
	long double ldbl_max = LDBL_MAX;
	int ldbl_max_10_exp = LDBL_MAX_10_EXP;
	int ldbl_max_exp = LDBL_MAX_EXP;
	long double ldbl_min = LDBL_MIN;
	int ldbl_min_10_exp = LDBL_MIN_10_EXP;
	int ldbl_min_exp = LDBL_MIN_EXP;
	ieq(ldbl_dig, LDBL_DIG);
	chk(ldbl_epsilon == LDBL_EPSILON);
	ieq(ldbl_mant_dig, LDBL_MANT_DIG);
	chk(ldbl_max == LDBL_MAX);
	ieq(ldbl_max_10_exp, LDBL_MAX_10_EXP);
	ieq(ldbl_max_exp, LDBL_MAX_EXP);
	chk(ldbl_min == LDBL_MIN);
	ieq(ldbl_min_10_exp, LDBL_MIN_10_EXP);
	ieq(ldbl_min_exp, LDBL_MIN_EXP);


	}
	end_chk("_1822T41");
#else
	skip_chk("_1822T41");
#endif /* CASE_1822T41 */

/* _183T21 Table 28: <cstdlib> synopsis */
#if !defined(SKIP_183T21)&&(!defined(ONLY)||defined(CASE_183T21))
	begin_chk("_183T21");
	{
	int zero = 0;
	if (zero)
		{	// never called
		abort();
		atexit(&abort);
		exit(0); }
	int fail = EXIT_FAILURE;
	int suc = EXIT_SUCCESS;
	ieq(fail, EXIT_FAILURE);
	ieq(suc, EXIT_SUCCESS);


	}
	end_chk("_183T21");
#else
	skip_chk("_183T21");
#endif /* CASE_183T21 */

/* _183T48 exit(EXIT_FAILURE) reports unsuccessful termination */
#if !defined(SKIP_183T48)&&(!defined(ONLY)||defined(CASE_183T48))
	begin_chk("_183T48");
	{
	chk(EXIT_SUCCESS != EXIT_FAILURE);


	}
	end_chk("_183T48");
#else
	skip_chk("_183T48");
#endif /* CASE_183T48 */

/* _184T31 <new> synopsis */
#if !defined(SKIP_184T31)&&(!defined(ONLY)||defined(CASE_184T31))
	begin_chk("_184T31");
	{
	STD nothrow_t nt;
	void *pv = operator new(sizeof (float));
	operator delete(pv);
	pv = operator new(sizeof (float), nt);
	operator delete(pv);
	#if !DISALLOW_ARRAY_NEW_DELETE
		pv = operator new[](10 * sizeof (float));
		operator delete[](pv);
		pv = operator new[](10 * sizeof (float), nt);
		operator delete[](pv);
		float fl;
		pv = operator new(sizeof (float), &fl);
		pv = operator new[](sizeof (float), &fl);
	#endif
	#if !DISALLOW_PLACEMENT_DELETE
		operator delete(pv, pv);
		operator delete[](pv, pv);
		pv = operator new(sizeof (float), nt);	// added [NOV95]
		operator delete(pv, nt);
		pv = operator new[](10 * sizeof (float), nt);
		operator delete(pv, nt);
	#endif
	STD new_handler ph = STD set_new_handler(&newh_184T31);
	STD set_new_handler(ph);
	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	STD bad_alloc ex;
	#endif /* IS_EMBEDDED */
	chk(1);


	}
	end_chk("_184T31");
#else
	skip_chk("_184T31");
#endif /* CASE_184T31 */

/* _18411T11 new(size_t) effects */
#if !defined(SKIP_18411T11)&&(!defined(ONLY)||defined(CASE_18411T11))
	begin_chk("_18411T11");
	{
	void *pv = operator new(sizeof (long double));
	chk(pv != 0);
	if (pv != 0)
		{long double *pld = (long double *)pv;
		*pld = 21.5L;
		ldeq(*pld, 21.5L); }
	operator delete(pv);


	}
	end_chk("_18411T11");
#else
	skip_chk("_18411T11");
#endif /* CASE_18411T11 */

/* _18411T44 new(size_t) returns pointer on success */
#if !defined(SKIP_18411T44)&&(!defined(ONLY)||defined(CASE_18411T44))
	begin_chk("_18411T44");
	{
	int *p = (int *)operator new(sizeof (int));
	chk(p != 0);
	if (p != 0)
		{*p = -27;
		ieq(*p, -27); }
	operator delete(p);


	}
	end_chk("_18411T44");
#else
	skip_chk("_18411T44");
#endif /* CASE_18411T44 */

/* _18411T51 new(size_t, const nothrow&) returns null, doesn't throw */
#if !defined(SKIP_18411T51)&&(!defined(ONLY)||defined(CASE_18411T51))
	begin_chk("_18411T51");
	{
	STD nothrow_t nt;
	void *pv = operator new(sizeof (long double), nt);
	chk(pv != 0);
	if (pv != 0)
		{long double *pld = (long double *)pv;
		*pld = 21.5L;
		ldeq(*pld, 21.5L); }
	operator delete(pv);


	}
	end_chk("_18411T51");
#else
	skip_chk("_18411T51");
#endif /* CASE_18411T51 */

/* _18411T91 delete(void *) effects */
#if !defined(SKIP_18411T91)&&(!defined(ONLY)||defined(CASE_18411T91))
	begin_chk("_18411T91");
	{
	operator delete(0);
	chk(1);


	}
	end_chk("_18411T91");
#else
	skip_chk("_18411T91");
#endif /* CASE_18411T91 */

/* _18411T_121 delete(void *) does nothing with null argument */
#if !defined(SKIP_18411T_121)&&(!defined(ONLY)||defined(CASE_18411T_121))
	begin_chk("_18411T_121");
	{
	operator delete(0);
	chk(1);


	}
	end_chk("_18411T_121");
#else
	skip_chk("_18411T_121");
#endif /* CASE_18411T_121 */

/* _18411T_131 delete(void *) reclaims storage from earlier new call */
#if !defined(SKIP_18411T_131)&&(!defined(ONLY)||defined(CASE_18411T_131))
	begin_chk("_18411T_131");
	{
	int *p = (int *)operator new(sizeof (int));
	operator delete(p);
	chk(1);


	}
	end_chk("_18411T_131");
#else
	skip_chk("_18411T_131");
#endif /* CASE_18411T_131 */

/* _18412T11 new[](size_t) called by array new expression */
#if !defined(SKIP_18412T11)&&(!defined(ONLY)||defined(CASE_18412T11))
	begin_chk("_18412T11");
	{
	#if !DISALLOW_ARRAY_NEW_DELETE
		void *pv = operator new[](sizeof (long double));
		chk(pv != 0);
		if (pv != 0)
			{long double *pld = (long double *)pv;
			*pld = 21.5L;
			ldeq(*pld, 21.5L); }
		operator delete[](pv);
	#else
		chk(1);
	#endif


	}
	end_chk("_18412T11");
#else
	skip_chk("_18412T11");
#endif /* CASE_18412T11 */

/* _18412T61 new[](size_t, const nothrow&) effects */
#if !defined(SKIP_18412T61)&&(!defined(ONLY)||defined(CASE_18412T61))
	begin_chk("_18412T61");
	{
	STD nothrow_t nt; TOUCH(nt);
	#if !DISALLOW_ARRAY_NEW_DELETE
		void *pv = operator new[](sizeof (long double), nt);
		chk(pv != 0);
		if (pv != 0)
			{long double *pld = (long double *)pv;
			*pld = 21.5L;
			ldeq(*pld, 21.5L); }
		operator delete[](pv);
	#else
		chk(1);
	#endif


	}
	end_chk("_18412T61");
#else
	skip_chk("_18412T61");
#endif /* CASE_18412T61 */

/* _18412T_111 delete[](void *) effects */
#if !defined(SKIP_18412T_111)&&(!defined(ONLY)||defined(CASE_18412T_111))
	begin_chk("_18412T_111");
	{
	#if !DISALLOW_ARRAY_NEW_DELETE
		operator delete[](0);
	#endif
	chk(1);


	}
	end_chk("_18412T_111");
#else
	skip_chk("_18412T_111");
#endif /* CASE_18412T_111 */

/* _18412T_141 delete[](void *) does nothing with null argument */
#if !defined(SKIP_18412T_141)&&(!defined(ONLY)||defined(CASE_18412T_141))
	begin_chk("_18412T_141");
	{
	#if !DISALLOW_ARRAY_NEW_DELETE
		operator delete[](0);
	#endif
	chk(1);


	}
	end_chk("_18412T_141");
#else
	skip_chk("_18412T_141");
#endif /* CASE_18412T_141 */

/* _18412T_152 delete[](void *) reclaims storage from new[] */
#if !defined(SKIP_18412T_152)&&(!defined(ONLY)||defined(CASE_18412T_152))
	begin_chk("_18412T_152");
	{
	#if !DISALLOW_ARRAY_NEW_DELETE
		int *p = (int *)operator new[](sizeof (int));
		operator delete[](p);
	#endif
	chk(1);


	}
	end_chk("_18412T_152");
#else
	skip_chk("_18412T_152");
#endif /* CASE_18412T_152 */

/* _18413T13 new(size_t, void *) returns */
#if !defined(SKIP_18413T13)&&(!defined(ONLY)||defined(CASE_18413T13))
	begin_chk("_18413T13");
	{
	int i;
	chk(operator new(0, &i) == (void *)&i);
	chk(operator new(10000, &i) == (void *)&i);


	}
	end_chk("_18413T13");
#else
	skip_chk("_18413T13");
#endif /* CASE_18413T13 */

/* _18413T51 new[](size_t, void *) returns */
#if !defined(SKIP_18413T51)&&(!defined(ONLY)||defined(CASE_18413T51))
	begin_chk("_18413T51");
	{
	int i; TOUCH(i);
	#if !DISALLOW_ARRAY_NEW_DELETE
		chk(operator new[](0, &i) == (void *)&i);
		chk(operator new[](10000, &i) == (void *)&i);
	#else
		chk(1);
	#endif


	}
	end_chk("_18413T51");
#else
	skip_chk("_18413T51");
#endif /* CASE_18413T51 */

/* _18413T81 delete(void *, void *) effects */
#if !defined(SKIP_18413T81)&&(!defined(ONLY)||defined(CASE_18413T81))
	begin_chk("_18413T81");
	{
	#if !DISALLOW_PLACEMENT_DELETE
		int i;
		operator delete(&i, &i);
	#endif
	chk(1);


	}
	end_chk("_18413T81");
#else
	skip_chk("_18413T81");
#endif /* CASE_18413T81 */

/* _18413T_111 delete[](void *, void *) effects */
#if !defined(SKIP_18413T_111)&&(!defined(ONLY)||defined(CASE_18413T_111))
	begin_chk("_18413T_111");
	{
	#if !DISALLOW_PLACEMENT_DELETE
		int i;
		operator delete[](&i, &i);
	#endif
	chk(1);

	}
	end_chk("_18413T_111");
#else
	skip_chk("_18413T_111");
#endif /* CASE_18413T_111 */

return leave_chk("t182");
}
/* V3.10:0009 */
