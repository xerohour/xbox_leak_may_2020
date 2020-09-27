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
#define CASE_2781T11	<fstream> synopsis
#define CASE_27811T11	basic_filebuf definition
#define CASE_27811T31	basic_filebuf restrictions, must open for reading to read
#define CASE_27811T32	basic_filebuf restrictions, must open for writing to write
#define CASE_27811T33	basic_filebuf restrictions, streams positioned jointly
//#define CASE_27811T42	basic_filebuf restrictions, codecvt converts input
//#define CASE_27811T43	basic_filebuf restrictions, codecvt converts output
#define CASE_27812T21	basic_filebuf::basic_filebuf effects
#define CASE_27812T31	basic_filebuf::basic_filebuf postcondition
#define CASE_27812T52	basic_filebuf::~basic_filebuf calls close
#define CASE_27813T21	basic_filebuf::is_open returns
#define CASE_27813T23	basic_filebuf::open effects, is_open() is true
#define CASE_27813T24	basic_filebuf::open effects, open modes
#define CASE_27813T31	basic_filebuf::open effects, positions at end
#define CASE_27813T41	basic_filebuf::open effects, positioning fails
#define CASE_27813T51	basic_filebuf::open returns
#define CASE_27813T71	basic_filebuf::close effects, is_open() == false
#define CASE_27813T72	basic_filebuf::close effects, calls fclose
#define CASE_27813T81	basic_filebuf::close returns
#define CASE_27813T91	basic_filebuf::close postcondition
#endif /* IS_EMBEDDED */

/* _2781T11 <fstream> synopsis */
#if !defined(SKIP_2781T11)&&(!defined(ONLY)||defined(CASE_2781T11))
#include <fstream>
#endif /* CASE_2781T11 */

/* _27811T11 basic_filebuf definition */
#if !defined(SKIP_27811T11)&&(!defined(ONLY)||defined(CASE_27811T11))
#include <fstream>
#endif /* CASE_27811T11 */

/* _27811T31 basic_filebuf restrictions, must open for reading to read */
#if !defined(SKIP_27811T31)&&(!defined(ONLY)||defined(CASE_27811T31))
#include <fstream>
#endif /* CASE_27811T31 */

/* _27811T32 basic_filebuf restrictions, must open for writing to write */
#if !defined(SKIP_27811T32)&&(!defined(ONLY)||defined(CASE_27811T32))
#include <fstream>
#endif /* CASE_27811T32 */

/* _27811T33 basic_filebuf restrictions, streams positioned jointly */
#if !defined(SKIP_27811T33)&&(!defined(ONLY)||defined(CASE_27811T33))
#include <fstream>
#endif /* CASE_27811T33 */

/* _27811T42 basic_filebuf restrictions, codecvt converts input */
#if !defined(SKIP_27811T42)&&(!defined(ONLY)||defined(CASE_27811T42))
#include <cctype>
#include <fstream>
#include <locale>
typedef codecvt<char, char, mbstate_t> Cvtcc_27811T42;
int cnt_27811T42;
class Mycvtcc_27811T42 : public Cvtcc_27811T42 {
protected:
	virtual result do_in(mbstate_t& st,	// ADDED [JUL96]
		const char *f1, const char *l1, const char *& m1,
		char *f2, char *l2, char *& m2) const
		{return (do_out(st, f1, l1, m1, f2, l2, m2)); }
	virtual result do_out(mbstate_t&,
		const char *f1, const char *l1, const char *& m1,
		char *f2, char *l2, char *& m2) const
		{m1 = f1, m2 = f2;
		cnt_27811T42 |= 1;
		if (m1 == l1 || m2 == l2)
			return (partial);
		*m2++ = *m1++;
		return (ok); }
	virtual bool do_always_noconv() const THROW0()
		{return (false); }
	};
#endif /* CASE_27811T42 */

/* _27811T43 basic_filebuf restrictions, codecvt converts output */
#if !defined(SKIP_27811T43)&&(!defined(ONLY)||defined(CASE_27811T43))
#include <cctype>
#include <fstream>
#include <locale>
typedef codecvt<char, char, mbstate_t> Cvtcc_27811T43;
int cnt_27811T43;
class Mycvtcc_27811T43 : public Cvtcc_27811T43 {
protected:
	virtual result do_in(mbstate_t& st,	// ADDED [JUL96]
		const char *f1, const char *l1, const char *& m1,
		char *f2, char *l2, char *& m2) const
		{return (do_out(st, f1, l1, m1, f2, l2, m2)); }
	virtual result do_out(mbstate_t&,
		const char *f1, const char *l1, const char *& m1,
		char *f2, char *l2, char *& m2) const
		{m1 = f1, m2 = f2;
		cnt_27811T43 |= 1;
		if (m1 == l1 || m2 == l2)
			return (partial);
		*m2++ = *m1++;
		return (ok); }
	virtual bool do_always_noconv() const THROW0()
		{return (false); }
	};
#endif /* CASE_27811T43 */

/* _27812T21 basic_filebuf::basic_filebuf effects */
#if !defined(SKIP_27812T21)&&(!defined(ONLY)||defined(CASE_27812T21))
#include <fstream>
#endif /* CASE_27812T21 */

/* _27812T31 basic_filebuf::basic_filebuf postcondition */
#if !defined(SKIP_27812T31)&&(!defined(ONLY)||defined(CASE_27812T31))
#include <fstream>
#endif /* CASE_27812T31 */

/* _27812T52 basic_filebuf::~basic_filebuf calls close */
#if !defined(SKIP_27812T52)&&(!defined(ONLY)||defined(CASE_27812T52))
#include <fstream>
#endif /* CASE_27812T52 */

/* _27813T21 basic_filebuf::is_open returns */
#if !defined(SKIP_27813T21)&&(!defined(ONLY)||defined(CASE_27813T21))
#include <fstream>
#endif /* CASE_27813T21 */

/* _27813T23 basic_filebuf::open effects, is_open() is true */
#if !defined(SKIP_27813T23)&&(!defined(ONLY)||defined(CASE_27813T23))
#include <fstream>
#endif /* CASE_27813T23 */

/* _27813T24 basic_filebuf::open effects, open modes */
#if !defined(SKIP_27813T24)&&(!defined(ONLY)||defined(CASE_27813T24))
#include <fstream>
#endif /* CASE_27813T24 */

/* _27813T31 basic_filebuf::open effects, positions at end */
#if !defined(SKIP_27813T31)&&(!defined(ONLY)||defined(CASE_27813T31))
#include <fstream>
#endif /* CASE_27813T31 */

/* _27813T41 basic_filebuf::open effects, positioning fails */
#if !defined(SKIP_27813T41)&&(!defined(ONLY)||defined(CASE_27813T41))
#include <fstream>
#endif /* CASE_27813T41 */

/* _27813T51 basic_filebuf::open returns */
#if !defined(SKIP_27813T51)&&(!defined(ONLY)||defined(CASE_27813T51))
#include <fstream>
#endif /* CASE_27813T51 */

/* _27813T71 basic_filebuf::close effects, is_open() == false */
#if !defined(SKIP_27813T71)&&(!defined(ONLY)||defined(CASE_27813T71))
#include <fstream>
#endif /* CASE_27813T71 */

/* _27813T72 basic_filebuf::close effects, calls fclose */
#if !defined(SKIP_27813T72)&&(!defined(ONLY)||defined(CASE_27813T72))
#include <cstdio>
#include <fstream>
#endif /* CASE_27813T72 */

/* _27813T81 basic_filebuf::close returns */
#if !defined(SKIP_27813T81)&&(!defined(ONLY)||defined(CASE_27813T81))
#include <fstream>
#endif /* CASE_27813T81 */

/* _27813T91 basic_filebuf::close postcondition */
#if !defined(SKIP_27813T91)&&(!defined(ONLY)||defined(CASE_27813T91))
#include <fstream>
#endif /* CASE_27813T91 */

int t27j_main(int, char *[])
{
enter_chk("t27j.cpp");
/* _2781T11 <fstream> synopsis */
#if !defined(SKIP_2781T11)&&(!defined(ONLY)||defined(CASE_2781T11))
	begin_chk("_2781T11");
	{
	#if IS_EMBEDDED
	filebuf *p1 = 0; TOUCH(p1);
	ifstream *p3 = 0; TOUCH(p3);
	ofstream *p5 = 0; TOUCH(p5);
	chk(1);
	#else /* IS_EMBEDDED */
	basic_filebuf<char, char_traits<char> > *p1 = 0; TOUCH(p1);
	filebuf *p2 = p1; TOUCH(p2);
	basic_ifstream<char, char_traits<char> > *p3 = 0; TOUCH(p3);
	ifstream *p4 = p3; TOUCH(p4);
	basic_ofstream<char, char_traits<char> > *p5 = 0; TOUCH(p5);
	ofstream *p6 = p5; TOUCH(p6);
	basic_filebuf<wchar_t, char_traits<wchar_t> > *pw1 = 0; TOUCH(pw1);
	wfilebuf *pw2 = pw1; TOUCH(pw2);
	basic_ifstream<wchar_t, char_traits<wchar_t> > *pw3 = 0; TOUCH(pw3);
	wifstream *pw4 = pw3; TOUCH(pw4);
	basic_ofstream<wchar_t, char_traits<wchar_t> > *pw5 = 0; TOUCH(pw5);
	wofstream *pw6 = pw5; TOUCH(pw6);

	fstream *p7 = (basic_fstream<char, char_traits<char> > *)0; TOUCH(p7);
	wfstream *p8 = (basic_fstream<wchar_t, char_traits<wchar_t> > *)0; TOUCH(p8);

	typedef STD basic_fstream<Char, It> Iostr;	// ADDED [MAR96]
	Iostr::traits_type *ptt = (It *)0; TOUCH(ptt);	// ADDED [JUL96]
	Iostr::char_type *p9 = (Char *)0; TOUCH(p9);
	Iostr::int_type *p10 = 0; TOUCH(p10);
	Iostr::pos_type *p11 = 0; TOUCH(p11);
	Iostr::off_type *p12 = 0; TOUCH(p12);

	const char *tn = "d:\\abc"; // STD tmpnam(0);
	int i = 37;
		{Iostr iostr1(tn, ios_base::out | ios_base::trunc);
		chk(iostr1.is_open());
		iostr1 << i << endl; }
	Iostr iostr;
	iostr.open(tn);
	chk(iostr.is_open());
	iostr.rdbuf()->pubseekoff(0, ios_base::cur);
	i = 0;
	iostr >> i;
	ieq(i, 37);

	iostr.close();
	STD remove(tn);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_2781T11");
#else
	skip_chk("_2781T11");
#endif /* CASE_2781T11 */

/* _27811T11 basic_filebuf definition */
#if !defined(SKIP_27811T11)&&(!defined(ONLY)||defined(CASE_27811T11))
	begin_chk("_27811T11");
	{
	Bfs fbuf;
	Bs *p = &fbuf; TOUCH(p);
	Bfs::traits_type *ptt = (It *)0; TOUCH(ptt);	// ADDED [JUL96]
	Bfs::char_type *pct = (Char *)0; TOUCH(pct);
	Bfs::int_type *pit = (It::int_type *)0; TOUCH(pit);
	Bfs::pos_type *pst = (It::pos_type *)0; TOUCH(pst);
	Bfs::off_type *pot = (It::off_type *)0; TOUCH(pot);
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.is_open());
	fbuf.close();
	STD remove(tn);



	}
	end_chk("_27811T11");
#else
	skip_chk("_27811T11");
#endif /* CASE_27811T11 */

/* _27811T31 basic_filebuf restrictions, must open for reading to read */
#if !defined(SKIP_27811T31)&&(!defined(ONLY)||defined(CASE_27811T31))
	begin_chk("_27811T31");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.sputc(It::to_int_type((Char)'a')) == It::to_int_type((Char)'a'));
	chk(fbuf.sputc(It::to_int_type((Char)'b')) == It::to_int_type((Char)'b'));
	chk(fbuf.sputc(It::to_int_type((Char)'\n')) == It::to_int_type((Char)'\n'));
	fbuf.close();
	fbuf.open(tn, STD ios_base::out | STD ios_base::app);
	chk(fbuf.sgetc() == It::eof());
	fbuf.close();
	STD remove(tn);


	}
	end_chk("_27811T31");
#else
	skip_chk("_27811T31");
#endif /* CASE_27811T31 */

/* _27811T32 basic_filebuf restrictions, must open for writing to write */
#if !defined(SKIP_27811T32)&&(!defined(ONLY)||defined(CASE_27811T32))
	begin_chk("_27811T32");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.sputc(It::to_int_type((Char)'a')) == It::to_int_type((Char)'a'));
	chk(fbuf.sputc(It::to_int_type((Char)'b')) == It::to_int_type((Char)'b'));
	chk(fbuf.sputc(It::to_int_type((Char)'\n')) == It::to_int_type((Char)'\n'));
	fbuf.close();
	fbuf.open(tn, STD ios_base::in);
	chk(fbuf.sputc(It::to_int_type((Char)'a')) == It::eof());
	fbuf.close();
	STD remove(tn);


	}
	end_chk("_27811T32");
#else
	skip_chk("_27811T32");
#endif /* CASE_27811T32 */

/* _27811T33 basic_filebuf restrictions, streams positioned jointly */
#if !defined(SKIP_27811T33)&&(!defined(ONLY)||defined(CASE_27811T33))
	begin_chk("_27811T33");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::binary | STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.sputc(It::to_int_type((Char)'a')) == It::to_int_type((Char)'a'));
	chk(fbuf.sputc(It::to_int_type((Char)'b')) == It::to_int_type((Char)'b'));
	chk(fbuf.sputc(It::to_int_type((Char)'\n')) == It::to_int_type((Char)'\n'));
	fbuf.close();
	fbuf.open(tn, STD ios_base::binary | STD ios_base::in | STD ios_base::out);
	fbuf.pubseekoff(1, STD ios_base::beg, STD ios_base::out);
	chk(fbuf.sgetc() == It::to_int_type((Char)'b'));
	fbuf.pubseekoff(0, STD ios_base::beg, STD ios_base::in);
	chk(fbuf.sputc(It::to_int_type((Char)'x')) == It::to_int_type((Char)'x'));
	fbuf.pubseekoff(0, STD ios_base::beg, STD ios_base::out);
	chk(fbuf.sgetc() == It::to_int_type((Char)'x'));
	fbuf.close();
	STD remove(tn);


	}
	end_chk("_27811T33");
#else
	skip_chk("_27811T33");
#endif /* CASE_27811T33 */

/* _27811T42 basic_filebuf restrictions, codecvt converts input */
#if !defined(SKIP_27811T42)&&(!defined(ONLY)||defined(CASE_27811T42))
	begin_chk("_27811T42");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.sputc(It::to_int_type((Char)'a')) == It::to_int_type((Char)'a'));
	chk(fbuf.sputc(It::to_int_type((Char)'b')) == It::to_int_type((Char)'b'));
	chk(fbuf.sputc(It::to_int_type((Char)'\n')) == It::to_int_type((Char)'\n'));
	fbuf.close();
	Bfs fbufx;
	fbufx.pubimbue(ADDFAC(STD locale::classic(), new Mycvtcc_27811T42));
	cnt_27811T42 = 0;
	fbufx.open(tn, STD ios_base::in);
	fbufx.sbumpc();
	ieq(cnt_27811T42, 1);
	fbuf.close();
	STD remove(tn);


	}
	end_chk("_27811T42");
#else
	skip_chk("_27811T42");
#endif /* CASE_27811T42 */

/* _27811T43 basic_filebuf restrictions, codecvt converts output */
#if !defined(SKIP_27811T43)&&(!defined(ONLY)||defined(CASE_27811T43))
	begin_chk("_27811T43");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.pubimbue(ADDFAC(STD locale::classic(), new Mycvtcc_27811T43));
	cnt_27811T43 = 0;
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.sputc(It::to_int_type((Char)'a')) == It::to_int_type((Char)'a'));
	chk(fbuf.sputc(It::to_int_type((Char)'b')) == It::to_int_type((Char)'b'));
	chk(fbuf.sputc(It::to_int_type((Char)'\n')) == It::to_int_type((Char)'\n'));
	fbuf.close();
	ieq(cnt_27811T43, 1);
	STD remove(tn);


	}
	end_chk("_27811T43");
#else
	skip_chk("_27811T43");
#endif /* CASE_27811T43 */

/* _27812T21 basic_filebuf::basic_filebuf effects */
#if !defined(SKIP_27812T21)&&(!defined(ONLY)||defined(CASE_27812T21))
	begin_chk("_27812T21");
	{
	Bfs fbuf;
	chk(fbuf.sgetc() == It::eof());
	chk(fbuf.getloc() == locale());


	}
	end_chk("_27812T21");
#else
	skip_chk("_27812T21");
#endif /* CASE_27812T21 */

/* _27812T31 basic_filebuf::basic_filebuf postcondition */
#if !defined(SKIP_27812T31)&&(!defined(ONLY)||defined(CASE_27812T31))
	begin_chk("_27812T31");
	{
	Bfs fbuf;
	chk(!fbuf.is_open());


	}
	end_chk("_27812T31");
#else
	skip_chk("_27812T31");
#endif /* CASE_27812T31 */

/* _27812T52 basic_filebuf::~basic_filebuf calls close */
#if !defined(SKIP_27812T52)&&(!defined(ONLY)||defined(CASE_27812T52))
	begin_chk("_27812T52");
	{
	Bfs *p = new Bfs;
	#if IS_EMBEDDED
	p->DESTRUCTOR(filebuf, Bfs);
	#else /* IS_EMBEDDED */
	p->DESTRUCTOR(basic_filebuf, Bfs);
	#endif /* IS_EMBEDDED */
	chk(1);



	}
	end_chk("_27812T52");
#else
	skip_chk("_27812T52");
#endif /* CASE_27812T52 */

/* _27813T21 basic_filebuf::is_open returns */
#if !defined(SKIP_27813T21)&&(!defined(ONLY)||defined(CASE_27813T21))
	begin_chk("_27813T21");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	chk(!fbuf.is_open());
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.is_open());
	fbuf.close();
	chk(!fbuf.is_open());
	STD remove(tn);



	}
	end_chk("_27813T21");
#else
	skip_chk("_27813T21");
#endif /* CASE_27813T21 */

/* _27813T23 basic_filebuf::open effects, is_open() is true */
#if !defined(SKIP_27813T23)&&(!defined(ONLY)||defined(CASE_27813T23))
	begin_chk("_27813T23");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.open(STD tmpnam(0), STD ios_base::out | STD ios_base::trunc) == 0);
	fbuf.close();
	STD remove(tn);





	}
	end_chk("_27813T23");
#else
	skip_chk("_27813T23");
#endif /* CASE_27813T23 */

/* _27813T24 basic_filebuf::open effects, open modes */
#if !defined(SKIP_27813T24)&&(!defined(ONLY)||defined(CASE_27813T24))
	begin_chk("_27813T24");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.is_open());
	fbuf.sputc(It::to_int_type((Char)'a'));
	fbuf.sputc(It::to_int_type((Char)'b'));
	fbuf.sputc(It::to_int_type((Char)'\n'));
	fbuf.close();
	fbuf.open(tn, STD ios_base::in);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::out | STD ios_base::app);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::in | STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.is_open());
	fbuf.sputc(It::to_int_type((Char)'a'));
	fbuf.sputc(It::to_int_type((Char)'b'));
	fbuf.sputc(It::to_int_type((Char)'\n'));
	fbuf.close();
	fbuf.open(tn, STD ios_base::in | STD ios_base::out);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::in | STD ios_base::out | STD ios_base::app);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::out
		| STD ios_base::binary);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc
		| STD ios_base::binary);
	chk(fbuf.is_open());
	fbuf.sputc(It::to_int_type((Char)'a'));
	fbuf.sputc(It::to_int_type((Char)'b'));
	fbuf.sputc(It::to_int_type((Char)'\n'));
	fbuf.close();
	fbuf.open(tn, STD ios_base::in
		| STD ios_base::binary);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::out | STD ios_base::app
		| STD ios_base::binary);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::in | STD ios_base::out | STD ios_base::trunc
		| STD ios_base::binary);
	chk(fbuf.is_open());
	fbuf.sputc(It::to_int_type((Char)'a'));
	fbuf.sputc(It::to_int_type((Char)'b'));
	fbuf.sputc(It::to_int_type((Char)'\n'));
	fbuf.close();
	fbuf.open(tn, STD ios_base::in | STD ios_base::out
		| STD ios_base::binary);
	chk(fbuf.is_open());
	fbuf.close();
	fbuf.open(tn, STD ios_base::in | STD ios_base::out | STD ios_base::app
		| STD ios_base::binary);
	chk(fbuf.is_open());
	fbuf.close();
	STD remove(tn);


	}
	end_chk("_27813T24");
#else
	skip_chk("_27813T24");
#endif /* CASE_27813T24 */

/* _27813T31 basic_filebuf::open effects, positions at end */
#if !defined(SKIP_27813T31)&&(!defined(ONLY)||defined(CASE_27813T31))
	begin_chk("_27813T31");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	fbuf.sputc(It::to_int_type((Char)'a'));
	fbuf.sputc(It::to_int_type((Char)'b'));
	fbuf.sputc(It::to_int_type((Char)'\n'));
	fbuf.close();
	fbuf.open(tn, STD ios_base::in | STD ios_base::ate);
	chk(fbuf.is_open());
	chk(fbuf.sgetc() == It::eof());
	fbuf.pubseekoff(0, STD ios_base::beg);
	chk(fbuf.sgetc() == It::to_int_type((Char)'a'));
	fbuf.close();
	STD remove(tn);


	}
	end_chk("_27813T31");
#else
	skip_chk("_27813T31");
#endif /* CASE_27813T31 */

/* _27813T41 basic_filebuf::open effects, positioning fails */
#if !defined(SKIP_27813T41)&&(!defined(ONLY)||defined(CASE_27813T41))
	begin_chk("_27813T41");
	{
	// UNTESTABLE
	chk(1);


	}
	end_chk("_27813T41");
#else
	skip_chk("_27813T41");
#endif /* CASE_27813T41 */

/* _27813T51 basic_filebuf::open returns */
#if !defined(SKIP_27813T51)&&(!defined(ONLY)||defined(CASE_27813T51))
	begin_chk("_27813T51");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	chk(fbuf.open(tn, STD ios_base::out | STD ios_base::trunc) == &fbuf);
	chk(fbuf.open(STD tmpnam(0),
		STD ios_base::out | STD ios_base::trunc) == 0);
	fbuf.close();
	STD remove(tn);


	}
	end_chk("_27813T51");
#else
	skip_chk("_27813T51");
#endif /* CASE_27813T51 */

/* _27813T71 basic_filebuf::close effects, is_open() == false */
#if !defined(SKIP_27813T71)&&(!defined(ONLY)||defined(CASE_27813T71))
	begin_chk("_27813T71");
	{
	Bfs fbuf;
	chk(fbuf.close() == 0);


	}
	end_chk("_27813T71");
#else
	skip_chk("_27813T71");
#endif /* CASE_27813T71 */

/* _27813T72 basic_filebuf::close effects, calls fclose */
#if !defined(SKIP_27813T72)&&(!defined(ONLY)||defined(CASE_27813T72))
	begin_chk("_27813T72");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	chk(fbuf.open(tn, STD ios_base::out | STD ios_base::trunc) == &fbuf);
	fbuf.close();
	STD FILE *pf = STD fopen(tn, "w");
	chk(pf != 0);
	fclose(pf);
	STD remove(tn);


	}
	end_chk("_27813T72");
#else
	skip_chk("_27813T72");
#endif /* CASE_27813T72 */

/* _27813T81 basic_filebuf::close returns */
#if !defined(SKIP_27813T81)&&(!defined(ONLY)||defined(CASE_27813T81))
	begin_chk("_27813T81");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	chk(fbuf.close() == &fbuf);
	chk(fbuf.close() == 0);
	STD remove(tn);


	}
	end_chk("_27813T81");
#else
	skip_chk("_27813T81");
#endif /* CASE_27813T81 */

/* _27813T91 basic_filebuf::close postcondition */
#if !defined(SKIP_27813T91)&&(!defined(ONLY)||defined(CASE_27813T91))
	begin_chk("_27813T91");
	{
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, STD ios_base::out | STD ios_base::trunc);
	fbuf.close();
	chk(!fbuf.is_open());
	STD remove(tn);

	}
	end_chk("_27813T91");
#else
	skip_chk("_27813T91");
#endif /* CASE_27813T91 */

return leave_chk("t27j");
}
/* V3.10:0009 */
