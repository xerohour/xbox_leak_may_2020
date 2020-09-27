//-----------------------------------------------------------------------------
//  AniMsgs.cpp
//
//  Animate (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef DISABLE_WIN95_MESSAGES
//////////////////////////////////////////////////////////////////////////////

DECODE(ACM_OPEN)
{
	PARM(lpszName, LPSTR, LP);

	POUTC(lpszName, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

DECODERET(ACM_OPEN)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(ACM_OPEN)
{
	P2WPOUT();

	P2LPOUTPTRSTR;
	P2IFDATA()
	{
		PARM(pstr, LPSTR, ED2);
		INDENT();
		P2OUTS(pstr);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(ACM_PLAY)
{
	PARM(cRepeat, UINT, WP);
	PARM(wFrom, UINT, LOLP);
	PARM(wTo, UINT, HILP);

	POUTC(cRepeat, INT);
	POUTC(wFrom, INT);
	POUTC(wTo, INT);

	return TRUE;
}

DECODERET(ACM_PLAY)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(ACM_PLAY);

//////////////////////////////////////////////////////////////////////////////

NODECODE(ACM_STOP);

NODECODERET(ACM_STOP);

NODECODEPARM(ACM_STOP);

//////////////////////////////////////////////////////////////////////////////

NODECODE(ACM_OPENW);

NODECODERET(ACM_OPENW);

NODECODEPARM(ACM_OPENW);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
