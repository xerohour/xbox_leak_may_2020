/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _2763T41 resetiosflags returns */
#if !defined(SKIP_2763T41)&&(!defined(ONLY)||defined(CASE_2763T41))
#include <iomanip>
#include <istream>
#include <ostream>
#endif /* CASE_2763T41 */

/* _2763T61 setiosflags returns */
#if !defined(SKIP_2763T61)&&(!defined(ONLY)||defined(CASE_2763T61))
#include <iomanip>
#include <istream>
#include <ostream>
#endif /* CASE_2763T61 */

/* _2763T81 setbase returns */
#if !defined(SKIP_2763T81)&&(!defined(ONLY)||defined(CASE_2763T81))
#include <iomanip>
#include <istream>
#include <ostream>
#endif /* CASE_2763T81 */

/* _2763T_101 setfill returns */
#if !defined(SKIP_2763T_101)&&(!defined(ONLY)||defined(CASE_2763T_101))
#include <climits>
#include <iomanip>
#include <sstream>
#if IS_EMBEDDED
#else /* IS_EMBEDDED */
#include <cwchar>
#endif /* IS_EMBEDDED */
#endif /* CASE_2763T_101 */

/* _2763T_121 setprecision returns */
#if !defined(SKIP_2763T_121)&&(!defined(ONLY)||defined(CASE_2763T_121))
#include <iomanip>
#include <istream>
#include <ostream>
#endif /* CASE_2763T_121 */

/* _2763T_141 setw returns */
#if !defined(SKIP_2763T_141)&&(!defined(ONLY)||defined(CASE_2763T_141))
#include <iomanip>
#include <istream>
#include <ostream>
#endif /* CASE_2763T_141 */

int t27e_main(int, char *[])
{
enter_chk("t27e.cpp");
/* _2763T41 resetiosflags returns */
#if !defined(SKIP_2763T41)&&(!defined(ONLY)||defined(CASE_2763T41))
	begin_chk("_2763T41");
	{
	STD ios_base::fmtflags allf =
		STD ios_base::skipws | STD ios_base::unitbuf
		| STD ios_base::uppercase | STD ios_base::showbase
		| STD ios_base::showpoint | STD ios_base::showpos
		| STD ios_base::adjustfield | STD ios_base::basefield
		| STD ios_base::floatfield;
	Is istr(0);
	istr.setf(allf);
	chk(&(istr >> resetiosflags(STD ios_base::uppercase)) == &istr);
	chk(istr.flags() == (allf & ~STD ios_base::uppercase));
	istr >> resetiosflags(STD ios_base::hex);
	chk(istr.flags() == (allf &
		~(STD ios_base::uppercase | STD ios_base::hex)));
	istr >> resetiosflags(allf);
	chk(istr.flags() == (STD ios_base::fmtflags)0);
	Os ostr(0);
	ostr.setf(allf);
	chk(&(ostr << resetiosflags(STD ios_base::uppercase)) == &ostr);
	chk(ostr.flags() == (allf & ~STD ios_base::uppercase));
	ostr << resetiosflags(STD ios_base::hex);
	chk(ostr.flags() == (allf &
		~(STD ios_base::uppercase | STD ios_base::hex)));
	ostr << resetiosflags(allf);
	chk(ostr.flags() == (STD ios_base::fmtflags)0);


	}
	end_chk("_2763T41");
#else
	skip_chk("_2763T41");
#endif /* CASE_2763T41 */

/* _2763T61 setiosflags returns */
#if !defined(SKIP_2763T61)&&(!defined(ONLY)||defined(CASE_2763T61))
	begin_chk("_2763T61");
	{
	STD ios_base::fmtflags allf =
		STD ios_base::skipws | STD ios_base::unitbuf
		| STD ios_base::uppercase | STD ios_base::showbase
		| STD ios_base::showpoint | STD ios_base::showpos
		| STD ios_base::adjustfield | STD ios_base::basefield
		| STD ios_base::floatfield;
	Is istr(0);
	istr.unsetf(allf);
	chk(&(istr >> setiosflags(STD ios_base::uppercase)) == &istr);
	chk(istr.flags() == STD ios_base::uppercase);
	istr >> setiosflags(STD ios_base::hex);
	chk(istr.flags() == (STD ios_base::uppercase | STD ios_base::hex));
	istr >> setiosflags(allf);
	chk(istr.flags() == allf);
	Os ostr(0);
	ostr.unsetf(allf);
	chk(&(ostr << setiosflags(STD ios_base::uppercase)) == &ostr);
	chk(ostr.flags() == STD ios_base::uppercase);
	ostr << setiosflags(STD ios_base::hex);
	chk(ostr.flags() == (STD ios_base::uppercase | STD ios_base::hex));
	ostr << setiosflags(allf);
	chk(ostr.flags() == allf);


	}
	end_chk("_2763T61");
#else
	skip_chk("_2763T61");
#endif /* CASE_2763T61 */

/* _2763T81 setbase returns */
#if !defined(SKIP_2763T81)&&(!defined(ONLY)||defined(CASE_2763T81))
	begin_chk("_2763T81");
	{
	STD ios_base::fmtflags allf =
		STD ios_base::skipws | STD ios_base::unitbuf
		| STD ios_base::uppercase | STD ios_base::showbase
		| STD ios_base::showpoint | STD ios_base::showpos
		| STD ios_base::adjustfield | STD ios_base::basefield
		| STD ios_base::floatfield;
	Is istr(0);
	istr >> setbase(8);
	chk((istr.flags() & STD ios_base::basefield) == STD ios_base::oct);
	istr >> setbase(10);
	chk((istr.flags() & STD ios_base::basefield) == STD ios_base::dec);
	istr >> setbase(16);
	chk((istr.flags() & STD ios_base::basefield) == STD ios_base::hex);
	istr >> setbase(0);
	chk((istr.flags() & STD ios_base::basefield) == (STD ios_base::fmtflags)0);
	istr.setf(allf);
	istr >> setbase(23);
	chk(istr.flags() == (allf & ~STD ios_base::basefield));
	Os ostr(0);
	ostr << setbase(8);
	chk((ostr.flags() & STD ios_base::basefield) == STD ios_base::oct);
	ostr << setbase(10);
	chk((ostr.flags() & STD ios_base::basefield) == STD ios_base::dec);
	ostr << setbase(16);
	chk((ostr.flags() & STD ios_base::basefield) == STD ios_base::hex);
	ostr << setbase(0);
	chk((ostr.flags() & STD ios_base::basefield) == (STD ios_base::fmtflags)0);
	ostr.setf(allf);
	ostr << setbase(23);
	chk(ostr.flags() == (allf & ~STD ios_base::basefield));


	}
	end_chk("_2763T81");
#else
	skip_chk("_2763T81");
#endif /* CASE_2763T81 */

/* _2763T_101 setfill returns */
#if !defined(SKIP_2763T_101)&&(!defined(ONLY)||defined(CASE_2763T_101))
	begin_chk("_2763T_101");
	{
	//Is istr(0);	// disappeared
	//istr >> setfill((Char)'\0');	// changed [JUL95]
	//ieq(istr.fill(), (Char)'\0');
	//istr >> setfill((Char)'x');
	//ieq(istr.fill(), (Char)'x');
	//istr >> setfill((Char)CHAR_MAX);
	//ieq(istr.fill(), (Char)CHAR_MAX);
	//istr >> setfill((Char)CHAR_MIN);
	//ieq(istr.fill(), (Char)CHAR_MIN);

	Os ostr(0);
	ostr << setfill((Char)'\0');
	ieq(ostr.fill(), (Char)'\0');
	ostr << setfill((Char)'x');
	ieq(ostr.fill(), (Char)'x');
	ostr << setfill((Char)CHAR_MAX);
	ieq(ostr.fill(), (Char)CHAR_MAX);
	ostr << setfill((Char)CHAR_MIN);
	ieq(ostr.fill(), (Char)CHAR_MIN);

	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	wostringstream wostr;
	wostr << setfill((wchar_t)L'\0');
	ieq(wostr.fill(), L'\0');
	wostr << setfill((wchar_t)L'x');
	ieq(wostr.fill(), L'x');
	wostr << setfill((wchar_t)WCHAR_MAX);
	ieq(wostr.fill(), WCHAR_MAX);
	wostr << setfill((wchar_t)WCHAR_MIN);
	ieq(wostr.fill(), WCHAR_MIN);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2763T_101");
#else
	skip_chk("_2763T_101");
#endif /* CASE_2763T_101 */

/* _2763T_121 setprecision returns */
#if !defined(SKIP_2763T_121)&&(!defined(ONLY)||defined(CASE_2763T_121))
	begin_chk("_2763T_121");
	{
	Is istr(0);
	istr >> setprecision(0);
	ieq(istr.precision(), 0);
	istr >> setprecision('x');
	ieq(istr.precision(), 'x');
	istr >> setprecision(32767);
	ieq(istr.precision(), 32767);
	istr >> setprecision(-32767);
	ieq(istr.precision(), -32767);
	Os ostr(0);
	ostr << setprecision(0);
	ieq(ostr.precision(), 0);
	ostr << setprecision('x');
	ieq(ostr.precision(), 'x');
	ostr << setprecision(32767);
	ieq(ostr.precision(), 32767);
	ostr << setprecision(-32767);
	ieq(ostr.precision(), -32767);


	}
	end_chk("_2763T_121");
#else
	skip_chk("_2763T_121");
#endif /* CASE_2763T_121 */

/* _2763T_141 setw returns */
#if !defined(SKIP_2763T_141)&&(!defined(ONLY)||defined(CASE_2763T_141))
	begin_chk("_2763T_141");
	{
	Is istr(0);
	istr >> setw(0);
	ieq(istr.width(), 0);
	istr >> setw('x');
	ieq(istr.width(), 'x');
	istr >> setw(32767);
	ieq(istr.width(), 32767);
	istr >> setw(-32767);
	ieq(istr.width(), -32767);
	Os ostr(0);
	ostr << setw(0);
	ieq(ostr.width(), 0);
	ostr << setw('x');
	ieq(ostr.width(), 'x');
	ostr << setw(32767);
	ieq(ostr.width(), 32767);
	ostr << setw(-32767);
	ieq(ostr.width(), -32767);

	}
	end_chk("_2763T_141");
#else
	skip_chk("_2763T_141");
#endif /* CASE_2763T_141 */

return leave_chk("t27e");
}
/* V3.10:0009 */
