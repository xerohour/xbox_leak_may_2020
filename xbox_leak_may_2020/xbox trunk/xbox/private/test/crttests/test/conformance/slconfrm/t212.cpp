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
#define CASE_21115T21	basic_string::begin() returns
#define CASE_21115T41	basic_string::end() returns
#define CASE_21115T61	basic_string::rbegin() returns
#define CASE_21115T81	basic_string::rend() returns
#define CASE_21116T21	basic_string::size() returns
#define CASE_21116T51	basic_string::length() returns
#define CASE_21116T71	basic_string::max_size() returns
//#define CASE_21116T91	basic_string::resize(size_type, charT) throws
#define CASE_21116T_111	basic_string::resize(size_type, charT) for n <= size()
#define CASE_21116T_121	basic_string::resize(size_type, charT) for n > size()
#define CASE_21116T_141	basic_string::resize(size_type) effects
#define CASE_21116T_171	basic_string::capacity() returns
#define CASE_21116T_201	basic_string::reserve effects
#define CASE_21116T_211	basic_string::reserve complexity
#define CASE_21116T_222	basic_string::reserve notes
#define CASE_21116T_241	basic_string::empty() returns
#define CASE_21117T21	basic_string::operator[]() returns
//#define CASE_21117T51	basic_string::at() throws
#define CASE_21117T61	basic_string::at() returns
#define CASE_211181T21	basic_string::operator+=(const basic_string&) returns
#define CASE_211181T41	basic_string::operator+=(const charT *) returns
//#define CASE_211181T51	basic_string::operator+=(const charT *) notes
#define CASE_211181T71	basic_string::operator+=(charT) returns
//#define CASE_211182T31	basic_string::append(const basic_string) throws
//#define CASE_211182T42	basic_string::append() effects, throws length_error
#define CASE_211182T51	basic_string::append(const basic_string&), effects replacement
#define CASE_211182T61	basic_string::append(const basic_string&) returns
#define CASE_211182T81	basic_string::append(const charT *, size_type) returns
#define CASE_211182T_101	basic_string::append(const charT *) returns
//#define CASE_211182T_111	basic_string::append(const CharT*) notes
#define CASE_211182T_131	basic_string::append(size_type, charT) returns
#define CASE_211182T_151	basic_string::append(InIt, InIt) returns
//#define CASE_211183T31	basic_string::assign(const basic_string) throws
#define CASE_211183T42	basic_string::assign(const basic_string&), effects
#define CASE_211183T51	basic_string::assign(const basic_string&) returns
#define CASE_211183T71	basic_string::assign(const charT *, size_type) returns
#define CASE_211183T91	basic_string::assign(const charT *) returns
//#define CASE_211183T_101	basic_string::assign(const CharT*) notes
#define CASE_211183T_121	basic_string::assign(size_type, charT) returns
#define CASE_211183T_141	basic_string::assign(InIt, InIt) returns
#endif /* IS_EMBEDDED */

/* _21115T21 basic_string::begin() returns */
#if !defined(SKIP_21115T21)&&(!defined(ONLY)||defined(CASE_21115T21))
#include <string>
#endif /* CASE_21115T21 */

/* _21115T41 basic_string::end() returns */
#if !defined(SKIP_21115T41)&&(!defined(ONLY)||defined(CASE_21115T41))
#include <string>
#endif /* CASE_21115T41 */

/* _21115T61 basic_string::rbegin() returns */
#if !defined(SKIP_21115T61)&&(!defined(ONLY)||defined(CASE_21115T61))
#include <string>
#endif /* CASE_21115T61 */

/* _21115T81 basic_string::rend() returns */
#if !defined(SKIP_21115T81)&&(!defined(ONLY)||defined(CASE_21115T81))
#include <string>
#endif /* CASE_21115T81 */

/* _21116T21 basic_string::size() returns */
#if !defined(SKIP_21116T21)&&(!defined(ONLY)||defined(CASE_21116T21))
#include <string>
#endif /* CASE_21116T21 */

/* _21116T51 basic_string::length() returns */
#if !defined(SKIP_21116T51)&&(!defined(ONLY)||defined(CASE_21116T51))
#include <string>
#endif /* CASE_21116T51 */

/* _21116T71 basic_string::max_size() returns */
#if !defined(SKIP_21116T71)&&(!defined(ONLY)||defined(CASE_21116T71))
#include <string>
#endif /* CASE_21116T71 */

/* _21116T91 basic_string::resize(size_type, charT) throws */
#if !defined(SKIP_21116T91)&&(!defined(ONLY)||defined(CASE_21116T91))
#include <string>
template<class T>
	class Myalloc_21116T91 : public allocator<T> {
public:
#if DISALLOW_TYPENAME
	typedef allocator<T>::pointer pointer;
	typedef allocator<T>::size_type size_type;
#else
	typedef typename allocator<T>::pointer pointer;
	typedef typename allocator<T>::size_type size_type;
#endif
	Myalloc_21116T91()
		{}
#if !DISALLOW_MEMBER_TEMPLATES_REBIND
//	Myalloc_21116T91(const Myalloc_21116T91<T>&)
//		{}
//	template<class U>
//		Myalloc_21116T91(const Myalloc_21116T91<U>&)
//		{}
	template<class U>
		Myalloc_21116T91<T>& operator=(const Myalloc_21116T91<U>&)
		{return (this); }
	template<class U>
		struct rebind {
			typedef Myalloc_21116T91<U> other;
		};
#endif
	size_t max_size() const
		{return (10); }
	};
#endif /* CASE_21116T91 */

/* _21116T_111 basic_string::resize(size_type, charT) for n <= size() */
#if !defined(SKIP_21116T_111)&&(!defined(ONLY)||defined(CASE_21116T_111))
#include <cstring>
#include <string>
#endif /* CASE_21116T_111 */

/* _21116T_121 basic_string::resize(size_type, charT) for n > size() */
#if !defined(SKIP_21116T_121)&&(!defined(ONLY)||defined(CASE_21116T_121))
#include <cstring>
#include <string>
#endif /* CASE_21116T_121 */

/* _21116T_141 basic_string::resize(size_type) effects */
#if !defined(SKIP_21116T_141)&&(!defined(ONLY)||defined(CASE_21116T_141))
#include <cstring>
#include <string>
#endif /* CASE_21116T_141 */

/* _21116T_171 basic_string::capacity() returns */
#if !defined(SKIP_21116T_171)&&(!defined(ONLY)||defined(CASE_21116T_171))
#include <string>
#endif /* CASE_21116T_171 */

/* _21116T_201 basic_string::reserve effects */
#if !defined(SKIP_21116T_201)&&(!defined(ONLY)||defined(CASE_21116T_201))
#include <string>
#endif /* CASE_21116T_201 */

/* _21116T_211 basic_string::reserve complexity */
#if !defined(SKIP_21116T_211)&&(!defined(ONLY)||defined(CASE_21116T_211))
#include <string>
#endif /* CASE_21116T_211 */

/* _21116T_222 basic_string::reserve notes */
#if !defined(SKIP_21116T_222)&&(!defined(ONLY)||defined(CASE_21116T_222))
#include <cstring>
#include <string>
#endif /* CASE_21116T_222 */

/* _21116T_241 basic_string::empty() returns */
#if !defined(SKIP_21116T_241)&&(!defined(ONLY)||defined(CASE_21116T_241))
#include <string>
#endif /* CASE_21116T_241 */

/* _21117T21 basic_string::operator[]() returns */
#if !defined(SKIP_21117T21)&&(!defined(ONLY)||defined(CASE_21117T21))
#include <string>
#endif /* CASE_21117T21 */

/* _21117T51 basic_string::at() throws */
#if !defined(SKIP_21117T51)&&(!defined(ONLY)||defined(CASE_21117T51))
#include <string>
#endif /* CASE_21117T51 */

/* _21117T61 basic_string::at() returns */
#if !defined(SKIP_21117T61)&&(!defined(ONLY)||defined(CASE_21117T61))
#include <string>
#endif /* CASE_21117T61 */

/* _211181T21 basic_string::operator+=(const basic_string&) returns */
#if !defined(SKIP_211181T21)&&(!defined(ONLY)||defined(CASE_211181T21))
#include <cstring>
#include <string>
#endif /* CASE_211181T21 */

/* _211181T41 basic_string::operator+=(const charT *) returns */
#if !defined(SKIP_211181T41)&&(!defined(ONLY)||defined(CASE_211181T41))
#include <cstring>
#include <string>
#endif /* CASE_211181T41 */

/* _211181T51 basic_string::operator+=(const charT *) notes */
#if !defined(SKIP_211181T51)&&(!defined(ONLY)||defined(CASE_211181T51))
#include <cctype>
#include <string>
int cnt_211181T51;
struct My_char_traits_211181T51 : public char_traits<Char> {
	static size_t length(const char_type *U)
		{cnt_211181T51 |= 1;
		return (strlen((const char *)U)); }
	};
#endif /* CASE_211181T51 */

/* _211181T71 basic_string::operator+=(charT) returns */
#if !defined(SKIP_211181T71)&&(!defined(ONLY)||defined(CASE_211181T71))
#include <cstring>
#include <string>
#endif /* CASE_211181T71 */

/* _211182T31 basic_string::append(const basic_string) throws */
#if !defined(SKIP_211182T31)&&(!defined(ONLY)||defined(CASE_211182T31))
#include <string>
#endif /* CASE_211182T31 */

/* _211182T42 basic_string::append() effects, throws length_error */
#if !defined(SKIP_211182T42)&&(!defined(ONLY)||defined(CASE_211182T42))
#include <string>
template<class T>
	class Myalloc_211182T42 : public allocator<T> {
public:
#if DISALLOW_TYPENAME
	typedef allocator<T>::pointer pointer;
	typedef allocator<T>::size_type size_type;
#else
	typedef typename allocator<T>::pointer pointer;
	typedef typename allocator<T>::size_type size_type;
#endif
	Myalloc_211182T42()
		{}
#if !DISALLOW_MEMBER_TEMPLATES_REBIND
//	Myalloc_211182T42(const Myalloc_211182T42<T>&)
//		{}
//	template<class U>
//		Myalloc_211182T42(const Myalloc_211182T42<U>&)
//		{}
	template<class U>
		Myalloc_211182T42<T>& operator=(const Myalloc_211182T42<U>&)
		{return (this); }
	template<class U>
		struct rebind {
			typedef Myalloc_211182T42<U> other;
		};
#endif
	size_t max_size() const
		{return (10); }
	};
#endif /* CASE_211182T42 */

/* _211182T51 basic_string::append(const basic_string&), effects replacement */
#if !defined(SKIP_211182T51)&&(!defined(ONLY)||defined(CASE_211182T51))
#include <cstring>
#include <string>
#endif /* CASE_211182T51 */

/* _211182T61 basic_string::append(const basic_string&) returns */
#if !defined(SKIP_211182T61)&&(!defined(ONLY)||defined(CASE_211182T61))
#include <string>
#endif /* CASE_211182T61 */

/* _211182T81 basic_string::append(const charT *, size_type) returns */
#if !defined(SKIP_211182T81)&&(!defined(ONLY)||defined(CASE_211182T81))
#include <cstring>
#include <string>
#endif /* CASE_211182T81 */

/* _211182T_101 basic_string::append(const charT *) returns */
#if !defined(SKIP_211182T_101)&&(!defined(ONLY)||defined(CASE_211182T_101))
#include <cstring>
#include <string>
#endif /* CASE_211182T_101 */

/* _211182T_111 basic_string::append(const CharT*) notes */
#if !defined(SKIP_211182T_111)&&(!defined(ONLY)||defined(CASE_211182T_111))
#include <cctype>
#include <string>
int cnt_211182T_111;
struct My_char_traits_211182T_111 : public char_traits<Char> {
	static size_t length(const char_type *U)
		{cnt_211182T_111 |= 1;
		return (strlen((const char *)U)); }
	};
#endif /* CASE_211182T_111 */

/* _211182T_131 basic_string::append(size_type, charT) returns */
#if !defined(SKIP_211182T_131)&&(!defined(ONLY)||defined(CASE_211182T_131))
#include <cstring>
#include <string>
#endif /* CASE_211182T_131 */

/* _211182T_151 basic_string::append(InIt, InIt) returns */
#if !defined(SKIP_211182T_151)&&(!defined(ONLY)||defined(CASE_211182T_151))
#if !IS_EMBEDDED && !DISALLOW_MEMBER_TEMPLATES
#include "iter.h"
#endif
#include <cstring>
#include <string>
#endif /* CASE_211182T_151 */

/* _211183T31 basic_string::assign(const basic_string) throws */
#if !defined(SKIP_211183T31)&&(!defined(ONLY)||defined(CASE_211183T31))
#include <string>
#endif /* CASE_211183T31 */

/* _211183T42 basic_string::assign(const basic_string&), effects */
#if !defined(SKIP_211183T42)&&(!defined(ONLY)||defined(CASE_211183T42))
#include <cstring>
#include <string>
#endif /* CASE_211183T42 */

/* _211183T51 basic_string::assign(const basic_string&) returns */
#if !defined(SKIP_211183T51)&&(!defined(ONLY)||defined(CASE_211183T51))
#include <string>
#endif /* CASE_211183T51 */

/* _211183T71 basic_string::assign(const charT *, size_type) returns */
#if !defined(SKIP_211183T71)&&(!defined(ONLY)||defined(CASE_211183T71))
#include <cstring>
#include <string>
#endif /* CASE_211183T71 */

/* _211183T91 basic_string::assign(const charT *) returns */
#if !defined(SKIP_211183T91)&&(!defined(ONLY)||defined(CASE_211183T91))
#include <cstring>
#include <string>
#endif /* CASE_211183T91 */

/* _211183T_101 basic_string::assign(const CharT*) notes */
#if !defined(SKIP_211183T_101)&&(!defined(ONLY)||defined(CASE_211183T_101))
#include <cctype>
#include <string>
int cnt_211183T_101;
struct My_char_traits_211183T_101 : public char_traits<Char> {
	static size_t length(const char_type *U)
		{cnt_211183T_101 |= 1;
		return (strlen((const char *)U)); }
	};
#endif /* CASE_211183T_101 */

/* _211183T_121 basic_string::assign(size_type, charT) returns */
#if !defined(SKIP_211183T_121)&&(!defined(ONLY)||defined(CASE_211183T_121))
#include <cstring>
#include <string>
#endif /* CASE_211183T_121 */

/* _211183T_141 basic_string::assign(InIt, InIt) returns */
#if !defined(SKIP_211183T_141)&&(!defined(ONLY)||defined(CASE_211183T_141))
#if !IS_EMBEDDED && !DISALLOW_MEMBER_TEMPLATES
#include "iter.h"
#endif
#include <cstring>
#include <string>
#endif /* CASE_211183T_141 */

int t212_main(int, char *[])
{
enter_chk("t212.cpp");
/* _21115T21 basic_string::begin() returns */
#if !defined(SKIP_21115T21)&&(!defined(ONLY)||defined(CASE_21115T21))
	begin_chk("_21115T21");
	{
	Ustr s1((const Char *)"abcd");
	Ustr::iterator p1 = s1.begin();
	ieq(*p1, (Char)'a');
	const Ustr s2((const Char *)"efgh");
	Ustr::const_iterator p2 = s2.begin();
	ieq(*p2, (Char)'e');
	Ustr s3 = s1;
	*p1 = (Char)'x';
	ieq(*s3.c_str(), (Char)'a');
	*s3.begin() = (Char)'y';
	ieq(*p1, (Char)'x');


	}
	end_chk("_21115T21");
#else
	skip_chk("_21115T21");
#endif /* CASE_21115T21 */

/* _21115T41 basic_string::end() returns */
#if !defined(SKIP_21115T41)&&(!defined(ONLY)||defined(CASE_21115T41))
	begin_chk("_21115T41");
	{
	Ustr s0;
	chk(s0.begin() == s0.end());
	Ustr s1((const Char *)"abcd");
	Ustr::iterator p1 = s1.end();
	ieq(*--p1, (Char)'d');
	const Ustr s2((const Char *)"efgh");
	Ustr::const_iterator p2 = s2.end();
	ieq(*--p2, (Char)'h');
	Ustr s3 = s1;
	*p1 = (Char)'x';
	ieq(*(s3.c_str() + 3), (Char)'d');
	*(s3.end() - 1) = (Char)'y';
	ieq(*p1, (Char)'x');


	}
	end_chk("_21115T41");
#else
	skip_chk("_21115T41");
#endif /* CASE_21115T41 */

/* _21115T61 basic_string::rbegin() returns */
#if !defined(SKIP_21115T61)&&(!defined(ONLY)||defined(CASE_21115T61))
	begin_chk("_21115T61");
	{
	Ustr s1((const Char *)"abcd");
	Ustr::reverse_iterator p1 = s1.rbegin();
	ieq(*p1, (Char)'d');
	const Ustr s2((const Char *)"efgh");
	Ustr::const_reverse_iterator p2 = s2.rbegin();
	ieq(*p2, (Char)'h');
	Ustr s3 = s1;
	*p1 = (Char)'x';
	ieq(*(s3.c_str() + 3), (Char)'d');
	*s3.rbegin() = (Char)'y';
	ieq(*p1, (Char)'x');


	}
	end_chk("_21115T61");
#else
	skip_chk("_21115T61");
#endif /* CASE_21115T61 */

/* _21115T81 basic_string::rend() returns */
#if !defined(SKIP_21115T81)&&(!defined(ONLY)||defined(CASE_21115T81))
	begin_chk("_21115T81");
	{
	Ustr s0;
	chk(s0.rbegin() == s0.rend());
	Ustr s1((const Char *)"abcd");
	Ustr::reverse_iterator p1 = s1.rend();
	ieq(*--p1, (Char)'a');
	const Ustr s2((const Char *)"efgh");
	Ustr::const_reverse_iterator p2 = s2.rend();
	ieq(*--p2, (Char)'e');
	Ustr s3 = s1;
	*p1 = (Char)'x';
	ieq(*s3.c_str(), (Char)'a');
	*(s3.rend() - 1) = (Char)'y';
	ieq(*p1, (Char)'x');


	}
	end_chk("_21115T81");
#else
	skip_chk("_21115T81");
#endif /* CASE_21115T81 */

/* _21116T21 basic_string::size() returns */
#if !defined(SKIP_21116T21)&&(!defined(ONLY)||defined(CASE_21116T21))
	begin_chk("_21116T21");
	{
	Ustr s1((const Char *)"abcd");
	ieq(s1.size(), 4);
	const Ustr s2;
	ieq(s2.size(), 0);




	}
	end_chk("_21116T21");
#else
	skip_chk("_21116T21");
#endif /* CASE_21116T21 */

/* _21116T51 basic_string::length() returns */
#if !defined(SKIP_21116T51)&&(!defined(ONLY)||defined(CASE_21116T51))
	begin_chk("_21116T51");
	{
	Ustr s1((const Char *)"abcd");
	ieq(s1.size(), s1.length());
	const Ustr s2;
	ieq(s2.size(), s2.length());


	}
	end_chk("_21116T51");
#else
	skip_chk("_21116T51");
#endif /* CASE_21116T51 */

/* _21116T71 basic_string::max_size() returns */
#if !defined(SKIP_21116T71)&&(!defined(ONLY)||defined(CASE_21116T71))
	begin_chk("_21116T71");
	{
	Ustr s1((const Char *)"abcd");
	Ustr::size_type ms = s1.max_size();
	chk(0 < ms);
	chk(ms < Ustr::npos);
	const Ustr s2;
	chk(s2.max_size() == ms);



	}
	end_chk("_21116T71");
#else
	skip_chk("_21116T71");
#endif /* CASE_21116T71 */

/* _21116T91 basic_string::resize(size_type, charT) throws */
#if !defined(SKIP_21116T91)&&(!defined(ONLY)||defined(CASE_21116T91))
	begin_chk("_21116T91");
	{
	#if DISALLOW_EXCEPTIONS
	chk(1);
	#else
	typedef STD basic_string<Char,
		STD char_traits<Char>,
		Myalloc_21116T91<Char> > Mystr;
	Mystr s0;
	int cnt = 0;
	try
		{s0.resize(s0.max_size(), 0);
		cnt += 5;
		s0.resize(s0.max_size() + 1, 0); }
	catch (STD length_error)
		{++cnt; }
	catch (...)
		{cnt += 10; }
	ieq(cnt, 6);
	#endif


	}
	end_chk("_21116T91");
#else
	skip_chk("_21116T91");
#endif /* CASE_21116T91 */

/* _21116T_111 basic_string::resize(size_type, charT) for n <= size() */
#if !defined(SKIP_21116T_111)&&(!defined(ONLY)||defined(CASE_21116T_111))
	begin_chk("_21116T_111");
	{
	Ustr s1((const Char *)"abcd");
	s1.resize(4, 0);
	ieq(s1.size(), 4);
	ieq(STD memcmp(s1.c_str(), "abcd", 5), 0);
	s1.resize(2, 0);
	ieq(s1.size(), 2);
	ieq(STD memcmp(s1.c_str(), "ab", 3), 0);
	s1.resize(0, 0);
	ieq(s1.size(), 0);
	ieq(STD memcmp(s1.c_str(), "", 1), 0);


	}
	end_chk("_21116T_111");
#else
	skip_chk("_21116T_111");
#endif /* CASE_21116T_111 */

/* _21116T_121 basic_string::resize(size_type, charT) for n > size() */
#if !defined(SKIP_21116T_121)&&(!defined(ONLY)||defined(CASE_21116T_121))
	begin_chk("_21116T_121");
	{
	Ustr s1((const Char *)"abcd");
	s1.resize(6, (Char)'x');
	ieq(s1.size(), 6);
	ieq(STD memcmp(s1.c_str(), "abcdxx", 7), 0);
	s1.resize(11, (Char)'y');
	ieq(s1.size(), 11);
	ieq(STD memcmp(s1.c_str(), "abcdxxyyyyy", 12), 0);




	}
	end_chk("_21116T_121");
#else
	skip_chk("_21116T_121");
#endif /* CASE_21116T_121 */

/* _21116T_141 basic_string::resize(size_type) effects */
#if !defined(SKIP_21116T_141)&&(!defined(ONLY)||defined(CASE_21116T_141))
	begin_chk("_21116T_141");
	{
	Ustr s1((const Char *)"abcd");
	s1.resize(7);
	ieq(s1.size(), 7);
	ieq(STD memcmp(s1.c_str(), "abcd\0\0\0", 8), 0);




	}
	end_chk("_21116T_141");
#else
	skip_chk("_21116T_141");
#endif /* CASE_21116T_141 */

/* _21116T_171 basic_string::capacity() returns */
#if !defined(SKIP_21116T_171)&&(!defined(ONLY)||defined(CASE_21116T_171))
	begin_chk("_21116T_171");
	{
	Ustr s1((const Char *)"abcd");
	chk(s1.size() <= s1.capacity());
	const Ustr s2;
	chk(s2.size() <= s2.capacity());


	}
	end_chk("_21116T_171");
#else
	skip_chk("_21116T_171");
#endif /* CASE_21116T_171 */

/* _21116T_201 basic_string::reserve effects */
#if !defined(SKIP_21116T_201)&&(!defined(ONLY)||defined(CASE_21116T_201))
	begin_chk("_21116T_201");
	{
	Ustr s1((const Char *)"abcd");
	s1.reserve();	// ADDED [MAR96]
	s1.reserve(50);
	chk(50 <= s1.capacity());

	Ustr s2;
	s2.reserve(80);
	chk(80 <= s2.capacity());


	}
	end_chk("_21116T_201");
#else
	skip_chk("_21116T_201");
#endif /* CASE_21116T_201 */

/* _21116T_211 basic_string::reserve complexity */
#if !defined(SKIP_21116T_211)&&(!defined(ONLY)||defined(CASE_21116T_211))
	begin_chk("_21116T_211");
	{
	Ustr s1((const Char *)"abcd");
	s1.reserve(50);
	ieq(s1.size(), 4);
	ieq(STD memcmp(s1.c_str(), "abcd", 5), 0);
	Ustr s2;
	s2.reserve(80);
	ieq(s2.size(), 0);
	ieq(STD memcmp(s2.c_str(), "", 1), 0);



	}
	end_chk("_21116T_211");
#else
	skip_chk("_21116T_211");
#endif /* CASE_21116T_211 */

/* _21116T_222 basic_string::reserve notes */
#if !defined(SKIP_21116T_222)&&(!defined(ONLY)||defined(CASE_21116T_222))
	begin_chk("_21116T_222");
	{
	const Char *s = (const Char *)"abcd";
	Ustr s1(s);
	s1.reserve(600);
	Ustr::iterator p = s1.begin();
	ieq(STD memcmp(&*p, "abcd", 5), 0);
	int i;
	for (i = 8; i < 600; i += 4)
		{s1 += s;
		ieq(s1.begin() == p ? i : 0, i); }
	Ustr s2;
	s2.reserve(600);
	const char *q = s2.c_str();
	for (i = 4; i < 600; i += 4)
		{s2 += s;
		ieq(s2.c_str() == q ? i : 0, i); }


	}
	end_chk("_21116T_222");
#else
	skip_chk("_21116T_222");
#endif /* CASE_21116T_222 */

/* _21116T_241 basic_string::empty() returns */
#if !defined(SKIP_21116T_241)&&(!defined(ONLY)||defined(CASE_21116T_241))
	begin_chk("_21116T_241");
	{
	Ustr s1((const Char *)"abcd");
	chk(!s1.empty());
	const Ustr s2;
	chk(s2.empty());


	}
	end_chk("_21116T_241");
#else
	skip_chk("_21116T_241");
#endif /* CASE_21116T_241 */

/* _21117T21 basic_string::operator[]() returns */
#if !defined(SKIP_21117T21)&&(!defined(ONLY)||defined(CASE_21117T21))
	begin_chk("_21117T21");
	{
	Ustr s1((const Char *)"abcd");
	ieq(s1[3], (Char)'d');
	ieq(s1[4], 0);
	s1[2] = (Char)'x';
	ieq(s1[2], (Char)'x');
	const Ustr s2;
	ieq(s2[0], 0);
	Ustr s3 = s1;
	s1[0] = (Char)'x';
	ieq(*s3.c_str(), (Char)'a');
	s3[0] = (Char)'y';
	ieq(s1[0], (Char)'x');


	}
	end_chk("_21117T21");
#else
	skip_chk("_21117T21");
#endif /* CASE_21117T21 */

/* _21117T51 basic_string::at() throws */
#if !defined(SKIP_21117T51)&&(!defined(ONLY)||defined(CASE_21117T51))
	begin_chk("_21117T51");
	{
	#if DISALLOW_EXCEPTIONS
	chk(1);
	#else
	Ustr s1((const Char *)"abcd");
	int cnt = 0;
	try
		{s1.at(4); }
	catch (STD out_of_range)
		{++cnt; }
	catch (...)
		{cnt += 10; }
	ieq(cnt, 1);
	cnt = 0;
	try
		{s1.at(23); }
	catch (STD out_of_range)
		{++cnt; }
	catch (...)
		{cnt += 10; }
	ieq(cnt, 1);
	#endif


	}
	end_chk("_21117T51");
#else
	skip_chk("_21117T51");
#endif /* CASE_21117T51 */

/* _21117T61 basic_string::at() returns */
#if !defined(SKIP_21117T61)&&(!defined(ONLY)||defined(CASE_21117T61))
	begin_chk("_21117T61");
	{
	Ustr s1((const Char *)"abcd");
	ieq(s1.at(3), (Char)'d');
	s1.at(2) = (Char)'x';
	ieq(s1.at(2), (Char)'x');
	const Ustr s2((const Char *)"efg");
	ieq(s2.at(1), (Char)'f');
	Ustr s3 = s1;
	s1.at(0) = (Char)'x';
	ieq(*s3.c_str(), (Char)'a');
	s3.at(0) = (Char)'y';
	ieq(s1.at(0), (Char)'x');


	}
	end_chk("_21117T61");
#else
	skip_chk("_21117T61");
#endif /* CASE_21117T61 */

/* _211181T21 basic_string::operator+=(const basic_string&) returns */
#if !defined(SKIP_211181T21)&&(!defined(ONLY)||defined(CASE_211181T21))
	begin_chk("_211181T21");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Ustr s2((const Char *)"efgh");
	s1 += s2;
	ieq(STD memcmp(s1.c_str(), "abcdefgh", 9), 0);
	s1 += s0;
	ieq(STD memcmp(s1.c_str(), "abcdefgh", 9), 0);
	s1 += s1;
	ieq(STD memcmp(s1.c_str(), "abcdefghabcdefgh", 17), 0);
	s0 += s0;
	ieq(STD memcmp(s0.c_str(), "", 1), 0);


	}
	end_chk("_211181T21");
#else
	skip_chk("_211181T21");
#endif /* CASE_211181T21 */

/* _211181T41 basic_string::operator+=(const charT *) returns */
#if !defined(SKIP_211181T41)&&(!defined(ONLY)||defined(CASE_211181T41))
	begin_chk("_211181T41");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	s1 += (const Char *)"efgh";
	ieq(STD memcmp(s1.c_str(), "abcdefgh", 9), 0);
	s1 += (const Char *)"";
	ieq(STD memcmp(s1.c_str(), "abcdefgh", 9), 0);
	s0 += (const Char *)"";
	ieq(STD memcmp(s0.c_str(), "", 1), 0);


	}
	end_chk("_211181T41");
#else
	skip_chk("_211181T41");
#endif /* CASE_211181T41 */

/* _211181T51 basic_string::operator+=(const charT *) notes */
#if !defined(SKIP_211181T51)&&(!defined(ONLY)||defined(CASE_211181T51))
	begin_chk("_211181T51");
	{
	typedef STD basic_string<Char,
		My_char_traits_211181T51,
		Al> Mystr;
	Mystr s0((const Char *)"abcd");
	cnt_211181T51 = 0;
	s0 += (const Char *)"efgh";
	ieq(cnt_211181T51, 1);


	}
	end_chk("_211181T51");
#else
	skip_chk("_211181T51");
#endif /* CASE_211181T51 */

/* _211181T71 basic_string::operator+=(charT) returns */
#if !defined(SKIP_211181T71)&&(!defined(ONLY)||defined(CASE_211181T71))
	begin_chk("_211181T71");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	s1 += (Char)'e';
	ieq(STD memcmp(s1.c_str(), "abcde", 6), 0);
	s0 += (Char)'x';
	ieq(STD memcmp(s0.c_str(), "x", 2), 0);


	}
	end_chk("_211181T71");
#else
	skip_chk("_211181T71");
#endif /* CASE_211181T71 */

/* _211182T31 basic_string::append(const basic_string) throws */
#if !defined(SKIP_211182T31)&&(!defined(ONLY)||defined(CASE_211182T31))
	begin_chk("_211182T31");
	{
	#if DISALLOW_EXCEPTIONS
	chk(1);
	#else
	const Ustr s0;
	Ustr s1((const Char *)"abcd");
	int cnt = 0;
	try
		{s1.append(s0, 1, 20); }
	catch (STD out_of_range)
		{++cnt; }
	catch (...)
		{cnt += 10; }
	ieq(cnt, 1);
	#endif


	}
	end_chk("_211182T31");
#else
	skip_chk("_211182T31");
#endif /* CASE_211182T31 */

/* _211182T42 basic_string::append() effects, throws length_error */
#if !defined(SKIP_211182T42)&&(!defined(ONLY)||defined(CASE_211182T42))
	begin_chk("_211182T42");
	{
	#if DISALLOW_EXCEPTIONS
	chk(1);
	#else
	typedef STD basic_string<Char,
		STD char_traits<Char>,
		Myalloc_211182T42<Char> > Mystr;
	Mystr s1 = (const Char *)"abcd";
	const Mystr s2(s1.max_size() - 4, (Char)'x');
	int cnt = 0;
	try
		{s1.append(s2);
		cnt += 5;
		s1 = (const Char *)"abcde";
		s1.append(s2); }
	catch (STD length_error)
		{++cnt; }
	catch (...)
		{cnt += 10; }
	ieq(cnt, 6);
	s1 = (const Char *)"abcdef";
	cnt = 0;
	try
		{s1.append(s2); }
	catch (STD length_error)
		{++cnt; }
	catch (...)
		{cnt += 10; }
	ieq(cnt, 1);
	#endif


	}
	end_chk("_211182T42");
#else
	skip_chk("_211182T42");
#endif /* CASE_211182T42 */

/* _211182T51 basic_string::append(const basic_string&), effects replacement */
#if !defined(SKIP_211182T51)&&(!defined(ONLY)||defined(CASE_211182T51))
	begin_chk("_211182T51");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Ustr s2((const Char *)"efgh");
	s1.append(s2, 1, 1);
	ieq(STD memcmp(s1.c_str(), "abcdf", 6), 0);
	s1.append(s2, 2, 20);
	ieq(STD memcmp(s1.c_str(), "abcdfgh", 8), 0);
	s1.append(s1);
	ieq(STD memcmp(s1.c_str(), "abcdfghabcdfgh", 15), 0);
	s1.append(s0);
	ieq(STD memcmp(s1.c_str(), "abcdfghabcdfgh", 15), 0);
	s0.append(s0);
	ieq(s0.size(), 0);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);


	}
	end_chk("_211182T51");
#else
	skip_chk("_211182T51");
#endif /* CASE_211182T51 */

/* _211182T61 basic_string::append(const basic_string&) returns */
#if !defined(SKIP_211182T61)&&(!defined(ONLY)||defined(CASE_211182T61))
	begin_chk("_211182T61");
	{
	Ustr s1((const Char *)"abcd");
	const Ustr s2((const Char *)"efgh");
	chk(&s1.append(s2, 1, 1) == &s1);
	chk(&s1.append(s2, 2, 20) == &s1);
	chk(&s1.append(s1) == &s1);


	}
	end_chk("_211182T61");
#else
	skip_chk("_211182T61");
#endif /* CASE_211182T61 */

/* _211182T81 basic_string::append(const charT *, size_type) returns */
#if !defined(SKIP_211182T81)&&(!defined(ONLY)||defined(CASE_211182T81))
	begin_chk("_211182T81");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Char *s = (const Char *)"efgh";
	s1.append(s, 5);
	ieq(STD memcmp(s1.c_str(), "abcdefgh\0", 10), 0);
	s1.append(s, (Ustr::size_type)0);
	ieq(STD memcmp(s1.c_str(), "abcdefgh\0", 10), 0);
	s0.append(s, (Ustr::size_type)0);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);
	s0.append(s, 1);
	ieq(STD memcmp(s0.c_str(), "e", 2), 0);
	s0.append(&*s0.begin(), 1);	// ADDED [JUL97]
	ieq(STD memcmp(s0.c_str(), "ee", 3), 0);


	}
	end_chk("_211182T81");
#else
	skip_chk("_211182T81");
#endif /* CASE_211182T81 */

/* _211182T_101 basic_string::append(const charT *) returns */
#if !defined(SKIP_211182T_101)&&(!defined(ONLY)||defined(CASE_211182T_101))
	begin_chk("_211182T_101");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Char *s = (const Char *)"efgh";
	const Char *sn = (const Char *)"";
	s1.append(s);
	ieq(STD memcmp(s1.c_str(), "abcdefgh", 9), 0);
	s1.append(sn);
	ieq(STD memcmp(s1.c_str(), "abcdefgh", 9), 0);
	s0.append(sn);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);
	s0.append(s);
	ieq(STD memcmp(s0.c_str(), "efgh", 5), 0);


	}
	end_chk("_211182T_101");
#else
	skip_chk("_211182T_101");
#endif /* CASE_211182T_101 */

/* _211182T_111 basic_string::append(const CharT*) notes */
#if !defined(SKIP_211182T_111)&&(!defined(ONLY)||defined(CASE_211182T_111))
	begin_chk("_211182T_111");
	{
	typedef STD basic_string<Char,
		My_char_traits_211182T_111,
		Al> Mystr;
	Mystr s0((const Char *)"abcd");
	cnt_211182T_111 = 0;
	s0.append((const Char *)"efgh");
	ieq(cnt_211182T_111, 1);



	}
	end_chk("_211182T_111");
#else
	skip_chk("_211182T_111");
#endif /* CASE_211182T_111 */

/* _211182T_131 basic_string::append(size_type, charT) returns */
#if !defined(SKIP_211182T_131)&&(!defined(ONLY)||defined(CASE_211182T_131))
	begin_chk("_211182T_131");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	s1.append(2, (Char)'x');
	ieq(STD memcmp(s1.c_str(), "abcdxx", 7), 0);
	s1.append(0, (Char)'y');
	ieq(STD memcmp(s1.c_str(), "abcdxx", 7), 0);
	s0.append((Ustr::size_type)0, (Char)0);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);
	s0.append(4, (Char)'z');
	ieq(STD memcmp(s0.c_str(), "zzzz", 5), 0);


	}
	end_chk("_211182T_131");
#else
	skip_chk("_211182T_131");
#endif /* CASE_211182T_131 */

/* _211182T_151 basic_string::append(InIt, InIt) returns */
#if !defined(SKIP_211182T_151)&&(!defined(ONLY)||defined(CASE_211182T_151))
	begin_chk("_211182T_151");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Char *s = (const Char *)"efgh";
	s1.append(s, s + 2);
	ieq(STD memcmp(s1.c_str(), "abcdef", 7), 0);
	s1.append(s, s);
	ieq(STD memcmp(s1.c_str(), "abcdef", 7), 0);
	s0.append(s, s);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);
	s0.append(s, s + 5);
	ieq(STD memcmp(s0.c_str(), "efgh\0", 6), 0);

	#if !IS_EMBEDDED && !DISALLOW_MEMBER_TEMPLATES
	s0 = s;
	s1 = s;
	int err1 = 0, nrec1 = 4;
	strict_input_iterator<Ustr::const_iterator, Char> it1
		(s1.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::const_iterator, Char> it2
		(s1.end(), err2, nrec2);
	s0.append(it1, it2);
	ieq(STD memcmp(s0.c_str(), "efghefgh", 9), 0);
	ieq(err1, 0);
	ieq(err2, 0);

	s0.append((short)3, (short)'x');	// ADDED [JUL97]
	ieq(STD memcmp(s0.c_str(), "efghefghxxx", 12), 0);
	#endif


	}
	end_chk("_211182T_151");
#else
	skip_chk("_211182T_151");
#endif /* CASE_211182T_151 */

/* _211183T31 basic_string::assign(const basic_string) throws */
#if !defined(SKIP_211183T31)&&(!defined(ONLY)||defined(CASE_211183T31))
	begin_chk("_211183T31");
	{
	#if DISALLOW_EXCEPTIONS
	chk(1);
	#else
	const Ustr s0;
	Ustr s1((const Char *)"abcd");
	int cnt = 0;
	try
		{s1.assign(s0, 1, 20); }
	catch (STD out_of_range)
		{++cnt; }
	catch (...)
		{cnt += 10; }
	ieq(cnt, 1);
	#endif


	}
	end_chk("_211183T31");
#else
	skip_chk("_211183T31");
#endif /* CASE_211183T31 */

/* _211183T42 basic_string::assign(const basic_string&), effects */
#if !defined(SKIP_211183T42)&&(!defined(ONLY)||defined(CASE_211183T42))
	begin_chk("_211183T42");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Ustr s2((const Char *)"efgh");
	s1.assign(s2, 1, 1);
	ieq(STD memcmp(s1.c_str(), "f", 2), 0);
	s1.assign(s2, 2, 20);
	ieq(STD memcmp(s1.c_str(), "gh", 3), 0);
	s1.assign(s2);
	ieq(STD memcmp(s1.c_str(), "efgh", 5), 0);
	s1.assign(s1);
	ieq(STD memcmp(s1.c_str(), "efgh", 5), 0);
	s1.assign(s0);
	ieq(STD memcmp(s1.c_str(), "", 1), 0);
	s0.assign(s0);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);


	}
	end_chk("_211183T42");
#else
	skip_chk("_211183T42");
#endif /* CASE_211183T42 */

/* _211183T51 basic_string::assign(const basic_string&) returns */
#if !defined(SKIP_211183T51)&&(!defined(ONLY)||defined(CASE_211183T51))
	begin_chk("_211183T51");
	{
	Ustr s1((const Char *)"abcd");
	const Ustr s2((const Char *)"efgh");
	chk(&s1.assign(s2, 1, 1) == &s1);
	chk(&s1.assign(s2, 2, 20) == &s1);
	chk(&s1.assign(s1) == &s1);


	}
	end_chk("_211183T51");
#else
	skip_chk("_211183T51");
#endif /* CASE_211183T51 */

/* _211183T71 basic_string::assign(const charT *, size_type) returns */
#if !defined(SKIP_211183T71)&&(!defined(ONLY)||defined(CASE_211183T71))
	begin_chk("_211183T71");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Char *s = (const Char *)"efgh";
	s1.assign(s, 5);
	ieq(STD memcmp(s1.c_str(), "efgh\0", 6), 0);
	s1.assign(s, (Ustr::size_type)0);
	ieq(STD memcmp(s1.c_str(), "", 1), 0);
	s0.assign(s, (Ustr::size_type)0);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);
	s0.assign(s, 1);
	ieq(STD memcmp(s0.c_str(), "e", 2), 0);

	s0.assign(&*s0.begin(), 1);	// ADDED [JUL97]
	ieq(STD memcmp(s0.c_str(), "e", 2), 0);


	}
	end_chk("_211183T71");
#else
	skip_chk("_211183T71");
#endif /* CASE_211183T71 */

/* _211183T91 basic_string::assign(const charT *) returns */
#if !defined(SKIP_211183T91)&&(!defined(ONLY)||defined(CASE_211183T91))
	begin_chk("_211183T91");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Char *s = (const Char *)"efgh";
	const Char *sn = (const Char *)"";
	s1.assign(s);
	ieq(STD memcmp(s1.c_str(), "efgh", 5), 0);
	s1.assign(sn);
	ieq(STD memcmp(s1.c_str(), "", 1), 0);
	s0.assign(sn);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);
	s0.assign(s);
	ieq(STD memcmp(s0.c_str(), "efgh", 5), 0);


	}
	end_chk("_211183T91");
#else
	skip_chk("_211183T91");
#endif /* CASE_211183T91 */

/* _211183T_101 basic_string::assign(const CharT*) notes */
#if !defined(SKIP_211183T_101)&&(!defined(ONLY)||defined(CASE_211183T_101))
	begin_chk("_211183T_101");
	{
	typedef STD basic_string<Char,
		My_char_traits_211183T_101,
		Al> Mystr;
	Mystr s0((const Char *)"abcd");
	cnt_211183T_101 = 0;
	s0.assign((const Char *)"efgh");
	ieq(cnt_211183T_101, 1);



	}
	end_chk("_211183T_101");
#else
	skip_chk("_211183T_101");
#endif /* CASE_211183T_101 */

/* _211183T_121 basic_string::assign(size_type, charT) returns */
#if !defined(SKIP_211183T_121)&&(!defined(ONLY)||defined(CASE_211183T_121))
	begin_chk("_211183T_121");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	s1.assign(2, (Char)'x');
	ieq(STD memcmp(s1.c_str(), "xx", 3), 0);
	s1.assign(0, (Char)'y');
	ieq(STD memcmp(s1.c_str(), "", 1), 0);
	s0.assign((Ustr::size_type)0, (Char)0);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);
	s0.assign(4, (Char)'z');
	ieq(STD memcmp(s0.c_str(), "zzzz", 5), 0);


	}
	end_chk("_211183T_121");
#else
	skip_chk("_211183T_121");
#endif /* CASE_211183T_121 */

/* _211183T_141 basic_string::assign(InIt, InIt) returns */
#if !defined(SKIP_211183T_141)&&(!defined(ONLY)||defined(CASE_211183T_141))
	begin_chk("_211183T_141");
	{
	Ustr s0;
	Ustr s1((const Char *)"abcd");
	const Char *s = (const Char *)"efgh";
	s1.assign(s, s + 2);
	ieq(STD memcmp(s1.c_str(), "ef", 3), 0);
	s1.assign(s, s);
	ieq(STD memcmp(s1.c_str(), "", 1), 0);
	s0.assign(s, s);
	ieq(STD memcmp(s0.c_str(), "", 1), 0);
	s0.assign(s, s + 5);
	ieq(STD memcmp(s0.c_str(), "efgh\0", 6), 0);

	#if !IS_EMBEDDED && !DISALLOW_MEMBER_TEMPLATES
	s1 = s;
	int err1 = 0, nrec1 = 4;
	strict_input_iterator<Ustr::const_iterator, Char> it1
		(s1.begin(), err1, nrec1);
	int err2 = 0, nrec2 = 0;
	strict_input_iterator<Ustr::const_iterator, Char> it2
		(s1.end(), err2, nrec2);
	s0.assign(it1, it2);
	ieq(STD memcmp(s0.c_str(), "efgh", 5), 0);
	ieq(err1, 0);
	ieq(err2, 0);

	s0.assign((short)3, (short)'x');	// ADDED [JUL97]
	ieq(STD memcmp(s0.c_str(), "xxx", 4), 0);
	#endif

	}
	end_chk("_211183T_141");
#else
	skip_chk("_211183T_141");
#endif /* CASE_211183T_141 */

return leave_chk("t212");
}
/* V3.10:0009 */
