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
#define CASE_18422T11   5 new_handler definition
#define CASE_18423T21   8 set_new_handler returns
#define CASE_186T21    24 <exception> synopsis
#define CASE_18611T11  25 bad_exception definition
#define CASE_18611T31  26 bad_exception::bad_exception() effects
#define CASE_18611T51  27 bad_exception::bad_exception(const bad_exception&), operator= effects
#define CASE_18611T81  28 bad_exception::what returns
#define CASE_18612T11  29 unexpected_handler definition
#define CASE_18612T31  30 unexpected handler default behavior
#define CASE_18613T11  31 set_unexpected effects
#define CASE_18613T31  32 set_unexpected returns
#define CASE_18614T11  33 unexpected() called by exception-specification failure
#define CASE_18614T21  34 unexpected() effects
#define CASE_18621T11  35 terminate_handler definition
#define CASE_18621T31  36 terminate handler default behavior
#define CASE_18622T11  37 set_terminate effects
#define CASE_18622T31  38 set_terminate returns
#define CASE_18623T11  39 terminate() called by exception handling
#define CASE_18623T21  40 terminate() effects
#define CASE_187T21    41 Table 28: <cstdarg> synopsis
#define CASE_187T31    42 Table 28 [sic]: <csetjmp> synopsis
#define CASE_187T41    43 Table 28 [sic]: <ctime> synopsis
#define CASE_187T51    44 Table 28 [sic]: <csignal> synopsis
#define CASE_187T61    45 Table 28 [sic]: <cstdlib> synopsis
#endif /* IS_EMBEDDED */
#if DISALLOW_EXCEPTIONS
#define SKIP_186T21    24 <exception> synopsis
#define SKIP_18611T11  25 bad_exception definition
#define SKIP_18611T31  26 bad_exception::bad_exception() effects
#define SKIP_18611T51  27 bad_exception::bad_exception(const bad_exception&), operator= effects
#define SKIP_18611T81  28 bad_exception::what returns
#define SKIP_18612T11  29 unexpected_handler definition
#define SKIP_18612T31  30 unexpected handler default behavior
#define SKIP_18613T11  31 set_unexpected effects
#define SKIP_18613T31  32 set_unexpected returns
#define SKIP_18614T11  33 unexpected() called by exception-specification failure
#define SKIP_18614T21  34 unexpected() effects
#define SKIP_18621T11  35 terminate_handler definition
#define SKIP_18621T31  36 terminate handler default behavior
#define SKIP_18622T11  37 set_terminate effects
#define SKIP_18622T31  38 set_terminate returns
#define SKIP_18623T11  39 terminate() called by exception handling
#define SKIP_18623T21  40 terminate() effects
#endif

/* _18421T11 bad_alloc definition */
#if !defined(SKIP_18421T11)&&(!defined(ONLY)||defined(CASE_18421T11))
#include <new>
#endif /* CASE_18421T11 */

/* _18421T31 bad_alloc::bad_alloc() effects */
#if !defined(SKIP_18421T31)&&(!defined(ONLY)||defined(CASE_18421T31))
#include <new>
#endif /* CASE_18421T31 */

/* _18421T51 bad_alloc::operator= effects */
#if !defined(SKIP_18421T51)&&(!defined(ONLY)||defined(CASE_18421T51))
#include <new>
#endif /* CASE_18421T51 */

/* _18421T81 bad_alloc::what() returns */
#if !defined(SKIP_18421T81)&&(!defined(ONLY)||defined(CASE_18421T81))
#include <cstring>
#include <new>
#endif /* CASE_18421T81 */

/* _18422T11 new_handler definition */
#if !defined(SKIP_18422T11)&&(!defined(ONLY)||defined(CASE_18422T11))
#include <new>
static void newh_18422T11()
	{}
#include <new>
#endif /* CASE_18422T11 */

/* _18422T31 new handler default behavior */
#if !defined(SKIP_18422T31)&&(!defined(ONLY)||defined(CASE_18422T31))
#include <new>
#endif /* CASE_18422T31 */

/* _18423T21 set_new_handler returns */
#if !defined(SKIP_18423T21)&&(!defined(ONLY)||defined(CASE_18423T21))
#include <new>
static void newh_18423T21()
	{}
#endif /* CASE_18423T21 */

/* _185T31 <stdexcept> synopsis */
#if !defined(SKIP_185T31)&&(!defined(ONLY)||defined(CASE_185T31))
#if DISALLOW_USER_NAME_PROTECTION
 #include <typeinfo>
#else
	#define Bad_cast	"Bad_cast"	/* looking for trouble */
	#define Bad_typeid	"Bad_typeid"
	#define typeinfo	"typeinfo"
 #include <typeinfo>
	#undef Bad_cast
	#undef Bad_typeid
	#undef typeinfo
#endif
#endif /* CASE_185T31 */

/* _1851T11 type_info definition */
#if !defined(SKIP_1851T11)&&(!defined(ONLY)||defined(CASE_1851T11))
#include <typeinfo>
#endif /* CASE_1851T11 */

/* _1851T31 type_info::operator== returns */
#if !defined(SKIP_1851T31)&&(!defined(ONLY)||defined(CASE_1851T31))
#include <typeinfo>
#endif /* CASE_1851T31 */

/* _1851T51 type_info::operator!= returns */
#if !defined(SKIP_1851T51)&&(!defined(ONLY)||defined(CASE_1851T51))
#include <typeinfo>
#endif /* CASE_1851T51 */

/* _1851T81 type_info::before returns */
#if !defined(SKIP_1851T81)&&(!defined(ONLY)||defined(CASE_1851T81))
#include <typeinfo>
#endif /* CASE_1851T81 */

/* _1851T_101 type_info::name returns */
#if !defined(SKIP_1851T_101)&&(!defined(ONLY)||defined(CASE_1851T_101))
#include <cstring>
#include <typeinfo>
#endif /* CASE_1851T_101 */

/* _1851T_131 type_info::type_info, operator= effects */
#if !defined(SKIP_1851T_131)&&(!defined(ONLY)||defined(CASE_1851T_131))
#include <typeinfo>
#endif /* CASE_1851T_131 */

/* _1852T11 bad_cast definition */
#if !defined(SKIP_1852T11)&&(!defined(ONLY)||defined(CASE_1852T11))
#include <typeinfo>
#endif /* CASE_1852T11 */

/* _1852T31 bad_cast::bad_cast() effects */
#if !defined(SKIP_1852T31)&&(!defined(ONLY)||defined(CASE_1852T31))
#include <cstring>
#include <typeinfo>
#endif /* CASE_1852T31 */

/* _1852T51 bad_cast::bad_cast(const bad_cast&), operator= effects */
#if !defined(SKIP_1852T51)&&(!defined(ONLY)||defined(CASE_1852T51))
#include <typeinfo>
#endif /* CASE_1852T51 */

/* _1852T81 bad_cast::what returns */
#if !defined(SKIP_1852T81)&&(!defined(ONLY)||defined(CASE_1852T81))
#include <cstring>
#include <typeinfo>
#endif /* CASE_1852T81 */

/* _1853T11 bad_typeid definition */
#if !defined(SKIP_1853T11)&&(!defined(ONLY)||defined(CASE_1853T11))
#include <typeinfo>
#endif /* CASE_1853T11 */

/* _1853T31 bad_typeid::bad_typeid() effects */
#if !defined(SKIP_1853T31)&&(!defined(ONLY)||defined(CASE_1853T31))
#include <typeinfo>
#endif /* CASE_1853T31 */

/* _1853T51 bad_typeid::bad_typeid(const bad_typeid&), operator= effects */
#if !defined(SKIP_1853T51)&&(!defined(ONLY)||defined(CASE_1853T51))
#include <typeinfo>
#endif /* CASE_1853T51 */

/* _1853T81 bad_typeid::what returns */
#if !defined(SKIP_1853T81)&&(!defined(ONLY)||defined(CASE_1853T81))
#include <cstring>
#include <typeinfo>
#endif /* CASE_1853T81 */

/* _186T21 <exception> synopsis */
#if !defined(SKIP_186T21)&&(!defined(ONLY)||defined(CASE_186T21))
#if DISALLOW_USER_NAME_PROTECTION
 #include <exception>
#else
	#define xmsg	"xmsg"	/* looking for trouble */
	#define xalloc	"xalloc"
 #include <exception>
	#undef xmsg
	#undef xalloc
#endif
#endif /* CASE_186T21 */

/* _18611T11 bad_exception definition */
#if !defined(SKIP_18611T11)&&(!defined(ONLY)||defined(CASE_18611T11))
#include <exception>
#endif /* CASE_18611T11 */

/* _18611T31 bad_exception::bad_exception() effects */
#if !defined(SKIP_18611T31)&&(!defined(ONLY)||defined(CASE_18611T31))
#include <exception>
#endif /* CASE_18611T31 */

/* _18611T51 bad_exception::bad_exception(const bad_exception&), operator= effects */
#if !defined(SKIP_18611T51)&&(!defined(ONLY)||defined(CASE_18611T51))
#include <exception>
#endif /* CASE_18611T51 */

/* _18611T81 bad_exception::what returns */
#if !defined(SKIP_18611T81)&&(!defined(ONLY)||defined(CASE_18611T81))
#include <cstring>
#include <exception>
#endif /* CASE_18611T81 */

/* _18612T11 unexpected_handler definition */
#if !defined(SKIP_18612T11)&&(!defined(ONLY)||defined(CASE_18612T11))
static void unh_18612T11()
	{}
#include <exception>
#endif /* CASE_18612T11 */

/* _18612T31 unexpected handler default behavior */
#if !defined(SKIP_18612T31)&&(!defined(ONLY)||defined(CASE_18612T31))
#include <csetjmp>
#include <exception>
static int cnt_18612T31;
static jmp_buf jbuf_18612T31;
static void terh_18612T31()
	{++cnt_18612T31;
	longjmp(jbuf_18612T31, 1); }
#endif /* CASE_18612T31 */

/* _18613T11 set_unexpected effects */
#if !defined(SKIP_18613T11)&&(!defined(ONLY)||defined(CASE_18613T11))
#include <csetjmp>
#include <exception>
static int cnt_18613T11;
static jmp_buf jbuf_18613T11;
static void unh_18613T11()
	{++cnt_18613T11;
	longjmp(jbuf_18613T11, 1); }
#endif /* CASE_18613T11 */

/* _18613T31 set_unexpected returns */
#if !defined(SKIP_18613T31)&&(!defined(ONLY)||defined(CASE_18613T31))
#include <exception>
static void unh_18613T31()
	{}
#endif /* CASE_18613T31 */

/* _18614T21 unexpected() effects */
#if !defined(SKIP_18614T21)&&(!defined(ONLY)||defined(CASE_18614T21))
#include <csetjmp>
#include <exception>
static int cnt_18614T21;
static jmp_buf jbuf_18614T21;
static void unh_18614T21()
	{++cnt_18614T21;
	longjmp(jbuf_18614T21, 1); }
#endif /* CASE_18614T21 */

/* _18621T11 terminate_handler definition */
#if !defined(SKIP_18621T11)&&(!defined(ONLY)||defined(CASE_18621T11))
static void terh_18621T11()
	{}
#include <exception>
#endif /* CASE_18621T11 */

/* _18622T11 set_terminate effects */
#if !defined(SKIP_18622T11)&&(!defined(ONLY)||defined(CASE_18622T11))
#include <csetjmp>
#include <exception>
static int cnt_18622T11;
static jmp_buf jbuf_18622T11;
static void terh_18622T11()
	{++cnt_18622T11;
	longjmp(jbuf_18622T11, 1); }
#endif /* CASE_18622T11 */

/* _18622T31 set_terminate returns */
#if !defined(SKIP_18622T31)&&(!defined(ONLY)||defined(CASE_18622T31))
#include <exception>
static void terh_18622T31()
	{}
#endif /* CASE_18622T31 */

/* _18623T21 terminate() effects */
#if !defined(SKIP_18623T21)&&(!defined(ONLY)||defined(CASE_18623T21))
#include <csetjmp>
#include <cstdio>
#include <exception>
static int cnt_18623T21;
static jmp_buf jbuf_18623T21;
static void terh_18623T21()
	{++cnt_18623T21;
	longjmp(jbuf_18623T21, 1); }
#endif /* CASE_18623T21 */

/* _187T21 Table 28: <cstdarg> synopsis */
#if !defined(SKIP_187T21)&&(!defined(ONLY)||defined(CASE_187T21))
#include <cstdarg>
int fun_187T21(int x, ...)
	{va_list ap;
	va_start(ap, x);
	int val = va_arg(ap, int);
	va_end(ap);
	return (val); }
#endif /* CASE_187T21 */

/* _187T31 Table 28 [sic]: <csetjmp> synopsis */
#if !defined(SKIP_187T31)&&(!defined(ONLY)||defined(CASE_187T31))
#include <csetjmp>
static jmp_buf jbuf_187T31;
static int cnt_187T31;
void fun_187T31()
	{++cnt_187T31;
	longjmp(jbuf_187T31, 1); }
#endif /* CASE_187T31 */

/* _187T41 Table 28 [sic]: <ctime> synopsis */
#if !defined(SKIP_187T41)&&(!defined(ONLY)||defined(CASE_187T41))
#include <ctime>
#endif /* CASE_187T41 */

/* _187T51 Table 28 [sic]: <csignal> synopsis */
#if !defined(SKIP_187T51)&&(!defined(ONLY)||defined(CASE_187T51))
#include <csignal>
static int cnt_187T51;
static const int sigs_187T51[] = {
	SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM};

extern "C" {
static void (*CLINK ret1_187T51)(int) = {SIG_DFL};
static void (*CLINK ret2_187T51)(int) = {SIG_ERR};
static void (*CLINK ret3_187T51)(int) = {SIG_IGN};
void CLINK fpe_187T51(int sig)
	{++cnt_187T51;
	ieq(sig, SIGFPE); }
void (*CLINK ps_187T51)(int) = 0;
}
#endif /* CASE_187T51 */

/* _187T61 Table 28 [sic]: <cstdlib> synopsis */
#if !defined(SKIP_187T61)&&(!defined(ONLY)||defined(CASE_187T61))
#include <cstdlib>
#endif /* CASE_187T61 */

int t183_main(int, char *[])
{
enter_chk("t183.cpp");
/* _18421T11 bad_alloc definition */
#if !defined(SKIP_18421T11)&&(!defined(ONLY)||defined(CASE_18421T11))
	begin_chk("_18421T11");
	{
	STD bad_alloc *p; TOUCH(p);
	chk(1);


	}
	end_chk("_18421T11");
#else
	skip_chk("_18421T11");
#endif /* CASE_18421T11 */

/* _18421T31 bad_alloc::bad_alloc() effects */
#if !defined(SKIP_18421T31)&&(!defined(ONLY)||defined(CASE_18421T31))
	begin_chk("_18421T31");
	{
	STD bad_alloc x;
	STD exception *pr = &x; TOUCH(pr);
	chk(1);


	}
	end_chk("_18421T31");
#else
	skip_chk("_18421T31");
#endif /* CASE_18421T31 */

/* _18421T51 bad_alloc::operator= effects */
#if !defined(SKIP_18421T51)&&(!defined(ONLY)||defined(CASE_18421T51))
	begin_chk("_18421T51");
	{
	STD bad_alloc x;
	STD bad_alloc y(x);
	x = y;
	chk(1);


	}
	end_chk("_18421T51");
#else
	skip_chk("_18421T51");
#endif /* CASE_18421T51 */

/* _18421T81 bad_alloc::what() returns */
#if !defined(SKIP_18421T81)&&(!defined(ONLY)||defined(CASE_18421T81))
	begin_chk("_18421T81");
	{
	STD bad_alloc x;
	const char *s = x.what();
	chk(s != 0 && 0 < strlen(s));


	}
	end_chk("_18421T81");
#else
	skip_chk("_18421T81");
#endif /* CASE_18421T81 */

/* _18422T11 new_handler definition */
#if !defined(SKIP_18422T11)&&(!defined(ONLY)||defined(CASE_18422T11))
	begin_chk("_18422T11");
	{
	STD new_handler ph = &newh_18422T11; TOUCH(ph);
	chk(1);


	}
	end_chk("_18422T11");
#else
	skip_chk("_18422T11");
#endif /* CASE_18422T11 */

/* _18422T31 new handler default behavior */
#if !defined(SKIP_18422T31)&&(!defined(ONLY)||defined(CASE_18422T31))
	begin_chk("_18422T31");
	{
	#if DISALLOW_EXCEPTIONS
	chk(1);
	#else
	struct x {
		struct x *next;
		char a[BIG_OBJECT]; };
	struct x *head = 0, **qb;
	int cnt = 0;
	try {	// consume all of heap
		for (qb = &head; (*qb = (x *)operator new(sizeof (x))) != 0; )
			qb = &(*qb)->next, *qb = 0; }
	catch (STD bad_alloc) {
		cnt = 1; }
	catch (...) {
		chk("shouldn't get here" == 0); }
	ieq(cnt, 1);
	while (head != 0)
		{struct x *p = head->next;
		operator delete(head);
		head = p; }
	#endif


	}
	end_chk("_18422T31");
#else
	skip_chk("_18422T31");
#endif /* CASE_18422T31 */

/* _18423T21 set_new_handler returns */
#if !defined(SKIP_18423T21)&&(!defined(ONLY)||defined(CASE_18423T21))
	begin_chk("_18423T21");
	{
	STD new_handler ph = STD set_new_handler(&newh_18423T21);
	chk(STD set_new_handler(ph) == &newh_18423T21);


	}
	end_chk("_18423T21");
#else
	skip_chk("_18423T21");
#endif /* CASE_18423T21 */

/* _185T31 <stdexcept> synopsis */
#if !defined(SKIP_185T31)&&(!defined(ONLY)||defined(CASE_185T31))
	begin_chk("_185T31");
	{
	STD bad_cast *pbc = 0; TOUCH(pbc); 
	STD bad_typeid *pbt = 0; TOUCH(pbt);
	STD type_info *pti = 0; TOUCH(pti);
	chk(1);




	}
	end_chk("_185T31");
#else
	skip_chk("_185T31");
#endif /* CASE_185T31 */

/* _1851T11 type_info definition */
#if !defined(SKIP_1851T11)&&(!defined(ONLY)||defined(CASE_1851T11))
	begin_chk("_1851T11");
	{
	STD type_info *p = 0; TOUCH(p);
	bool x = typeid(char) == typeid(int); TOUCH(x);
	chk(1);


	}
	end_chk("_1851T11");
#else
	skip_chk("_1851T11");
#endif /* CASE_1851T11 */

/* _1851T31 type_info::operator== returns */
#if !defined(SKIP_1851T31)&&(!defined(ONLY)||defined(CASE_1851T31))
	begin_chk("_1851T31");
	{
	chk(typeid(char) == typeid(char));
	chk(!(typeid(char) == typeid(int)));


	}
	end_chk("_1851T31");
#else
	skip_chk("_1851T31");
#endif /* CASE_1851T31 */

/* _1851T51 type_info::operator!= returns */
#if !defined(SKIP_1851T51)&&(!defined(ONLY)||defined(CASE_1851T51))
	begin_chk("_1851T51");
	{
	chk(typeid(char) != typeid(int));
	chk(!(typeid(char) != typeid(char)));


	}
	end_chk("_1851T51");
#else
	skip_chk("_1851T51");
#endif /* CASE_1851T51 */

/* _1851T81 type_info::before returns */
#if !defined(SKIP_1851T81)&&(!defined(ONLY)||defined(CASE_1851T81))
	begin_chk("_1851T81");
	{
	chk(!typeid(int).before(typeid(int)));
	if (typeid(int).before(typeid(char)))
		chk(!typeid(char).before(typeid(int)));
	if (typeid(char).before(typeid(int)))
		chk(!typeid(int).before(typeid(char)));


	}
	end_chk("_1851T81");
#else
	skip_chk("_1851T81");
#endif /* CASE_1851T81 */

/* _1851T_101 type_info::name returns */
#if !defined(SKIP_1851T_101)&&(!defined(ONLY)||defined(CASE_1851T_101))
	begin_chk("_1851T_101");
	{
	const char *nm = typeid(int).name();
	chk(nm != 0 && 0 < STD strlen(nm));


	}
	end_chk("_1851T_101");
#else
	skip_chk("_1851T_101");
#endif /* CASE_1851T_101 */

/* _1851T_131 type_info::type_info, operator= effects */
#if !defined(SKIP_1851T_131)&&(!defined(ONLY)||defined(CASE_1851T_131))
	begin_chk("_1851T_131");
	{
	// UNTESTABLE
	chk(1);


	}
	end_chk("_1851T_131");
#else
	skip_chk("_1851T_131");
#endif /* CASE_1851T_131 */

/* _1852T11 bad_cast definition */
#if !defined(SKIP_1852T11)&&(!defined(ONLY)||defined(CASE_1852T11))
	begin_chk("_1852T11");
	{
	STD bad_cast *p; TOUCH(p);
	chk(1);


	}
	end_chk("_1852T11");
#else
	skip_chk("_1852T11");
#endif /* CASE_1852T11 */

/* _1852T31 bad_cast::bad_cast() effects */
#if !defined(SKIP_1852T31)&&(!defined(ONLY)||defined(CASE_1852T31))
	begin_chk("_1852T31");
	{
	STD bad_cast x;
	STD exception *pr = &x; TOUCH(pr);
	chk(1);


	}
	end_chk("_1852T31");
#else
	skip_chk("_1852T31");
#endif /* CASE_1852T31 */

/* _1852T51 bad_cast::bad_cast(const bad_cast&), operator= effects */
#if !defined(SKIP_1852T51)&&(!defined(ONLY)||defined(CASE_1852T51))
	begin_chk("_1852T51");
	{
	STD bad_cast x, y(x);
	x = y;
	chk(1);


	}
	end_chk("_1852T51");
#else
	skip_chk("_1852T51");
#endif /* CASE_1852T51 */

/* _1852T81 bad_cast::what returns */
#if !defined(SKIP_1852T81)&&(!defined(ONLY)||defined(CASE_1852T81))
	begin_chk("_1852T81");
	{
	STD bad_cast x;
	const char *s = x.what();
	chk(s != 0);


	}
	end_chk("_1852T81");
#else
	skip_chk("_1852T81");
#endif /* CASE_1852T81 */

/* _1853T11 bad_typeid definition */
#if !defined(SKIP_1853T11)&&(!defined(ONLY)||defined(CASE_1853T11))
	begin_chk("_1853T11");
	{
	STD bad_typeid *p; TOUCH(p);
	chk(1);


	}
	end_chk("_1853T11");
#else
	skip_chk("_1853T11");
#endif /* CASE_1853T11 */

/* _1853T31 bad_typeid::bad_typeid() effects */
#if !defined(SKIP_1853T31)&&(!defined(ONLY)||defined(CASE_1853T31))
	begin_chk("_1853T31");
	{
	STD bad_typeid x;
	STD exception *pr = &x; TOUCH(pr);
	chk(1);


	}
	end_chk("_1853T31");
#else
	skip_chk("_1853T31");
#endif /* CASE_1853T31 */

/* _1853T51 bad_typeid::bad_typeid(const bad_typeid&), operator= effects */
#if !defined(SKIP_1853T51)&&(!defined(ONLY)||defined(CASE_1853T51))
	begin_chk("_1853T51");
	{
	STD bad_typeid x, y(x);
	x = y;
	chk(1);


	}
	end_chk("_1853T51");
#else
	skip_chk("_1853T51");
#endif /* CASE_1853T51 */

/* _1853T81 bad_typeid::what returns */
#if !defined(SKIP_1853T81)&&(!defined(ONLY)||defined(CASE_1853T81))
	begin_chk("_1853T81");
	{
	STD bad_typeid x;
	const char *s = x.what();
	chk(s != 0);


	}
	end_chk("_1853T81");
#else
	skip_chk("_1853T81");
#endif /* CASE_1853T81 */

/* _186T21 <exception> synopsis */
#if !defined(SKIP_186T21)&&(!defined(ONLY)||defined(CASE_186T21))
	begin_chk("_186T21");
	{
	STD exception *pbc = 0; TOUCH(pbc);		// [JUL95]
	STD terminate_handler th; TOUCH(th);
	STD unexpected_handler uh; TOUCH(uh);
	STD terminate_handler (*pst)(STD terminate_handler) = &STD set_terminate; TOUCH(pst);
	STD unexpected_handler (*pun)(STD unexpected_handler) = &STD set_unexpected; TOUCH(pun);
	void (*pt)() = &STD terminate; TOUCH(pt);
	void (*pu)() = &STD unexpected; TOUCH(pu);
	chk(1);



	}
	end_chk("_186T21");
#else
	skip_chk("_186T21");
#endif /* CASE_186T21 */

/* _18611T11 bad_exception definition */
#if !defined(SKIP_18611T11)&&(!defined(ONLY)||defined(CASE_18611T11))
	begin_chk("_18611T11");
	{
	STD bad_exception *p; TOUCH(p);
	chk(1);


	}
	end_chk("_18611T11");
#else
	skip_chk("_18611T11");
#endif /* CASE_18611T11 */

/* _18611T31 bad_exception::bad_exception() effects */
#if !defined(SKIP_18611T31)&&(!defined(ONLY)||defined(CASE_18611T31))
	begin_chk("_18611T31");
	{
	STD bad_exception x;
	STD exception *pr = &x; TOUCH(pr);
	chk(1);


	}
	end_chk("_18611T31");
#else
	skip_chk("_18611T31");
#endif /* CASE_18611T31 */

/* _18611T51 bad_exception::bad_exception(const bad_exception&), operator= effects */
#if !defined(SKIP_18611T51)&&(!defined(ONLY)||defined(CASE_18611T51))
	begin_chk("_18611T51");
	{
	STD bad_exception x, y(x);
	x = y;
	chk(1);


	}
	end_chk("_18611T51");
#else
	skip_chk("_18611T51");
#endif /* CASE_18611T51 */

/* _18611T81 bad_exception::what returns */
#if !defined(SKIP_18611T81)&&(!defined(ONLY)||defined(CASE_18611T81))
	begin_chk("_18611T81");
	{
	STD bad_exception x;
	const char *s = x.what();
	chk(s != 0);


	}
	end_chk("_18611T81");
#else
	skip_chk("_18611T81");
#endif /* CASE_18611T81 */

/* _18612T11 unexpected_handler definition */
#if !defined(SKIP_18612T11)&&(!defined(ONLY)||defined(CASE_18612T11))
	begin_chk("_18612T11");
	{
	STD unexpected_handler uh = &unh_18612T11; TOUCH(uh);
	chk(1);


	}
	end_chk("_18612T11");
#else
	skip_chk("_18612T11");
#endif /* CASE_18612T11 */

/* _18612T31 unexpected handler default behavior */
#if !defined(SKIP_18612T31)&&(!defined(ONLY)||defined(CASE_18612T31))
	begin_chk("_18612T31");
	{
	STD terminate_handler th = STD set_terminate(&terh_18612T31); TOUCH(th);
	cnt_18612T31 = 0;
	if (setjmp(jbuf_18612T31) == 0)
		STD unexpected();
	ieq(cnt_18612T31, 1);
	STD set_terminate(th);


	}
	end_chk("_18612T31");
#else
	skip_chk("_18612T31");
#endif /* CASE_18612T31 */

/* _18613T11 set_unexpected effects */
#if !defined(SKIP_18613T11)&&(!defined(ONLY)||defined(CASE_18613T11))
	begin_chk("_18613T11");
	{
	STD unexpected_handler uh = STD set_unexpected(&unh_18613T11);
	cnt_18613T11 = 0;
	if (setjmp(jbuf_18613T11) == 0)
		STD unexpected();
	ieq(cnt_18613T11, 1);
	STD set_unexpected(uh);


	}
	end_chk("_18613T11");
#else
	skip_chk("_18613T11");
#endif /* CASE_18613T11 */

/* _18613T31 set_unexpected returns */
#if !defined(SKIP_18613T31)&&(!defined(ONLY)||defined(CASE_18613T31))
	begin_chk("_18613T31");
	{
	STD unexpected_handler uh = STD set_unexpected(&unh_18613T31);
	chk(STD set_unexpected(uh) == &unh_18613T31);


	}
	end_chk("_18613T31");
#else
	skip_chk("_18613T31");
#endif /* CASE_18613T31 */

/* _18614T21 unexpected() effects */
#if !defined(SKIP_18614T21)&&(!defined(ONLY)||defined(CASE_18614T21))
	begin_chk("_18614T21");
	{
	STD unexpected_handler uh = STD set_unexpected(&unh_18614T21);
	cnt_18614T21 = 0;
	if (setjmp(jbuf_18614T21) == 0)
		STD unexpected();
	ieq(cnt_18614T21, 1);
	STD set_unexpected(unh_18614T21);


	}
	end_chk("_18614T21");
#else
	skip_chk("_18614T21");
#endif /* CASE_18614T21 */

/* _18621T11 terminate_handler definition */
#if !defined(SKIP_18621T11)&&(!defined(ONLY)||defined(CASE_18621T11))
	begin_chk("_18621T11");
	{
	STD terminate_handler th = &terh_18621T11; TOUCH(th);
	chk(1);


	}
	end_chk("_18621T11");
#else
	skip_chk("_18621T11");
#endif /* CASE_18621T11 */

/* _18622T11 set_terminate effects */
#if !defined(SKIP_18622T11)&&(!defined(ONLY)||defined(CASE_18622T11))
	begin_chk("_18622T11");
	{
	STD terminate_handler th = STD set_terminate(&terh_18622T11);
	cnt_18622T11 = 0;
	if (setjmp(jbuf_18622T11) == 0)
		STD terminate();
	ieq(cnt_18622T11, 1);
	STD set_terminate(th);


	}
	end_chk("_18622T11");
#else
	skip_chk("_18622T11");
#endif /* CASE_18622T11 */

/* _18622T31 set_terminate returns */
#if !defined(SKIP_18622T31)&&(!defined(ONLY)||defined(CASE_18622T31))
	begin_chk("_18622T31");
	{
	STD terminate_handler th = STD set_terminate(&terh_18622T31);
	chk(STD set_terminate(th) == &terh_18622T31);


	}
	end_chk("_18622T31");
#else
	skip_chk("_18622T31");
#endif /* CASE_18622T31 */

/* _18623T21 terminate() effects */
#if !defined(SKIP_18623T21)&&(!defined(ONLY)||defined(CASE_18623T21))
	begin_chk("_18623T21");
	{
	STD terminate_handler th = STD set_terminate(&terh_18623T21);
	cnt_18623T21 = 0;
	if (setjmp(jbuf_18623T21) == 0)
		STD terminate();
	ieq(cnt_18623T21, 1);
	STD set_terminate(th);


	}
	end_chk("_18623T21");
#else
	skip_chk("_18623T21");
#endif /* CASE_18623T21 */

/* _187T21 Table 28: <cstdarg> synopsis */
#if !defined(SKIP_187T21)&&(!defined(ONLY)||defined(CASE_187T21))
	begin_chk("_187T21");
	{
	ieq(fun_187T21(1, 3), 3);




	}
	end_chk("_187T21");
#else
	skip_chk("_187T21");
#endif /* CASE_187T21 */

/* _187T31 Table 28 [sic]: <csetjmp> synopsis */
#if !defined(SKIP_187T31)&&(!defined(ONLY)||defined(CASE_187T31))
	begin_chk("_187T31");
	{
	cnt_187T31 = 0;
	if (setjmp(jbuf_187T31) == 0)
		{fun_187T31();
		chk("shouldn't get here" == 0); }
	ieq(cnt_187T31, 1);


	}
	end_chk("_187T31");
#else
	skip_chk("_187T31");
#endif /* CASE_187T31 */

/* _187T41 Table 28 [sic]: <ctime> synopsis */
#if !defined(SKIP_187T41)&&(!defined(ONLY)||defined(CASE_187T41))
	begin_chk("_187T41");
	{
	clock_t ticks = clock();
	chk(0 < CLOCKS_PER_SEC);
	chk(ticks <= clock());


	}
	end_chk("_187T41");
#else
	skip_chk("_187T41");
#endif /* CASE_187T41 */

/* _187T51 Table 28 [sic]: <csignal> synopsis */
#if !defined(SKIP_187T51)&&(!defined(ONLY)||defined(CASE_187T51))
	begin_chk("_187T51");
	{
	TOUCH(sigs_187T51);
	TOUCH(ret1_187T51);
	TOUCH(ret2_187T51);
	TOUCH(ret3_187T51);
	sig_atomic_t x = 3; TOUCH(x);
	ps_187T51 = signal(SIGFPE, &fpe_187T51);
	raise(SIGFPE);
	ieq(cnt_187T51, 1);
	signal(SIGFPE, ps_187T51);


	}
	end_chk("_187T51");
#else
	skip_chk("_187T51");
#endif /* CASE_187T51 */

/* _187T61 Table 28 [sic]: <cstdlib> synopsis */
#if !defined(SKIP_187T61)&&(!defined(ONLY)||defined(CASE_187T61))
	begin_chk("_187T61");
	{
	//getenv("TZ");
	//system(0);
	chk(1);

	}
	end_chk("_187T61");
#else
	skip_chk("_187T61");
#endif /* CASE_187T61 */

return leave_chk("t183");
}
/* V3.10:0009 */
