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
//#define CASE_232T11	<bitset> synopsis
#define CASE_232T21	<deque> synopsis
#define CASE_232T31	<list> synopsis
#define CASE_232T41	<queue> synopsis
#define CASE_232T51	<stack> synopsis
#define CASE_232T61	<vector> synopsis
#else /* IS_EMBEDDED */
#endif /* IS_EMBEDDED */

/* _232T11 <bitset> synopsis */
#if !defined(SKIP_232T11)&&(!defined(ONLY)||defined(CASE_232T11))
#if DISALLOW_USER_NAME_PROTECTION
 #include <bitset>
#else
	#define toggle	"toggle"	/* looking for trouble */
	#define bits	"bits"
 #include <bitset>
	#undef bits
	#undef toggle
#endif
#include <sstream>
#endif /* CASE_232T11 */

/* _232T21 <deque> synopsis */
#if !defined(SKIP_232T21)&&(!defined(ONLY)||defined(CASE_232T21))
#include <deque>
#endif /* CASE_232T21 */

/* _232T31 <list> synopsis */
#if !defined(SKIP_232T31)&&(!defined(ONLY)||defined(CASE_232T31))
#include <list>
#endif /* CASE_232T31 */

/* _232T41 <queue> synopsis */
#if !defined(SKIP_232T41)&&(!defined(ONLY)||defined(CASE_232T41))
#include <functional>
#include <list>
#include <queue>
#include <vector>
#endif /* CASE_232T41 */

/* _232T51 <stack> synopsis */
#if !defined(SKIP_232T51)&&(!defined(ONLY)||defined(CASE_232T51))
#include <stack>
#include <vector>
#endif /* CASE_232T51 */

/* _232T61 <vector> synopsis */
#if !defined(SKIP_232T61)&&(!defined(ONLY)||defined(CASE_232T61))
#include <vector>
#endif /* CASE_232T61 */

int t230_main(int, char *[])
{
enter_chk("t230.cpp");
/* _232T11 <bitset> synopsis */
#if !defined(SKIP_232T11)&&(!defined(ONLY)||defined(CASE_232T11))
	begin_chk("_232T11");
	{
	typedef bitset<23> Cont;
	Cont v0, v1;
	v0 = v0 & v1;
	v0 = v0 | v1;
	v0 = v0 ^ v1;
	istringstream is;
	is >> v0;
	ostringstream os;
	os << v0; 
	chk(1);


	}
	end_chk("_232T11");
#else
	skip_chk("_232T11");
#endif /* CASE_232T11 */

/* _232T21 <deque> synopsis */
#if !defined(SKIP_232T21)&&(!defined(ONLY)||defined(CASE_232T21))
	begin_chk("_232T21");
	{
	typedef deque<Char, ALLOCATOR(Char)> Cont;
	Cont v0;
	chk(v0 == v0);
	chk(!(v0 < v0));


	}
	end_chk("_232T21");
#else
	skip_chk("_232T21");
#endif /* CASE_232T21 */

/* _232T31 <list> synopsis */
#if !defined(SKIP_232T31)&&(!defined(ONLY)||defined(CASE_232T31))
	begin_chk("_232T31");
	{
	typedef list<Char, ALLOCATOR(Char)> Cont;
	Cont v0;
	chk(v0 == v0);
	chk(!(v0 < v0));


	}
	end_chk("_232T31");
#else
	skip_chk("_232T31");
#endif /* CASE_232T31 */

/* _232T41 <queue> synopsis */
#if !defined(SKIP_232T41)&&(!defined(ONLY)||defined(CASE_232T41))
	begin_chk("_232T41");
	{
	typedef list<Char, ALLOCATOR(Char)> Cont1;
	typedef queue<Char, Cont1> Adap1;	// CHANGED [JUL96]
	Adap1 v0;
	chk(v0 == v0);
	chk(!(v0 < v0));
	typedef vector<Char, ALLOCATOR(Char)> Cont2;
	typedef priority_queue<Char, Cont2,
		less<Cont2::value_type> > Adap2;	// CHANGED [JUL96]
	Adap2 vp0;


	}
	end_chk("_232T41");
#else
	skip_chk("_232T41");
#endif /* CASE_232T41 */

/* _232T51 <stack> synopsis */
#if !defined(SKIP_232T51)&&(!defined(ONLY)||defined(CASE_232T51))
	begin_chk("_232T51");
	{
	typedef vector<Char, ALLOCATOR(Char)> Cont;
	typedef stack<Char, Cont> Adap;	// CHANGED [JUL96]
	Adap v0;
	chk(v0 == v0);
	chk(!(v0 < v0));


	}
	end_chk("_232T51");
#else
	skip_chk("_232T51");
#endif /* CASE_232T51 */

/* _232T61 <vector> synopsis */
#if !defined(SKIP_232T61)&&(!defined(ONLY)||defined(CASE_232T61))
	begin_chk("_232T61");
	{
	typedef vector<Char, ALLOCATOR(Char)> Cont;
	Cont v0;
	chk(v0 == v0);
	chk(!(v0 < v0));
	typedef vector<bool, ALLOCATOR(bool)> Bvec;
	Bvec v1;
	chk(v1 == v1);
	chk(!(v1 < v1));

	}
	end_chk("_232T61");
#else
	skip_chk("_232T61");
#endif /* CASE_232T61 */

return leave_chk("t230");
}
/* V3.10:0009 */
