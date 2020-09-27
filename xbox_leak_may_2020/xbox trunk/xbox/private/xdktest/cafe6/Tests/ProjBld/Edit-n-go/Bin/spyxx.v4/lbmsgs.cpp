//-----------------------------------------------------------------------------
//  LBMsgs.cpp
//
//  Listbox message (LB_*) crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

VALUETABLE tblFileAttrs[] =
{
	TABLEENTRY(DDL_READONLY),
	TABLEENTRY(DDL_HIDDEN),
	TABLEENTRY(DDL_SYSTEM),
	TABLEENTRY(DDL_DIRECTORY),
	TABLEENTRY(DDL_ARCHIVE),
	TABLEENTRY(DDL_DRIVES),
	TABLEENTRY(DDL_EXCLUSIVE),
	//
	// DDL_READWRITE is 0x0000, and so it should not be in the table.
	//
	//TABLEENTRY(DDL_READWRITE),
	TABLEEND
};

VALUETABLE tblLBRet[] =
{
	TABLEENTRY(LB_ERR),
	TABLEENTRY(LB_ERRSPACE),
	TABLEEND
};

static VALUETABLE tblLBRetOK[] =
{
	TABLEENTRY(LB_OKAY),
	TABLEENTRY(LB_ERR),
	TABLEENTRY(LB_ERRSPACE),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_ADDFILE)
{
	PARM(lpszFilename, LPCTSTR, LP);

	POUTC(lpszFilename, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

DECODERET(LB_ADDFILE)
{
	DECODERETLIKE(LB_ADDSTRING);
}

DECODEPARM(LB_ADDFILE)
{
	DECODEPARMLIKE(LB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_ADDSTRING)
{
	PARM(lpsz, LPCTSTR, LP);

	POUTC(lpsz, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

DECODERET(LB_ADDSTRING)
{
	PARM(index, INT, RET);

	TABLEORINT(tblLBRet, index);

	return TRUE;
}

DECODEPARM(LB_ADDSTRING)
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

DECODE(LB_DELETESTRING)
{
	PARM(index, INT, WP);

	POUT(index);

	return TRUE;
}

DECODERET(LB_DELETESTRING)
{
	PARM(cItemsRemaining, INT, RET);

	TABLEORINT(tblLBRet, cItemsRemaining);

	return TRUE;
}

NODECODEPARM(LB_DELETESTRING);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_DIR)
{
	PARM(uAttrs, UINT, WP);
	PARM(lpszFileSpec, LPCTSTR, LP);

	TABLEFLAGS(tblFileAttrs, uAttrs);
	POUTC(lpszFileSpec, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

DECODERET(LB_DIR)
{
	DECODERETLIKE(LB_ADDSTRING);
}

DECODEPARM(LB_DIR)
{
	DECODEPARMLIKE(LB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_FINDSTRING)
{
	PARM(indexStart, INT, WP);
	PARM(lpszFind, LPCTSTR, LP);

	POUT(indexStart);
	if (indexStart == -1)
	{
		MOUT << ids(IDS_FROM_THE_BEGINNING);
	}

	POUTC(lpszFind, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

DECODERET(LB_FINDSTRING)
{
	DECODERETLIKE(LB_ADDSTRING);
}

DECODEPARM(LB_FINDSTRING)
{
	DECODEPARMLIKE(LB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_FINDSTRINGEXACT)
{
	DECODELIKE(LB_FINDSTRING);
}

DECODERET(LB_FINDSTRINGEXACT)
{
	DECODERETLIKE(LB_FINDSTRING);
}

DECODEPARM(LB_FINDSTRINGEXACT)
{
	DECODEPARMLIKE(LB_FINDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(LB_GETANCHORINDEX);

DECODERET(LB_GETANCHORINDEX)
{
	DECODERETLIKE(LB_ADDSTRING);
}

NODECODEPARM(LB_GETANCHORINDEX);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LB_GETCARETINDEX);

DECODERET(LB_GETCARETINDEX)
{
	DECODERETLIKE(LB_ADDSTRING);
}

NODECODEPARM(LB_GETCARETINDEX);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LB_GETCOUNT);

DECODERET(LB_GETCOUNT)
{
	PARM(cItems, INT, RET);

	TABLEORINT(tblLBRet, cItems);

	return TRUE;
}

NODECODEPARM(LB_GETCOUNT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LB_GETCURSEL);

DECODERET(LB_GETCURSEL)
{
	DECODERETLIKE(LB_ADDSTRING);
}

NODECODEPARM(LB_GETCURSEL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LB_GETHORIZONTALEXTENT);

DECODERET(LB_GETHORIZONTALEXTENT)
{
	PARM(cxExtent, INT, RET);

	TABLEORINT(tblLBRet, cxExtent);

	return TRUE;
}

NODECODEPARM(LB_GETHORIZONTALEXTENT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_GETITEMDATA)
{
	PARM(index, INT, WP);

	POUT(index);

	return TRUE;
}

DECODERET(LB_GETITEMDATA)
{
	PARM(dwData, DWORD, RET);

	TABLEORDWORD(tblLBRet, dwData);

	return TRUE;
}

NODECODEPARM(LB_GETITEMDATA);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_GETITEMHEIGHT)
{
	DECODELIKE(LB_GETITEMDATA);
}

DECODERET(LB_GETITEMHEIGHT)
{
	PARM(cyItem, INT, RET);

	TABLEORINT(tblLBRet, cyItem);

	return TRUE;
}

NODECODEPARM(LB_GETITEMHEIGHT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_GETITEMRECT)
{
	PARM(index, INT, WP);
	PARM(lprc, LPRECT, LP);

	POUT(index);
	POUTC(lprc, DWORD);

	return TRUE;
}

DECODERET(LB_GETITEMRECT)
{
	PARM(lResult, UINT, RET);
	PARM(lprc, LPRECT, LP);

	TABLEORVALUE(tblLBRetOK, lResult);

	if (lResult != LB_ERR)
	{
		POUTC(lprc, DWORD);

		PIFSIZEOF(RECT)
		{
			PARM(prc, LPRECT, ED);
			POUTRECT(prc);
		}
	}

	return TRUE;
}

DECODEPARM(LB_GETITEMRECT)
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

NODECODE(LB_GETLOCALE);

DECODERET(LB_GETLOCALE)
{
	PARM(dwLocaleID, DWORD, RET);

	POUT(dwLocaleID);

	return TRUE;
}

NODECODEPARM(LB_GETLOCALE);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_GETSEL)
{
	DECODELIKE(LB_GETITEMDATA);
}

DECODERET(LB_GETSEL)
{
	PARM(fSelected, UINT, RET);

	PLABEL(fSelected);
	if (!TableOrNothing(tblLBRet, fSelected))
	{
		MOUT << tf(fSelected);
	}

	return TRUE;
}

NODECODEPARM(LB_GETSEL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LB_GETSELCOUNT);

DECODERET(LB_GETSELCOUNT)
{
	PARM(cItemsSelected, INT, RET);

	TABLEORINT(tblLBRet, cItemsSelected);

	return TRUE;
}

NODECODEPARM(LB_GETSELCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_GETSELITEMS)
{
	PARM(cItems, INT, WP);
	PARM(lpnItems, LPINT, LP);

	POUT(cItems);
	POUTC(lpnItems, DWORD);

	return TRUE;
}

DECODERET(LB_GETSELITEMS)
{
	PARM(cItems, INT, RET);
	PARM(lpnItems, LPINT, LP);

	TABLEORINT(tblLBRet, cItems);

	PIFDATA()
	{
		PARM(pItems, PINT, ED);

		POUTC(lpnItems, DWORD);
		PrintIntArray(pItems, PSIZEOFED() / sizeof(INT), cItems);
	}

	return TRUE;
}

DECODEPARM(LB_GETSELITEMS)
{
	P2WPOUT();

	P2LPOUTPTR(INT array);
	P2IFDATA()
	{
		PARM(pItems, PINT, ED2);
		PARM(cItems, INT, RET2);
		INDENT();
		PrintIntArray2(pmps, pItems, P2SIZEOFED() / sizeof(INT), cItems);
		P2ENDLINE();
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_GETTEXT)
{
	PARM(index, INT, WP);
	PARM(lpszBuffer, LPCTSTR, LP);

	POUT(index);
	POUTC(lpszBuffer, DWORD);

	return TRUE;
}

DECODERET(LB_GETTEXT)
{
	PARM(cchText, INT, RET);
	PARM(lpszBuffer, LPCTSTR, LP);

	TABLEORINT(tblLBRet, cchText);
	POUTC(lpszBuffer, DWORD);
	PIFDATA()
	{
		PARM(pso, PPACK_STRINGORD, ED);

		PIFISORD(pso)
		{
			POUTORD(pso);
		}
		else
		{
			PARM(pstr, LPSTR, ED);
			POUTS(pstr);
		}
	}

	return TRUE;
}

DECODEPARM(LB_GETTEXT)
{
	P2WPOUT();

	P2LPOUTPTRSTR;
	P2IFDATA()
	{
		PARM(pso, PPACK_STRINGORD, ED2);

		INDENT();

		P2IFISORD(pso)
		{
			P2OUTORD(pso);
		}
		else
		{
			PARM(pstr, LPSTR, ED2);
			P2OUTS(pstr);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_GETTEXTLEN)
{
	DECODELIKE(LB_GETITEMDATA);
}

DECODERET(LB_GETTEXTLEN)
{
	PARM(cchText, INT, RET);

	TABLEORINT(tblLBRet, cchText);

	return TRUE;
}

NODECODEPARM(LB_GETTEXTLEN);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LB_GETTOPINDEX);

DECODERET(LB_GETTOPINDEX)
{
	PARM(index, INT, RET);

	TABLEORINT(tblLBRet, index);

	return TRUE;
}

NODECODEPARM(LB_GETTOPINDEX);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_INSERTSTRING)
{
	PARM(index, INT, WP);
	PARM(lpsz, LPCTSTR, LP);

	POUT(index);
	if (index == -1)
	{
		MOUT << ids(IDS_END_OF_THE_LIST);
	}

	POUTC(lpsz, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

DECODERET(LB_INSERTSTRING)
{
	DECODERETLIKE(LB_GETITEMDATA);
}

DECODEPARM(LB_INSERTSTRING)
{
	DECODEPARMLIKE(LB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(LB_RESETCONTENT);

NODECODERET(LB_RESETCONTENT);

NODECODEPARM(LB_RESETCONTENT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SELECTSTRING)
{
	DECODELIKE(LB_FINDSTRING);
}

DECODERET(LB_SELECTSTRING)
{
	DECODERETLIKE(LB_GETITEMDATA);
}

DECODEPARM(LB_SELECTSTRING)
{
	DECODEPARMLIKE(LB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SELITEMRANGE)
{
	PARM(fSelect, BOOL, WP);
	PARM(wFirst, UINT, LOLP);
	PARM(wLast, UINT, HILP);

	POUTB(fSelect);
	POUTC(wFirst, INT);
	POUTC(wLast, INT);

	return TRUE;
}

DECODERET(LB_SELITEMRANGE)
{
	PARM(lResult, UINT, RET);

	TABLEORVALUE(tblLBRetOK, lResult);

	return TRUE;
}

NODECODEPARM(LB_SELITEMRANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SELITEMRANGEEX)
{
	PARM(indexFirst, UINT, WP);
	PARM(indexLast, UINT, LP);

	POUTC(indexFirst, INT);
	POUTC(indexLast, INT);
	if (indexFirst > indexLast)
	{
		MOUT << ids(IDS_UNSELECT_THE_RANGE);
	}

	return TRUE;
}

DECODERET(LB_SELITEMRANGEEX)
{
	DECODERETLIKE(LB_SELITEMRANGE);
}

NODECODEPARM(LB_SELITEMRANGEEX);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETANCHORINDEX)
{
	DECODELIKE(LB_GETITEMDATA);
}

DECODERET(LB_SETANCHORINDEX)
{
	DECODERETLIKE(LB_SELITEMRANGE);
}

NODECODEPARM(LB_SETANCHORINDEX);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETCARETINDEX)
{
	PARM(index, INT, WP);
	PARM(fScroll, BOOL, LP);

	POUT(index);
	POUTB(fScroll);

	return TRUE;
}

DECODERET(LB_SETCARETINDEX)
{
	DECODERETLIKE(LB_SELITEMRANGE);
}

NODECODEPARM(LB_SETCARETINDEX);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETCOLUMNWIDTH)
{
	PARM(cxColumn, INT, WP);

	POUT(cxColumn);

	return TRUE;
}

NODECODERET(LB_SETCOLUMNWIDTH);

NODECODEPARM(LB_SETCOLUMNWIDTH);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETCOUNT)
{
	PARM(cItems, INT, WP);

	POUT(cItems);

	return TRUE;
}

DECODERET(LB_SETCOUNT)
{
	DECODERETLIKE(LB_SELITEMRANGE);
}

NODECODEPARM(LB_SETCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETCURSEL)
{
	PARM(index, INT, WP);

	POUT(index);
	if (index == -1)
	{
		MOUT << ids(IDS_NO_SELECTION);
	}

	return TRUE;
}

DECODERET(LB_SETCURSEL)
{
	PARM(index, INT, WP);
	PARM(lResult, UINT, RET);

	PLABEL(lResult);
	if (index == -1 && lResult == (UINT)LB_ERR)
	{
		MOUT << "-1" << ids(IDS_SUCCESSFULLY_REMOVED_SELECT);
	}
	else
	{
		TableOrValue(tblLBRetOK, lResult);
	}

	return TRUE;
}

NODECODEPARM(LB_SETCURSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETHORIZONTALEXTENT)
{
	PARM(cxExtent, INT, WP);

	POUT(cxExtent);

	return TRUE;
}

NODECODERET(LB_SETHORIZONTALEXTENT);

NODECODEPARM(LB_SETHORIZONTALEXTENT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETITEMDATA)
{
	PARM(index, INT, WP);
	PARM(dwData, DWORD, LP);

	POUT(index);
	POUT(dwData);
	MOUT << " (" << (INT)dwData << ')';

	return TRUE;
}

DECODERET(LB_SETITEMDATA)
{
	DECODERETLIKE(LB_SELITEMRANGE);
}

NODECODEPARM(LB_SETITEMDATA);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETITEMHEIGHT)
{
	PARM(index, INT, WP);
	PARM(cyItem, INT, LP);

	POUT(index);
	POUT(cyItem);

	return TRUE;
}

DECODERET(LB_SETITEMHEIGHT)
{
	DECODERETLIKE(LB_SELITEMRANGE);
}

NODECODEPARM(LB_SETITEMHEIGHT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETLOCALE)
{
	PARM(dwLocaleID, DWORD, WP);

	POUT(dwLocaleID);

	return TRUE;
}

DECODERET(LB_SETLOCALE)
{
	PARM(dwPrevLocaleID, DWORD, RET);

	TABLEORDWORD(tblLBRet, dwPrevLocaleID);

	return TRUE;
}

NODECODEPARM(LB_SETLOCALE);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETSEL)
{
	PARM(fSelect, BOOL, WP);
	PARM(index, INT, LP);

	POUTB(fSelect);
	POUT(index);
	if (index == -1)
	{
		MOUT << ids(IDS_ALL_ITEMS);
	}

	return TRUE;
}

DECODERET(LB_SETSEL)
{
	DECODERETLIKE(LB_SELITEMRANGE);
}

NODECODEPARM(LB_SETSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETTABSTOPS)
{
	PARM(cTabs, INT, WP);
	PARM(lpnTabs, LPINT, LP);

	POUT(cTabs);
	POUTC(lpnTabs, DWORD);

	PIFDATA()
	{
		PARM(pTabs, PINT, ED);

		PrintIntArray(pTabs, PSIZEOFED() / sizeof(INT), cTabs);
	}

	return TRUE;
}

DECODERET(LB_SETTABSTOPS)
{
	PARM(fSuccess, BOOL, RET);

	POUTB(fSuccess);

	return TRUE;
}

DECODEPARM(LB_SETTABSTOPS)
{
	P2WPOUT();

	P2LPOUTPTR(INT array);
	P2IFDATA()
	{
		PARM(pTabs, PINT, ED2);
		PARM(cTabs, INT, WP2);
		INDENT();
		PrintIntArray2(pmps, pTabs, P2SIZEOFED() / sizeof(INT), cTabs);
		P2ENDLINE();
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LB_SETTOPINDEX)
{
	DECODELIKE(LB_GETITEMDATA);
}

DECODERET(LB_SETTOPINDEX)
{
	DECODERETLIKE(LB_SELITEMRANGE);
}

NODECODEPARM(LB_SETTOPINDEX);

//////////////////////////////////////////////////////////////////////////////
