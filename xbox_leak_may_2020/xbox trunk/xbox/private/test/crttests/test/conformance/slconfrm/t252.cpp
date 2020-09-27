/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _2521T21 copy effects, returns, complexity */
#if !defined(SKIP_2521T21)&&(!defined(ONLY)||defined(CASE_2521T21))
#include "iter.h"
#include <algorithm>
#include <string>
int assts_2521T21 = 0;
class Mychar_2521T21 {
public:
	Mychar_2521T21(Char c = 0)
		: val(c) {}
	Mychar_2521T21(const Mychar_2521T21& x)
		: val(x.val) {}
	Mychar_2521T21& operator=(const Mychar_2521T21& x)
		{val = x.val;
		++assts_2521T21;
		return (*this); }
	~Mychar_2521T21()
		{ }
private:
	Char val;
	};
#endif /* CASE_2521T21 */

/* _2521T71 copy_backward effects, returns, complexity */
#if !defined(SKIP_2521T71)&&(!defined(ONLY)||defined(CASE_2521T71))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
int assts_2521T71 = 0;
class Mychar_2521T71 {
public:
	Mychar_2521T71(Char c = 0)
		: val(c) {}
	Mychar_2521T71(const Mychar_2521T71& x)
		: val(x.val) {}
	Mychar_2521T71& operator=(const Mychar_2521T71& x)
		{val = x.val;
		++assts_2521T71;
		return (*this); }
	~Mychar_2521T71()
		{ }
private:
	Char val;
	};
#endif /* CASE_2521T71 */

/* _2522T21 swap effects */
#if !defined(SKIP_2522T21)&&(!defined(ONLY)||defined(CASE_2522T21))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
#endif /* CASE_2522T21 */

/* _2522T51 swap_ranges effects, returns, complexity */
#if !defined(SKIP_2522T51)&&(!defined(ONLY)||defined(CASE_2522T51))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
int cnt_2522T51;
struct Mychar_2522T51 {
	Mychar_2522T51()
		: c(0) {}
	Mychar_2522T51(const Mychar_2522T51& x)
		{++cnt_2522T51;
		c = x.c; }
	Mychar_2522T51& operator=(const Mychar_2522T51& x)
		{++cnt_2522T51;
		c = x.c;
		return (*this); }
	char c;
	};
#endif /* CASE_2522T51 */

/* _2522T71 iter_swap effects */
#if !defined(SKIP_2522T71)&&(!defined(ONLY)||defined(CASE_2522T71))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
#endif /* CASE_2522T71 */

/* _2523T21a transform(-, unary_op) effects, returns, complexity */
#if !defined(SKIP_2523T21a)&&(!defined(ONLY)||defined(CASE_2523T21a))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <string>
int nops_2523T21a = 0;
int f_2523T21a(Char c1)
	{++nops_2523T21a;
	return (c1 + 1); }
#endif /* CASE_2523T21a */

/* _2523T21b transform(-, binaryop) effects, returns, complexity */
#if !defined(SKIP_2523T21b)&&(!defined(ONLY)||defined(CASE_2523T21b))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <string>
int nops_2523T21b = 0;
int f_2523T21b(Char c1, Char c2)
	{++nops_2523T21b;
	return (c1 + (c2 - 'a')); }
#endif /* CASE_2523T21b */

/* _2524T21a replace effects, complexity */
#if !defined(SKIP_2524T21a)&&(!defined(ONLY)||defined(CASE_2524T21a))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <string>
#endif /* CASE_2524T21a */

/* _2524T21b replace_if effects, complexity */
#if !defined(SKIP_2524T21b)&&(!defined(ONLY)||defined(CASE_2524T21b))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <string>
int nops_2524T21b = 0;
bool f_2524T21b(Char c1)
	{++nops_2524T21b;
	return (c1 == 'a' || c1 == 'b' || c1 == 'c'); }
#endif /* CASE_2524T21b */

/* _2524T41a replace_copy effects, returns */
#if !defined(SKIP_2524T41a)&&(!defined(ONLY)||defined(CASE_2524T41a))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <string>
#endif /* CASE_2524T41a */

/* _2524T41b replace_copy_if effects, complexity */
#if !defined(SKIP_2524T41b)&&(!defined(ONLY)||defined(CASE_2524T41b))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <string>
int nops_2524T41b = 0;
bool f_2524T41b(Char c1)
	{++nops_2524T41b;
	return (c1 == 'a' || c1 == 'b' || c1 == 'c'); }
#endif /* CASE_2524T41b */

/* _2525T21a fill effects, complexity */
#if !defined(SKIP_2525T21a)&&(!defined(ONLY)||defined(CASE_2525T21a))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
int assts_2525T21a = 0;
class Mychar_2525T21a {
public:
	Mychar_2525T21a(Char c = 0)
		: val(c) {}
	Mychar_2525T21a(const Mychar_2525T21a& x)
		: val(x.val) {}
	Mychar_2525T21a& operator=(const Mychar_2525T21a& x)
		{val = x.val;
		++assts_2525T21a;
		return (*this); }
	~Mychar_2525T21a()
		{ }
private:
	Char val;
	};
#endif /* CASE_2525T21a */

/* _2525T21b fill_n effects, complexity */
#if !defined(SKIP_2525T21b)&&(!defined(ONLY)||defined(CASE_2525T21b))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
int assts_2525T21b = 0;
class Mychar_2525T21b {
public:
	Mychar_2525T21b(Char c = 0)
		: val(c) {}
	Mychar_2525T21b(const Mychar_2525T21b& x)
		: val(x.val) {}
	Mychar_2525T21b& operator=(const Mychar_2525T21b& x)
		{val = x.val;
		++assts_2525T21b;
		return (*this); }
	~Mychar_2525T21b()
		{ }
private:
	Char val;
	};
#endif /* CASE_2525T21b */

/* _2526T21a generate    effects, complexity */
#if !defined(SKIP_2526T21a)&&(!defined(ONLY)||defined(CASE_2526T21a))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
int assts_2526T21a = 0;
class Mychar_2526T21a {
public:
	Mychar_2526T21a(Char c = 0)
		: val(c) {}
	Mychar_2526T21a(const Mychar_2526T21a& x)
		: val(x.val) {}
	Mychar_2526T21a& operator=(const Mychar_2526T21a& x)
		{val = x.val;
		++assts_2526T21a;
		return (*this); }
	~Mychar_2526T21a()
		{ }
private:
	Char val;
	};
int calls_2526T21a = 0;
int f_2526T21a()
	{return ('a' + calls_2526T21a++); }
#endif /* CASE_2526T21a */

/* _2526T21b generate_n  effects, complexity */
#if !defined(SKIP_2526T21b)&&(!defined(ONLY)||defined(CASE_2526T21b))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
int assts_2526T21b = 0;
class Mychar_2526T21b {
public:
	Mychar_2526T21b(Char c = 0)
		: val(c) {}
	Mychar_2526T21b(const Mychar_2526T21b& x)
		: val(x.val) {}
	Mychar_2526T21b& operator=(const Mychar_2526T21b& x)
		{val = x.val;
		++assts_2526T21b;
		return (*this); }
	~Mychar_2526T21b()
		{ }
private:
	Char val;
	};
int calls_2526T21b = 0;
int f_2526T21b()
	{return ('a' + calls_2526T21b++); }
#endif /* CASE_2526T21b */

int t252_main(int, char *[])
{
enter_chk("t252.cpp");
/* _2521T21 copy effects, returns, complexity */
#if !defined(SKIP_2521T21)&&(!defined(ONLY)||defined(CASE_2521T21))
	begin_chk("_2521T21");
	{
	// simple pointers
	Char buf[] = "abdx";
	Char bufb[5] = "yyy";
	copy(buf, buf, bufb);
	steq(bufb, "yyy");
	Char *ret = copy(buf, buf + 3, bufb);
	aeq(ret, bufb + 3);
	steq(bufb, "abd");

	// strings
	Ustr s ("xabcd");
	Ustr t ("yyyxx");
	Ustr::iterator ret2 (copy(s.begin(), s.end(), t.begin()));
	ieq(ret2 - t.begin(), 5);  // returns
	chk(t == "xabcd");         // effects
	steq(t.c_str(), "xabcd");

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcd";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_input_iterator<Ustr::const_iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::const_iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *ptrb = (const Char *)"xabcd";
	Ustr s4b (ptrb, ptrb+5);
	int err3 = 0, nrec3 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s4b.begin(), err3, nrec3);
	strict_forward_iterator<Ustr::iterator, Char> it4
		(copy(it1, it2, it3));
	chk(s4b == "xabcd");
	steq(s4b.c_str(), "xabcd");  // effects
	ieq(err1, 0); // no iterator violations
	ieq(err2, 0);
	ieq(err3, 0);

	// complexity
	Mychar_2521T21 a[10];
	assts_2521T21 = 0;
	copy(a, a + 5, a + 5);
	ieq(assts_2521T21, 5); // complexity, assignments


	}
	end_chk("_2521T21");
#else
	skip_chk("_2521T21");
#endif /* CASE_2521T21 */

/* _2521T71 copy_backward effects, returns, complexity */
#if !defined(SKIP_2521T71)&&(!defined(ONLY)||defined(CASE_2521T71))
	begin_chk("_2521T71");
	{
	// simple pointers
	Char buf[] = "abdyyy";
	copy_backward(buf, buf, buf + 4);
	steq(buf, "abdyyy");
	Char *ret = copy_backward(buf, buf + 3, buf + 4);
	aeq(ret, buf + 1);  // returns 
	steq(buf, "aabdyy");        // effects

	// strings
	Ustr s ("abcdx");
	Ustr::iterator ret2 (copy_backward(s.begin(), s.end()-1, s.end()));
	ieq(ret2 - s.begin(), 1);
	chk(s == "aabcd");

	// strict iterators over strings
	const Char *ptr = (const Char *)"abcxx";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 3;
	strict_bidirectional_iterator<Ustr::/*const_2521T71*/iterator, /*const */Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 3;
	strict_bidirectional_iterator<Ustr::/*const_2521T71*/iterator, /*const */Char> it2
		(s4.begin() + nrec2, err2, nrec2, nrec2);
	int err3 = 0, nrec3 = 3;
	strict_bidirectional_iterator<Ustr::iterator, Char> it3
		(s4.begin() + 5, err3, nrec3, nrec3);
	strict_bidirectional_iterator<Ustr::iterator, Char> it4
		(copy_backward(it1, it2, it3));
	chk(s4 == "ababc");
	steq(s4.c_str(), "ababc");
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);

	// complexity
	Mychar_2521T71 a[6];
	assts_2521T71 = 0;
	copy_backward(a, a + 5, a + 6);
	ieq(assts_2521T71, 5); // complexity, assignments


	}
	end_chk("_2521T71");
#else
	skip_chk("_2521T71");
#endif /* CASE_2521T71 */

/* _2522T21 swap effects */
#if !defined(SKIP_2522T21)&&(!defined(ONLY)||defined(CASE_2522T21))
	begin_chk("_2522T21");
	{
	// simple pointers
	Char buf[] = "abdx";
	Char *p1 = buf;
	Char *p2 = buf+3;
	swap(*p1, *p2);
	steq(buf, "xbda");
	swap(buf[3], buf[3]);  // same location
	steq(buf, "xbda");

	// strings
	Ustr s ("xabcd");
	swap(*s.begin(), *(s.end()-1));
	chk(s == "dabcx");

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcd";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 1;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin() + 1, err1, nrec1);
	int err2 = 0, nrec2 = 1;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.end() - 1, err2, nrec2);
	swap(*it1, *it2);
	chk(s4 == "xdbca");
	steq(s4.c_str(), "xdbca");
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_2522T21");
#else
	skip_chk("_2522T21");
#endif /* CASE_2522T21 */

/* _2522T51 swap_ranges effects, returns, complexity */
#if !defined(SKIP_2522T51)&&(!defined(ONLY)||defined(CASE_2522T51))
	begin_chk("_2522T51");
	{
	// simple pointers
	Char buf[] = "abdx";
	Char *p1 = buf;
	Char *p2 = buf+2;
	Char *p3 = buf+2;
	swap_ranges(p1, p1, p3);
	steq(buf, "abdx");
	Char *p4 = swap_ranges(p1, p2, p3);
	steq(buf, "dxab");
	aeq(p4, buf+4);  // check returns

	// strings
	Ustr s ("xabcd");
	swap_ranges(s.begin(), s.begin()+2, s.end()-2);
	chk(s == "cdbxa");

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcde";
	Ustr s4 (ptr, ptr+6);
	int err1 = 0, nrec1 = 3;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 3;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.begin() + nrec2, err2, nrec2, nrec2);
	int err3 = 0, nrec3 = 3;
	strict_random_access_iterator<Ustr::iterator, Char> it3
		(s4.begin() + 3, err3, nrec3);
	swap_ranges(it1, it2, it3);
	chk(s4 == "cdexab");
	steq(s4.c_str(), "cdexab");
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);

	// complexity
	Mychar_2522T51 a[10], b[10];
	cnt_2522T51 = 0;
	swap_ranges(&a[0], &a[10], &b[0]);
	ieq(cnt_2522T51, 30);


	}
	end_chk("_2522T51");
#else
	skip_chk("_2522T51");
#endif /* CASE_2522T51 */

/* _2522T71 iter_swap effects */
#if !defined(SKIP_2522T71)&&(!defined(ONLY)||defined(CASE_2522T71))
	begin_chk("_2522T71");
	{
	// simple pointers
	Char buf[] = "abdx";
	Char *p1 = buf;
	Char *p2 = buf+2;
	iter_swap(p1, p1);
	steq(buf, "abdx");
	iter_swap(p1, p2);
	steq(buf, "dbax");

	// strings
	Ustr s ("xabcd");
	iter_swap(s.begin()+1, s.begin()+3);
	chk(s == "xcbad");

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcde";
	Ustr s4 (ptr, ptr+6);
	int err1 = 0, nrec1 = 1;
	strict_random_access_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 1;
	strict_random_access_iterator<Ustr::iterator, Char> it2
		(s4.begin() + 3, err2, nrec2);
	iter_swap(it1, it2);
	chk(s4 == "cabxde");
	steq(s4.c_str(), "cabxde");
	ieq(err1, 0);
	ieq(err2, 0);



	}
	end_chk("_2522T71");
#else
	skip_chk("_2522T71");
#endif /* CASE_2522T71 */

/* _2523T21a transform(-, unary_op) effects, returns, complexity */
#if !defined(SKIP_2523T21a)&&(!defined(ONLY)||defined(CASE_2523T21a))
	begin_chk("_2523T21a");
	{
	// simple pointers
	Char buf[] = "abcd";
	Char bufc[] = "wxyz";
	Char *p5;
	transform(&buf[0], &buf[0], &bufc[0], f_2523T21a);
	steq(bufc, "wxyz");
	p5 = transform(&buf[0], &buf[4], &bufc[0], f_2523T21a);
	aeq(p5, &bufc[4]);
	steq(&bufc[0], "bcde");
	ieq(nops_2523T21a, 4);  // complexity

	// strings
	nops_2523T21a = 0;
	Ustr s ("axy");
	Ustr::iterator i =
		transform(s.begin(), s.end(), s.begin(), f_2523T21a);
	ieq(i - s.begin(), 3);
	ieq(*(i-1), 'z');
	steq(s.c_str(), "byz");
	chk(nops_2523T21a == 3);  // complexity

	// strict iterators over strings
	const Char *buf2 = (const Char *)"b";
	Ustr s4 (buf2, buf2+1);
	int err1 = 0, nrec1 = 1;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *buf3 = (const Char *)"xef";
	Ustr s5 (buf3, buf3+3);
	int err3 = 0, nrec3 = 1;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	transform(it1, it2, it3, f_2523T21a);
	steq(s5.c_str(), "cef");
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);

	}
	end_chk("_2523T21a");
#else
	skip_chk("_2523T21a");
#endif /* CASE_2523T21a */

/* _2523T21b transform(-, binaryop) effects, returns, complexity */
#if !defined(SKIP_2523T21b)&&(!defined(ONLY)||defined(CASE_2523T21b))
	begin_chk("_2523T21b");
	{
	// simple pointers
	Char buf[] = "abcd";
	Char bufb[] = "aabb";
	Char bufc[] = "wxyz";
	Char *p5;
	transform(&buf[0], &buf[0], &bufb[0], &bufc[0], f_2523T21b);
	steq(bufc, "wxyz");
	p5 = transform(&buf[0], &buf[4], &bufb[0], &bufc[0], f_2523T21b);
	aeq(p5, bufc+4);
	steq(bufc, "abde");
	ieq(nops_2523T21b, 4);  // complexity

	// strings
	nops_2523T21b = 0;
	Ustr s ("axy");
	Ustr t ("cba");
	Ustr::iterator i =
		transform(s.begin(), s.end(), t.begin(), t.begin(), f_2523T21b);
	ieq(i - t.begin(), 3);
	ieq(*(i-1), 'y');
	steq(t.c_str(), "cyy");
	chk(nops_2523T21b == 3);  // complexity

	// strict iterators over strings
	const STD plus<Char> x = plus<Char>();
	const Char *buf2 = (const Char *)"\1";
	Ustr s4 (buf2, buf2+1);
	int err1 = 0, nrec1 = 1;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *buf3 = (const Char *)"xef";
	Ustr s5 (buf3, buf3+3);
	int err3 = 0, nrec3 = 1;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 1;
	strict_forward_iterator<Ustr::iterator, Char> it4
		(s4.begin(), err4, nrec4);
	transform(it1, it2, it3, it4, x);
	steq(s4.c_str(), "y");
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);


	}
	end_chk("_2523T21b");
#else
	skip_chk("_2523T21b");
#endif /* CASE_2523T21b */

/* _2524T21a replace effects, complexity */
#if !defined(SKIP_2524T21a)&&(!defined(ONLY)||defined(CASE_2524T21a))
	begin_chk("_2524T21a");
	{
	// simple pointers
	Char buf[] = "abcd";
	replace(&buf[0], &buf[0], 'c', 'x');
	steq(buf, "abcd");
	replace(&buf[0], &buf[4], 'c', 'x');
	steq(buf, "abxd");

	// strings
	Ustr s ("axy");
	replace(s.begin(), s.end(), 'y', 'k');
	steq(s.c_str(), "axk");

	// strict iterators over strings
	const Char *buf2 = (const Char *)"b";
	Ustr s4 (buf2, buf2+1);
	int err1 = 0, nrec1 = 1;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	replace(it1, it2, 'b', 'x');
	steq(s4.c_str(), "x");
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_2524T21a");
#else
	skip_chk("_2524T21a");
#endif /* CASE_2524T21a */

/* _2524T21b replace_if effects, complexity */
#if !defined(SKIP_2524T21b)&&(!defined(ONLY)||defined(CASE_2524T21b))
	begin_chk("_2524T21b");
	{
	// simple pointers
	Char buf[] = "abcd";
	replace_if(&buf[0], &buf[0], f_2524T21b, 'x');
	steq(buf, "abcd");
	replace_if(&buf[0], &buf[4], f_2524T21b, 'x');
	steq(buf, "xxxd"); // effects
	ieq(nops_2524T21b, 4);  // complexity

	// strings
	nops_2524T21b = 0;
	Ustr s ("axy");
	replace_if(s.begin(), s.end(), f_2524T21b, 'j');
	steq(s.c_str(), "jxy");
	chk(nops_2524T21b == 3);  // complexity

	// strict iterators over strings
	const STD plus<int> x = plus<int>();
	const Char *buf2 = (const Char *)"xyabc";
	Ustr s4 (buf2, buf2+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	replace_if(it1, it2, f_2524T21b, 'e');
	steq(s4.c_str(), "xyeee");
	ieq(err1, 0);
	ieq(err2, 0);




	}
	end_chk("_2524T21b");
#else
	skip_chk("_2524T21b");
#endif /* CASE_2524T21b */

/* _2524T41a replace_copy effects, returns */
#if !defined(SKIP_2524T41a)&&(!defined(ONLY)||defined(CASE_2524T41a))
	begin_chk("_2524T41a");
	{
	// simple pointers
	Char buf[] = "abcd";
	Char bufb[] = "wxyz";
	replace_copy(&buf[0], &buf[0], &bufb[0], 'c', 'x');
	steq(bufb, "wxyz");
	replace_copy(&buf[0], &buf[4], &bufb[0], 'c', 'x');
	steq(bufb, "abxd");

	// strings
	Ustr s ("axy");
	Ustr t ("lmn");
	replace_copy(s.begin(), s.end(), t.begin(), 'y', 'k');
	steq(t.c_str(), "axk");

	// strict iterators over strings
	const Char *buf2 = (const Char *)"b";
	Ustr s4 (buf2, buf2+1);
	int err1 = 0, nrec1 = 1;
	strict_input_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	Char *ptrb = "z";
	Ustr s4b (ptrb, ptrb+1);
	int err3 = 0, nrec3 = 1;
	strict_output_iterator<Ustr::iterator, Char> it3
		(s4b.begin(), err3, nrec3);
	strict_output_iterator<Ustr::iterator, Char> it4
		(replace_copy(it1, it2, it3, 'b', 'x'));
	steq(s4b.c_str(), "x");
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);

	}
	end_chk("_2524T41a");
#else
	skip_chk("_2524T41a");
#endif /* CASE_2524T41a */

/* _2524T41b replace_copy_if effects, complexity */
#if !defined(SKIP_2524T41b)&&(!defined(ONLY)||defined(CASE_2524T41b))
	begin_chk("_2524T41b");
	{
	// simple pointers
	Char buf[] = "abcd";
	Char bufb[] = "wxyz";
	replace_copy_if(&buf[0], &buf[0], &bufb[0], f_2524T41b, 'x');
	steq(bufb, "wxyz");
	replace_copy_if(&buf[0], &buf[4], &bufb[0], f_2524T41b, 'x');
	steq(bufb, "xxxd"); // effects
	ieq(nops_2524T41b, 4);  // complexity

	// strings
	nops_2524T41b = 0;
	Ustr s ("axy");
	Ustr t ("lmn");
	replace_copy_if(s.begin(), s.end(), t.begin(), f_2524T41b, 'j');
	steq(t.c_str(), "jxy");
	chk(nops_2524T41b == 3);  // complexity

	// strict iterators over strings
	const STD plus<int> x = plus<int>();
	const Char *buf2 = (const Char *)"xyabc";
	Ustr s4 (buf2, buf2+5);
	int err1 = 0, nrec1 = 5;
	strict_input_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	Char *ptrb = "lmnop";
	Ustr s4b (ptrb, ptrb+5);
	int err3 = 0, nrec3 = 5;
	strict_output_iterator<Ustr::iterator, Char> it3
		(s4b.begin(), err3, nrec3);
	nops_2524T41b = 0;
	strict_output_iterator<Ustr::iterator, Char> it4
		(replace_copy_if(it1, it2, it3, f_2524T41b, 'e'));
	steq(s4b.c_str(), "xyeee");
	ieq(nops_2524T41b, 5);
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);




	}
	end_chk("_2524T41b");
#else
	skip_chk("_2524T41b");
#endif /* CASE_2524T41b */

/* _2525T21a fill effects, complexity */
#if !defined(SKIP_2525T21a)&&(!defined(ONLY)||defined(CASE_2525T21a))
	begin_chk("_2525T21a");
	{

	// simple pointers
	Char buf[] = "abcdyy";
	fill(buf, buf, 'z');
	steq(buf, "abcdyy");
	fill(buf, buf + 3, 'z');
	steq(buf, "zzzdyy");        // effects

	// strings
	Ustr s ("abcdx");
	fill(s.begin(), s.end()-1, 'm');
	chk(s == "mmmmx");

	// strict iterators over strings
	const Char *ptr = (const Char *)"abcxx";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	fill(it1, it2, 'n');
	chk(s4 == "nnnnn");
	steq(s4.c_str(), "nnnnn");
	ieq(err1, 0);
	ieq(err2, 0);

	// complexity
	Mychar_2525T21a a[10];
	assts_2525T21a = 0;
	fill(a, a + 10, Mychar_2525T21a('x'));
	ieq(assts_2525T21a, 10); // complexity, assignments

	}
	end_chk("_2525T21a");
#else
	skip_chk("_2525T21a");
#endif /* CASE_2525T21a */

/* _2525T21b fill_n effects, complexity */
#if !defined(SKIP_2525T21b)&&(!defined(ONLY)||defined(CASE_2525T21b))
	begin_chk("_2525T21b");
	{

	// simple pointers
	Char buf[] = "abcdyy";
	fill_n(buf, 0, 'z');
	steq(buf, "abcdyy");
	fill_n(buf, 2, 'z');
	steq(buf, "zzcdyy");        // effects

	// strings
	Ustr s ("abcdx");
	fill_n(s.begin(), 4, 'm');
	chk(s == "mmmmx");

	// strict iterators over strings
	const Char *ptr = (const Char *)"abcxx";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 2;
	strict_output_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	fill_n(it1, 2, 'n');
	steq(s4.c_str(), "nncxx");
	ieq(err1, 0);

	// complexity
	Mychar_2525T21b a[10];
	assts_2525T21b = 0;
	fill_n(a, 10, Mychar_2525T21b('x'));
	ieq(assts_2525T21b, 10); // complexity, assignments


	}
	end_chk("_2525T21b");
#else
	skip_chk("_2525T21b");
#endif /* CASE_2525T21b */

/* _2526T21a generate    effects, complexity */
#if !defined(SKIP_2526T21a)&&(!defined(ONLY)||defined(CASE_2526T21a))
	begin_chk("_2526T21a");
	{
	// simple pointers
	Char buf[] = "uvwxyz";
	generate(buf, buf, f_2526T21a);
	steq(buf, "uvwxyz");        // effects
	generate(buf, buf + 3, f_2526T21a);
	steq(buf, "abcxyz");

	// strings
	Ustr s ("abcdx");
	calls_2526T21a = 3;
	generate(s.begin(), s.end()-1, f_2526T21a);
	chk(s == "defgx");
	ieq(calls_2526T21a, 7); // complexity

	// strict iterators over strings
	const Char *ptr = (const Char *)"abcxx";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	calls_2526T21a = 7;
	generate(it1, it2, f_2526T21a);
	chk(s4 == "hijkl");
	steq(s4.c_str(), "hijkl");
	ieq(calls_2526T21a, 12); // complexity
	ieq(err1, 0);
	ieq(err2, 0);

	// complexity
	Mychar_2526T21a a[10];
	assts_2526T21a = 0;
	calls_2526T21a = 0;
	generate(a, a + 10, f_2526T21a);
	ieq(assts_2526T21a, 10); // complexity, assignments
	ieq(calls_2526T21a, 10); // complexity, invocations

	}
	end_chk("_2526T21a");
#else
	skip_chk("_2526T21a");
#endif /* CASE_2526T21a */

/* _2526T21b generate_n  effects, complexity */
#if !defined(SKIP_2526T21b)&&(!defined(ONLY)||defined(CASE_2526T21b))
	begin_chk("_2526T21b");
	{
	// simple pointers
	Char buf[] = "uvwxyz";
	generate_n(buf, 0, f_2526T21b);
	steq(buf, "uvwxyz");        // effects
	generate_n(buf, 3, f_2526T21b);
	steq(buf, "abcxyz");

	// strings
	Ustr s ("abcdx");
	calls_2526T21b = 3;
	generate_n(s.begin(), 4, f_2526T21b);
	chk(s == "defgx");
	ieq(calls_2526T21b, 7); // complexity

	// strict iterators over strings
	const Char *ptr = (const Char *)"abcxx";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_output_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	calls_2526T21b = 7;
	generate_n(it1, 5, f_2526T21b);
	chk(s4 == "hijkl");
	steq(s4.c_str(), "hijkl");
	ieq(calls_2526T21b, 12); // complexity
	ieq(err1, 0);

	// complexity
	Mychar_2526T21b a[10];
	assts_2526T21b = 0;
	calls_2526T21b = 0;
	generate_n(a, 10, f_2526T21b);
	ieq(assts_2526T21b, 10); // complexity, assignments
	ieq(calls_2526T21b, 10); // complexity, invocations

	}
	end_chk("_2526T21b");
#else
	skip_chk("_2526T21b");
#endif /* CASE_2526T21b */

return leave_chk("t252");
}
/* V3.10:0009 */
