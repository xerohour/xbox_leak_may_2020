//-----------------------------------------------------------------------------
//  StbMsgs.cpp
//
//  Status bar (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static VALUETABLE tblSTBGetText[] =
{
	TABLEENTRY(SBT_NOBORDERS),
	TABLEENTRY(SBT_POPOUT),
	TABLEENTRY(SBT_OWNERDRAW),
	TABLEEND
};

#ifndef DISABLE_WIN95_MESSAGES
//////////////////////////////////////////////////////////////////////////////

DECODE(SB_SETTEXT)
{
	PARM(iPart, BYTE, LOWP);
	PARM(uType, BYTE, HIWP);
	PARM(szText, LPSTR, LP);

	POUTC(iPart, BYTE);
	POUTC(uType, BYTE);
	POUTC(szText, DWORD);

	return TRUE;
}

DECODERET(SB_SETTEXT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(SB_SETTEXT);
	// REVIEW: ownerdraw text returns different value

//////////////////////////////////////////////////////////////////////////////

DECODE(SB_GETTEXT)
{
	PARM(iPart, INT, WP);
	PARM(szText, LPSTR, LP);

	POUT(iPart);
	POUTC(szText, DWORD);

	return TRUE;
}

DECODERET(SB_GETTEXT)
{
	PARM(nLength, INT, LORET);
	PARM(type, INT, HIRET);

	POUT(nLength);
	TABLEORVALUE(tblSTBGetText, type);

	// REVIEW: ownerdraw text returns different value

	return TRUE;
}

NODECODEPARM(SB_GETTEXT);
	// REVIEW: ownerdraw text returns different value

//////////////////////////////////////////////////////////////////////////////

DECODE(SB_GETTEXTLENGTH)
{
	PARM(iPart, INT, WP);
	POUT(iPart);

	return TRUE;
}

DECODERET(SB_GETTEXTLENGTH)
{
	PARM(nLength, INT, LORET);
	PARM(type, INT, HIRET);

	POUT(nLength);
	TABLEORVALUE(tblSTBGetText, type);

	return TRUE;
}

NODECODEPARM(SB_GETTEXTLENGTH);

//////////////////////////////////////////////////////////////////////////////

DECODE(SB_SETPARTS)
{
	PARM(nParts, INT, WP);
	PARM(aWidths, LPINT, LP);

	POUT(nParts);
	POUTC(aWidths, DWORD);

	return TRUE;
}

DECODERET(SB_SETPARTS)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(SB_SETPARTS);

//////////////////////////////////////////////////////////////////////////////

DECODE(SB_GETPARTS)
{
	PARM(nParts, INT, WP);
	PARM(aRightCoord, LPINT, LP);

	POUT(nParts);
	POUTC(aRightCoord, DWORD);

	return TRUE;
}

DECODERET(SB_GETPARTS)
{
	PARM(nParts, INT, RET);
	POUT(nParts);

	return TRUE;
}

NODECODEPARM(SB_GETPARTS);

//////////////////////////////////////////////////////////////////////////////

DECODE(SB_GETBORDERS)
{
	PARM(aBorders, LPINT, LP);
	POUTC(aBorders, DWORD);

	return TRUE;
}

DECODERET(SB_GETBORDERS)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(SB_GETBORDERS);

//////////////////////////////////////////////////////////////////////////////

DECODE(SB_SETMINHEIGHT)
{
	PARM(minHeight, INT, WP);
	POUT(minHeight);

	return TRUE;
}

NODECODERET(SB_SETMINHEIGHT);

NODECODEPARM(SB_SETMINHEIGHT);

//////////////////////////////////////////////////////////////////////////////

DECODE(SB_SIMPLE)
{
	PARM(fSimple, BOOL, WP);
	POUTB(fSimple);

	return TRUE;
}

DECODERET(SB_SIMPLE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(SB_SIMPLE);

//////////////////////////////////////////////////////////////////////////////

DECODE(SB_GETRECT)
{
	PARM(iPart, INT, WP);
	PARM(lprc, LPRECT, LP);

	POUT(iPart);
	POUTC(lprc, DWORD);

	return TRUE;
}

DECODERET(SB_GETRECT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(SB_GETRECT)
{
	P2WPOUT();

	P2LPOUTPTR(RECT);
	P2IFSIZEOF(RECT)
	{
		PARM(lprc, LPRECT, ED2);
		INDENT();
		P2OUT(lprc, left);
		P2OUT(lprc, top);
		P2OUT(lprc, right);
		P2OUT(lprc, bottom);
	}
}

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
