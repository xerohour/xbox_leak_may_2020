/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _233T11 <map> synopsis */
#if !defined(SKIP_233T11)&&(!defined(ONLY)||defined(CASE_233T11))
#include <functional>
#include <map>
#include <utility>

#ifdef PAIR_DEFINITION
 PAIR_DEFINITION;
#else
 typedef pair<const Char, int> Pair;
#endif

#endif /* CASE_233T11 */

/* _233T21 <set> synopsis */
#if !defined(SKIP_233T21)&&(!defined(ONLY)||defined(CASE_233T21))
#include <functional>
#include <set>
#endif /* CASE_233T21 */

/* _2331T11 map definition */
#if !defined(SKIP_2331T11)&&(!defined(ONLY)||defined(CASE_2331T11))
#include "iter.h"
#include <map>
#include <new>
#include <utility>

#ifdef PAIR_DEFINITION
 PAIR_DEFINITION;
#else
 typedef pair<const Char, int> Pair;
#endif

typedef map<Char, int, less<Char>, ALLOCATOR(Pair)> Cont_2331T11;	// [NOV97]
Cont_2331T11::reference f1_2331T11()
	{static Pair uc((Char)'x', 5);
	return ((Cont_2331T11::reference)uc);
	}
Cont_2331T11::const_reference f2_2331T11()
	{static Pair uc((Char)'x', 5);
	return ((Cont_2331T11::const_reference)uc);
	}
#endif /* CASE_2331T11 */

/* _23315T21 map::operator[] returns */
#if !defined(SKIP_23315T21)&&(!defined(ONLY)||defined(CASE_23315T21))
#include "iter.h"
#include <map>
#include <new>
#include <utility>

#ifdef PAIR_DEFINITION
 PAIR_DEFINITION;
#else
 typedef pair<const Char, int> Pair;
#endif

typedef map<Char, int, less<Char>, ALLOCATOR(Pair)> Cont_23315T21;	// [NOV97]
#endif /* CASE_23315T21 */

int t238_main(int, char *[])
{
enter_chk("t238.cpp");
/* _233T11 <map> synopsis */
#if !defined(SKIP_233T11)&&(!defined(ONLY)||defined(CASE_233T11))
	begin_chk("_233T11");
	{
	typedef map<Char, int, less<Char>, ALLOCATOR(Pair)> Cont;	// [NOV97]
	Cont v0;
	chk(v0 == v0);
	chk(!(v0 < v0));
	typedef multimap<Char, int, less<Char>, ALLOCATOR(Pair)> Mcont;	// [NOV97]
	Mcont m0;
	chk(m0 == m0);
	chk(!(m0 < m0));


	}
	end_chk("_233T11");
#else
	skip_chk("_233T11");
#endif /* CASE_233T11 */

/* _233T21 <set> synopsis */
#if !defined(SKIP_233T21)&&(!defined(ONLY)||defined(CASE_233T21))
	begin_chk("_233T21");
	{
	typedef set<Char, less<Char>, ALLOCATOR(Char)> Cont;
	Cont v0;
	chk(v0 == v0);
	chk(!(v0 < v0));
	typedef multiset<Char, less<Char>, ALLOCATOR(Char)> Mcont;
	Mcont m0;
	chk(m0 == m0);
	chk(!(m0 < m0));








	}
	end_chk("_233T21");
#else
	skip_chk("_233T21");
#endif /* CASE_233T21 */

/* _2331T11 map definition */
#if !defined(SKIP_2331T11)&&(!defined(ONLY)||defined(CASE_2331T11))
	begin_chk("_2331T11");
	{
	typedef Cont_2331T11 Cont;
	const Pair s[] = {
		Pair('a', 'A'), Pair('b', 'B'), Pair('c', 'C'),
		Pair('d', 'D'), Pair('e', 'E')};
	const int NC = 5;
	const Cont a(s, s + 5);	// container specific
	// UNAUTHORIZED CHANGE FROM referent_type int *p0 = (Cont::mapped_type *)0;
	Cont::allocator_type::const_pointer *p00
		= (Cont::const_pointer *)0; TOUCH(p00);
	Cont::allocator_type::pointer *p01 = (Cont::pointer *)0; TOUCH(p01);

		// Table 50 -- for all containers
	ALLOCATOR(Pair) *pal
		= (Cont::allocator_type *)0; TOUCH(pal);	// ADDED [MAR96] CHANGED [NOV97]
	Pair *p1 = (Cont::value_type *)0; TOUCH(p1);
	size_t *p2 = (Cont::size_type *)0; TOUCH(p2);
	ptrdiff_t *p3 = (Cont::difference_type *)0; TOUCH(p3);
	Pair uc1 = f1_2331T11();
	Pair uc2 = f2_2331T11();
	Cont::iterator *p6 = (Cont::iterator *)0; TOUCH(p6);
	Cont::const_iterator *p7 = (Cont::const_iterator *)0; TOUCH(p7);
	ALLOCATOR(Pair) al;	// CHANGED [NOV97]
	Cont u0;
	ieq(u0.size(), 0);
	chk(u0.empty());
	ieq(a.size(), NC);
	chk(!a.empty());
	chk(u0 != a);
	Cont u1;
	Cont u3(a);
	chk(u3 == a);

	Cont::allocator_type al3 = u3.get_allocator();	// added [JUL95]
	chk(al3 == al);
	u3.clear();
	chk(u3.empty());
	u3 = a;
	chk(u3 == a);

	const Cont u4 = a;
	chk(u4 == a);
	Cont *p = new Cont(a);
	ieq(p->size(), NC);
	p->DESTRUCTOR(map, Cont);
	ieq(p->size(), 0);
	operator delete(p);
	Cont::size_type n = distance(u3.begin(), u3.end());	// CHANGED [MAR96]
	ieq(n, NC);
	Cont::const_iterator pa = a.begin();
	pa = a.end();
	u1.swap(u3);
	chk(a == u1);
	swap(u1, u3);	// [NOV95]
	chk(a != u1);
	swap(u1, u3);
	chk(a == u1);
	ieq(u3.size(), 0);
	chk(&(u3 = a) == &u3);
	chk(u3 == a);
	chk(0 < a.max_size());
	n = u0.max_size();
	chk(a.max_size() == n);
	chk(u0 < a && !(u0 < u0) && !(a < a));
	chk(a > u0 && !(u0 > u0) && !(a > a));
	chk(u0 <= a && u0 <= u0 && !(a <= u0));
	chk(a >= u0 && u0 >= u0 && !(u0 >= a));
		// Table 51 -- for reversible (bidirectional) containers
	//typedef POINTER_X(Cont::value_type, Cont::allocator_type)
	//	Ptrty;
	//typedef POINTER_X(const Cont::value_type, Cont::allocator_type)
	//	Cptrty;
	//STD reverse_bidirectional_iterator<Cont::const_iterator, Cont::value_type,
	//	Cont::const_reference, Cptrty, Cont::difference_type> *p8 =
	//		(Cont::const_reverse_iterator *)0;
	//STD reverse_bidirectional_iterator<Cont::iterator, Cont::value_type,
	//	Cont::reference, Ptrty, Cont::difference_type> *p9 =
	//		(Cont::reverse_iterator *)0;
	Cont::const_reverse_iterator *p8 = 0; TOUCH(p8);	// CHANGED [NOV96]
	Cont::reverse_iterator *p9 = 0; TOUCH(p9);
	n = distance(a.rbegin(), a.rend());	// CHANGED [MAR96]
	ieq(n, NC);
	chk(*u3.rbegin() == s[4]);
	chk(*(--u3.rend()) == s[0]);
		// Table 54 -- for associative containers
	Char *p10 = (Cont::key_type *)0; TOUCH(p10);
	less<Char> *p11 = (Cont::key_compare *)0; TOUCH(p11);
	Cont::value_compare *p12 = (Cont::value_compare *)0; TOUCH(p12);
	less<Char> flt;
	Cont a0(flt);
	ieq(a0.size(), 0);
	Cont a1(flt, al);
	ieq(a1.size(), 0);
	Cont a2(s, s + 1, flt);
	ieq(a2.size(), 1);
	Cont a3(s, s + 2, flt, al);
	ieq(a3.size(), 2);
	Cont::key_compare kc = a3.key_comp();
	chk(kc((Char)'a', (Char)'b'));
	chk(!kc((Char)'b', (Char)'a'));
	chk(!kc((Char)'a', (Char)'a'));
	Cont::value_compare vc = a3.value_comp();
	chk(vc(s[0], s[1]));
	chk(!vc(s[1], s[0]));
	chk(!vc(s[0], s[0]));
	chk(*(a3.insert(s[4])).first == s[4]);
	ieq(a3.size(), 3);
	chk(*--a3.end() == s[4]);
	chk((a3.insert(s[3])).second);
	ieq(a3.size(), 4);
	chk(*--a3.end() == s[4]);
	chk(*(a3.insert(s[1])).first == s[1]);
	ieq(a3.size(), 4);	// 5 for multi
	chk(*--a3.end() == s[4]);
	chk(!(a3.insert(s[1])).second);	// true for multi
	ieq(a3.count((Char)'b'), 1);	// 3 for multi
	ieq(a3.erase((Char)'b'), 1);	// 3 for multi
	chk(*a3.insert(a3.end(), s[3]) == s[3]);
	chk(*a3.insert(a3.begin(), s[3]) == s[3]);
	a0.insert(s, s);
	ieq(a0.size(), 0);
	a0.insert(s + 1, s + 3);
	ieq(a0.size(), 2);
	chk(*a0.begin() == s[1]);
	chk(*a0.erase(a0.begin()) == s[2]);	// [NOV95]
	chk(*a0.erase(a0.begin(), a0.begin()) == s[2]);
	chk(*a0.begin() == s[2]);
	a0 = a;
	a0.erase(a0.end(), a0.end());
	ieq(a0.size(), NC);
	a0.erase(a0.begin(), ++a0.begin());
	ieq(a0.size(), NC - 1);
	a0.erase(a0.begin(), a0.end());
	ieq(a0.size(), 0);
	chk(a.find((Char)'a') == a.begin());
	chk(a.find((Char)'x') == a.end());
	chk(a0.find((Char)'x') == a0.end());
	ieq(a.count((Char)'a'), 1);
	ieq(a.count((Char)'x'), 0);
	ieq(a0.count((Char)'x'), 0);
	chk(a.lower_bound((Char)'b') == ++a.begin());
	chk(a.lower_bound((Char)'e') == --a.end());
	chk(a0.lower_bound((Char)'e') == a0.end());
	chk(a.upper_bound((Char)'a') == ++a.begin());
	chk(a.upper_bound((Char)'e') == a.end());
	chk(a0.upper_bound((Char)'e') == a0.end());
	chk((a.equal_range((Char)'b')).first == ++a.begin());
	chk((a.equal_range((Char)'e')).second == a.end());
	chk((a0.equal_range((Char)'e')).first == a0.end());
	 #if !DISALLOW_MEMBER_TEMPLATES
	int err1 = 0, nrec1 = NC;
	strict_input_iterator<const Pair *, Pair> it1
		(s, err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<const Pair *, Pair> it2
		(s + NC, err2, nrec2);
	Cont s0(it1, it2);
	ieq(err1, 0);
	ieq(err2, 0);
	 #endif




	}
	end_chk("_2331T11");
#else
	skip_chk("_2331T11");
#endif /* CASE_2331T11 */

/* _23315T21 map::operator[] returns */
#if !defined(SKIP_23315T21)&&(!defined(ONLY)||defined(CASE_23315T21))
	begin_chk("_23315T21");
	{
	typedef Cont_23315T21 Cont;
	Cont s0;
	s0[(Char)'a'] = 32767;
	ieq(s0[(Char)'a'], 32767);
	ieq(s0.size(), 1);
	s0[(Char)'a'] = -3;
	ieq(s0[(Char)'a'], -3);
	ieq(s0.size(), 1);

	}
	end_chk("_23315T21");
#else
	skip_chk("_23315T21");
#endif /* CASE_23315T21 */

return leave_chk("t238");
}
/* V3.10:0009 */
