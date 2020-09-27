/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _25361T31a push_heap(first, last)   effects, complexity */
#if !defined(SKIP_25361T31a)&&(!defined(ONLY)||defined(CASE_25361T31a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
#endif /* CASE_25361T31a */

/* _25361T31b push_heap(first, last, comp)   effects, complexity */
#if !defined(SKIP_25361T31b)&&(!defined(ONLY)||defined(CASE_25361T31b))
#include<algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include "iter.h"
int npreds_25361T31b = 0;
int f_25361T31b(Char c1, Char c2)
	{++npreds_25361T31b;
	return (tolower(c1) < tolower(c2)); }
#endif /* CASE_25361T31b */

/* _25362T31a pop_heap(first, last)   effects, complexity */
#if !defined(SKIP_25362T31a)&&(!defined(ONLY)||defined(CASE_25362T31a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
#endif /* CASE_25362T31a */

/* _25362T31b pop_heap(first, last, comp)   effects, complexity */
#if !defined(SKIP_25362T31b)&&(!defined(ONLY)||defined(CASE_25362T31b))
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include "iter.h"
int npreds_25362T31b = 0;
int f_25362T31b(Char c1, Char c2)
	{++npreds_25362T31b;
	return (tolower(c1) < tolower(c2)); }
#endif /* CASE_25362T31b */

/* _25363T31a make_heap(first, last)   effects, complexity */
#if !defined(SKIP_25363T31a)&&(!defined(ONLY)||defined(CASE_25363T31a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
#endif /* CASE_25363T31a */

/* _25363T31b make_heap(first, last, comp)   effects, complexity */
#if !defined(SKIP_25363T31b)&&(!defined(ONLY)||defined(CASE_25363T31b))
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include "iter.h"
int npreds_25363T31b = 0;
int f_25363T31b(Char c1, Char c2)
	{++npreds_25363T31b;
	return (tolower(c1) < tolower(c2)); }
#endif /* CASE_25363T31b */

/* _25364T21a sort_heap(first, last)   effects, complexity */
#if !defined(SKIP_25364T21a)&&(!defined(ONLY)||defined(CASE_25364T21a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
#endif /* CASE_25364T21a */

/* _25364T21b sort_heap(first, last, comp)   effects, complexity */
#if !defined(SKIP_25364T21b)&&(!defined(ONLY)||defined(CASE_25364T21b))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
int npreds_25364T21b = 0;
int f_25364T21b(Char c1, Char c2)
	{++npreds_25364T21b;
	return (tolower(c1) < tolower(c2)); }
#endif /* CASE_25364T21b */

/* _2537T21a min(a, b)   returns, notes */
#if !defined(SKIP_2537T21a)&&(!defined(ONLY)||defined(CASE_2537T21a))
#include <algorithm>
#endif /* CASE_2537T21a */

/* _2537T21b min(a, b, comp)  returns, notes */
#if !defined(SKIP_2537T21b)&&(!defined(ONLY)||defined(CASE_2537T21b))
#include <algorithm>
#include <cctype>
bool g_2537T21b(char c1, char c2)
	{return (tolower(c1) < tolower(c2)); }
bool h_2537T21b(double d1, double d2)
	{return (d1 > d2); }
bool i_2537T21b(long n1, long n2)
	{ return (n1 % 10 < n2 % 10); }
#endif /* CASE_2537T21b */

/* _2537T51a max(a, b)   returns, notes */
#if !defined(SKIP_2537T51a)&&(!defined(ONLY)||defined(CASE_2537T51a))
#include <algorithm>
#endif /* CASE_2537T51a */

/* _2537T51b max(a, b, comp)  returns, notes */
#if !defined(SKIP_2537T51b)&&(!defined(ONLY)||defined(CASE_2537T51b))
#include <algorithm>
#include <cctype>
bool g_2537T51b(char c1, char c2)
	{return (tolower(c1) < tolower(c2)); }
bool h_2537T51b(double d1, double d2)
	{return (d1 > d2); }
bool i_2537T51b(long n1, long n2)
	{return (n1 % 10 < n2 % 10); }
#endif /* CASE_2537T51b */

/* _2537T81a min_element(a, b)   returns */
#if !defined(SKIP_2537T81a)&&(!defined(ONLY)||defined(CASE_2537T81a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
#endif /* CASE_2537T81a */

/* _2537T81b min_element(a, b, comp)  returns, complexity */
#if !defined(SKIP_2537T81b)&&(!defined(ONLY)||defined(CASE_2537T81b))
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include "iter.h"
int npreds_2537T81b = 0;
int f_2537T81b(Char c1, Char c2)
	{++npreds_2537T81b;
	return (tolower(c1) < tolower(c2)); }
#endif /* CASE_2537T81b */

/* _2537T_111a max_element(a, b)   returns, complexity */
#if !defined(SKIP_2537T_111a)&&(!defined(ONLY)||defined(CASE_2537T_111a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
#endif /* CASE_2537T_111a */

/* _2537T_111b max_element(a, b, comp)  returns, complexity */
#if !defined(SKIP_2537T_111b)&&(!defined(ONLY)||defined(CASE_2537T_111b))
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include "iter.h"
int npreds_2537T_111b = 0;
int f_2537T_111b(Char c1, Char c2)
	{++npreds_2537T_111b;
	return (tolower(c1) < tolower(c2)); }
#endif /* CASE_2537T_111b */

/* _2538T21a lexicographical_compare(-)   returns, complexity */
#if !defined(SKIP_2538T21a)&&(!defined(ONLY)||defined(CASE_2538T21a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
#endif /* CASE_2538T21a */

/* _2538T21b lexicographical_compare(-,comp)   returns, complexity */
#if !defined(SKIP_2538T21b)&&(!defined(ONLY)||defined(CASE_2538T21b))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
int npreds_2538T21b = 0;
int f_2538T21b(Char c1, Char c2)
	{++npreds_2538T21b;
	return (tolower(c1) < tolower(c2)); }
#endif /* CASE_2538T21b */

/* _2538T31a next_permutation(first, last)   effects, complexity */
#if !defined(SKIP_2538T31a)&&(!defined(ONLY)||defined(CASE_2538T31a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
int cnt_2538T31a = 0;
struct Mychar_2538T31a {
	Mychar_2538T31a(char ch = 0)
		: c(ch) {}
	Mychar_2538T31a(const Mychar_2538T31a& x)
		{++cnt_2538T31a;
		c = x.c; }
	Mychar_2538T31a& operator=(const Mychar_2538T31a& x)
		{++cnt_2538T31a;
		c = x.c;
		return (*this); }
	Mychar_2538T31a& operator=(char ch)
		{c = ch;
		return (*this); }
	bool operator<(const Mychar_2538T31a& x)
		{return (c == x.c); }
	char c;
	};
#endif /* CASE_2538T31a */

/* _2538T31b next_permutation(first, last, comp)   effects, complexity */
#if !defined(SKIP_2538T31b)&&(!defined(ONLY)||defined(CASE_2538T31b))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
int f_2538T31b(Char c1, Char c2)
	{return (tolower(c1) < tolower(c2)); }
#endif /* CASE_2538T31b */

/* _2538T41a prev_permutation(first, last)   effects, complexity */
#if !defined(SKIP_2538T41a)&&(!defined(ONLY)||defined(CASE_2538T41a))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
int cnt_2538T41a = 0;
struct Mychar_2538T41a {
	Mychar_2538T41a(char ch = 0)
		: c(ch) {}
	Mychar_2538T41a(const Mychar_2538T41a& x)
		{++cnt_2538T41a;
		c = x.c; }
	Mychar_2538T41a& operator=(const Mychar_2538T41a& x)
		{++cnt_2538T41a;
		c = x.c;
		return (*this); }
	Mychar_2538T41a& operator=(char ch)
		{c = ch;
		return (*this); }
	bool operator<(const Mychar_2538T41a& x)
		{return (c == x.c); }
	char c;
	};
#endif /* CASE_2538T41a */

/* _2538T41b prev_permutation(first, last, comp)   effects, complexity */
#if !defined(SKIP_2538T41b)&&(!defined(ONLY)||defined(CASE_2538T41b))
#include <algorithm>
#include <cstring>
#include <string>
#include "iter.h"
int f_2538T41b(Char c1, Char c2)
	{return (tolower(c1) < tolower(c2)); }
#endif /* CASE_2538T41b */

/* _254T21 <cstdlib> synopsis */
#if !defined(SKIP_254T21)&&(!defined(ONLY)||defined(CASE_254T21))
#include <cstdlib>
int CLINK cmp_254T21(const void *p1, const void *p2)
	{return (*(unsigned char *)p1 - *(unsigned char *)p2); }
#endif /* CASE_254T21 */

int t258_main(int, char *[])
{
enter_chk("t258.cpp");
/* _25361T31a push_heap(first, last)   effects, complexity */
#if !defined(SKIP_25361T31a)&&(!defined(ONLY)||defined(CASE_25361T31a))
	begin_chk("_25361T31a");
	{
	// simple pointers
	Char buf[] = "yxcbaz";
	Char *p1 = buf;
	Char *p2 = buf+6;
	push_heap(p1, p1 + 1);
	steq(buf, "yxcbaz");
	push_heap(p1, p1 + 2);
	steq(buf, "yxcbaz");
	push_heap(p1, p1 + 5);
	steq(buf, "yxcbaz");
	push_heap(p1, p2);
	ieq(*p1, 'z');

	// strings
	Ustr s ("7653214");
	push_heap(s.begin(), s.end());
	ieq(s[0], '7');
	chk(s[7] != '4');

	// strict iterators over strings
	const Char *buf2 = (const Char *)"dbcax";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	push_heap(it1, it2);
	ieq(s4[0], 'x');
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_25361T31a");
#else
	skip_chk("_25361T31a");
#endif /* CASE_25361T31a */

/* _25361T31b push_heap(first, last, comp)   effects, complexity */
#if !defined(SKIP_25361T31b)&&(!defined(ONLY)||defined(CASE_25361T31b))
	begin_chk("_25361T31b");
	{
	// simple pointers
	Char buf[] = "yxCbaZ";
	Char *p1 = buf;
	Char *p2 = buf+6;
	push_heap(p1, p1 + 1, f_25361T31b);
	steq(buf, "yxCbaZ");
	push_heap(p1, p1 + 2, f_25361T31b);
	steq(buf, "yxCbaZ");
	push_heap(p1, p1 + 5, f_25361T31b);
	steq(buf, "yxCbaZ");
	npreds_25361T31b = 0;
	push_heap(p1, p2, f_25361T31b);
	int n = 6;
	ileq(npreds_25361T31b,  ceil_log2(n));
	ieq(*p1, 'Z');

	// strings
	Ustr s ("7653214");
	npreds_25361T31b = 0;
	push_heap(s.begin(), s.end(), f_25361T31b);
	n = 6;
	ileq(npreds_25361T31b, ceil_log2(n));
	ieq(s[0], '7');
	chk(s[7] != '4');

	// strict iterators over strings
	const Char *buf2 = (const Char *)"dBCax";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	push_heap(it1, it2, f_25361T31b);
	ieq(s4[0], 'x');
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_25361T31b");
#else
	skip_chk("_25361T31b");
#endif /* CASE_25361T31b */

/* _25362T31a pop_heap(first, last)   effects, complexity */
#if !defined(SKIP_25362T31a)&&(!defined(ONLY)||defined(CASE_25362T31a))
	begin_chk("_25362T31a");
	{
	// simple pointers
	Char buf[] = "zxycba";
	Char *p1 = buf;
	Char *p2 = buf+6;
	pop_heap(p1, p1 + 1);
	steq(buf, "zxycba");
	pop_heap(p1, p2);
	ieq(*p1, 'y');
	ieq(*(p2 - 1), 'z');

	// strings
	Ustr s ("7654321");
	pop_heap(s.begin(), s.end());
	ieq(s[0], '6');
	ieq(s[6], '7');

	// strict iterators over strings
	const Char *buf2 = (const Char *)"xcdba";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	pop_heap(it1, it2);
	ieq(s4[0], 'd');
	ieq(s4[4], 'x');
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_25362T31a");
#else
	skip_chk("_25362T31a");
#endif /* CASE_25362T31a */

/* _25362T31b pop_heap(first, last, comp)   effects, complexity */
#if !defined(SKIP_25362T31b)&&(!defined(ONLY)||defined(CASE_25362T31b))
	begin_chk("_25362T31b");
	{
	// simple pointers
	Char buf[] = "ZyxCba";
	Char *p1 = buf;
	Char *p2 = buf+6;
	npreds_25362T31b = 0;
	pop_heap(p1, p2, f_25362T31b);
	int n = 6;
	ileq(npreds_25362T31b, 2 * ceil_log2(n));
	ieq(buf[0], 'y');
	ieq(buf[5], 'Z');

	// strings
	npreds_25362T31b = 0;
	Ustr s ("ZyxCba");
	pop_heap(s.begin(), s.end(), f_25362T31b);
	ieq(buf[0], 'y');
	ieq(buf[5], 'Z');

	// strict iterators over strings
	const Char *buf2 = (const Char *)"xCdBa";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	pop_heap(it1, it2, f_25362T31b);
	ieq(s4[0], 'd');
	ieq(s4[4], 'x');
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_25362T31b");
#else
	skip_chk("_25362T31b");
#endif /* CASE_25362T31b */

/* _25363T31a make_heap(first, last)   effects, complexity */
#if !defined(SKIP_25363T31a)&&(!defined(ONLY)||defined(CASE_25363T31a))
	begin_chk("_25363T31a");
	{
	// simple pointers
	Char buf[] = "xazbcy";
	Char *p1 = buf;
	Char *p2 = buf+6;
	make_heap(p1, p1);
	steq(buf, "xazbcy");
	make_heap(p1, p1 + 1);
	steq(buf, "xazbcy");
	make_heap(p1, p1 + 2);
	steq(buf, "xazbcy");
	make_heap(p1, p2);
	ieq(*p1, 'z');

	// strings
	Ustr s("22101");
	make_heap(s.begin(), s.end());
	ieq(s[0], '2');
	pop_heap(s.begin(), s.end());
	ieq(s[0], '2');
	pop_heap(s.begin(), s.end() - 1);
	ieq(s[0], '1');

	// strict iterators over strings
	const Char *buf2 = (const Char *)"baxcd";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	make_heap(it1, it2);
	ieq(s4[0], 'x');
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_25363T31a");
#else
	skip_chk("_25363T31a");
#endif /* CASE_25363T31a */

/* _25363T31b make_heap(first, last, comp)   effects, complexity */
#if !defined(SKIP_25363T31b)&&(!defined(ONLY)||defined(CASE_25363T31b))
	begin_chk("_25363T31b");
	{
	// simple pointers
	Char buf[] = "xaZBcy";
	Char *p1 = buf;
	Char *p2 = buf+6;
	make_heap(p1, p1, f_25363T31b);
	steq(buf, "xaZBcy");
	make_heap(p1, p1 + 1, f_25363T31b);
	steq(buf, "xaZBcy");
	make_heap(p1, p1 + 2, f_25363T31b);
	steq(buf, "xaZBcy");
	npreds_25363T31b = 0;
	make_heap(p1, p2, f_25363T31b);
	int n = 6;
	ileq(npreds_25363T31b, 3 * n);
	ieq(*p1, 'Z');

	// strings
	npreds_25363T31b = 0;
	Ustr s ("22101");
	make_heap(s.begin(), s.end(), f_25363T31b);
	ieq(s[0], '2');
	pop_heap(s.begin(), s.end(), f_25363T31b);
	ieq(s[0], '2');
	pop_heap(s.begin(), s.end() - 1, f_25363T31b);
	ieq(s[0], '1');

	// strict iterators over strings
	const Char *buf2 = (const Char *)"bAXcd";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	make_heap(it1, it2, f_25363T31b);
	ieq(s4[0], 'X');
	ieq(err1, 0);
	ieq(err2, 0);



	}
	end_chk("_25363T31b");
#else
	skip_chk("_25363T31b");
#endif /* CASE_25363T31b */

/* _25364T21a sort_heap(first, last)   effects, complexity */
#if !defined(SKIP_25364T21a)&&(!defined(ONLY)||defined(CASE_25364T21a))
	begin_chk("_25364T21a");
	{
	// simple pointers
	Char buf[] = "zxycba";
	Char *p1 = buf;
	Char *p2 = buf+6;
	sort_heap(p1, p1);
	steq(buf, "zxycba");
	sort_heap(p1, p1 + 1);
	steq(buf, "zxycba");
	sort_heap(p1 + 3, p1 + 5);
	steq(buf, "zxybca");
	sort_heap(p1, p2);
	steq(buf, "abcxyz");

	// strings
	Ustr s ("765432210");
	sort_heap(s.begin(), s.end());
	steq(s.c_str(), "012234567");

	// strict iterators over strings
	const Char *buf2 = (const Char *)"xcdba";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	sort_heap(it1, it2);
	steq(s4.c_str(), "abcdx");
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_25364T21a");
#else
	skip_chk("_25364T21a");
#endif /* CASE_25364T21a */

/* _25364T21b sort_heap(first, last, comp)   effects, complexity */
#if !defined(SKIP_25364T21b)&&(!defined(ONLY)||defined(CASE_25364T21b))
	begin_chk("_25364T21b");
	{
	// simple pointers
	Char buf[] = "zXyCba";
	Char *p1 = buf;
	Char *p2 = buf+6;
	sort_heap(p1, p1, f_25364T21b);
	steq(buf, "zXyCba");
	sort_heap(p1, p1 + 1, f_25364T21b);
	steq(buf, "zXyCba");
	sort_heap(p1 + 3, p1 + 5, f_25364T21b);
	steq(buf, "zXybCa");
	npreds_25364T21b = 0;
	sort_heap(p1, p2, f_25364T21b);
	int n = 6;
	ileq(npreds_25364T21b, n * ceil_log2(n));
	steq(buf, "abCXyz");

	// strings
	Ustr s("765432210");
	sort_heap(s.begin(), s.end(), f_25364T21b);
	steq(s.c_str(), "012234567");

	// strict iterators over strings
	const Char *buf2 = (const Char *)"xCDba";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	sort_heap(it1, it2, f_25364T21b);
	steq(s4.c_str(), "abCDx");
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_25364T21b");
#else
	skip_chk("_25364T21b");
#endif /* CASE_25364T21b */

/* _2537T21a min(a, b)   returns, notes */
#if !defined(SKIP_2537T21a)&&(!defined(ONLY)||defined(CASE_2537T21a))
	begin_chk("_2537T21a");
	{
	ieq(min('a', 'b'), 'a');
	ieq(min('b', 'a'), 'a');
	deq(min(1.0, -2.0), -2.0);
	leq(min(2L, 3L), 2L);

	}
	end_chk("_2537T21a");
#else
	skip_chk("_2537T21a");
#endif /* CASE_2537T21a */

/* _2537T21b min(a, b, comp)  returns, notes */
#if !defined(SKIP_2537T21b)&&(!defined(ONLY)||defined(CASE_2537T21b))
	begin_chk("_2537T21b");
	{
	ieq(min('a', 'b', g_2537T21b), 'a');
	ieq(min('b', 'B', g_2537T21b), 'b');
	ieq(min('B', 'b', g_2537T21b), 'B');
	ieq(min('A', 'b', g_2537T21b), 'A');
	deq(min(1.0, -2.0, h_2537T21b), 1.0);
	leq(min(12L, 3L, i_2537T21b), 12L);


	}
	end_chk("_2537T21b");
#else
	skip_chk("_2537T21b");
#endif /* CASE_2537T21b */

/* _2537T51a max(a, b)   returns, notes */
#if !defined(SKIP_2537T51a)&&(!defined(ONLY)||defined(CASE_2537T51a))
	begin_chk("_2537T51a");
	{
	ieq(max('a', 'b'), 'b');
	ieq(max('b', 'a'), 'b');
	deq(max(1.0, -2.0), 1.0);
	leq(max(2L, 3L), 3L);

	}
	end_chk("_2537T51a");
#else
	skip_chk("_2537T51a");
#endif /* CASE_2537T51a */

/* _2537T51b max(a, b, comp)  returns, notes */
#if !defined(SKIP_2537T51b)&&(!defined(ONLY)||defined(CASE_2537T51b))
	begin_chk("_2537T51b");
	{
	ieq(max('a', 'b', g_2537T51b), 'b');
	ieq(max('B', 'b', g_2537T51b), 'B');
	ieq(max('b', 'B', g_2537T51b), 'b');
	ieq(max('A', 'b', g_2537T51b), 'b');
	deq(max(1.0, -2.0, h_2537T51b), -2.0);
	leq(max(12L, 3L, i_2537T51b), 3L);



	}
	end_chk("_2537T51b");
#else
	skip_chk("_2537T51b");
#endif /* CASE_2537T51b */

/* _2537T81a min_element(a, b)   returns */
#if !defined(SKIP_2537T81a)&&(!defined(ONLY)||defined(CASE_2537T81a))
	begin_chk("_2537T81a");
	{
	// simple pointers
	const Char buf[] = "zayxca";
	const Char *p1 = buf;
	const Char *p2 = buf+6;
	chk(min_element(p1, p1) == p1);
	chk(min_element(p1, p1 + 1) == p1);
	chk(min_element(p1, p1 + 2) == p1 + 1);
	chk(min_element(p1 + 1, p1 + 3) == p1 + 1);
	const Char *r = min_element(p1, p2);
	chk(r == p1 + 1);
	ieq(*r, 'a');

	// strings
	Ustr s ("azaxay");
	Ustr::const_iterator r2 = min_element(s.begin(), s.end());
	ieq(*r2, 'a');
	ieq(r2 - s.begin(), 0);

	// strict iterators over strings
	const Char *buf2 = (const Char *)"31011";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	strict_forward_iterator<Ustr::iterator, Char> it3
		(min_element(it1, it2));
	ieq(*it3, '0');
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_2537T81a");
#else
	skip_chk("_2537T81a");
#endif /* CASE_2537T81a */

/* _2537T81b min_element(a, b, comp)  returns, complexity */
#if !defined(SKIP_2537T81b)&&(!defined(ONLY)||defined(CASE_2537T81b))
	begin_chk("_2537T81b");
	{
	// simple pointers
	const Char buf[] = "zayXcA";
	const Char *p1 = buf;
	const Char *p2 = buf+6;
	chk(min_element(p1, p1, f_2537T81b) == p1);
	chk(min_element(p1, p1 + 1, f_2537T81b) == p1);
	chk(min_element(p1, p1 + 2, f_2537T81b) == p1 + 1);
	chk(min_element(p1 + 1, p1 + 3, f_2537T81b) == p1 + 1);
	npreds_2537T81b = 0;
	const Char *r = min_element(p1, p2, f_2537T81b);
	int n = 6;
	ileq(npreds_2537T81b, (n - 1 < 0 ? 0 : n - 1));
	chk(r == p1 + 1);
	ieq(*r, 'a');

	// strings
	Ustr s ("azAxay");
	npreds_2537T81b = 0;
	Ustr::const_iterator r2 = min_element(s.begin(), s.end(), f_2537T81b);
	n = 6;
	ileq(npreds_2537T81b, (n - 1 < 0 ? 0 : n - 1));
	ieq(*r2, 'a');
	ieq(r2 - s.begin(), 0);

	// strict iterators over strings
	const Char *buf2 = (const Char *)"31011";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	strict_forward_iterator<Ustr::iterator, Char> it3
		(min_element(it1, it2, f_2537T81b));
	ieq(*it3, '0');
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_2537T81b");
#else
	skip_chk("_2537T81b");
#endif /* CASE_2537T81b */

/* _2537T_111a max_element(a, b)   returns, complexity */
#if !defined(SKIP_2537T_111a)&&(!defined(ONLY)||defined(CASE_2537T_111a))
	begin_chk("_2537T_111a");
	{
	// simple pointers
	const Char buf[] = "zayxca";
	const Char *p1 = buf;
	const Char *p2 = buf+6;
	chk(max_element(p1, p1) == p1);
	chk(max_element(p1, p1 + 1) == p1);
	chk(max_element(p1, p1 + 2) == p1);
	chk(max_element(p1 + 1, p1 + 3) == p1 + 2);
	const Char *r = max_element(p1, p2);
	chk(r == p1);
	ieq(*r, 'z');

	// strings
	Ustr s ("axazay");
	Ustr::const_iterator r2 = max_element(s.begin(), s.end());
	ieq(*r2, 'z');
	ieq(r2 - s.begin(), 3);

	// strict iterators over strings
	const Char *buf2 = (const Char *)"31411";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	strict_forward_iterator<Ustr::iterator, Char> it3
		(max_element(it1, it2));
	ieq(*it3, '4');
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_2537T_111a");
#else
	skip_chk("_2537T_111a");
#endif /* CASE_2537T_111a */

/* _2537T_111b max_element(a, b, comp)  returns, complexity */
#if !defined(SKIP_2537T_111b)&&(!defined(ONLY)||defined(CASE_2537T_111b))
	begin_chk("_2537T_111b");
	{
	// simple pointers
	const Char buf[] = "ZayxCa";
	const Char *p1 = buf;
	const Char *p2 = buf+6;
	chk(max_element(p1, p1, f_2537T_111b) == p1);
	chk(max_element(p1, p1 + 1, f_2537T_111b) == p1);
	chk(max_element(p1, p1 + 2, f_2537T_111b) == p1);
	chk(max_element(p1 + 1, p1 + 3, f_2537T_111b) == p1 + 2);
	npreds_2537T_111b = 0;
	const Char *r = max_element(p1, p2, f_2537T_111b);
	int n = 6;
	ileq(npreds_2537T_111b, (n - 1 < 0 ? 0 : n - 1));
	chk(r == p1);
	ieq(*r, 'Z');

	// strings
	Ustr s ("AxaZzy");
	npreds_2537T_111b = 0;
	Ustr::const_iterator r2 = max_element(s.begin(), s.end(), f_2537T_111b);
	n = 6;
	ileq(npreds_2537T_111b, (n - 1 < 0 ? 0 : n - 1));
	ieq(*r2, 'Z');
	ieq(r2 - s.begin(), 3);

	// strict iterators over strings
	const Char *buf2 = (const Char *)"31411";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	strict_forward_iterator<Ustr::iterator, Char> it3
		(max_element(it1, it2, f_2537T_111b));
	ieq(*it3, '4');
	ieq(err1, 0);
	ieq(err2, 0);



	}
	end_chk("_2537T_111b");
#else
	skip_chk("_2537T_111b");
#endif /* CASE_2537T_111b */

/* _2538T21a lexicographical_compare(-)   returns, complexity */
#if !defined(SKIP_2538T21a)&&(!defined(ONLY)||defined(CASE_2538T21a))
	begin_chk("_2538T21a");
	{
	// simple pointers
	Char buf[] = "aby";
	Char bufb[] = "abz";
	chk(!lexicographical_compare(buf, buf, bufb, bufb));
	chk(!lexicographical_compare(buf, buf + 1, bufb, bufb));
	chk(lexicographical_compare(buf, buf, bufb, bufb + 1));
	chk(lexicographical_compare(buf, buf + 1, bufb, bufb + 2));
	chk(!lexicographical_compare(buf, buf + 2, bufb, bufb + 2));
	bool r = lexicographical_compare(buf, buf + 3, bufb, bufb + 3);
	ieq(r, true);

	// strings
	Ustr s ("zzaxz");
	Ustr sb ("zzaxzY");
	chk(lexicographical_compare(s.begin(), s.end(), sb.begin(), sb.end()));

	// strict iterators over strings
	const Char *buf2 = (const Char *)"01133";
	Ustr s4(buf2, buf2 + 5);
	const Char *buf3 = (const Char *)"0113";
	Ustr s5(buf3, buf3 + 4);
	int err1 = 0, nrec1 = 5;
	strict_input_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	int err3 = 0, nrec3 = 4;
	strict_input_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 0;
	strict_input_iterator<Ustr::iterator, Char> it4
		(s5.end(), err4, nrec4);
	chk(!lexicographical_compare(it1, it2, it3, it4));
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);

	}
	end_chk("_2538T21a");
#else
	skip_chk("_2538T21a");
#endif /* CASE_2538T21a */

/* _2538T21b lexicographical_compare(-,comp)   returns, complexity */
#if !defined(SKIP_2538T21b)&&(!defined(ONLY)||defined(CASE_2538T21b))
	begin_chk("_2538T21b");
	{
	// simple pointers
	Char buf[] = "ABy";
	Char bufb[] = "abz";
	chk(!lexicographical_compare(buf, buf, bufb, bufb, f_2538T21b));
	chk(!lexicographical_compare(buf, buf + 1, bufb, bufb, f_2538T21b));
	chk(lexicographical_compare(buf, buf, bufb, bufb + 1, f_2538T21b));
	chk(lexicographical_compare(buf, buf + 1, bufb, bufb + 2, f_2538T21b));
	chk(!lexicographical_compare(buf, buf + 2, bufb, bufb + 2, f_2538T21b));
	npreds_2538T21b = 0;
	bool r = lexicographical_compare(buf, buf + 3, bufb, bufb + 3, f_2538T21b);
	int n = 3;
	int m = 3;
	ileq(npreds_2538T21b, 2 * (n < m ? n : m));
	ieq(r, true);

	// strings
	Ustr s ("zzaxz");
	Ustr sb ("zZAxzY");
	npreds_2538T21b = 0;
	chk(lexicographical_compare(s.begin(), s.end(), sb.begin(), sb.end(), f_2538T21b));
	n = 5;
	m = 6;
	ileq(npreds_2538T21b, 2 * (n < m ? n : m));

	// strict iterators over strings
	const Char *buf2 = (const Char *)"01133";
	Ustr s4(buf2, buf2 + 5);
	const Char *buf3 = (const Char *)"0113";
	Ustr s5(buf3, buf3 + 4);
	int err1 = 0, nrec1 = 5;
	strict_input_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	int err3 = 0, nrec3 = 4;
	strict_input_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 0;
	strict_input_iterator<Ustr::iterator, Char> it4
		(s5.end(), err4, nrec4);
	chk(!lexicographical_compare(it1, it2, it3, it4, f_2538T21b));
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);


	}
	end_chk("_2538T21b");
#else
	skip_chk("_2538T21b");
#endif /* CASE_2538T21b */

/* _2538T31a next_permutation(first, last)   effects, complexity */
#if !defined(SKIP_2538T31a)&&(!defined(ONLY)||defined(CASE_2538T31a))
	begin_chk("_2538T31a");
	{
	// simple pointers
	Char buf[] = "akz";
	Char *p1 = buf;
	Char *p2 = buf + 2;
	chk(!next_permutation(buf, buf));
	steq(buf, "akz");
	chk(!next_permutation(buf, buf + 1));
	steq(buf, "akz");
	chk(next_permutation(buf, buf + 2));
	steq(buf, "kaz");
	chk(!next_permutation(buf, buf + 2));
	steq(buf, "akz");
	bool r = next_permutation(p1, p2);
	steq(buf, "kaz");
	ieq(r, true);

	// strings
	Ustr s("zyxba");
	bool r2 = next_permutation(s.begin(), s.end());
	steq(s.c_str(), "abxyz");
	ieq(r2, false);

	// strict iterators over strings
	const Char *buf2 = (const Char *)"aedcb";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_bidirectional_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_bidirectional_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	chk(next_permutation(it1, it2));
	ieq(err1, 0);
	ieq(err2, 0);

	// complexity
	Mychar_2538T31a a[10];
	int n = 10;
	for (int i = 0; i < n; i ++)
		a[i] = '0' + i;
	random_shuffle(a, a + 10);
	cnt_2538T31a = 0;
	next_permutation(a, a + 10);
	ileq(cnt_2538T31a, (3 * n) / 2);
	random_shuffle(a, a + 10);
	cnt_2538T31a = 0;
	next_permutation(a, a + 10);
	ileq(cnt_2538T31a, (3 * n) / 2);

	}
	end_chk("_2538T31a");
#else
	skip_chk("_2538T31a");
#endif /* CASE_2538T31a */

/* _2538T31b next_permutation(first, last, comp)   effects, complexity */
#if !defined(SKIP_2538T31b)&&(!defined(ONLY)||defined(CASE_2538T31b))
	begin_chk("_2538T31b");
	{
	// simple pointers
	Char buf[] = "aKz";
	Char *p1 = buf;
	Char *p2 = buf + 2;
	chk(!next_permutation(buf, buf, f_2538T31b));
	steq(buf, "aKz");
	chk(!next_permutation(buf, buf + 1, f_2538T31b));
	steq(buf, "aKz");
	chk(next_permutation(buf, buf + 2, f_2538T31b));
	steq(buf, "Kaz");
	chk(!next_permutation(buf, buf + 2, f_2538T31b));
	steq(buf, "aKz");
	bool r = next_permutation(p1, p2, f_2538T31b);
	steq(buf, "Kaz");
	ieq(r, true);

	// strings
	Ustr s("zyxba");
	bool r2 = next_permutation(s.begin(), s.end(), f_2538T31b);
	steq(s.c_str(), "abxyz");
	ieq(r2, false);

	// strict iterators over strings
	const Char *buf2 = (const Char *)"aedcb";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_bidirectional_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_bidirectional_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	chk(next_permutation(it1, it2, f_2538T31b));
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_2538T31b");
#else
	skip_chk("_2538T31b");
#endif /* CASE_2538T31b */

/* _2538T41a prev_permutation(first, last)   effects, complexity */
#if !defined(SKIP_2538T41a)&&(!defined(ONLY)||defined(CASE_2538T41a))
	begin_chk("_2538T41a");
	{
	// simple pointers
	Char buf[] = "akz";
	Char *p1 = buf;
	Char *p2 = buf + 2;
	chk(!prev_permutation(buf, buf));
	steq(buf, "akz");
	chk(!prev_permutation(buf, buf + 1));
	steq(buf, "akz");
	chk(!prev_permutation(buf, buf + 2));
	steq(buf, "kaz");
	chk(prev_permutation(buf, buf + 2));
	steq(buf, "akz");
	chk(!prev_permutation(buf, buf + 2));
	steq(buf, "kaz");
	bool r = prev_permutation(p1, p2);
	steq(buf, "akz");
	ieq(r, true);

	// strings
	Ustr s("abxyz");
	bool r2 = prev_permutation(s.begin(), s.end());
	steq(s.c_str(), "zyxba");
	ieq(r2, false);

	// strict iterators over strings
	const Char *buf2 = (const Char *)"aedcb";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_bidirectional_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_bidirectional_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	chk(prev_permutation(it1, it2));
	ieq(err1, 0);
	ieq(err2, 0);

	// complexity
	Mychar_2538T41a a[10];
	int n = 10;
	for (int i = 0; i < n; i ++)
		a[i] = '0' + i;
	random_shuffle(a, a + 10);
	cnt_2538T41a = 0;
	prev_permutation(a, a + 10);
	ileq(cnt_2538T41a, (3 * n) / 2);
	random_shuffle(a, a + 10);
	cnt_2538T41a = 0;
	prev_permutation(a, a + 10);
	ileq(cnt_2538T41a, (3 * n) / 2);

	}
	end_chk("_2538T41a");
#else
	skip_chk("_2538T41a");
#endif /* CASE_2538T41a */

/* _2538T41b prev_permutation(first, last, comp)   effects, complexity */
#if !defined(SKIP_2538T41b)&&(!defined(ONLY)||defined(CASE_2538T41b))
	begin_chk("_2538T41b");
	{
	// simple pointers
	Char buf[] = "aKz";
	Char *p1 = buf;
	Char *p2 = buf + 2;
	chk(!prev_permutation(buf, buf, f_2538T41b));
	steq(buf, "aKz");
	chk(!prev_permutation(buf, buf + 1, f_2538T41b));
	steq(buf, "aKz");
	chk(!prev_permutation(buf, buf + 2, f_2538T41b));
	steq(buf, "Kaz");
	chk(prev_permutation(buf, buf + 2, f_2538T41b));
	steq(buf, "aKz");
	chk(!prev_permutation(buf, buf + 2, f_2538T41b));
	steq(buf, "Kaz");
	bool r = prev_permutation(p1, p2, f_2538T41b);
	steq(buf, "aKz");
	ieq(r, true);

	// strings
	Ustr s("abxyz");
	bool r2 = prev_permutation(s.begin(), s.end(), f_2538T41b);
	steq(s.c_str(), "zyxba");
	ieq(r2, false);

	// strict iterators over strings
	const Char *buf2 = (const Char *)"aedcb";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_bidirectional_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 5;
	strict_bidirectional_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2, nrec2);
	chk(prev_permutation(it1, it2, f_2538T41b));
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_2538T41b");
#else
	skip_chk("_2538T41b");
#endif /* CASE_2538T41b */

/* _254T21 <cstdlib> synopsis */
#if !defined(SKIP_254T21)&&(!defined(ONLY)||defined(CASE_254T21))
	begin_chk("_254T21");
	{
	char abc[] = "fghijpqrstxyzuklabcdemnovw";
	qsort(abc, 26, 1, &cmp_254T21);
	chk((char *)bsearch("0", abc, 26, 1, &cmp_254T21) == 0);
	chk((char *)bsearch("c", abc, 26, 1, &cmp_254T21) == &abc[2]);



	}
	end_chk("_254T21");
#else
	skip_chk("_254T21");
#endif /* CASE_254T21 */

return leave_chk("t258");
}
/* V3.10:0009 */
