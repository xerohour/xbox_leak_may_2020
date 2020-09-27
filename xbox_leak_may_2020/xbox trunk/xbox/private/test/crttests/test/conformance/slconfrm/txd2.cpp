/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _XD32T11 istrstream definition */
#if !defined(SKIP_XD32T11)&&(!defined(ONLY)||defined(CASE_XD32T11))
#include <strstream>
#endif /* CASE_XD32T11 */

/* _XD321T21 istrstream::istrstream(const char *) */
#if !defined(SKIP_XD321T21)&&(!defined(ONLY)||defined(CASE_XD321T21))
#include <strstream>
#endif /* CASE_XD321T21 */

/* _XD321T41 istrstream::istrstream(const char *, streamsize) */
#if !defined(SKIP_XD321T41)&&(!defined(ONLY)||defined(CASE_XD321T41))
#include <strstream>
#endif /* CASE_XD321T41 */

/* _XD322T21 istrstream::rdbuf */
#if !defined(SKIP_XD322T21)&&(!defined(ONLY)||defined(CASE_XD322T21))
#include <strstream>
#endif /* CASE_XD322T21 */

/* _XD322T41 istrstream::str */
#if !defined(SKIP_XD322T41)&&(!defined(ONLY)||defined(CASE_XD322T41))
#include <strstream>
#endif /* CASE_XD322T41 */

/* _XD33T11 ostrstream definition */
#if !defined(SKIP_XD33T11)&&(!defined(ONLY)||defined(CASE_XD33T11))
#include <strstream>
#endif /* CASE_XD33T11 */

/* _XD331T21 ostrstream::ostrstream() */
#if !defined(SKIP_XD331T21)&&(!defined(ONLY)||defined(CASE_XD331T21))
#include <strstream>
#endif /* CASE_XD331T21 */

/* _XD331T43 ostrstream::ostrstream(char *, int, openmode), mode & app == 0 */
#if !defined(SKIP_XD331T43)&&(!defined(ONLY)||defined(CASE_XD331T43))
#include <strstream>
#endif /* CASE_XD331T43 */

/* _XD331T45 ostrstream::ostrstream(char *, int, openmode), mode & app != 0 */
#if !defined(SKIP_XD331T45)&&(!defined(ONLY)||defined(CASE_XD331T45))
#include <strstream>
#endif /* CASE_XD331T45 */

/* _XD332T21 ostrstream::rdbuf */
#if !defined(SKIP_XD332T21)&&(!defined(ONLY)||defined(CASE_XD332T21))
#include <strstream>
#endif /* CASE_XD332T21 */

/* _XD332T41 ostrstream::freeze */
#if !defined(SKIP_XD332T41)&&(!defined(ONLY)||defined(CASE_XD332T41))
#include <strstream>
#endif /* CASE_XD332T41 */

/* _XD332T61 ostrstream::str */
#if !defined(SKIP_XD332T61)&&(!defined(ONLY)||defined(CASE_XD332T61))
#include <strstream>
#endif /* CASE_XD332T61 */

/* _XD332T81 ostrstream::pcount */
#if !defined(SKIP_XD332T81)&&(!defined(ONLY)||defined(CASE_XD332T81))
#include <strstream>
#endif /* CASE_XD332T81 */

int txd2_main(int, char *[])
{
enter_chk("txd2.cpp");
/* _XD32T11 istrstream definition */
#if !defined(SKIP_XD32T11)&&(!defined(ONLY)||defined(CASE_XD32T11))
	begin_chk("_XD32T11");
	{
	istrstream is("abc");
	chk(1);


	}
	end_chk("_XD32T11");
#else
	skip_chk("_XD32T11");
#endif /* CASE_XD32T11 */

/* _XD321T21 istrstream::istrstream(const char *) */
#if !defined(SKIP_XD321T21)&&(!defined(ONLY)||defined(CASE_XD321T21))
	begin_chk("_XD321T21");
	{
	const char *s1 = "abc";
	char *s2 = "def";
	istrstream is1(s1);
	ieq(is1.get(), 'a');
	ieq(is1.rdbuf()->sputc('x'), EOF);
	ieq(is1.rdbuf()->pubseekoff(0, ios::end, ios::in) - streampos(0), 3);
	istrstream is2(s2);
	ieq(is2.get(), 'd');
	ieq(is2.rdbuf()->sputc('x'), EOF);
	ieq(is2.rdbuf()->pubseekoff(0, ios::end, ios::in) - streampos(0), 3);


	}
	end_chk("_XD321T21");
#else
	skip_chk("_XD321T21");
#endif /* CASE_XD321T21 */

/* _XD321T41 istrstream::istrstream(const char *, streamsize) */
#if !defined(SKIP_XD321T41)&&(!defined(ONLY)||defined(CASE_XD321T41))
	begin_chk("_XD321T41");
	{
	const char *s1 = "abc";
	istrstream is1(s1, 2);
	ieq(is1.get(), 'a');
	ieq(is1.rdbuf()->sputc('x'), EOF);
	ieq(is1.rdbuf()->pubseekoff(0, ios::end, ios::in) - streampos(0), 2);


	}
	end_chk("_XD321T41");
#else
	skip_chk("_XD321T41");
#endif /* CASE_XD321T41 */

/* _XD322T21 istrstream::rdbuf */
#if !defined(SKIP_XD322T21)&&(!defined(ONLY)||defined(CASE_XD322T21))
	begin_chk("_XD322T21");
	{
	istrstream is1("abc");
	strstreambuf *p = is1.rdbuf();
	const char *s = p->str();
	steq(s, "abc");
	p->freeze(false);


	}
	end_chk("_XD322T21");
#else
	skip_chk("_XD322T21");
#endif /* CASE_XD322T21 */

/* _XD322T41 istrstream::str */
#if !defined(SKIP_XD322T41)&&(!defined(ONLY)||defined(CASE_XD322T41))
	begin_chk("_XD322T41");
	{
	const char *s1 = "abc";
	istrstream is1(s1);
	char *s = is1.str();
	steq(s, s1);








	}
	end_chk("_XD322T41");
#else
	skip_chk("_XD322T41");
#endif /* CASE_XD322T41 */

/* _XD33T11 ostrstream definition */
#if !defined(SKIP_XD33T11)&&(!defined(ONLY)||defined(CASE_XD33T11))
	begin_chk("_XD33T11");
	{
	ostrstream os;
	chk(1);


	}
	end_chk("_XD33T11");
#else
	skip_chk("_XD33T11");
#endif /* CASE_XD33T11 */

/* _XD331T21 ostrstream::ostrstream() */
#if !defined(SKIP_XD331T21)&&(!defined(ONLY)||defined(CASE_XD331T21))
	begin_chk("_XD331T21");
	{
	ostrstream os;
	os.put('x');
	char *s = os.str();
	ieq(*s, 'x');
	os.freeze(false);


	}
	end_chk("_XD331T21");
#else
	skip_chk("_XD331T21");
#endif /* CASE_XD331T21 */

/* _XD331T43 ostrstream::ostrstream(char *, int, openmode), mode & app == 0 */
#if !defined(SKIP_XD331T43)&&(!defined(ONLY)||defined(CASE_XD331T43))
	begin_chk("_XD331T43");
	{
	char buf[] = "abcde";
	ostrstream os(buf, 5);
	ieq(os.rdbuf()->sgetc(), 'a');
	os.put('x');
	ieq(os.rdbuf()->sgetc(), 'x');


	}
	end_chk("_XD331T43");
#else
	skip_chk("_XD331T43");
#endif /* CASE_XD331T43 */

/* _XD331T45 ostrstream::ostrstream(char *, int, openmode), mode & app != 0 */
#if !defined(SKIP_XD331T45)&&(!defined(ONLY)||defined(CASE_XD331T45))
	begin_chk("_XD331T45");
	{
	char buf[10] = "abcde";
	ostrstream os(buf, 10, ios::app);
	ieq(os.rdbuf()->sgetc(), 'a');
	os.put('x');
	ieq(os.rdbuf()->sgetc(), 'a');
	char *s = os.str();
	steq(s, "abcdex");
	os.freeze(false);


	}
	end_chk("_XD331T45");
#else
	skip_chk("_XD331T45");
#endif /* CASE_XD331T45 */

/* _XD332T21 ostrstream::rdbuf */
#if !defined(SKIP_XD332T21)&&(!defined(ONLY)||defined(CASE_XD332T21))
	begin_chk("_XD332T21");
	{
	char buf[] = "abcde";
	ostrstream os(buf, 6);
	strstreambuf *p = os.rdbuf();
	const char *s = p->str();
	steq(s, "abcde");
	p->freeze(false);


	}
	end_chk("_XD332T21");
#else
	skip_chk("_XD332T21");
#endif /* CASE_XD332T21 */

/* _XD332T41 ostrstream::freeze */
#if !defined(SKIP_XD332T41)&&(!defined(ONLY)||defined(CASE_XD332T41))
	begin_chk("_XD332T41");
	{
	ostrstream os;
	os.freeze();
	strstreambuf*p = os.rdbuf();
	ieq(p->sputc('x'), EOF);
	os.freeze(false);
	ieq(p->sputc('x'), 'x');
	os.freeze(true);
	ieq(p->sputc('x'), EOF);
	os.freeze(false);


	}
	end_chk("_XD332T41");
#else
	skip_chk("_XD332T41");
#endif /* CASE_XD332T41 */

/* _XD332T61 ostrstream::str */
#if !defined(SKIP_XD332T61)&&(!defined(ONLY)||defined(CASE_XD332T61))
	begin_chk("_XD332T61");
	{
	ostrstream os;
	os.write("abcde", 6);
	char *s = os.str();
	steq(s, "abcde");
	os.freeze(false);


	}
	end_chk("_XD332T61");
#else
	skip_chk("_XD332T61");
#endif /* CASE_XD332T61 */

/* _XD332T81 ostrstream::pcount */
#if !defined(SKIP_XD332T81)&&(!defined(ONLY)||defined(CASE_XD332T81))
	begin_chk("_XD332T81");
	{
	ostrstream os;
	os.write("abcde", 6);
	ieq(os.pcount(), 6);
	}
	end_chk("_XD332T81");
#else
	skip_chk("_XD332T81");
#endif /* CASE_XD332T81 */

return leave_chk("txd2");
}
/* V3.10:0009 */
