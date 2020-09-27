//-----------------------------------------------------------------------------
//  LVMsgs.cpp
//
//  ListView (Chicago control) message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static VALUETABLE tblLVRetOK[] =
{
	TABLEENTRY(LB_OKAY),
	TABLEENTRY(LB_ERR),
	TABLEENTRY(LB_ERRSPACE),
	TABLEEND
};
#ifndef DISABLE_WIN95_MESSAGES
static VALUETABLE tblLVArrange[] =
{
	TABLEENTRY(LVA_ALIGNBOTTOM),
	TABLEENTRY(LVA_ALIGNLEFT),
	TABLEENTRY(LVA_ALIGNRIGHT),
	TABLEENTRY(LVA_ALIGNTOP),
	TABLEENTRY(LVA_DEFAULT),
	TABLEENTRY(LVA_SNAPTOGRID),
	TABLEENTRY(LVA_SORTASCENDING | LVA_ALIGNBOTTOM),
	TABLEENTRY(LVA_SORTASCENDING | LVA_ALIGNLEFT),
	TABLEENTRY(LVA_SORTASCENDING | LVA_ALIGNRIGHT),
	TABLEENTRY(LVA_SORTASCENDING | LVA_ALIGNTOP),
	TABLEENTRY(LVA_SORTASCENDING | LVA_DEFAULT),
	TABLEENTRY(LVA_SORTASCENDING | LVA_SNAPTOGRID),
	TABLEENTRY(LVA_SORTDESCENDING | LVA_ALIGNBOTTOM),
	TABLEENTRY(LVA_SORTDESCENDING | LVA_ALIGNLEFT),
	TABLEENTRY(LVA_SORTDESCENDING | LVA_ALIGNRIGHT),
	TABLEENTRY(LVA_SORTDESCENDING | LVA_ALIGNTOP),
	TABLEENTRY(LVA_SORTDESCENDING | LVA_DEFAULT),
	TABLEENTRY(LVA_SORTDESCENDING | LVA_SNAPTOGRID),
	TABLEEND
};

static VALUETABLE tblLVImageList[] =
{
	TABLEENTRY(LVSIL_NORMAL),
	TABLEENTRY(LVSIL_SMALL),
	TABLEENTRY(LVSIL_STATE),
	TABLEEND
};

static VALUETABLE tblLVItemRect[] =
{
	TABLEENTRY(LVIR_BOUNDS),
	TABLEENTRY(LVIR_ICON),
	TABLEENTRY(LVIR_LABEL),
	TABLEENTRY(LVIR_SELECTBOUNDS),
	TABLEEND
};

static VALUETABLE tblLVGetNext[] =
{
	TABLEENTRY(LVNI_ABOVE),
	TABLEENTRY(LVNI_ALL),
	TABLEENTRY(LVNI_BELOW),
	TABLEENTRY(LVNI_PREVIOUS),
	TABLEENTRY(LVNI_TOLEFT),
	TABLEENTRY(LVNI_TORIGHT),
	TABLEENTRY(LVNI_ABOVE | LVNI_CUT),
	TABLEENTRY(LVNI_ALL | LVNI_CUT),
	TABLEENTRY(LVNI_BELOW | LVNI_CUT),
	TABLEENTRY(LVNI_PREVIOUS | LVNI_CUT),
	TABLEENTRY(LVNI_TOLEFT | LVNI_CUT),
	TABLEENTRY(LVNI_TORIGHT | LVNI_CUT),
	TABLEENTRY(LVNI_ABOVE | LVNI_DROPHILITED),
	TABLEENTRY(LVNI_ALL | LVNI_DROPHILITED),
	TABLEENTRY(LVNI_BELOW | LVNI_DROPHILITED),
	TABLEENTRY(LVNI_PREVIOUS | LVNI_DROPHILITED),
	TABLEENTRY(LVNI_TOLEFT | LVNI_DROPHILITED),
	TABLEENTRY(LVNI_TORIGHT | LVNI_DROPHILITED),
	TABLEENTRY(LVNI_ABOVE | LVNI_FOCUSED),
	TABLEENTRY(LVNI_ALL | LVNI_FOCUSED),
	TABLEENTRY(LVNI_BELOW | LVNI_FOCUSED),
	TABLEENTRY(LVNI_PREVIOUS | LVNI_FOCUSED),
	TABLEENTRY(LVNI_TOLEFT | LVNI_FOCUSED),
	TABLEENTRY(LVNI_TORIGHT | LVNI_FOCUSED),
	TABLEENTRY(LVNI_ABOVE | LVNI_SELECTED),
	TABLEENTRY(LVNI_ALL | LVNI_SELECTED),
	TABLEENTRY(LVNI_BELOW | LVNI_SELECTED),
	TABLEENTRY(LVNI_PREVIOUS | LVNI_SELECTED),
	TABLEENTRY(LVNI_TOLEFT | LVNI_SELECTED),
	TABLEENTRY(LVNI_TORIGHT | LVNI_SELECTED),
	TABLEEND
};

static VALUETABLE tblLVColumnWidth[] =
{
	TABLEENTRY(LVSCW_AUTOSIZE),
	TABLEENTRY(LVSCW_AUTOSIZE_USEHEADER),
	TABLEEND
};

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETBKCOLOR);

DECODERET(LVM_GETBKCOLOR)
{
	PARM(clrBk, COLORREF, RET);
	POUTC(clrBk, DWORD);

	return TRUE;
}

NODECODEPARM(LVM_GETBKCOLOR);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETBKCOLOR)
{
	PARM(clrBk, COLORREF, LP);
	POUTC(clrBk, DWORD);

	return TRUE;
}

DECODERET(LVM_SETBKCOLOR)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_SETBKCOLOR);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETIMAGELIST)
{
	PARM(iImageList, INT, WP);
	TABLEORVALUE(tblLVImageList, iImageList);

	return TRUE;
}

DECODERET(LVM_GETIMAGELIST)
{
	PARM(hImageList, HIMAGELIST, RET);
	POUTC(hImageList, DWORD);

	return TRUE;
}

NODECODEPARM(LVM_GETIMAGELIST);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETIMAGELIST)
{
	PARM(iImageList, INT, WP);
	PARM(himl, HIMAGELIST, LP);

	TABLEORVALUE(tblLVImageList, iImageList);
	POUTC(himl, DWORD);

	return TRUE;
}

NODECODERET(LVM_SETIMAGELIST);

NODECODEPARM(LVM_SETIMAGELIST);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETITEMCOUNT);

DECODERET(LVM_GETITEMCOUNT)
{
	PARM(iCount, INT, RET);
	POUT(iCount);

	return TRUE;
}

NODECODEPARM(LVM_GETITEMCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETITEM)
{
	PARM(pitem, LV_ITEM FAR *, LP);
	POUTC(pitem, DWORD);

	return TRUE;
}

DECODERET(LVM_GETITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_GETITEM)
{
	P2WPOUT();

	P2LPOUTPTR(LV_ITEM);
	P2IFSIZEOF(LV_ITEM)
	{
		PARM(pitem, LV_ITEM FAR *, ED2);
//		PARM(pszName, LPSTR, pitem->pszText);
		INDENT();
		P2OUTC(pitem, mask, UINT);
		P2OUT(pitem, iItem);
		P2OUT(pitem, iSubItem);
		P2OUTC(pitem, state, UINT);
		P2OUTC(pitem, stateMask, UINT);
		P2OUTPTRSTR(pitem, pszText);
//		INDENT();
//		P2OUTS(pszName);
//		OUTDENT();
		P2OUT(pitem, cchTextMax);
		P2OUT(pitem, iImage);
		P2OUTC(pitem, lParam, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETITEM)
{
	DECODELIKE(LVM_GETITEM);
}

DECODERET(LVM_SETITEM)
{
	DECODERETLIKE(LVM_GETITEM);
}

DECODEPARM(LVM_SETITEM)
{
	DECODEPARMLIKE(LVM_GETITEM);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_INSERTITEM)
{
	DECODELIKE(LVM_GETITEM);
}

DECODERET(LVM_INSERTITEM)
{
	PARM(nIndex, INT, RET);
	POUT(nIndex);

	return TRUE;
}

DECODEPARM(LVM_INSERTITEM)
{
	DECODEPARMLIKE(LVM_GETITEM);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_DELETEITEM)
{
	PARM(iItem, INT, WP);
	POUT(iItem);

	return TRUE;
}

DECODERET(LVM_DELETEITEM)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_DELETEITEM);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_DELETEALLITEMS);

DECODERET(LVM_DELETEALLITEMS)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_DELETEALLITEMS);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETCALLBACKMASK);

DECODERET(LVM_GETCALLBACKMASK)
{
	PARM(mask, UINT, RET);
	POUTC(mask, UINT);

	return TRUE;
}

NODECODEPARM(LVM_GETCALLBACKMASK);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETCALLBACKMASK)
{
	PARM(mask, UINT, WP);
	POUTC(mask, UINT);

	return TRUE;
}

DECODERET(LVM_SETCALLBACKMASK)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_SETCALLBACKMASK);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETNEXTITEM)
{
	PARM(iStart, INT, WP);
	PARM(flags, UINT, LP);

	POUT(iStart);
	TABLEORVALUE(tblLVGetNext, flags);

	return TRUE;
}

DECODERET(LVM_GETNEXTITEM)
{
	PARM(iIndex, INT, RET);
	POUT(iIndex);

	return TRUE;
}

NODECODEPARM(LVM_GETNEXTITEM);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_FINDITEM)
{
	PARM(iStart, INT, WP);
	PARM(plvfi, LV_FINDINFO FAR *, LP);

	POUT(iStart);
	POUTC(plvfi, DWORD);

	return TRUE;
}

DECODERET(LVM_FINDITEM)
{
	PARM(iIndex, INT, RET);
	POUT(iIndex);

	return TRUE;
}

DECODEPARM(LVM_FINDITEM)
{
	P2WPOUT();

	P2LPOUTPTR(LV_FINDINFO);
	P2IFSIZEOF(LV_FINDINFO)
	{
		PARM(plvfi, LV_FINDINFO FAR *, ED2);
//		PARM(pszName, LPSTR, plvfi->psz);
		INDENT();
		P2OUTC(plvfi, flags, UINT);
		P2OUTPTRSTR(plvfi, psz);
//		INDENT();
//		P2OUTS(pszName);
//		OUTDENT();
		P2OUTC(plvfi, lParam, DWORD);
		P2OUTPT(plvfi, pt);
		P2OUTC(plvfi, vkDirection, UINT);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETITEMRECT)
{
	PARM(i, INT, WP);
	PARM(prc, LPRECT, LP);

	POUT(i);
	POUTC(prc, DWORD);

	return TRUE;
}

DECODERET(LVM_GETITEMRECT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_GETITEMRECT)
{
	P2WPOUT();

	P2LPOUTPTR(RECT);
	P2IFSIZEOF(RECT)
	{
		if (pmsd2->fPostType == POSTTYPE_SENT)
		{
			PARM(prc, LPRECT, ED2);
			PARM(code, INT, prc->left);
			INDENT();
			P2LABEL(code);
			TableOrInt2(pmps, tblLVItemRect, code);
		}
		else
		{
			PARM(prc, LPRECT, ED2);
			INDENT();
			P2OUT(prc, left);
			P2OUT(prc, top);
			P2OUT(prc, right);
			P2OUT(prc, bottom);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETITEMPOSITION)
{
	PARM(i, INT, WP);
	PARM(x, INT, LOLP);
	PARM(y, INT, HILP);

	POUT(i);
	POUT(x);
	POUT(y);

	return TRUE;
}

DECODERET(LVM_SETITEMPOSITION)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_SETITEMPOSITION);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETITEMPOSITION);

DECODERET(LVM_GETITEMPOSITION)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_GETITEMPOSITION)
{
	P2WPOUT();

	P2LPOUTPTR(POINT);
	P2IFSIZEOF(POINT)
	{
		PARM(pt, LPPOINT, ED2);
		INDENT();
		P2OUT(pt, x);
		P2OUT(pt, y);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETSTRINGWIDTH)
{
	PARM(psz, LPSTR, LP);
	POUTC(psz, DWORD);

	return TRUE;
}

DECODERET(LVM_GETSTRINGWIDTH)
{
	PARM(nWidth, INT, RET);
	POUT(nWidth);

	return TRUE;
}

DECODEPARM(LVM_GETSTRINGWIDTH)
{
	P2WPOUT();

	P2LPOUTPTRSTR;
	P2IFDATA()
	{
		PARM(psz, LPSTR, ED2);
		INDENT();
		P2OUTS(psz);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_HITTEST)
{
	PARM(pinfo, LV_HITTESTINFO FAR *, LP);
	POUTC(pinfo, DWORD);

	return TRUE;
}

DECODERET(LVM_HITTEST)
{
	PARM(nIndex, INT, RET);
	POUT(nIndex);

	return TRUE;
}

DECODEPARM(LVM_HITTEST)
{
	P2WPOUT();

	P2LPOUTPTR(LV_HITTESTINFO);
	P2IFSIZEOF(LV_HITTESTINFO)
	{
		PARM(pinfo, LV_HITTESTINFO FAR *, ED2);
		INDENT();
		P2OUTPT(pinfo, pt);
		P2OUTC(pinfo, flags, UINT);
		P2OUT(pinfo, iItem);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_ENSUREVISIBLE)
{
	PARM(iItem, INT, WP);
	PARM(fPartialOK, BOOL, LP);

	POUT(iItem);
	POUTB(fPartialOK);

	return TRUE;
}

DECODERET(LVM_ENSUREVISIBLE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_ENSUREVISIBLE);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SCROLL)
{
	PARM(dx, INT, LOLP);
	PARM(dy, INT, HILP);

	POUT(dx);
	POUT(dy);

	return TRUE;
}

DECODERET(LVM_SCROLL)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_SCROLL);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_REDRAWITEMS)
{
	PARM(iFirst, INT, LOLP);
	PARM(iLast, INT, HILP);

	POUT(iFirst);
	POUT(iLast);

	return TRUE;
}

DECODERET(LVM_REDRAWITEMS)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_REDRAWITEMS);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_ARRANGE)
{
	PARM(code, UINT, WP);
	TABLEORVALUE(tblLVArrange, code);

	return TRUE;
}

DECODERET(LVM_ARRANGE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_ARRANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_EDITLABEL)
{
	PARM(iItem, INT, WP);
	POUT(iItem);

	return TRUE;
}

DECODERET(LVM_EDITLABEL)
{
	PARM(hwndEdit, HWND, RET);
	POUTC(hwndEdit, DWORD);

	return TRUE;
}

NODECODEPARM(LVM_EDITLABEL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETEDITCONTROL);

DECODERET(LVM_GETEDITCONTROL)
{
	PARM(hwndEdit, HWND, RET);
	POUTC(hwndEdit, DWORD);

	return TRUE;
}

NODECODEPARM(LVM_GETEDITCONTROL);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETCOLUMN)
{
	PARM(iCol, INT, WP);
	PARM(pcol, LV_COLUMN FAR *, LP);

	POUT(iCol);
	POUTC(pcol, DWORD);

	return TRUE;
}

DECODERET(LVM_GETCOLUMN)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_GETCOLUMN)
{
	P2WPOUT();

	P2LPOUTPTR(LV_COLUMN);
	P2IFSIZEOF(LV_COLUMN)
	{
		PARM(pcol, LV_COLUMN FAR *, ED2);
//		PARM(pszName, LPSTR, pcol->pszText);
		INDENT();
		P2OUTC(pcol, mask, UINT);
		P2OUT(pcol, fmt);
		P2OUT(pcol, cx);
		P2OUTPTRSTR(pcol, pszText);
//		INDENT();
//		P2OUTS(pszName);
//		OUTDENT();
		P2OUT(pcol, cchTextMax);
		P2OUT(pcol, iSubItem);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETCOLUMN)
{
	DECODELIKE(LVM_GETCOLUMN);
}

DECODERET(LVM_SETCOLUMN)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_SETCOLUMN)
{
	DECODEPARMLIKE(LVM_GETCOLUMN);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_INSERTCOLUMN)
{
	DECODELIKE(LVM_GETCOLUMN);
}

DECODERET(LVM_INSERTCOLUMN)
{
	PARM(nIndex, INT, RET);
	POUT(nIndex);

	return TRUE;
}

DECODEPARM(LVM_INSERTCOLUMN)
{
	DECODEPARMLIKE(LVM_GETCOLUMN);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_DELETECOLUMN)
{
	PARM(iCol, INT, WP);
	POUT(iCol);

	return TRUE;
}

DECODERET(LVM_DELETECOLUMN)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_DELETECOLUMN);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETCOLUMNWIDTH)
{
	PARM(iCol, INT, WP);
	POUT(iCol);

	return TRUE;
}

DECODERET(LVM_GETCOLUMNWIDTH)
{
	PARM(iWidth, INT, RET);
	POUT(iWidth);

	return TRUE;
}

NODECODEPARM(LVM_GETCOLUMNWIDTH);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETCOLUMNWIDTH)
{
	PARM(iCol, INT, WP);
	PARM(cx, INT, LOLP);

	POUT(iCol);
	TABLEORVALUE(tblLVColumnWidth, cx);

	return TRUE;
}

DECODERET(LVM_SETCOLUMNWIDTH)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_SETCOLUMNWIDTH);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_CREATEDRAGIMAGE)
{
	PARM(iItem, INT, WP);
	PARM(lpptUpLeft, LPPOINT, LP);

	POUT(iItem);
	POUTC(lpptUpLeft, DWORD);

	return TRUE;
}

DECODERET(LVM_CREATEDRAGIMAGE)
{
	PARM(hImage, HANDLE, RET);
	POUTC(hImage, DWORD);

	return TRUE;
}

DECODEPARM(LVM_CREATEDRAGIMAGE)
{
	P2WPOUT();

	P2LPOUTPTR(POINT);
	P2IFSIZEOF(POINT)
	{
		PARM(pt, LPPOINT, ED2);
		INDENT();
		P2OUT(pt, x);
		P2OUT(pt, y);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETVIEWRECT)
{
	PARM(prc, LPRECT, LP);
	POUTC(prc, DWORD);

	return TRUE;
}

DECODERET(LVM_GETVIEWRECT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_GETVIEWRECT)
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

NODECODE(LVM_GETTEXTCOLOR);

DECODERET(LVM_GETTEXTCOLOR)
{
	PARM(clrText, COLORREF, RET);
	POUTC(clrText, DWORD);

	return TRUE;
}

NODECODEPARM(LVM_GETTEXTCOLOR);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETTEXTCOLOR)
{
	DECODELIKE(LVM_SETTEXTBKCOLOR);
}

DECODERET(LVM_SETTEXTCOLOR)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_SETTEXTCOLOR);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETTEXTBKCOLOR);

DECODERET(LVM_GETTEXTBKCOLOR)
{
	PARM(clrText, COLORREF, RET);
	POUTC(clrText, DWORD);

	return TRUE;
}

NODECODEPARM(LVM_GETTEXTBKCOLOR);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETTEXTBKCOLOR)
{
	PARM(clrText, COLORREF, LP);
	POUTC(clrText, DWORD);

	return TRUE;
}

DECODERET(LVM_SETTEXTBKCOLOR)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_SETTEXTBKCOLOR);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETTOPINDEX);

DECODERET(LVM_GETTOPINDEX)
{
	PARM(nTopIndex, INT, RET);
	POUT(nTopIndex);

	return TRUE;
}

NODECODEPARM(LVM_GETTOPINDEX);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETCOUNTPERPAGE);

DECODERET(LVM_GETCOUNTPERPAGE)
{
	PARM(iCount, INT, RET);
	POUT(iCount);

	return TRUE;
}

NODECODEPARM(LVM_GETCOUNTPERPAGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETORIGIN)
{
	PARM(lpptOrg, LPPOINT, LP);
	POUTC(lpptOrg, DWORD);

	return TRUE;
}

DECODERET(LVM_GETORIGIN)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_GETORIGIN)
{
	DECODEPARMLIKE(LVM_GETITEMPOSITION);
}
//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_UPDATE)
{
	PARM(iItem, INT, WP);
	POUT(iItem);

	return TRUE;
}

DECODERET(LVM_UPDATE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_UPDATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETITEMSTATE)
{
	PARM(i, INT, WP);
	PARM(pitem, LV_ITEM FAR *, LP);

	POUT(i);
	POUTC(pitem, DWORD);

	return TRUE;
}

DECODERET(LVM_SETITEMSTATE)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_SETITEMSTATE)
{
	DECODEPARMLIKE(LVM_GETITEM);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETITEMSTATE)
{
	PARM(i, INT, WP);
	PARM(mask, UINT, LP);

	POUT(i);
	POUTC(mask, UINT);

	return TRUE;
}

DECODERET(LVM_GETITEMSTATE)
{
	PARM(flags, UINT, RET);
	POUTC(flags, UINT);

	return TRUE;
}

NODECODEPARM(LVM_GETITEMSTATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETITEMTEXT)
{
	PARM(iItem, INT, WP);
	PARM(pitem, LV_ITEM FAR *, LP);

	POUT(iItem);
	POUTC(pitem, DWORD);

	return TRUE;
}

DECODERET(LVM_GETITEMTEXT)
{
	PARM(cchChar, INT, RET);
	POUT(cchChar);

	return TRUE;
}

DECODEPARM(LVM_GETITEMTEXT)
{
	DECODEPARMLIKE(LVM_GETITEM);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETITEMTEXT)
{
	DECODELIKE(LVM_SETITEMSTATE);
}

DECODERET(LVM_SETITEMTEXT)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(LVM_SETITEMTEXT)
{
	DECODEPARMLIKE(LVM_SETITEMSTATE);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETITEMCOUNT)
{
	PARM(cItems, INT, WP);
	POUT(cItems);

	return TRUE;
}

NODECODERET(LVM_SETITEMCOUNT);

NODECODEPARM(LVM_SETITEMCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SORTITEMS)
{
	PARM(lParamSort, LPARAM, WP);
	PARM(pfnCompare, PFNLVCOMPARE, LP);

	POUTC(lParamSort, DWORD);
	POUTC(pfnCompare, DWORD);

	return TRUE;
}

DECODERET(LVM_SORTITEMS)
{
	PARM(fSuccess, INT, RET);
	TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

NODECODEPARM(LVM_SORTITEMS);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_SETITEMPOSITION32)
{
	PARM(iItem, INT, WP);
	PARM(lpptNewPos, LPPOINT, LP);

	POUT(iItem);
	POUTC(lpptNewPos, DWORD);

	return TRUE;
}

NODECODERET(LVM_SETITEMPOSITION32);

DECODEPARM(LVM_SETITEMPOSITION32)
{
	DECODEPARMLIKE(LVM_GETITEMPOSITION);
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETSELECTEDCOUNT);

DECODERET(LVM_GETSELECTEDCOUNT)
{
	PARM(nSelected, INT, RET);
	POUT(nSelected);

	return TRUE;
}

NODECODEPARM(LVM_GETSELECTEDCOUNT);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETITEMSPACING)
{
	PARM(fSmall, BOOL, WP);
	POUTB(fSmall);

	return TRUE;
}

DECODERET(LVM_GETITEMSPACING)
{
	PARM(iSpacing, INT, RET);
	POUT(iSpacing);

	return TRUE;
}

NODECODEPARM(LVM_GETITEMSPACING);

//////////////////////////////////////////////////////////////////////////////

DECODE(LVM_GETISEARCHSTRING)
{
	PARM(lpsz, LPSTR, LP);
	POUTC(lpsz, DWORD);

	return TRUE;
}

DECODERET(LVM_GETISEARCHSTRING)
{
	PARM(cchChar, INT, RET);
	POUT(cchChar);

	return TRUE;
}

DECODEPARM(LVM_GETISEARCHSTRING)
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

NODECODE(LVM_GETITEMW);

NODECODERET(LVM_GETITEMW);

NODECODEPARM(LVM_GETITEMW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_SETITEMW);

NODECODERET(LVM_SETITEMW);

NODECODEPARM(LVM_SETITEMW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_INSERTITEMW);

NODECODERET(LVM_INSERTITEMW);

NODECODEPARM(LVM_INSERTITEMW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_FINDITEMW);

NODECODERET(LVM_FINDITEMW);

NODECODEPARM(LVM_FINDITEMW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETSTRINGWIDTHW);

DECODERET(LVM_GETSTRINGWIDTHW)
{
	PARM(nWidth, INT, RET);
	POUT(nWidth);

	return TRUE;
}

NODECODEPARM(LVM_GETSTRINGWIDTHW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETCOLUMNW);

NODECODERET(LVM_GETCOLUMNW);

NODECODEPARM(LVM_GETCOLUMNW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_SETCOLUMNW);

NODECODERET(LVM_SETCOLUMNW);

NODECODEPARM(LVM_SETCOLUMNW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_INSERTCOLUMNW);

NODECODERET(LVM_INSERTCOLUMNW);

NODECODEPARM(LVM_INSERTCOLUMNW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETITEMTEXTW);

NODECODERET(LVM_GETITEMTEXTW);

NODECODEPARM(LVM_GETITEMTEXTW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_SETITEMTEXTW);

NODECODERET(LVM_SETITEMTEXTW);

NODECODEPARM(LVM_SETITEMTEXTW);

//////////////////////////////////////////////////////////////////////////////

NODECODE(LVM_GETISEARCHSTRINGW);

NODECODERET(LVM_GETISEARCHSTRINGW);

NODECODEPARM(LVM_GETISEARCHSTRINGW);

//////////////////////////////////////////////////////////////////////////////
#endif	// DISABLE_WIN95_MESSAGES
