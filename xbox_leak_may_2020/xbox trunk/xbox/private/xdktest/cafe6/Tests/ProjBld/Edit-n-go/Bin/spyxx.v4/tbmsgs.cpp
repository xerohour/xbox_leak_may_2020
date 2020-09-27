//-----------------------------------------------------------------------------
//  TBMsgs.cpp
//
//  Toolbar (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef DISABLE_WIN95_MESSAGES

static VALUETABLE tblTBStyles[] =
{
	TABLEENTRY(TBSTYLE_CHECKGROUP),
	TABLEENTRY(TBSTYLE_BUTTON),
	TABLEENTRY(TBSTYLE_SEP),
	TABLEENTRY(TBSTYLE_CHECK),
	TABLEENTRY(TBSTYLE_GROUP),
	TABLEENTRY(TBSTYLE_TOOLTIPS),
	TABLEENTRY(TBSTYLE_WRAPABLE),
	TABLEENTRY(TBSTYLE_ALTDRAG),
	TABLEEND
};

static VALUETABLE tblTBStates[] =
{
	TABLEENTRY(TBSTATE_CHECKED),
	TABLEENTRY(TBSTATE_PRESSED),
	TABLEENTRY(TBSTATE_ENABLED),
	TABLEENTRY(TBSTATE_HIDDEN),
	TABLEENTRY(TBSTATE_INDETERMINATE),
	TABLEENTRY(TBSTATE_WRAP),
	TABLEEND
};
#endif // DISABLE_WIN95_MESSAGES
#ifndef DISABLE_WIN95_MESSAGES
//////////////////////////////////////////////////////////////////////////////

DECODE(TB_ENABLEBUTTON)
{
	PARM(idButton, INT, WP);
	PARM(fEnable, BOOL, LOLP);

	POUT(idButton);
	POUTB(fEnable);

	return TRUE;
}

DECODERET(TB_ENABLEBUTTON)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_ENABLEBUTTON);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_CHECKBUTTON)
{
	PARM(idButton, INT, WP);
	PARM(fCheck, BOOL, LOLP);

	POUT(idButton);
	POUTB(fCheck);

	return TRUE;
}

DECODERET(TB_CHECKBUTTON)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_CHECKBUTTON);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_PRESSBUTTON)
{
	PARM(idButton, INT, WP);
	PARM(fPress, BOOL, LOLP);

	POUT(idButton);
	POUTB(fPress);

	return TRUE;
}

DECODERET(TB_PRESSBUTTON)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_PRESSBUTTON);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_HIDEBUTTON)
{
	PARM(idButton, INT, WP);
	PARM(fShow, BOOL, LOLP);

	POUT(idButton);
	POUTB(fShow);

	return TRUE;
}

DECODERET(TB_HIDEBUTTON)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_HIDEBUTTON);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_INDETERMINATE)
{
	PARM(idButton, INT, WP);
	PARM(fIndeterminate, BOOL, LOLP);

	POUT(idButton);
	POUTB(fIndeterminate);

	return TRUE;
}

DECODERET(TB_INDETERMINATE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_INDETERMINATE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_ISBUTTONENABLED);

DECODERET(TB_ISBUTTONENABLED)
{
	PARM(fIsEnabled, INT, RET);
	TABLEORINT(tblTrueFalse, fIsEnabled);

	return TRUE;
}

NODECODEPARM(TB_ISBUTTONENABLED);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_ISBUTTONCHECKED);

DECODERET(TB_ISBUTTONCHECKED)
{
	PARM(fIsChecked, INT, RET);
	TABLEORINT(tblTrueFalse, fIsChecked);

	return TRUE;
}

NODECODEPARM(TB_ISBUTTONCHECKED);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_ISBUTTONPRESSED);

DECODERET(TB_ISBUTTONPRESSED)
{
	PARM(fIsPressed, INT, RET);
	TABLEORINT(tblTrueFalse, fIsPressed);

	return TRUE;
}

NODECODEPARM(TB_ISBUTTONPRESSED);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_ISBUTTONHIDDEN);

DECODERET(TB_ISBUTTONHIDDEN)
{
	PARM(fIsHidden, INT, RET);
	TABLEORINT(tblTrueFalse, fIsHidden);

	return TRUE;
}

NODECODEPARM(TB_ISBUTTONHIDDEN);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_ISBUTTONINDETERMINATE);

DECODERET(TB_ISBUTTONINDETERMINATE)
{
	PARM(fIsIndeterminate, INT, RET);
	TABLEORINT(tblTrueFalse, fIsIndeterminate);

	return TRUE;
}

NODECODEPARM(TB_ISBUTTONINDETERMINATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_SETSTATE)
{
	PARM(idButton, INT, WP);
	PARM(fState, UINT, LOLP);

	POUT(idButton);
	POUTC(fState, UINT);

	return TRUE;
}

DECODERET(TB_SETSTATE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_SETSTATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_GETSTATE)
{
	PARM(idButton, INT, WP);
	POUT(idButton);

	return TRUE;
}

NODECODERET(TB_GETSTATE);
	// REVIEW: crack state?

NODECODEPARM(TB_GETSTATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_ADDBITMAP)
{
	PARM(nButtons, INT, WP);
	PARM(lptbab, LPTBADDBITMAP, LP);

	POUT(nButtons);
	POUTC(lptbab, DWORD);

	return TRUE;
}

DECODERET(TB_ADDBITMAP)
{
	PARM(nIndex, INT, RET);
	POUT(nIndex);

	return TRUE;
}

DECODEPARM(TB_ADDBITMAP)
{
	P2WPOUT();

	P2LPOUTPTR(TBADDBITMAP);
	P2IFSIZEOF(TBADDBITMAP)
	{
		PARM(lptbab, LPTBADDBITMAP, ED2);
		INDENT();
		P2OUTC(lptbab, hInst, DWORD);
		P2OUTC(lptbab, nID, UINT);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_ADDBUTTONS)
{
	PARM(uNumButtons, UINT, WP);
	PARM(lpButtons, LPTBBUTTON, LP);

	POUTC(uNumButtons, UINT);
	POUTC(lpButtons, DWORD);

	return TRUE;
}

DECODERET(TB_ADDBUTTONS)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TB_ADDBUTTONS)
{
	P2WPOUT();

	P2LPOUTPTR(TBBUTTON);
	P2IFSIZEOF(TBBUTTON)
	{
		PARM(lpButtons, LPTBBUTTON, ED2);
		INDENT();
		P2OUT(lpButtons, iBitmap);
		P2OUT(lpButtons, idCommand);
		P2OUTC(lpButtons, fsState, BYTE);
		P2OUTC(lpButtons, fsStyle, BYTE);
#ifdef _WIN32
		P2OUTC(lpButtons, bReserved[1], BYTE);
		P2OUTC(lpButtons, bReserved[2], BYTE);
#endif
		P2OUTC(lpButtons, dwData, DWORD);
		P2OUT(lpButtons, iString);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_INSERTBUTTON)
{
	DECODELIKE(TB_GETBUTTON);
}

DECODERET(TB_INSERTBUTTON)
{
	DECODERETLIKE(TB_GETBUTTON);
}

DECODEPARM(TB_INSERTBUTTON)
{
	DECODEPARMLIKE(TB_GETBUTTON);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_DELETEBUTTON)
{
	PARM(iButton, INT, WP);
	POUT(iButton);

	return TRUE;
}

DECODERET(TB_DELETEBUTTON)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_DELETEBUTTON);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_GETBUTTON)
{
	PARM(iButton, INT, WP);
	PARM(lpButton, LPTBBUTTON, LP);

	POUT(iButton);
	POUTC(lpButton, DWORD);

	return TRUE;
}

DECODERET(TB_GETBUTTON)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TB_GETBUTTON)
{
	P2WPOUT();

	P2LPOUTPTR(TBBUTTON);
	P2IFSIZEOF(TBBUTTON)
	{
		PARM(lpButton, LPTBBUTTON, ED2);
		INDENT();
		P2OUT(lpButton, iBitmap);
		P2OUT(lpButton, idCommand);
		P2OUTC(lpButton, fsState, BYTE);
		P2OUTC(lpButton, fsStyle, BYTE);
#ifdef _WIN32
		P2OUTC(lpButton, bReserved[1], BYTE);
		P2OUTC(lpButton, bReserved[2], BYTE);
#endif
		P2OUTC(lpButton, dwData, DWORD);
		P2OUT(lpButton, iString);
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_BUTTONCOUNT);

DECODERET(TB_BUTTONCOUNT)
{
	PARM(nCount, INT, RET);
	POUT(nCount);

	return TRUE;
}

NODECODEPARM(TB_BUTTONCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_COMMANDTOINDEX)
{
	PARM(idButton, INT, WP);
	POUT(idButton);

	return TRUE;
}

DECODERET(TB_COMMANDTOINDEX)
{
	PARM(nIndex, INT, RET);
	POUT(nIndex);

	return TRUE;
}

NODECODEPARM(TB_COMMANDTOINDEX);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_SAVERESTORE)
{
	PARM(fSave, BOOL, WP);
	PARM(ptbsp, TBSAVEPARAMS FAR *, LP);

	POUTB(fSave);
	POUTC(ptbsp, DWORD);

	return TRUE;
}

NODECODERET(TB_SAVERESTORE);

DECODEPARM(TB_SAVERESTORE)
{
	P2WPOUT();

	P2LPOUTPTR(TBSAVEPARAMS);
	P2IFSIZEOF(TBSAVEPARAMS)
	{
		PARM(ptbsp, TBSAVEPARAMS FAR *, ED2);
		INDENT();
//		PARM(pszName1, LPSTR, ptbsp->pszSubKey);
//		PARM(pszName2, LPSTR, ptbsp->pszValueName);
		P2OUTC(ptbsp, hkr, DWORD);
		P2OUTPTRSTR(ptbsp, pszSubKey);
//		INDENT();
//		P2OUTS(pszName1);
//		OUTDENT();
		P2OUTPTRSTR(ptbsp, pszValueName);
//		INDENT();
//		P2OUTS(pszName2);
//		OUTDENT();
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_CUSTOMIZE);

NODECODERET(TB_CUSTOMIZE);

NODECODEPARM(TB_CUSTOMIZE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_ADDSTRING)
{
	PARM(hinst, HINSTANCE, WP);
	PARM(idString, INT, LOLP);

	POUTC(hinst, DWORD);
	POUT(idString);

	return TRUE;
}

DECODERET(TB_ADDSTRING)
{
	PARM(nIndex, INT, RET);
	POUT(nIndex);

	return TRUE;
}

NODECODEPARM(TB_ADDSTRING);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_GETITEMRECT)
{
	PARM(iButton, INT, WP);
	PARM(lprc, LPRECT, LP);

	POUT(iButton);
	POUTC(lprc, DWORD);

	return TRUE;
}

DECODERET(TB_GETITEMRECT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TB_GETITEMRECT)
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

DECODE(TB_BUTTONSTRUCTSIZE)
{
	PARM(cb, INT, WP);
	POUT(cb);

	return TRUE;
}

NODECODERET(TB_BUTTONSTRUCTSIZE);

NODECODEPARM(TB_BUTTONSTRUCTSIZE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_SETBUTTONSIZE)
{
	PARM(dxButton, INT, LOLP);
	PARM(dyButton, INT, HILP);

	POUT(dxButton);
	POUT(dyButton);

	return TRUE;
}

DECODERET(TB_SETBUTTONSIZE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_SETBUTTONSIZE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_SETBITMAPSIZE)
{
	PARM(dxBitmap, INT, LOLP);
	PARM(dyBitmap, INT, HILP);

	POUT(dxBitmap);
	POUT(dyBitmap);

	return TRUE;
}

DECODERET(TB_SETBITMAPSIZE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_SETBITMAPSIZE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_AUTOSIZE);

NODECODERET(TB_AUTOSIZE);

NODECODEPARM(TB_AUTOSIZE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_SETBUTTONTYPE);

DECODERET(TB_SETBUTTONTYPE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_SETBUTTONTYPE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_GETTOOLTIPS);

DECODERET(TB_GETTOOLTIPS)
{
	PARM(hwndToolTip, HWND, RET);
	POUTC(hwndToolTip, DWORD);

	return TRUE;
}

NODECODEPARM(TB_GETTOOLTIPS);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_SETTOOLTIPS)
{
	PARM(hwndToolTip, HWND, WP);
	POUTC(hwndToolTip, DWORD);

	return TRUE;
}

NODECODERET(TB_SETTOOLTIPS);

NODECODEPARM(TB_SETTOOLTIPS);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_SETPARENT)
{
	PARM(hwndParent, HWND, WP);
	POUTC(hwndParent, DWORD);

	return TRUE;
}

NODECODERET(TB_SETPARENT);

NODECODEPARM(TB_SETPARENT);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_SETROWS)
{
	PARM(cRows, INT, LOWP);
	PARM(fLarger, BOOL, HIWP);
	PARM(lprc, LPRECT, LP);

	POUT(cRows);
	POUTB(fLarger);
	POUTC(lprc, DWORD);

	return TRUE;
}

NODECODERET(TB_SETROWS);

DECODEPARM(TB_SETROWS)
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

NODECODE(TB_GETROWS);

DECODERET(TB_GETROWS)
{
	PARM(nCount, INT, RET);
	POUT(nCount);

	return TRUE;
}

NODECODEPARM(TB_GETROWS);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_SETCMDID)
{
	PARM(index, UINT, WP);
	PARM(cmdId, UINT, LP);

	POUTC(index, UINT);
	POUTC(cmdId, UINT);

	return TRUE;
}

DECODERET(TB_SETCMDID)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_SETCMDID);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_CHANGEBITMAP)
{
	PARM(idButton, INT, WP);
	PARM(iBitmap, INT, LOLP);

	POUT(idButton);
	POUT(iBitmap);

	return TRUE;
}

DECODERET(TB_CHANGEBITMAP)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_CHANGEBITMAP);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_GETBITMAP)
{
	PARM(idButton, INT, WP);
	POUT(idButton);

	return TRUE;
}

DECODERET(TB_GETBITMAP)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TB_GETBITMAP);

//////////////////////////////////////////////////////////////////////////////

DECODE(TB_GETBUTTONTEXT)
{
	PARM(idButton, INT, WP);
	PARM(lpszText, LPSTR, LP);

	POUT(idButton);
	POUTC(lpszText, DWORD);

	return TRUE;
}

DECODERET(TB_GETBUTTONTEXT)
{
	PARM(nLength, INT, RET);
	POUT(nLength);

	return TRUE;
}

DECODEPARM(TB_GETBUTTONTEXT)
{
	P2WPOUT();

	P2LPOUTPTRSTR;
	P2IFDATA()
	{
		PARM(lpszText, LPSTR, ED2);
		INDENT();
		P2OUTS(lpszText);
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(TB_GETBITMAPFLAGS);

NODECODERET(TB_GETBITMAPFLAGS);
	// REVIEW: crack?

NODECODEPARM(TB_GETBITMAPFLAGS);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
