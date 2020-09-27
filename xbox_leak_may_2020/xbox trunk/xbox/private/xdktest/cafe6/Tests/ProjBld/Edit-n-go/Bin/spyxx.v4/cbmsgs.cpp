//-----------------------------------------------------------------------------
//  CBMsgs.cpp
//
//  Combobox message (CB_*) crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

VALUETABLE tblCBRet[] =
{
	TABLEENTRY(CB_ERR),
	TABLEENTRY(CB_ERRSPACE),
	TABLEEND
};

static VALUETABLE tblCBRetOK[] =
{
	TABLEENTRY(CB_OKAY),
	TABLEENTRY(CB_ERR),
	TABLEENTRY(CB_ERRSPACE),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_ADDSTRING)
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

DECODERET(CB_ADDSTRING)
{
	PARM(index, INT, RET);

	TABLEORINT(tblCBRet, index);

	return TRUE;
}

DECODEPARM(CB_ADDSTRING)
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

DECODE(CB_DELETESTRING)
{
	PARM(index, INT, WP);

	POUT(index);

	return TRUE;
}

DECODERET(CB_DELETESTRING)
{
	PARM(cItemsRemaining, INT, RET);

	TABLEORINT(tblCBRet, cItemsRemaining);

	return TRUE;
}

NODECODEPARM(CB_DELETESTRING);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_DIR)
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

DECODERET(CB_DIR)
{
	DECODERETLIKE(CB_ADDSTRING);
}

DECODEPARM(CB_DIR)
{
	DECODEPARMLIKE(CB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_FINDSTRING)
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

DECODERET(CB_FINDSTRING)
{
	DECODERETLIKE(CB_ADDSTRING);
}

DECODEPARM(CB_FINDSTRING)
{
	DECODEPARMLIKE(CB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_FINDSTRINGEXACT)
{
	DECODELIKE(CB_FINDSTRING);
}

DECODERET(CB_FINDSTRINGEXACT)
{
	DECODERETLIKE(CB_FINDSTRING);
}

DECODEPARM(CB_FINDSTRINGEXACT)
{
	DECODEPARMLIKE(CB_FINDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(CB_GETCOUNT);

DECODERET(CB_GETCOUNT)
{
	PARM(cItems, INT, RET);

	TABLEORINT(tblCBRet, cItems);

	return TRUE;
}

NODECODEPARM(CB_GETCOUNT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(CB_GETCURSEL);

DECODERET(CB_GETCURSEL)
{
	DECODERETLIKE(CB_ADDSTRING);
}

NODECODEPARM(CB_GETCURSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_GETDROPPEDCONTROLRECT)
{
	PARM(lprc, LPRECT, LP);

	POUTC(lprc, DWORD);

	return TRUE;
}

DECODERET(CB_GETDROPPEDCONTROLRECT)
{
	PARM(lResult, UINT, RET);
	PARM(lprc, LPRECT, LP);

	TABLEORVALUE(tblCBRetOK, lResult);

	if (lResult != CB_ERR)
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

DECODEPARM(CB_GETDROPPEDCONTROLRECT)
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

NODECODE(CB_GETDROPPEDSTATE);

DECODERET(CB_GETDROPPEDSTATE)
{
	PARM(fDropped, BOOL, RET);

	POUTB(fDropped);

	return TRUE;
}

NODECODEPARM(CB_GETDROPPEDSTATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_GETEDITSEL)
{
	PARM(lpdwStart, DWORD, WP);
	PARM(lpdwEnd, DWORD, LP);

	POUT(lpdwStart);
	POUT(lpdwEnd);

	return TRUE;
}

DECODERET(CB_GETEDITSEL)
{
	PARM(wStart, UINT, LORET);
	PARM(wEnd, UINT, HIRET);
	PARM(lpdwStart, DWORD, WP);
	PARM(lpdwEnd, DWORD, LP);

	POUT(wStart);
	POUT(wEnd);

	PIFSIZEOF(PACK_WPLPDWORDS)
	{
		PARM(ppwld, PPACK_WPLPDWORDS, ED);

		POUTC(lpdwStart, DWORD);
		if (ppwld->fValidWP)
		{
			MOUT << " (" << (INT)ppwld->nWP << ')';
		}

		POUTC(lpdwEnd, DWORD);
		if (ppwld->fValidLP)
		{
			MOUT << " (" << (INT)ppwld->nLP << ')';
		}
	}
	else
	{
		POUTC(lpdwStart, DWORD);
		POUTC(lpdwEnd, DWORD);
	}

	return TRUE;
}

DECODEPARM(CB_GETEDITSEL)
{
	P2IFSIZEOF(PACK_WPLPDWORDS)
	{
		PARM(ppwld, PPACK_WPLPDWORDS, ED2);

		if (ppwld->fValidWP)
		{
			P2WPOUTPTR(DWORD);
			INDENT();
			M2OUT << (INT)ppwld->nWP;
			P2ENDLINE();
			OUTDENT();
		}
		else
		{
			P2WPOUT();
		}

		if (ppwld->fValidLP)
		{
			P2LPOUTPTR(DWORD);
			INDENT();
			M2OUT << (INT)ppwld->nLP;
			P2ENDLINE();
			OUTDENT();
		}
		else
		{
			P2WPOUT();
		}
	}
	else
	{
		P2WPOUT();
		P2LPOUT();
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(CB_GETEXTENDEDUI);

DECODERET(CB_GETEXTENDEDUI)
{
	PARM(fExtended, BOOL, RET);

	POUTB(fExtended);

	return FALSE;
}

NODECODEPARM(CB_GETEXTENDEDUI);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_GETITEMDATA)
{
	PARM(index, INT, WP);

	POUT(index);

	return TRUE;
}

DECODERET(CB_GETITEMDATA)
{
	PARM(dwData, DWORD, RET);

	TABLEORDWORD(tblCBRet, dwData);

	return TRUE;
}

NODECODEPARM(CB_GETITEMDATA);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_GETITEMHEIGHT)
{
	PARM(index, INT, WP);

	POUT(index);
	if (index == -1)
	{
		MOUT << ids(IDS_THE_SELECTION_FIELD);
	}

	return TRUE;
}

DECODERET(CB_GETITEMHEIGHT)
{
	PARM(cyItem, INT, RET);

	TABLEORINT(tblCBRet, cyItem);

	return TRUE;
}

NODECODEPARM(CB_GETITEMHEIGHT);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_GETLBTEXT)
{
	PARM(index, INT, WP);
	PARM(lpszBuffer, LPCTSTR, LP);

	POUT(index);
	POUTC(lpszBuffer, DWORD);

	return TRUE;
}

DECODERET(CB_GETLBTEXT)
{
	PARM(cchText, INT, RET);
	PARM(lpszBuffer, LPCTSTR, LP);

	TABLEORINT(tblCBRet, cchText);
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

DECODEPARM(CB_GETLBTEXT)
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

DECODE(CB_GETLBTEXTLEN)
{
	DECODELIKE(CB_GETITEMDATA);
}

DECODERET(CB_GETLBTEXTLEN)
{
	PARM(cchText, INT, RET);

	TABLEORINT(tblCBRet, cchText);

	return TRUE;
}

NODECODEPARM(CB_GETLBTEXTLEN);

//////////////////////////////////////////////////////////////////////////////

NODECODE(CB_GETLOCALE);

DECODERET(CB_GETLOCALE)
{
	PARM(dwLocaleID, DWORD, RET);

	POUT(dwLocaleID);

	return TRUE;
}

NODECODEPARM(CB_GETLOCALE);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_INSERTSTRING)
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

DECODERET(CB_INSERTSTRING)
{
	DECODERETLIKE(CB_GETITEMDATA);
}

DECODEPARM(CB_INSERTSTRING)
{
	DECODEPARMLIKE(CB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_LIMITTEXT)
{
	PARM(cchLimit, UINT, WP);

	POUT(cchLimit);

	return TRUE;
}

NODECODERET(CB_LIMITTEXT);

NODECODEPARM(CB_LIMITTEXT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(CB_RESETCONTENT);

NODECODERET(CB_RESETCONTENT);

NODECODEPARM(CB_RESETCONTENT);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_SELECTSTRING)
{
	DECODELIKE(CB_FINDSTRING);
}

DECODERET(CB_SELECTSTRING)
{
	DECODERETLIKE(CB_GETITEMDATA);
}

DECODEPARM(CB_SELECTSTRING)
{
	DECODEPARMLIKE(CB_ADDSTRING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_SETCURSEL)
{
	PARM(index, INT, WP);

	POUT(index);
	if (index == -1)
	{
		MOUT << ids(IDS_NO_SELECTION);
	}

	return TRUE;
}

DECODERET(CB_SETCURSEL)
{
	PARM(index, INT, WP);
	PARM(lResult, UINT, RET);

	PLABEL(lResult);
	if (index == -1 && lResult == (UINT)CB_ERR)
	{
		MOUT << "-1" << ids(IDS_SUCCESSFULLY_REMOVED_SELECT);
	}
	else
	{
		TableOrValue(tblCBRetOK, lResult);
	}

	return TRUE;
}

NODECODEPARM(CB_SETCURSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_SETEDITSEL)
{
	PARM(ichStart, INT, LOLP);
	PARM(ichEnd, INT, HILP);

	POUT(ichStart);
	if (ichStart == -1)
	{
		MOUT << ids(IDS_REMOVE_THE_SELECTION);
	}

	POUT(ichEnd);
	if (ichEnd == -1)
	{
		MOUT << ids(IDS_THE_LAST_CHARACTER);
	}

	return TRUE;
}

DECODERET(CB_SETEDITSEL)
{
	PARM(lResult, UINT, RET);

	TABLEORVALUE(tblCBRetOK, lResult);
	if (lResult == TRUE)
	{
		MOUT << ids(IDS_SUCCESS);
	}

	return TRUE;
}

NODECODEPARM(CB_SETEDITSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_SETEXTENDEDUI)
{
	PARM(fExtended, BOOL, WP);

	POUTB(fExtended);

	return FALSE;
}

DECODERET(CB_SETEXTENDEDUI)
{
	PARM(lResult, UINT, RET);

	TABLEORVALUE(tblCBRetOK, lResult);

	return TRUE;
}

NODECODEPARM(CB_SETEXTENDEDUI);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_SETITEMDATA)
{
	PARM(index, INT, WP);
	PARM(dwData, DWORD, LP);

	POUT(index);
	POUT(dwData);
	MOUT << " (" << (INT)dwData << ')';

	return TRUE;
}

DECODERET(CB_SETITEMDATA)
{
	DECODERETLIKE(CB_SETEXTENDEDUI);
}

NODECODEPARM(CB_SETITEMDATA);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_SETITEMHEIGHT)
{
	PARM(index, INT, WP);
	PARM(cyItem, INT, LP);

	POUT(index);
	if (index == -1)
	{
		MOUT << ids(IDS_THE_SELECTION_FIELD);
	}

	POUT(cyItem);

	return TRUE;
}

DECODERET(CB_SETITEMHEIGHT)
{
	DECODERETLIKE(CB_SETEXTENDEDUI);
}

NODECODEPARM(CB_SETITEMHEIGHT);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_SETLOCALE)
{
	PARM(dwLocaleID, DWORD, WP);

	POUT(dwLocaleID);

	return TRUE;
}

DECODERET(CB_SETLOCALE)
{
	PARM(dwPrevLocaleID, DWORD, RET);

	TABLEORDWORD(tblCBRet, dwPrevLocaleID);

	return TRUE;
}

NODECODEPARM(CB_SETLOCALE);

//////////////////////////////////////////////////////////////////////////////

DECODE(CB_SHOWDROPDOWN)
{
	PARM(fShow, BOOL, WP);

	POUTB(fShow);

	return TRUE;
}

NODECODERET(CB_SHOWDROPDOWN);

NODECODEPARM(CB_SHOWDROPDOWN);

//////////////////////////////////////////////////////////////////////////////
