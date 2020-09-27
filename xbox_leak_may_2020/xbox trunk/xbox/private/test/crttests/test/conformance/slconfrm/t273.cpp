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
#define CASE_27451T21	boolalpha effects
#define CASE_27451T31	boolalpha returns
#define CASE_27451T41	noboolalpha effects
#define CASE_27451T51	noboolalpha returns
#define CASE_27451T71	showbase effects
#define CASE_27451T71a	unitbuf effects ADDED [MAR96]
#define CASE_27451T81	showbase returns
#define CASE_27451T81a	unitbuf returns	ADDED [MAR96]
#define CASE_27451T_111	noshowbase effects
#define CASE_27451T_111a	nounitbuf effects	ADDED [MAR96]
#define CASE_27451T_121	noshowbase returns
#define CASE_27451T_121a	nounitbuf returns	ADDED [MAR96]
#define CASE_27451T_141	showpoint effects
#define CASE_27451T_151	showpoint returns
#define CASE_27451T_171	noshowpoint effects
#define CASE_27451T_181	noshowpoint returns
#define CASE_27451T_191	showpos effects
#define CASE_27451T_201	showpos returns
#define CASE_27451T_221	noshowpos effects
#define CASE_27451T_231	noshowpos returns
#define CASE_27451T_251	skipws effects
#define CASE_27451T_261	skipws returns
#define CASE_27451T_281	noskipws effects
#define CASE_27451T_291	noskipws returns
#define CASE_27451T_301	uppercase effects
#define CASE_27451T_311	uppercase returns
#define CASE_27451T_331	nouppercase effects
#define CASE_27451T_341	nouppercase returns
#define CASE_27452T21	internal effects
#define CASE_27452T31	internal returns
#define CASE_27452T41	left effects
#define CASE_27452T51	left returns
#define CASE_27452T61	right effects
#define CASE_27452T71	right returns
#define CASE_27453T21	dec effects
#define CASE_27453T31	dec returns
#define CASE_27453T51	hex effects
#define CASE_27453T61	hex returns
#define CASE_27453T81	oct effects
#define CASE_27453T91	oct returns
#define CASE_27454T21	fixed effects
#define CASE_27454T31	fixed returns
#define CASE_27454T51	scientific effects
#define CASE_27454T61	scientific returns
#define CASE_275T11	<streambuf> synopsis
#define CASE_2752T11	basic_streambuf definition
//#define CASE_2752T21	streambuf definition
//#define CASE_2752T31	wstreambuf definition
#endif /* IS_EMBEDDED */

/* _27451T21 boolalpha effects */
#if !defined(SKIP_27451T21)&&(!defined(ONLY)||defined(CASE_27451T21))
#include <ios>
#endif /* CASE_27451T21 */

/* _27451T31 boolalpha returns */
#if !defined(SKIP_27451T31)&&(!defined(ONLY)||defined(CASE_27451T31))
#include <ios>
#endif /* CASE_27451T31 */

/* _27451T41 noboolalpha effects */
#if !defined(SKIP_27451T41)&&(!defined(ONLY)||defined(CASE_27451T41))
#include <ios>
#endif /* CASE_27451T41 */

/* _27451T51 noboolalpha returns */
#if !defined(SKIP_27451T51)&&(!defined(ONLY)||defined(CASE_27451T51))
#include <ios>
#endif /* CASE_27451T51 */

/* _27451T71 showbase effects */
#if !defined(SKIP_27451T71)&&(!defined(ONLY)||defined(CASE_27451T71))
#include <ios>
#endif /* CASE_27451T71 */

/* _27451T71a unitbuf effects ADDED [MAR96] */
#if !defined(SKIP_27451T71a)&&(!defined(ONLY)||defined(CASE_27451T71a))
#include <ios>
#endif /* CASE_27451T71a */

/* _27451T81 showbase returns */
#if !defined(SKIP_27451T81)&&(!defined(ONLY)||defined(CASE_27451T81))
#include <ios>
#endif /* CASE_27451T81 */

/* _27451T81a unitbuf returns	ADDED [MAR96] */
#if !defined(SKIP_27451T81a)&&(!defined(ONLY)||defined(CASE_27451T81a))
#include <ios>
#endif /* CASE_27451T81a */

/* _27451T_111 noshowbase effects */
#if !defined(SKIP_27451T_111)&&(!defined(ONLY)||defined(CASE_27451T_111))
#include <ios>
#endif /* CASE_27451T_111 */

/* _27451T_111a nounitbuf effects	ADDED [MAR96] */
#if !defined(SKIP_27451T_111a)&&(!defined(ONLY)||defined(CASE_27451T_111a))
#include <ios>
#endif /* CASE_27451T_111a */

/* _27451T_121 noshowbase returns */
#if !defined(SKIP_27451T_121)&&(!defined(ONLY)||defined(CASE_27451T_121))
#include <ios>
#endif /* CASE_27451T_121 */

/* _27451T_121a nounitbuf returns	ADDED [MAR96] */
#if !defined(SKIP_27451T_121a)&&(!defined(ONLY)||defined(CASE_27451T_121a))
#include <ios>
#endif /* CASE_27451T_121a */

/* _27451T_141 showpoint effects */
#if !defined(SKIP_27451T_141)&&(!defined(ONLY)||defined(CASE_27451T_141))
#include <ios>
#endif /* CASE_27451T_141 */

/* _27451T_151 showpoint returns */
#if !defined(SKIP_27451T_151)&&(!defined(ONLY)||defined(CASE_27451T_151))
#include <ios>
#endif /* CASE_27451T_151 */

/* _27451T_171 noshowpoint effects */
#if !defined(SKIP_27451T_171)&&(!defined(ONLY)||defined(CASE_27451T_171))
#include <ios>
#endif /* CASE_27451T_171 */

/* _27451T_181 noshowpoint returns */
#if !defined(SKIP_27451T_181)&&(!defined(ONLY)||defined(CASE_27451T_181))
#include <ios>
#endif /* CASE_27451T_181 */

/* _27451T_191 showpos effects */
#if !defined(SKIP_27451T_191)&&(!defined(ONLY)||defined(CASE_27451T_191))
#include <ios>
#endif /* CASE_27451T_191 */

/* _27451T_201 showpos returns */
#if !defined(SKIP_27451T_201)&&(!defined(ONLY)||defined(CASE_27451T_201))
#include <ios>
#endif /* CASE_27451T_201 */

/* _27451T_221 noshowpos effects */
#if !defined(SKIP_27451T_221)&&(!defined(ONLY)||defined(CASE_27451T_221))
#include <ios>
#endif /* CASE_27451T_221 */

/* _27451T_231 noshowpos returns */
#if !defined(SKIP_27451T_231)&&(!defined(ONLY)||defined(CASE_27451T_231))
#include <ios>
#endif /* CASE_27451T_231 */

/* _27451T_251 skipws effects */
#if !defined(SKIP_27451T_251)&&(!defined(ONLY)||defined(CASE_27451T_251))
#include <ios>
#endif /* CASE_27451T_251 */

/* _27451T_261 skipws returns */
#if !defined(SKIP_27451T_261)&&(!defined(ONLY)||defined(CASE_27451T_261))
#include <ios>
#endif /* CASE_27451T_261 */

/* _27451T_281 noskipws effects */
#if !defined(SKIP_27451T_281)&&(!defined(ONLY)||defined(CASE_27451T_281))
#include <ios>
#endif /* CASE_27451T_281 */

/* _27451T_291 noskipws returns */
#if !defined(SKIP_27451T_291)&&(!defined(ONLY)||defined(CASE_27451T_291))
#include <ios>
#endif /* CASE_27451T_291 */

/* _27451T_301 uppercase effects */
#if !defined(SKIP_27451T_301)&&(!defined(ONLY)||defined(CASE_27451T_301))
#include <ios>
#endif /* CASE_27451T_301 */

/* _27451T_311 uppercase returns */
#if !defined(SKIP_27451T_311)&&(!defined(ONLY)||defined(CASE_27451T_311))
#include <ios>
#endif /* CASE_27451T_311 */

/* _27451T_331 nouppercase effects */
#if !defined(SKIP_27451T_331)&&(!defined(ONLY)||defined(CASE_27451T_331))
#include <ios>
#endif /* CASE_27451T_331 */

/* _27451T_341 nouppercase returns */
#if !defined(SKIP_27451T_341)&&(!defined(ONLY)||defined(CASE_27451T_341))
#include <ios>
#endif /* CASE_27451T_341 */

/* _27452T21 internal effects */
#if !defined(SKIP_27452T21)&&(!defined(ONLY)||defined(CASE_27452T21))
#include <ios>
#endif /* CASE_27452T21 */

/* _27452T31 internal returns */
#if !defined(SKIP_27452T31)&&(!defined(ONLY)||defined(CASE_27452T31))
#include <ios>
#endif /* CASE_27452T31 */

/* _27452T41 left effects */
#if !defined(SKIP_27452T41)&&(!defined(ONLY)||defined(CASE_27452T41))
#include <ios>
#endif /* CASE_27452T41 */

/* _27452T51 left returns */
#if !defined(SKIP_27452T51)&&(!defined(ONLY)||defined(CASE_27452T51))
#include <ios>
#endif /* CASE_27452T51 */

/* _27452T61 right effects */
#if !defined(SKIP_27452T61)&&(!defined(ONLY)||defined(CASE_27452T61))
#include <ios>
#endif /* CASE_27452T61 */

/* _27452T71 right returns */
#if !defined(SKIP_27452T71)&&(!defined(ONLY)||defined(CASE_27452T71))
#include <ios>
#endif /* CASE_27452T71 */

/* _27453T21 dec effects */
#if !defined(SKIP_27453T21)&&(!defined(ONLY)||defined(CASE_27453T21))
#include <ios>
#endif /* CASE_27453T21 */

/* _27453T31 dec returns */
#if !defined(SKIP_27453T31)&&(!defined(ONLY)||defined(CASE_27453T31))
#include <ios>
#endif /* CASE_27453T31 */

/* _27453T51 hex effects */
#if !defined(SKIP_27453T51)&&(!defined(ONLY)||defined(CASE_27453T51))
#include <ios>
#endif /* CASE_27453T51 */

/* _27453T61 hex returns */
#if !defined(SKIP_27453T61)&&(!defined(ONLY)||defined(CASE_27453T61))
#include <ios>
#endif /* CASE_27453T61 */

/* _27453T81 oct effects */
#if !defined(SKIP_27453T81)&&(!defined(ONLY)||defined(CASE_27453T81))
#include <ios>
#endif /* CASE_27453T81 */

/* _27453T91 oct returns */
#if !defined(SKIP_27453T91)&&(!defined(ONLY)||defined(CASE_27453T91))
#include <ios>
#endif /* CASE_27453T91 */

/* _27454T21 fixed effects */
#if !defined(SKIP_27454T21)&&(!defined(ONLY)||defined(CASE_27454T21))
#include <ios>
#endif /* CASE_27454T21 */

/* _27454T31 fixed returns */
#if !defined(SKIP_27454T31)&&(!defined(ONLY)||defined(CASE_27454T31))
#include <ios>
#endif /* CASE_27454T31 */

/* _27454T51 scientific effects */
#if !defined(SKIP_27454T51)&&(!defined(ONLY)||defined(CASE_27454T51))
#include <ios>
#endif /* CASE_27454T51 */

/* _27454T61 scientific returns */
#if !defined(SKIP_27454T61)&&(!defined(ONLY)||defined(CASE_27454T61))
#include <ios>
#endif /* CASE_27454T61 */

/* _275T11 <streambuf> synopsis */
#if !defined(SKIP_275T11)&&(!defined(ONLY)||defined(CASE_275T11))
#include <streambuf>
#endif /* CASE_275T11 */

/* _2752T11 basic_streambuf definition */
#if !defined(SKIP_2752T11)&&(!defined(ONLY)||defined(CASE_2752T11))
#include <cstring>
#include <sstream>
#include <streambuf>
#endif /* CASE_2752T11 */

/* _2752T21 streambuf definition */
#if !defined(SKIP_2752T21)&&(!defined(ONLY)||defined(CASE_2752T21))
#include <streambuf>
#endif /* CASE_2752T21 */

/* _2752T31 wstreambuf definition */
#if !defined(SKIP_2752T31)&&(!defined(ONLY)||defined(CASE_2752T31))
#include <streambuf>
#endif /* CASE_2752T31 */

int t273_main(int, char *[])
{
enter_chk("t273.cpp");
/* _27451T21 boolalpha effects */
#if !defined(SKIP_27451T21)&&(!defined(ONLY)||defined(CASE_27451T21))
	begin_chk("_27451T21");
	{
	Bi x(0);
	x.flags(Bi::scientific);
	boolalpha(x);
	chk(x.flags() == (Bi::boolalpha | Bi::scientific));


	}
	end_chk("_27451T21");
#else
	skip_chk("_27451T21");
#endif /* CASE_27451T21 */

/* _27451T31 boolalpha returns */
#if !defined(SKIP_27451T31)&&(!defined(ONLY)||defined(CASE_27451T31))
	begin_chk("_27451T31");
	{
	Bi x(0);
	chk(&boolalpha(x) == &x);


	}
	end_chk("_27451T31");
#else
	skip_chk("_27451T31");
#endif /* CASE_27451T31 */

/* _27451T41 noboolalpha effects */
#if !defined(SKIP_27451T41)&&(!defined(ONLY)||defined(CASE_27451T41))
	begin_chk("_27451T41");
	{
	Bi x(0);
	x.flags(Bi::boolalpha | Bi::scientific);
	noboolalpha(x);
	chk(x.flags() == Bi::scientific);


	}
	end_chk("_27451T41");
#else
	skip_chk("_27451T41");
#endif /* CASE_27451T41 */

/* _27451T51 noboolalpha returns */
#if !defined(SKIP_27451T51)&&(!defined(ONLY)||defined(CASE_27451T51))
	begin_chk("_27451T51");
	{
	Bi x(0);
	chk(&noboolalpha(x) == &x);


	}
	end_chk("_27451T51");
#else
	skip_chk("_27451T51");
#endif /* CASE_27451T51 */

/* _27451T71 showbase effects */
#if !defined(SKIP_27451T71)&&(!defined(ONLY)||defined(CASE_27451T71))
	begin_chk("_27451T71");
	{
	Bi x(0);
	x.flags(Bi::scientific);
	showbase(x);
	chk(x.flags() == (Bi::showbase | Bi::scientific));

	}
	end_chk("_27451T71");
#else
	skip_chk("_27451T71");
#endif /* CASE_27451T71 */

/* _27451T71a unitbuf effects ADDED [MAR96] */
#if !defined(SKIP_27451T71a)&&(!defined(ONLY)||defined(CASE_27451T71a))
	begin_chk("_27451T71a");
	{
	Bi x(0);
	x.flags(Bi::scientific);
	unitbuf(x);
	chk(x.flags() == (Bi::unitbuf | Bi::scientific));


	}
	end_chk("_27451T71a");
#else
	skip_chk("_27451T71a");
#endif /* CASE_27451T71a */

/* _27451T81 showbase returns */
#if !defined(SKIP_27451T81)&&(!defined(ONLY)||defined(CASE_27451T81))
	begin_chk("_27451T81");
	{
	Bi x(0);
	chk(&showbase(x) == &x);

	}
	end_chk("_27451T81");
#else
	skip_chk("_27451T81");
#endif /* CASE_27451T81 */

/* _27451T81a unitbuf returns	ADDED [MAR96] */
#if !defined(SKIP_27451T81a)&&(!defined(ONLY)||defined(CASE_27451T81a))
	begin_chk("_27451T81a");
	{
	Bi x(0);
	chk(&unitbuf(x) == &x);


	}
	end_chk("_27451T81a");
#else
	skip_chk("_27451T81a");
#endif /* CASE_27451T81a */

/* _27451T_111 noshowbase effects */
#if !defined(SKIP_27451T_111)&&(!defined(ONLY)||defined(CASE_27451T_111))
	begin_chk("_27451T_111");
	{
	Bi x(0);
	x.flags(Bi::showbase | Bi::scientific);
	noshowbase(x);
	chk(x.flags() == Bi::scientific);

	}
	end_chk("_27451T_111");
#else
	skip_chk("_27451T_111");
#endif /* CASE_27451T_111 */

/* _27451T_111a nounitbuf effects	ADDED [MAR96] */
#if !defined(SKIP_27451T_111a)&&(!defined(ONLY)||defined(CASE_27451T_111a))
	begin_chk("_27451T_111a");
	{
	Bi x(0);
	x.flags(Bi::unitbuf | Bi::scientific);
	nounitbuf(x);
	chk(x.flags() == Bi::scientific);


	}
	end_chk("_27451T_111a");
#else
	skip_chk("_27451T_111a");
#endif /* CASE_27451T_111a */

/* _27451T_121 noshowbase returns */
#if !defined(SKIP_27451T_121)&&(!defined(ONLY)||defined(CASE_27451T_121))
	begin_chk("_27451T_121");
	{
	Bi x(0);
	chk(&noshowbase(x) == &x);

	}
	end_chk("_27451T_121");
#else
	skip_chk("_27451T_121");
#endif /* CASE_27451T_121 */

/* _27451T_121a nounitbuf returns	ADDED [MAR96] */
#if !defined(SKIP_27451T_121a)&&(!defined(ONLY)||defined(CASE_27451T_121a))
	begin_chk("_27451T_121a");
	{
	Bi x(0);
	chk(&nounitbuf(x) == &x);


	}
	end_chk("_27451T_121a");
#else
	skip_chk("_27451T_121a");
#endif /* CASE_27451T_121a */

/* _27451T_141 showpoint effects */
#if !defined(SKIP_27451T_141)&&(!defined(ONLY)||defined(CASE_27451T_141))
	begin_chk("_27451T_141");
	{
	Bi x(0);
	x.flags(Bi::scientific);
	showpoint(x);
	chk(x.flags() == (Bi::showpoint | Bi::scientific));


	}
	end_chk("_27451T_141");
#else
	skip_chk("_27451T_141");
#endif /* CASE_27451T_141 */

/* _27451T_151 showpoint returns */
#if !defined(SKIP_27451T_151)&&(!defined(ONLY)||defined(CASE_27451T_151))
	begin_chk("_27451T_151");
	{
	Bi x(0);
	chk(&showpoint(x) == &x);


	}
	end_chk("_27451T_151");
#else
	skip_chk("_27451T_151");
#endif /* CASE_27451T_151 */

/* _27451T_171 noshowpoint effects */
#if !defined(SKIP_27451T_171)&&(!defined(ONLY)||defined(CASE_27451T_171))
	begin_chk("_27451T_171");
	{
	Bi x(0);
	x.flags(Bi::showpoint | Bi::scientific);
	noshowpoint(x);
	chk(x.flags() == Bi::scientific);


	}
	end_chk("_27451T_171");
#else
	skip_chk("_27451T_171");
#endif /* CASE_27451T_171 */

/* _27451T_181 noshowpoint returns */
#if !defined(SKIP_27451T_181)&&(!defined(ONLY)||defined(CASE_27451T_181))
	begin_chk("_27451T_181");
	{
	Bi x(0);
	chk(&noshowpoint(x) == &x);


	}
	end_chk("_27451T_181");
#else
	skip_chk("_27451T_181");
#endif /* CASE_27451T_181 */

/* _27451T_191 showpos effects */
#if !defined(SKIP_27451T_191)&&(!defined(ONLY)||defined(CASE_27451T_191))
	begin_chk("_27451T_191");
	{
	Bi x(0);
	x.flags(Bi::scientific);
	showpos(x);
	chk(x.flags() == (Bi::showpos | Bi::scientific));


	}
	end_chk("_27451T_191");
#else
	skip_chk("_27451T_191");
#endif /* CASE_27451T_191 */

/* _27451T_201 showpos returns */
#if !defined(SKIP_27451T_201)&&(!defined(ONLY)||defined(CASE_27451T_201))
	begin_chk("_27451T_201");
	{
	Bi x(0);
	chk(&showpos(x) == &x);


	}
	end_chk("_27451T_201");
#else
	skip_chk("_27451T_201");
#endif /* CASE_27451T_201 */

/* _27451T_221 noshowpos effects */
#if !defined(SKIP_27451T_221)&&(!defined(ONLY)||defined(CASE_27451T_221))
	begin_chk("_27451T_221");
	{
	Bi x(0);
	x.flags(Bi::showpos | Bi::scientific);
	noshowpos(x);
	chk(x.flags() == Bi::scientific);


	}
	end_chk("_27451T_221");
#else
	skip_chk("_27451T_221");
#endif /* CASE_27451T_221 */

/* _27451T_231 noshowpos returns */
#if !defined(SKIP_27451T_231)&&(!defined(ONLY)||defined(CASE_27451T_231))
	begin_chk("_27451T_231");
	{
	Bi x(0);
	chk(&noshowpos(x) == &x);


	}
	end_chk("_27451T_231");
#else
	skip_chk("_27451T_231");
#endif /* CASE_27451T_231 */

/* _27451T_251 skipws effects */
#if !defined(SKIP_27451T_251)&&(!defined(ONLY)||defined(CASE_27451T_251))
	begin_chk("_27451T_251");
	{
	Bi x(0);
	x.flags(Bi::scientific);
	skipws(x);
	chk(x.flags() == (Bi::skipws | Bi::scientific));


	}
	end_chk("_27451T_251");
#else
	skip_chk("_27451T_251");
#endif /* CASE_27451T_251 */

/* _27451T_261 skipws returns */
#if !defined(SKIP_27451T_261)&&(!defined(ONLY)||defined(CASE_27451T_261))
	begin_chk("_27451T_261");
	{
	Bi x(0);
	chk(&skipws(x) == &x);


	}
	end_chk("_27451T_261");
#else
	skip_chk("_27451T_261");
#endif /* CASE_27451T_261 */

/* _27451T_281 noskipws effects */
#if !defined(SKIP_27451T_281)&&(!defined(ONLY)||defined(CASE_27451T_281))
	begin_chk("_27451T_281");
	{
	Bi x(0);
	x.flags(Bi::skipws | Bi::scientific);
	noskipws(x);
	chk(x.flags() == Bi::scientific);


	}
	end_chk("_27451T_281");
#else
	skip_chk("_27451T_281");
#endif /* CASE_27451T_281 */

/* _27451T_291 noskipws returns */
#if !defined(SKIP_27451T_291)&&(!defined(ONLY)||defined(CASE_27451T_291))
	begin_chk("_27451T_291");
	{
	Bi x(0);
	chk(&noskipws(x) == &x);


	}
	end_chk("_27451T_291");
#else
	skip_chk("_27451T_291");
#endif /* CASE_27451T_291 */

/* _27451T_301 uppercase effects */
#if !defined(SKIP_27451T_301)&&(!defined(ONLY)||defined(CASE_27451T_301))
	begin_chk("_27451T_301");
	{
	Bi x(0);
	x.flags(Bi::scientific);
	uppercase(x);
	chk(x.flags() == (Bi::uppercase | Bi::scientific));


	}
	end_chk("_27451T_301");
#else
	skip_chk("_27451T_301");
#endif /* CASE_27451T_301 */

/* _27451T_311 uppercase returns */
#if !defined(SKIP_27451T_311)&&(!defined(ONLY)||defined(CASE_27451T_311))
	begin_chk("_27451T_311");
	{
	Bi x(0);
	chk(&uppercase(x) == &x);


	}
	end_chk("_27451T_311");
#else
	skip_chk("_27451T_311");
#endif /* CASE_27451T_311 */

/* _27451T_331 nouppercase effects */
#if !defined(SKIP_27451T_331)&&(!defined(ONLY)||defined(CASE_27451T_331))
	begin_chk("_27451T_331");
	{
	Bi x(0);
	x.flags(Bi::uppercase | Bi::scientific);
	nouppercase(x);
	chk(x.flags() == Bi::scientific);


	}
	end_chk("_27451T_331");
#else
	skip_chk("_27451T_331");
#endif /* CASE_27451T_331 */

/* _27451T_341 nouppercase returns */
#if !defined(SKIP_27451T_341)&&(!defined(ONLY)||defined(CASE_27451T_341))
	begin_chk("_27451T_341");
	{
	Bi x(0);
	chk(&nouppercase(x) == &x);


	}
	end_chk("_27451T_341");
#else
	skip_chk("_27451T_341");
#endif /* CASE_27451T_341 */

/* _27452T21 internal effects */
#if !defined(SKIP_27452T21)&&(!defined(ONLY)||defined(CASE_27452T21))
	begin_chk("_27452T21");
	{
	Bi x(0);
	x.flags(Bi::adjustfield | Bi::skipws);
	internal(x);
	chk(x.flags() == (Bi::internal | Bi::skipws));


	}
	end_chk("_27452T21");
#else
	skip_chk("_27452T21");
#endif /* CASE_27452T21 */

/* _27452T31 internal returns */
#if !defined(SKIP_27452T31)&&(!defined(ONLY)||defined(CASE_27452T31))
	begin_chk("_27452T31");
	{
	Bi x(0);
	chk(&internal(x) == &x);


	}
	end_chk("_27452T31");
#else
	skip_chk("_27452T31");
#endif /* CASE_27452T31 */

/* _27452T41 left effects */
#if !defined(SKIP_27452T41)&&(!defined(ONLY)||defined(CASE_27452T41))
	begin_chk("_27452T41");
	{
	Bi x(0);
	x.flags(Bi::adjustfield | Bi::skipws);
	left(x);
	chk(x.flags() == (Bi::left | Bi::skipws));


	}
	end_chk("_27452T41");
#else
	skip_chk("_27452T41");
#endif /* CASE_27452T41 */

/* _27452T51 left returns */
#if !defined(SKIP_27452T51)&&(!defined(ONLY)||defined(CASE_27452T51))
	begin_chk("_27452T51");
	{
	Bi x(0);
	chk(&left(x) == &x);


	}
	end_chk("_27452T51");
#else
	skip_chk("_27452T51");
#endif /* CASE_27452T51 */

/* _27452T61 right effects */
#if !defined(SKIP_27452T61)&&(!defined(ONLY)||defined(CASE_27452T61))
	begin_chk("_27452T61");
	{
	Bi x(0);
	x.flags(Bi::adjustfield | Bi::skipws);
	right(x);
	chk(x.flags() == (Bi::right | Bi::skipws));


	}
	end_chk("_27452T61");
#else
	skip_chk("_27452T61");
#endif /* CASE_27452T61 */

/* _27452T71 right returns */
#if !defined(SKIP_27452T71)&&(!defined(ONLY)||defined(CASE_27452T71))
	begin_chk("_27452T71");
	{
	Bi x(0);
	chk(&right(x) == &x);


	}
	end_chk("_27452T71");
#else
	skip_chk("_27452T71");
#endif /* CASE_27452T71 */

/* _27453T21 dec effects */
#if !defined(SKIP_27453T21)&&(!defined(ONLY)||defined(CASE_27453T21))
	begin_chk("_27453T21");
	{
	Bi x(0);
	x.flags(Bi::basefield | Bi::skipws);
	dec(x);
	chk(x.flags() == (Bi::dec | Bi::skipws));


	}
	end_chk("_27453T21");
#else
	skip_chk("_27453T21");
#endif /* CASE_27453T21 */

/* _27453T31 dec returns */
#if !defined(SKIP_27453T31)&&(!defined(ONLY)||defined(CASE_27453T31))
	begin_chk("_27453T31");
	{
	Bi x(0);
	chk(&dec(x) == &x);


	}
	end_chk("_27453T31");
#else
	skip_chk("_27453T31");
#endif /* CASE_27453T31 */

/* _27453T51 hex effects */
#if !defined(SKIP_27453T51)&&(!defined(ONLY)||defined(CASE_27453T51))
	begin_chk("_27453T51");
	{
	Bi x(0);
	x.flags(Bi::basefield | Bi::skipws);
	hex(x);
	chk(x.flags() == (Bi::hex | Bi::skipws));


	}
	end_chk("_27453T51");
#else
	skip_chk("_27453T51");
#endif /* CASE_27453T51 */

/* _27453T61 hex returns */
#if !defined(SKIP_27453T61)&&(!defined(ONLY)||defined(CASE_27453T61))
	begin_chk("_27453T61");
	{
	Bi x(0);
	chk(&hex(x) == &x);


	}
	end_chk("_27453T61");
#else
	skip_chk("_27453T61");
#endif /* CASE_27453T61 */

/* _27453T81 oct effects */
#if !defined(SKIP_27453T81)&&(!defined(ONLY)||defined(CASE_27453T81))
	begin_chk("_27453T81");
	{
	Bi x(0);
	x.flags(Bi::basefield | Bi::skipws);
	oct(x);
	chk(x.flags() == (Bi::oct | Bi::skipws));


	}
	end_chk("_27453T81");
#else
	skip_chk("_27453T81");
#endif /* CASE_27453T81 */

/* _27453T91 oct returns */
#if !defined(SKIP_27453T91)&&(!defined(ONLY)||defined(CASE_27453T91))
	begin_chk("_27453T91");
	{
	Bi x(0);
	chk(&oct(x) == &x);


	}
	end_chk("_27453T91");
#else
	skip_chk("_27453T91");
#endif /* CASE_27453T91 */

/* _27454T21 fixed effects */
#if !defined(SKIP_27454T21)&&(!defined(ONLY)||defined(CASE_27454T21))
	begin_chk("_27454T21");
	{
	Bi x(0);
	x.flags(Bi::floatfield | Bi::skipws);
	fixed(x);
	chk(x.flags() == (Bi::fixed | Bi::skipws));


	}
	end_chk("_27454T21");
#else
	skip_chk("_27454T21");
#endif /* CASE_27454T21 */

/* _27454T31 fixed returns */
#if !defined(SKIP_27454T31)&&(!defined(ONLY)||defined(CASE_27454T31))
	begin_chk("_27454T31");
	{
	Bi x(0);
	chk(&fixed(x) == &x);


	}
	end_chk("_27454T31");
#else
	skip_chk("_27454T31");
#endif /* CASE_27454T31 */

/* _27454T51 scientific effects */
#if !defined(SKIP_27454T51)&&(!defined(ONLY)||defined(CASE_27454T51))
	begin_chk("_27454T51");
	{
	Bi x(0);
	x.flags(Bi::floatfield | Bi::skipws);
	scientific(x);
	chk(x.flags() == (Bi::scientific | Bi::skipws));


	}
	end_chk("_27454T51");
#else
	skip_chk("_27454T51");
#endif /* CASE_27454T51 */

/* _27454T61 scientific returns */
#if !defined(SKIP_27454T61)&&(!defined(ONLY)||defined(CASE_27454T61))
	begin_chk("_27454T61");
	{
	Bi x(0);
	chk(&scientific(x) == &x);


	}
	end_chk("_27454T61");
#else
	skip_chk("_27454T61");
#endif /* CASE_27454T61 */

/* _275T11 <streambuf> synopsis */
#if !defined(SKIP_275T11)&&(!defined(ONLY)||defined(CASE_275T11))
	begin_chk("_275T11");
	{
	streambuf *p2 = 0; TOUCH(p2);
	#if IS_EMBEDDED
	#else /* IS_EMBEDDED */
	basic_streambuf<Char, char_traits<Char> > *p1 = 0; TOUCH(p1);
	wstreambuf *p3 = 0; TOUCH(p3);
	#endif /* IS_EMBEDDED */
	chk(1);










	}
	end_chk("_275T11");
#else
	skip_chk("_275T11");
#endif /* CASE_275T11 */

/* _2752T11 basic_streambuf definition */
#if !defined(SKIP_2752T11)&&(!defined(ONLY)||defined(CASE_2752T11))
	begin_chk("_2752T11");
	{
	Bss sbuf;
	Bs *p = &sbuf;

	Bs::char_type *pct = (char *)0; TOUCH(pct);
	Bs::traits_type *ptt = (It *)0; TOUCH(ptt);	// ADDED [MAR96]
	Bs::int_type *pit = (It::int_type *)0; TOUCH(pit);
	Bs::pos_type *pst = (It::pos_type *)0; TOUCH(pst);
	Bs::off_type *pot = (It::off_type *)0; TOUCH(pot);

	chk(p->sputc((Char)'T') == (Char)'T');
	ieq(p->sputn((const Char *)"his is a test", 13), 13);
	Bs::pos_type pos = p->pubseekoff(0, ios_base::beg);
	chk(pos == p->pubseekpos(pos, ios_base::in));
	p->pubsync();
	p->in_avail();
	chk(p->sbumpc() == (Char)'T');
	chk(p->sgetc() == (Char)'h');
	chk(p->snextc() == (Char)'i');
	chk(p->sputbackc((Char)'h') == (Char)'h');
	chk(p->sungetc() == (Char)'T');
	char arr[14];
	ieq(p->sgetn(arr, 14), 14);
	ieq(STD memcmp(arr, "This is a test", 14), 0);
	locale Myloc("");
	p->pubimbue(Myloc);
	chk(p->getloc() == Myloc);
	p->pubsetbuf(0, 0);


	}
	end_chk("_2752T11");
#else
	skip_chk("_2752T11");
#endif /* CASE_2752T11 */

/* _2752T21 streambuf definition */
#if !defined(SKIP_2752T21)&&(!defined(ONLY)||defined(CASE_2752T21))
	begin_chk("_2752T21");
	{
	basic_streambuf<Char, char_traits<Char> > *p
		= (streambuf *)0; TOUCH(p);
	chk(1);


	}
	end_chk("_2752T21");
#else
	skip_chk("_2752T21");
#endif /* CASE_2752T21 */

/* _2752T31 wstreambuf definition */
#if !defined(SKIP_2752T31)&&(!defined(ONLY)||defined(CASE_2752T31))
	begin_chk("_2752T31");
	{
	basic_streambuf<wchar_t, char_traits<wchar_t> > *p
		= (wstreambuf *)0; TOUCH(p);
	chk(1);
	}
	end_chk("_2752T31");
#else
	skip_chk("_2752T31");
#endif /* CASE_2752T31 */

return leave_chk("t273");
}
/* V3.10:0009 */
