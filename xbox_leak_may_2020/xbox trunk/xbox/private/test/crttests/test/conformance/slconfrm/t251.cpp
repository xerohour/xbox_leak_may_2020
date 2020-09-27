/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _2511T21a for_each    effects, returns, complexity, notes */
#if !defined(SKIP_2511T21a)&&(!defined(ONLY)||defined(CASE_2511T21a))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
int sum_2511T21a = 0;
int f_2511T21a(Char c)
	{sum_2511T21a += c;
	return (0); }
typedef int (*f_type_2511T21a)(Char c);
f_type_2511T21a pf_2511T21a = 0;
#endif /* CASE_2511T21a */

/* _2512T21a find    returns, complexity */
#if !defined(SKIP_2512T21a)&&(!defined(ONLY)||defined(CASE_2512T21a))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
#endif /* CASE_2512T21a */

/* _2512T21b find_if    returns, complexity */
#if !defined(SKIP_2512T21b)&&(!defined(ONLY)||defined(CASE_2512T21b))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
#include <functional>
bool f_2512T21b(Char c)
	{return (c == 'b'); }
class equal_to_b_2512T21b 
	{
public:
	int operator() (Char c)
		{
		const STD equal_to<Char> x = equal_to<Char>();
		return x(c, 'b');
		}
	};
#endif /* CASE_2512T21b */

/* _2513T31a find_end(-) effects, returns */
#if !defined(SKIP_2513T31a)&&(!defined(ONLY)||defined(CASE_2513T31a))
#include "iter.h"
#include <algorithm>
#include <cstring>
#include <string>
#endif /* CASE_2513T31a */

/* _2513T31b find_end(-, pred) effects, returns, complexity */
#if !defined(SKIP_2513T31b)&&(!defined(ONLY)||defined(CASE_2513T31b))
#include "iter.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <functional>
int npreds_2513T31b;
bool f_2513T31b(Char c, Char c2)
	{++npreds_2513T31b;
	return (tolower(c) == tolower(c2)); }
#endif /* CASE_2513T31b */

/* _2514T31a find_first_of(-) effects, returns, complexity */
#if !defined(SKIP_2514T31a)&&(!defined(ONLY)||defined(CASE_2514T31a))
#include <algorithm>
#include <string>
#include <cstring>
#include "iter.h"
#endif /* CASE_2514T31a */

/* _2514T31b find_first_of(-, pred) effects, returns, complexity */
#if !defined(SKIP_2514T31b)&&(!defined(ONLY)||defined(CASE_2514T31b))
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <functional>
#include "iter.h"
int npreds_2514T31b;
bool f_2514T31b(Char c1, Char c2)
	{++npreds_2514T31b;
	return (tolower(c1) == tolower(c2)); }
#endif /* CASE_2514T31b */

/* _2515T31a adjacent_find(-) effects, returns, complexity */
#if !defined(SKIP_2515T31a)&&(!defined(ONLY)||defined(CASE_2515T31a))
#include <algorithm>
#include <string>
#include <cstring>
#include "iter.h"
#endif /* CASE_2515T31a */

/* _2515T31b adjacent_find(-, pred) effects, returns, complexity */
#if !defined(SKIP_2515T31b)&&(!defined(ONLY)||defined(CASE_2515T31b))
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <functional>
#include "iter.h"
int npreds_2515T31b = 0;
bool f_2515T31b(Char c1, Char c2)
	{++npreds_2515T31b;
	return (tolower(c1) == tolower(c2)); }
#endif /* CASE_2515T31b */

/* _2516T31a count effects, complexity */
#if !defined(SKIP_2516T31a)&&(!defined(ONLY)||defined(CASE_2516T31a))
#include <algorithm>
#include <string>
#include <cstring>
#include "iter.h"
#endif /* CASE_2516T31a */

/* _2516T31b count_if   effects, complexity */
#if !defined(SKIP_2516T31b)&&(!defined(ONLY)||defined(CASE_2516T31b))
#include <algorithm>
#include <cstring>
#include <string>
#include <functional>
#include "iter.h"
int npreds_2516T31b = 0;
bool f_2516T31b(Char c1)
	{++npreds_2516T31b;
	return (c1 == 'b'); }
class not_equal_to_b_2516T31b 
	{
public:
	int operator() (Char c)
		{
		const STD equal_to<Char> x = equal_to<Char>();
		return !x(c, 'b');
		}
	};
#endif /* CASE_2516T31b */

/* _2517T31a mismatch(-)  returns, complexity */
#if !defined(SKIP_2517T31a)&&(!defined(ONLY)||defined(CASE_2517T31a))
#include <algorithm>
#include <string>
#include <cstring>
#include "iter.h"
#endif /* CASE_2517T31a */

/* _2517T31b mismatch(-, pred) returns, complexity */
#if !defined(SKIP_2517T31b)&&(!defined(ONLY)||defined(CASE_2517T31b))
#include <algorithm>
#include <cstring>
#include <string>
#include <functional>
#include "iter.h"
int npreds_2517T31b = 0;
bool f_2517T31b(Char c1, Char c2)
	{++npreds_2517T31b;
	return (c1 == c2); }
const STD equal_to<Char> x_2517T31b = equal_to<Char>();
#endif /* CASE_2517T31b */

/* _2518T31a equal(-)  returns [not complexity] */
#if !defined(SKIP_2518T31a)&&(!defined(ONLY)||defined(CASE_2518T31a))
#include <algorithm>
#include <string>
#include <cstring>
#include "iter.h"
#endif /* CASE_2518T31a */

/* _2518T31b equal(-, pred) returns, complexity */
#if !defined(SKIP_2518T31b)&&(!defined(ONLY)||defined(CASE_2518T31b))
#include <algorithm>
#include <cstring>
#include <string>
#include <functional>
#include "iter.h"
int npreds_2518T31b = 0;
bool f_2518T31b(Char c1, Char c2)
	{++npreds_2518T31b;
	return (c1 == c2); }
#endif /* CASE_2518T31b */

/* _2519T31a search(-)        effects, returns, complexity */
#if !defined(SKIP_2519T31a)&&(!defined(ONLY)||defined(CASE_2519T31a))
#include <algorithm>
#include <string>
#include <cstring>
#include "iter.h"
#endif /* CASE_2519T31a */

/* _2519T31b search(-, pred)  effects, returns, complexity */
#if !defined(SKIP_2519T31b)&&(!defined(ONLY)||defined(CASE_2519T31b))
#include <algorithm>
#include <cstring>
#include <string>
#include <functional>
#include "iter.h"
int npreds_2519T31b;
bool f_2519T31b(Char c1, Char c2)
	{++npreds_2519T31b;
	return (c1 == c2); }
#endif /* CASE_2519T31b */

/* _2519T31c search_n(-)        effects, returns, complexity ADDED [MAR96] */
#if !defined(SKIP_2519T31c)&&(!defined(ONLY)||defined(CASE_2519T31c))
#include <algorithm>
#include <string>
#include <cstring>
#include "iter.h"
#endif /* CASE_2519T31c */

/* _2519T31d search_n(-, pred)  effects, returns, complexity ADDED [MAR96] */
#if !defined(SKIP_2519T31d)&&(!defined(ONLY)||defined(CASE_2519T31d))
#include <algorithm>
#include <cstring>
#include <string>
#include <functional>
#include "iter.h"
int npreds_2519T31d;
bool f_2519T31d(Char c1, Char c2)
	{++npreds_2519T31d;
	return (c1 == c2); }
#endif /* CASE_2519T31d */

int t251_main(int, char *[])
{
enter_chk("t251.cpp");
/* _2511T21a for_each    effects, returns, complexity, notes */
#if !defined(SKIP_2511T21a)&&(!defined(ONLY)||defined(CASE_2511T21a))
	begin_chk("_2511T21a");
	{
	// simple pointers
	Char buf[] = "\1\2";
	sum_2511T21a = 0;
	pf_2511T21a = for_each(buf, buf, f_2511T21a);
	chk(pf_2511T21a == f_2511T21a);
	ieq(sum_2511T21a, 0);
	sum_2511T21a = 0;
	pf_2511T21a = 0;
	Char *p1 = buf;
	Char *p2 = buf+3;
	pf_2511T21a = for_each(p1, p2, f_2511T21a);
	chk(pf_2511T21a == f_2511T21a);
	ieq(sum_2511T21a, 3);

	// strings
	sum_2511T21a = 0;
	pf_2511T21a = 0;
	Ustr s("\1\2");
	pf_2511T21a = for_each(s.begin(), s.end(), f_2511T21a);
	chk(pf_2511T21a == f_2511T21a);
	ieq(sum_2511T21a, 3);

	// strict iterators over strings
	sum_2511T21a = 0;
	pf_2511T21a = 0;
	const Char *buf2 = (const Char *)"abcd";
	Ustr s4 (buf2, buf2+4);
	int err1 = 0, nrec1 = 4;
	strict_input_iterator<Ustr::/*const_2511T21a*/iterator, /*const */Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::/*const_2511T21a*/iterator, /*const */Char> it2
		(s4.end(), err2, nrec2);
	pf_2511T21a = for_each(it1, it2, f_2511T21a);
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_2511T21a");
#else
	skip_chk("_2511T21a");
#endif /* CASE_2511T21a */

/* _2512T21a find    returns, complexity */
#if !defined(SKIP_2512T21a)&&(!defined(ONLY)||defined(CASE_2512T21a))
	begin_chk("_2512T21a");
	{
	// simple pointers
	Char buf[] = "ab";
	Char *p1 = buf;
	Char *p2 = buf+3;
	Char *p3;
	p3 = find(p1, p1, 'b');
	aeq(p3, buf);
	p3 = find(p1, p2, 'b');
	aeq(p3, buf+1);
	ieq(*p3, 'b');

	// strings
	Ustr s("ab");
	ieq(*find(s.begin(), s.end(), 'b'), 'b');

	// iterators over strings
	const Char *buf2 = (const Char *)"abcd";
	Ustr s4(buf2, buf2+4);
	int err1 = 0, nrec1 = 4;
	strict_input_iterator<Ustr::/*const_2512T21a*/iterator, /*const */Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::/*const_2512T21a*/iterator, /*const */Char> it2
		(s4.end(), err2, nrec2);
	ieq(*find(it1, it2, 'b'), 'b');
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_2512T21a");
#else
	skip_chk("_2512T21a");
#endif /* CASE_2512T21a */

/* _2512T21b find_if    returns, complexity */
#if !defined(SKIP_2512T21b)&&(!defined(ONLY)||defined(CASE_2512T21b))
	begin_chk("_2512T21b");
	{
	// simple pointers
	Char buf[] = "ab";
	Char *p1 = buf;
	Char *p2 = buf+3;
	Char *p3;
	p3 = find_if(p1, p1, f_2512T21b);
	aeq(p3, buf);
	p3 = find_if(p1, p2, f_2512T21b);
	aeq(p3, buf+1);
	ieq(*p3, 'b');

	// strings
	Ustr s ("ab");
	Ustr::iterator p4 = s.begin();
	Ustr::iterator p5 = find_if(s.begin(), s.end(), f_2512T21b);
	aeq(&*p5, &*p4+1);
	ieq(*p5, 'b');

	// strict iterators over strings
	const Char *buf2 = (const Char *)"abcd";
	Ustr s4(buf2, buf2+4);
	int err1 = 0, nrec1 = 4;
	strict_input_iterator<Ustr::/*const_2512T21b*/iterator, /*const */Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::/*const_2512T21b*/iterator, /*const */Char> it2
		(s4.end(), err2, nrec2);
	equal_to_b_2512T21b pred;
	ieq(*find_if(it1, it2, pred), 'b');
	ieq(err1, 0);
	ieq(err2, 0);








	}
	end_chk("_2512T21b");
#else
	skip_chk("_2512T21b");
#endif /* CASE_2512T21b */

/* _2513T31a find_end(-) effects, returns */
#if !defined(SKIP_2513T31a)&&(!defined(ONLY)||defined(CASE_2513T31a))
	begin_chk("_2513T31a");
	{
	// simple pointers
	Char buf0[] = "abcbc";
	Char *p1 = buf0;
	Char *p2 = buf0 + 5;
	Char *p3;
	Char buf1[] = "bc";
	Char *p4 = buf1;
	Char *p5 = buf1 + 2;
	p3 = find_end(p1, p1, p4, p5);
	aeq(p3, p1);
	p3 = find_end(p1, p2, p4, p4);
	aeq(p3, p2);
	p3 = find_end(p1, p2, p4, p5);
	aeq(p3, buf0 + 3);	// [NOV95]
	ieq(*p3, 'b');

	// strings
	Ustr s("xab");
	Ustr t("ab");
	ieq(*find_end(s.begin(), s.end(), t.begin(), t.end()), 'a');	// [NOV95]

	// strict iterators over strings
	const Char *buf2 = (const Char *)"xabcd";
	Ustr s4(buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::/*const_2513T31a*/iterator, /*const */Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::/*const_2513T31a*/iterator, /*const */Char> it2
		(s4.end(), err2, nrec2);
	const Char *buf3 = (const Char *)"ab";
	Ustr s5 (buf3, buf3 + 2);
	int err3 = 0, nrec3 = 2;
	strict_forward_iterator<Ustr::/*const_2513T31a*/iterator, /*const */Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 0;
	strict_forward_iterator<Ustr::/*const_2513T31a*/iterator, /*const */Char> it4
		(s5.end(), err4, nrec4);
	ieq(*find_end(it1, it2, it3, it4), 'a');	// [NOV95]
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);

	}
	end_chk("_2513T31a");
#else
	skip_chk("_2513T31a");
#endif /* CASE_2513T31a */

/* _2513T31b find_end(-, pred) effects, returns, complexity */
#if !defined(SKIP_2513T31b)&&(!defined(ONLY)||defined(CASE_2513T31b))
	begin_chk("_2513T31b");
	{
	// simple pointers
	Char buf0[] = "abcbC";
	Char *p1 = buf0;
	Char *p2 = buf0 + 5;
	Char *p3;
	Char buf1[] = "Bc";
	Char *p4 = buf1;
	Char *p5 = buf1 + 2;
	npreds_2513T31b = 0;
	p3 = find_end(p1, p1, p4, p5, f_2513T31b);
	aeq(p3, p1);
	p3 = find_end(p1, p2, p4, p4, f_2513T31b);
	aeq(p3, p2);
	p3 = find_end(p1, p2, p4, p5, f_2513T31b);
	aeq(p3, buf0 + 3);	// [NOV95]
	ieq(*p3, 'b');
	chk(npreds_2513T31b <= 9);

	// strings
	Ustr s("xab");
	Ustr t("AB");
	npreds_2513T31b = 0;
	ieq(*find_end(s.begin(), s.end(), t.begin(), t.end(), f_2513T31b), 'a');	// [NOV95]
	chk(npreds_2513T31b <= 6);

	// strict iterators over strings
	const STD equal_to<Char> x = equal_to<Char>();
	const Char *buf2 = (const Char *)"xabcd";
	Ustr s4 (buf2, buf2 + 5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *buf3 = (const Char *)"ab";
	Ustr s5 (buf3, buf3 + 2);
	int err3 = 0, nrec3 = 2;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it4
		(s5.end(), err4, nrec4);
	ieq(*find_end(it1, it2, it3, it4, x), 'a');	// [NOV95]
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);



	}
	end_chk("_2513T31b");
#else
	skip_chk("_2513T31b");
#endif /* CASE_2513T31b */

/* _2514T31a find_first_of(-) effects, returns, complexity */
#if !defined(SKIP_2514T31a)&&(!defined(ONLY)||defined(CASE_2514T31a))
	begin_chk("_2514T31a");
	{
	// simple pointers
	Char buf[] = "ab";
	Char *p1 = buf;
	Char *p2 = buf+2;
	Char *p3;
	Char *p4 = (Char *)"b";
	Char *p5 = p4+1;
	p3 = find_first_of(p1, p1, p4, p5);
	aeq(p3, p1);
	p3 = find_first_of(p1, p2, p4, p4);
	aeq(p3, p2);
	p3 = find_first_of(p1, p2, p4, p5);
	aeq(p3, buf+1);
	ieq(*p3, 'b');

	// strings
	Ustr s("xab");
	Ustr t("ab");
	ieq(*find_first_of(s.begin(), s.end(), t.begin(), t.end()), 'a');

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcd";
	Ustr s4(ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *ptr2 = (const Char *)"ab";
	Ustr s5 (ptr2, ptr2+2);
	int err3 = 0, nrec3 = 2;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it4
		(s5.end(), err4, nrec4);
	ieq(*find_first_of(it1, it2, it3, it4), 'a');
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);

	}
	end_chk("_2514T31a");
#else
	skip_chk("_2514T31a");
#endif /* CASE_2514T31a */

/* _2514T31b find_first_of(-, pred) effects, returns, complexity */
#if !defined(SKIP_2514T31b)&&(!defined(ONLY)||defined(CASE_2514T31b))
	begin_chk("_2514T31b");
	{
	// simple pointers
	Char buf[] = "ab";
	Char *p1 = buf;
	Char *p2 = buf+2;
	Char *p3;
	Char *p4 = (Char *)"B";
	Char *p5 = p4+1;
	npreds_2514T31b = 0;
	p3 = find_first_of(p1, p2, p4, p5, f_2514T31b);
	aeq(p3, buf+1);
	ieq(*p3, 'b');
	chk(npreds_2514T31b <= 2);

	// strings
	Ustr s("xabab");
	Ustr t("AB");
	npreds_2514T31b = 0;
	Ustr::iterator i =
		find_first_of(s.begin(), s.end(), t.begin(), t.end(), f_2514T31b);
	ieq(*(i+1), 'b');
	ieq(*i, 'a');
	ieq(*(i-1), 'x');
	chk(npreds_2514T31b <= 10);	// [NOV95]

	// strict iterators over strings
	const STD equal_to<Char> x = equal_to<Char>();
	const Char *buf2 = (const Char *)"xabcd";
	Ustr s4 (buf2, buf2+4);
	int err1 = 0, nrec1 = 4;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *buf3 = (const Char *)"ab";
	Ustr s5 (buf3, buf3+2);
	int err3 = 0, nrec3 = 4;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it4
		(s5.end(), err4, nrec4);
	ieq(*find_first_of(it1, it2, it3, it4, x), 'a');
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);




	// We will validate adjacent_find using forward_iterator.



	}
	end_chk("_2514T31b");
#else
	skip_chk("_2514T31b");
#endif /* CASE_2514T31b */

/* _2515T31a adjacent_find(-) effects, returns, complexity */
#if !defined(SKIP_2515T31a)&&(!defined(ONLY)||defined(CASE_2515T31a))
	begin_chk("_2515T31a");
	{
	// simple pointers
	Char buf[] = "abb";
	Char *p1 = buf;
	Char *p2 = buf+3;
	Char *p3;
	p3 = adjacent_find(p1, p1);
	aeq(p3, p1);
	p3 = adjacent_find(p1, p1 + 1);
	aeq(p3, p1 + 1);
	p3 = adjacent_find(p1, p2);
	aeq(p3, buf+1);
	ieq(*p3, 'b');
	p3 = adjacent_find(p1, p2 - 1);  // no repeated instance
	aeq(p3, p2 - 1);
	ieq(*p3, 'b');

	// strings
	Ustr s("xbb");
	ieq(*adjacent_find(s.begin(), s.end()), 'b');

	// strict iterators over strings
	const Char *buf2 = (const Char *)"xbbcd";
	Ustr s4 (buf2, buf2+4);
	int err1 = 0, nrec1 = 4;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	strict_forward_iterator<Ustr::iterator, Char> it 
		(adjacent_find(it1, it2));
	ieq(*it, 'b');
	ieq(*++it, 'b');
	ieq(*++it, 'c');
	ieq(err1, 0);
	ieq(err2, 0);
		
	}
	end_chk("_2515T31a");
#else
	skip_chk("_2515T31a");
#endif /* CASE_2515T31a */

/* _2515T31b adjacent_find(-, pred) effects, returns, complexity */
#if !defined(SKIP_2515T31b)&&(!defined(ONLY)||defined(CASE_2515T31b))
	begin_chk("_2515T31b");
	{
	// simple pointers
	Char buf[] = "abb";
	Char *p1 = buf;
	Char *p2 = buf+3;
	Char *p3;
	p3 = adjacent_find(p1, p2, f_2515T31b);
	aeq(p3, buf+1);
	ieq(*p3, 'b');
	ieq(npreds_2515T31b, 2); // complexity

	// strings
	Ustr s("xbBab");
	npreds_2515T31b = 0;
	Ustr::iterator i =
		adjacent_find(s.begin(), s.end(), f_2515T31b);
	ieq(*i, 'b');
	ieq(*++i, 'B');
	ieq(npreds_2515T31b, 2); // complexity

	// strict iterators over strings
	const STD equal_to<Char> x = equal_to<Char>();
	const Char *buf2 = (const Char *)"xbbcd";
	Ustr s4 (buf2, buf2+4);
	int err1 = 0, nrec1 = 4;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	strict_forward_iterator<Ustr::iterator, Char> it 
		(adjacent_find(it1, it2, x));
	ieq(*it, 'b');
	ieq(*++it, 'b');
	ieq(*++it, 'c');
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_2515T31b");
#else
	skip_chk("_2515T31b");
#endif /* CASE_2515T31b */

/* _2516T31a count effects, complexity */
#if !defined(SKIP_2516T31a)&&(!defined(ONLY)||defined(CASE_2516T31a))
	begin_chk("_2516T31a");
	{
	// simple pointers
	Char buf[] = "ab";
	Char *p1 = buf;
	Char *p2 = buf+2;
	size_t sum = count(p1, p1, 'b');	// CHANGED [MAR96]
	ieq(sum, 0);
	sum = count(p1, p1 + 1, 'b');
	ieq(sum, 0);
	sum = count(p1, p2, 'b');
	ieq(sum, 1);

	// strings
	Ustr s ("xab");
	sum = count(s.begin(), s.end(), 'b');	// CHANGED [MAR96]
	ieq(sum, 1);

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcd";
	Ustr s4 (ptr, ptr+4);
	int err1 = 0, nrec1 = 4;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 2;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	sum = count(it1, it2, 'b');	// CHANGED [MAR96]
	ieq(sum, 1);
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_2516T31a");
#else
	skip_chk("_2516T31a");
#endif /* CASE_2516T31a */

/* _2516T31b count_if   effects, complexity */
#if !defined(SKIP_2516T31b)&&(!defined(ONLY)||defined(CASE_2516T31b))
	begin_chk("_2516T31b");
	{
	// simple pointers
	Char buf[] = "ab";
	Char *p1 = buf;
	Char *p2 = buf+2;
	size_t sum = count_if(p1, p2, f_2516T31b);	// CHANGED [MAR96]
	ieq(sum, 1);
	chk(npreds_2516T31b == 2);

	// strings
	npreds_2516T31b = 0;
	Ustr s ("xabab");
	sum = count_if(s.begin(), s.end(), f_2516T31b);
	ieq(sum, 2);
	chk(npreds_2516T31b == 5);

	// strict iterators over strings
	not_equal_to_b_2516T31b x = not_equal_to_b_2516T31b();
	const Char *buf2 = (const Char *)"xabcd";
	Ustr s4 (buf2, buf2+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	sum = count_if(it1, it2, x);
	ieq(sum, 4);
	ieq(err1, 0);
	ieq(err2, 0);


	}
	end_chk("_2516T31b");
#else
	skip_chk("_2516T31b");
#endif /* CASE_2516T31b */

/* _2517T31a mismatch(-)  returns, complexity */
#if !defined(SKIP_2517T31a)&&(!defined(ONLY)||defined(CASE_2517T31a))
	begin_chk("_2517T31a");
	{
	// simple pointers
	Char buf[] = "abxd";
	Char *p1 = buf;
	Char *p2 = buf+4;
	Char bufb[4] = "abd";
	Char *p3 = bufb;
	pair<Char*,Char*> r (mismatch(p1, p2, p3));
	ieq(*r.first, 'x');
	ieq(*r.second, 'd');
	r = mismatch(p2, p2, p3);
	chk(r.first == p2);
	ieq(*r.second, 'a');

	// strings
	Ustr s ("xabcd");
	Ustr t ("xab");
	pair<Ustr::iterator, Ustr::iterator>
		r2(mismatch(s.begin(), s.end(), t.begin()));
	ieq(*r2.first, 'c');
	ieq(r2.second - t.begin(), 3);

	// strict iterators over strings
	const Char *ptr = (const Char *)"abcdy";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *ptrb = (const Char *)"abcdx";
	Ustr s4b (ptrb, ptrb+5);
	int err3 = 0, nrec3 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s4b.begin(), err3, nrec3);
	typedef strict_forward_iterator<Ustr::iterator, Char> FI;
	pair<FI, FI> r3 (mismatch(it1, it2, it3));
	ieq(*r3.first, 'y');
	ieq(*r3.second, 'x');
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);

	}
	end_chk("_2517T31a");
#else
	skip_chk("_2517T31a");
#endif /* CASE_2517T31a */

/* _2517T31b mismatch(-, pred) returns, complexity */
#if !defined(SKIP_2517T31b)&&(!defined(ONLY)||defined(CASE_2517T31b))
	begin_chk("_2517T31b");
	{
	// simple pointers
	Char buf[] = "abxd";
	Char *p1 = buf;
	Char *p2 = buf+4;
	Char bufb[4] = "abd";
	Char *p3 = bufb;
	pair<Char*,Char*> r (mismatch(p1, p2, p3, f_2517T31b));
	ieq(*r.first, 'x');
	ieq(*r.second, 'd');
	chk(npreds_2517T31b <= 4);

	// strings
	Ustr s ("xabcd");
	Ustr t ("xab");
	pair<Ustr::iterator, Ustr::iterator>
		r2(mismatch(s.begin(), s.end(), t.begin(), f_2517T31b));
	ieq(*r2.first, 'c');
	ieq(r2.second - t.begin(), 3);

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcd";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *ptrb = (const Char *)"abcd";
	Ustr s4b (ptrb, ptrb+4);
	int err3 = 0, nrec3 = 4;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s4b.begin(), err3, nrec3);
	typedef strict_forward_iterator<Ustr::iterator, Char> FI;
	pair<FI, FI> r3 (mismatch(it1, it2, it3, x_2517T31b));
	ieq(*r3.first, 'x');
	ieq(*r3.second, 'a');
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);


	}
	end_chk("_2517T31b");
#else
	skip_chk("_2517T31b");
#endif /* CASE_2517T31b */

/* _2518T31a equal(-)  returns [not complexity] */
#if !defined(SKIP_2518T31a)&&(!defined(ONLY)||defined(CASE_2518T31a))
	begin_chk("_2518T31a");
	{

	// simple pointers
	Char buf[] = "abdx";
	Char *p1 = buf;
	Char *p2 = buf+3;  // first 3 chars only
	Char bufb[4] = "abd";
	Char *p3 = bufb;
	bool ret (equal(p1, p2, p3));
	ieq(ret, 1);
	chk(equal(p1, p1, p3));
	chk(!equal(p1, p1 + 1, p3 + 1));

	// strings
	Ustr s ("xabcd");
	Ustr t ("xab");
	bool ret2 (equal(s.begin(), s.end(), t.begin()));
	ieq(ret2, 0);

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcd";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *ptrb = (const Char *)"xabcd";
	Ustr s4b (ptrb, ptrb+5);
	int err3 = 0, nrec3 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s4b.begin(), err3, nrec3);
	bool ret3 
		(equal(it1, it2, it3));
	ieq(ret3, 1);
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);

	}
	end_chk("_2518T31a");
#else
	skip_chk("_2518T31a");
#endif /* CASE_2518T31a */

/* _2518T31b equal(-, pred) returns, complexity */
#if !defined(SKIP_2518T31b)&&(!defined(ONLY)||defined(CASE_2518T31b))
	begin_chk("_2518T31b");
	{

	// simple pointers
	Char buf[] = "abdx";
	Char *p1 = buf;
	Char *p2 = buf+3;  // first 3 chars only
	Char bufb[4] = "abd";
	Char *p3 = bufb;
	bool ret = equal(p1, p2, p3, f_2518T31b);
	ieq(ret, 1);

	// strings
	Ustr s ("xabcd");
	Ustr t ("xab");
	bool ret2 = equal(s.begin(), s.end(), t.begin(), f_2518T31b);
	ieq(ret2, 0);

	// strict iterators over strings
	const STD equal_to<Char> x = equal_to<Char>();
	const Char *ptr = (const Char *)"xabcd";
	Ustr s4 (ptr, ptr+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *ptrb = (const Char *)"xabcd";
	Ustr s4b (ptrb, ptrb+5);
	int err3 = 0, nrec3 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s4b.begin(), err3, nrec3);
	bool ret3 =
		equal(it1, it2, it3, x);
	ieq(ret3, 1);
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);



	}
	end_chk("_2518T31b");
#else
	skip_chk("_2518T31b");
#endif /* CASE_2518T31b */

/* _2519T31a search(-)        effects, returns, complexity */
#if !defined(SKIP_2519T31a)&&(!defined(ONLY)||defined(CASE_2519T31a))
	begin_chk("_2519T31a");
	{

	// simple pointers
	Char buf[] = "ab";
	Char *p1 = buf;
	Char *p2 = buf+2;
	Char *p3;
	Char *p4 = (Char *)"b";
	Char *p5 = p4+1;
	p3 = search(p1, p2, p4, p5);
	aeq(p3, buf+1);
	ieq(*p3, 'b');
	chk(search(p1, p1, p4, p5) == p1);
	chk(search(p1, p2, p4, p4) == p1);

	// strings
	Ustr s ("xab");
	Ustr t ("ab");
	ieq(*search(s.begin(), s.end(), t.begin(), t.end()), 'a');

	// strict iterators over strings
	const Char *ptr = (const Char *)"xabcd";
	Ustr s4 (ptr, ptr+4);
	int err1 = 0, nrec1 = 4;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *ptr2 = (const Char *)"ab";
	Ustr s5 (ptr2, ptr2+2);
	int err3 = 0, nrec3 = 4;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it4
		(s5.end(), err4, nrec4);
	ieq(*search(it1, it2, it3, it4), 'a');
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);

	}
	end_chk("_2519T31a");
#else
	skip_chk("_2519T31a");
#endif /* CASE_2519T31a */

/* _2519T31b search(-, pred)  effects, returns, complexity */
#if !defined(SKIP_2519T31b)&&(!defined(ONLY)||defined(CASE_2519T31b))
	begin_chk("_2519T31b");
	{
	// simple pointers
	Char buf[] = "abcxabcd";
	Char *p1 = buf;
	Char *p2 = buf+8;
	Char *p3;
	Char *p4 = (Char *)"abcd";
	Char *p5 = p4+4;
	npreds_2519T31b = 0;
	p3 = search(p1, p2, p4, p5, f_2519T31b);
	aeq(p3, buf+4);
	ieq(*p3, 'a');
	ieq(*++p3, 'b');
	ieq(*++p3, 'c');
	ieq(*++p3, 'd');
	chk(npreds_2519T31b <= 8 * 4);

	// strings
	npreds_2519T31b = 0;
	Ustr s("xababd");
	Ustr t("abd");
	Ustr::iterator i =
		search(s.begin(), s.end(), t.begin(), t.end());
	ieq(i - s.begin(), 3);
	ieq(*(i - 1), 'b');
	chk(npreds_2519T31b <= 5 * 3);

	// strict iterators over strings
	const STD equal_to<Char> x = equal_to<Char>();
	const Char *buf2 = (const Char *)"xabcd";
	Ustr s4 (buf2, buf2+5);
	int err1 = 0, nrec1 = 5;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	const Char *buf3 = (const Char *)"ab";
	Ustr s5 (buf3, buf3+2);
	int err3 = 0, nrec3 = 2;
	strict_forward_iterator<Ustr::iterator, Char> it3
		(s5.begin(), err3, nrec3);
	int err4 = 0, nrec4 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it4
		(s5.end(), err4, nrec4);
	ieq(*search(it1, it2, it3, it4, x), 'a');
	ieq(err1, 0);
	ieq(err2, 0);
	ieq(err3, 0);
	ieq(err4, 0);

	}
	end_chk("_2519T31b");
#else
	skip_chk("_2519T31b");
#endif /* CASE_2519T31b */

/* _2519T31c search_n(-)        effects, returns, complexity ADDED [MAR96] */
#if !defined(SKIP_2519T31c)&&(!defined(ONLY)||defined(CASE_2519T31c))
	begin_chk("_2519T31c");
	{

	// simple pointers
	Char buf[] = "ab";
	Char *p1 = buf;
	Char *p2 = buf+2;
	Char *p3;
	p3 = search_n(p1, p2, 1, 'b');
	aeq(p3, buf+1);
	ieq(*p3, 'b');
	aeq(search_n(p1, p1, 1, 'b'), p1);
	aeq(search_n(p1, p2, 0, 'b'), p1);

	// strings
	Ustr s ("babb");
	aeq(&*search_n(s.begin(), s.end(), 2, 'b'), &*s.begin() + 2);

	// strict iterators over strings
	const Char *ptr = (const Char *)"ababba";
	Ustr s4 (ptr, ptr + 6);
	int err1 = 0, nrec1 = 6;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	ieq(*search_n(it1, it2, 2, 'b'), 'b');
	ieq(err1, 0);
	ieq(err2, 0);

	}
	end_chk("_2519T31c");
#else
	skip_chk("_2519T31c");
#endif /* CASE_2519T31c */

/* _2519T31d search_n(-, pred)  effects, returns, complexity ADDED [MAR96] */
#if !defined(SKIP_2519T31d)&&(!defined(ONLY)||defined(CASE_2519T31d))
	begin_chk("_2519T31d");
	{
	// simple pointers
	Char buf[] = "abcbbbcd";
	Char *p1 = buf;
	Char *p2 = buf+8;
	Char *p3;
	npreds_2519T31d = 0;
	p3 = search_n(p1, p2, 2, 'b', f_2519T31d);
	aeq(p3, buf + 3);
	ieq(*p3, 'b');
	chk(npreds_2519T31d <= 8 * 2);

	// strings
	npreds_2519T31d = 0;
	Ustr s("xbabb");
	Ustr::iterator i =
		search_n(s.begin(), s.end(), 2, 'b');
	ieq(i - s.begin(), 3);
	ieq(*(i - 1), 'a');
	chk(npreds_2519T31d <= 5 * 2);

	// strict iterators over strings
	const STD equal_to<Char> x = equal_to<Char>();
	const Char *buf2 = (const Char *)"xababba";
	Ustr s4 (buf2, buf2 + 7);
	int err1 = 0, nrec1 = 7;
	strict_forward_iterator<Ustr::iterator, Char> it1
		(s4.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_forward_iterator<Ustr::iterator, Char> it2
		(s4.end(), err2, nrec2);
	ieq(*search_n(it1, it2, 2, 'b', x), 'b');
	ieq(err1, 0);
	ieq(err2, 0);




	}
	end_chk("_2519T31d");
#else
	skip_chk("_2519T31d");
#endif /* CASE_2519T31d */

return leave_chk("t251");
}
/* V3.10:0009 */
