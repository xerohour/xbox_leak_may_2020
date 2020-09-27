//-----------------------------------------------------------------------------
//  ClipMsgs.cpp
//
//  Clipboard message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

VALUETABLE tblCBFormat[] =
{
	TABLEENTRY(CF_BITMAP),
	TABLEENTRY(CF_DIB),
	TABLEENTRY(CF_DIF),
	TABLEENTRY(CF_DSPBITMAP),
	TABLEENTRY(CF_DSPENHMETAFILE),
	TABLEENTRY(CF_DSPMETAFILEPICT),
	TABLEENTRY(CF_DSPTEXT),
	TABLEENTRY(CF_ENHMETAFILE),
	TABLEENTRY(CF_METAFILEPICT),
	TABLEENTRY(CF_OEMTEXT),
	TABLEENTRY(CF_OWNERDISPLAY),
	TABLEENTRY(CF_PALETTE),
	TABLEENTRY(CF_PENDATA),
	TABLEENTRY(CF_RIFF),
	TABLEENTRY(CF_SYLK),
	TABLEENTRY(CF_TEXT),
	TABLEENTRY(CF_TIFF),
	TABLEENTRY(CF_UNICODETEXT),
	TABLEENTRY(CF_WAVE),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ASKCBFORMATNAME)
{
	PARM(cchName, INT, WP);
	PARM(lpszFormatName, LPTSTR, LP);

	POUT(cchName);
	POUTC(lpszFormatName, DWORD);

	return TRUE;
}

DECODERET(WM_ASKCBFORMATNAME)
{
	PARM(lpszFormatName, LPTSTR, LP);

	POUTC(lpszFormatName, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

DECODEPARM(WM_ASKCBFORMATNAME)
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

DECODE(WM_CHANGECBCHAIN)
{
	PARM(hwndRemove, HWND, WP);
	PARM(hwndNext, HWND, LP);

	POUT(hwndRemove);
	POUT(hwndNext);

	return TRUE;
}

NODECODERET(WM_CHANGECBCHAIN);

NODECODEPARM(WM_CHANGECBCHAIN);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_CLEAR);

NODECODERET(WM_CLEAR);

NODECODEPARM(WM_CLEAR);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_COPY);

NODECODERET(WM_COPY);

NODECODEPARM(WM_COPY);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_CUT);

NODECODERET(WM_CUT);

NODECODEPARM(WM_CUT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_DESTROYCLIPBOARD);

NODECODERET(WM_DESTROYCLIPBOARD);

NODECODEPARM(WM_DESTROYCLIPBOARD);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_DRAWCLIPBOARD);

NODECODERET(WM_DRAWCLIPBOARD);

NODECODEPARM(WM_DRAWCLIPBOARD);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_HSCROLLCLIPBOARD)
{
	PARM(hwndViewer, HWND, WP);
	PARM(fuScrollCode, UINT, LOLP);
	PARM(nPos, INT, HILP);

	POUT(hwndViewer);
	TABLEORVALUE(tblHScroll, fuScrollCode);

	switch (fuScrollCode)
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			POUT(nPos);
			break;
	}

	return TRUE;
}

NODECODERET(WM_HSCROLLCLIPBOARD);

NODECODEPARM(WM_HSCROLLCLIPBOARD);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_PAINTCLIPBOARD)
{
	PARM(hwndViewer, HWND, WP);
	PARM(hglbPs, HGLOBAL, LP);

	POUT(hwndViewer);
	POUTC(hglbPs, DWORD);

	return TRUE;
}

NODECODERET(WM_PAINTCLIPBOARD);

DECODEPARM(WM_PAINTCLIPBOARD)
{
	P2WPOUT();

	P2LPOUTPTR(PAINTSTRUCT);
	P2IFSIZEOF(PAINTSTRUCT)
	{
		PARM(pps, LPPAINTSTRUCT, ED2);
		INDENT();
		P2OUTC(pps, hdc, DWORD);
		P2OUTB(pps, fErase);
		P2OUTRECT(pps, rcPaint);
		P2OUTB(pps, fRestore);
		P2OUTB(pps, fIncUpdate);

		P2LABEL(rgbReserved[32]);
		for (int i = 0; i < 32; i++)
		{
			CHAR sz[8];
			sprintf(sz, "%02X ", pps->rgbReserved[i]);
			M2OUT << sz;
		}
		P2ENDLINE();
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_PASTE);

NODECODERET(WM_PASTE);

NODECODEPARM(WM_PASTE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_RENDERALLFORMATS);

NODECODERET(WM_RENDERALLFORMATS);

NODECODEPARM(WM_RENDERALLFORMATS);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_RENDERFORMAT)
{
	PARM(uFormat, UINT, WP);

	TABLEORVALUE(tblCBFormat, uFormat);

	return TRUE;
}

NODECODERET(WM_RENDERFORMAT);

NODECODEPARM(WM_RENDERFORMAT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SIZECLIPBOARD)
{
	PARM(hwndViewer, HWND, WP);
	PARM(hglbPs, HGLOBAL, LP);

	POUT(hwndViewer);
	POUTC(hglbPs, DWORD);

	return TRUE;
}

NODECODERET(WM_SIZECLIPBOARD);

DECODEPARM(WM_SIZECLIPBOARD)
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

NODECODE(WM_UNDO);

DECODERET(WM_UNDO)
{
	PARM(fSuccess, BOOL, RET);

	POUT(fSuccess);

	return TRUE;
}

NODECODEPARM(WM_UNDO);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_VSCROLLCLIPBOARD)
{
	PARM(hwndViewer, HWND, WP);
	PARM(fuScrollCode, UINT, LOLP);
	PARM(nPos, INT, HILP);

	POUT(hwndViewer);
	TABLEORVALUE(tblVScroll, fuScrollCode);

	switch (fuScrollCode)
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			POUT(nPos);
			break;
	}

	return TRUE;
}

NODECODERET(WM_VSCROLLCLIPBOARD);

NODECODEPARM(WM_VSCROLLCLIPBOARD);

//////////////////////////////////////////////////////////////////////////////
