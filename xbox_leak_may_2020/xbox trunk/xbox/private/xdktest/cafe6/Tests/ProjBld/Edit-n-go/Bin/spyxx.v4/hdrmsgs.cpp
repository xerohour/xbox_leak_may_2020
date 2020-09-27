//-----------------------------------------------------------------------------
//  HdrMsgs.cpp
//
//  System header (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef DISABLE_WIN95_MESSAGES
//////////////////////////////////////////////////////////////////////////////

NODECODE(HDM_GETITEMCOUNT);

DECODERET(HDM_GETITEMCOUNT)
{
	PARM(nCount, INT, RET);
	POUTC(nCount, INT);

	return TRUE;
}

NODECODEPARM(HDM_GETITEMCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(HDM_INSERTITEM)
{
	PARM(index, UINT, WP);
	PARM(phdi, HD_ITEM FAR *, LP);

	POUTC(index, INT);
	POUTC(phdi, DWORD);

	return TRUE;
}

DECODERET(HDM_INSERTITEM)
{
	PARM(nIndex, INT, RET);
	POUTC(nIndex, INT);

	return TRUE;
}

DECODEPARM(HDM_INSERTITEM)
{
	P2WPOUT();

	P2LPOUTPTR(HD_ITEM);
	P2IFSIZEOF(HD_ITEM)
	{
		PARM(phdi, HD_ITEM FAR *, ED2);
//		PARM(pszItem, LPSTR, phdi->pszText);
		INDENT();
		P2OUTC(phdi, mask, UINT);
		P2OUTC(phdi, cxy, INT);
		P2OUTPTRSTR(phdi, pszText);
//		INDENT();
//		P2OUTS(pszItem);
//		OUTDENT();
		P2OUTC(phdi, hbm, DWORD);
		P2OUTC(phdi, cchTextMax, INT);
		P2OUTC(phdi, fmt, INT);
		P2OUTC(phdi, lParam, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(HDM_DELETEITEM)
{
	PARM(index, UINT, WP);
	PARM(phdi, HD_ITEM FAR *, LP);

	POUTC(index, INT);
	POUTC(phdi, DWORD);

	return TRUE;
}

DECODERET(HDM_DELETEITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(HDM_DELETEITEM)
{
	P2WPOUT();

	P2LPOUTPTR(HD_ITEM);
	P2IFSIZEOF(HD_ITEM)
	{
		PARM(phdi, HD_ITEM FAR *, ED2);
//		PARM(pszItem, LPSTR, phdi->pszText);
		INDENT();
		P2OUTC(phdi, mask, UINT);
		P2OUTC(phdi, cxy, INT);
		P2OUTPTRSTR(phdi, pszText);
//		INDENT();
//		P2OUTS(pszItem);
//		OUTDENT();
		P2OUTC(phdi, hbm, DWORD);
		P2OUTC(phdi, cchTextMax, INT);
		P2OUTC(phdi, fmt, INT);
		P2OUTC(phdi, lParam, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(HDM_GETITEM)
{
	DECODELIKE(HDM_SETITEM);
}

DECODERET(HDM_GETITEM)
{
	DECODERETLIKE(HDM_SETITEM);
}

DECODEPARM(HDM_GETITEM)
{
	DECODEPARMLIKE(HDM_SETITEM);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(HDM_SETITEM)
{
	PARM(index, UINT, WP);
	PARM(phdi, HD_ITEM FAR *, LP);

	POUTC(index, INT);
	POUTC(phdi, DWORD);

	return TRUE;
}

DECODERET(HDM_SETITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(HDM_SETITEM)
{
	P2WPOUT();

	P2LPOUTPTR(HD_ITEM);
	P2IFSIZEOF(HD_ITEM)
	{
		PARM(phdi, HD_ITEM FAR *, ED2);
//		PARM(pszItem, LPSTR, phdi->pszText);
		INDENT();
		P2OUTC(phdi, mask, UINT);
		P2OUTC(phdi, cxy, INT);
		P2OUTPTRSTR(phdi, pszText);
//		INDENT();
//		P2OUTS(pszItem);
//		OUTDENT();
		P2OUTC(phdi, hbm, DWORD);
		P2OUTC(phdi, cchTextMax, INT);
		P2OUTC(phdi, fmt, INT);
		P2OUTC(phdi, lParam, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(HDM_LAYOUT)
{
	PARM(playout, HD_LAYOUT FAR *, LP);

	POUTC(playout, DWORD);

	return TRUE;
}

DECODERET(HDM_LAYOUT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(HDM_LAYOUT)
{
	P2WPOUT();

	P2LPOUTPTR(HD_LAYOUT);
	P2IFSIZEOF(HD_LAYOUT)
	{
		PARM(playout, HD_LAYOUT FAR *, ED2);
		INDENT();
		P2OUTPTR(playout, prc, RECT);
		P2OUTPTR(playout, pwpos, WINDOWPOS);
//		M2OUT << '(' << playout->prc->left << ',' << playout->prc->top << ") - ("
//				<< playout->prc->right << ',' << playout->prc->bottom << ')';							
//		// REVIEW: crack WINDOWPOS structure here
//		P2OUTC(playout, pwpos, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(HDM_HITTEST)
{
	PARM(phdhti, HD_HITTESTINFO FAR *, LP);

	POUTC(phdhti, DWORD);

	return TRUE;
}

DECODERET(HDM_HITTEST)
{
	PARM(nIndex, INT, RET);
	POUTC(nIndex, INT);

	return TRUE;
}

DECODEPARM(HDM_HITTEST)
{
	P2WPOUT();

	P2LPOUTPTR(HD_HITTESTINFO);
	P2IFSIZEOF(HD_HITTESTINFO)
	{
		PARM(phdhti, HD_HITTESTINFO FAR *, ED2);
		INDENT();
		P2OUTPT(phdhti, pt);
		P2OUTC(phdhti, flags, UINT);
		P2OUTC(phdhti, iItem, INT);
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(HDM_INSERTITEMW);

NODECODERET(HDM_INSERTITEMW);

NODECODEPARM(HDM_INSERTITEMW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(HDM_GETITEMW);

NODECODERET(HDM_GETITEMW);

NODECODEPARM(HDM_GETITEMW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(HDM_SETITEMW);

NODECODERET(HDM_SETITEMW);

NODECODEPARM(HDM_SETITEMW);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
