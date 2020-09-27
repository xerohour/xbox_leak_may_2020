/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _277T11 <sstream> synopsis */
#if !defined(SKIP_277T11)&&(!defined(ONLY)||defined(CASE_277T11))
#include <sstream>
#endif /* CASE_277T11 */

/* _2771T11 basic_stringbuf definition */
#if !defined(SKIP_2771T11)&&(!defined(ONLY)||defined(CASE_2771T11))
#include <sstream>
#endif /* CASE_2771T11 */

/* _2771T31 basic_stringbuf::basic_stringbuf(openmode) effects */
#if !defined(SKIP_2771T31)&&(!defined(ONLY)||defined(CASE_2771T31))
#include <sstream>
#endif /* CASE_2771T31 */

/* _2771T51 basic_stringbuf::basic_stringbuf(string) effects */
#if !defined(SKIP_2771T51)&&(!defined(ONLY)||defined(CASE_2771T51))
#include <sstream>
#endif /* CASE_2771T51 */

/* _2771T61 basic_stringbuf::basic_stringbuf(string) postcondition, str() == str */
#if !defined(SKIP_2771T61)&&(!defined(ONLY)||defined(CASE_2771T61))
#include <sstream>
#endif /* CASE_2771T61 */

/* _2771T62 basic_stringbuf::basic_stringbuf(string) postcondition, pointers */
#if !defined(SKIP_2771T62)&&(!defined(ONLY)||defined(CASE_2771T62))
#include <sstream>
#endif /* CASE_2771T62 */

/* _27712T21 basic_stringbuf::str() returns */
#if !defined(SKIP_27712T21)&&(!defined(ONLY)||defined(CASE_27712T21))
#include <sstream>
#endif /* CASE_27712T21 */

/* _27712T41 basic_stringbuf::str(string) effects */
#if !defined(SKIP_27712T41)&&(!defined(ONLY)||defined(CASE_27712T41))
#include <sstream>
#endif /* CASE_27712T41 */

/* _27712T51 basic_stringbuf::str(string) postcondition */
#if !defined(SKIP_27712T51)&&(!defined(ONLY)||defined(CASE_27712T51))
#include <sstream>
#endif /* CASE_27712T51 */

/* _27713T21 basic_stringbuf::underflow returns, *gptr() */
#if !defined(SKIP_27713T21)&&(!defined(ONLY)||defined(CASE_27713T21))
#include <sstream>
class Mystringbuf_27713T21 : public Bss {
public:
	int_type Myunderflow()
		{return (underflow()); }
	};
#endif /* CASE_27713T21 */

/* _27713T22 basic_stringbuf::underflow returns, eof() */
#if !defined(SKIP_27713T22)&&(!defined(ONLY)||defined(CASE_27713T22))
#include <sstream>
class Mystringbuf_27713T22 : public Bss {
public:
	int_type Myunderflow()
		{return (underflow()); }
	};
#endif /* CASE_27713T22 */

/* _27713T41 basic_stringbuf::pbackfail effects, --gptr() */
#if !defined(SKIP_27713T41)&&(!defined(ONLY)||defined(CASE_27713T41))
#include <sstream>
class Mystringbuf_27713T41 : public Bss {
public:
	Mystringbuf_27713T41(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Mypbackfail(int_type c)
		{return (pbackfail(c)); }
	};
#endif /* CASE_27713T41 */

/* _27713T42 basic_stringbuf::pbackfail effects, return c after --gptr() */
#if !defined(SKIP_27713T42)&&(!defined(ONLY)||defined(CASE_27713T42))
#include <sstream>
class Mystringbuf_27713T42 : public Bss {
public:
	Mystringbuf_27713T42(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Mypbackfail(int_type c)
		{return (pbackfail(c)); }
	};
#endif /* CASE_27713T42 */

/* _27713T43 basic_stringbuf::pbackfail effects, *--gptr() = c */
#if !defined(SKIP_27713T43)&&(!defined(ONLY)||defined(CASE_27713T43))
#include <sstream>
class Mystringbuf_27713T43 : public Bss {
public:
	int_type Mypbackfail(int_type c)
		{return (pbackfail(c)); }
	};
#endif /* CASE_27713T43 */

/* _27713T44 basic_stringbuf::pbackfail effects, return c after *--gptr() = c */
#if !defined(SKIP_27713T44)&&(!defined(ONLY)||defined(CASE_27713T44))
#include <sstream>
class Mystringbuf_27713T44 : public Bss {
public:
	int_type Mypbackfail(int_type c)
		{return (pbackfail(c)); }
	};
#endif /* CASE_27713T44 */

/* _27713T45 basic_stringbuf::pbackfail effects, c == eof() */
#if !defined(SKIP_27713T45)&&(!defined(ONLY)||defined(CASE_27713T45))
#include <sstream>
class Mystringbuf_27713T45 : public Bss {
public:
	Mystringbuf_27713T45(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Mypbackfail(int_type c)
		{return (pbackfail(c)); }
	};
#endif /* CASE_27713T45 */

/* _27713T46 basic_stringbuf::pbackfail effects, return not_eof(c) */
#if !defined(SKIP_27713T46)&&(!defined(ONLY)||defined(CASE_27713T46))
#include <sstream>
class Mystringbuf_27713T46 : public Bss {
public:
	Mystringbuf_27713T46(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Mypbackfail(int_type c)
		{return (pbackfail(c)); }
	};
#endif /* CASE_27713T46 */

/* _27713T51 basic_stringbuf::pbackfail returns */
#if !defined(SKIP_27713T51)&&(!defined(ONLY)||defined(CASE_27713T51))
#include <sstream>
class Mystringbuf_27713T51 : public Bss {
public:
	Mystringbuf_27713T51(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Mypbackfail(int_type c)
		{return (pbackfail(c)); }
	};
#endif /* CASE_27713T51 */

/* _27713T71 basic_stringbuf::overflow effects, c != eof() */
#if !defined(SKIP_27713T71)&&(!defined(ONLY)||defined(CASE_27713T71))
#include <sstream>
class Mystringbuf_27713T71 : public Bss {
public:
	Mystringbuf_27713T71(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Myoverflow(int_type c)
		{return (overflow(c)); }
	};
#endif /* CASE_27713T71 */

/* _27713T72 basic_stringbuf::overflow effects, returns c after c != eof() */
#if !defined(SKIP_27713T72)&&(!defined(ONLY)||defined(CASE_27713T72))
#include <sstream>
class Mystringbuf_27713T72 : public Bss {
public:
	Mystringbuf_27713T72(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Myoverflow(int_type c)
		{return (overflow(c)); }
	};
#endif /* CASE_27713T72 */

/* _27713T73 basic_stringbuf::overflow effects, c == eof() */
#if !defined(SKIP_27713T73)&&(!defined(ONLY)||defined(CASE_27713T73))
#include <sstream>
class Mystringbuf_27713T73 : public Bss {
public:
	Mystringbuf_27713T73(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Myoverflow(int_type c)
		{return (overflow(c)); }
	};
#endif /* CASE_27713T73 */

/* _27713T74 basic_stringbuf::overflow effects, returns not_eof(c) */
#if !defined(SKIP_27713T74)&&(!defined(ONLY)||defined(CASE_27713T74))
#include <sstream>
class Mystringbuf_27713T74 : public Bss {
public:
	Mystringbuf_27713T74(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Myoverflow(int_type c)
		{return (overflow(c)); }
	};
#endif /* CASE_27713T74 */

/* _27713T91 basic_stringbuf::overflow returns */
#if !defined(SKIP_27713T91)&&(!defined(ONLY)||defined(CASE_27713T91))
#include <sstream>
class Mystringbuf_27713T91 : public Bss {
public:
	Mystringbuf_27713T91(STD ios_base::openmode mode)
		: Bss(mode) {}
	int_type Myoverflow(int_type c)
		{return (overflow(c)); }
	};
#endif /* CASE_27713T91 */

int t27f_main(int, char *[])
{
enter_chk("t27f.cpp");
/* _277T11 <sstream> synopsis */
#if !defined(SKIP_277T11)&&(!defined(ONLY)||defined(CASE_277T11))
	begin_chk("_277T11");
	{
	#if IS_EMBEDDED
	stringbuf *p1 = 0; TOUCH(p1);
	istringstream *p3 = 0; TOUCH(p3);
	ostringstream *p5 = 0; TOUCH(p5);
	chk(1);
	#else /* IS_EMBEDDED */
	typedef STD char_traits<char> CIt;
	typedef ALLOCATOR(char) CAl;
	typedef STD char_traits<wchar_t> WIt;
	typedef ALLOCATOR(wchar_t) WAl;
	basic_stringbuf<char, CIt, CAl> *p1 = 0; TOUCH(p1);
	stringbuf *p2 = p1; TOUCH(p2);
	basic_istringstream<char, CIt, CAl> *p3 = 0; TOUCH(p3);
	istringstream *p4 = p3; TOUCH(p4);
	basic_ostringstream<char, CIt, CAl> *p5 = 0; TOUCH(p5);
	ostringstream *p6 = p5; TOUCH(p6);
	basic_stringbuf<wchar_t, WIt, WAl> *pw1 = 0; TOUCH(pw1);
	wstringbuf *pw2 = pw1; TOUCH(pw2);
	basic_istringstream<wchar_t, WIt, WAl> *pw3 = 0; TOUCH(pw3);
	wistringstream *pw4 = pw3; TOUCH(pw4);
	basic_ostringstream<wchar_t, WIt, WAl> *pw5 = 0; TOUCH(pw5);
	wostringstream *pw6 = pw5; TOUCH(pw6);

	stringstream *p7 = (basic_stringstream<char, CIt, CAl> *)0; TOUCH(p7);
	wstringstream *p8 = (basic_stringstream<wchar_t, WIt, WAl> *)0; TOUCH(p8);

	typedef STD basic_stringstream<Char, It, Al> Iostr;	// ADDED [MAR96]
	Iostr::traits_type *ptt = (It *)0; TOUCH(ptt);	// ADDED [JUL96]
	Iostr::char_type *p9 = (Char *)0; TOUCH(p9);
	Iostr::int_type *p10 = 0; TOUCH(p10);
	Iostr::pos_type *p11 = 0; TOUCH(p11);
	Iostr::off_type *p12 = 0; TOUCH(p12);

	Iostr iostr, iostr1(ios_base::in);
	int i = 37;
	iostr << i << endl;

	iostr.rdbuf()->pubseekoff(0, ios_base::beg);
	iostr1.str(iostr.str());
	i = 0;
	iostr >> i;
	ieq(i, 37);
	#endif /* IS_EMBEDDED */




	}
	end_chk("_277T11");
#else
	skip_chk("_277T11");
#endif /* CASE_277T11 */

/* _2771T11 basic_stringbuf definition */
#if !defined(SKIP_2771T11)&&(!defined(ONLY)||defined(CASE_2771T11))
	begin_chk("_2771T11");
	{
	Cstr input((const Char *)"abcdefg");
	Bss sbuf, sbuf1(STD ios_base::out), sbuf2(input),
		subf3(input, STD ios_base::in);
	Bs *p = &sbuf; TOUCH(p);
	Bss::traits_type *ptt = (It *)0; TOUCH(ptt);	// ADDED [JUL96]
	Bss::char_type *pct = (Char *)0; TOUCH(pct);
	Bss::int_type *pit = (It::int_type *)0; TOUCH(pit);
	Bss::pos_type *pst = (It::pos_type *)0; TOUCH(pst);
	Bss::off_type *pot = (It::off_type *)0; TOUCH(pot);
	sbuf1.str(input);
	chk(sbuf2.str() == input);


	}
	end_chk("_2771T11");
#else
	skip_chk("_2771T11");
#endif /* CASE_2771T11 */

/* _2771T31 basic_stringbuf::basic_stringbuf(openmode) effects */
#if !defined(SKIP_2771T31)&&(!defined(ONLY)||defined(CASE_2771T31))
	begin_chk("_2771T31");
	{
	Cstr input((const Char *)"abcdefg");
	Bss sbuf, sbuf1(STD ios_base::out), sbuf2(STD ios_base::in),
		sbuf3(STD ios_base::in | STD ios_base::out),
		sbuf4((STD ios_base::openmode)0);
	Char ch = (Char)'a';
	Bss::int_type eof = It::eof();
	chk(sbuf.sputc(ch) == ch);
	chk(sbuf.sgetc() == ch);
	chk(sbuf1.sputc(ch) == ch);
	chk(sbuf1.sgetc() == eof);
	chk(sbuf2.sputc(ch) == eof);
	sbuf2.str(input);
	chk(sbuf2.sgetc() == ch);
	chk(sbuf3.sputc(ch) == ch);
	chk(sbuf3.sgetc() == ch);
	chk(sbuf4.sputc(ch) == eof);
	sbuf4.str(input);
	chk(sbuf4.sgetc() == eof);



	}
	end_chk("_2771T31");
#else
	skip_chk("_2771T31");
#endif /* CASE_2771T31 */

/* _2771T51 basic_stringbuf::basic_stringbuf(string) effects */
#if !defined(SKIP_2771T51)&&(!defined(ONLY)||defined(CASE_2771T51))
	begin_chk("_2771T51");
	{
	Cstr input((const Char *)"abcdefg");
	Bss sbuf(input), sbuf1(input, STD ios_base::out),
		sbuf2(input, STD ios_base::in),
		sbuf3(input, STD ios_base::in | STD ios_base::out),
		sbuf4(input, (STD ios_base::openmode)0);
	Char ch = (Char)'a';
	Bss::int_type eof = It::eof();
	chk(sbuf.sputc(ch) == ch);
	chk(sbuf.sgetc() == ch);
	chk(sbuf1.sputc(ch) == ch);
	chk(sbuf1.sgetc() == eof);
	chk(sbuf2.sgetc() == ch);
	chk(sbuf3.sputc(ch) == ch);
	chk(sbuf3.sgetc() == ch);
	chk(sbuf4.sputc(ch) == eof);
	chk(sbuf4.sgetc() == eof);



	}
	end_chk("_2771T51");
#else
	skip_chk("_2771T51");
#endif /* CASE_2771T51 */

/* _2771T61 basic_stringbuf::basic_stringbuf(string) postcondition, str() == str */
#if !defined(SKIP_2771T61)&&(!defined(ONLY)||defined(CASE_2771T61))
	begin_chk("_2771T61");
	{
	Cstr input((const Char *)"abcdefg");
	Bss sbuf(input), sbuf1(input, STD ios_base::out),
		sbuf2(input, STD ios_base::in),
		sbuf3(input, STD ios_base::in | STD ios_base::out);
	chk(sbuf.str() == input);
	sbuf1.pubseekoff(0, STD ios_base::end, STD ios_base::out);
	chk(sbuf1.str() == input);
	chk(sbuf2.str() == input);
	chk(sbuf3.str() == input);



	}
	end_chk("_2771T61");
#else
	skip_chk("_2771T61");
#endif /* CASE_2771T61 */

/* _2771T62 basic_stringbuf::basic_stringbuf(string) postcondition, pointers */
#if !defined(SKIP_2771T62)&&(!defined(ONLY)||defined(CASE_2771T62))
	begin_chk("_2771T62");
	{
	Cstr input((const Char *)"abcdefg");
	Bss sbuf(input), sbuf1(input, STD ios_base::out),
		sbuf2(input, STD ios_base::in),
		sbuf3(input, STD ios_base::in | STD ios_base::out);
	chk(sbuf.str() == input);
	sbuf1.pubseekoff(0, STD ios_base::end, STD ios_base::out);
	chk(sbuf1.str() == input);
	chk(sbuf2.str() == input);
	chk(sbuf3.str() == input);


	}
	end_chk("_2771T62");
#else
	skip_chk("_2771T62");
#endif /* CASE_2771T62 */

/* _27712T21 basic_stringbuf::str() returns */
#if !defined(SKIP_27712T21)&&(!defined(ONLY)||defined(CASE_27712T21))
	begin_chk("_27712T21");
	{
	Cstr input((const Char *)"abc");
	Bss sbuf(input), sbuf1(input, STD ios_base::out),
		sbuf2(input, STD ios_base::in);
	chk(sbuf.sbumpc() == (Char)'a');
	chk(sbuf.sbumpc() == (Char)'b');
	chk(sbuf.sbumpc() == (Char)'c');
	chk(sbuf.sbumpc() == It::eof());
	chk(sbuf.sputc((Char)'x') == (Char)'x');
	sbuf.pubseekoff(-1, STD ios_base::end, STD ios_base::out);
	chk(sbuf.sputc((Char)'z') == (Char)'z');
	sbuf.pubseekoff(0, STD ios_base::beg, STD ios_base::in);
	chk(sbuf.sbumpc() == (Char)'x');
	chk(sbuf.sbumpc() == (Char)'b');
	chk(sbuf.sbumpc() == (Char)'z');
	chk(sbuf.sbumpc() == It::eof());


	}
	end_chk("_27712T21");
#else
	skip_chk("_27712T21");
#endif /* CASE_27712T21 */

/* _27712T41 basic_stringbuf::str(string) effects */
#if !defined(SKIP_27712T41)&&(!defined(ONLY)||defined(CASE_27712T41))
	begin_chk("_27712T41");
	{
	Cstr input((const Char *)"");
	Bss sbuf(input);
	chk(sbuf.sgetc() == It::eof());





	}
	end_chk("_27712T41");
#else
	skip_chk("_27712T41");
#endif /* CASE_27712T41 */

/* _27712T51 basic_stringbuf::str(string) postcondition */
#if !defined(SKIP_27712T51)&&(!defined(ONLY)||defined(CASE_27712T51))
	begin_chk("_27712T51");
	{
	Cstr input((const Char *)"abcdefg");
	Bss sbuf, sbuf1(STD ios_base::out), sbuf2(STD ios_base::in),
		sbuf3(STD ios_base::in | STD ios_base::out);
	sbuf.str(input);
	chk(sbuf.str() == input);
	sbuf1.str(input);
	sbuf1.pubseekoff(0, STD ios_base::end, STD ios_base::out);
	chk(sbuf1.str() == input);
	sbuf2.str(input);
	chk(sbuf2.str() == input);
	sbuf3.str(input);
	chk(sbuf3.str() == input);



	}
	end_chk("_27712T51");
#else
	skip_chk("_27712T51");
#endif /* CASE_27712T51 */

/* _27713T21 basic_stringbuf::underflow returns, *gptr() */
#if !defined(SKIP_27713T21)&&(!defined(ONLY)||defined(CASE_27713T21))
	begin_chk("_27713T21");
	{
	Cstr input((const Char *)"abcdefg");
	Mystringbuf_27713T21 sbuf;
	sbuf.str(input);
	chk(sbuf.Myunderflow() == It::to_int_type((Char)'a'));
	chk(sbuf.Myunderflow() == It::to_int_type((Char)'a'));


	}
	end_chk("_27713T21");
#else
	skip_chk("_27713T21");
#endif /* CASE_27713T21 */

/* _27713T22 basic_stringbuf::underflow returns, eof() */
#if !defined(SKIP_27713T22)&&(!defined(ONLY)||defined(CASE_27713T22))
	begin_chk("_27713T22");
	{
	Mystringbuf_27713T22 sbuf;
	chk(sbuf.Myunderflow() == It::eof());


	}
	end_chk("_27713T22");
#else
	skip_chk("_27713T22");
#endif /* CASE_27713T22 */

/* _27713T41 basic_stringbuf::pbackfail effects, --gptr() */
#if !defined(SKIP_27713T41)&&(!defined(ONLY)||defined(CASE_27713T41))
	begin_chk("_27713T41");
	{
	Cstr input((const Char *)"abcdefg");
	Mystringbuf_27713T41 sbuf(STD ios_base::in);
	sbuf.str(input);
	sbuf.sbumpc();
	sbuf.Mypbackfail(It::to_int_type((Char)'a'));
	chk(sbuf.sbumpc() == It::to_int_type((Char)'a'));
	chk(sbuf.sgetc() == It::to_int_type((Char)'b'));


	}
	end_chk("_27713T41");
#else
	skip_chk("_27713T41");
#endif /* CASE_27713T41 */

/* _27713T42 basic_stringbuf::pbackfail effects, return c after --gptr() */
#if !defined(SKIP_27713T42)&&(!defined(ONLY)||defined(CASE_27713T42))
	begin_chk("_27713T42");
	{
	Cstr input((const Char *)"abcdefg");
	Mystringbuf_27713T42 sbuf(STD ios_base::in);
	sbuf.str(input);
	sbuf.sbumpc();
	chk(sbuf.Mypbackfail(It::to_int_type((Char)'a'))
		== It::to_int_type((Char)'a'));


	}
	end_chk("_27713T42");
#else
	skip_chk("_27713T42");
#endif /* CASE_27713T42 */

/* _27713T43 basic_stringbuf::pbackfail effects, *--gptr() = c */
#if !defined(SKIP_27713T43)&&(!defined(ONLY)||defined(CASE_27713T43))
	begin_chk("_27713T43");
	{
	Cstr input((const Char *)"abcdefg");
	Mystringbuf_27713T43 sbuf;
	sbuf.str(input);
	sbuf.sbumpc();
	sbuf.Mypbackfail(It::to_int_type((Char)'x'));
	chk(sbuf.sbumpc() == It::to_int_type((Char)'x'));
	chk(sbuf.sgetc() == It::to_int_type((Char)'b'));



	}
	end_chk("_27713T43");
#else
	skip_chk("_27713T43");
#endif /* CASE_27713T43 */

/* _27713T44 basic_stringbuf::pbackfail effects, return c after *--gptr() = c */
#if !defined(SKIP_27713T44)&&(!defined(ONLY)||defined(CASE_27713T44))
	begin_chk("_27713T44");
	{
	Cstr input((const Char *)"abcdefg");
	Mystringbuf_27713T44 sbuf;
	sbuf.str(input);
	sbuf.sbumpc();
	chk(sbuf.Mypbackfail(It::to_int_type((Char)'x'))
		== It::to_int_type((Char)'x'));


	}
	end_chk("_27713T44");
#else
	skip_chk("_27713T44");
#endif /* CASE_27713T44 */

/* _27713T45 basic_stringbuf::pbackfail effects, c == eof() */
#if !defined(SKIP_27713T45)&&(!defined(ONLY)||defined(CASE_27713T45))
	begin_chk("_27713T45");
	{
	Cstr input((const Char *)"abcdefg");
	Mystringbuf_27713T45 sbuf(STD ios_base::in);
	sbuf.str(input);
	sbuf.sbumpc();
	sbuf.Mypbackfail(It::eof());
	chk(sbuf.sbumpc() == It::to_int_type((Char)'a'));
	chk(sbuf.sgetc() == It::to_int_type((Char)'b'));



	}
	end_chk("_27713T45");
#else
	skip_chk("_27713T45");
#endif /* CASE_27713T45 */

/* _27713T46 basic_stringbuf::pbackfail effects, return not_eof(c) */
#if !defined(SKIP_27713T46)&&(!defined(ONLY)||defined(CASE_27713T46))
	begin_chk("_27713T46");
	{
	Cstr input((const Char *)"abcdefg");
	Mystringbuf_27713T46 sbuf(STD ios_base::in);
	sbuf.str(input);
	sbuf.sbumpc();
	chk(sbuf.Mypbackfail(It::eof()) != It::eof());


	}
	end_chk("_27713T46");
#else
	skip_chk("_27713T46");
#endif /* CASE_27713T46 */

/* _27713T51 basic_stringbuf::pbackfail returns */
#if !defined(SKIP_27713T51)&&(!defined(ONLY)||defined(CASE_27713T51))
	begin_chk("_27713T51");
	{
	Cstr input((const Char *)"abcdefg");
	Mystringbuf_27713T51 sbuf(STD ios_base::in);
	sbuf.str(input);
	sbuf.sbumpc();
	chk(sbuf.Mypbackfail(It::to_int_type((Char)'x'))
		== It::eof());
	Mystringbuf_27713T51 sbufx(STD ios_base::in | STD ios_base::out);
	chk(sbufx.Mypbackfail(It::to_int_type((Char)'x'))
		== It::eof());


	}
	end_chk("_27713T51");
#else
	skip_chk("_27713T51");
#endif /* CASE_27713T51 */

/* _27713T71 basic_stringbuf::overflow effects, c != eof() */
#if !defined(SKIP_27713T71)&&(!defined(ONLY)||defined(CASE_27713T71))
	begin_chk("_27713T71");
	{
	Mystringbuf_27713T71 sbuf(STD ios_base::in | STD ios_base::out);
	sbuf.Myoverflow(It::to_int_type((Char)'x'));
	chk(sbuf.sgetc() == It::to_int_type((Char)'x'));


	}
	end_chk("_27713T71");
#else
	skip_chk("_27713T71");
#endif /* CASE_27713T71 */

/* _27713T72 basic_stringbuf::overflow effects, returns c after c != eof() */
#if !defined(SKIP_27713T72)&&(!defined(ONLY)||defined(CASE_27713T72))
	begin_chk("_27713T72");
	{
	Mystringbuf_27713T72 sbuf(STD ios_base::in | STD ios_base::out);
	chk(sbuf.Myoverflow(It::to_int_type((Char)'x'))
		== It::to_int_type((Char)'x'));


	}
	end_chk("_27713T72");
#else
	skip_chk("_27713T72");
#endif /* CASE_27713T72 */

/* _27713T73 basic_stringbuf::overflow effects, c == eof() */
#if !defined(SKIP_27713T73)&&(!defined(ONLY)||defined(CASE_27713T73))
	begin_chk("_27713T73");
	{
	Mystringbuf_27713T73 sbuf(STD ios_base::in | STD ios_base::out);
	sbuf.Myoverflow(It::eof());
	chk(sbuf.sgetc() == It::eof());


	}
	end_chk("_27713T73");
#else
	skip_chk("_27713T73");
#endif /* CASE_27713T73 */

/* _27713T74 basic_stringbuf::overflow effects, returns not_eof(c) */
#if !defined(SKIP_27713T74)&&(!defined(ONLY)||defined(CASE_27713T74))
	begin_chk("_27713T74");
	{
	Mystringbuf_27713T74 sbuf(STD ios_base::in | STD ios_base::out);
	chk(sbuf.Myoverflow(It::eof()) != It::eof());


	}
	end_chk("_27713T74");
#else
	skip_chk("_27713T74");
#endif /* CASE_27713T74 */

/* _27713T91 basic_stringbuf::overflow returns */
#if !defined(SKIP_27713T91)&&(!defined(ONLY)||defined(CASE_27713T91))
	begin_chk("_27713T91");
	{
	Mystringbuf_27713T91 sbuf(STD ios_base::in);
	chk(sbuf.Myoverflow(It::to_int_type((Char)'x')) == It::eof());

	}
	end_chk("_27713T91");
#else
	skip_chk("_27713T91");
#endif /* CASE_27713T91 */

return leave_chk("t27f");
}
/* V3.10:0009 */
