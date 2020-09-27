/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as	agreed in writing with
 *  Dinkumware, Ltd.
 */

/* defs.h -- define support for tests
 */
#ifndef DEFS_H
 #define DEFS_H
#include <float.h>
#include <stdio.h>
#ifdef _XBOX
extern "C" int DbgPrint(const char *, ...);
#endif
#include "flags.h"

	// macros and types used throughout tests
typedef char Char;	// should also permit [un]signed char too
typedef unsigned char Mychar;	// signed char or unsigned char
typedef int Ty;	// should be an integer type

#if IS_EMBEDDED
 #define CPXfloat	float_complex
 #define CPXdouble	double_complex
#else /* IS_EMBEDDED */
 #define CPXfloat	complex<float>
 #define CPXdouble	complex<double>
 #define CPXldouble	complex<long double>

struct Myfloat
	{	// stressful float for template class complex
	Myfloat(double x = 0)
		: val(x)
		{}	// construct from x

	operator double() const
		{	// return stored value
		return (val); }

private:
	double val;	// the stored value
	};
#endif /* IS_EMBEDDED */

static const long double e      = 2.7182818284590452353602874713526625L;
static const long double ln2    = 0.69314718055994530941723212145817657L;
static const long double piby4  = 0.78539816339744830961566084581987572L;
static const long double rthalf = 0.70710678118654752440084436210484904L;
static const long double cos1   = rthalf * (e + 1 / e) / 2;
static const long double sin1   = rthalf * (e - 1 / e) / 2;

#define CLINK	/* nonstandard C linkage, otherwise empty */

#ifdef _LONGLONG	/* common extension */
 #define Longlong	_LONGLONG
 #ifdef _ULONGLONG
  #define ULonglong	_ULONGLONG
 #else
  #define ULonglong	unsigned _LONGLONG
 #endif
#endif

	// macros used by bitset
#define NITSY	1	/* NITSY == 0 for maximum stress, otherwise 1 */
#define NTINY	6	/* NTINY < # of bits in char */
#define NBIG	131	/* NBIG > # of bits in unsigned long */

	// macros used by iostreams classes
#if IS_EMBEDDED
 #define Al		STD char_allocator
 #define Bi		STD ios
 #define Bfs	STD filebuf
 #define Bs		STD streambuf
 #define Bss	STD stringbuf
 #define Cstr	STD string
 #define Ifs	STD ifstream
 #define Is		STD istream
 #define Isit	STD istreambuf_iterator<char, It>
 #define Iss	STD istringstream
 #define It		STD char_traits
 #define Ofs	STD ofstream
 #define Os		STD ostream
 #define Osit	STD ostreambuf_iterator<char, It>
 #define Oss	STD ostringstream
 #define Sb		STD stringbuf
 #define Ustr	STD string
#else /* IS_EMBEDDED */
 #define Al 	STD allocator<Char>
 #define Bi		STD basic_ios<Char, It >
 #define Bfs	STD basic_filebuf<Char, It >
 #define Bs		STD basic_streambuf<Char, It >
 #define Bss	STD basic_stringbuf<Char, It >
 #define Cstr	STD basic_string<Char, It, Al >
 #define Ifs	STD basic_ifstream<Char, It >
 #define Is		STD basic_istream<Char, It >
 #define Isit	STD istreambuf_iterator<Char, It >
 #define Iss	STD basic_istringstream<Char, It >
 #define It		STD char_traits<Char>
 #define Ofs	STD basic_ofstream<Char, It >
 #define Os		STD basic_ostream<Char, It >
 #define Osit	STD ostreambuf_iterator<Char, It >
 #define Oss	STD basic_ostringstream<Char, It >
 #define Sb		STD basic_stringbuf<Char, It, Al >
 #define Ustr	STD basic_string<Char, It, Al >
#endif /* IS_EMBEDDED */

#if IS_EMBEDDED
 #define CHAR_TRAITS			char_traits
 #define STR_ALLOCATOR(T)		char_allocator
#else /* IS_EMBEDDED */
 #define CHAR_TRAITS			char_traits<char>
 #define STR_ALLOCATOR(T)		ALLOCATOR(T)
#endif /* IS_EMBEDDED */

 #define ALLOCATOR(T)			STD allocator<T>	/* CHANGED [MAR96] */
 #define ADDRESS(T, al, x)		al.address(x)
 #define ALLOCATE(T, al, n)		al.allocate(n, (void *)0)
 #define CONSTRUCT(T, al, p, v)	al.construct(p, v)
 #define DEALLOCATE(T, al, p)	al.deallocate(p)
 #define DESTROY(T, al, p)		al.destroy(p)
 #define MAX_SIZE(T, al)		al.max_size()

 #define PTR_TYPE(T, A)			A
 #define SIZ_TYPE(T, A)			A

#if !DISALLOW_TOLERANT_TEMPLATES
 #define ADDFAC(loc, pfac)		locale(loc, pfac)
 #define HAS(loc, fac)	has_facet<fac>(loc)
 #define USE(loc, fac)	use_facet<fac>(loc)
#else
// tailor as needed for actual locale implementation
 #define ADDFAC(loc, pfac)	_Addfac(loc, pfac)
 #define HAS(loc, fac)	has_facet(loc, (fac *)0)
 #define USE(loc, fac)	use_facet(loc, (fac *)0)
#endif

#if DISALLOW_NAMESPACES
 #define STD	/* should be :: */
 #define CSTD	/* should be :: */
#else
 #define STD	/* should be std:: */
 #define CSTD	/* should be std:: or at least :: */
namespace std {}
using namespace std;	// temporary, until more detailed tests
#endif

#if DISALLOW_BOOLEANS
// tailor as needed for any names not defined in library
// #define true 1
// #define false 0
// #define bool int
 #define Bool	_Bool
#else
 #define Bool	bool
#endif

#if DISALLOW_BOOLEANS || DISALLOW_PARTIAL_SPECIALIZATION
// tailor as needed for actual vector<bool> implementation
// (assumed to look like vector<Bool, Bool_allocator>)
#define Bool_allocator	_Bool_allocator
#endif

#if DISALLOW_ACTUAL_DESTRUCTOR_NAME
 #define DESTRUCTOR(actual, alias)	~alias()
#else
 #define DESTRUCTOR(actual, alias)	~actual()
#endif

#ifdef THROW
// THROW and THROW0 are predefined in flags.h
#elif DISALLOW_EXCEPTIONS
 #define THROW(x)
 #define THROW0()
#else
 #define THROW(x)	throw (x)
 #define THROW0()	throw ()
#endif

	// MACROS USED IN TEST CODE
#define errmes(mesg)	fail_report_chk(__LINE__, mesg)
#define chk(x)			beq_chk(__LINE__, x, #x)
#define ileq(x, y)		ile_chk(__LINE__, x, y)
#define ieq(x, y)		ieq_chk(__LINE__, x, y)
#define leq(x, y)		leq_chk(__LINE__, x, y)
#define ueq(x, y)		ueq_chk(__LINE__, x, y)
#define feq(x, y)		feq_chk(__LINE__, x, y)
#define deq(x, y)		deq_chk(__LINE__, x, y)
#define ldeq(x, y)		eeq_chk(__LINE__, x, y)
#define steq(x, y)		seq_chk(__LINE__, x, y)
#define aeq(x, y)		aeq_chk(__LINE__, x, y)

#define FLT1_CHK(x, y)	((x) < 0 ? -(x) <= (y) : (x) <= (y))
#define FLT_CHK(x, y, eps)	((x) == 0 ? FLT1_CHK(y, eps) \
	: (y) == 0 ? FLT1_CHK(x, eps) \
	: FLT1_CHK(((x) - (y)) / (x), eps))

#define TOUCH(x)	_touch((void *)&(x))

	// STATIC STORAGE
const float feps_chk = FLT_EPSILON * 2;
const double deps_chk = DBL_EPSILON * 4;
const long double eeps_chk = LDBL_EPSILON * 6;

static char buf_chk[64] = "";
static const char *fname_chk = "";
static const char *iname_chk = "";

static bool reached_chk = false;

static int fail_chk = 0;
static int succ_chk = 0;
static int total_cfail_chk = 0;
static int total_csucc_chk = 0;
static int total_fail_chk = 0;
static int total_cskip_chk = 0;
static int total_succ_chk = 0;

	// FUNCTIONS
inline void _touch(void *)
	{}	// to quiet warnings about unused declarations

static void enter_chk(const char *fname)
	{	// enter test arena
	fname_chk = fname; }

static bool leave_chk(const char *tname)
	{	// leave test arena
	if (0 < total_fail_chk)
		DbgPrint("***** %d erroneous items in %s *****\n",
			total_fail_chk, tname);
	if (0 < total_succ_chk)
		DbgPrint("***** %d successful items in %s *****\n",
			total_succ_chk, tname);
	if (0 < total_cskip_chk)
		DbgPrint("***** %d skipped test cases in %s *****\n",
			total_cskip_chk, tname);
	DbgPrint("***** %d erroneous test cases in %s *****\n",
			total_cfail_chk, tname);
	DbgPrint("***** %d successful test cases in %s *****\n",
		total_csucc_chk, tname);
	return (total_fail_chk != 0); }

static void begin_chk(const char *iname)
	{	// enter a test
	iname_chk = iname;
	fail_chk = 0;
	succ_chk = 0; }

static void end_chk(const char *iname)
	{	// end a test
	if (fail_chk == 0)
		++total_csucc_chk;
	else
		++total_cfail_chk;
	total_fail_chk += fail_chk;
	total_succ_chk += succ_chk;
	DbgPrint("#%s: %s\n", 0 < fail_chk ? "FAILED" : "PASSED",
		iname); }

static void skip_chk(const char *sname)
	{	// skip a test
	++total_cskip_chk;
	DbgPrint("#SKIPPED: %s\n", sname); }

static void first_chk()
	{	// first-time check
	if (!reached_chk)
		{	// print entry message
		DbgPrint("***** Reached first test *****\n");
		reached_chk = true; }}

static bool succ_report_chk()
	{	// record success
	first_chk();
	++succ_chk;
	return (true); }

static bool fail_report_chk(int line, const char *mesg)
	{	// record failure
	first_chk();
	++fail_chk;
	DbgPrint("error in %s at line %d%s%s\n", fname_chk,
		line, mesg[0] == '\0' ? "" : ": ", mesg);
	return (false); }

static bool beq_chk(int line, bool x, const char *mesg)
	{	// boolean check
	if (x)
		return (succ_report_chk());
	else
		return (fail_report_chk(line, mesg)); }

static bool ile_chk(int line, int x, int y)
	{	// int x <= y check
	if (x <= y)
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "%d > %d", x, y);
		return (fail_report_chk(line, buf_chk)); }}

static bool ieq_chk(int line, int x, int y)
	{	// int x == y check
	if (x == y)
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "%d != %d", x, y);
		return (fail_report_chk(line, buf_chk)); }}

static bool leq_chk(int line, long x, long y)
	{	// long x == y check
	if (x == y)
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "%ld != %ld", x, y);
		return (fail_report_chk(line, buf_chk)); }}

static bool ueq_chk(int line, unsigned long x, unsigned long y)
	{	// unsigned long x == y check
	if (x == y)
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "%ld != %ld", x, y);
		return (fail_report_chk(line, buf_chk)); }}

static bool feq_chk(int line, float x, float y)
	{	// float x == y check
	if (FLT_CHK(x, y, feps_chk))
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "%.6g != %.6g", x, y);
		return (fail_report_chk(line, buf_chk)); }}

static bool deq_chk(int line, double x, double y)
	{	// double x == y check
	if (FLT_CHK(x, y, deps_chk))
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "%.10g != %.10g", x, y);
		return (fail_report_chk(line, buf_chk)); }}

static bool eeq_chk(int line, long double x, long double y)
	{	// long double x == y check
	if (FLT_CHK(x, y, eeps_chk))
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "%.10Lg != %.10Lg", x, y);
		return (fail_report_chk(line, buf_chk)); }}

static bool seq_chk(int line, const char *x, const char *y)
	{	// string x == y check
	int i;
	for (i = 0; x[i] != '\0' && x[i] == y[i]; ++i)
		;
	if (x[i] == y[i])
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "\"%s\" != \"%s\"", x, y);
		return (fail_report_chk(line, buf_chk)); }}

static bool aeq_chk(int line, const void *x, const void *y)
	{	// address x == y check
	if (x == y)
		return (succ_report_chk());
	else
		{	// print error message
		sprintf(buf_chk, "%p != %p", x, y);
		return (fail_report_chk(line, buf_chk)); }}

inline int ceil_log2(int n)
	{	// compute log2(n) rounded up
	if (n < 2)
		return (0);
	else
		return (1 + ceil_log2((n + 1) / 2)); }

#endif  /* of DEFS_H */

/*
991124 pjp: revised for 3.0
000328 pjp: changed ERROR to error for grep
 */
