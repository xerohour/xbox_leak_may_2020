//-----------------------------------------------------------------------------
//  TVMsgs.cpp
//
//  TreeView (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static VALUETABLE tblTVRetOK[] =
{
	TABLEENTRY(LB_OKAY),
	TABLEENTRY(LB_ERR),
	TABLEENTRY(LB_ERRSPACE),
	TABLEEND
};

#ifndef DISABLE_WIN95_MESSAGES

static VALUETABLE tblTVExpand[] =
{
	TABLEENTRY(TVE_COLLAPSE),
	TABLEENTRY(TVE_COLLAPSERESET),
	TABLEENTRY(TVE_EXPAND),
	TABLEENTRY(TVE_TOGGLE),
	TABLEEND
};

static VALUETABLE tblTVGetImage[] =
{
	TABLEENTRY(TVSIL_NORMAL),
	TABLEENTRY(TVSIL_STATE),
	TABLEEND
};

static VALUETABLE tblTVGetNext[] =
{
	TABLEENTRY(TVGN_CARET),
	TABLEENTRY(TVGN_CHILD),
	TABLEENTRY(TVGN_DROPHILITE),
	TABLEENTRY(TVGN_FIRSTVISIBLE),
	TABLEENTRY(TVGN_NEXT),
	TABLEENTRY(TVGN_NEXTVISIBLE),
	TABLEENTRY(TVGN_PARENT),
	TABLEENTRY(TVGN_PREVIOUS),
	TABLEENTRY(TVGN_PREVIOUSVISIBLE),
	TABLEENTRY(TVGN_ROOT),
	TABLEEND
};


//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_INSERTITEM)
{
	PARM(lpis, TV_INSERTSTRUCT FAR *, LP);
	POUTC(lpis, DWORD);

	return TRUE;
}

DECODERET(TVM_INSERTITEM)
{
	PARM(hItem, HANDLE, RET);
	POUTC(hItem, DWORD);

	return TRUE;
}

DECODEPARM(TVM_INSERTITEM)
{
	P2WPOUT();

	P2LPOUTPTR(TV_INSERTSTRUCT);
	P2IFSIZEOF(TV_INSERTSTRUCT)
	{
		PARM(lpis, TV_INSERTSTRUCT FAR *, ED2);
		INDENT();
		P2OUTC(lpis, hParent, DWORD);
		P2OUTC(lpis, hInsertAfter, DWORD);
		P2OUTPTR(&lpis, item, TV_ITEM);
		{
			PARM(item, TV_ITEM FAR *, &lpis->item);
//			PARM(pszName, LPSTR, item->pszText);
			INDENT();
			P2OUTC(item, mask, UINT);
			P2OUTC(item, hItem, DWORD);
			P2OUTC(item, state, UINT);
			P2OUTC(item, stateMask, UINT);
			P2OUTPTRSTR(item, pszText);
//			INDENT();
//			P2OUTS(pszName);
//			OUTDENT();
			P2OUT(item, cchTextMax);
			P2OUT(item, iImage);
			P2OUT(item, iSelectedImage);
			P2OUT(item, cChildren);
			P2OUTC(item, lParam, DWORD);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_DELETEITEM)
{
	PARM(hitem, HTREEITEM, LP);
	POUTC(hitem, DWORD);

	return TRUE;
}

DECODERET(TVM_DELETEITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TVM_DELETEITEM);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_EXPAND)
{
	PARM(flag, UINT, WP);
	PARM(hitem, HTREEITEM, LP);

	TABLEORVALUE(tblTVExpand, flag);
	POUTC(hitem, DWORD);

	return TRUE;
}

DECODERET(TVM_EXPAND)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TVM_EXPAND);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_GETITEMRECT)
{
	PARM(fItemRect, BOOL, WP);
	PARM(prc, LPRECT, LP);

	POUTB(fItemRect);
	POUTC(prc, DWORD);

	return TRUE;
}

DECODERET(TVM_GETITEMRECT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TVM_GETITEMRECT)
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

NODECODE(TVM_GETCOUNT);

DECODERET(TVM_GETCOUNT)
{
	PARM(nCount, INT, RET);
	POUT(nCount);

	return TRUE;
}

NODECODEPARM(TVM_GETCOUNT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TVM_GETINDENT);

DECODERET(TVM_GETINDENT)
{
	PARM(indent, INT, RET);
	POUT(indent);

	return TRUE;
}

NODECODEPARM(TVM_GETINDENT);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_SETINDENT)
{
	PARM(indent, INT, WP);
	POUT(indent);

	return TRUE;
}

NODECODERET(TVM_SETINDENT);

NODECODEPARM(TVM_SETINDENT);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_GETIMAGELIST)
{
	PARM(iImage, UINT, WP);
	TABLEORVALUE(tblTVGetImage, iImage);

	return TRUE;
}

DECODERET(TVM_GETIMAGELIST)
{
	PARM(hImageList, HANDLE, RET);
	POUTC(hImageList, DWORD);

	return TRUE;
}

NODECODEPARM(TVM_GETIMAGELIST);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_SETIMAGELIST)
{
	PARM(iImage, UINT, WP);
	PARM(himl, HIMAGELIST, LP);

	TABLEORVALUE(tblTVGetImage, iImage);
	POUTC(himl, DWORD);

	return TRUE;
}

DECODERET(TVM_SETIMAGELIST)
{
	PARM(hPrevImageList, HANDLE, RET);
	POUTC(hPrevImageList, DWORD);

	return TRUE;
}

NODECODEPARM(TVM_SETIMAGELIST);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_GETNEXTITEM)
{
	PARM(flag, UINT, WP);
	PARM(hitem, HTREEITEM, LP);

	TABLEORVALUE(tblTVGetNext, flag);
	POUTC(hitem, DWORD);

	return TRUE;
}

DECODERET(TVM_GETNEXTITEM)
{
	PARM(hItem, HANDLE, RET);
	POUTC(hItem, DWORD);

	return TRUE;
}

NODECODEPARM(TVM_GETNEXTITEM);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_SELECTITEM)
{
	DECODELIKE(TVM_GETNEXTITEM);
}

DECODERET(TVM_SELECTITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TVM_SELECTITEM);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_GETITEM)
{
	PARM(pitem, TV_ITEM FAR *, LP);
	POUTC(pitem, DWORD);

	return TRUE;
}

DECODERET(TVM_GETITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(TVM_GETITEM)
{
	P2WPOUT();

	P2LPOUTPTR(TV_ITEM);
	P2IFSIZEOF(TV_ITEM)
	{
		PARM(pitem, TV_ITEM FAR *, ED2);
//		PARM(pszName, LPSTR, pitem->pszText);
		INDENT();
		P2OUTC(pitem, mask, UINT);
		P2OUTC(pitem, hItem, DWORD);
		P2OUTC(pitem, state, UINT);
		P2OUTC(pitem, stateMask, UINT);
		P2OUTPTRSTR(pitem, pszText);
//		INDENT();
//		P2OUTS(pszName);
//		OUTDENT();
		P2OUT(pitem, cchTextMax);
		P2OUT(pitem, iImage);
		P2OUT(pitem, iSelectedImage);
		P2OUT(pitem, cChildren);
		P2OUTC(pitem, lParam, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_SETITEM)
{
	DECODELIKE(TVM_GETITEM);
}

DECODERET(TVM_SETITEM)
{
	PARM(nRet, INT, RET);
	POUT(nRet);

	return TRUE;
}

DECODEPARM(TVM_SETITEM)
{
	DECODEPARMLIKE(TVM_GETITEM);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_EDITLABEL)
{
	PARM(hitem, HTREEITEM, LP);
	POUTC(hitem, DWORD);

	return TRUE;
}

DECODERET(TVM_EDITLABEL)
{
	PARM(hwndEdit, HWND, RET);
	POUTC(hwndEdit, DWORD);

	return TRUE;
}

NODECODEPARM(TVM_EDITLABEL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TVM_GETEDITCONTROL);

DECODERET(TVM_GETEDITCONTROL)
{
	PARM(hwndEdit, HWND, RET);
	POUTC(hwndEdit, DWORD);

	return TRUE;
}

NODECODEPARM(TVM_GETEDITCONTROL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(TVM_GETVISIBLECOUNT);

DECODERET(TVM_GETVISIBLECOUNT)
{
	PARM(nCount, INT, RET);
	POUT(nCount);

	return TRUE;
}

NODECODEPARM(TVM_GETVISIBLECOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_HITTEST)
{
	PARM(lpht, TV_HITTESTINFO FAR *, LP);
	POUTC(lpht, DWORD);

	return TRUE;
}

DECODERET(TVM_HITTEST)
{
	PARM(hItem, HANDLE, RET);
	POUTC(hItem, DWORD);

	return TRUE;
}

DECODEPARM(TVM_HITTEST)
{
	P2WPOUT();

	P2LPOUTPTR(TV_HITTESTINFO);
	P2IFSIZEOF(TV_HITTESTINFO)
	{
		PARM(lpht, TV_HITTESTINFO FAR *, ED2);
		INDENT();
		P2OUTPT(lpht, pt);
		P2OUTC(lpht, flags, UINT);
		P2OUTC(lpht, hItem, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_CREATEDRAGIMAGE)
{
	PARM(hitem, HTREEITEM, LP);
	POUTC(hitem, DWORD);

	return TRUE;
}

DECODERET(TVM_CREATEDRAGIMAGE)
{
	PARM(hImageList, HIMAGELIST, RET);
	POUTC(hImageList, DWORD);

	return TRUE;
}

NODECODEPARM(TVM_CREATEDRAGIMAGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_SORTCHILDREN)
{
	PARM(fReserved, UINT, WP);
	PARM(hitem, HTREEITEM, LP);

	POUTC(fReserved, UINT);
	POUTC(hitem, DWORD);

	return TRUE;
}

DECODERET(TVM_SORTCHILDREN)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TVM_SORTCHILDREN);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_ENSUREVISIBLE)
{
	PARM(hitem, HTREEITEM, LP);
	POUTC(hitem, DWORD);

	return TRUE;
}

DECODERET(TVM_ENSUREVISIBLE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TVM_ENSUREVISIBLE);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_SORTCHILDRENCB)
{
	PARM(fReserved, UINT, WP);
	PARM(psort, TV_SORTCB FAR *, LP);

	POUTC(fReserved, UINT);
	POUTC(psort, DWORD);

	return TRUE;
}

DECODERET(TVM_SORTCHILDRENCB)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TVM_SORTCHILDRENCB);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_ENDEDITLABELNOW)
{
	PARM(fCancel, BOOL, WP);
	POUTB(fCancel);

	return TRUE;
}

DECODERET(TVM_ENDEDITLABELNOW)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(TVM_ENDEDITLABELNOW);

//////////////////////////////////////////////////////////////////////////////

DECODE(TVM_GETISEARCHSTRING)
{
	PARM(lpsz, LPSTR, LP);
	POUTC(lpsz, DWORD);

	return TRUE;
}

DECODERET(TVM_GETISEARCHSTRING)
{
	PARM(cchChar, INT, RET);
	POUT(cchChar);

	return TRUE;
}

DECODEPARM(TVM_GETISEARCHSTRING)
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
#endif	// DISABLE_WIN95_MESSAGES
