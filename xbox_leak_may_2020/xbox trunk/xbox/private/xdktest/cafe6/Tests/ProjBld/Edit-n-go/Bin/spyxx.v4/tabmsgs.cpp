//-----------------------------------------------------------------------------
//  TabMsgs.cpp
//
//  Tab (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static VALUETABLE tblTCRetOK[] =
{
	TABLEENTRY(LB_OKAY),
	TABLEENTRY(LB_ERR),
	TABLEENTRY(LB_ERRSPACE),
	TABLEEND
};

#ifndef DISABLE_WIN95_MESSAGES
//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_GETBKCOLOR);

DECODERET(TCM_GETBKCOLOR)
{
	PARM(clrBk, COLORREF, RET);
	POUTC(clrBk, DWORD);

	return TRUE;
}

NODECODEPARM(TCM_GETBKCOLOR);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_SETBKCOLOR)
{
	PARM(clrBk, COLORREF, LP);
	POUTC(clrBk, DWORD);

	return TRUE;
}

DECODERET(TCM_SETBKCOLOR)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TCM_SETBKCOLOR);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_GETIMAGELIST);

DECODERET(TCM_GETIMAGELIST)
{
	PARM(hImageList, HIMAGELIST, RET);
	POUTC(hImageList, DWORD);

	return TRUE;
}

NODECODEPARM(TCM_GETIMAGELIST);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_SETIMAGELIST)
{
	PARM(hImageList, HIMAGELIST, LP);
	POUTC(hImageList, DWORD);

	return TRUE;
}

DECODERET(TCM_SETIMAGELIST)
{
	PARM(hPrevImageList, HIMAGELIST, RET);
	POUTC(hPrevImageList, DWORD);

	return TRUE;
}

NODECODEPARM(TCM_SETIMAGELIST);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_GETITEMCOUNT);

DECODERET(TCM_GETITEMCOUNT)
{
	PARM(nCount, INT, RET);
	POUT(nCount);

	return TRUE;
}

NODECODEPARM(TCM_GETITEMCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_GETITEM)
{
	PARM(iItem, INT, WP);
	PARM(pitem, TC_ITEM FAR *, LP);

	POUT(iItem);
	POUTC(pitem, DWORD);

	return TRUE;
}

DECODERET(TCM_GETITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TCM_GETITEM)
{
	P2WPOUT();

	P2LPOUTPTR(TC_ITEM);
	P2IFSIZEOF(TC_ITEM)
	{
		PARM(pitem, TC_ITEM FAR *, ED2);
//		PARM(pszName, LPSTR, pitem->pszText);
		INDENT();
		P2OUTC(pitem, mask, UINT);
		P2OUTC(pitem, lpReserved1, UINT);
		P2OUTC(pitem, lpReserved2, UINT);
		P2OUTPTRSTR(pitem, pszText);
//		INDENT();
//		P2OUTS(pszName);
//		OUTDENT();
		P2OUT(pitem, cchTextMax);
		P2OUT(pitem, iImage);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_SETITEM)
{
	DECODELIKE(TCM_GETITEM);
}

DECODERET(TCM_SETITEM)
{
	DECODERETLIKE(TCM_GETITEM);
}

DECODEPARM(TCM_SETITEM)
{
	DECODEPARMLIKE(TCM_GETITEM);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_INSERTITEM)
{
	DECODELIKE(TCM_GETITEM);
}

DECODERET(TCM_INSERTITEM)
{
	PARM(iIndex, INT, RET);
	POUT(iIndex);

	return TRUE;
}

DECODEPARM(TCM_INSERTITEM)
{
	DECODEPARMLIKE(TCM_GETITEM);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_DELETEITEM)
{
	PARM(iItem, INT, WP);
	POUT(iItem);

	return TRUE;
}

DECODERET(TCM_DELETEITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TCM_DELETEITEM);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_DELETEALLITEMS);

DECODERET(TCM_DELETEALLITEMS)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TCM_DELETEALLITEMS);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_GETITEMRECT)
{
	PARM(iItem, INT, WP);
	PARM(prc, LPRECT, LP);

	POUT(iItem);
	POUTC(prc, DWORD);

	return TRUE;
}

DECODERET(TCM_GETITEMRECT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TCM_GETITEMRECT)
{
	P2WPOUT();

	P2LPOUTPTR(RECT);
	P2IFSIZEOF(RECT)
	{
		PARM(prc, LPRECT, ED2);
		INDENT();
		P2OUT(prc, left);
		P2OUT(prc, top);
		P2OUT(prc, right);
		P2OUT(prc, bottom);
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_GETCURSEL);

DECODERET(TCM_GETCURSEL)
{
	PARM(iSelectedTab, INT, RET);
	POUT(iSelectedTab);

	return TRUE;
}

NODECODEPARM(TCM_GETCURSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_SETCURSEL)
{
	PARM(iItem, INT, WP);
	POUT(iItem);

	return TRUE;
}

DECODERET(TCM_SETCURSEL)
{
	PARM(iPrevIndex, INT, RET);
	POUT(iPrevIndex);

	return TRUE;
}

NODECODEPARM(TCM_SETCURSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_HITTEST)
{
	PARM(pinfo, TC_HITTESTINFO FAR *, LP);
	POUTC(pinfo, DWORD);

	return TRUE;
}

DECODERET(TCM_HITTEST)
{
	PARM(iIndex, INT, RET);
	POUT(iIndex);

	return TRUE;
}

DECODEPARM(TCM_HITTEST)
{
	P2WPOUT();

	P2LPOUTPTR(TC_HITTESTINFO);
	P2IFSIZEOF(TC_HITTESTINFO)
	{
		PARM(pinfo, TC_HITTESTINFO FAR *, ED2);
		INDENT();
		P2OUTPT(pinfo, pt);
		P2OUTC(pinfo, flags, UINT);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_SETITEMEXTRA)
{
	PARM(cb, INT, WP);
	POUT(cb);

	return TRUE;
}

DECODERET(TCM_SETITEMEXTRA)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TCM_SETITEMEXTRA);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_ADJUSTRECT)
{
	PARM(fLarger, BOOL, WP);
	PARM(prc, LPRECT, LP);

	POUTB(fLarger);
	POUTC(prc, DWORD);

	return TRUE;
}

NODECODERET(TCM_ADJUSTRECT);

DECODEPARM(TCM_ADJUSTRECT)
{
	P2WPOUT();

	P2LPOUTPTR(RECT);
	P2IFSIZEOF(RECT)
	{
		PARM(prc, LPRECT, ED2);
		INDENT();
		P2OUT(prc, left);
		P2OUT(prc, top);
		P2OUT(prc, right);
		P2OUT(prc, bottom);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_SETITEMSIZE)
{
	PARM(cx, INT, LOLP);
	PARM(cy, INT, HILP);

	POUT(cx);
	POUT(cy);

	return TRUE;
}

DECODERET(TCM_SETITEMSIZE)
{
	PARM(cxPrev, INT, LORET);
	PARM(cyPrev, INT, HIRET);

	POUT(cxPrev);
	POUT(cyPrev);

	return TRUE;
}

NODECODEPARM(TCM_SETITEMSIZE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_REMOVEIMAGE)
{
	PARM(iImage, INT, WP);
	POUT(iImage);

	return TRUE;
}

NODECODERET(TCM_REMOVEIMAGE);

NODECODEPARM(TCM_REMOVEIMAGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_SETPADDING)
{
	PARM(cx, INT, LOLP);
	PARM(cy, INT, HILP);

	POUT(cx);
	POUT(cy);

	return TRUE;
}

NODECODERET(TCM_SETPADDING);

NODECODEPARM(TCM_SETPADDING);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_GETROWCOUNT);

DECODERET(TCM_GETROWCOUNT)
{
	PARM(iCount, INT, RET);
	POUT(iCount);

	return TRUE;
}

NODECODEPARM(TCM_GETROWCOUNT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_GETTOOLTIPS);

DECODERET(TCM_GETTOOLTIPS)
{
	PARM(hwndTT, HWND, RET);
	POUTC(hwndTT, DWORD);

	return TRUE;
}

NODECODEPARM(TCM_GETTOOLTIPS);

//////////////////////////////////////////////////////////////////////////////

DECODE(TCM_SETTOOLTIPS)
{
	PARM(hwndTT, HWND, WP);
	POUTC(hwndTT, DWORD);

	return TRUE;
}

NODECODERET(TCM_SETTOOLTIPS);

NODECODEPARM(TCM_SETTOOLTIPS);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_GETCURFOCUS);

DECODERET(TCM_GETCURFOCUS)
{
	PARM(iItem, INT, RET);
	POUT(iItem);

	return TRUE;
}

NODECODEPARM(TCM_GETCURFOCUS);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TCM_SETCURFOCUS);

NODECODERET(TCM_SETCURFOCUS);

NODECODEPARM(TCM_SETCURFOCUS);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
