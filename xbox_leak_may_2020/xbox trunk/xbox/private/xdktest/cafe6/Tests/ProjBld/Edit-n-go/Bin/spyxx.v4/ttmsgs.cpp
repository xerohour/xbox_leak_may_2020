//-----------------------------------------------------------------------------
//  TTMsgs.cpp
//
//  Tooltips (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef DISABLE_WIN95_MESSAGES

static VALUETABLE tblTTDelayTime[] =
{
	TABLEENTRY(TTDT_AUTOMATIC),
//	TABLEENTRY(TTDT_AUTOPOPUP),
	TABLEENTRY(TTDT_INITIAL),
	TABLEENTRY(TTDT_RESHOW),
	TABLEEND
};


//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_ACTIVATE)
{
	PARM(Activate, BOOL, WP);
	POUTB(Activate);

	return TRUE;
}

NODECODERET(TTM_ACTIVATE);

NODECODEPARM(TTM_ACTIVATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_SETDELAYTIME)
{
	PARM(uFlag, UINT, WP);
	PARM(iDelay, INT, LP);

	TABLEORVALUE(tblTTDelayTime, uFlag);
	POUT(iDelay);

	return TRUE;
}

NODECODERET(TTM_SETDELAYTIME);

NODECODEPARM(TTM_SETDELAYTIME);

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_ADDTOOL)
{
	PARM(lpti, LPTOOLINFO, LP);
	POUTC(lpti, DWORD);

	return TRUE;
}

DECODERET(TTM_ADDTOOL)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TTM_ADDTOOL)
{
	P2WPOUT();

	P2LPOUTPTR(TOOLINFO);
	P2IFSIZEOF(TOOLINFO)
	{
		PARM(lpti, LPTOOLINFO, ED2);
//		PARM(pszName, LPSTR, lpti->lpszText);
		INDENT();
		P2OUTC(lpti, cbSize, UINT);
		P2OUTC(lpti, uFlags, UINT);
		P2OUTC(lpti, hwnd, HWND);
		P2OUTC(lpti, uId, UINT);
		P2OUTRECT(lpti, rect);
		P2OUTC(lpti, hinst, DWORD);
		P2OUTPTRSTR(lpti, lpszText);
//		INDENT();
//		P2OUTS(pszName);
//		OUTDENT();
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_DELTOOL)
{
	DECODELIKE(TTM_ADDTOOL);
}

NODECODERET(TTM_DELTOOL);

DECODEPARM(TTM_DELTOOL)
{
	DECODEPARMLIKE(TTM_ADDTOOL);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_NEWTOOLRECT)
{
	DECODELIKE(TTM_ADDTOOL);
}

NODECODERET(TTM_NEWTOOLRECT);

DECODEPARM(TTM_NEWTOOLRECT)
{
	DECODEPARMLIKE(TTM_ADDTOOL);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_RELAYEVENT)
{
	PARM(lpmsg, LPMSG, LP);
	POUTC(lpmsg, DWORD);

	return TRUE;
}

NODECODERET(TTM_RELAYEVENT);

DECODEPARM(TTM_RELAYEVENT)
{
	P2WPOUT();

	P2LPOUTPTR(MSG);
	P2IFSIZEOF(MSG)
	{
		PARM(lpmsg, LPMSG, ED2);
		INDENT();
		P2OUTC(lpmsg, hwnd, HWND);
		P2OUTC(lpmsg, message, UINT);
		P2OUTC(lpmsg, wParam, WPARAM);
		P2OUTC(lpmsg, lParam, LPARAM);
		P2OUTC(lpmsg, time, DWORD);
		P2OUTPT(lpmsg, pt);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_GETTOOLINFO)
{
	DECODELIKE(TTM_ADDTOOL);
}

DECODERET(TTM_GETTOOLINFO)
{
	DECODERETLIKE(TTM_ADDTOOL);
}

DECODEPARM(TTM_GETTOOLINFO)
{
	DECODEPARMLIKE(TTM_ADDTOOL);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_SETTOOLINFO)
{
	DECODELIKE(TTM_ADDTOOL);
}

NODECODERET(TTM_SETTOOLINFO);

DECODEPARM(TTM_SETTOOLINFO)
{
	DECODEPARMLIKE(TTM_ADDTOOL);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_HITTEST)
{
	PARM(lphti, TTHITTESTINFO FAR *, LP);
	POUTC(lphti, DWORD);

	return TRUE;
}

DECODERET(TTM_HITTEST)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TTM_HITTEST)
{
	P2WPOUT();

	P2LPOUTPTR(TTHITTESTINFO);
	P2IFSIZEOF(TTHITTESTINFO)
	{
		PARM(lphti, TTHITTESTINFO FAR *, ED2);
		INDENT();
		P2OUTC(lphti, hwnd, HWND);
		P2OUTPT(lphti, pt);
		P2OUTPTR(&lphti, ti, TOOLINFO);
		{
			PARM(lpti, LPTOOLINFO, &lphti->ti);
//			PARM(pszName, LPSTR, lpti->lpszText);
			INDENT();
			P2OUTC(lpti, cbSize, UINT);
			P2OUTC(lpti, uFlags, UINT);
			P2OUTC(lpti, hwnd, HWND);
			P2OUTC(lpti, uId, UINT);
			P2OUTRECT(lpti, rect);
			P2OUTC(lpti, hinst, DWORD);
			P2OUTPTRSTR(lpti, lpszText);
//			INDENT();
//			P2OUTS(pszName);
//			OUTDENT();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_GETTEXT)
{
	DECODELIKE(TTM_ADDTOOL);
}

DECODERET(TTM_GETTEXT)
{
	DECODERETLIKE(TTM_ADDTOOL);
}

DECODEPARM(TTM_GETTEXT)
{
	DECODEPARMLIKE(TTM_ADDTOOL);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_UPDATETIPTEXT)
{
	DECODELIKE(TTM_ADDTOOL);
}

NODECODERET(TTM_UPDATETIPTEXT);

DECODEPARM(TTM_UPDATETIPTEXT)
{
	DECODEPARMLIKE(TTM_ADDTOOL);
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(TTM_GETTOOLCOUNT);

DECODERET(TTM_GETTOOLCOUNT)
{
	PARM(nCount, INT, RET);
	POUT(nCount);

	return TRUE;
}

NODECODEPARM(TTM_GETTOOLCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_ENUMTOOLS)
{
	PARM(iTool, UINT, WP);
	PARM(lpti, LPTOOLINFO, LP);

	POUTC(iTool, UINT);
	POUTC(lpti, DWORD);

	return TRUE;
}

DECODERET(TTM_ENUMTOOLS)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TTM_ENUMTOOLS)
{
	DECODEPARMLIKE(TTM_ADDTOOL);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TTM_GETCURRENTTOOL)
{
	DECODELIKE(TTM_ADDTOOL);
}

DECODERET(TTM_GETCURRENTTOOL)
{
	DECODERETLIKE(TTM_ADDTOOL);
}

DECODEPARM(TTM_GETCURRENTTOOL)
{
	DECODEPARMLIKE(TTM_ADDTOOL);
}

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
