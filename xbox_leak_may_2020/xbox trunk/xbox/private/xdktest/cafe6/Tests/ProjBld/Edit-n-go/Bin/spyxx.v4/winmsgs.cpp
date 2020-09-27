//-----------------------------------------------------------------------------
//  WinMsgs.cpp
//
//  Window message crackers.

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
// Tables
//-----------------------------------------------------------------------------

VALUETABLE tblWindowStyles[] =
{
	TABLEENTRY(WS_POPUP),
	TABLEENTRY(WS_CHILD),
	TABLEENTRY(WS_MINIMIZE),
	TABLEENTRY(WS_VISIBLE),
	TABLEENTRY(WS_DISABLED),
	TABLEENTRY(WS_CLIPSIBLINGS),
	TABLEENTRY(WS_CLIPCHILDREN),
	TABLEENTRY(WS_MAXIMIZE),
	TABLEENTRY(WS_BORDER),
	TABLEENTRY(WS_DLGFRAME),
	TABLEENTRY(WS_VSCROLL),
	TABLEENTRY(WS_HSCROLL),
	TABLEENTRY(WS_SYSMENU),
	TABLEENTRY(WS_THICKFRAME),
	TABLEENTRY(WS_GROUP),
	TABLEENTRY(WS_TABSTOP),
	TABLEEND
};

VALUETABLE tblDialogStyles[] =
{
	TABLEENTRY(DS_ABSALIGN),
	TABLEENTRY(DS_SYSMODAL),
	TABLEENTRY(DS_LOCALEDIT),
	TABLEENTRY(DS_SETFONT),
	TABLEENTRY(DS_MODALFRAME),
	TABLEENTRY(DS_NOIDLEMSG),
	TABLEENTRY(DS_SETFOREGROUND),
	TABLEEND
};

VALUETABLE tblButtonStyles[] =
{
	// don't change ordering here--having the styles ordered
	// from largest hex value to smallest allows property page
	// to display them correctly.
	TABLEENTRY(BS_LEFTTEXT),
	TABLEENTRY(BS_OWNERDRAW),
	TABLEENTRY(BS_AUTORADIOBUTTON),
	TABLEENTRY(BS_USERBUTTON),
	TABLEENTRY(BS_GROUPBOX),
	TABLEENTRY(BS_AUTO3STATE),
	TABLEENTRY(BS_3STATE),
	TABLEENTRY(BS_RADIOBUTTON),
	TABLEENTRY(BS_AUTOCHECKBOX),
	TABLEENTRY(BS_CHECKBOX),
	TABLEENTRY(BS_DEFPUSHBUTTON),
	TABLEENTRY(BS_PUSHBUTTON),
	TABLEEND
};

VALUETABLE tblComboBoxStyles[] =
{
	// don't change ordering here--having the styles ordered
	// from largest hex value to smallest allows property page
	// to display them correctly.
	TABLEENTRY(CBS_DISABLENOSCROLL),
	TABLEENTRY(CBS_NOINTEGRALHEIGHT),
	TABLEENTRY(CBS_HASSTRINGS),
	TABLEENTRY(CBS_SORT),
	TABLEENTRY(CBS_OEMCONVERT),
	TABLEENTRY(CBS_AUTOHSCROLL),
	TABLEENTRY(CBS_OWNERDRAWVARIABLE),
	TABLEENTRY(CBS_OWNERDRAWFIXED),
	TABLEENTRY(CBS_DROPDOWNLIST),
	TABLEENTRY(CBS_DROPDOWN),
	TABLEENTRY(CBS_SIMPLE),
	TABLEEND
};

VALUETABLE tblEditStyles[] =
{
	// don't change ordering here--having the styles ordered
	// from largest hex value to smallest allows property page
	// to display them correctly.
	TABLEENTRY(ES_WANTRETURN),
	TABLEENTRY(ES_READONLY),
	TABLEENTRY(ES_OEMCONVERT),
	TABLEENTRY(ES_NOHIDESEL),
	TABLEENTRY(ES_AUTOHSCROLL),
	TABLEENTRY(ES_AUTOVSCROLL),
	TABLEENTRY(ES_PASSWORD),
	TABLEENTRY(ES_LOWERCASE),
	TABLEENTRY(ES_UPPERCASE),
	TABLEENTRY(ES_MULTILINE),
	TABLEENTRY(ES_RIGHT),
	TABLEENTRY(ES_CENTER),
	TABLEENTRY(ES_LEFT),
	TABLEEND
};

VALUETABLE tblListBoxStyles[] =
{
	TABLEENTRY(LBS_NOTIFY),
	TABLEENTRY(LBS_SORT),
	TABLEENTRY(LBS_NOREDRAW),
	TABLEENTRY(LBS_MULTIPLESEL),
	TABLEENTRY(LBS_OWNERDRAWFIXED),
	TABLEENTRY(LBS_OWNERDRAWVARIABLE),
	TABLEENTRY(LBS_HASSTRINGS),
	TABLEENTRY(LBS_USETABSTOPS),
	TABLEENTRY(LBS_NOINTEGRALHEIGHT),
	TABLEENTRY(LBS_MULTICOLUMN),
	TABLEENTRY(LBS_WANTKEYBOARDINPUT),
	TABLEENTRY(LBS_EXTENDEDSEL),
	TABLEENTRY(LBS_DISABLENOSCROLL),
	TABLEENTRY(LBS_NODATA),
	TABLEENTRY(LBS_STANDARD),
	TABLEEND
};

VALUETABLE tblScrollBarStyles[] =
{
	TABLEENTRY(SBS_HORZ),
	TABLEENTRY(SBS_VERT),
	TABLEENTRY(SBS_TOPALIGN),
	TABLEENTRY(SBS_LEFTALIGN),
	TABLEENTRY(SBS_BOTTOMALIGN),
	TABLEENTRY(SBS_RIGHTALIGN),
	TABLEENTRY(SBS_SIZEBOXTOPLEFTALIGN),
	TABLEENTRY(SBS_SIZEBOXBOTTOMRIGHTALIGN),
	TABLEENTRY(SBS_SIZEBOX),
// TODO: TABLEENTRY(SBS_SIZEGRIP),
	TABLEEND
};

VALUETABLE tblStaticStyles[] =
{
	TABLEENTRY(SS_NOPREFIX),
	TABLEENTRY(SS_LEFTNOWORDWRAP),
	TABLEENTRY(SS_SIMPLE),
	TABLEENTRY(SS_USERITEM),	  // TODO: is this obsolete?
	TABLEENTRY(SS_WHITEFRAME),
	TABLEENTRY(SS_GRAYFRAME),
	TABLEENTRY(SS_BLACKFRAME),
	TABLEENTRY(SS_WHITERECT),
	TABLEENTRY(SS_GRAYRECT),
	TABLEENTRY(SS_BLACKRECT),
	TABLEENTRY(SS_ICON),
	TABLEENTRY(SS_RIGHT),
	TABLEENTRY(SS_CENTER),
	TABLEENTRY(SS_LEFT),
// TODO: TABLEENTRY(SS_BITMAP),
// TODO: TABLEENTRY(SS_CENTERIMAGE),
// TODO: TABLEENTRY(SS_METAPICT),
// TODO: TABLEENTRY(SS_NOTIFY),
// TODO: TABLEENTRY(SS_OWNERDRAW),
// TODO: TABLEENTRY(SS_RIGHTIMAGE),
	TABLEEND
};

VALUETABLE tblExtWindowStyles[] =
{
	TABLEENTRY(WS_EX_TRANSPARENT),	// TODO: is this obsolete?
	TABLEENTRY(WS_EX_ACCEPTFILES),
	TABLEENTRY(WS_EX_TOPMOST),
	TABLEENTRY(WS_EX_NOPARENTNOTIFY),
	TABLEENTRY(WS_EX_DLGMODALFRAME),
	TABLEEND
};
#ifndef DISABLE_WIN95_MESSAGES
VALUETABLE tblHeaderStyles[] =
{
	TABLEENTRY(HDS_HORZ),
	TABLEENTRY(HDS_BUTTONS),
	TABLEENTRY(HDS_HIDDEN),
	TABLEEND
};
#endif
VALUETABLE tblToolbarStyles[] =
{
	TABLEENTRY(TBSTYLE_BUTTON),
	TABLEENTRY(TBSTYLE_SEP),
	TABLEENTRY(TBSTYLE_CHECK),
	TABLEENTRY(TBSTYLE_GROUP),
	#ifndef DISABLE_WIN95_MESSAGES
	TABLEENTRY(TBSTYLE_TOOLTIPS),
	TABLEENTRY(TBSTYLE_WRAPABLE),
	TABLEENTRY(TBSTYLE_ALTDRAG),
	#endif
	TABLEEND
};
#ifndef DISABLE_WIN95_MESSAGES
VALUETABLE tblToolTipStyles[] =
{
	TABLEENTRY(TTS_ALWAYSTIP),
	TABLEENTRY(TTS_NOPREFIX),
	TABLEEND
};
#endif
#ifndef DISABLE_WIN95_MESSAGES
VALUETABLE tblStatusBarStyles[] =
{
	TABLEENTRY(SBT_OWNERDRAW),
	TABLEENTRY(SBT_NOBORDERS),
	TABLEENTRY(SBT_POPOUT),
	TABLEENTRY(HBT_SPRING),
	TABLEEND
};
#endif
#ifndef DISABLE_WIN95_MESSAGES
VALUETABLE tblTrackBarStyles[] =
{
	TABLEENTRY(TBS_AUTOTICKS),
	TABLEENTRY(TBS_VERT),
	TABLEENTRY(TBS_HORZ),
	TABLEENTRY(TBS_TOP),
	TABLEENTRY(TBS_BOTTOM),
	TABLEENTRY(TBS_LEFT),
	TABLEENTRY(TBS_RIGHT),
	TABLEENTRY(TBS_BOTH),
	TABLEENTRY(TBS_NOTICKS),
	TABLEENTRY(TBS_ENABLESELRANGE),
	TABLEENTRY(TBS_FIXEDLENGTH),
	TABLEENTRY(TBS_NOTHUMB),
	TABLEEND
};
#endif
VALUETABLE tblUpDownStyles[] =
{
	TABLEENTRY(UDS_WRAP),
	TABLEENTRY(UDS_SETBUDDYINT),
	TABLEENTRY(UDS_ALIGNRIGHT),
	TABLEENTRY(UDS_ALIGNLEFT),
	TABLEENTRY(UDS_AUTOBUDDY),
	TABLEENTRY(UDS_ARROWKEYS),
	#ifndef DISABLE_WIN95_MESSAGES
	TABLEENTRY(UDS_HORZ),
	TABLEENTRY(UDS_NOTHOUSANDS),
	#endif
	TABLEEND
};
#ifndef DISABLE_WIN95_MESSAGES
VALUETABLE tblListViewStyles[] =
{
	TABLEENTRY(LVS_LIST),
	TABLEENTRY(LVS_SMALLICON),
	TABLEENTRY(LVS_REPORT),
	TABLEENTRY(LVS_ICON),
	TABLEENTRY(LVS_SINGLESEL),
	TABLEENTRY(LVS_SHOWSELALWAYS),
	TABLEENTRY(LVS_SORTASCENDING),
	TABLEENTRY(LVS_SORTDESCENDING),
	TABLEENTRY(LVS_SHAREIMAGELISTS),
	TABLEENTRY(LVS_NOLABELWRAP),
	TABLEENTRY(LVS_AUTOARRANGE),
	TABLEENTRY(LVS_EDITLABELS),
	TABLEENTRY(LVS_NOSCROLL),
	TABLEENTRY(LVS_NOCOLUMNHEADER),
	TABLEENTRY(LVS_NOSORTHEADER),
	TABLEEND
};

VALUETABLE tblTreeViewStyles[] =
{
	TABLEENTRY(TVS_HASBUTTONS),
	TABLEENTRY(TVS_HASLINES),
	TABLEENTRY(TVS_LINESATROOT),
	TABLEENTRY(TVS_EDITLABELS),
	TABLEENTRY(TVS_DISABLEDRAGDROP),
	TABLEENTRY(TVS_SHOWSELALWAYS),
	TABLEEND
};

VALUETABLE tblTabControlStyles[] =
{
	TABLEENTRY(TCS_FORCEICONLEFT),
	TABLEENTRY(TCS_FORCELABELLEFT),
	TABLEENTRY(TCS_SHAREIMAGELISTS),
	TABLEENTRY(TCS_TABS),
	TABLEENTRY(TCS_BUTTONS),
	TABLEENTRY(TCS_SINGLELINE),
	TABLEENTRY(TCS_MULTILINE),
	TABLEENTRY(TCS_RIGHTJUSTIFY),
	TABLEENTRY(TCS_FIXEDWIDTH),
	TABLEENTRY(TCS_RAGGEDRIGHT),
	TABLEENTRY(TCS_FOCUSONBUTTONDOWN),
	TABLEENTRY(TCS_OWNERDRAWFIXED),
	TABLEENTRY(TCS_TOOLTIPS),
	TABLEENTRY(TCS_FOCUSNEVER),
	TABLEEND
};

VALUETABLE tblAnimateStyles[] =
{
	TABLEENTRY(ACS_CENTER),
	TABLEENTRY(ACS_TRANSPARENT),
	TABLEENTRY(ACS_AUTOPLAY),
	TABLEEND
};
#endif
VALUETABLE tblHScroll[] =
{
	TABLEENTRY(SB_LINELEFT),
	TABLEENTRY(SB_LINERIGHT),
	TABLEENTRY(SB_PAGELEFT),
	TABLEENTRY(SB_PAGERIGHT),
	TABLEENTRY(SB_LEFT),
	TABLEENTRY(SB_RIGHT),
	TABLEENTRY(SB_THUMBPOSITION),
	TABLEENTRY(SB_THUMBTRACK),
	TABLEENTRY(SB_ENDSCROLL),
	TABLEEND
};

VALUETABLE tblVScroll[] =
{
	TABLEENTRY(SB_LINEUP),
	TABLEENTRY(SB_LINEDOWN),
	TABLEENTRY(SB_PAGEUP),
	TABLEENTRY(SB_PAGEDOWN),
	TABLEENTRY(SB_TOP),
	TABLEENTRY(SB_BOTTOM),
	TABLEENTRY(SB_THUMBPOSITION),
	TABLEENTRY(SB_THUMBTRACK),
	TABLEENTRY(SB_ENDSCROLL),
	TABLEEND
};

VALUETABLE tblWindowPos[] =
{
	TABLEENTRY(SWP_NOSIZE),
	TABLEENTRY(SWP_NOMOVE),
	TABLEENTRY(SWP_NOZORDER),
	TABLEENTRY(SWP_NOREDRAW),
	TABLEENTRY(SWP_NOACTIVATE),
	TABLEENTRY(SWP_DRAWFRAME),
	TABLEENTRY(SWP_SHOWWINDOW),
	TABLEENTRY(SWP_HIDEWINDOW),
	TABLEENTRY(SWP_NOCOPYBITS),
	TABLEENTRY(SWP_NOREPOSITION),
	TABLEEND
};

VALUETABLE tblCreateWindowPos[] =
{
	TABLEENTRY(CW_USEDEFAULT),
	TABLEEND
};

VALUETABLE tblTrueFalse[] =
{
	// didn't use TABLEENTRY because 
	// wanted True and False in mixed case.
	(UINT)TRUE, "True",
	(UINT)FALSE, "False",
	TABLEEND
};

static VALUETABLE tblActivate[] =
{
	TABLEENTRY(WA_ACTIVE),
	TABLEENTRY(WA_CLICKACTIVE),
	TABLEENTRY(WA_INACTIVE),
	TABLEEND
};

static VALUETABLE tblButtonNC[] =
{
	TABLEENTRY(BN_CLICKED),
	TABLEENTRY(BN_PAINT),
	TABLEENTRY(BN_HILITE),
	TABLEENTRY(BN_UNHILITE),
	TABLEENTRY(BN_DISABLE),
	TABLEENTRY(BN_DOUBLECLICKED),
	TABLEEND
};

static VALUETABLE tblComboboxNC[] =
{
	TABLEENTRY(CBN_ERRSPACE),
	TABLEENTRY(CBN_SELCHANGE),
	TABLEENTRY(CBN_DBLCLK),
	TABLEENTRY(CBN_SETFOCUS),
	TABLEENTRY(CBN_KILLFOCUS),
	TABLEENTRY(CBN_EDITCHANGE),
	TABLEENTRY(CBN_EDITUPDATE),
	TABLEENTRY(CBN_DROPDOWN),
	TABLEENTRY(CBN_CLOSEUP),
	TABLEEND
};

static VALUETABLE tblListboxNC[] =
{
	TABLEENTRY(LBN_ERRSPACE),
	TABLEENTRY(LBN_SELCHANGE),
	TABLEENTRY(LBN_DBLCLK),
	TABLEENTRY(LBN_SELCANCEL),
	TABLEENTRY(LBN_SETFOCUS),
	TABLEENTRY(LBN_KILLFOCUS),
	TABLEEND
};

static VALUETABLE tblEditNC[] =
{
	TABLEENTRY(EN_SETFOCUS),
	TABLEENTRY(EN_KILLFOCUS),
	TABLEENTRY(EN_CHANGE),
	TABLEENTRY(EN_UPDATE),
	TABLEENTRY(EN_ERRSPACE),
	TABLEENTRY(EN_MAXTEXT),
	TABLEENTRY(EN_HSCROLL),
	TABLEENTRY(EN_VSCROLL),
	TABLEEND
};

static VALUETABLE tblEnterIdle[] =
{
	TABLEENTRY(MSGF_DIALOGBOX),
	TABLEENTRY(MSGF_MENU),
	TABLEEND
};

static VALUETABLE tblDlgCode[] =
{
	TABLEENTRY(DLGC_BUTTON),
	TABLEENTRY(DLGC_DEFPUSHBUTTON),
	TABLEENTRY(DLGC_HASSETSEL),
	TABLEENTRY(DLGC_RADIOBUTTON),
	TABLEENTRY(DLGC_STATIC),
	TABLEENTRY(DLGC_UNDEFPUSHBUTTON),
	TABLEENTRY(DLGC_WANTALLKEYS),
	TABLEENTRY(DLGC_WANTARROWS),
	TABLEENTRY(DLGC_WANTCHARS),
//
// The DLGC_WANTMESSAGE value is the same as DLGC_WANTALLKEYS
//
//	TABLEENTRY(DLGC_WANTMESSAGE),
	TABLEENTRY(DLGC_WANTTAB),
	TABLEEND
};

static VALUETABLE tblMenuFlags[] =
{
	TABLEENTRY(MF_BITMAP),
	TABLEENTRY(MF_CHECKED),
	TABLEENTRY(MF_DISABLED),
	TABLEENTRY(MF_GRAYED),
	TABLEENTRY(MF_HILITE),
	TABLEENTRY(MF_MOUSESELECT),
	TABLEENTRY(MF_OWNERDRAW),
	TABLEENTRY(MF_POPUP),
	TABLEENTRY(MF_SYSMENU),
	TABLEEND
};

static VALUETABLE tblPower[] =
{
	TABLEENTRY(PWR_SUSPENDREQUEST),
	TABLEENTRY(PWR_SUSPENDRESUME),
	TABLEENTRY(PWR_CRITICALRESUME),
	TABLEEND
};

static VALUETABLE tblPowerRet[] =
{
	TABLEENTRY(PWR_OK),
	TABLEENTRY(PWR_FAIL),
	TABLEEND
};

static VALUETABLE tblShowWindow[] =
{
	TABLEENTRY(SW_PARENTCLOSING),
	TABLEENTRY(SW_OTHERZOOM),
	TABLEENTRY(SW_PARENTOPENING),
	TABLEENTRY(SW_OTHERUNZOOM),
	TABLEEND
};

static VALUETABLE tblSizeType[] =
{
	TABLEENTRY(SIZE_MAXIMIZED),
	TABLEENTRY(SIZE_MINIMIZED),
	TABLEENTRY(SIZE_RESTORED),
	TABLEENTRY(SIZE_MAXHIDE),
	TABLEENTRY(SIZE_MAXSHOW),
	TABLEEND
};

static VALUETABLE tblSpoolerStatus[] =
{
	TABLEENTRY(PR_JOBSTATUS),
	TABLEEND
};

static VALUETABLE tblSysCmds[] =
{
	TABLEENTRY(SC_SIZE),
	TABLEENTRY(SC_MOVE),
	TABLEENTRY(SC_MINIMIZE),
	TABLEENTRY(SC_MAXIMIZE),
	TABLEENTRY(SC_NEXTWINDOW),
	TABLEENTRY(SC_PREVWINDOW),
	TABLEENTRY(SC_CLOSE),
	TABLEENTRY(SC_VSCROLL),
	TABLEENTRY(SC_HSCROLL),
	TABLEENTRY(SC_MOUSEMENU),
	TABLEENTRY(SC_KEYMENU),
	TABLEENTRY(SC_ARRANGE),
	TABLEENTRY(SC_RESTORE),
	TABLEENTRY(SC_TASKLIST),
	TABLEENTRY(SC_SCREENSAVE),
	TABLEENTRY(SC_HOTKEY),
	TABLEENTRY(SC_ARRANGE),  // Is this constant obsolete?
	TABLEEND
};

static VALUETABLE tblOwnerDrawCtlType[] =
{
	TABLEENTRY(ODT_MENU),
	TABLEENTRY(ODT_LISTBOX),
	TABLEENTRY(ODT_COMBOBOX),
	TABLEENTRY(ODT_BUTTON),
	TABLEEND
};

static VALUETABLE tblOwnerDrawAction[] =
{
	TABLEENTRY(ODA_DRAWENTIRE),
	TABLEENTRY(ODA_SELECT),
	TABLEENTRY(ODA_FOCUS),
	TABLEEND
};

static VALUETABLE tblOwnerDrawState[] =
{
	TABLEENTRY(ODS_SELECTED),
	TABLEENTRY(ODS_GRAYED),
	TABLEENTRY(ODS_DISABLED),
	TABLEENTRY(ODS_CHECKED),
	TABLEENTRY(ODS_FOCUS),
	TABLEEND
};

static VALUETABLE tblOkCancel[] =
{
	TABLEENTRY(IDOK),
	TABLEENTRY(IDCANCEL),
	TABLEENTRY(IDABORT),
	TABLEENTRY(IDRETRY),
	TABLEENTRY(IDIGNORE),
	TABLEENTRY(IDYES),
	TABLEENTRY(IDNO),
	TABLEEND
};

static VALUETABLE tblNotifyCode[] =
{
	#ifndef DISABLE_WIN95_MESSAGES
	TABLEENTRY(NM_OUTOFMEMORY),
	TABLEENTRY(NM_CLICK),
	TABLEENTRY(NM_DBLCLK),
	TABLEENTRY(NM_RETURN),
	TABLEENTRY(NM_RCLICK),
	TABLEENTRY(NM_RDBLCLK),
	TABLEENTRY(NM_SETFOCUS),
	TABLEENTRY(NM_KILLFOCUS),
	TABLEENTRY(NM_STARTWAIT),
	TABLEENTRY(NM_ENDWAIT),
	TABLEENTRY(NM_BTNCLK),
	TABLEENTRY(LVN_ITEMCHANGING),
	TABLEENTRY(LVN_ITEMCHANGED),
	TABLEENTRY(LVN_INSERTITEM),
	TABLEENTRY(LVN_DELETEITEM),
	TABLEENTRY(LVN_DELETEALLITEMS),
	TABLEENTRY(LVN_BEGINLABELEDIT),
	TABLEENTRY(LVN_ENDLABELEDIT),
	TABLEENTRY(LVN_COLUMNCLICK),
	TABLEENTRY(LVN_BEGINDRAG),
	TABLEENTRY(LVN_ENDDRAG),
	TABLEENTRY(LVN_BEGINRDRAG),
	TABLEENTRY(LVN_ENDRDRAG),
//	TABLEENTRY(LVN_PEN),
	TABLEENTRY(LVN_GETDISPINFO),
	TABLEENTRY(LVN_SETDISPINFO),
	TABLEENTRY(LVN_KEYDOWN),
	TABLEENTRY(HDN_ITEMCHANGING),
	TABLEENTRY(HDN_ITEMCHANGED),
	TABLEENTRY(HDN_ITEMCLICK),
	TABLEENTRY(HDN_ITEMDBLCLICK),
	TABLEENTRY(HDN_DIVIDERDBLCLICK),
	TABLEENTRY(HDN_BEGINTRACK),
	TABLEENTRY(HDN_ENDTRACK),
	TABLEENTRY(HDN_TRACK),
	TABLEENTRY(TVN_SELCHANGING),
	TABLEENTRY(TVN_SELCHANGED),
	TABLEENTRY(TVN_GETDISPINFO),
	TABLEENTRY(TVN_SETDISPINFO),
	TABLEENTRY(TVN_ITEMEXPANDING),
	TABLEENTRY(TVN_ITEMEXPANDED),
	TABLEENTRY(TVN_BEGINDRAG),
	TABLEENTRY(TVN_BEGINRDRAG),
	TABLEENTRY(TVN_DELETEITEM),
	TABLEENTRY(TVN_BEGINLABELEDIT),
	TABLEENTRY(TVN_ENDLABELEDIT),
	TABLEENTRY(TVN_KEYDOWN),
	TABLEENTRY(TTN_NEEDTEXT),
	TABLEENTRY(TTN_SHOW),
	TABLEENTRY(TTN_POP),
	TABLEENTRY(TCN_KEYDOWN),
	TABLEENTRY(TCN_SELCHANGE),
	TABLEENTRY(TCN_SELCHANGING),
	TABLEENTRY(TBN_GETBUTTONINFO),
	TABLEENTRY(TBN_BEGINDRAG),
	TABLEENTRY(TBN_ENDDRAG),
	TABLEENTRY(TBN_BEGINADJUST),
	TABLEENTRY(TBN_ENDADJUST),
	TABLEENTRY(TBN_RESET),
	TABLEENTRY(TBN_QUERYINSERT),
	TABLEENTRY(TBN_QUERYDELETE),
	TABLEENTRY(TBN_TOOLBARCHANGE),
	TABLEENTRY(TBN_CUSTHELP),
	TABLEENTRY(UDN_DELTAPOS),
	TABLEENTRY(ACN_START),
	TABLEENTRY(ACN_STOP),
	#endif
	TABLEENTRY(TB_LINEUP),
	TABLEENTRY(TB_LINEDOWN),
	TABLEENTRY(TB_PAGEUP),
	TABLEENTRY(TB_PAGEDOWN),
	TABLEENTRY(TB_THUMBPOSITION),
	TABLEENTRY(TB_THUMBTRACK),
	TABLEENTRY(TB_TOP),
	TABLEENTRY(TB_BOTTOM),
	TABLEENTRY(TB_ENDTRACK),
	TABLEEND
};

static VALUETABLE tblStyleChange[] =
{
	TABLEENTRY(GWL_EXSTYLE),
	TABLEENTRY(GWL_STYLE),
	TABLEEND
};

#ifndef DISABLE_WIN95_MESSAGES

static VALUETABLE tblSizingFlags[] =
{
	TABLEENTRY(WMSZ_BOTTOM),
	TABLEENTRY(WMSZ_BOTTOMLEFT),
	TABLEENTRY(WMSZ_BOTTOMRIGHT),
	TABLEENTRY(WMSZ_LEFT),
	TABLEENTRY(WMSZ_RIGHT),
	TABLEENTRY(WMSZ_TOP),
	TABLEENTRY(WMSZ_TOPLEFT),
	TABLEENTRY(WMSZ_TOPRIGHT),
	TABLEEND
};

static VALUETABLE tblPrintFlags[] =
{
	TABLEENTRY(PRF_CHECKVISIBLE),
	TABLEENTRY(PRF_CHILDREN),
	TABLEENTRY(PRF_CLIENT),
	TABLEENTRY(PRF_ERASEBKGND),
	TABLEENTRY(PRF_NONCLIENT),
	TABLEENTRY(PRF_OWNED),
	TABLEEND
};

#endif

/*
static VALUETABLE tblDeviceChange[] =
{
	// TODO: activate this table/code for Chicago!
	TABLEENTRY(DBT_DeviceArrival),
	TABLEENTRY(DBT_DeviceQueryRemove),
	TABLEENTRY(DBT_DeviceQueryRemoveFailed),
	TABLEENTRY(DBT_DeviceRemovePending),
	TABLEENTRY(DBT_DeviceRemoveComplete),
	TABLEENTRY(DBT_ConfigChanged),
	TABLEENTRY(DBT_DevnodesChanged),
	TABLEEND
};
*/

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ACTIVATE)
{
	PARM(fActive, UINT, LOWP);
	PARM(fMinimized, BOOL, HIWP);
	PARM(hwnd, HWND, LP);

	TABLEORVALUE(tblActivate, fActive);
	POUTB(fMinimized);
	POUT(hwnd);

	return TRUE;
}

NODECODERET(WM_ACTIVATE);

NODECODEPARM(WM_ACTIVATE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ACTIVATEAPP)
{
	PARM(fActive, BOOL, WP);
	PARM(dwThreadID, DWORD, LP);

	POUTB(fActive);
	PLABEL(dwThreadID);
	MOUT << (DWORD)dwThreadID;

	return TRUE;
}

NODECODERET(WM_ACTIVATEAPP);

NODECODEPARM(WM_ACTIVATEAPP);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_CANCELJOURNAL);

NODECODERET(WM_CANCELJOURNAL);

NODECODEPARM(WM_CANCELJOURNAL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_CANCELMODE);

NODECODERET(WM_CANCELMODE);

NODECODEPARM(WM_CANCELMODE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_CLOSE);

NODECODERET(WM_CLOSE);

NODECODEPARM(WM_CLOSE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_COMMAND)
{
	PARM(wNotifyCode, UINT, HIWP);
	PARM(wID, UINT, LOWP);
	PARM(hwndCtl, HWND, LP);

	PLABEL(wNotifyCode);

	//
	// Is this message being sent from a control or from a menu/accelerator?
	//
	if (hwndCtl)
	{
		UINT nClass;

		PIFSIZEOF(PACK_CLASS)
		{
			PARM(ppc, PPACK_CLASS, ED);
			nClass = ppc->nClass;
		}
		else
		{
			nClass = CLS_UNKNOWN;
		}

		switch (nClass)
		{
			case CLS_BUTTON:
				TableOrValue(tblButtonNC, wNotifyCode);
				PLABEL(wID);
				TableOrInt(tblOkCancel, wID);
				break;

			case CLS_COMBOBOX:
				TableOrValue(tblComboboxNC, wNotifyCode);
				POUTC(wID, INT);
				break;

			case CLS_EDIT:
				TableOrValue(tblEditNC, wNotifyCode);
				POUTC(wID, INT);
				break;

			case CLS_LISTBOX:
				TableOrValue(tblListboxNC, wNotifyCode);
				POUTC(wID, INT);
				break;

			default:
				MOUT << (WORD)wNotifyCode;
				POUTC(wID, INT);
				break;
		}

		POUT(hwndCtl);
	}
	else
	{
		switch (wNotifyCode)
		{
			case 0:
				MOUT << '0' << ids(IDS_SENT_FROM_A_MENU);
				break;

			case 1:
				MOUT << '1' << ids(IDS_SENT_FROM_AN_ACCEL);
				break;

			default:
				MOUT << (WORD)wNotifyCode;
				break;
		}

		POUT(wID);
	}

	return TRUE;
}

NODECODERET(WM_COMMAND);

NODECODEPARM(WM_COMMAND);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_COMPACTING)
{
	PARM(wCompactRatio, UINT, WP);

	POUT(wCompactRatio);
	MOUT << " (" << ((100 * wCompactRatio) / 0x10000) << "%)";

	return TRUE;
}

NODECODERET(WM_COMPACTING);

NODECODEPARM(WM_COMPACTING);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_COMPAREITEM)
{
	PARM(idCtl, INT, WP);
	PARM(lpcis, LPCOMPAREITEMSTRUCT, LP);

	POUT(idCtl);
	POUTC(lpcis, DWORD);

	return TRUE;
}

DECODERET(WM_COMPAREITEM)
{
	PARM(fCompare, INT, RET);

	POUT(fCompare);
	switch (fCompare)
	{
		case -1:
			MOUT << ids(IDS_1_PRECEDES_2);
			break;

		case 0:
			MOUT << ids(IDS_1_MATCHES_2);
			break;

		case 1:
			MOUT << ids(IDS_1_FOLLOWS_2);
			break;
	}

	return TRUE;
}

DECODEPARM(WM_COMPAREITEM)
{
	P2WPOUT();

	P2LPOUTPTR(COMPAREITEMSTRUCT);
	P2IFSIZEOF(COMPAREITEMSTRUCT)
	{
		PARM(pcis, LPCOMPAREITEMSTRUCT, ED2);
		INDENT();
		P2TABLEORVALUE(tblOwnerDrawCtlType, pcis, CtlType);
		P2OUT(pcis, CtlID);
		P2OUT(pcis, hwndItem);
		P2OUT(pcis, itemID1);
		P2OUT(pcis, itemData1);
		P2OUT(pcis, itemID2);
		P2OUT(pcis, itemData2);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_COPYDATA)
{
	PARM(hwndFrom, HWND, WP);
	PARM(pcds, PCOPYDATASTRUCT, LP);

	POUT(hwndFrom);
	POUTC(pcds, DWORD);

	return TRUE;
}

DECODERET(WM_COPYDATA)
{
	PARM(fProcessed, BOOL, RET);

	POUTB(fProcessed);

	return TRUE;
}

DECODEPARM(WM_COPYDATA)
{
	P2WPOUT();

	P2LPOUTPTR(COPYDATASTRUCT);
	P2IFSIZEOF(COPYDATASTRUCT)
	{
		PARM(pcds, PCOPYDATASTRUCT, ED2);
		INDENT();
		P2OUT(pcds, dwData);
		P2OUTC(pcds, cbData, UINT);
		P2OUTC(pcds, lpData, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////
/*
// TODO: do some cracking here!
NODECODE(WM_CPL_LAUNCH);

NODECODERET(WM_CPL_LAUNCH);

NODECODEPARM(WM_CPL_LAUNCH);
*/
//////////////////////////////////////////////////////////////////////////////
/*
// TODO: do some cracking here!
NODECODE(WM_CPL_LAUNCHED);

NODECODERET(WM_CPL_LAUNCHED);

NODECODEPARM(WM_CPL_LAUNCHED);
*/
//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CREATE)
{
	PARM(lpcs, LPCREATESTRUCT, LP);

	POUTC(lpcs, DWORD);

	return TRUE;
}

DECODERET(WM_CREATE)
{
	PARM(fContinue, INT, RET);

	//
	// Note the return value is different from WM_NCCREATE.
	//
	POUT(fContinue);
	switch (fContinue)
	{
		case 0:
			MOUT << ids(IDS_CONTINUECREATION);
			break;

		case -1:
			MOUT << ids(IDS_DONTCONTINUECREATION);
			break;
	}

	return TRUE;
}

DECODEPARM(WM_CREATE)
{
	P2WPOUT();

	P2LPOUTPTR(CREATESTRUCT);

	P2IFSIZEOF(PACK_CREATESTRUCT)
	{
		PARM(ppcs, PPACK_CREATESTRUCT, ED2);
		PARM(pcs, LPCREATESTRUCT, &ppcs->cs);

		INDENT();
		P2OUTC(pcs, lpCreateParams, DWORD);
		P2OUTC(pcs, hInstance, DWORD);
		P2OUTC(pcs, hMenu, DWORD);
		P2OUT(pcs, hwndParent);
		P2TABLEORINT(tblCreateWindowPos, pcs, cy);
		P2TABLEORINT(tblCreateWindowPos, pcs, cx);
		P2TABLEORINT(tblCreateWindowPos, pcs, y);
		P2TABLEORINT(tblCreateWindowPos, pcs, x);
		P2TABLEFLAGS(tblWindowStyles, pcs, style);

		if (ppcs->fValidName)
		{
			PARM(pszName, LPSTR, ppcs->szName);
			P2OUTPTRSTR(pcs, lpszName);
			INDENT();
			P2OUTS(pszName);
			OUTDENT();
		}
		else
		{
			P2OUTC(pcs, lpszName, DWORD);
		}

		if (ppcs->fValidClass)
		{
			PARM(pszClass, LPSTR, ppcs->szClass);
			P2OUTPTRSTR(pcs, lpszClass);
			INDENT();
			P2OUTS(pszClass);
			OUTDENT();
		}
		else
		{
			CHAR szClass[50];

			if (GetClipboardFormatName((UINT)pcs->lpszClass, szClass,
				sizeof(szClass) / sizeof(CHAR)))
			{
				P2LABEL(lpszClass);
				M2OUT << (DWORD)pcs->lpszClass << " (Class: \""
					<< szClass << "\")";
				P2ENDLINE();
			}
			else
			{
				P2OUTC(pcs, lpszClass, DWORD);
			}
		}

		P2TABLEFLAGS(tblExtWindowStyles, pcs, dwExStyle);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CTLCOLORBTN)
{
	PARM(hdcButton, HDC, WP);
	PARM(hwndButton, HWND, LP);

	POUTC(hdcButton, DWORD);
	POUT(hwndButton);

	return TRUE;
}

DECODERET(WM_CTLCOLORBTN)
{
	PARM(hBrush, HBRUSH, RET);

	POUTC(hBrush, DWORD);

	return TRUE;
}

NODECODEPARM(WM_CTLCOLORBTN);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CTLCOLORDLG)
{
	PARM(hdcDlg, HDC, WP);
	PARM(hwndDlg, HWND, LP);

	POUTC(hdcDlg, DWORD);
	POUT(hwndDlg);

	return TRUE;
}

DECODERET(WM_CTLCOLORDLG)
{
	DECODERETLIKE(WM_CTLCOLORBTN);
}

NODECODEPARM(WM_CTLCOLORDLG);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CTLCOLOREDIT)
{
	PARM(hdcEdit, HDC, WP);
	PARM(hwndEdit, HWND, LP);

	POUTC(hdcEdit, DWORD);
	POUT(hwndEdit);

	return TRUE;
}

DECODERET(WM_CTLCOLOREDIT)
{
	DECODERETLIKE(WM_CTLCOLORBTN);
}

NODECODEPARM(WM_CTLCOLOREDIT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CTLCOLORLISTBOX)
{
	PARM(hdcLB, HDC, WP);
	PARM(hwndLB, HWND, LP);

	POUTC(hdcLB, DWORD);
	POUT(hwndLB);

	return TRUE;
}

DECODERET(WM_CTLCOLORLISTBOX)
{
	DECODERETLIKE(WM_CTLCOLORBTN);
}

NODECODEPARM(WM_CTLCOLORLISTBOX);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CTLCOLORMSGBOX)
{
	PARM(hdcMB, HDC, WP);
	PARM(hwndMB, HWND, LP);

	POUTC(hdcMB, DWORD);
	POUT(hwndMB);

	return TRUE;
}

DECODERET(WM_CTLCOLORMSGBOX)
{
	DECODERETLIKE(WM_CTLCOLORBTN);
}

NODECODEPARM(WM_CTLCOLORMSGBOX);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CTLCOLORSCROLLBAR)
{
	PARM(hdcSB, HDC, WP);
	PARM(hwndSB, HWND, LP);

	POUTC(hdcSB, DWORD);
	POUT(hwndSB);

	return TRUE;
}

DECODERET(WM_CTLCOLORSCROLLBAR)
{
	DECODERETLIKE(WM_CTLCOLORBTN);
}

NODECODEPARM(WM_CTLCOLORSCROLLBAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CTLCOLORSTATIC)
{
	PARM(hdcStatic, HDC, WP);
	PARM(hwndStatic, HWND, LP);

	POUTC(hdcStatic, DWORD);
	POUT(hwndStatic);

	return TRUE;
}

DECODERET(WM_CTLCOLORSTATIC)
{
	DECODERETLIKE(WM_CTLCOLORBTN);
}

NODECODEPARM(WM_CTLCOLORSTATIC);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DELETEITEM)
{
	PARM(idCtl, UINT, WP);
	PARM(lpdis, LPDELETEITEMSTRUCT, LP);

	POUT(idCtl);
	POUTC(lpdis, DWORD);

	return TRUE;
}

NODECODERET(WM_DELETEITEM);

DECODEPARM(WM_DELETEITEM)
{
	P2WPOUT();

	P2LPOUTPTR(DELETEITEMSTRUCT);
	P2IFSIZEOF(DELETEITEMSTRUCT)
	{
		PARM(pdis, LPDELETEITEMSTRUCT, ED2);
		INDENT();
		P2TABLEORVALUE(tblOwnerDrawCtlType, pdis, CtlType);
		P2OUT(pdis, CtlID);
		P2OUT(pdis, itemID);
		P2OUT(pdis, hwndItem);
		P2OUTC(pdis, itemData, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_DESTROY);

NODECODERET(WM_DESTROY);

NODECODEPARM(WM_DESTROY);

//////////////////////////////////////////////////////////////////////////////
/*
// TODO: activate this table/code for Chicago!
DECODE(WM_DEVICECHANGE)
{
	PARM(wParam, WPARAM, WP);

	PLABEL(wParam);

	TableOrValue(tblDeviceChange, wNotifyCode);
}

// TODO: activate this table/code for Chicago!
NODECODERET(WM_DEVICECHANGE);

// TODO: activate this table/code for Chicago!
NODECODEPARM(WM_DEVICECHANGE);
*/
//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DEVMODECHANGE)
{
	PARM(lpszDev, LPCTSTR, LP);

	POUTC(lpszDev, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

NODECODERET(WM_DEVMODECHANGE);

DECODEPARM(WM_DEVMODECHANGE)
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

DECODE(WM_DRAWITEM)
{
	PARM(idCtl, UINT, WP);
	PARM(lpdis, LPDRAWITEMSTRUCT, LP);

	POUT(idCtl);
	POUTC(lpdis, DWORD);

	return TRUE;
}

DECODERET(WM_DRAWITEM)
{
	PARM(fProcessed, BOOL, RET);

	POUTB(fProcessed);

	return TRUE;
}

DECODEPARM(WM_DRAWITEM)
{
	P2WPOUT();

	P2LPOUTPTR(DRAWITEMSTRUCT);
	P2IFSIZEOF(DRAWITEMSTRUCT)
	{
		PARM(pdis, LPDRAWITEMSTRUCT, ED2);
		INDENT();
		P2TABLEORVALUE(tblOwnerDrawCtlType, pdis, CtlType);
		P2OUT(pdis, CtlID);
		P2OUT(pdis, itemID);
		P2TABLEORVALUE(tblOwnerDrawAction, pdis, itemAction);
		P2TABLEORVALUE(tblOwnerDrawState, pdis, itemState);
		P2OUT(pdis, hwndItem);
		P2OUTC(pdis, hDC, DWORD);
		P2OUTRECT(pdis, rcItem);
		P2OUTC(pdis, itemData, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DROPFILES)
{
	PARM(hDrop, HANDLE, WP);

	POUTC(hDrop, DWORD);

	return TRUE;
}

NODECODERET(WM_DROPFILES);

NODECODEPARM(WM_DROPFILES);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ENABLE)
{
	PARM(fEnable, BOOL, WP);

	POUTB(fEnable);

	return TRUE;
}

NODECODERET(WM_ENABLE);

NODECODEPARM(WM_ENABLE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ENDSESSION)
{
	PARM(fEndSession, BOOL, WP);

	POUTB(fEndSession);

	return TRUE;
}

NODECODERET(WM_ENDSESSION);

NODECODEPARM(WM_ENDSESSION);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ENTERIDLE)
{
	PARM(fuSource, UINT, WP);
	PARM(hwnd, HWND, LP);

	TABLEORVALUE(tblEnterIdle, fuSource);
	POUT(hwnd);

	return TRUE;
}

NODECODERET(WM_ENTERIDLE);

NODECODEPARM(WM_ENTERIDLE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ENTERMENULOOP)
{
	PARM(fIsTrackPopupMenu, BOOL, WP);

	POUTB(fIsTrackPopupMenu);

	return TRUE;
}

NODECODERET(WM_ENTERMENULOOP);

NODECODEPARM(WM_ENTERMENULOOP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_EXITMENULOOP)
{
	DECODELIKE(WM_ENTERMENULOOP);
}

NODECODERET(WM_EXITMENULOOP);

NODECODEPARM(WM_EXITMENULOOP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ERASEBKGND)
{
	PARM(hdc, HDC, WP);

	POUTC(hdc, DWORD);

	return TRUE;
}

DECODERET(WM_ERASEBKGND)
{
	PARM(fErased, BOOL, RET);

	POUTB(fErased);

	return TRUE;
}

NODECODEPARM(WM_ERASEBKGND);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_FONTCHANGE);

NODECODERET(WM_FONTCHANGE);

NODECODEPARM(WM_FONTCHANGE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_GETDLGCODE);

DECODERET(WM_GETDLGCODE)
{
	PARM(fuDlgCode, UINT, RET);

	TABLEFLAGS(tblDlgCode, fuDlgCode);

	return TRUE;
}

NODECODEPARM(WM_GETDLGCODE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_GETFONT);

DECODERET(WM_GETFONT)
{
	PARM(hfont, HFONT, RET);

	POUTC(hfont, DWORD);
	if (hfont)
	{
		PIFSIZEOF(PACK_FONT)
		{
			PARM(ppf, PPACK_FONT, ED);

			PrintFont(ppf);
		}
	}
	else
	{
		MOUT << ids(IDS_SYSTEM_FONT);
	}

	return TRUE;
}

NODECODEPARM(WM_GETFONT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_GETMINMAXINFO)
{
	PARM(lpmmi, LPMINMAXINFO, LP);

	POUTC(lpmmi, DWORD);

	return TRUE;
}

DECODERET(WM_GETMINMAXINFO)
{
	PARM(lpmmi, LPMINMAXINFO, LP);

	POUTC(lpmmi, DWORD);

	return TRUE;
}

DECODEPARM(WM_GETMINMAXINFO)
{
	P2WPOUT();

	P2LPOUTPTR(MINMAXINFO);
	P2IFSIZEOF(MINMAXINFO)
	{
		PARM(pmmi, LPMINMAXINFO, ED2);
		INDENT();
		P2OUTPT(pmmi, ptReserved);
		P2OUTPT(pmmi, ptMaxSize);
		P2OUTPT(pmmi, ptMaxPosition);
		P2OUTPT(pmmi, ptMinTrackSize);
		P2OUTPT(pmmi, ptMaxTrackSize);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_GETTEXT)
{
	PARM(cchTextMax, INT, WP);
	PARM(lpszText, LPSTR, LP);

	POUT(cchTextMax);
	POUTC(lpszText, DWORD);

	return TRUE;
}

DECODERET(WM_GETTEXT)
{
	PARM(cchCopied, INT, RET);
	PARM(lpszText, LPSTR, LP);

	POUT(cchCopied);
	POUTC(lpszText, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		// REVIEW: is this the way to check for ordinal returned?  assuming that if
		// an ordinal is returned in lParam here--four bytes with no NULL?
		if (cchCopied == 4 && *(pstr + 4) != 0)
		{
			POUTC(pstr, DWORD);
		}
		else
		{
			POUTS(pstr);
		}
	}

	return TRUE;
}

DECODEPARM(WM_GETTEXT)
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

NODECODE(WM_GETTEXTLENGTH);

DECODERET(WM_GETTEXTLENGTH)
{
	PARM(cch, INT, RET);

	POUT(cch);

	return TRUE;
}

NODECODEPARM(WM_GETTEXTLENGTH);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_HSCROLL)
{
	PARM(fuScrollCode, UINT, LOWP);
	PARM(nPos, INT, (short)HIWP);
	PARM(hwndScrollBar, HWND, LP);

	TABLEORVALUE(tblHScroll, fuScrollCode);

	switch (fuScrollCode)
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			POUT(nPos);
			break;
	}

	POUT(hwndScrollBar);

	return TRUE;
}

NODECODERET(WM_HSCROLL);

NODECODEPARM(WM_HSCROLL);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_ICONERASEBKGND)
{
	PARM(hdc, HDC, WP);

	POUTC(hdc, DWORD);

	return TRUE;
}

NODECODERET(WM_ICONERASEBKGND);

NODECODEPARM(WM_ICONERASEBKGND);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_INITDIALOG)
{
	PARM(hwndFocus, HWND, WP);
	PARM(lInitParam, DWORD, LP);

	POUT(hwndFocus);
	POUT(lInitParam);

	return TRUE;
}

DECODERET(WM_INITDIALOG)
{
	PARM(fSetFocus, BOOL, RET);

	POUTB(fSetFocus);

	return TRUE;
}

NODECODEPARM(WM_INITDIALOG);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_INITMENU)
{
	PARM(hmenuInit, HMENU, WP);

	POUTC(hmenuInit, DWORD);

	return TRUE;
}

NODECODERET(WM_INITMENU);

NODECODEPARM(WM_INITMENU);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_INITMENUPOPUP)
{
	PARM(hmenuPopup, HMENU, WP);
	PARM(uPos, UINT, LOLP);
	PARM(fSystemMenu, BOOL, HILP);

	POUTC(hmenuPopup, DWORD);
	POUT(uPos);
	POUTB(fSystemMenu);

	return TRUE;
}

NODECODERET(WM_INITMENUPOPUP);

NODECODEPARM(WM_INITMENUPOPUP);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_KILLFOCUS)
{
	PARM(hwndGetFocus, HWND, WP);

	POUT(hwndGetFocus);

	return TRUE;
}

NODECODERET(WM_KILLFOCUS);

NODECODEPARM(WM_KILLFOCUS);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MEASUREITEM)
{
	PARM(idCtl, UINT, WP);
	PARM(lpmis, LPMEASUREITEMSTRUCT, LP);

	POUT(idCtl);
	POUTC(lpmis, DWORD);

	return TRUE;
}

DECODERET(WM_MEASUREITEM)
{
	PARM(fProcessed, BOOL, RET);

	POUTB(fProcessed);

	return TRUE;
}

DECODEPARM(WM_MEASUREITEM)
{
	P2WPOUT();

	P2LPOUTPTR(MEASUREITEMSTRUCT);
	P2IFSIZEOF(MEASUREITEMSTRUCT)
	{
		PARM(pmis, LPMEASUREITEMSTRUCT, ED2);
		INDENT();
		P2TABLEORVALUE(tblOwnerDrawCtlType, pmis, CtlType);
		P2OUT(pmis, CtlID);
		P2OUT(pmis, itemID);
		P2OUT(pmis, itemWidth);
		P2OUT(pmis, itemHeight);
		P2OUTC(pmis, itemData, DWORD);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MENUCHAR)
{
	PARM(chUser, CHAR, LOWP);
	PARM(fuFlag, UINT, HIWP);
	PARM(hmenu, HMENU, LP);

	MOUT << " chUser:'" << chUser
		<< "' (" << (INT)(UCHAR)chUser << ')';
	TABLEORVALUE(tblMenuFlags, fuFlag);
	POUTC(hmenu, DWORD);

	return TRUE;
}

DECODERET(WM_MENUCHAR)
{
	PARM(fAction, UINT, HIRET);
	PARM(nPos, UINT, LORET);

	POUT(fAction);
	switch (fAction)
	{
		case 0:
			MOUT << ids(IDS_DISCARD_CHAR_AND_BEEP);
			break;

		case 1:
			MOUT << ids(IDS_CLOSE_ACTIVE_MENU);
			break;

		case 2:
			MOUT << ids(IDS_SELECT_MENU_ITEM) << nPos << ')';
			break;
	}

	return TRUE;
}

NODECODEPARM(WM_MENUCHAR);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MENUSELECT)
{
	PARM(uItem, UINT, LOWP);
	PARM(fuFlags, UINT, HIWP);
	PARM(hmenu, HMENU, LP);

	POUT(uItem);
	PLABEL(fuFlags);
	if (fuFlags == 0xFFFF && hmenu == 0)
	{
		MOUT << "FFFF" << ids(IDS_MENU_WAS_CLOSED);
	}
	else
	{
		TableFlags(tblMenuFlags, fuFlags);
	}

	POUTC(hmenu, DWORD);

	return TRUE;
}

NODECODERET(WM_MENUSELECT);

NODECODEPARM(WM_MENUSELECT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MOVE)
{
	PARM(xPos, INT, LOLP);
	PARM(yPos, INT, HILP);

	POUT(xPos);
	POUT(yPos);

	return TRUE;
}

NODECODERET(WM_MOVE);

NODECODEPARM(WM_MOVE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NEXTDLGCTL)
{
	PARM(wCtlFocus, DWORD, WP);
	PARM(fHandle, BOOL, LOLP);

	if (fHandle)
	{
		POUTC(wCtlFocus, HWND);
		MOUT << ids(IDS_HANDLE_CONTROL_FOCUS);
	}
	else
	{
		POUT(wCtlFocus);
		if (wCtlFocus)
		{
			MOUT << ids(IDS_PREV_RECEIVES_FOCUS);
		}
		else
		{
			MOUT << ids(IDS_NEXT_RECEIVES_FOCUS);
		}
	}

	POUTB(fHandle);

	return TRUE;
}

NODECODERET(WM_NEXTDLGCTL);

NODECODEPARM(WM_NEXTDLGCTL);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_NOTIFY)
{
	PARM(idCtrl, UINT, WP);
	PARM(pnmh, NMHDR FAR *, LP);

	POUT(idCtrl);
	POUTC(pnmh, DWORD);

	return TRUE;
}

NODECODERET(WM_NOTIFY);

DECODEPARM(WM_NOTIFY)
{
	P2WPOUT();

	P2LPOUTPTR(NMHDR);
	P2IFSIZEOF(NMHDR)
	{
		PARM(pnmh, NMHDR FAR *, ED2);
		INDENT();
		P2OUT(pnmh, hwndFrom);
		P2OUT(pnmh, idFrom);
		P2TABLEORVALUE(tblNotifyCode, pnmh, code);
	}
}

//////////////////////////////////////////////////////////////////////////////
NODECODE(WM_NULL);

NODECODERET(WM_NULL);

NODECODEPARM(WM_NULL);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_PAINT);

NODECODERET(WM_PAINT);

NODECODEPARM(WM_PAINT);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_PAINTICON);

NODECODERET(WM_PAINTICON);

NODECODEPARM(WM_PAINTICON);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_PALETTECHANGED)
{
	PARM(hwndPalChg, HWND, WP);

	POUT(hwndPalChg);

	return TRUE;
}

NODECODERET(WM_PALETTECHANGED);

NODECODEPARM(WM_PALETTECHANGED);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_PALETTEISCHANGING)
{
	PARM(hwndRealize, HWND, WP);

	POUT(hwndRealize);

	return TRUE;
}

NODECODERET(WM_PALETTEISCHANGING);

NODECODEPARM(WM_PALETTEISCHANGING);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_POWER)
{
	PARM(fwPowerEvt, UINT, WP);

	TABLEORVALUE(tblPower, fwPowerEvt);

	return TRUE;
}

DECODERET(WM_POWER)
{
	PARM(lResult, UINT, RET);

	TABLEORVALUE(tblPowerRet, lResult);

	return TRUE;
}

NODECODEPARM(WM_POWER);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_QUERYDRAGICON);

DECODERET(WM_QUERYDRAGICON)
{
	PARM(hcur, HCURSOR, RET);

	POUTC(hcur, DWORD);
	if (!hcur)
	{
		MOUT << ids(IDS_USE_DEFAULT_CURSOR);
	}

	return TRUE;
}

NODECODEPARM(WM_QUERYDRAGICON);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_QUERYENDSESSION)
{
	PARM(nSource, UINT, WP);

	POUT(nSource);
	if (nSource)
	{
		MOUT << ids(IDS_END_FROM_TASK_LIST);
	}
	else
	{
		MOUT << ids(IDS_LOGOFF_OR_SHUTDOWN);
	}

	return TRUE;
}

DECODERET(WM_QUERYENDSESSION)
{
	PARM(fShutdownIsOk, BOOL, RET);

	POUTB(fShutdownIsOk);

	return TRUE;
}

NODECODEPARM(WM_QUERYENDSESSION);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_QUERYNEWPALETTE);

DECODERET(WM_QUERYNEWPALETTE)
{
	PARM(fLogicalPalRealized, BOOL, RET);

	POUTB(fLogicalPalRealized);

	return TRUE;
}

NODECODEPARM(WM_QUERYNEWPALETTE);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_QUERYOPEN);

DECODERET(WM_QUERYOPEN)
{
	PARM(fOpenIsOk, BOOL, RET);

	POUTB(fOpenIsOk);

	return TRUE;
}

NODECODEPARM(WM_QUERYOPEN);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_QUEUESYNC);

NODECODERET(WM_QUEUESYNC);

NODECODEPARM(WM_QUEUESYNC);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_QUIT)
{
	PARM(nExitCode, INT, WP);

	POUT(nExitCode);

	return TRUE;
}

NODECODERET(WM_QUIT);

NODECODEPARM(WM_QUIT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SETFOCUS)
{
	PARM(hwndLoseFocus, HWND, WP);

	POUT(hwndLoseFocus);

	return TRUE;
}

NODECODERET(WM_SETFOCUS);

NODECODEPARM(WM_SETFOCUS);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SETFONT)
{
	PARM(hfont, HFONT, WP);
	PARM(fRedraw, BOOL, LP);

	POUTC(hfont, DWORD);
	if (hfont)
	{
		PIFSIZEOF(PACK_FONT)
		{
			PARM(ppf, PPACK_FONT, ED);

			PrintFont(ppf);
		}
	}
	else
	{
		MOUT << ids(IDS_USE_DEFAULT_FONT);
	}

	POUTB(fRedraw);

	return TRUE;
}

NODECODERET(WM_SETFONT);

NODECODEPARM(WM_SETFONT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SETREDRAW)
{
	PARM(fRedraw, BOOL, WP);

	POUTB(fRedraw);

	return TRUE;
}

NODECODERET(WM_SETREDRAW);

NODECODEPARM(WM_SETREDRAW);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SETTEXT)
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

DECODERET(WM_SETTEXT)
{
	PARM(fSuccess, INT, RET);

	CHAR szClass[64];
	GetClassName(pmsd->hwnd, szClass, sizeof(szClass));

	if (!_tcsicmp(szClass, "COMBOBOX"))
	{
		if (fSuccess < 0)
		{
			TABLEORINT(tblCBRet, fSuccess);
		}
		else
		{
			TABLEORINT(tblTrueFalse, fSuccess);
		}
	}
	else if (!_tcsicmp(szClass, "LISTBOX"))
	{
		if (fSuccess < 0)
		{
			TABLEORINT(tblLBRet, fSuccess);
		}
		else
		{
			TABLEORINT(tblTrueFalse, fSuccess);
		}
	}
	else
		TABLEORINT(tblTrueFalse, fSuccess);

	return TRUE;
}

DECODEPARM(WM_SETTEXT)
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

DECODE(WM_SHOWWINDOW)
{
	PARM(fShow, BOOL, WP);
	PARM(fuStatus, UINT, LP);

	POUTB(fShow);

	PLABEL(fuStatus);
	if (fuStatus == 0)
	{
		MOUT << '0' << ids(IDS_SHOWWINDOW_CALLED);
	}
	else
	{
		TableOrValue(tblShowWindow, fuStatus);
	}

	return TRUE;
}

NODECODERET(WM_SHOWWINDOW);

NODECODEPARM(WM_SHOWWINDOW);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SIZE)
{
	PARM(fwSizeType, UINT, WP);
	PARM(nWidth, UINT, LOLP);
	PARM(nHeight, UINT, HILP);

	TABLEORVALUE(tblSizeType, fwSizeType);
	POUTC(nWidth, INT);
	POUTC(nHeight, INT);

	return TRUE;
}

NODECODERET(WM_SIZE);

NODECODEPARM(WM_SIZE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SPOOLERSTATUS)
{
	PARM(fwJobStatus, UINT, WP);
	PARM(cJobsLeft, UINT, LOLP);

	TABLEORVALUE(tblSpoolerStatus, fwJobStatus);
	POUTC(cJobsLeft, INT);

	return TRUE;
}

NODECODERET(WM_SPOOLERSTATUS);

NODECODEPARM(WM_SPOOLERSTATUS);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_SYSCOLORCHANGE);

NODECODERET(WM_SYSCOLORCHANGE);

NODECODEPARM(WM_SYSCOLORCHANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SYSCOMMAND)
{
	PARM(uCmdType, UINT, WP);
	PARM(xPos, UINT, LOLP);
	PARM(yPos, UINT, HILP);

	PLABEL(uCmdType);
	TableOrValue(tblSysCmds, uCmdType & 0xFFF0);
	POUTC(xPos, INT);
	POUTC(yPos, INT);

	return TRUE;
}

NODECODERET(WM_SYSCOMMAND);

NODECODEPARM(WM_SYSCOMMAND);

//////////////////////////////////////////////////////////////////////////////

NODECODE(WM_TIMECHANGE);

NODECODERET(WM_TIMECHANGE);

NODECODEPARM(WM_TIMECHANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_TIMER)
{
	PARM(wTimerID, INT, WP);
//
// This message should never be seen by the app if tmprc is not
// NULL, so there is no reason to decode it...
//
//	PARM(tmprc, DWORD, LP);

	POUT(wTimerID);

	return TRUE;
}

NODECODERET(WM_TIMER);

NODECODEPARM(WM_TIMER);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_VSCROLL)
{
	PARM(fuScrollCode, UINT, LOWP);
	PARM(nPos, INT, (short)HIWP);
	PARM(hwndScrollBar, HWND, LP);

	TABLEORVALUE(tblVScroll, fuScrollCode);

	switch (fuScrollCode)
	{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			POUT(nPos);
			break;
	}

	POUT(hwndScrollBar);

	return TRUE;
}

NODECODERET(WM_VSCROLL);

NODECODEPARM(WM_VSCROLL);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_WINDOWPOSCHANGED)
{
	PARM(lpwp, LPWINDOWPOS, LP);

	POUTC(lpwp, DWORD);

	return TRUE;
}

NODECODERET(WM_WINDOWPOSCHANGED);

DECODEPARM(WM_WINDOWPOSCHANGED)
{
	P2WPOUT();

	P2LPOUTPTR(WINDOWPOS);
	P2IFSIZEOF(WINDOWPOS)
	{
		PARM(pwp, LPWINDOWPOS, ED2);
		INDENT();
		P2OUT(pwp, hwnd);
		P2OUT(pwp, hwndInsertAfter);
		P2OUT(pwp, x);
		P2OUT(pwp, y);
		P2OUT(pwp, cx);
		P2OUT(pwp, cy);
		P2TABLEFLAGS(tblWindowPos, pwp, flags);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_WINDOWPOSCHANGING)
{
	DECODELIKE(WM_WINDOWPOSCHANGED);
}

NODECODERET(WM_WINDOWPOSCHANGING);

DECODEPARM(WM_WINDOWPOSCHANGING)
{
	DECODEPARMLIKE(WM_WINDOWPOSCHANGED);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_WININICHANGE)
{
	PARM(pszSection, LPCTSTR, LP);

	POUTC(pszSection, DWORD);
	PIFDATA()
	{
		PARM(pstr, LPSTR, ED);
		POUTS(pstr);
	}

	return TRUE;
}

NODECODERET(WM_WININICHANGE);

DECODEPARM(WM_WININICHANGE)
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

DECODE(WM_INPUTLANGCHANGEREQUEST)
{
	PARM(fSysCharset, BOOL, WP);
	PARM(hkl, HKL, LP);

	POUTB(fSysCharset);
	POUTC(hkl, DWORD);

	return TRUE;
}

NODECODERET(WM_INPUTLANGCHANGEREQUEST);

NODECODEPARM(WM_INPUTLANGCHANGEREQUEST);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_INPUTLANGCHANGE)
{
	PARM(charset, WPARAM, WP);
	PARM(hkl, HKL, LP);

	POUT(charset);
	POUTC(hkl, DWORD);

	return TRUE;
}

NODECODERET(WM_INPUTLANGCHANGE);

NODECODEPARM(WM_INPUTLANGCHANGE);

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_TCARD);

NODECODERET(WM_TCARD);

NODECODEPARM(WM_TCARD);

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_HELP);

NODECODERET(WM_HELP);

NODECODEPARM(WM_HELP);

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_USERCHANGED);

NODECODERET(WM_USERCHANGED);

NODECODEPARM(WM_USERCHANGED);

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_NOTIFYFORMAT);

NODECODERET(WM_NOTIFYFORMAT);

NODECODEPARM(WM_NOTIFYFORMAT);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CONTEXTMENU)
{
	PARM(hwnd, HWND, WP);
	PARM(xPos, WORD, LOLP);
	PARM(yPos, WORD, HILP);

	POUTC(hwnd, DWORD);
	POUTC(xPos, UINT);
	POUTC(yPos, UINT);

	return TRUE;
}

NODECODERET(WM_CONTEXTMENU);

NODECODEPARM(WM_CONTEXTMENU);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_STYLECHANGING)
{
	PARM(wStyleType, WPARAM, WP);
	PARM(lpss, DWORD, LP);

	TABLEFLAGS(tblStyleChange, wStyleType);
	POUTC(lpss, DWORD);

	return TRUE;
}

NODECODERET(WM_STYLECHANGING);

DECODEPARM(WM_STYLECHANGING)
{
	P2WPOUT();

#ifndef DISABLE_WIN95_MESSAGES
	P2LPOUTPTR(STYLESTRUCT);
	P2IFSIZEOF(STYLESTRUCT)
	{
		PARM(lpss, LPSTYLESTRUCT, ED2);
		INDENT();
		P2TABLEFLAGS(tblWindowStyles, lpss, styleOld);
		P2TABLEFLAGS(tblWindowStyles, lpss, styleNew);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_STYLECHANGED)
{
	DECODELIKE(WM_STYLECHANGING);
}

DECODERET(WM_STYLECHANGED)
{
	DECODERETLIKE(WM_STYLECHANGING);
}

DECODEPARM(WM_STYLECHANGED)
{
	DECODEPARMLIKE(WM_STYLECHANGING);
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_DISPLAYCHANGE)
{
	PARM(fChanged, BOOL, WP);
	PARM(cxScreen, WORD, LOLP);
	PARM(cyScreen, WORD, HILP);

	POUTB(fChanged);
	POUTC(cxScreen, UINT);
	POUTC(cyScreen, UINT);

	return TRUE;
}

NODECODERET(WM_DISPLAYCHANGE);

NODECODEPARM(WM_DISPLAYCHANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_GETICON)
{
	PARM(fType, BOOL, WP);

	POUTB(fType);

	return TRUE;
}

DECODERET(WM_GETICON)
{
	PARM(hicon, HANDLE, RET);

	POUTC(hicon, DWORD);

	return TRUE;
}

NODECODEPARM(WM_GETICON);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SETICON)
{
	PARM(fType, BOOL, WP);
	PARM(hicon, HANDLE, LP);

	POUTB(fType);
	POUTC(hicon, DWORD);

	return TRUE;
}

DECODERET(WM_SETICON)
{
	PARM(hPrevIcon, HANDLE, RET);

	POUTC(hPrevIcon, DWORD);

	return TRUE;
}

NODECODEPARM(WM_SETICON);

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_NEXTMENU);

NODECODERET(WM_NEXTMENU);

NODECODEPARM(WM_NEXTMENU);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_SIZING)
{
	PARM(fwSide, WPARAM, WP);
	PARM(lprc, LPRECT, LP);

#ifndef DISABLE_WIN95_MESSAGES
	TABLEFLAGS(tblSizingFlags, fwSide);
#endif
	POUTC(lprc, DWORD);

	return TRUE;
}

NODECODERET(WM_SIZING);

DECODEPARM(WM_SIZING)
{
	P2WPOUT();

	P2LPOUTPTR(RECT);
	P2IFSIZEOF(RECT)
	{
		PARM(lprc, LPRECT, ED2);
		INDENT();
		P2OUT(lprc, top);
		P2OUT(lprc, bottom);
		P2OUT(lprc, left);
		P2OUT(lprc, right);
	}
}

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_CAPTURECHANGED)
{
	PARM(hwndNewCapture, HWND, LP);

	POUTC(hwndNewCapture, DWORD);

	return TRUE;
}

NODECODERET(WM_CAPTURECHANGED);

NODECODEPARM(WM_CAPTURECHANGED);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_MOVING)
{
	DECODELIKE(WM_SIZING);
}

DECODERET(WM_MOVING)
{
	DECODERETLIKE(WM_SIZING);
}

DECODEPARM(WM_MOVING)
{
	DECODEPARMLIKE(WM_SIZING);
}

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_POWERBROADCAST);

NODECODERET(WM_POWERBROADCAST);

NODECODEPARM(WM_POWERBROADCAST);

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_DEVICECHANGE);

NODECODERET(WM_DEVICECHANGE);

NODECODEPARM(WM_DEVICECHANGE);

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_SETTINGCHANGE);

NODECODERET(WM_SETTINGCHANGE);

NODECODEPARM(WM_SETTINGCHANGE);

//////////////////////////////////////////////////////////////////////////////

DECODE(WM_PRINT)
{
	PARM(hdc, HDC, WP);
	PARM(uFlags, LPARAM, LP);

	POUTC(hdc, DWORD);
#ifndef DISABLE_WIN95_MESSAGES
	TABLEFLAGS(tblPrintFlags, uFlags);
#endif
	return TRUE;
}

NODECODERET(WM_PRINT);

NODECODEPARM(WM_PRINT);

//////////////////////////////////////////////////////////////////////////////
// TODO: find information about this message
NODECODE(WM_PRINTCLIENT);

NODECODERET(WM_PRINTCLIENT);

NODECODEPARM(WM_PRINTCLIENT);

//////////////////////////////////////////////////////////////////////////////
