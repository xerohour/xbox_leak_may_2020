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
#define CASE_192T23		Table 30: <cassert> synopsis
#define CASE_193T21		Table 31: <cerrno> synopsis
#endif /* IS_EMBEDDED */

/* _191T41 <exception> synopsis */
#if !defined(SKIP_191T41)&&(!defined(ONLY)||defined(CASE_191T41))
#if DISALLOW_USER_NAME_PROTECTION
 #include <stdexcept>
#else
	#define xmsg	"xmsg"	/* looking for trouble */
 #include <stdexcept>
	#undef xmsg
#endif
#endif /* CASE_191T41 */

/* _1911T11 exception definition */
#if !defined(SKIP_1911T11)&&(!defined(ONLY)||defined(CASE_1911T11))
#include <exception>	/* now in exception [JUL95] */
#endif /* CASE_1911T11 */

/* _1911T31 exception::exception() effects */
#if !defined(SKIP_1911T31)&&(!defined(ONLY)||defined(CASE_1911T31))
#include <exception>	/* now in exception [JUL95] */
#endif /* CASE_1911T31 */

/* _1911T61 exception::exception(const exception&), operator= effects */
#if !defined(SKIP_1911T61)&&(!defined(ONLY)||defined(CASE_1911T61))
#include <exception>	/* now in exception [JUL95] */
#endif /* CASE_1911T61 */

/* _1911T91 exception::~exception() effects */
#if !defined(SKIP_1911T91)&&(!defined(ONLY)||defined(CASE_1911T91))
#include <exception>	/* now in exception [JUL95] */
#endif /* CASE_1911T91 */

/* _1911T_111 exception::what returns */
#if !defined(SKIP_1911T_111)&&(!defined(ONLY)||defined(CASE_1911T_111))
#include <cstring>
#include <exception>	/* now in exception [JUL95] */
#endif /* CASE_1911T_111 */

/* _1912T11 logic_error definition */
#if !defined(SKIP_1912T11)&&(!defined(ONLY)||defined(CASE_1912T11))
#include <stdexcept>
#endif /* CASE_1912T11 */

/* _1912T31 logic_error::logic_error(const string&) effects */
#if !defined(SKIP_1912T31)&&(!defined(ONLY)||defined(CASE_1912T31))
#include <stdexcept>
#include <string>
#endif /* CASE_1912T31 */

/* _1912T41 logic_error::logic_error(const string&) postcondition */
#if !defined(SKIP_1912T41)&&(!defined(ONLY)||defined(CASE_1912T41))
#include <stdexcept>
#include <string>
#endif /* CASE_1912T41 */

/* _1913T11 domain_error definition */
#if !defined(SKIP_1913T11)&&(!defined(ONLY)||defined(CASE_1913T11))
#include <stdexcept>
#endif /* CASE_1913T11 */

/* _1913T31 domain_error::domain_error(const string&) effects */
#if !defined(SKIP_1913T31)&&(!defined(ONLY)||defined(CASE_1913T31))
#include <stdexcept>
#include <string>
#endif /* CASE_1913T31 */

/* _1913T41 domain_error::domain_error(const string&) postcondition */
#if !defined(SKIP_1913T41)&&(!defined(ONLY)||defined(CASE_1913T41))
#include <stdexcept>
#include <string>
#endif /* CASE_1913T41 */

/* _1914T11 invalid_argument definition */
#if !defined(SKIP_1914T11)&&(!defined(ONLY)||defined(CASE_1914T11))
#include <stdexcept>
#endif /* CASE_1914T11 */

/* _1914T31 invalid_argument::invalid_argument(const string&) effects */
#if !defined(SKIP_1914T31)&&(!defined(ONLY)||defined(CASE_1914T31))
#include <stdexcept>
#include <string>
#endif /* CASE_1914T31 */

/* _1914T41 invalid_argument::invalid_argument(const string&) postcondition */
#if !defined(SKIP_1914T41)&&(!defined(ONLY)||defined(CASE_1914T41))
#include <stdexcept>
#include <string>
#endif /* CASE_1914T41 */

/* _1915T11 length_error definition */
#if !defined(SKIP_1915T11)&&(!defined(ONLY)||defined(CASE_1915T11))
#include <stdexcept>
#endif /* CASE_1915T11 */

/* _1915T31 length_error::length_error(const string&) effects */
#if !defined(SKIP_1915T31)&&(!defined(ONLY)||defined(CASE_1915T31))
#include <stdexcept>
#include <string>
#endif /* CASE_1915T31 */

/* _1915T41 length_error::length_error(const string&) postcondition */
#if !defined(SKIP_1915T41)&&(!defined(ONLY)||defined(CASE_1915T41))
#include <stdexcept>
#include <string>
#endif /* CASE_1915T41 */

/* _1916T11 out_of_range definition */
#if !defined(SKIP_1916T11)&&(!defined(ONLY)||defined(CASE_1916T11))
#include <stdexcept>
#endif /* CASE_1916T11 */

/* _1916T31 out_of_range::out_of_range(const string&) effects */
#if !defined(SKIP_1916T31)&&(!defined(ONLY)||defined(CASE_1916T31))
#include <stdexcept>
#include <string>
#endif /* CASE_1916T31 */

/* _1916T41 out_of_range::out_of_range(const string&) postcondition */
#if !defined(SKIP_1916T41)&&(!defined(ONLY)||defined(CASE_1916T41))
#include <stdexcept>
#include <string>
#endif /* CASE_1916T41 */

/* _1917T11 runtime_error definition */
#if !defined(SKIP_1917T11)&&(!defined(ONLY)||defined(CASE_1917T11))
#include <stdexcept>
#endif /* CASE_1917T11 */

/* _1917T31 runtime_error::runtime_error(const string&) effects */
#if !defined(SKIP_1917T31)&&(!defined(ONLY)||defined(CASE_1917T31))
#include <stdexcept>
#include <string>
#endif /* CASE_1917T31 */

/* _1917T41 runtime_error::runtime_error(const string&) postcondition */
#if !defined(SKIP_1917T41)&&(!defined(ONLY)||defined(CASE_1917T41))
#include <stdexcept>
#include <string>
#endif /* CASE_1917T41 */

/* _1918T11 range_error definition */
#if !defined(SKIP_1918T11)&&(!defined(ONLY)||defined(CASE_1918T11))
#include <stdexcept>
#endif /* CASE_1918T11 */

/* _1918T31 range_error::range_error(const string&) effects */
#if !defined(SKIP_1918T31)&&(!defined(ONLY)||defined(CASE_1918T31))
#include <stdexcept>
#include <string>
#endif /* CASE_1918T31 */

/* _1918T41 range_error::range_error(const string&) postcondition */
#if !defined(SKIP_1918T41)&&(!defined(ONLY)||defined(CASE_1918T41))
#include <stdexcept>
#include <string>
#endif /* CASE_1918T41 */

/* _1919T11 overflow_error definition */
#if !defined(SKIP_1919T11)&&(!defined(ONLY)||defined(CASE_1919T11))
#include <stdexcept>
#endif /* CASE_1919T11 */

/* _1919T11a underflow_error definition */
#if !defined(SKIP_1919T11a)&&(!defined(ONLY)||defined(CASE_1919T11a))
#include <stdexcept>
#endif /* CASE_1919T11a */

/* _1919T31 overflow_error::overflow_error(const string&) effects */
#if !defined(SKIP_1919T31)&&(!defined(ONLY)||defined(CASE_1919T31))
#include <stdexcept>
#include <string>
#endif /* CASE_1919T31 */

/* _1919T31a underflow_error::underflow_error(const string&) effects */
#if !defined(SKIP_1919T31a)&&(!defined(ONLY)||defined(CASE_1919T31a))
#include <stdexcept>
#include <string>
#endif /* CASE_1919T31a */

/* _1919T41 overflow_error::overflow_error(const string&) postcondition */
#if !defined(SKIP_1919T41)&&(!defined(ONLY)||defined(CASE_1919T41))
#include <stdexcept>
#include <string>
#endif /* CASE_1919T41 */

/* _1919T41a underflow_error::underflow_error(const string&) postcondition */
#if !defined(SKIP_1919T41a)&&(!defined(ONLY)||defined(CASE_1919T41a))
#include <stdexcept>
#include <string>
#endif /* CASE_1919T41a */

/* _192T23 Table 30: <cassert> synopsis */
#if !defined(SKIP_192T23)&&(!defined(ONLY)||defined(CASE_192T23))
#ifdef NDEBUG
 #include <cassert>
void f1_192T23()
	{assert(0); }
 #undef NDEBUG
 #include <cassert>
void f2_192T23()
	{assert(1); }
 #define NDEBUG
 #include <cassert>
#else
 #define NDEBUG
 #include <cassert>
void f1_192T23()
	{assert(0); }
 #undef NDEBUG
 #include <cassert>
void f2_192T23()
	{assert(1); }
#endif
#endif /* CASE_192T23 */

/* _193T21 Table 31: <cerrno> synopsis */
#if !defined(SKIP_193T21)&&(!defined(ONLY)||defined(CASE_193T21))
#include <cerrno>
#endif /* CASE_193T21 */

int t190_main(int, char *[])
{
enter_chk("t190.cpp");
/* _191T41 <exception> synopsis */
#if !defined(SKIP_191T41)&&(!defined(ONLY)||defined(CASE_191T41))
	begin_chk("_191T41");
	{
	STD exception *p1; TOUCH(p1);
	STD logic_error *p2; TOUCH(p2);
	STD domain_error *p3; TOUCH(p3);
	STD invalid_argument *p4; TOUCH(p4);
	STD length_error *p5; TOUCH(p5);
	STD out_of_range *p6; TOUCH(p6);
	STD runtime_error *p7; TOUCH(p7);
	STD range_error *p8; TOUCH(p8);
	STD overflow_error *p9; TOUCH(p9);
	STD underflow_error *p10; TOUCH(p10);	// [MAR96]
	chk(1);




	}
	end_chk("_191T41");
#else
	skip_chk("_191T41");
#endif /* CASE_191T41 */

/* _1911T11 exception definition */
#if !defined(SKIP_1911T11)&&(!defined(ONLY)||defined(CASE_1911T11))
	begin_chk("_1911T11");
	{
	STD exception *p1; TOUCH(p1);
	chk(1);


	}
	end_chk("_1911T11");
#else
	skip_chk("_1911T11");
#endif /* CASE_1911T11 */

/* _1911T31 exception::exception() effects */
#if !defined(SKIP_1911T31)&&(!defined(ONLY)||defined(CASE_1911T31))
	begin_chk("_1911T31");
	{
	STD exception x;
	chk(1);


	}
	end_chk("_1911T31");
#else
	skip_chk("_1911T31");
#endif /* CASE_1911T31 */

/* _1911T61 exception::exception(const exception&), operator= effects */
#if !defined(SKIP_1911T61)&&(!defined(ONLY)||defined(CASE_1911T61))
	begin_chk("_1911T61");
	{
	STD exception x;
	STD exception y(x);
	x = y;
	chk(1);


	}
	end_chk("_1911T61");
#else
	skip_chk("_1911T61");
#endif /* CASE_1911T61 */

/* _1911T91 exception::~exception() effects */
#if !defined(SKIP_1911T91)&&(!defined(ONLY)||defined(CASE_1911T91))
	begin_chk("_1911T91");
	{
	STD exception *px = new STD exception;
	px->~exception();
	chk(1);


	}
	end_chk("_1911T91");
#else
	skip_chk("_1911T91");
#endif /* CASE_1911T91 */

/* _1911T_111 exception::what returns */
#if !defined(SKIP_1911T_111)&&(!defined(ONLY)||defined(CASE_1911T_111))
	begin_chk("_1911T_111");
	{
	STD exception x;
	const char *s = x.what();
	chk(s != 0);


	}
	end_chk("_1911T_111");
#else
	skip_chk("_1911T_111");
#endif /* CASE_1911T_111 */

/* _1912T11 logic_error definition */
#if !defined(SKIP_1912T11)&&(!defined(ONLY)||defined(CASE_1912T11))
	begin_chk("_1912T11");
	{
	STD logic_error *p = 0;
	STD exception *q = p; TOUCH(q);
	chk(1);


	}
	end_chk("_1912T11");
#else
	skip_chk("_1912T11");
#endif /* CASE_1912T11 */

/* _1912T31 logic_error::logic_error(const string&) effects */
#if !defined(SKIP_1912T31)&&(!defined(ONLY)||defined(CASE_1912T31))
	begin_chk("_1912T31");
	{
	const STD string arg("argument");
	STD logic_error x(arg);
	chk(1);


	}
	end_chk("_1912T31");
#else
	skip_chk("_1912T31");
#endif /* CASE_1912T31 */

/* _1912T41 logic_error::logic_error(const string&) postcondition */
#if !defined(SKIP_1912T41)&&(!defined(ONLY)||defined(CASE_1912T41))
	begin_chk("_1912T41");
	{
	const STD string arg("argument");
	STD logic_error x(arg);
	steq(x.what(), arg.c_str());


	}
	end_chk("_1912T41");
#else
	skip_chk("_1912T41");
#endif /* CASE_1912T41 */

/* _1913T11 domain_error definition */
#if !defined(SKIP_1913T11)&&(!defined(ONLY)||defined(CASE_1913T11))
	begin_chk("_1913T11");
	{
	STD domain_error *p = 0;
	STD logic_error *q = p; TOUCH(q);
	chk(1);


	}
	end_chk("_1913T11");
#else
	skip_chk("_1913T11");
#endif /* CASE_1913T11 */

/* _1913T31 domain_error::domain_error(const string&) effects */
#if !defined(SKIP_1913T31)&&(!defined(ONLY)||defined(CASE_1913T31))
	begin_chk("_1913T31");
	{
	const STD string arg("argument");
	STD domain_error x(arg);
	chk(1);


	}
	end_chk("_1913T31");
#else
	skip_chk("_1913T31");
#endif /* CASE_1913T31 */

/* _1913T41 domain_error::domain_error(const string&) postcondition */
#if !defined(SKIP_1913T41)&&(!defined(ONLY)||defined(CASE_1913T41))
	begin_chk("_1913T41");
	{
	const STD string arg("argument");
	STD domain_error x(arg);
	steq(x.what(), arg.c_str());


	}
	end_chk("_1913T41");
#else
	skip_chk("_1913T41");
#endif /* CASE_1913T41 */

/* _1914T11 invalid_argument definition */
#if !defined(SKIP_1914T11)&&(!defined(ONLY)||defined(CASE_1914T11))
	begin_chk("_1914T11");
	{
	STD invalid_argument *p = 0;
	STD logic_error *q = p; TOUCH(q);
	chk(1);


	}
	end_chk("_1914T11");
#else
	skip_chk("_1914T11");
#endif /* CASE_1914T11 */

/* _1914T31 invalid_argument::invalid_argument(const string&) effects */
#if !defined(SKIP_1914T31)&&(!defined(ONLY)||defined(CASE_1914T31))
	begin_chk("_1914T31");
	{
	const STD string arg("argument");
	STD invalid_argument x(arg);
	chk(1);


	}
	end_chk("_1914T31");
#else
	skip_chk("_1914T31");
#endif /* CASE_1914T31 */

/* _1914T41 invalid_argument::invalid_argument(const string&) postcondition */
#if !defined(SKIP_1914T41)&&(!defined(ONLY)||defined(CASE_1914T41))
	begin_chk("_1914T41");
	{
	const STD string arg("argument");
	STD invalid_argument x(arg);
	steq(x.what(), arg.c_str());


	}
	end_chk("_1914T41");
#else
	skip_chk("_1914T41");
#endif /* CASE_1914T41 */

/* _1915T11 length_error definition */
#if !defined(SKIP_1915T11)&&(!defined(ONLY)||defined(CASE_1915T11))
	begin_chk("_1915T11");
	{
	STD length_error *p = 0;
	STD logic_error *q = p; TOUCH(q);
	chk(1);


	}
	end_chk("_1915T11");
#else
	skip_chk("_1915T11");
#endif /* CASE_1915T11 */

/* _1915T31 length_error::length_error(const string&) effects */
#if !defined(SKIP_1915T31)&&(!defined(ONLY)||defined(CASE_1915T31))
	begin_chk("_1915T31");
	{
	const STD string arg("argument");
	STD length_error x(arg);
	chk(1);


	}
	end_chk("_1915T31");
#else
	skip_chk("_1915T31");
#endif /* CASE_1915T31 */

/* _1915T41 length_error::length_error(const string&) postcondition */
#if !defined(SKIP_1915T41)&&(!defined(ONLY)||defined(CASE_1915T41))
	begin_chk("_1915T41");
	{
	const STD string arg("argument");
	STD length_error x(arg);
	steq(x.what(), arg.c_str());


	}
	end_chk("_1915T41");
#else
	skip_chk("_1915T41");
#endif /* CASE_1915T41 */

/* _1916T11 out_of_range definition */
#if !defined(SKIP_1916T11)&&(!defined(ONLY)||defined(CASE_1916T11))
	begin_chk("_1916T11");
	{
	STD out_of_range *p = 0;
	STD logic_error *q = p; TOUCH(q);
	chk(1);


	}
	end_chk("_1916T11");
#else
	skip_chk("_1916T11");
#endif /* CASE_1916T11 */

/* _1916T31 out_of_range::out_of_range(const string&) effects */
#if !defined(SKIP_1916T31)&&(!defined(ONLY)||defined(CASE_1916T31))
	begin_chk("_1916T31");
	{
	const STD string arg("argument");
	STD out_of_range x(arg);
	chk(1);


	}
	end_chk("_1916T31");
#else
	skip_chk("_1916T31");
#endif /* CASE_1916T31 */

/* _1916T41 out_of_range::out_of_range(const string&) postcondition */
#if !defined(SKIP_1916T41)&&(!defined(ONLY)||defined(CASE_1916T41))
	begin_chk("_1916T41");
	{
	const STD string arg("argument");
	STD out_of_range x(arg);
	steq(x.what(), arg.c_str());


	}
	end_chk("_1916T41");
#else
	skip_chk("_1916T41");
#endif /* CASE_1916T41 */

/* _1917T11 runtime_error definition */
#if !defined(SKIP_1917T11)&&(!defined(ONLY)||defined(CASE_1917T11))
	begin_chk("_1917T11");
	{
	STD runtime_error *p = 0;
	STD exception *q = p; TOUCH(q);
	chk(1);


	}
	end_chk("_1917T11");
#else
	skip_chk("_1917T11");
#endif /* CASE_1917T11 */

/* _1917T31 runtime_error::runtime_error(const string&) effects */
#if !defined(SKIP_1917T31)&&(!defined(ONLY)||defined(CASE_1917T31))
	begin_chk("_1917T31");
	{
	const STD string arg("argument");
	STD runtime_error x(arg);
	chk(1);


	}
	end_chk("_1917T31");
#else
	skip_chk("_1917T31");
#endif /* CASE_1917T31 */

/* _1917T41 runtime_error::runtime_error(const string&) postcondition */
#if !defined(SKIP_1917T41)&&(!defined(ONLY)||defined(CASE_1917T41))
	begin_chk("_1917T41");
	{
	const STD string arg("argument");
	STD runtime_error x(arg);
	steq(x.what(), arg.c_str());


	}
	end_chk("_1917T41");
#else
	skip_chk("_1917T41");
#endif /* CASE_1917T41 */

/* _1918T11 range_error definition */
#if !defined(SKIP_1918T11)&&(!defined(ONLY)||defined(CASE_1918T11))
	begin_chk("_1918T11");
	{
	STD range_error *p = 0;
	STD runtime_error *q = p; TOUCH(q);
	chk(1);


	}
	end_chk("_1918T11");
#else
	skip_chk("_1918T11");
#endif /* CASE_1918T11 */

/* _1918T31 range_error::range_error(const string&) effects */
#if !defined(SKIP_1918T31)&&(!defined(ONLY)||defined(CASE_1918T31))
	begin_chk("_1918T31");
	{
	const STD string arg("argument");
	STD range_error x(arg);
	chk(1);


	}
	end_chk("_1918T31");
#else
	skip_chk("_1918T31");
#endif /* CASE_1918T31 */

/* _1918T41 range_error::range_error(const string&) postcondition */
#if !defined(SKIP_1918T41)&&(!defined(ONLY)||defined(CASE_1918T41))
	begin_chk("_1918T41");
	{
	const STD string arg("argument");
	STD range_error x(arg);
	steq(x.what(), arg.c_str());


	}
	end_chk("_1918T41");
#else
	skip_chk("_1918T41");
#endif /* CASE_1918T41 */

/* _1919T11 overflow_error definition */
#if !defined(SKIP_1919T11)&&(!defined(ONLY)||defined(CASE_1919T11))
	begin_chk("_1919T11");
	{
	STD overflow_error *p = 0;
	STD runtime_error *q = p; TOUCH(q);
	chk(1);

	}
	end_chk("_1919T11");
#else
	skip_chk("_1919T11");
#endif /* CASE_1919T11 */

/* _1919T11a underflow_error definition */
#if !defined(SKIP_1919T11a)&&(!defined(ONLY)||defined(CASE_1919T11a))
	begin_chk("_1919T11a");
	{
	STD underflow_error *p = 0;	// [MAR96]
	STD runtime_error *q = p; TOUCH(q);
	chk(1);


	}
	end_chk("_1919T11a");
#else
	skip_chk("_1919T11a");
#endif /* CASE_1919T11a */

/* _1919T31 overflow_error::overflow_error(const string&) effects */
#if !defined(SKIP_1919T31)&&(!defined(ONLY)||defined(CASE_1919T31))
	begin_chk("_1919T31");
	{
	const STD string arg("argument");
	STD overflow_error x(arg);
	chk(1);

	}
	end_chk("_1919T31");
#else
	skip_chk("_1919T31");
#endif /* CASE_1919T31 */

/* _1919T31a underflow_error::underflow_error(const string&) effects */
#if !defined(SKIP_1919T31a)&&(!defined(ONLY)||defined(CASE_1919T31a))
	begin_chk("_1919T31a");
	{
	const STD string arg("argument");
	STD underflow_error x(arg);	// [MAR96]
	chk(1);


	}
	end_chk("_1919T31a");
#else
	skip_chk("_1919T31a");
#endif /* CASE_1919T31a */

/* _1919T41 overflow_error::overflow_error(const string&) postcondition */
#if !defined(SKIP_1919T41)&&(!defined(ONLY)||defined(CASE_1919T41))
	begin_chk("_1919T41");
	{
	const STD string arg("argument");
	STD overflow_error x(arg);
	steq(x.what(), arg.c_str());

	}
	end_chk("_1919T41");
#else
	skip_chk("_1919T41");
#endif /* CASE_1919T41 */

/* _1919T41a underflow_error::underflow_error(const string&) postcondition */
#if !defined(SKIP_1919T41a)&&(!defined(ONLY)||defined(CASE_1919T41a))
	begin_chk("_1919T41a");
	{
	const STD string arg("argument");
	STD underflow_error x(arg);	// [MAR96]
	steq(x.what(), arg.c_str());


	}
	end_chk("_1919T41a");
#else
	skip_chk("_1919T41a");
#endif /* CASE_1919T41a */

/* _192T23 Table 30: <cassert> synopsis */
#if !defined(SKIP_192T23)&&(!defined(ONLY)||defined(CASE_192T23))
	begin_chk("_192T23");
	{
	f1_192T23();	// suppressed failure
	f2_192T23();	// unsuppressed success
	chk(1);


	}
	end_chk("_192T23");
#else
	skip_chk("_192T23");
#endif /* CASE_192T23 */

/* _193T21 Table 31: <cerrno> synopsis */
#if !defined(SKIP_193T21)&&(!defined(ONLY)||defined(CASE_193T21))
	begin_chk("_193T21");
	{
	errno = EDOM;
	ieq(errno, EDOM);
	errno = ERANGE;
	ieq(errno, ERANGE);
	errno = 0;
	ieq(errno, 0);

	}
	end_chk("_193T21");
#else
	skip_chk("_193T21");
#endif /* CASE_193T21 */

return leave_chk("t190");
}
/* V3.10:0009 */
