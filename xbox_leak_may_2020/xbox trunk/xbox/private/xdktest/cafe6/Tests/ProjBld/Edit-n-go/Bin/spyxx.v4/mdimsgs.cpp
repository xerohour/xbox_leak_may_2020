//-----------------------------------------------------------------------------
//  MDIMsgs.cpp
//
//  MDI message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

VALUETABLE tblMDITile[] =
{
	TABLEENTRY(MDITILE_SKIPDISABLED),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_CHILDACTIVATE);

NODECODERET(WM_CHILDACTIVATE);

NODECODEPARM(WM_CHILDACTIVATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDIACTIVATE)
{
	UINT nClass;

	PARM(hwnd, HWND, HW);

	PIFSIZEOF(PACK_CLASS)
	{
		PARM(ppc, PPACK_CLASS, ED);
		nClass = ppc->nClass;
	}
	else
	{
		nClass = CLS_UNKNOWN;
	}

	if (nClass == CLS_MDICLIENT)
	{
		PARM(hwndChildAct, HWND, WP);
		POUT(hwndChildAct);
	}
	else
	{
		PARM(hwndChildDeact, HWND, WP);
		PARM(hwndChildAct, HWND, LP);

		POUT(hwndChildDeact);
		POUT(hwndChildAct);

		if (hwndChildAct == hwnd)
		{
			MOUT << ids(IDS_ACTIVATING);
		}
	}

	return TRUE;
}

NODECODERET(WM_MDIACTIVATE);

NODECODEPARM(WM_MDIACTIVATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDICASCADE)
{
	PARM(fuCascade, UINT, WP);

	TABLEFLAGS(tblMDITile, fuCascade);

	return TRUE;
}

DECODERET(WM_MDICASCADE)
{
	PARM(fSuccess, BOOL, RET);

	POUTB(fSuccess);

	return TRUE;
}

NODECODEPARM(WM_MDICASCADE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDICREATE)
{
	PARM(lpmdic, LPMDICREATESTRUCT, LP);

	POUTC(lpmdic, DWORD);

	return TRUE;
}

DECODERET(WM_MDICREATE)
{
	PARM(hwndChild, HWND, RET);

	POUT(hwndChild);

	return TRUE;
}

DECODEPARM(WM_MDICREATE)
{
	P2WPOUT();

	P2LPOUTPTR(MDICREATESTRUCT);
	P2IFSIZEOF(PACK_MDICREATESTRUCT)
	{
		PARM(ppmdic, PPACK_MDICREATESTRUCT, ED2);
		PARM(pmdic, LPMDICREATESTRUCT, &ppmdic->mdic);

		INDENT();

		if (ppmdic->fValidClass)
		{
			PARM(pszClass, LPSTR, ppmdic->szClass);
			P2OUTPTRSTR(pmdic, szClass);
			INDENT();
			P2OUTS(pszClass);
			OUTDENT();
		}
		else
		{
			CHAR szClass[50];

			if (GetClipboardFormatName((UINT)pmdic->szClass, szClass,
				sizeof(szClass) / sizeof(CHAR)))
			{
				P2LABEL(szClass);
				M2OUT << (DWORD)pmdic->szClass << ids(IDS_CLASS) << szClass << "')";
				P2ENDLINE();
			}
			else
			{
				P2OUTC(pmdic, szClass, DWORD);
			}
		}

		if (ppmdic->fValidTitle)
		{
			PARM(pszTitle, LPSTR, ppmdic->szTitle);
			P2OUTPTRSTR(pmdic, szTitle);
			INDENT();
			P2OUTS(pszTitle);
			OUTDENT();
		}
		else
		{
			P2OUTC(pmdic, szTitle, DWORD);
		}

		P2OUTC(pmdic, hOwner, DWORD);
		P2TABLEORINT(tblCreateWindowPos, pmdic, x);
		P2TABLEORINT(tblCreateWindowPos, pmdic, y);
		P2TABLEORINT(tblCreateWindowPos, pmdic, cx);
		P2TABLEORINT(tblCreateWindowPos, pmdic, cy);
		P2TABLEFLAGS(tblWindowStyles, pmdic, style);
		P2OUTC(pmdic, lParam, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDIDESTROY)
{
	PARM(hwndChild, HWND, WP);

	POUT(hwndChild);

	return TRUE;
}

NODECODERET(WM_MDIDESTROY);

NODECODEPARM(WM_MDIDESTROY);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDIGETACTIVE)
{
	PARM(lpfMaximized, LPBOOL, LP);

	POUTC(lpfMaximized, DWORD);

	return TRUE;
}

DECODERET(WM_MDIGETACTIVE)
{
	PARM(hwndActiveChild, HWND, RET);
	PARM(lpfMaximized, LPBOOL, LP);

	POUT(hwndActiveChild);
	POUTC(lpfMaximized, DWORD);
	PIFDATA()
	{
		PARM(fMaximized, LPBOOL, ED);
		POUTPB(fMaximized);
	}

	return TRUE;
}

NODECODEPARM(WM_MDIGETACTIVE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_MDIICONARRANGE);

NODECODERET(WM_MDIICONARRANGE);

NODECODEPARM(WM_MDIICONARRANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDIMAXIMIZE)
{
	PARM(hwndMax, HWND, WP);

	POUT(hwndMax);

	return TRUE;
}

NODECODERET(WM_MDIMAXIMIZE);

NODECODEPARM(WM_MDIMAXIMIZE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDINEXT)
{
	PARM(hwndChild, HWND, WP);
	PARM(fPrev, BOOL, LP);

	POUT(hwndChild);
	POUTB(fPrev);

	return TRUE;
}

NODECODERET(WM_MDINEXT);

NODECODEPARM(WM_MDINEXT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_MDIREFRESHMENU);

DECODERET(WM_MDIREFRESHMENU)
{
	PARM(hmenuFrame, HMENU, RET);

	POUTC(hmenuFrame, DWORD);

	return TRUE;
}

NODECODEPARM(WM_MDIREFRESHMENU);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDIRESTORE)
{
	PARM(hwndRes, HWND, WP);

	POUT(hwndRes);

	return TRUE;
}

NODECODERET(WM_MDIRESTORE);

NODECODEPARM(WM_MDIRESTORE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDISETMENU)
{
	PARM(hmenuFrame, HMENU, WP);
	PARM(hmenuWindow, HMENU, LP);

	POUTC(hmenuFrame, DWORD);
	POUTC(hmenuWindow, DWORD);

	return TRUE;
}

DECODERET(WM_MDISETMENU)
{
	PARM(hmenuFrameOld, HMENU, RET);

	POUTC(hmenuFrameOld, DWORD);

	return TRUE;
}

NODECODEPARM(WM_MDISETMENU);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MDITILE)
{
	PARM(fuTile, UINT, WP);

	PLABEL(fuTile);
	if (fuTile & MDITILE_HORIZONTAL)
	{
		MOUT << "MDITILE_HORIZONTAL";
	}
	else
	{
		MOUT << "MDITILE_VERTICAL";
	}

	if (fuTile & MDITILE_SKIPDISABLED)
	{
		MOUT << " | MDITILE_SKIPDISABLED";
	}

	return TRUE;
}

DECODERET(WM_MDITILE)
{
	PARM(fSuccess, BOOL, RET);

	POUTB(fSuccess);

	return TRUE;
}

NODECODEPARM(WM_MDITILE);

//////////////////////////////////////////////////////////////////////////////
