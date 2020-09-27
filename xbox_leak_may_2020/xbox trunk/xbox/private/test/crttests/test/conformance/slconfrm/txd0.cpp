/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _XD1T11 <*.h> headers are present */
#if !defined(SKIP_XD1T11)&&(!defined(ONLY)||defined(CASE_XD1T11))
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if IS_EMBEDDED
#else /* IS_EMBEDDED */
#include <iso646.h>
#include <wchar.h>
#include <wctype.h>
#endif /* IS_EMBEDDED */
#endif /* CASE_XD1T11 */

/* _XD1T21 <*.h> headers include <c*>, plus using */
#if !defined(SKIP_XD1T21)&&(!defined(ONLY)||defined(CASE_XD1T21))
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if IS_EMBEDDED
#else /* IS_EMBEDDED */
#include <iso646.h>
#include <wchar.h>
#include <wctype.h>
#endif /* IS_EMBEDDED */
#endif /* CASE_XD1T21 */

/* _XD2T11 ios_base defines io_state, open_mode, seek_dir */
#if !defined(SKIP_XD2T11)&&(!defined(ONLY)||defined(CASE_XD2T11))
#include <ios>
#endif /* CASE_XD2T11 */

/* _XD2T21 io_state is an integer type */
#if !defined(SKIP_XD2T21)&&(!defined(ONLY)||defined(CASE_XD2T21))
#include <ios>
#endif /* CASE_XD2T21 */

/* _XD2T31 open_mode is an integer type */
#if !defined(SKIP_XD2T31)&&(!defined(ONLY)||defined(CASE_XD2T31))
#include <ios>
#endif /* CASE_XD2T31 */

/* _XD2T41 seek_dir is an integer type */
#if !defined(SKIP_XD2T41)&&(!defined(ONLY)||defined(CASE_XD2T41))
#include <ios>
#endif /* CASE_XD2T41 */

/* _XD2T61 overloads on io_state, open_mode, seek_dir, declaration */
#if !defined(SKIP_XD2T61)&&(!defined(ONLY)||defined(CASE_XD2T61))
#include <fstream>
#include <ios>
#include <iostream>
#include <streambuf>
#if IS_EMBEDDED
typedef char_traits It_XD2T61;
typedef streambuf Bs_XD2T61;
#else /* IS_EMBEDDED */
typedef STD char_traits<char> It_XD2T61;
typedef STD basic_streambuf<char, It_XD2T61> Bs_XD2T61;
#endif /* IS_EMBEDDED */
class Mybuf_XD2T61 : public Bs_XD2T61 {
	};
#endif /* CASE_XD2T61 */

/* _XD31T11 strstreambuf definition */
#if !defined(SKIP_XD31T11)&&(!defined(ONLY)||defined(CASE_XD31T11))
#include <strstream>
#endif /* CASE_XD31T11 */

/* _XD311T21 strstreambuf::strstreambuf(streamsize) effects */
#if !defined(SKIP_XD311T21)&&(!defined(ONLY)||defined(CASE_XD311T21))
#include <strstream>
#endif /* CASE_XD311T21 */

/* _XD311T31 strstreambuf::strstreambuf(streamsize) postconditions */
#if !defined(SKIP_XD311T31)&&(!defined(ONLY)||defined(CASE_XD311T31))
#include <strstream>
#endif /* CASE_XD311T31 */

/* _XD311T51 strstreambuf::strstreambuf(palloc, pfree) effects */
#if !defined(SKIP_XD311T51)&&(!defined(ONLY)||defined(CASE_XD311T51))
#include <cstdlib>
#include <strstream>
static int cnt1_XD311T51;
static int cnt2_XD311T51;
void *salloc_XD311T51(size_t n)
	{++cnt1_XD311T51;
	return (STD malloc(n)); }
void sfree_XD311T51(void *p)
	{++cnt2_XD311T51;
	STD free(p); }
#endif /* CASE_XD311T51 */

/* _XD311T61 strstreambuf::strstreambuf(palloc, pfree) postconditions */
#if !defined(SKIP_XD311T61)&&(!defined(ONLY)||defined(CASE_XD311T61))
#include <cstdlib>
#include <strstream>
static int cnt1_XD311T61;
static int cnt2_XD311T61;
void *salloc_XD311T61(size_t n)
	{++cnt1_XD311T61;
	return (STD malloc(n)); }
void sfree_XD311T61(void *p)
	{++cnt2_XD311T61;
	STD free(p); }
#endif /* CASE_XD311T61 */

/* _XD311T81 strstreambuf::strstreambuf(char *) effects */
#if !defined(SKIP_XD311T81)&&(!defined(ONLY)||defined(CASE_XD311T81))
#include <strstream>
#endif /* CASE_XD311T81 */

/* _XD311T91 strstreambuf::strstreambuf(char *) postconditions */
#if !defined(SKIP_XD311T91)&&(!defined(ONLY)||defined(CASE_XD311T91))
#include <strstream>
#endif /* CASE_XD311T91 */

/* _XD311T_111 strstreambuf::strstreambuf(char *), pbeg_arg is null */
#if !defined(SKIP_XD311T_111)&&(!defined(ONLY)||defined(CASE_XD311T_111))
#include <cstring>
#include <strstream>
#endif /* CASE_XD311T_111 */

/* _XD311T_121 strstreambuf::strstreambuf(char *), pbeg_arg not null */
#if !defined(SKIP_XD311T_121)&&(!defined(ONLY)||defined(CASE_XD311T_121))
#include <cstdlib>
#include <strstream>
#endif /* CASE_XD311T_121 */

/* _XD311T_141 strstreambuf::strstreambuf(const char *), effects */
#if !defined(SKIP_XD311T_141)&&(!defined(ONLY)||defined(CASE_XD311T_141))
#include <cstring>
#include <strstream>
#endif /* CASE_XD311T_141 */

/* _XD311T_161 strstreambuf::~strstreambuf() frees object */
#if !defined(SKIP_XD311T_161)&&(!defined(ONLY)||defined(CASE_XD311T_161))
#include <cstdlib>
#include <strstream>
static int cnt1_XD311T_161;
static int cnt2_XD311T_161;
void *salloc_XD311T_161(size_t n)
	{++cnt1_XD311T_161;
	return (STD malloc(n)); }
void sfree_XD311T_161(void *p)
	{++cnt2_XD311T_161;
	STD free(p); }
#endif /* CASE_XD311T_161 */

int txd0_main(int, char *[])
{
enter_chk("txd0.cpp");
/* _XD1T11 <*.h> headers are present */
#if !defined(SKIP_XD1T11)&&(!defined(ONLY)||defined(CASE_XD1T11))
	begin_chk("_XD1T11");
	{
	int n = strlen("abc");
	chk(1);


	}
	end_chk("_XD1T11");
#else
	skip_chk("_XD1T11");
#endif /* CASE_XD1T11 */

/* _XD1T21 <*.h> headers include <c*>, plus using */
#if !defined(SKIP_XD1T21)&&(!defined(ONLY)||defined(CASE_XD1T21))
	begin_chk("_XD1T21");
	{
	int n = STD strlen("abc");
	chk(1);


	}
	end_chk("_XD1T21");
#else
	skip_chk("_XD1T21");
#endif /* CASE_XD1T21 */

/* _XD2T11 ios_base defines io_state, open_mode, seek_dir */
#if !defined(SKIP_XD2T11)&&(!defined(ONLY)||defined(CASE_XD2T11))
	begin_chk("_XD2T11");
	{
	ios_base::io_state s1 = ios_base::failbit; TOUCH(s1);
	ios_base::open_mode s2 = ios_base::trunc; TOUCH(s2);
	ios_base::seek_dir s3 = ios_base::cur; TOUCH(s3);
	chk(1);


	}
	end_chk("_XD2T11");
#else
	skip_chk("_XD2T11");
#endif /* CASE_XD2T11 */

/* _XD2T21 io_state is an integer type */
#if !defined(SKIP_XD2T21)&&(!defined(ONLY)||defined(CASE_XD2T21))
	begin_chk("_XD2T21");
	{
	ios_base::io_state s1 = 37;
	chk(s1 / 12 == 3);


	}
	end_chk("_XD2T21");
#else
	skip_chk("_XD2T21");
#endif /* CASE_XD2T21 */

/* _XD2T31 open_mode is an integer type */
#if !defined(SKIP_XD2T31)&&(!defined(ONLY)||defined(CASE_XD2T31))
	begin_chk("_XD2T31");
	{
	ios_base::open_mode s1 = 37;
	chk(s1 / 12 == 3);


	}
	end_chk("_XD2T31");
#else
	skip_chk("_XD2T31");
#endif /* CASE_XD2T31 */

/* _XD2T41 seek_dir is an integer type */
#if !defined(SKIP_XD2T41)&&(!defined(ONLY)||defined(CASE_XD2T41))
	begin_chk("_XD2T41");
	{
	ios_base::seek_dir s1 = 37;
	chk(s1 / 12 == 3);


	//### _XD2T51 basic_streambuf::stossc -- DELETED as optional
	//FILE-SCOPE:
	//#include <streambuf>
	//#if IS_EMBEDDED
	//typedef char_traits It_XD2T41;
	//typedef streambuf Bs_XD2T41;
	//#else /* IS_EMBEDDED */
	//typedef STD char_traits<char> It_XD2T41;
	//typedef STD basic_streambuf<char, It_XD2T41> Bs_XD2T41;
	//#endif /* IS_EMBEDDED */
	//char a_XD2T41[5];
	//class Mybuf_XD2T41 : public Bs_XD2T41 {
	//public:
	//	void pubsetg(char *beg, char *cur, char *end)
	//		{setg(beg, cur, end); }
	//	};
	//BLOCK-SCOPE:
	//Mybuf_XD2T41 buf;
	//a_XD2T41[0] = 'a';
	//a_XD2T41[1] = 'b';
	//buf.pubsetg(&a_XD2T41[0], &a_XD2T41[0], &a_XD2T41[5]);
	//buf.stossc();
	//ieq(buf.sgetc(), a_XD2T41[1]);
	//

	}
	end_chk("_XD2T41");
#else
	skip_chk("_XD2T41");
#endif /* CASE_XD2T41 */

/* _XD2T61 overloads on io_state, open_mode, seek_dir, declaration */
#if !defined(SKIP_XD2T61)&&(!defined(ONLY)||defined(CASE_XD2T61))
	begin_chk("_XD2T61");
	{
	typedef char Char;	// override flexible character type for strstream only
	Bi x(STD cin.rdbuf());
	x.clear((Bi::io_state)Bi::failbit);
	chk(x.rdstate() == Bi::failbit);
	x.setstate((Bi::io_state)Bi::badbit);
	chk(x.rdstate() == (Bi::badbit | Bi::failbit));
	x.clear();
	x.exceptions((Bi::io_state)Bi::badbit);
	chk(x.exceptions() == Bi::badbit);
	Mybuf_XD2T61 buf;
	Bs_XD2T61::pos_type pt = buf.pubseekoff((Bs_XD2T61::off_type)5,
		(STD ios_base::seek_dir)ios_base::cur,
		(STD ios_base::open_mode)ios_base::in);
	pt = buf.pubseekpos(pt, (ios_base::open_mode)ios_base::in);
	Bfs fbuf;
	const char *tn = STD tmpnam(0);
	fbuf.open(tn, (STD ios_base::open_mode)(STD ios_base::out
		| STD ios_base::trunc));
	fbuf.close();
	Ifs istr;
	istr.open(tn, (STD ios_base::open_mode)(STD ios_base::out));
	istr.close();
	Ofs ostr;
	ostr.open(tn, (STD ios_base::open_mode)(STD ios_base::out));
	ostr.close();
	STD remove(tn);
	chk(1);










	}
	end_chk("_XD2T61");
#else
	skip_chk("_XD2T61");
#endif /* CASE_XD2T61 */

/* _XD31T11 strstreambuf definition */
#if !defined(SKIP_XD31T11)&&(!defined(ONLY)||defined(CASE_XD31T11))
	begin_chk("_XD31T11");
	{
	strstreambuf sb0;

	#if IS_EMBEDDED
	chk(1);
	#else /* IS_EMBEDDED */
	typedef STD strstream Iostr;	// ADDED [MAR96]
	Iostr::char_type *p9 = (char *)0; TOUCH(p9);
	Iostr::int_type *p10 = 0; TOUCH(p10);
	Iostr::pos_type *p11 = 0; TOUCH(p11);
	Iostr::off_type *p12 = 0; TOUCH(p12);

	char abc[] = "abc";
	Iostr iostr, iostr1(abc, 4, ios_base::in);
	int i = 37;
	iostr << i << endl;
	ieq(iostr.pcount(), 3);

	char *s = iostr.str();
	ieq(*s, '3');
	iostr.freeze(0);

	iostr.rdbuf()->pubseekoff(0, ios_base::beg);
	i = 0;
	iostr >> i;
	ieq(i, 37);
	#endif /* IS_EMBEDDED */


	}
	end_chk("_XD31T11");
#else
	skip_chk("_XD31T11");
#endif /* CASE_XD31T11 */

/* _XD311T21 strstreambuf::strstreambuf(streamsize) effects */
#if !defined(SKIP_XD311T21)&&(!defined(ONLY)||defined(CASE_XD311T21))
	begin_chk("_XD311T21");
	{
	strstreambuf sb0, sb1(0), sb2(32767);
	ieq(sb0.pcount(), 0);


	}
	end_chk("_XD311T21");
#else
	skip_chk("_XD311T21");
#endif /* CASE_XD311T21 */

/* _XD311T31 strstreambuf::strstreambuf(streamsize) postconditions */
#if !defined(SKIP_XD311T31)&&(!defined(ONLY)||defined(CASE_XD311T31))
	begin_chk("_XD311T31");
	{
	strstreambuf sb0;
	ieq(sb0.sputn("abcde", 5), 5);
	ieq(sb0.pcount(), 5);


	}
	end_chk("_XD311T31");
#else
	skip_chk("_XD311T31");
#endif /* CASE_XD311T31 */

/* _XD311T51 strstreambuf::strstreambuf(palloc, pfree) effects */
#if !defined(SKIP_XD311T51)&&(!defined(ONLY)||defined(CASE_XD311T51))
	begin_chk("_XD311T51");
	{
	strstreambuf sb0(&salloc_XD311T51, &sfree_XD311T51);
	ieq(sb0.pcount(), 0);


	}
	end_chk("_XD311T51");
#else
	skip_chk("_XD311T51");
#endif /* CASE_XD311T51 */

/* _XD311T61 strstreambuf::strstreambuf(palloc, pfree) postconditions */
#if !defined(SKIP_XD311T61)&&(!defined(ONLY)||defined(CASE_XD311T61))
	begin_chk("_XD311T61");
	{
	cnt1_XD311T61 = 0, cnt2_XD311T61 = 0;
		{strstreambuf sb0(&salloc_XD311T61, &sfree_XD311T61);
		ieq(sb0.sputn("abcde", 5), 5);
		ieq(sb0.pcount(), 5); }
	chk(0 < cnt1_XD311T61);
	chk(0 < cnt2_XD311T61);


	}
	end_chk("_XD311T61");
#else
	skip_chk("_XD311T61");
#endif /* CASE_XD311T61 */

/* _XD311T81 strstreambuf::strstreambuf(char *) effects */
#if !defined(SKIP_XD311T81)&&(!defined(ONLY)||defined(CASE_XD311T81))
	begin_chk("_XD311T81");
	{
	char a[20] = "abcde";
	streamsize n = 5;
	strstreambuf sb0(a, n), sb1(a, n, a);
	signed char sa[20] = "abcde";
	strstreambuf ssb0(sa, n), ssb1(sa, n, sa);
	unsigned char ua[20] = "abcde";
	strstreambuf usb0(ua, n), usb1(ua, n, ua);
	chk(1);


	}
	end_chk("_XD311T81");
#else
	skip_chk("_XD311T81");
#endif /* CASE_XD311T81 */

/* _XD311T91 strstreambuf::strstreambuf(char *) postconditions */
#if !defined(SKIP_XD311T91)&&(!defined(ONLY)||defined(CASE_XD311T91))
	begin_chk("_XD311T91");
	{
	char a[20] = "abcde";
	strstreambuf sb0(a, 5);
	chk(sb0.sputn("xyz", 3) != 3);
	signed char sa[20] = "abcde";
	strstreambuf ssb0(sa, 5);
	chk(ssb0.sputn("xyz", 3) != 3);
	unsigned char ua[20] = "abcde"; TOUCH(ua);
	strstreambuf usb0(a, 5);
	chk(usb0.sputn("xyz", 3) != 3);


	}
	end_chk("_XD311T91");
#else
	skip_chk("_XD311T91");
#endif /* CASE_XD311T91 */

/* _XD311T_111 strstreambuf::strstreambuf(char *), pbeg_arg is null */
#if !defined(SKIP_XD311T_111)&&(!defined(ONLY)||defined(CASE_XD311T_111))
	begin_chk("_XD311T_111");
	{
	char a[20] = "abcde";
	char b[20];
	strstreambuf sb0(a, 5, 0);
	ieq(sb0.sgetn(b, 20), 5);
	ieq(STD memcmp(a, b, 5), 0);
	ieq(sb0.sputbackc('x'), 'x');
	ieq(sb0.sgetc(), 'x');
	signed char sa[20] = "fghij";
	strstreambuf ssb0(sa, 0, 0);
	ieq(ssb0.sgetn(b, 20), 5);
	ieq(STD memcmp(sa, b, 5), 0);
	unsigned char ua[20] = "klmno";
	strstreambuf usb0(ua, -1, 0);
	ieq(usb0.sgetn(b, 20), 20);
	ieq(STD memcmp(ua, b, 20), 0);



	}
	end_chk("_XD311T_111");
#else
	skip_chk("_XD311T_111");
#endif /* CASE_XD311T_111 */

/* _XD311T_121 strstreambuf::strstreambuf(char *), pbeg_arg not null */
#if !defined(SKIP_XD311T_121)&&(!defined(ONLY)||defined(CASE_XD311T_121))
	begin_chk("_XD311T_121");
	{
	char a[20] = "abcde";
	char b[20];
	strstreambuf sb0(a, 0, a);
	ieq(sb0.sputc('x'), 'x');
	ieq(sb0.sbumpc(), 'x');
	ieq(sb0.sgetn(b, 20), 4);
	ieq(STD memcmp(a + 1, b, 4), 0);
	signed char sa[20] = "fghij";
	strstreambuf ssb0(sa, 20, sa + 1);
	ieq(ssb0.sputc('x'), 'x');
	ieq(ssb0.sbumpc(), 'f');
	ieq(ssb0.sbumpc(), 'x');
	ieq(ssb0.sgetn(b, 20), 18);
	ieq(STD memcmp(sa + 2, b, 18), 0);
	unsigned char ua[20] = "klmno";
	strstreambuf usb0(ua, 20, ua + 2);
	ieq(usb0.sputc('x'), 'x');
	ieq(usb0.sbumpc(), 'k');
	ieq(usb0.sbumpc(), 'l');
	ieq(usb0.sbumpc(), 'x');
	ieq(usb0.sgetc(), 'n');


	}
	end_chk("_XD311T_121");
#else
	skip_chk("_XD311T_121");
#endif /* CASE_XD311T_121 */

/* _XD311T_141 strstreambuf::strstreambuf(const char *), effects */
#if !defined(SKIP_XD311T_141)&&(!defined(ONLY)||defined(CASE_XD311T_141))
	begin_chk("_XD311T_141");
	{
	const char a[20] = "abcde";
	char b[20];
	strstreambuf sb0(a, 5);
	ieq(sb0.sgetn(b, 20), 5);
	ieq(STD memcmp(a, b, 5), 0);
	ieq(sb0.sputbackc('y'), EOF);
	const signed char sa[20] = "fghij";
	strstreambuf ssb0(sa, 0);
	ieq(ssb0.sgetn(b, 20), 5);
	ieq(STD memcmp(sa, b, 5), 0);
	const unsigned char ua[20] = "klmno";
	strstreambuf usb0(&ua[0], -1);
	ieq(usb0.sgetn(&b[0], 20), 20);
	ieq(STD memcmp(&ua[0], &b[0], 20), 0);


	}
	end_chk("_XD311T_141");
#else
	skip_chk("_XD311T_141");
#endif /* CASE_XD311T_141 */

/* _XD311T_161 strstreambuf::~strstreambuf() frees object */
#if !defined(SKIP_XD311T_161)&&(!defined(ONLY)||defined(CASE_XD311T_161))
	begin_chk("_XD311T_161");
	{
	strstreambuf *p = new strstreambuf(&salloc_XD311T_161, &sfree_XD311T_161);
	cnt2_XD311T_161 = 0;
	delete p;
	ieq(cnt2_XD311T_161, 0);
	p = new strstreambuf(&salloc_XD311T_161, &sfree_XD311T_161);
	p->sputn("abcde", 5);
	cnt2_XD311T_161 = 0;
	delete p;
	chk(0 < cnt2_XD311T_161);
	p = new strstreambuf(&salloc_XD311T_161, &sfree_XD311T_161);
	p->sputn("abcde", 5);
	char *s = p->str();
	cnt2_XD311T_161 = 0;
	delete p;
	ieq(cnt2_XD311T_161, 0);
	delete s;

	}
	end_chk("_XD311T_161");
#else
	skip_chk("_XD311T_161");
#endif /* CASE_XD311T_161 */

return leave_chk("txd0");
}
/* V3.10:0009 */
