//-----------------------------------------------------------------------------
//  EditMsgs.cpp
//
//  Edit message (EM_*) crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

static VALUETABLE tblVScrollCode[] =
{
	TABLEENTRY(SB_LINEUP),
	TABLEENTRY(SB_LINEDOWN),
	TABLEENTRY(SB_PAGEUP),
	TABLEENTRY(SB_PAGEDOWN),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_CANUNDO);

DECODERET(EM_CANUNDO)
{
	PARM(fCanUndo, BOOL, RET);

	POUTB(fCanUndo);

	return TRUE;
}

NODECODEPARM(EM_CANUNDO);

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_EMPTYUNDOBUFFER);

NODECODERET(EM_EMPTYUNDOBUFFER);

NODECODEPARM(EM_EMPTYUNDOBUFFER);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_FMTLINES)
{
	PARM(fAddEOL, BOOL, WP);

	POUTB(fAddEOL);

	return TRUE;
}

NODECODERET(EM_FMTLINES);

NODECODEPARM(EM_FMTLINES);

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_GETFIRSTVISIBLELINE);

DECODERET(EM_GETFIRSTVISIBLELINE)
{
	PARM(iFirstVisibleLine, INT, RET);

	POUT(iFirstVisibleLine);

	return TRUE;
}

NODECODEPARM(EM_GETFIRSTVISIBLELINE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_GETHANDLE);

DECODERET(EM_GETHANDLE)
{
	PARM(hloc, HLOCAL, RET);

	POUTC(hloc, DWORD);

	return TRUE;
}

NODECODEPARM(EM_GETHANDLE);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_GETLINE)
{
	PARM(line, INT, WP);
	PARM(lpch, LPCSTR, LP);

	POUT(line);
	POUTC(lpch, DWORD);
	PIFDATA()
	{
		//
		// There will be one word in the extra data that is
		// the maximum characters to copy.  Note that the
		// format of the extra data changes for the ret cracker
		// of this message below, however!
		//
		PARM(pw, LPWORD, ED);
		PLABEL(cchMax);
		MOUT << (INT)(UINT)*pw;
	}

	return TRUE;
}

DECODERET(EM_GETLINE)
{
	PARM(cchCopied, INT, RET);

	POUT(cchCopied);

	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

DECODEPARM(EM_GETLINE)
{
	P2WPOUT();

	P2LPOUTPTRSTR;

	//
	// Only look for the string on a return.  The extra data that
	// will be there on a send/post is the cchMax of the buffer in
	// a single WORD.  We don't bother to show this in the parm
	// cracker here, however.
	//
	P2IFRETURN()
	{
		P2IFDATA()
		{
			PARM(pstr, LPSTR, ED2);
			INDENT();
			P2OUTS(pstr);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_GETLINECOUNT);

DECODERET(EM_GETLINECOUNT)
{
	PARM(cLines, INT, RET);

	POUT(cLines);

	return TRUE;
}

NODECODEPARM(EM_GETLINECOUNT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_GETMODIFY);

DECODERET(EM_GETMODIFY)
{
	PARM(fModified, BOOL, RET);

	POUTB(fModified);

	return TRUE;
}

NODECODEPARM(EM_GETMODIFY);

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_GETPASSWORDCHAR);

DECODERET(EM_GETPASSWORDCHAR)
{
	PARM(ch, CHAR, RET);

	PLABEL(ch);
	if (ch)
	{
		MOUT << '\'' << ch << "' (" << (INT)(UCHAR)ch << ')';
	}
	else
	{
		MOUT << '0' << ids(IDS_NO_PASSWORD_CHARACTER);
	}

	return TRUE;
}

NODECODEPARM(EM_GETPASSWORDCHAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_GETRECT)
{
	PARM(lprc, LPRECT, LP);

	POUTC(lprc, DWORD);

	return TRUE;
}

DECODERET(EM_GETRECT)
{
	PARM(lprc, LPRECT, LP);

	POUTC(lprc, DWORD);
	PIFSIZEOF(RECT)
	{
		PARM(prc, LPRECT, ED);
		POUTRECT(prc);
	}

	return TRUE;
}

DECODEPARM(EM_GETRECT)
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

DECODE(EM_GETSEL)
{
	PARM(lpdwStart, LPDWORD, WP);
	PARM(lpdwEnd, LPDWORD, LP);

	POUTC(lpdwStart, DWORD);
	POUTC(lpdwEnd, DWORD);

	return TRUE;
}

DECODERET(EM_GETSEL)
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

DECODEPARM(EM_GETSEL)
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

NODECODE(EM_GETTHUMB);

NODECODERET(EM_GETTHUMB);

NODECODEPARM(EM_GETTHUMB);

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_GETWORDBREAKPROC);

DECODERET(EM_GETWORDBREAKPROC)
{
	PARM(ewbprc, EDITWORDBREAKPROC, RET);

	POUTC(ewbprc, DWORD);

	return TRUE;
}

NODECODEPARM(EM_GETWORDBREAKPROC);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_LIMITTEXT)
{
	PARM(cchMax, UINT, WP);

	POUT(cchMax);

	return TRUE;
}

NODECODERET(EM_LIMITTEXT);

NODECODEPARM(EM_LIMITTEXT);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_LINEFROMCHAR)
{
	PARM(ich, INT, WP);

	POUT(ich);

	if (ich == -1)
	{
		MOUT << ids(IDS_CURRENT_LINE);
	}

	return TRUE;
}

DECODERET(EM_LINEFROMCHAR)
{
	PARM(iLine, INT, RET);

	POUT(iLine);

	return TRUE;
}

NODECODEPARM(EM_LINEFROMCHAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_LINEINDEX)
{
	PARM(line, INT, WP);

	POUT(line);

	if (line == -1)
	{
		MOUT << ids(IDS_CURRENT_LINE);
	}

	return TRUE;
}

DECODERET(EM_LINEINDEX)
{
	PARM(ich, INT, RET);

	POUT(ich);

	if (ich == -1)
	{
		MOUT << ids(IDS_LINE_IS_OUT_OF_RANGE);
	}

	return TRUE;
}

NODECODEPARM(EM_LINEINDEX);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_LINELENGTH)
{
	PARM(ich, INT, WP);

	POUT(ich);

	return TRUE;
}

DECODERET(EM_LINELENGTH)
{
	PARM(cchLine, INT, RET);

	POUT(cchLine);

	return TRUE;
}

NODECODEPARM(EM_LINELENGTH);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_LINESCROLL)
{
	PARM(cxScroll, INT, WP);
	PARM(cyScroll, INT, LP);

	POUT(cxScroll);
	POUT(cyScroll);

	return TRUE;
}

DECODERET(EM_LINESCROLL)
{
	PARM(fMultiLineEditCtrl, BOOL, RET);

	POUTB(fMultiLineEditCtrl);

	return TRUE;
}

NODECODEPARM(EM_LINESCROLL);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_REPLACESEL)
{
	PARM(lpszReplace, LPCTSTR, LP);

	POUTC(lpszReplace, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

NODECODERET(EM_REPLACESEL);

DECODEPARM(EM_REPLACESEL)
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

DECODE(EM_SCROLL)
{
	PARM(nScroll, INT, WP);

	TABLEORVALUE(tblVScrollCode, nScroll);

	return TRUE;
}

DECODERET(EM_SCROLL)
{
	PARM(fSuccess, BOOL, HIRET);
	PARM(nScrolled, UINT, LORET);

	POUTB(fSuccess);
	if (fSuccess)
	{
		POUTC(nScrolled, INT);
	}

	return TRUE;
}

NODECODEPARM(EM_SCROLL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_SCROLLCARET);

DECODERET(EM_SCROLLCARET)
{
	PARM(fSentToEditCtrl, BOOL, RET);

	POUTB(fSentToEditCtrl);

	return TRUE;
}

NODECODEPARM(EM_SCROLLCARET);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_SETHANDLE)
{
	PARM(hloc, HLOCAL, WP);

	POUTC(hloc, DWORD);

	return TRUE;
}

NODECODERET(EM_SETHANDLE);

NODECODEPARM(EM_SETHANDLE);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_SETMODIFY)
{
	PARM(fModified, BOOL, WP);

	POUTB(fModified);

	return TRUE;
}

NODECODERET(EM_SETMODIFY);

NODECODEPARM(EM_SETMODIFY);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_SETPASSWORDCHAR)
{
	PARM(ch, CHAR, WP);

	PLABEL(ch);
	if (ch == 0)
	{
		MOUT << '0' << ids(IDS_NONE);
	}
	else
	{
		MOUT << '\'' << ch << "' (" << (INT)(UCHAR)ch << ')';
	}

	return TRUE;
}

NODECODERET(EM_SETPASSWORDCHAR);

NODECODEPARM(EM_SETPASSWORDCHAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_SETREADONLY)
{
	PARM(fReadOnly, BOOL, WP);

	POUTB(fReadOnly);

	return TRUE;
}

DECODERET(EM_SETREADONLY)
{
	PARM(fSuccess, BOOL, RET);

	POUTB(fSuccess);

	return TRUE;
}

NODECODEPARM(EM_SETREADONLY);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_SETRECT)
{
	PARM(lprc, LPRECT, LP);

	POUTC(lprc, DWORD);
	PIFSIZEOF(RECT)
	{
		PARM(prc, LPRECT, ED);
		POUTRECT(prc);
	}

	return TRUE;
}

NODECODERET(EM_SETRECT);

DECODEPARM(EM_SETRECT)
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

DECODE(EM_SETRECTNP)
{
	DECODELIKE(EM_SETRECT);
}

NODECODERET(EM_SETRECTNP);

DECODEPARM(EM_SETRECTNP)
{
	DECODEPARMLIKE(EM_SETRECT);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_SETSEL)
{
	PARM(nStart, INT, WP);
	PARM(nEnd, INT, LP);

	POUT(nStart);
	POUT(nEnd);

	if (nStart == 0 && nEnd == -1)
	{
		MOUT << ids(IDS_SELECT_ALL_TEXT);
	}

	return TRUE;
}

NODECODERET(EM_SETSEL);

NODECODEPARM(EM_SETSEL);

//////////////////////////////////////////////////////////////////////////////

DECODE(EM_SETTABSTOPS)
{
	PARM(cTabs, INT, WP);
	PARM(lpnTabs, PUINT, LP);

	POUT(cTabs);

	if (cTabs)
	{
		POUTC(lpnTabs, DWORD);
		PIFDATA()
		{
			PARM(pTabs, PINT, ED);

			PrintIntArray(pTabs, PSIZEOFED() / sizeof(INT), cTabs);
		}
	}
	else
	{
		MOUT << ids(IDS_SET_TABS_TO_DEFAULT);
	}

	return TRUE;
}

DECODERET(EM_SETTABSTOPS)
{
	PARM(fSuccess, BOOL, RET);

	POUTB(fSuccess);

	return TRUE;
}

DECODEPARM(EM_SETTABSTOPS)
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

DECODE(EM_SETWORDBREAKPROC)
{
	PARM(ewbprc, EDITWORDBREAKPROC, LP);

	POUTC(ewbprc, DWORD);

	return TRUE;
}

NODECODERET(EM_SETWORDBREAKPROC);

NODECODEPARM(EM_SETWORDBREAKPROC);

//////////////////////////////////////////////////////////////////////////////

NODECODE(EM_UNDO);

DECODERET(EM_UNDO)
{
	PARM(fSuccess, BOOL, RET);

	POUTB(fSuccess);

	return TRUE;
}

NODECODEPARM(EM_UNDO);

//////////////////////////////////////////////////////////////////////////////
