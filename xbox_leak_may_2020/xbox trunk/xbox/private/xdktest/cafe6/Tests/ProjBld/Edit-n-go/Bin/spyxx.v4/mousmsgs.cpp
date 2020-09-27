//-----------------------------------------------------------------------------
//  MousMsgs.cpp
//
//  Mouse message crackers.

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

VALUETABLE tblHitTest[] =
{
	TABLEENTRY(HTERROR),
	TABLEENTRY(HTTRANSPARENT),
	TABLEENTRY(HTNOWHERE),
	TABLEENTRY(HTCLIENT),
	TABLEENTRY(HTCAPTION),
	TABLEENTRY(HTSYSMENU),
	TABLEENTRY(HTGROWBOX),
	TABLEENTRY(HTSIZE),
	TABLEENTRY(HTMENU),
	TABLEENTRY(HTHSCROLL),
	TABLEENTRY(HTVSCROLL),
	TABLEENTRY(HTREDUCE),
	TABLEENTRY(HTZOOM),
	TABLEENTRY(HTLEFT),
	TABLEENTRY(HTRIGHT),
	TABLEENTRY(HTTOP),
	TABLEENTRY(HTTOPLEFT),
	TABLEENTRY(HTTOPRIGHT),
	TABLEENTRY(HTBOTTOM),
	TABLEENTRY(HTBOTTOMLEFT),
	TABLEENTRY(HTBOTTOMRIGHT),
	TABLEENTRY(HTBORDER),
	TABLEEND
};

static VALUETABLE tblKeyFlags[] =
{
	TABLEENTRY(MK_CONTROL),
	TABLEENTRY(MK_LBUTTON),
	TABLEENTRY(MK_MBUTTON),
	TABLEENTRY(MK_RBUTTON),
	TABLEENTRY(MK_SHIFT),
	TABLEEND
};

static VALUETABLE tblActivate[] =
{
	TABLEENTRY(MA_ACTIVATE),
	TABLEENTRY(MA_NOACTIVATE),
	TABLEENTRY(MA_ACTIVATEANDEAT),
	TABLEENTRY(MA_NOACTIVATEANDEAT),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_LBUTTONDBLCLK)
{
	PARM(fwKeys, UINT, WP);
	PARM(xPos, INT, (short)LOLP);
	PARM(yPos, INT, (short)HILP);

	TABLEFLAGS(tblKeyFlags, fwKeys);
	POUT(xPos);
	POUT(yPos);

	return TRUE;
}

NODECODERET(WM_LBUTTONDBLCLK);

NODECODEPARM(WM_LBUTTONDBLCLK);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_LBUTTONDOWN)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_LBUTTONDOWN);

NODECODEPARM(WM_LBUTTONDOWN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_LBUTTONUP)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_LBUTTONUP);

NODECODEPARM(WM_LBUTTONUP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MBUTTONDBLCLK)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_MBUTTONDBLCLK);

NODECODEPARM(WM_MBUTTONDBLCLK);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MBUTTONDOWN)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_MBUTTONDOWN);

NODECODEPARM(WM_MBUTTONDOWN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MBUTTONUP)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_MBUTTONUP);

NODECODEPARM(WM_MBUTTONUP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MOUSEACTIVATE)
{
	PARM(hwndTopLevel, HWND, WP);
	PARM(nHittest, UINT, (INT)LOLP);
	PARM(uMsg, UINT, HILP);

	POUT(hwndTopLevel);
	TABLEORVALUE(tblHitTest, nHittest);
	PLABEL(uMsg);
	if (uMsg < WM_USER && CMsgDoc::m_apmdLT[uMsg])
	{
		MOUT << CMsgDoc::m_apmdLT[uMsg]->pszMsg;
	}
	else
	{
		MOUT << (WORD)uMsg;
	}

	return TRUE;
}

DECODERET(WM_MOUSEACTIVATE)
{
	PARM(fuActivate, UINT, RET);

	TABLEORVALUE(tblActivate, fuActivate);

	return TRUE;
}

NODECODEPARM(WM_MOUSEACTIVATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MOUSEMOVE)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_MOUSEMOVE);

NODECODEPARM(WM_MOUSEMOVE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_PARENTNOTIFY)
{
	PARM(fwEvent, UINT, LOWP);

	PLABEL(fwEvent);
	if (fwEvent < WM_USER && CMsgDoc::m_apmdLT[fwEvent])
	{
		MOUT << CMsgDoc::m_apmdLT[fwEvent]->pszMsg;
	}
	else
	{
		MOUT << (WORD)fwEvent;
	}

	switch (fwEvent)
	{
		case WM_CREATE:
		case WM_DESTROY:
			{
				PARM(idChild, UINT, HIWP);
				PARM(hwndChild, HWND, LP);

				POUTC(idChild, WORD);
				MOUT << " (" << (INT)idChild << ')';
				POUT(hwndChild);
			}

			break;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				PARM(xPos, INT, LOLP);
				PARM(yPos, INT, HILP);

				POUT(xPos);
				POUT(yPos);
			}

			break;
	}

	return TRUE;
}

NODECODERET(WM_PARENTNOTIFY);

NODECODEPARM(WM_PARENTNOTIFY);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_RBUTTONDBLCLK)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_RBUTTONDBLCLK);

NODECODEPARM(WM_RBUTTONDBLCLK);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_RBUTTONDOWN)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_RBUTTONDOWN);

NODECODEPARM(WM_RBUTTONDOWN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_RBUTTONUP)
{
	DECODELIKE(WM_LBUTTONDBLCLK);
}

NODECODERET(WM_RBUTTONUP);

NODECODEPARM(WM_RBUTTONUP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SETCURSOR)
{
	PARM(hwnd, HWND, WP);
	PARM(nHittest, UINT, (INT)LOLP);
	PARM(wMouseMsg, WORD, HILP);

	POUT(hwnd);
	TABLEORVALUE(tblHitTest, nHittest);

	//
	// Note that a wMouseMsg value of zero is possible, when the
	// window enters menu mode.  That is the reason for "greater
	// than zero" test below.
	//
	PLABEL(wMouseMsg);
	if (wMouseMsg > 0 && wMouseMsg < WM_USER &&
		CMsgDoc::m_apmdLT[wMouseMsg])
	{
		MOUT << CMsgDoc::m_apmdLT[wMouseMsg]->pszMsg;
	}
	else
	{
		MOUT << (WORD)wMouseMsg;
	}

	return TRUE;
}

DECODERET(WM_SETCURSOR)
{
	PARM(fHaltProcessing, BOOL, RET);

	POUTB(fHaltProcessing);

	return TRUE;
}

NODECODEPARM(WM_SETCURSOR);

//////////////////////////////////////////////////////////////////////////////
