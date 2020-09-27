/*  Dinkum(R) C++ Proofer(TM)
 *  Copyright (C) 1995-2001 by P.J. Plauger. All rights reserved.
 *  This program is the property of P.J. Plauger. Its contents are
 *  proprietary information. No part of it is to be disclosed to anyone
 *  except employees of Dinkumware, Ltd., or as agreed in writing with
 *  Dinkumware, Ltd.
 */
#include "defs.h"


/* _17412T21 Table 21: C++ library headers */
#if !defined(SKIP_17412T21)&&(!defined(ONLY)||defined(CASE_17412T21))
#if !DISALLOW_USER_NAME_PROTECTION
	#define smanip	"smanip"	/* looking for trouble */
	#define T	"T"
	#define x	"x"
#endif
#if IS_EMBEDDED
#else /* IS_EMBEDDED */
#include <algorithm>
#include <bitset>
#include <deque>
#include <exception>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>
#endif /* IS_EMBEDDED */
#include <complex>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <new>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>
#if !DISALLOW_USER_NAME_PROTECTION
	#undef smanip
	#undef T
	#undef x
#endif
#endif /* CASE_17412T21 */

/* _17412T31 Table 22: C++ headers for C library */
#if !defined(SKIP_17412T31)&&(!defined(ONLY)||defined(CASE_17412T31))
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#if IS_EMBEDDED
#else /* IS_EMBEDDED */
#include <ciso646>
#include <cwchar>
#include <cwctype>
#endif /* IS_EMBEDDED */
#endif /* CASE_17412T31 */

/* _17412T41 contents of cname same as name.h */
#if !defined(SKIP_17412T41)&&(!defined(ONLY)||defined(CASE_17412T41))
#include <cstdio>
static int fun1_17412T41() {return (EOF); }
#include <stdio.h>
static int fun2_17412T41() {return (EOF); }
#endif /* CASE_17412T41 */

/* _17412T42 declarations are in namespace std */
#if !defined(SKIP_17412T42)&&(!defined(ONLY)||defined(CASE_17412T42))
#include <cstdio>
static int fun1_17412T42() {return (STD ferror(stdin)); }
#include <stdio.h>
static int fun2_17412T42() {return (ferror(stdin)); }
#endif /* CASE_17412T42 */

/* _17321T21 may include headers in any order */
#if !defined(SKIP_17321T21)&&(!defined(ONLY)||defined(CASE_17321T21))
#include <iostream>
#include <cstddef>
#include <istream>
#endif /* CASE_17321T21 */

/* _17321T22 may be included more than once (except <assert.h>) */
#if !defined(SKIP_17321T22)&&(!defined(ONLY)||defined(CASE_17321T22))
#include <cstddef>
#include <cstddef>
#include <iostream>
#include <iostream>
#undef NDEBUG
#include <cassert>
static int fun1_17321T22() {assert(1); return (1); }
#define NDEBUG
#include <cassert>
static int fun2_17321T22() {assert(0); return (1); }
#undef NDEBUG
#include <cassert>
static int fun3_17321T22() {assert(1); return (1); }
#endif /* CASE_17321T22 */

/* _17335T21 may install different handler functions during execution */
#if !defined(SKIP_17335T21)&&(!defined(ONLY)||defined(CASE_17335T21))
#include <cstdlib>
#include <new>
#if DISALLOW_EXCEPTIONS
#else /* DISALLOW_EXCEPTIONS */
#include <exception>
static void unexh_17335T21() {abort(); }
static void termh_17335T21() {abort(); }
#endif /* DISALLOW_EXCEPTIONS */
static void newh_17335T21() {abort(); }
#endif /* CASE_17335T21 */

/* _17341T12 second header may also define certain types and macros */
#if !defined(SKIP_17341T12)&&(!defined(ONLY)||defined(CASE_17341T12))
#include <cstddef>
static size_t s1_17341T12 = 0;
#include <stddef.h>
static size_t s2_17341T12 = 0;
#include <streambuf>
static int eof1 = EOF;
#include <stdio.h>
static int eof2 = EOF;
#endif /* CASE_17341T12 */

/* _17341T22 any C++ header may include any other */
#if !defined(SKIP_17341T22)&&(!defined(ONLY)||defined(CASE_17341T22))
#include <istream>
#endif /* CASE_17341T22 */

/* _17342T21 object-like macros are #if expressions */
#if !defined(SKIP_17342T21)&&(!defined(ONLY)||defined(CASE_17342T21))
#include <cfloat>
#if FLT_RADIX == 0
 #error FLT_RADIX not usable in #if
#endif
#include <climits>
#if CHAR_MAX == 0
 #error CHAR_MAX not usable in #if
#endif
#include <clocale>
#if LC_ALL == 0 && !defined(LC_ALL)
 #error LC_ALL not usable in #if
#endif
#include <csignal>
#if SIGILL == 0
 #error SIGILL not usable in #if
#endif
#include <cstdlib>
#if EXIT_FAILURE == 0
 #error EXIT_FAILURE not usable in #if
#endif
#include <streambuf>
#if EOF == 0
 #error EOF not usable in #if
#endif
#endif /* CASE_17342T21 */

/* _17347T21 distinct type names are different types */
#if !defined(SKIP_17347T21)&&(!defined(ONLY)||defined(CASE_17347T21))
#include <stdio.h>
#include <iostream>
int fun_17347T21(int x) {return (x); }
int fun_17347T21(FILE) {return (0); }
int fun_17347T21(STD ios) {return (0); }
int fun_17347T21(STD streampos) {return (0); }
#endif /* CASE_17347T21 */

int t170_main(int, char *[])
{
enter_chk("t170.cpp");
/* _17412T21 Table 21: C++ library headers */
#if !defined(SKIP_17412T21)&&(!defined(ONLY)||defined(CASE_17412T21))
	begin_chk("_17412T21");
	{
	chk(1);


	}
	end_chk("_17412T21");
#else
	skip_chk("_17412T21");
#endif /* CASE_17412T21 */

/* _17412T31 Table 22: C++ headers for C library */
#if !defined(SKIP_17412T31)&&(!defined(ONLY)||defined(CASE_17412T31))
	begin_chk("_17412T31");
	{
	chk(1);


	}
	end_chk("_17412T31");
#else
	skip_chk("_17412T31");
#endif /* CASE_17412T31 */

/* _17412T41 contents of cname same as name.h */
#if !defined(SKIP_17412T41)&&(!defined(ONLY)||defined(CASE_17412T41))
	begin_chk("_17412T41");
	{
	ieq(fun1_17412T41(), fun2_17412T41());


	}
	end_chk("_17412T41");
#else
	skip_chk("_17412T41");
#endif /* CASE_17412T41 */

/* _17412T42 declarations are in namespace std */
#if !defined(SKIP_17412T42)&&(!defined(ONLY)||defined(CASE_17412T42))
	begin_chk("_17412T42");
	{
	ieq(fun1_17412T42(), fun2_17412T42());




	}
	end_chk("_17412T42");
#else
	skip_chk("_17412T42");
#endif /* CASE_17412T42 */

/* _17321T21 may include headers in any order */
#if !defined(SKIP_17321T21)&&(!defined(ONLY)||defined(CASE_17321T21))
	begin_chk("_17321T21");
	{
	chk(1);


	}
	end_chk("_17321T21");
#else
	skip_chk("_17321T21");
#endif /* CASE_17321T21 */

/* _17321T22 may be included more than once (except <assert.h>) */
#if !defined(SKIP_17321T22)&&(!defined(ONLY)||defined(CASE_17321T22))
	begin_chk("_17321T22");
	{
	ieq(fun1_17321T22(), 1);
	ieq(fun2_17321T22(), 1);
	ieq(fun3_17321T22(), 1);




	}
	end_chk("_17321T22");
#else
	skip_chk("_17321T22");
#endif /* CASE_17321T22 */

/* _17335T21 may install different handler functions during execution */
#if !defined(SKIP_17335T21)&&(!defined(ONLY)||defined(CASE_17335T21))
	begin_chk("_17335T21");
	{
	STD new_handler newp = STD set_new_handler(&newh_17335T21);
	newp = STD set_new_handler(newp);
	chk(newp == &newh_17335T21);
	#if DISALLOW_EXCEPTIONS
	#else /* DISALLOW_EXCEPTIONS */
	STD unexpected_handler unexp = STD set_unexpected(&unexh_17335T21);
	STD terminate_handler termp = STD set_terminate(&termh_17335T21);
	unexp = STD set_unexpected(unexp);
	termp = STD set_terminate(termp);
	chk(unexp == &unexh_17335T21);
	chk(termp == &termh_17335T21);
	#endif /* DISALLOW_EXCEPTIONS */


	}
	end_chk("_17335T21");
#else
	skip_chk("_17335T21");
#endif /* CASE_17335T21 */

/* _17341T12 second header may also define certain types and macros */
#if !defined(SKIP_17341T12)&&(!defined(ONLY)||defined(CASE_17341T12))
	begin_chk("_17341T12");
	{
	TOUCH(s1_17341T12);
	TOUCH(s2_17341T12);
	ieq(eof1, EOF);
	ieq(eof1, eof2);


	}
	end_chk("_17341T12");
#else
	skip_chk("_17341T12");
#endif /* CASE_17341T12 */

/* _17341T22 any C++ header may include any other */
#if !defined(SKIP_17341T22)&&(!defined(ONLY)||defined(CASE_17341T22))
	begin_chk("_17341T22");
	{
	STD ios::fmtflags ff = STD ios::hex; TOUCH(ff);
	chk(1);




	}
	end_chk("_17341T22");
#else
	skip_chk("_17341T22");
#endif /* CASE_17341T22 */

/* _17342T21 object-like macros are #if expressions */
#if !defined(SKIP_17342T21)&&(!defined(ONLY)||defined(CASE_17342T21))
	begin_chk("_17342T21");
	{
	chk(1);


	}
	end_chk("_17342T21");
#else
	skip_chk("_17342T21");
#endif /* CASE_17342T21 */

/* _17347T21 distinct type names are different types */
#if !defined(SKIP_17347T21)&&(!defined(ONLY)||defined(CASE_17347T21))
	begin_chk("_17347T21");
	{
	ieq(fun_17347T21(3), 3);

	}
	end_chk("_17347T21");
#else
	skip_chk("_17347T21");
#endif /* CASE_17347T21 */

return leave_chk("t170");
}
/* V3.10:0009 */
