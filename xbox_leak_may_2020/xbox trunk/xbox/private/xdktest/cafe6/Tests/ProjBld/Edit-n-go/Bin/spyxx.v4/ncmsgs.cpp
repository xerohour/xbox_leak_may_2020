//-----------------------------------------------------------------------------
//  NCMsgs.cpp
//
//  Non-Client message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////

static VALUETABLE tblValidRectFlags[] =
{
	TABLEENTRY(WVR_ALIGNTOP),
	TABLEENTRY(WVR_ALIGNLEFT),
	TABLEENTRY(WVR_ALIGNBOTTOM),
	TABLEENTRY(WVR_ALIGNRIGHT),
	TABLEENTRY(WVR_HREDRAW),
	TABLEENTRY(WVR_VREDRAW),
	TABLEENTRY(WVR_VALIDRECTS),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCACTIVATE)
{
	PARM(fActive, BOOL, WP);

	POUTB(fActive);

	return TRUE;
}

DECODERET(WM_NCACTIVATE)
{
	PARM(fActive, BOOL, WP);
	PARM(fDeactivateOK, BOOL, RET);

	if (fActive)
	{
		//
		// Print nothing.
		//
		return FALSE;
	}
	else
	{
		POUTB(fDeactivateOK);
		return TRUE;
	}
}

NODECODEPARM(WM_NCACTIVATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCCALCSIZE)
{
	PARM(fCalcValidRects, BOOL, WP);

	POUTB(fCalcValidRects);

	if (fCalcValidRects)
	{
		PARM(lpncsp, LPNCCALCSIZE_PARAMS, LP);
		POUTC(lpncsp, DWORD);
	}
	else
	{
		PARM(lprc, LPRECT, LP);
		POUTC(lprc, DWORD);
	}

	return TRUE;
}

DECODERET(WM_NCCALCSIZE)
{
	PARM(fuValidRect, UINT, RET);
	PARM(fCalcValidRects, BOOL, WP);

	TABLEFLAGS(tblValidRectFlags, fuValidRect);

	if (fCalcValidRects)
	{
		PARM(lpncsp, LPNCCALCSIZE_PARAMS, LP);
		POUTC(lpncsp, DWORD);
	}
	else
	{
		PARM(lprc, LPRECT, LP);
		POUTC(lprc, DWORD);
	}

	return TRUE;
}

DECODEPARM(WM_NCCALCSIZE)
{
	PARM(fCalcValidRects, BOOL, WP2);

	P2WPOUT();

	if (fCalcValidRects)
	{
		//
		// If fCalcValidRects is not TRUE, then lParam does NOT
		// point to a valid NCCALCSIZE_PARAMS structure, so we
		// don't want to label it as doing so (and we certainly
		// wouldn't want to dereference it in the hook!)
		//
		P2LPOUTPTR(NCCALCSIZE_PARAMS);
		P2IFSIZEOF(PACK_NCCALCSIZE)
		{
			PARM(ppack, PPACK_NCCALCSIZE, ED2);

			PARM(pnccp, LPNCCALCSIZE_PARAMS, &ppack->nccp);
			INDENT();
			P2OUTRECT(pnccp, rgrc[0]);
			P2OUTRECT(pnccp, rgrc[1]);
			P2OUTRECT(pnccp, rgrc[2]);

			P2OUTPTR(pnccp, lppos, WINDOWPOS);
			PARM(lppos, PWINDOWPOS, &ppack->wp);
			INDENT();
			P2OUT(lppos, hwnd);
			P2OUT(lppos, hwndInsertAfter);
			P2OUT(lppos, x);
			P2OUT(lppos, y);
			P2OUT(lppos, cx);
			P2OUT(lppos, cy);
			P2TABLEFLAGS(tblWindowPos, lppos, flags);
		}
	}
	else
	{
		//
		// If fCalcValidRects is FALSE, lParam points to a RECT.
		//
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
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCCREATE)
{
	PARM(lpcs, LPCREATESTRUCT, LP);

	POUTC(lpcs, DWORD);

	return TRUE;
}

DECODERET(WM_NCCREATE)
{
	PARM(fContinue, BOOL, RET);

	POUTB(fContinue);

	return TRUE;
}

DECODEPARM(WM_NCCREATE)
{
	DECODEPARMLIKE(WM_CREATE);
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_NCDESTROY);

NODECODERET(WM_NCDESTROY);

NODECODEPARM(WM_NCDESTROY);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCHITTEST)
{
	PARM(xPos, INT, (short)LOLP);
	PARM(yPos, INT, (short)HILP);

	POUT(xPos);
	POUT(yPos);

	return TRUE;
}

DECODERET(WM_NCHITTEST)
{
	PARM(nHittest, UINT, RET);

	TABLEORVALUE(tblHitTest, nHittest);

	return TRUE;
}

NODECODEPARM(WM_NCHITTEST);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCLBUTTONDBLCLK)
{
	PARM(nHittest, INT, WP);
	PARM(xPos, INT, (short)LOLP);
	PARM(yPos, INT, (short)HILP);

	TABLEORVALUEC(tblHitTest, WORD, nHittest);
	POUT(xPos);
	POUT(yPos);

	return TRUE;
}

NODECODERET(WM_NCLBUTTONDBLCLK);

NODECODEPARM(WM_NCLBUTTONDBLCLK);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCLBUTTONDOWN)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCLBUTTONDOWN);

NODECODEPARM(WM_NCLBUTTONDOWN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCLBUTTONUP)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCLBUTTONUP);

NODECODEPARM(WM_NCLBUTTONUP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCMBUTTONDBLCLK)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCMBUTTONDBLCLK);

NODECODEPARM(WM_NCMBUTTONDBLCLK);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCMBUTTONDOWN)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCMBUTTONDOWN);

NODECODEPARM(WM_NCMBUTTONDOWN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCMBUTTONUP)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCMBUTTONUP);

NODECODEPARM(WM_NCMBUTTONUP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCMOUSEMOVE)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCMOUSEMOVE);

NODECODEPARM(WM_NCMOUSEMOVE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCPAINT)
{
	PARM(hrgn, HRGN, WP);

	POUTC(hrgn, DWORD);

	return TRUE;
}

NODECODERET(WM_NCPAINT);

NODECODEPARM(WM_NCPAINT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCRBUTTONDBLCLK)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCRBUTTONDBLCLK);

NODECODEPARM(WM_NCRBUTTONDBLCLK);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCRBUTTONDOWN)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCRBUTTONDOWN);

NODECODEPARM(WM_NCRBUTTONDOWN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NCRBUTTONUP)
{
	DECODELIKE(WM_NCLBUTTONDBLCLK);
}

NODECODERET(WM_NCRBUTTONUP);

NODECODEPARM(WM_NCRBUTTONUP);

//////////////////////////////////////////////////////////////////////////////
