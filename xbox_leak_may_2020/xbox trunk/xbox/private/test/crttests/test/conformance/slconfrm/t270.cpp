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
#define CASE_272T01	<iosfwd> synopsis
#define CASE_273T11	<iostream> synopsis
#define CASE_273T32	<iostream> objects constructed by basic_ios::Init
#define CASE_2731T21	cin.tie() returns
#define CASE_2731T51	cerr.flags() has unitbuf set
#define CASE_2731T61	clog associated with stderr
//#define CASE_2732T11	wcin associated with stdin
//#define CASE_2732T21	wcin.tie() returns
//#define CASE_2732T31	wcout associated with stdout
//#define CASE_2732T41	wcerr associated with stderr
//#define CASE_2732T51	wcerr.flags() has unitbuf set
//#define CASE_2732T61	wclog associated with stderr
#define CASE_274T11	<ios> synopsis
#define CASE_2741T11	streamoff requirements
#define CASE_2741T31	streampos requirements
//#define CASE_2741T41	wstreampos requirements
#define CASE_2741T51	streamsize requirements
//#define CASE_2742T12	char_traits definition
//#define CASE_2742T21	char_traits instantiations
//#define CASE_27421T12	char_traits::state_type requirements
//#define CASE_27422T41	char_traits::eof returns
//#define CASE_27422T61	char_traits::not_eof returns
//#define CASE_27422T_121	char_traits::length returns
//#define CASE_27423T21	char_traits::eq returns	CHANGED [MAR96]
//#define CASE_27423T41	char_traits::eq_int_type returns
//#define CASE_27424T21	char_traits::to_char_type returns
//#define CASE_27424T41	char_traits::to_int_type returns
//#define CASE_27424T61	char_traits::copy returns
#endif /* IS_EMBEDDED */

/* _272T01 <iosfwd> synopsis */
#if !defined(SKIP_272T01)&&(!defined(ONLY)||defined(CASE_272T01))
#if DISALLOW_USER_NAME_PROTECTION
 #include <iosfwd>
#else
	#define charT	"charT"	/* looking for trouble */
	#define traits	"traits"
 #include <iosfwd>
	#undef traits
	#undef charT
#endif
#endif /* CASE_272T01 */

/* _273T11 <iostream> synopsis */
#if !defined(SKIP_273T11)&&(!defined(ONLY)||defined(CASE_273T11))
#if DISALLOW_USER_NAME_PROTECTION
 #include <iostream>
#else
	#define stdio		"stdio"	/* looking for trouble */
	#define ispecial	"ispecial"
	#define delbuf		"delbuf"
 #include <iostream>
	#undef stdio
	#undef ispecial
	#undef delbuf
#endif
#endif /* CASE_273T11 */

/* _273T32 <iostream> objects constructed by basic_ios::Init */
#if !defined(SKIP_273T32)&&(!defined(ONLY)||defined(CASE_273T32))
#include <iostream>
int cnt_273T32 = 0;
STD ios::Init x2;
struct x_273T32 {
	x_273T32()
		{if (STD cin.tie() == &STD cout)
			++cnt_273T32; }
	} staticx_273T32;
#endif /* CASE_273T32 */

/* _2731T21 cin.tie() returns */
#if !defined(SKIP_2731T21)&&(!defined(ONLY)||defined(CASE_2731T21))
#include <iostream>
#endif /* CASE_2731T21 */

/* _2731T51 cerr.flags() has unitbuf set */
#if !defined(SKIP_2731T51)&&(!defined(ONLY)||defined(CASE_2731T51))
#include <iostream>
#endif /* CASE_2731T51 */

/* _2731T61 clog associated with stderr */
#if !defined(SKIP_2731T61)&&(!defined(ONLY)||defined(CASE_2731T61))
#include <iostream>
#endif /* CASE_2731T61 */

/* _2732T11 wcin associated with stdin */
#if !defined(SKIP_2732T11)&&(!defined(ONLY)||defined(CASE_2732T11))
#include <iostream>
#endif /* CASE_2732T11 */

/* _2732T21 wcin.tie() returns */
#if !defined(SKIP_2732T21)&&(!defined(ONLY)||defined(CASE_2732T21))
#include <iostream>
#endif /* CASE_2732T21 */

/* _2732T31 wcout associated with stdout */
#if !defined(SKIP_2732T31)&&(!defined(ONLY)||defined(CASE_2732T31))
#include <iostream>
#endif /* CASE_2732T31 */

/* _2732T41 wcerr associated with stderr */
#if !defined(SKIP_2732T41)&&(!defined(ONLY)||defined(CASE_2732T41))
#include <iostream>
#endif /* CASE_2732T41 */

/* _2732T51 wcerr.flags() has unitbuf set */
#if !defined(SKIP_2732T51)&&(!defined(ONLY)||defined(CASE_2732T51))
#include <iostream>
#endif /* CASE_2732T51 */

/* _2732T61 wclog associated with stderr */
#if !defined(SKIP_2732T61)&&(!defined(ONLY)||defined(CASE_2732T61))
#include <iostream>
#endif /* CASE_2732T61 */

/* _274T11 <ios> synopsis */
#if !defined(SKIP_274T11)&&(!defined(ONLY)||defined(CASE_274T11))
#if DISALLOW_USER_NAME_PROTECTION
 #include <ios>
#else
	#define charT	"charT"
	#define POS_T	"POS_T"
	#define OFF_T	"OFF_T"
	#define traits	"traits"
 #include <ios>
	#undef traits
	#undef charT
	#undef POS_T
	#undef OFF_T
#endif
#include <iostream>
#endif /* CASE_274T11 */

/* _2741T11 streamoff requirements */
#if !defined(SKIP_2741T11)&&(!defined(ONLY)||defined(CASE_2741T11))
#include <climits>
#include <ios>
#endif /* CASE_2741T11 */

/* _2741T31 streampos requirements */
#if !defined(SKIP_2741T31)&&(!defined(ONLY)||defined(CASE_2741T31))
#include <climits>
#include <ios>
#endif /* CASE_2741T31 */

/* _2741T41 wstreampos requirements */
#if !defined(SKIP_2741T41)&&(!defined(ONLY)||defined(CASE_2741T41))
#include <climits>
#include <ios>
#endif /* CASE_2741T41 */

/* _2741T51 streamsize requirements */
#if !defined(SKIP_2741T51)&&(!defined(ONLY)||defined(CASE_2741T51))
#include <climits>
#include <ios>
#endif /* CASE_2741T51 */

/* _2742T12 char_traits definition */
#if !defined(SKIP_2742T12)&&(!defined(ONLY)||defined(CASE_2742T12))
#include <cstring>
#include <ios>
#include <locale>
class Mycty_2742T12 : public ctype<unsigned char> {
	};
#endif /* CASE_2742T12 */

/* _2742T21 char_traits instantiations */
#if !defined(SKIP_2742T21)&&(!defined(ONLY)||defined(CASE_2742T21))
#include <ios>
#endif /* CASE_2742T21 */

/* _27421T12 char_traits::state_type requirements */
#if !defined(SKIP_27421T12)&&(!defined(ONLY)||defined(CASE_27421T12))
#include <cstring>
#include <ios>
#include <string>
#endif /* CASE_27421T12 */

/* _27422T41 char_traits::eof returns */
#if !defined(SKIP_27422T41)&&(!defined(ONLY)||defined(CASE_27422T41))
#include <cstdio>
#include <cwchar>
#include <ios>
#include <string>
#endif /* CASE_27422T41 */

/* _27422T61 char_traits::not_eof returns */
#if !defined(SKIP_27422T61)&&(!defined(ONLY)||defined(CASE_27422T61))
#include <ios>
#include <string>
#endif /* CASE_27422T61 */

/* _27422T_121 char_traits::length returns */
#if !defined(SKIP_27422T_121)&&(!defined(ONLY)||defined(CASE_27422T_121))
#include <ios>
#include <string>
#endif /* CASE_27422T_121 */

/* _27423T21 char_traits::eq returns	CHANGED [MAR96] */
#if !defined(SKIP_27423T21)&&(!defined(ONLY)||defined(CASE_27423T21))
#include <ios>
#include <string>
#endif /* CASE_27423T21 */

/* _27423T41 char_traits::eq_int_type returns */
#if !defined(SKIP_27423T41)&&(!defined(ONLY)||defined(CASE_27423T41))
#include <ios>
#include <string>
#endif /* CASE_27423T41 */

/* _27424T21 char_traits::to_char_type returns */
#if !defined(SKIP_27424T21)&&(!defined(ONLY)||defined(CASE_27424T21))
#include <ios>
#include <string>
#endif /* CASE_27424T21 */

/* _27424T41 char_traits::to_int_type returns */
#if !defined(SKIP_27424T41)&&(!defined(ONLY)||defined(CASE_27424T41))
#include <ios>
#include <string>
#endif /* CASE_27424T41 */

/* _27424T61 char_traits::copy returns */
#if !defined(SKIP_27424T61)&&(!defined(ONLY)||defined(CASE_27424T61))
#include <cstring>
#include <cwchar>
#include <ios>
#include <string>
#endif /* CASE_27424T61 */

int t270_main(int, char *[])
{
enter_chk("t270.cpp");
/* _272T01 <iosfwd> synopsis */
#if !defined(SKIP_272T01)&&(!defined(ONLY)||defined(CASE_272T01))
	begin_chk("_272T01");
	{
	//STD streamoff *q0 = 0; TOUCH(q0);
	STD streampos *q1 = 0; TOUCH(q1);
	#if IS_EMBEDDED
	STD ios *p1 = 0; TOUCH(p1);

	STD streambuf *p3 = 0; TOUCH(p3);
	STD istream *p5 = 0; TOUCH(p5);
	STD ostream *p7 = 0; TOUCH(p7);

	STD stringbuf *p13 = 0; TOUCH(p13);
	STD istringstream *p15 = 0; TOUCH(p15);
	STD ostringstream *p17 = 0; TOUCH(p17);

	STD filebuf *p23 = 0; TOUCH(p23);
	STD ifstream *p25 = 0; TOUCH(p25);
	STD ofstream *p27 = 0; TOUCH(p27);
	#else /* IS_EMBEDDED */
	STD char_traits<char> *q2 = 0; TOUCH(q2);
	STD istreambuf_iterator<char, STD char_traits<char> > *q3; TOUCH(q3);
	STD ostreambuf_iterator<char, STD char_traits<char> > *q4; TOUCH(q4);

	STD wstreampos *wq1 = 0; TOUCH(wq1);
	STD char_traits<wchar_t> *wq2 = 0; TOUCH(wq2);

	STD basic_ios<char, STD char_traits<char> > *p0 = 0; TOUCH(p0);
	STD ios *p1 = p0; TOUCH(p1);

	STD basic_streambuf<char, STD char_traits<char> > *p2 = 0; TOUCH(p2);
	STD streambuf *p3 = p2; TOUCH(p3);
	STD basic_istream<char, STD char_traits<char> > *p4 = 0; TOUCH(p4);
	STD istream *p5 = p4; TOUCH(p5);
	STD basic_ostream<char, STD char_traits<char> > *p6 = 0; TOUCH(p6);
	STD ostream *p7 = p6; TOUCH(p7);
	STD basic_iostream<char, STD char_traits<char> > *p8 = 0; TOUCH(p8);
	STD iostream *p9 = p8; TOUCH(p9);

	STD basic_stringbuf<char, STD char_traits<char>,	// CHANGED [JUL96]
		STD allocator<char> > *p12 = 0; TOUCH(p12);
	STD stringbuf *p13 = p12; TOUCH(p13);
	STD basic_istringstream<char, STD char_traits<char>,
		STD allocator<char> > *p14 = 0; TOUCH(p14);
	STD istringstream *p15 = p14; TOUCH(p15);
	STD basic_ostringstream<char, STD char_traits<char>,
		STD allocator<char> > *p16 = 0; TOUCH(p16);
	STD ostringstream *p17 = p16; TOUCH(p17);
	STD basic_stringstream<char, STD char_traits<char>,
		STD allocator<char> > *p18 = 0; TOUCH(p18);
	STD stringstream *p19 = p18; TOUCH(p19);

	STD basic_filebuf<char, STD char_traits<char> > *p22 = 0; TOUCH(p22);
	STD filebuf *p23 = p22; TOUCH(p23);
	STD basic_ifstream<char, STD char_traits<char> > *p24 = 0; TOUCH(p24);
	STD ifstream *p25 = p24; TOUCH(p25);
	STD basic_ofstream<char, STD char_traits<char> > *p26 = 0; TOUCH(p26);
	STD ofstream *p27 = p26; TOUCH(p27);
	STD basic_fstream<char, STD char_traits<char> > *p28 = 0; TOUCH(p28);
	STD fstream *p29 = p28; TOUCH(p29);

	STD basic_ios<wchar_t, STD char_traits<wchar_t> > *wp0 = 0; TOUCH(wp0);
	STD wios *wp1 = wp0; TOUCH(wp1);

	STD basic_streambuf<wchar_t, STD char_traits<wchar_t> > *wp2 = 0; TOUCH(wp2);
	STD wstreambuf *wp3 = wp2; TOUCH(wp3);
	STD basic_istream<wchar_t, STD char_traits<wchar_t> > *wp4 = 0; TOUCH(wp4);
	STD wistream *wp5 = wp4; TOUCH(wp5);
	STD basic_ostream<wchar_t, STD char_traits<wchar_t> > *wp6 = 0; TOUCH(wp6);
	STD wostream *wp7 = wp6; TOUCH(wp7);
	STD basic_iostream<wchar_t, STD char_traits<wchar_t> > *wp8 = 0; TOUCH(wp8);
	STD wiostream *wp9 = wp8; TOUCH(wp9);

	STD basic_stringbuf<wchar_t, STD char_traits<wchar_t>,
		STD allocator<wchar_t> > *wp12 = 0; TOUCH(wp12);	// CHANGED [JUL96]
	STD wstringbuf *wp13 = wp12; TOUCH(wp13);
	STD basic_istringstream<wchar_t, STD char_traits<wchar_t>,
		STD allocator<wchar_t> > *wp14 = 0; TOUCH(wp14);
	STD wistringstream *wp15 = wp14; TOUCH(wp15);
	STD basic_ostringstream<wchar_t, STD char_traits<wchar_t>,
		STD allocator<wchar_t> > *wp16 = 0; TOUCH(wp16);
	STD wostringstream *wp17 = wp16; TOUCH(wp17);
	STD basic_stringstream<wchar_t, STD char_traits<wchar_t>,
		STD allocator<wchar_t> > *wp18 = 0; TOUCH(wp18);
	STD wstringstream *wp19 = wp18; TOUCH(wp19);

	STD basic_filebuf<wchar_t, STD char_traits<wchar_t> > *wp22 = 0; TOUCH(wp22);
	STD wfilebuf *wp23 = wp22; TOUCH(wp23);
	STD basic_ifstream<wchar_t, STD char_traits<wchar_t> > *wp24 = 0; TOUCH(wp24);
	STD wifstream *wp25 = wp24; TOUCH(wp25);
	STD basic_ofstream<wchar_t, STD char_traits<wchar_t> > *wp26 = 0; TOUCH(wp26);
	STD wofstream *wp27 = wp26; TOUCH(wp27);
	STD basic_fstream<wchar_t, STD char_traits<wchar_t> > *wp28 = 0; TOUCH(wp28);
	STD wfstream *wp29 = wp28; TOUCH(wp29);
	#endif /* IS_EMBEDDED */
	chk(1);




	}
	end_chk("_272T01");
#else
	skip_chk("_272T01");
#endif /* CASE_272T01 */

/* _273T11 <iostream> synopsis */
#if !defined(SKIP_273T11)&&(!defined(ONLY)||defined(CASE_273T11))
	begin_chk("_273T11");
	{
	STD istream *p1 = &STD cin; TOUCH(p1);
	STD ostream *p2 = &STD cout; TOUCH(p2);
	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	STD ostream *p3 = &STD cerr; TOUCH(p3);
	STD ostream *p4 = &STD clog; TOUCH(p4);
	STD wistream *p11 = &STD wcin; TOUCH(p11);
	STD wostream *p12 = &STD wcout; TOUCH(p12);
	STD wostream *p13 = &STD wcerr; TOUCH(p13);
	STD wostream *p14 = &STD wclog; TOUCH(p14);
	#endif /* IS_EMBEDDED */
	chk(1);


	}
	end_chk("_273T11");
#else
	skip_chk("_273T11");
#endif /* CASE_273T11 */

/* _273T32 <iostream> objects constructed by basic_ios::Init */
#if !defined(SKIP_273T32)&&(!defined(ONLY)||defined(CASE_273T32))
	begin_chk("_273T32");
	{
	ieq(cnt_273T32, 1);
	chk(STD cin != 0);	// CHANGED [JUL96]
	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	chk(STD wcin != 0);
	#endif /* IS_EMBEDDED */



	}
	end_chk("_273T32");
#else
	skip_chk("_273T32");
#endif /* CASE_273T32 */

/* _2731T21 cin.tie() returns */
#if !defined(SKIP_2731T21)&&(!defined(ONLY)||defined(CASE_2731T21))
	begin_chk("_2731T21");
	{
	chk(STD cin.tie() == &STD cout);


	}
	end_chk("_2731T21");
#else
	skip_chk("_2731T21");
#endif /* CASE_2731T21 */

/* _2731T51 cerr.flags() has unitbuf set */
#if !defined(SKIP_2731T51)&&(!defined(ONLY)||defined(CASE_2731T51))
	begin_chk("_2731T51");
	{
	#if IS_EMBEDDED
	chk(1);
	#else /* IS_EMBEDDED */
	chk((STD cerr.flags() & STD ios_base::unitbuf) != 0);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2731T51");
#else
	skip_chk("_2731T51");
#endif /* CASE_2731T51 */

/* _2731T61 clog associated with stderr */
#if !defined(SKIP_2731T61)&&(!defined(ONLY)||defined(CASE_2731T61))
	begin_chk("_2731T61");
	{
	// UNTESTABLE
	chk(1);


	}
	end_chk("_2731T61");
#else
	skip_chk("_2731T61");
#endif /* CASE_2731T61 */

/* _2732T11 wcin associated with stdin */
#if !defined(SKIP_2732T11)&&(!defined(ONLY)||defined(CASE_2732T11))
	begin_chk("_2732T11");
	{
	// UNTESTABLE
	chk(1);



	}
	end_chk("_2732T11");
#else
	skip_chk("_2732T11");
#endif /* CASE_2732T11 */

/* _2732T21 wcin.tie() returns */
#if !defined(SKIP_2732T21)&&(!defined(ONLY)||defined(CASE_2732T21))
	begin_chk("_2732T21");
	{
	chk(STD wcin.tie() == &STD wcout);


	}
	end_chk("_2732T21");
#else
	skip_chk("_2732T21");
#endif /* CASE_2732T21 */

/* _2732T31 wcout associated with stdout */
#if !defined(SKIP_2732T31)&&(!defined(ONLY)||defined(CASE_2732T31))
	begin_chk("_2732T31");
	{
	// UNTESTABLE
	chk(1);


	}
	end_chk("_2732T31");
#else
	skip_chk("_2732T31");
#endif /* CASE_2732T31 */

/* _2732T41 wcerr associated with stderr */
#if !defined(SKIP_2732T41)&&(!defined(ONLY)||defined(CASE_2732T41))
	begin_chk("_2732T41");
	{
	// UNTESTABLE
	chk(1);


	}
	end_chk("_2732T41");
#else
	skip_chk("_2732T41");
#endif /* CASE_2732T41 */

/* _2732T51 wcerr.flags() has unitbuf set */
#if !defined(SKIP_2732T51)&&(!defined(ONLY)||defined(CASE_2732T51))
	begin_chk("_2732T51");
	{
	chk((STD wcerr.flags() & STD ios_base::unitbuf) != 0);


	}
	end_chk("_2732T51");
#else
	skip_chk("_2732T51");
#endif /* CASE_2732T51 */

/* _2732T61 wclog associated with stderr */
#if !defined(SKIP_2732T61)&&(!defined(ONLY)||defined(CASE_2732T61))
	begin_chk("_2732T61");
	{
	// UNTESTABLE
	chk(1);







	}
	end_chk("_2732T61");
#else
	skip_chk("_2732T61");
#endif /* CASE_2732T61 */

/* _274T11 <ios> synopsis */
#if !defined(SKIP_274T11)&&(!defined(ONLY)||defined(CASE_274T11))
	begin_chk("_274T11");
	{
	STD streamoff so; TOUCH(so);	// restored
	//STD wstreamoff *pwso = &so; // deleted [JUL95]
	STD streampos sp;
	STD streamsize ss; TOUCH(ss);
	#if IS_EMBEDDED
	STD ios *p5 = 0; TOUCH(p5);
	#else /* IS_EMBEDDED */
	STD wstreampos *pwsp = &sp; TOUCH(pwsp);
	STD char_traits<char> *p1 = 0; TOUCH(p1);
	STD char_traits<wchar_t> *p2 = 0; TOUCH(p2);
	STD ios_base *p3
		= (STD basic_ios<char, STD char_traits<char> > *)&STD cin; TOUCH(p3);
	STD basic_ios<char, STD char_traits<char> > *p4 = 0; TOUCH(p4);
	STD ios *p5 = p4; TOUCH(p5);
	STD basic_ios<wchar_t, STD char_traits<wchar_t> > *p6 = 0; TOUCH(p6);
	STD wios *p7 = p6; TOUCH(p7);
	#endif /* IS_EMBEDDED */
	typedef STD ios_base& (*Pf)(STD ios_base&);
	Pf parr[] = {
		&STD boolalpha, &STD noboolalpha, &STD showbase, &STD noshowbase,
		&STD showpoint, &STD noshowpoint, &STD showpos, &STD noshowpos,
		&STD skipws, &STD noskipws, &STD uppercase, &STD nouppercase,
		&STD internal, &STD left, &STD right, &STD dec, &STD hex, &STD oct,
		&STD fixed, &STD scientific}; TOUCH(parr);
	chk(1);


	}
	end_chk("_274T11");
#else
	skip_chk("_274T11");
#endif /* CASE_274T11 */

/* _2741T11 streamoff requirements */
#if !defined(SKIP_2741T11)&&(!defined(ONLY)||defined(CASE_2741T11))
	begin_chk("_2741T11");
	{
	STD streamoff so = LONG_MAX;
	chk(so == LONG_MAX);



	// _2741T21 streamsize requirements // deleted [JUL95]
	//FILE-SCOPE:
	//#include <climits>
	//#include <ios>
	//BLOCK-SCOPE:
	//STD wstreamoff wso = LONG_MAX;
	//chk(wso == LONG_MAX);


	}
	end_chk("_2741T11");
#else
	skip_chk("_2741T11");
#endif /* CASE_2741T11 */

/* _2741T31 streampos requirements */
#if !defined(SKIP_2741T31)&&(!defined(ONLY)||defined(CASE_2741T31))
	begin_chk("_2741T31");
	{
	STD streamoff badoff = -1;
	STD streamoff s0 = 0;
	const STD streampos sp0 = s0;
	chk((streamoff)sp0 == s0);
	const STD streampos sp5((streamoff)5);
	chk((streamoff)sp5 == (streamoff)5);
	const STD streampos spx(badoff);
	chk((streamoff)spx == badoff);
	chk(sp5 - sp0 == 5);
	chk(sp0 - sp5 == -5);
	STD streampos sp;
	chk((streamoff)sp == 0);
	sp += LONG_MAX;
	chk((streamoff)sp == LONG_MAX);
	sp -= 30;
	chk((streamoff)sp == LONG_MAX - 30);
	sp = sp5 + (streamoff)10;
	chk((streamoff)sp == 15);
	sp = sp5 - (streamoff)10;
	chk((streamoff)sp == -5);
	chk(sp0 == sp0);
	chk(sp0 != sp5);
	chk(spx == spx);
	chk(spx != sp5);
	sp = sp0 - (streamoff)1;
	chk(sp0 != spx);



	}
	end_chk("_2741T31");
#else
	skip_chk("_2741T31");
#endif /* CASE_2741T31 */

/* _2741T41 wstreampos requirements */
#if !defined(SKIP_2741T41)&&(!defined(ONLY)||defined(CASE_2741T41))
	begin_chk("_2741T41");
	{
	STD streamoff badoff = -1;	// wstreamoff-->streamoff [JUL95]
	STD streamoff s0 = 0;
	const STD wstreampos sp0 = s0;
	chk((streamoff)sp0 == s0);
	const STD wstreampos sp5((streamoff)5);
	chk((streamoff)sp5 == (streamoff)5);
	const STD wstreampos spx(badoff);
	chk((streamoff)spx == badoff);
	chk(sp5 - sp0 == 5);
	chk(sp0 - sp5 == -5);
	STD wstreampos sp;
	chk((streamoff)sp == 0);
	sp += LONG_MAX;
	chk((streamoff)sp == LONG_MAX);
	sp -= 30;
	chk((streamoff)sp == LONG_MAX - 30);
	sp = sp5 + (streamoff)10;
	chk((streamoff)sp == 15);
	sp = sp5 - (streamoff)10;
	chk((streamoff)sp == -5);
	chk(sp0 == sp0);
	chk(sp0 != sp5);
	chk(spx == spx);
	chk(spx != sp5);
	sp = sp0 - (streamoff)1;
	chk(sp0 != spx);


	}
	end_chk("_2741T41");
#else
	skip_chk("_2741T41");
#endif /* CASE_2741T41 */

/* _2741T51 streamsize requirements */
#if !defined(SKIP_2741T51)&&(!defined(ONLY)||defined(CASE_2741T51))
	begin_chk("_2741T51");
	{
	STD streamsize ss = INT_MAX;
	ieq(ss, INT_MAX);







	}
	end_chk("_2741T51");
#else
	skip_chk("_2741T51");
#endif /* CASE_2741T51 */

/* _2742T12 char_traits definition */
#if !defined(SKIP_2742T12)&&(!defined(ONLY)||defined(CASE_2742T12))
	begin_chk("_2742T12");
	{
	typedef STD char_traits<unsigned char> Iost;
	Mycty_2742T12 ufac;
	unsigned char uc;
	Iost::int_type ui = 0;
	Iost::char_type *p1 = &uc; TOUCH(p1);
	Iost::pos_type pos = 0; TOUCH(pos);
	Iost::off_type off = 0; TOUCH(off);
	Iost::state_type state; TOUCH(state);
	streampos fpos;
	//Iost::state_type st = Iost::get_state(pos);	DROPPED [JUL96]
	//pos = Iost::get_pos(fpos, state);

	//uc = Iost::eos();	DROPPED [MAR96]
	//ieq(uc, Iost::eos());	DROPPED [MAR96]

	ui = Iost::eof();
	chk(ui == Iost::eof());
	ui = Iost::not_eof('x');
	chk(ui != Iost::eof());

	//uc = Iost::newline();	DROPPED [MAR96]
	//ieq(uc, Iost::newline());

	ieq(Iost::length((const unsigned char *)"abcd"), 4);

	chk(Iost::eq('x', 'x'));	// CHANGED [MAR96]
	chk(!Iost::eq('x', 'y'));
	chk(Iost::eq_int_type(Iost::to_int_type('x'), Iost::to_int_type('x')));

	//chk(Iost::is_eof(Iost::eof()));	DROPPED [MAR96]

	ieq(Iost::to_char_type(Iost::to_int_type('x')), (unsigned char)'x');

	//chk(Iost::is_whitespace((unsigned char)' ', ufac));	DROPPED [MAR96]

	char a[] = "xxxxx";
	Iost::copy((unsigned char *)a, (const unsigned char *)"abcd", 4);
	ieq(STD memcmp(a, "abcdx", 6), 0);




	}
	end_chk("_2742T12");
#else
	skip_chk("_2742T12");
#endif /* CASE_2742T12 */

/* _2742T21 char_traits instantiations */
#if !defined(SKIP_2742T21)&&(!defined(ONLY)||defined(CASE_2742T21))
	begin_chk("_2742T21");
	{
	STD char_traits<char> it1; TOUCH(it1);
	STD char_traits<wchar_t> it2; TOUCH(it2);
	chk(1);



	}
	end_chk("_2742T21");
#else
	skip_chk("_2742T21");
#endif /* CASE_2742T21 */

/* _27421T12 char_traits::state_type requirements */
#if !defined(SKIP_27421T12)&&(!defined(ONLY)||defined(CASE_27421T12))
	begin_chk("_27421T12");
	{
	static STD char_traits<unsigned char>::state_type st1, st2;
	ieq(STD memcmp(&st1, &st2, sizeof (st1)), 0);


	//### _27422T22 ios_traits::eos returns	DROPPED [MAR96]
	//FILE-SCOPE:
	//#include <ios>
	//BLOCK-SCOPE:
	//ieq(STD ios_traits<char>::eos(), '\0');
	//ieq(STD ios_traits<wchar_t>::eos(), L'\0');


	}
	end_chk("_27421T12");
#else
	skip_chk("_27421T12");
#endif /* CASE_27421T12 */

/* _27422T41 char_traits::eof returns */
#if !defined(SKIP_27422T41)&&(!defined(ONLY)||defined(CASE_27422T41))
	begin_chk("_27422T41");
	{
	ieq(STD char_traits<char>::eof(), EOF);
	ieq(STD char_traits<wchar_t>::eof(), WEOF);




	}
	end_chk("_27422T41");
#else
	skip_chk("_27422T41");
#endif /* CASE_27422T41 */

/* _27422T61 char_traits::not_eof returns */
#if !defined(SKIP_27422T61)&&(!defined(ONLY)||defined(CASE_27422T61))
	begin_chk("_27422T61");
	{
	chk(STD char_traits<char>::not_eof('x') == 'x');
	chk(STD char_traits<char>::not_eof(STD char_traits<char>::eof())
		!= STD char_traits<char>::eof());
	chk(STD char_traits<wchar_t>::not_eof(L'x') == L'x');
	chk(STD char_traits<wchar_t>::not_eof(STD char_traits<wchar_t>::eof())
		!= STD char_traits<wchar_t>::eof());




	//### _27422T_101 ios_traits::newline returns	DROPPED [MAR96]
	//FILE-SCOPE:
	//#include <ios>
	//BLOCK-SCOPE:
	//ieq(STD ios_traits<char>::newline(), '\n');
	//ieq(STD ios_traits<wchar_t>::newline(), L'\n');



	}
	end_chk("_27422T61");
#else
	skip_chk("_27422T61");
#endif /* CASE_27422T61 */

/* _27422T_121 char_traits::length returns */
#if !defined(SKIP_27422T_121)&&(!defined(ONLY)||defined(CASE_27422T_121))
	begin_chk("_27422T_121");
	{
	ieq(STD char_traits<char>::length("abcd"), 4);
	ieq(STD char_traits<char>::length(""), 0);
	wchar_t abcd[] = {L'a', L'b', L'c', L'd', 0};
	ieq(STD char_traits<wchar_t>::length(abcd), 4);
	ieq(STD char_traits<wchar_t>::length(abcd + 4), 0);


	}
	end_chk("_27422T_121");
#else
	skip_chk("_27422T_121");
#endif /* CASE_27422T_121 */

/* _27423T21 char_traits::eq returns	CHANGED [MAR96] */
#if !defined(SKIP_27423T21)&&(!defined(ONLY)||defined(CASE_27423T21))
	begin_chk("_27423T21");
	{
	chk(STD char_traits<char>::eq('a', 'a'));
	chk(!STD char_traits<char>::eq('a', 'b'));
	chk(STD char_traits<wchar_t>::eq(L'a', L'a'));
	chk(!STD char_traits<wchar_t>::eq(L'a', L'b'));


	}
	end_chk("_27423T21");
#else
	skip_chk("_27423T21");
#endif /* CASE_27423T21 */

/* _27423T41 char_traits::eq_int_type returns */
#if !defined(SKIP_27423T41)&&(!defined(ONLY)||defined(CASE_27423T41))
	begin_chk("_27423T41");
	{
	chk(STD char_traits<char>::eq_int_type((int)'a', (int)'a'));
	chk(!STD char_traits<char>::eq_int_type((int)'a', (int)'b'));
	chk(STD char_traits<wchar_t>::eq_int_type((wint_t)L'a', (wint_t)L'a'));
	chk(!STD char_traits<wchar_t>::eq_int_type((wint_t)L'a', (wint_t)L'b'));


	//### _27423T61 ios_traits::is_eof returns	DROPPED [MAR96]
	//FILE-SCOPE:
	//#include <ios>
	//BLOCK-SCOPE:
	//chk(STD ios_traits<char>::is_eof(STD ios_traits<char>::eof()));
	//chk(!STD ios_traits<char>::is_eof(STD ios_traits<char>::eos()));
	//chk(!STD ios_traits<char>::is_eof('a'));
	//chk(STD ios_traits<wchar_t>::is_eof(STD ios_traits<wchar_t>::eof()));
	//chk(!STD ios_traits<wchar_t>::is_eof(STD ios_traits<wchar_t>::eos()));
	//chk(!STD ios_traits<wchar_t>::is_eof(L'a'));





	//### _27423T82 ios_traits::is_whitespace returns	DROPPED [MAR96]
	//FILE-SCOPE:
	//#include <ios>
	//#include <locale>
	//class Myctyc_27423T41 : public ctype<char> {
	//	};
	//class Myctyw_27423T41 : public ctype<wchar_t> {
	//	};
	//BLOCK-SCOPE:
	//Myctyc_27423T41 cfac;
	//Myctyw_27423T41 wfac;
	//chk(STD ios_traits<char>::is_whitespace(' ', cfac));
	//chk(!STD ios_traits<char>::is_whitespace('x', cfac));
	//chk(STD ios_traits<wchar_t>::is_whitespace(L' ', wfac));
	//chk(!STD ios_traits<wchar_t>::is_whitespace(L'x', wfac));





	}
	end_chk("_27423T41");
#else
	skip_chk("_27423T41");
#endif /* CASE_27423T41 */

/* _27424T21 char_traits::to_char_type returns */
#if !defined(SKIP_27424T21)&&(!defined(ONLY)||defined(CASE_27424T21))
	begin_chk("_27424T21");
	{
	ieq(STD char_traits<char>::to_char_type((int)'x'), 'x');
	ieq(STD char_traits<wchar_t>::to_char_type((wint_t)L'x'), L'x');



	}
	end_chk("_27424T21");
#else
	skip_chk("_27424T21");
#endif /* CASE_27424T21 */

/* _27424T41 char_traits::to_int_type returns */
#if !defined(SKIP_27424T41)&&(!defined(ONLY)||defined(CASE_27424T41))
	begin_chk("_27424T41");
	{
	ieq(STD char_traits<char>::to_int_type('x'), (int)'x');
	ieq(STD char_traits<wchar_t>::to_int_type(L'x'), (wint_t)L'x');



	}
	end_chk("_27424T41");
#else
	skip_chk("_27424T41");
#endif /* CASE_27424T41 */

/* _27424T61 char_traits::copy returns */
#if !defined(SKIP_27424T61)&&(!defined(ONLY)||defined(CASE_27424T61))
	begin_chk("_27424T61");
	{
	char a[] = "xxxxx";
	char *pa = STD char_traits<char>::copy(a, "", 0);
	chk(pa == a);
	ieq(STD memcmp(a, "xxxxx", 6), 0);
	pa = STD char_traits<char>::copy(a, "abcd", 4);
	chk(pa == a);
	ieq(STD memcmp(a, "abcdx", 6), 0);
	wchar_t wa[6];
	const wchar_t xxxxx[] = {L'x', L'x', L'x', L'x', L'x', 0};
	const wchar_t abcd[] = {L'a', L'b', L'c', L'd', 0};
	wmemcpy(wa, xxxxx, 6);
	wchar_t *pwa = STD char_traits<wchar_t>::copy(wa, abcd, 0);
	chk(pwa == wa);
	ieq(STD wmemcmp(wa, xxxxx, 6), 0);
	pwa = STD char_traits<wchar_t>::copy(wa, abcd, 4);
	chk(pwa == wa);
	ieq(STD wmemcmp(wa, abcd, 4), 0);
	ieq(STD wmemcmp(wa + 4, xxxxx + 4, 2), 0);



	//### _27424T81 char_traits::get_state returns	DROPPED [JUL96]
	//FILE-SCOPE:
	//#include <ios>
	//BLOCK-SCOPE:
	//STD char_traits<char>::pos_type pos = 0;
	//STD char_traits<char>::state_type st =
	//	STD char_traits<char>::get_state(pos);
	//STD char_traits<wchar_t>::pos_type wpos = 0;
	//STD char_traits<wchar_t>::state_type wst =
	//	STD char_traits<wchar_t>::get_state(wpos);
	//chk(1);



	//### _27424T_101 char_traits::get_pos returns	DROPPED [JUL96]
	//FILE-SCOPE:
	//#include <ios>
	//BLOCK-SCOPE:
	//STD streampos fpos;
	//STD char_traits<char>::state_type state;
	//STD char_traits<char>::pos_type pt =
	//	STD char_traits<char>::get_pos(fpos, state);
	//STD char_traits<wchar_t>::state_type wstate;
	//STD char_traits<wchar_t>::pos_type wpt =
	//	STD char_traits<wchar_t>::get_pos(fpos, wstate);
	//chk(1);

	}
	end_chk("_27424T61");
#else
	skip_chk("_27424T61");
#endif /* CASE_27424T61 */

return leave_chk("t270");
}
/* V3.10:0009 */
