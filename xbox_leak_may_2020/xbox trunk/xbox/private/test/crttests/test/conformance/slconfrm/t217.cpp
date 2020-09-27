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
//#define CASE_2114T11	definition of char_traits<wchar_t>, wstring
//#define CASE_2115T21	char_traits<wchar_t>::assign effects
//#define CASE_2115T41	char_traits<wchar_t>::eq returns
//#define CASE_2115T81	char_traits<wchar_t>::lt returns
//#define CASE_2115T_181	char_traits<wchar_t>::compare returns
//#define CASE_2115T_201	char_traits<wchar_t>::length returns
//#define CASE_2115T_221	char_traits<wchar_t>::copy returns
#define CASE_212T12	<cctype> synopsis
//#define CASE_212T13	<cwctype> synopsis
#define CASE_212T14	<cstring> synopsis
//#define CASE_212T15	<cwchar> synopsis
#define CASE_212T16	<cstdlib> synopsis
#define CASE_212T51	strchr declarations
#define CASE_212T71	strpbrk declarations
#define CASE_212T91	strrchr declarations
#define CASE_212T_111	strstr declarations
#define CASE_212T_131	memchr declarations
//#define CASE_212T_151	wcschr declarations
//#define CASE_212T_171	wcspbrk declarations
//#define CASE_212T_191	wcsrchr declarations
//#define CASE_212T_211	wcsstr declarations
//#define CASE_212T_231	wmemchr declarations
#endif /* IS_EMBEDDED */

/* _2114T11 definition of char_traits<wchar_t>, wstring */
#if !defined(SKIP_2114T11)&&(!defined(ONLY)||defined(CASE_2114T11))
#include <sstream>
#include <string>
#endif /* CASE_2114T11 */

/* _2115T21 char_traits<wchar_t>::assign effects */
#if !defined(SKIP_2115T21)&&(!defined(ONLY)||defined(CASE_2115T21))
#include <string>
#endif /* CASE_2115T21 */

/* _2115T41 char_traits<wchar_t>::eq returns */
#if !defined(SKIP_2115T41)&&(!defined(ONLY)||defined(CASE_2115T41))
#include <string>
#endif /* CASE_2115T41 */

/* _2115T81 char_traits<wchar_t>::lt returns */
#if !defined(SKIP_2115T81)&&(!defined(ONLY)||defined(CASE_2115T81))
#include <string>
#endif /* CASE_2115T81 */

/* _2115T_181 char_traits<wchar_t>::compare returns */
#if !defined(SKIP_2115T_181)&&(!defined(ONLY)||defined(CASE_2115T_181))
#include <string>
#endif /* CASE_2115T_181 */

/* _2115T_201 char_traits<wchar_t>::length returns */
#if !defined(SKIP_2115T_201)&&(!defined(ONLY)||defined(CASE_2115T_201))
#include <string>
#endif /* CASE_2115T_201 */

/* _2115T_221 char_traits<wchar_t>::copy returns */
#if !defined(SKIP_2115T_221)&&(!defined(ONLY)||defined(CASE_2115T_221))
#include <string>
#endif /* CASE_2115T_221 */

/* _212T12 <cctype> synopsis */
#if !defined(SKIP_212T12)&&(!defined(ONLY)||defined(CASE_212T12))
#include <cctype>
#endif /* CASE_212T12 */

/* _212T13 <cwctype> synopsis */
#if !defined(SKIP_212T13)&&(!defined(ONLY)||defined(CASE_212T13))
#include <cwctype>
#endif /* CASE_212T13 */

/* _212T14 <cstring> synopsis */
#if !defined(SKIP_212T14)&&(!defined(ONLY)||defined(CASE_212T14))
#include <cstring>
#endif /* CASE_212T14 */

/* _212T15 <cwchar> synopsis */
#if !defined(SKIP_212T15)&&(!defined(ONLY)||defined(CASE_212T15))
#include <cwchar>
#endif /* CASE_212T15 */

/* _212T16 <cstdlib> synopsis */
#if !defined(SKIP_212T16)&&(!defined(ONLY)||defined(CASE_212T16))
#include <cstdlib>
#endif /* CASE_212T16 */

/* _212T51 strchr declarations */
#if !defined(SKIP_212T51)&&(!defined(ONLY)||defined(CASE_212T51))
#include <cstring>
#endif /* CASE_212T51 */

/* _212T71 strpbrk declarations */
#if !defined(SKIP_212T71)&&(!defined(ONLY)||defined(CASE_212T71))
#include <cstring>
#endif /* CASE_212T71 */

/* _212T91 strrchr declarations */
#if !defined(SKIP_212T91)&&(!defined(ONLY)||defined(CASE_212T91))
#include <cstring>
#endif /* CASE_212T91 */

/* _212T_111 strstr declarations */
#if !defined(SKIP_212T_111)&&(!defined(ONLY)||defined(CASE_212T_111))
#include <cstring>
#endif /* CASE_212T_111 */

/* _212T_131 memchr declarations */
#if !defined(SKIP_212T_131)&&(!defined(ONLY)||defined(CASE_212T_131))
#include <cstring>
#endif /* CASE_212T_131 */

/* _212T_151 wcschr declarations */
#if !defined(SKIP_212T_151)&&(!defined(ONLY)||defined(CASE_212T_151))
#include <cwchar>
#endif /* CASE_212T_151 */

/* _212T_171 wcspbrk declarations */
#if !defined(SKIP_212T_171)&&(!defined(ONLY)||defined(CASE_212T_171))
#include <cwchar>
#endif /* CASE_212T_171 */

/* _212T_191 wcsrchr declarations */
#if !defined(SKIP_212T_191)&&(!defined(ONLY)||defined(CASE_212T_191))
#include <cwchar>
#endif /* CASE_212T_191 */

/* _212T_211 wcsstr declarations */
#if !defined(SKIP_212T_211)&&(!defined(ONLY)||defined(CASE_212T_211))
#include <cwchar>
#endif /* CASE_212T_211 */

/* _212T_231 wmemchr declarations */
#if !defined(SKIP_212T_231)&&(!defined(ONLY)||defined(CASE_212T_231))
#include <cwchar>
#endif /* CASE_212T_231 */

int t217_main(int, char *[])
{
enter_chk("t217.cpp");
/* _2114T11 definition of char_traits<wchar_t>, wstring */
#if !defined(SKIP_2114T11)&&(!defined(ONLY)||defined(CASE_2114T11))
	begin_chk("_2114T11");
	{
	typedef STD char_traits<wchar_t> Strt;
	wchar_t c1;
	Strt::char_type *p0 = &c1; TOUCH(p0);
	typedef STD basic_string<wchar_t, Strt, ALLOCATOR(wchar_t)> Str;
	STD wstring *p1 = (Str *)0; TOUCH(p1);
	STD wstring s1, s2(20, L'a');
	const wchar_t abc[] = {L'a', L'b', L'c', 0};
	const wchar_t def[] = {L'd', L'e', L'f', 0};
	s1 = (abc + s1) + (s1 + def) + (L'g' + s1) + (s1 + L'h');
	bool t1 = s1 == s2 || s1 == abc || def == s1;
	t1 = s1 != s2 || s1 != abc || def != s1;
	t1 = s1 < s2 || s1 < abc || def < s1;
	t1 = s1 > s2 || s1 > abc || def > s1;
	t1 = s1 <= s2 || s1 <= abc || def <= s1;
	t1 = s1 >= s2 || s1 >= abc || def >= s1;
	const wchar_t s1s2[] = {L's', L'1', L' ', L's', L'2', L'\n', 0};
	const STD wstring input(s1s2);
	STD wistringstream ins(input);
	ins >> s1;
	STD getline(ins, s1, L'\n');
	STD wostringstream outs;
	outs << s1;
	chk(1);



	}
	end_chk("_2114T11");
#else
	skip_chk("_2114T11");
#endif /* CASE_2114T11 */

/* _2115T21 char_traits<wchar_t>::assign effects */
#if !defined(SKIP_2115T21)&&(!defined(ONLY)||defined(CASE_2115T21))
	begin_chk("_2115T21");
	{
	typedef STD char_traits<wchar_t> Strt;
	wchar_t x1 = 0, x2 = (wchar_t)(-3);
	Strt::assign(x1, x2);
	ieq(x1, (wchar_t)(-3));


	}
	end_chk("_2115T21");
#else
	skip_chk("_2115T21");
#endif /* CASE_2115T21 */

/* _2115T41 char_traits<wchar_t>::eq returns */
#if !defined(SKIP_2115T41)&&(!defined(ONLY)||defined(CASE_2115T41))
	begin_chk("_2115T41");
	{
	typedef STD char_traits<wchar_t> Strt;
	wchar_t x1 = 0, x2 = (wchar_t)(-3);
	chk(!Strt::eq(x1, x2));
	chk(Strt::eq(x1, x1));


	//###T_2115T61 string_char_traits<wchar_t>::ne returns	DROPPED [MAR96]
	//FILE-SCOPE:
	//#include <string>
	//BLOCK-SCOPE:
	//typedef STD string_char_traits<wchar_t> Strt;
	//wchar_t x1 = 0, x2 = -3;
	//chk(!Strt::ne(x1, x1));
	//chk(Strt::ne(x1, x2));


	}
	end_chk("_2115T41");
#else
	skip_chk("_2115T41");
#endif /* CASE_2115T41 */

/* _2115T81 char_traits<wchar_t>::lt returns */
#if !defined(SKIP_2115T81)&&(!defined(ONLY)||defined(CASE_2115T81))
	begin_chk("_2115T81");
	{
	typedef STD char_traits<wchar_t> Strt;
	wchar_t x1 = 0, x2 = 3;
	chk(Strt::lt(x1, x2));
	chk(!Strt::lt(x1, x1));


	//### _2115T_101 string_char_traits<wchar_t>::eos returns	DROPPED [MAR96]
	//FILE-SCOPE:
	//#include <string>
	//BLOCK-SCOPE:
	//typedef STD string_char_traits<wchar_t> Strt;
	//ieq(Strt::eos(), 0);


	}
	end_chk("_2115T81");
#else
	skip_chk("_2115T81");
#endif /* CASE_2115T81 */

/* _2115T_181 char_traits<wchar_t>::compare returns */
#if !defined(SKIP_2115T_181)&&(!defined(ONLY)||defined(CASE_2115T_181))
	begin_chk("_2115T_181");
	{
	typedef STD char_traits<wchar_t> Strt;
	const wchar_t str1[] = {'a', 'b', '1', 0};
	const wchar_t str2[] = {'a', 'b', '2', 0};
	ieq(Strt::compare(str1, str2, 3), -1);
	ieq(Strt::compare(str1, str2, 2), 0);
	ieq(Strt::compare(&str2[1], &str1[1], 3), 1);


	}
	end_chk("_2115T_181");
#else
	skip_chk("_2115T_181");
#endif /* CASE_2115T_181 */

/* _2115T_201 char_traits<wchar_t>::length returns */
#if !defined(SKIP_2115T_201)&&(!defined(ONLY)||defined(CASE_2115T_201))
	begin_chk("_2115T_201");
	{
	typedef STD char_traits<wchar_t> Strt;
	const wchar_t str1[] = {'a', 'b', '1', 0};
	ieq(Strt::length(str1), 3);
	ieq(Strt::length(&str1[3]), 0);


	}
	end_chk("_2115T_201");
#else
	skip_chk("_2115T_201");
#endif /* CASE_2115T_201 */

/* _2115T_221 char_traits<wchar_t>::copy returns */
#if !defined(SKIP_2115T_221)&&(!defined(ONLY)||defined(CASE_2115T_221))
	begin_chk("_2115T_221");
	{
	typedef STD char_traits<wchar_t> Strt;
	wchar_t str1[10] = {0};
	const wchar_t str2[] = {L'a', L'b', L'2', 0};
	chk(Strt::copy(str1, str2, 0) == str1);
	ieq(str1[0], 0);
	chk(Strt::copy(str1, str2, 2) == str1);
	const wchar_t str3[] = {L'a', L'b', 0};
	ieq(STD wmemcmp(str1, str3, 3), 0);
	chk(Strt::copy(&str1[2], str1, 4) == &str1[2]);
	const wchar_t str4[] = {L'a', L'b', L'a', L'b', L'a', L'b', 0};
	ieq(STD memcmp(str1, str4, 7), 0);


	}
	end_chk("_2115T_221");
#else
	skip_chk("_2115T_221");
#endif /* CASE_2115T_221 */

/* _212T12 <cctype> synopsis */
#if !defined(SKIP_212T12)&&(!defined(ONLY)||defined(CASE_212T12))
	begin_chk("_212T12");
	{
	int c; TOUCH(c);
	c = isalnum('x');
	c = isalpha('x');
	c = iscntrl('x');
	c = isdigit('x');
	c = isgraph('x');
	c = islower('x');
	c = isprint('x');
	c = ispunct('x');
	c = isspace('x');
	c = isupper('x');
	c = isxdigit('x');
	c = tolower('x');
	c = toupper('x');
	chk(1);




	}
	end_chk("_212T12");
#else
	skip_chk("_212T12");
#endif /* CASE_212T12 */

/* _212T13 <cwctype> synopsis */
#if !defined(SKIP_212T13)&&(!defined(ONLY)||defined(CASE_212T13))
	begin_chk("_212T13");
	{
	wint_t weof = WEOF; TOUCH(weof);
	wctrans_t wctr = wctrans("tolower");
	wctype_t wcty = wctype("alpha");
	int c = iswalnum(L'x');
	c = iswalpha(L'x');
	c = iswcntrl(L'x');
	c = iswdigit(L'x');
	c = iswgraph(L'x');
	c = iswlower(L'x');
	c = iswprint(L'x');
	c = iswpunct(L'x');
	c = iswspace(L'x');
	c = iswupper(L'x');
	c = iswxdigit(L'x');
	wint_t ch; TOUCH(ch);
	ch = towlower(L'x');
	ch = towupper(L'x');
	chk(1);



	}
	end_chk("_212T13");
#else
	skip_chk("_212T13");
#endif /* CASE_212T13 */

/* _212T14 <cstring> synopsis */
#if !defined(SKIP_212T14)&&(!defined(ONLY)||defined(CASE_212T14))
	begin_chk("_212T14");
	{
	void *pn = NULL; TOUCH(pn);
	size_t szt = sizeof (pn); TOUCH(szt);
	char a[10], *p; TOUCH(p);
	const char *pc; TOUCH(pc);
	const char *s1 = "abcd";
	const char *s2 = "efgh";
	int i; TOUCH(i);
	i = strcmp(s1, s2);
	i = strcoll(s1, s2);
	p = strcpy(a, s1);
	szt = strcspn(s1, s2);
	p = strerror(0);
	szt = strlen(s1);
	p = strncat(a, s2, 2);
	i = strncmp(s1, s2, 4);
	p = strncpy(a, s2, 3);
	szt = strspn(s1, s2);
	p = strtok(a, s2);
	szt = strxfrm(a, s1, 6);
	pc = strchr(s1, 'c');
	pc = strpbrk(s1, s2);
	pc = strrchr(s1, 'c');
	pc = strstr(s1, s2);


	}
	end_chk("_212T14");
#else
	skip_chk("_212T14");
#endif /* CASE_212T14 */

/* _212T15 <cwchar> synopsis */
#if !defined(SKIP_212T15)&&(!defined(ONLY)||defined(CASE_212T15))
	begin_chk("_212T15");
	{
	void *pn = NULL; TOUCH(pn);
	wchar_t wmin = WCHAR_MIN; TOUCH(wmin);
	wchar_t wmax = WCHAR_MAX; TOUCH(wmax);
	wint_t weof = WEOF; TOUCH(weof);
	mbstate_t mbst = {0}; TOUCH(mbst);
	typedef void (*Pf)(void);
	Pf ptrs[] = {(Pf)&fgetwc, (Pf)&fgetws, (Pf)&fputwc, (Pf)&fputws,
		(Pf)&fwide, (Pf)&fwprintf, (Pf)&fwscanf, (Pf)&getwc, (Pf)&getwchar,
		(Pf)&putwc, (Pf)&putwchar, (Pf)&swprintf, (Pf)&swscanf, (Pf)&ungetwc,
		(Pf)&vfwprintf, (Pf)&vswprintf, (Pf)&vwprintf, (Pf)&wprintf,
		(Pf)&wscanf, (Pf)&mbrlen, (Pf)&mbrtowc, (Pf)&mbsrtowcs, (Pf)&mbsinit,
		(Pf)&wcrtomb, (Pf)&wcsrtombs, (Pf)&wcstol, (Pf)&wcscat, (Pf)&wcscmp,
		(Pf)&wcscoll, (Pf)&wcscpy, (Pf)&wcscspn, (Pf)&wcslen, (Pf)&wcsncat,
		(Pf)&wcsncmp, (Pf)&wcsncpy, (Pf)&wcsspn, (Pf)&wcstok, (Pf)&wcsxfrm,
		(Pf)&wmemcmp, (Pf)&wmemcpy, (Pf)&wmemmove, (Pf)&wmemset, (Pf)&wcsftime,
		(Pf)&btowc, (Pf)&wcstod, (Pf)&wcstoul, (Pf)&wctob}; TOUCH(ptrs);
	const wchar_t *pc; TOUCH(pc);
	const wchar_t s1[] = {L'a', L'b', L'c', L'd', 0};
	const wchar_t s2[] = {L'e', L'f', L'g', L'h', 0};
	pc = wmemchr(s1, 4, L'c');
	pc = wcschr(s1, L'c');
	pc = wcspbrk(s1, s2);
	pc = wcsrchr(s1, L'c');
	pc = wcsstr(s1, s2);
	chk(1);


	}
	end_chk("_212T15");
#else
	skip_chk("_212T15");
#endif /* CASE_212T15 */

/* _212T16 <cstdlib> synopsis */
#if !defined(SKIP_212T16)&&(!defined(ONLY)||defined(CASE_212T16))
	begin_chk("_212T16");
	{
	size_t mbmax = MB_CUR_MAX; TOUCH(mbmax);
	typedef void (*Pf)(void);
	Pf ptrs[] = {(Pf)&atol, (Pf)&atof, (Pf)&atoi, (Pf)&mblen,
		(Pf)&mbstowcs, (Pf)&mbtowc, (Pf)&strtod, (Pf)&strtol, (Pf)&strtoul,
		(Pf)&wctomb, (Pf)&wcstombs}; TOUCH(ptrs);
	chk(1);


	}
	end_chk("_212T16");
#else
	skip_chk("_212T16");
#endif /* CASE_212T16 */

/* _212T51 strchr declarations */
#if !defined(SKIP_212T51)&&(!defined(ONLY)||defined(CASE_212T51))
	begin_chk("_212T51");
	{
	const char *pc = strchr((const char *)"abcd", 'b');
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, 'b');
	char *p = strchr((char *)"abcd", 'b');
	chk(p != 0);
	if (p != 0)
		ieq(*p, 'b');


	}
	end_chk("_212T51");
#else
	skip_chk("_212T51");
#endif /* CASE_212T51 */

/* _212T71 strpbrk declarations */
#if !defined(SKIP_212T71)&&(!defined(ONLY)||defined(CASE_212T71))
	begin_chk("_212T71");
	{
	const char *pc = strpbrk((const char *)"abcd", "b");
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, 'b');
	char *p = strpbrk((char *)"abcd", "b");
	chk(p != 0);
	if (p != 0)
		ieq(*p, 'b');


	}
	end_chk("_212T71");
#else
	skip_chk("_212T71");
#endif /* CASE_212T71 */

/* _212T91 strrchr declarations */
#if !defined(SKIP_212T91)&&(!defined(ONLY)||defined(CASE_212T91))
	begin_chk("_212T91");
	{
	const char *pc = strrchr((const char *)"abcd", 'b');
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, 'b');
	char *p = strrchr((char *)"abcd", 'b');
	chk(p != 0);
	if (p != 0)
		ieq(*p, 'b');


	}
	end_chk("_212T91");
#else
	skip_chk("_212T91");
#endif /* CASE_212T91 */

/* _212T_111 strstr declarations */
#if !defined(SKIP_212T_111)&&(!defined(ONLY)||defined(CASE_212T_111))
	begin_chk("_212T_111");
	{
	const char *pc = strstr((const char *)"abcd", "b");
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, 'b');
	char *p = strstr((char *)"abcd", "b");
	chk(p != 0);
	if (p != 0)
		ieq(*p, 'b');


	}
	end_chk("_212T_111");
#else
	skip_chk("_212T_111");
#endif /* CASE_212T_111 */

/* _212T_131 memchr declarations */
#if !defined(SKIP_212T_131)&&(!defined(ONLY)||defined(CASE_212T_131))
	begin_chk("_212T_131");
	{
	const void *pc = memchr((const char *)"abcd", 'b', 4);
	chk(pc != 0);
	if (pc != 0)
		ieq(*(const char *)pc, 'b');
	void *p = memchr((char *)"abcd", 'b', 4);
	chk(p != 0);
	if (p != 0)
		ieq(*(const char *)p, 'b');


	}
	end_chk("_212T_131");
#else
	skip_chk("_212T_131");
#endif /* CASE_212T_131 */

/* _212T_151 wcschr declarations */
#if !defined(SKIP_212T_151)&&(!defined(ONLY)||defined(CASE_212T_151))
	begin_chk("_212T_151");
	{
	const wchar_t s1[] = {L'a', L'b', L'c', L'd', 0};
	const wchar_t *pc = wcschr(s1, L'b');
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, L'b');
	wchar_t *p = wcschr((wchar_t *)s1, L'b');
	chk(p != 0);
	if (p != 0)
		ieq(*p, L'b');


	}
	end_chk("_212T_151");
#else
	skip_chk("_212T_151");
#endif /* CASE_212T_151 */

/* _212T_171 wcspbrk declarations */
#if !defined(SKIP_212T_171)&&(!defined(ONLY)||defined(CASE_212T_171))
	begin_chk("_212T_171");
	{
	const wchar_t s1[] = {L'a', L'b', L'c', L'd', 0};
	const wchar_t s2[] = {L'b', 0};
	const wchar_t *pc = wcspbrk(s1, s2);
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, L'b');
	wchar_t *p = wcspbrk((wchar_t *)s1, s2);
	chk(p != 0);
	if (p != 0)
		ieq(*p, L'b');


	}
	end_chk("_212T_171");
#else
	skip_chk("_212T_171");
#endif /* CASE_212T_171 */

/* _212T_191 wcsrchr declarations */
#if !defined(SKIP_212T_191)&&(!defined(ONLY)||defined(CASE_212T_191))
	begin_chk("_212T_191");
	{
	const wchar_t s1[] = {L'a', L'b', L'c', L'd', 0};
	const wchar_t *pc = wcsrchr(s1, L'b');
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, L'b');
	wchar_t *p = wcsrchr((wchar_t *)s1, L'b');
	chk(p != 0);
	if (p != 0)
		ieq(*p, L'b');


	}
	end_chk("_212T_191");
#else
	skip_chk("_212T_191");
#endif /* CASE_212T_191 */

/* _212T_211 wcsstr declarations */
#if !defined(SKIP_212T_211)&&(!defined(ONLY)||defined(CASE_212T_211))
	begin_chk("_212T_211");
	{
	const wchar_t s1[] = {L'a', L'b', L'c', L'd', 0};
	const wchar_t s2[] = {L'b', 0};
	const wchar_t *pc = wcsstr(s1, s2);
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, L'b');
	wchar_t *p = wcsstr((wchar_t *)s1, s2);
	chk(p != 0);
	if (p != 0)
		ieq(*p, L'b');


	}
	end_chk("_212T_211");
#else
	skip_chk("_212T_211");
#endif /* CASE_212T_211 */

/* _212T_231 wmemchr declarations */
#if !defined(SKIP_212T_231)&&(!defined(ONLY)||defined(CASE_212T_231))
	begin_chk("_212T_231");
	{
	const wchar_t s1[] = {L'a', L'b', L'c', L'd', 0};
	const wchar_t *pc = wmemchr(s1, L'b', 4);
	chk(pc != 0);
	if (pc != 0)
		ieq(*pc, L'b');
	wchar_t *p = wmemchr((wchar_t *)s1, L'b', 4);
	chk(p != 0);
	if (p != 0)
		ieq(*p, L'b');

	}
	end_chk("_212T_231");
#else
	skip_chk("_212T_231");
#endif /* CASE_212T_231 */

return leave_chk("t217");
}
/* V3.10:0009 */
