// propinsp.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "proptab.h"
#include "hotlinkc.h"
#include "wndptabs.h"
#include "prcptabs.h"
#include "thdptabs.h"
#include "msgptabs.h"
#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Support for the last selected object maintenance.

static DWORD g_dwObjectLast = 0;
static int g_nObjectTypeLast = OT_NONE;
static BOOL g_bSpyImmediately = FALSE;

void SetLastSelectedObject(DWORD dwObject, int nObjectType)
{
	g_dwObjectLast = dwObject;
	g_nObjectTypeLast = nObjectType;
}

DWORD GetLastSelectedObject()
{
	return g_dwObjectLast;
}

int GetLastSelectedObjectType()
{
	return g_nObjectTypeLast;
}

void ChangeToLastSelectedObject()
{
	if (g_pInspector && g_nObjectTypeLast != OT_NONE)
	{
		g_pInspector->ChangeObject(g_dwObjectLast, g_nObjectTypeLast);
	}
}

void SetSpyImmediate(BOOL bDirect)
{
	g_bSpyImmediately = bDirect;
}

BOOL GetSpyImmediate()
{
	return g_bSpyImmediately;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyInspector

//
// Global pointer to the one instance of the Properties Inspector.
//
CPropertyInspector* g_pInspector = NULL;

void CPropertyInspector::ShowObjectProperties(DWORD dwObject, int nObjectType)
{
	if (!g_pInspector)
	{
		int iSelectTab;

		switch (nObjectType)
		{
			case OT_WINDOW:
				iSelectTab = theApp.GetWinTabCur();
				break;

			case OT_PROCESS:
				iSelectTab = theApp.GetPrcTabCur();
				break;

			case OT_THREAD:
				iSelectTab = theApp.GetThdTabCur();
				break;

			case OT_MESSAGE:
				iSelectTab = theApp.GetMsgTabCur();
				break;
		}

		CPropertyInspector* pDlg = new CPropertyInspector(IDS_PROPERTYINSPECTOR, NULL, iSelectTab);

		pDlg->SetObject(dwObject, nObjectType);
		pDlg->Create();
	}
	else
	{
		g_pInspector->ChangeObject(dwObject, nObjectType);
	}
}

CPropertyInspector::CPropertyInspector(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectTab, COMMIT_MODEL commitModel)
	: CTabbedDialog(nIDCaption, pParentWnd, iSelectTab, commitModel)//, m_ProcessDatabase(&m_pdbError)
{
	SetValidObjectFlag(FALSE);
	m_nObjectType = OT_NONE;
	m_dwObject = (DWORD)-1;
	m_fObjectTypeChanged = TRUE;

	g_pInspector = this;
}

CPropertyInspector::~CPropertyInspector()
{
	switch (m_nObjectType)
	{
		case OT_WINDOW:
			theApp.SetWinTabCur(m_nTabCur);
			break;

		case OT_PROCESS:
			theApp.SetPrcTabCur(m_nTabCur);
			break;

		case OT_THREAD:
			theApp.SetThdTabCur(m_nTabCur);
			break;

		case OT_MESSAGE:
			theApp.SetMsgTabCur(m_nTabCur);
			break;
	}

	g_pInspector = NULL;
}

VALUETABLE CPropertyInspector::m_astClass[] =
{
	TABLEENTRY(CS_GLOBALCLASS),
	TABLEENTRY(CS_BYTEALIGNWINDOW),
	TABLEENTRY(CS_BYTEALIGNCLIENT),
	TABLEENTRY(CS_SAVEBITS),
	TABLEENTRY(CS_NOCLOSE),
	TABLEENTRY(CS_NOKEYCVT),
	TABLEENTRY(CS_PARENTDC),		// TODO: is this obsolete?
	TABLEENTRY(CS_CLASSDC),
	TABLEENTRY(CS_OWNDC),
	TABLEENTRY(CS_DBLCLKS),
	TABLEENTRY(CS_KEYCVTWINDOW),	// TODO: is this obsolete?
	TABLEENTRY(CS_HREDRAW),
	TABLEENTRY(CS_VREDRAW),
	0, NULL
};

VALUETABLE CPropertyInspector::m_astColor[] =
{
	TABLEENTRY(COLOR_SCROLLBAR),
	TABLEENTRY(COLOR_BACKGROUND),
	TABLEENTRY(COLOR_ACTIVECAPTION),
	TABLEENTRY(COLOR_INACTIVECAPTION),
	TABLEENTRY(COLOR_MENU),
	TABLEENTRY(COLOR_WINDOW),
	TABLEENTRY(COLOR_WINDOWFRAME),
	TABLEENTRY(COLOR_MENUTEXT),
	TABLEENTRY(COLOR_WINDOWTEXT),
	TABLEENTRY(COLOR_CAPTIONTEXT),
	TABLEENTRY(COLOR_ACTIVEBORDER),
	TABLEENTRY(COLOR_INACTIVEBORDER),
	TABLEENTRY(COLOR_APPWORKSPACE),
	TABLEENTRY(COLOR_HIGHLIGHT),
	TABLEENTRY(COLOR_HIGHLIGHTTEXT),
	TABLEENTRY(COLOR_BTNFACE),
	TABLEENTRY(COLOR_BTNSHADOW),
	TABLEENTRY(COLOR_GRAYTEXT),
	TABLEENTRY(COLOR_BTNTEXT),
	TABLEENTRY(COLOR_INACTIVECAPTIONTEXT),
	TABLEENTRY(COLOR_BTNHIGHLIGHT),
	0, NULL
};

HANDLESTABLE CPropertyInspector::m_ahtSystemCursors[] =
{
	HANDLEENTRY(IDC_ARROW),
	HANDLEENTRY(IDC_IBEAM),
	HANDLEENTRY(IDC_WAIT),
	HANDLEENTRY(IDC_CROSS),
	HANDLEENTRY(IDC_UPARROW),
//	HANDLEENTRY(IDC_SIZE),
//	HANDLEENTRY(IDC_ICON),
	HANDLEENTRY(IDC_SIZENWSE),
	HANDLEENTRY(IDC_SIZENESW),
	HANDLEENTRY(IDC_SIZEWE),
	HANDLEENTRY(IDC_SIZENS),
	HANDLEENTRY(IDC_SIZEALL),
	HANDLEENTRY(IDC_NO),
	HANDLEENTRY(IDC_APPSTARTING),
	HANDLEENTRYLAST
};

HANDLESTABLE CPropertyInspector::m_ahtSystemIcons[] =
{
	HANDLEENTRY(IDI_APPLICATION),
	HANDLEENTRY(IDI_HAND),
	HANDLEENTRY(IDI_QUESTION),
	HANDLEENTRY(IDI_EXCLAMATION),
	HANDLEENTRY(IDI_ASTERISK),
	HANDLEENTRYLAST
};

void CPropertyInspector::InitializeTables()
{
	HANDLESTABLE *pht;

	// Initialize the system cursor and icon table.  The system
	// cursors and icons will have the same handle value in all
	// processes.
	//
	// dolphin 11776 [patbr] don't DestroyCursor()/DestroyIcon the 
	// cursors/icons loaded here--improper, plus RIPs on Chicago.

	pht = m_ahtSystemCursors;
	while (pht->pszName)
	{
		pht->handle = (HANDLE)::LoadCursor(NULL, pht->idObject);
		pht++;
	}

	pht = m_ahtSystemIcons;
	while (pht->pszName)
	{
		pht->handle = (HANDLE)::LoadIcon(NULL, pht->idObject);
		pht++;
	}
}

BOOL CPropertyInspector::SetObject(DWORD dwObject, int nObjectType)
{
	BOOL fResult;

	switch (nObjectType)
	{
		case OT_WINDOW:
			fResult = SetWindowObject((HWND)dwObject);
			break;

		case OT_PROCESS:
			fResult = SetProcessObject(dwObject);
			break;

		case OT_THREAD:
			fResult = SetThreadObject(dwObject);
			break;

		case OT_MESSAGE:
			fResult = SetMessageObject((PMSGSTREAMDATA2)dwObject);
			break;

		case OT_NONE:
			fResult = SetNoObject();
			break;

		default:
			ASSERT(FALSE);
			fResult = FALSE;
			break;
	}

	m_dwObject = dwObject;
	m_nObjectType = nObjectType;

	return fResult;
}

BOOL CPropertyInspector::SetObjectType(int nObjectType)
{
	if ((nObjectType == m_nObjectType) && (m_tabRow.MaxTab() >= 0))
	{
		return(FALSE);
	}

	switch (m_nObjectType)
	{
		case OT_WINDOW:
			theApp.SetWinTabCur(m_nTabCur);
			break;

		case OT_PROCESS:
			theApp.SetPrcTabCur(m_nTabCur);
			break;

		case OT_THREAD:
			theApp.SetThdTabCur(m_nTabCur);
			break;

		case OT_MESSAGE:
			theApp.SetMsgTabCur(m_nTabCur);
			break;
	}

	ClearAllTabs();

	m_fObjectTypeChanged = TRUE;

	switch (nObjectType)
	{
		case OT_WINDOW:
			nCaptionID = IDS_WINDOWPROPERTIES;
			AddTab(new CWindowGeneralPropTab(IDD_WINDOWGENERALTAB, IDS_GENERALTAB));
			AddTab(new CWindowStylesPropTab(IDD_WINDOWSTYLESTAB, IDS_STYLESTAB));
			AddTab(new CWindowWindowsPropTab(IDD_WINDOWWINDOWSTAB, IDS_WINDOWSTAB));
			AddTab(new CWindowClassPropTab(IDD_WINDOWCLASSTAB, IDS_CLASSTAB));
			AddTab(new CWindowProcessPropTab(IDD_WINDOWPROCESSTAB, IDS_PROCESSTAB));
			break;

		case OT_PROCESS:
			nCaptionID = IDS_PROCESSPROPERTIES;
			AddTab(new CProcessGeneralPropTab(IDD_PROCESSGENERALTAB, IDS_GENERALTAB));
			if (!theApp.IsChicago())
			{
				AddTab(new CProcessMemoryPropTab(IDD_PROCESSMEMORYTAB, IDS_MEMORYTAB));
				AddTab(new CProcessPageFilePropTab(IDD_PROCESSPAGEFILETAB, IDS_PAGEFILETAB));
				AddTab(new CProcessSpacePropTab(IDD_PROCESSSPACETAB, IDS_SPACETAB));
			//	AddTab(new CProcessFileIOPropTab(IDD_PROCESSFILEIOTAB, IDS_FILEIOTAB));
			}
			break;

		case OT_THREAD:
			nCaptionID = IDS_THREADPROPERTIES;
			AddTab(new CThreadGeneralPropTab(IDD_THREADGENERALTAB, IDS_GENERALTAB));
			break;

		case OT_MESSAGE:
			nCaptionID = IDS_MESSAGEPROPERTIES;
			AddTab(new CMessageGeneralPropTab(IDD_MESSAGEGENERALTAB, IDS_GENERALTAB));
			break;

		case OT_NONE:
			nCaptionID = IDS_PROPERTYINSPECTOR;
			break;

		default:
			ASSERT(FALSE);
			break;
	}

	m_strCaption.LoadString(nCaptionID);

	return(TRUE);
}

BOOL CPropertyInspector::SetNoObject()
{
	return(SetObjectType(OT_NONE));
}

BOOL CPropertyInspector::SetWindowObject(HWND hwnd)
{
	BOOL fRet = SetObjectType(OT_WINDOW);
	m_hwnd = hwnd;
	return(fRet);
}

BOOL CPropertyInspector::RefreshWindowObjectData()
{
	CHAR szBuf[260];
	CHAR szClass[260];
	RECT rc;
	RECT rcClient;
	POINT pt;
	BOOL fGotOPD;
	OTHERPROCESSDATA opd;
	WORD w;
	DWORD dw;
	int cb;
	LPSTR psz;
	BOOL fOK;
	int i;
	int cbWndExtra;

	//
	// InitializeTables() must be called first!
	//
	if (!IsWindow(m_hwnd) || !::GetClassName(m_hwnd, szClass, sizeof(szClass) / sizeof(CHAR)))
	{
		return FALSE;
	}

	//
	// Try to obtain the other process data.  If this fails, we will
	// have to try and get our values another way.
	//
	fGotOPD = GetOtherProcessData(m_hwnd, &opd);

	//
	// Get the text.  Handle ordinal values properly.
	//
	*szBuf = 0;
	::GetWindowText(m_hwnd, szBuf, sizeof(szBuf) / sizeof(CHAR));
	if (*(PWORD)szBuf == 0xFFFF)
	{
		i = (INT)(*((PWORD)(&szBuf[2])));
		wsprintf(szBuf, "#%d", i);
	}
	m_strWindowCaption = szBuf;

	::GetWindowRect(m_hwnd, &rc);
	wsprintf(szBuf, "(%d, %d)-(%d, %d), %dx%d", rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top);
	if (::IsIconic(m_hwnd))
	{
		_tcscat(szBuf, ids(IDS_MINIMIZED));
	}
	else if (::IsZoomed(m_hwnd))
	{
		_tcscat(szBuf, ids(IDS_MAXIMIZED));
	}
	m_strRectangle = szBuf;

	::GetClientRect(m_hwnd, &rcClient);
	pt.x = 0;
	pt.y = 0;
	::ClientToScreen(m_hwnd, &pt);
	::OffsetRect(&rcClient, pt.x - rc.left, pt.y - rc.top);
	wsprintf(szBuf, "(%d, %d)-(%d, %d), %dx%d", rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	m_strClientRect = szBuf;

	if (::IsIconic(m_hwnd) || ::IsZoomed(m_hwnd))
	{
		WINDOWPLACEMENT wndpl;

		if (::GetWindowPlacement(m_hwnd, &wndpl))
		{
			wsprintf(szBuf, "(%d, %d)-(%d, %d), %dx%d",
				wndpl.rcNormalPosition.left, wndpl.rcNormalPosition.top,
				wndpl.rcNormalPosition.right, wndpl.rcNormalPosition.bottom,
				wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left,
				wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top);
			m_strRestoredRect = szBuf;
		}
		else
		{
			m_strRestoredRect = ids(IDS_UNAVAILABLE);
		}
	}
	else
	{
		m_strRestoredRect = m_strRectangle;
	}

	m_dwID = ::GetWindowLong(m_hwnd, GWL_ID);
	m_dwUserData = ::GetWindowLong(m_hwnd, GWL_USERDATA);

	m_strClassName = szClass;

	DWORD dwStyles = m_flStyles = GetWindowLong(m_hwnd, GWL_STYLE);
	FillStrListFromTable(&m_strListStyles, &dwStyles, tblWindowStyles);

	if (!m_strClassName.CompareNoCase("BUTTON"))
	{
		AppendStrListFromButtonTable(&m_strListStyles, dwStyles, tblButtonStyles);
	}
	else if (!m_strClassName.CompareNoCase("COMBOBOX"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblComboBoxStyles, FALSE, FALSE, TRUE);
	}
	else if (!m_strClassName.CompareNoCase("EDIT"))
	{
		AppendStrListFromEditTable(&m_strListStyles, dwStyles, tblEditStyles);
	}
	else if (!m_strClassName.CompareNoCase("LISTBOX"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblListBoxStyles, FALSE, FALSE, TRUE);
	}
	else if (!m_strClassName.CompareNoCase("SCROLLBAR"))
	{
		AppendStrListFromScrollTable(&m_strListStyles, dwStyles, tblScrollBarStyles);
	}
	else if (!m_strClassName.CompareNoCase("STATIC"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblStaticStyles, FALSE, TRUE, TRUE);
	}
#ifndef DISABLE_WIN95_MESSAGES
	else if (!m_strClassName.CompareNoCase("SysHeader32"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblHeaderStyles, FALSE, FALSE, TRUE);
	}
#endif
	else if (!m_strClassName.CompareNoCase("ToolbarWindow32"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblToolbarStyles, FALSE, FALSE, TRUE);
	}
#ifndef DISABLE_WIN95_MESSAGES
	else if (!m_strClassName.CompareNoCase("tooltips_class32"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblToolTipStyles, FALSE, FALSE, TRUE);
	}
	else if (!m_strClassName.CompareNoCase("msctls_statusbar32"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblStatusBarStyles, FALSE, FALSE, TRUE);
	}
	else if (!m_strClassName.CompareNoCase("msctls_trackbar32"))
	{
		AppendStrListFromTrackBarTable(&m_strListStyles, dwStyles, tblTrackBarStyles);
	}
#endif
	else if (!m_strClassName.CompareNoCase("msctls_updown32") || !m_strClassName.CompareNoCase("msctls_updown"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblUpDownStyles, FALSE, FALSE, TRUE);
	}
 #ifndef DISABLE_WIN95_MESSAGES
	else if (!m_strClassName.CompareNoCase("SysListView32"))
	{
		AppendStrListFromListViewTable(&m_strListStyles, dwStyles, tblListViewStyles);
	}
	else if (!m_strClassName.CompareNoCase("SysTreeView32"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblTreeViewStyles, FALSE, FALSE, TRUE);
	}
	else if (!m_strClassName.CompareNoCase("SysTabControl32"))
	{
		AppendStrListFromTabControlTable(&m_strListStyles, dwStyles, tblTabControlStyles);
	}
	else if (!m_strClassName.CompareNoCase("SysAnimate32"))
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblAnimateStyles, FALSE, FALSE, TRUE);
	}
#endif
	else
	{
		FillStrListFromTable(&m_strListStyles, &dwStyles, tblWindowStyles, FALSE, FALSE, TRUE);
	}

	dwStyles = m_flExtStyles = GetWindowLong(m_hwnd, GWL_EXSTYLE);
	FillStrListFromTable(&m_strListExtStyles, &dwStyles, tblExtWindowStyles, TRUE, FALSE, TRUE);

	m_hwndNext = ::GetWindow(m_hwnd, GW_HWNDNEXT);
	m_hwndPrevious = ::GetWindow(m_hwnd, GW_HWNDPREV);
	m_hwndChild = ::GetWindow(m_hwnd, GW_CHILD);
	m_hwndParent = ::GetParent(m_hwnd);
	m_hwndOwner = ::GetWindow(m_hwnd, GW_OWNER);

	if (psz = GetExpandedClassName(szClass))
	{
		m_strClassName += psz;
	}

	w = GetClassWord(m_hwnd, GCW_ATOM);
	if (w)
	{
		m_strClassAtom = FormatWord(w);
	}
	else
	{
		m_strClassAtom = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD && opd.fValidWC)
	{
		dw = opd.wc.style;
		fOK = TRUE;
	}
	else
	{
		SetLastError(0);
		dw = GetClassLong(m_hwnd, GCL_STYLE);
		fOK = !GetLastError();
	}

	if (fOK)
	{
		m_strClassStyles = FormatHex(dw);
		FillStrListFromTable(&m_strListClassStyles, &dw, m_astClass);
	}
	else
	{
		m_strClassStyles = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD && opd.fValidWC)
	{
		cbWndExtra = opd.wc.cbWndExtra;
		fOK = TRUE;
	}
	else
	{
		SetLastError(0);
		cbWndExtra = (INT)GetClassLong(m_hwnd, GCL_CBWNDEXTRA);
		fOK = !GetLastError();
	}

	m_strListExtraBytes.RemoveAll();
	if (fOK)
	{
		m_strClassWndExtraBytes = FormatInt(cbWndExtra);

		for (i = 0; i < cbWndExtra; i += sizeof(DWORD))
		{
			wsprintf(szBuf, "+%-8d", i);

			SetLastError(0);

			//
			// Check for the case where they don't have a full DWORD left.
			//
			if (cbWndExtra - i == 2)
			{
				dw = (WORD)::GetWindowWord(m_hwnd, i);
				if (GetLastError())
				{
					_tcscat(szBuf, ids(IDS_UNAVAILABLE));
				}
				else
				{
					_tcscat(szBuf, FormatWord((WORD)dw));
				}
			}
			else
			{
				dw = (DWORD)GetWindowLong(m_hwnd, i);
				if (GetLastError())
				{
					_tcscat(szBuf, ids(IDS_UNAVAILABLE));
				}
				else
				{
					_tcscat(szBuf, FormatHex(dw));
				}
			}

			m_strListExtraBytes.AddTail(szBuf);
		}
	}
	else
	{
		m_strClassWndExtraBytes = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD && opd.fValidWC)
	{
		cb = opd.wc.cbClsExtra;
		fOK = TRUE;
	}
	else
	{
		SetLastError(0);
		cb = (INT)GetClassLong(m_hwnd, GCL_CBCLSEXTRA);
		fOK = !GetLastError();
	}

	m_strListClassExtraBytes.RemoveAll();
	if (fOK)
	{
		m_strClassExtraBytes = FormatInt(cb);

		if (cb)
		{
			for (i = 0; i < cb; i += sizeof(DWORD))
			{
				wsprintf(szBuf, "+%-8d", i);

				SetLastError(0);

				//
				// Check for the case where they don't have a full DWORD left.
				//
				if (cb - i == 2)
				{
					dw = (WORD)::GetClassWord(m_hwnd, i);
					if (GetLastError())
					{
						_tcscat(szBuf, ids(IDS_UNAVAILABLE));
					}
					else
					{
						_tcscat(szBuf, FormatWord((WORD)dw));
					}
				}
				else
				{
					dw = (DWORD)GetClassLong(m_hwnd, i);
					if (GetLastError())
					{
						_tcscat(szBuf, ids(IDS_UNAVAILABLE));
					}
					else
					{
						_tcscat(szBuf, FormatHex(dw));
					}
				}

				m_strListClassExtraBytes.AddTail(szBuf);
			}
		}
	}
	else
	{
		m_strClassExtraBytes = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD && opd.fValidWC)
	{
		dw = (DWORD)opd.wc.hInstance;
		fOK = TRUE;
	}
	else
	{
		SetLastError(0);
		dw = GetClassLong(m_hwnd, GCL_HMODULE);
		fOK = !GetLastError();
	}

	if (fOK)
	{
		m_strClassInstance = FormatHex(dw);
	}
	else
	{
		m_strClassInstance = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD && opd.fValidWC)
	{
		dw = (DWORD)opd.wc.lpfnWndProc;
		fOK = TRUE;
	}
	else
	{
		SetLastError(0);
		if (IsWindowUnicode(m_hwnd))
		{
			dw = GetClassLongW(m_hwnd, GCL_WNDPROC);
		}
		else
		{
			dw = GetClassLongA(m_hwnd, GCL_WNDPROC);
		}
		fOK = !GetLastError();
	}

	if (fOK)
	{
		m_strClassWndProc = FormatHex(dw);
	}
	else
	{
		m_strClassWndProc = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD)
	{
		if (!(*opd.szMenuName))
		{
			m_strClassMenuName = ids(IDS_NONE_NOSPACE);
		}
		else
		{
			m_strClassMenuName = opd.szMenuName;
		}
	}
	else
	{
		m_strClassMenuName = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD && opd.fValidWC)
	{
		dw = (DWORD)opd.wc.hIcon;
		fOK = TRUE;
	}
	else
	{
		SetLastError(0);
		dw = GetClassLong(m_hwnd, GCL_HICON);
		fOK = !GetLastError();
	}

	if (fOK)
	{
		m_strClassIcon = FormatHandleFromTable((HANDLE)dw, m_ahtSystemIcons);
	}
	else
	{
		m_strClassIcon = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD && opd.fValidWC)
	{
		dw = (DWORD)opd.wc.hCursor;
		fOK = TRUE;
	}
	else
	{
		SetLastError(0);
		dw = GetClassLong(m_hwnd, GCL_HCURSOR);
		fOK = !GetLastError();
	}

	if (fOK)
	{
		m_strClassCursor = FormatHandleFromTable((HANDLE)dw, m_ahtSystemCursors);
	}
	else
	{
		m_strClassCursor = ids(IDS_UNAVAILABLE);
	}

	if (fGotOPD && opd.fValidWC)
	{
		dw = (DWORD)opd.wc.hbrBackground;
		fOK = TRUE;
	}
	else
	{
		SetLastError(0);
		dw = GetClassLong(m_hwnd, GCL_HBRBACKGROUND);
		fOK = !GetLastError();
	}

	if (fOK)
	{
		VALUETABLE* pvt = m_astColor;
		psz = NULL;

		while (pvt->pszValue)
		{
			if (dw - 1 == pvt->nValue)
			{
				psz = pvt->pszValue;
				break;
			}

			pvt++;
		}

		if (psz)
		{
			m_strClassBrush = psz;
		}
		else
		{
			m_strClassBrush = FormatHandle((HANDLE)dw);
		}
	}
	else
	{
		m_strClassBrush = ids(IDS_UNAVAILABLE);
	}

	m_ThreadID = ::GetWindowThreadProcessId(m_hwnd, &m_ProcessID);
	if (m_ThreadID)
	{
		m_fValidProcessThreadID = TRUE;
	}
	else
	{
		m_fValidProcessThreadID = FALSE;
	}

	m_hInstance = (HINSTANCE)::GetWindowLong(m_hwnd, GWL_HINSTANCE);

	if (fGotOPD && opd.pfnWndProc)
	{
		m_strWndProc = FormatHex((DWORD)opd.pfnWndProc);
	}
	else
	{
		m_strWndProc = ids(IDS_UNAVAILABLE);

	}

	if (IsWindowUnicode(m_hwnd))
	{
		m_strWndProc += ids(IDS_UNICODE);
	}

	if (fGotOPD && opd.fValidWC && opd.pfnWndProc != opd.wc.lpfnWndProc)
	{
		m_strWndProc += ids(IDS_SUBCLASSED);
	}

	return TRUE;
}

void CPropertyInspector::FillStrListFromTable(CStringList* pStrList, DWORD *flStyle, VALUETABLE *pvt,
	BOOL fRemoveAll, BOOL fOneEntry, BOOL fPrintExtras)
{
	DWORD dwTempStyle = *flStyle;
	BOOL  fFound = FALSE;

	if (fRemoveAll)
		pStrList->RemoveAll();

	while (pvt->pszValue && !fFound)
	{
		if ((dwTempStyle & pvt->nValue) == pvt->nValue)
		{
			pStrList->AddTail(pvt->pszValue);
			dwTempStyle &= ~pvt->nValue;
			if (fOneEntry)
				fFound = TRUE;
		}

		pvt++;
	}
	if (fPrintExtras && dwTempStyle)
		pStrList->AddTail(FormatHex(dwTempStyle));

	*flStyle = dwTempStyle;	// so styles don't get parsed twice
}

void CPropertyInspector::AppendStrListFromButtonTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt)
{
	DWORD dwTempStyle = flStyle;

	while (pvt->pszValue)
	{
		if ((dwTempStyle & pvt->nValue) == pvt->nValue)
		{
			if ((pvt->nValue == BS_PUSHBUTTON) && 
				(!(flStyle & BS_3STATE) && !(flStyle & BS_AUTO3STATE) && 
				!(flStyle & BS_AUTOCHECKBOX) && !(flStyle & BS_AUTORADIOBUTTON) && 
				!(flStyle & BS_CHECKBOX) && !(flStyle & BS_DEFPUSHBUTTON) && 
				!(flStyle & BS_GROUPBOX) && !(flStyle & BS_RADIOBUTTON)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if (pvt->nValue != BS_PUSHBUTTON)
			{
				pStrList->AddTail(pvt->pszValue);
			}
			dwTempStyle &= ~pvt->nValue;
		}

		pvt++;
	}
	if (dwTempStyle)
		pStrList->AddTail(FormatHex(dwTempStyle));
}

void CPropertyInspector::AppendStrListFromScrollTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt)
{
	DWORD dwTempStyle = flStyle;

	while (pvt->pszValue)
	{
		if ((dwTempStyle & pvt->nValue) == pvt->nValue)
		{
			if ((pvt->nValue == SBS_HORZ) && 
				(!(flStyle & SBS_VERT) && !(flStyle & SBS_SIZEBOX)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == SBS_VERT) && !(flStyle & SBS_SIZEBOX))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == SBS_TOPALIGN) && !(flStyle & SBS_SIZEBOX))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == SBS_LEFTALIGN) && !(flStyle & SBS_SIZEBOX))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == SBS_BOTTOMALIGN) && !(flStyle & SBS_SIZEBOX))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == SBS_RIGHTALIGN) && !(flStyle & SBS_SIZEBOX))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == SBS_SIZEBOXTOPLEFTALIGN) && (flStyle & SBS_SIZEBOX))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == SBS_SIZEBOXBOTTOMRIGHTALIGN) && (flStyle & SBS_SIZEBOX))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if (pvt->nValue == SBS_SIZEBOX)
			{
				pStrList->AddTail(pvt->pszValue);
			}
			dwTempStyle &= ~pvt->nValue;
		}

		pvt++;
	}
	if (dwTempStyle)
		pStrList->AddTail(FormatHex(dwTempStyle));
}

void CPropertyInspector::AppendStrListFromEditTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt)
{
	DWORD dwTempStyle = flStyle;

	while (pvt->pszValue)
	{
		if ((dwTempStyle & pvt->nValue) == pvt->nValue)
		{
			if ((pvt->nValue == ES_LEFT) && 
				(!(flStyle & ES_CENTER) && !(flStyle & ES_RIGHT)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if (pvt->nValue != ES_LEFT)
			{
				pStrList->AddTail(pvt->pszValue);
			}
			dwTempStyle &= ~pvt->nValue;
		}

		pvt++;
	}
	if (dwTempStyle)
		pStrList->AddTail(FormatHex(dwTempStyle));
}

#ifndef DISABLE_WIN95_MESSAGES
void CPropertyInspector::AppendStrListFromTrackBarTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt)
{
	DWORD dwTempStyle = flStyle;

	while (pvt->pszValue)
	{
		if ((dwTempStyle & pvt->nValue) == pvt->nValue)
		{
			if ((pvt->nValue == TBS_HORZ) && (!(flStyle & TBS_VERT)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == TBS_BOTTOM) && (!(flStyle & TBS_TOP)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == TBS_RIGHT) && (!(flStyle & TBS_LEFT)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if (pvt->nValue != TBS_HORZ && pvt->nValue != TBS_BOTTOM && pvt->nValue != TBS_RIGHT)
			{
				pStrList->AddTail(pvt->pszValue);
			}
			dwTempStyle &= ~pvt->nValue;
		}

		pvt++;
	}
	if (dwTempStyle)
		pStrList->AddTail(FormatHex(dwTempStyle));
}

void CPropertyInspector::AppendStrListFromListViewTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt)
{
	DWORD dwTempStyle = flStyle;

	while (pvt->pszValue)
	{
		if ((dwTempStyle & pvt->nValue) == pvt->nValue)
		{
			if ((pvt->nValue == LVS_ICON) && (!(flStyle & LVS_REPORT) && !(flStyle & LVS_SMALLICON) && !(flStyle & LVS_LIST)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if (pvt->nValue != LVS_ICON)
			{
				pStrList->AddTail(pvt->pszValue);
			}
			dwTempStyle &= ~pvt->nValue;
		}

		pvt++;
	}
	if (dwTempStyle)
		pStrList->AddTail(FormatHex(dwTempStyle));
}

void CPropertyInspector::AppendStrListFromTabControlTable(CStringList* pStrList, DWORD flStyle, VALUETABLE *pvt)
{
	DWORD dwTempStyle = flStyle;

	while (pvt->pszValue)
	{
		if ((dwTempStyle & pvt->nValue) == pvt->nValue)
		{
			if ((pvt->nValue == TCS_TABS) && (!(flStyle & TCS_BUTTONS)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == TCS_SINGLELINE) && (!(flStyle & TCS_MULTILINE)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if ((pvt->nValue == TCS_RIGHTJUSTIFY) && (!(flStyle & TCS_FIXEDWIDTH) && !(flStyle & TCS_RAGGEDRIGHT)))
			{
				pStrList->AddTail(pvt->pszValue);
			}
			else if (pvt->nValue != TCS_TABS && pvt->nValue != TCS_SINGLELINE && pvt->nValue != TCS_RIGHTJUSTIFY)
			{
				pStrList->AddTail(pvt->pszValue);
			}
			dwTempStyle &= ~pvt->nValue;
		}

		pvt++;
	}
	if (dwTempStyle)
		pStrList->AddTail(FormatHex(dwTempStyle));
}

#endif //DISABLE_WIN95_MESSAGES

BOOL CPropertyInspector::SetProcessObject(DWORD pid)
{
	BOOL fRet = SetObjectType(OT_PROCESS);
	m_pid = pid;
	return(fRet);
}

BOOL CPropertyInspector::RefreshProcessObjectData()
{
	if (!theApp.IsChicago())
	{
		// Win32
		WORD wQueryType = (PROCDBQ_PROCESS | PROCDBQ_PROCESSAS);
		CPROCDB ProcessDatabase(wQueryType/*, &error*/);
	//	int iPrcIndex;
		int iPrcIndex, iPrcASIndex;
		LPWSTR pszExe;

		if ((iPrcIndex = ProcessDatabase.GetProcIndexFromId(m_pid)) == -1)
			return(FALSE);
		if ((iPrcASIndex = ProcessDatabase.GetProcASIndexFromId(m_pid)) == -1)
			return(FALSE);
		memset((void *)&m_ProcessInfo, 0, sizeof(CPROCDB::PRFCNTRPROC));
		memset((void *)&m_ProcessAddrSpcInfo, 0, sizeof(CPROCDB::PRFCNTRPROCAS));
		ProcessDatabase.GetPrfCntrProc(iPrcIndex, &m_ProcessInfo);
		ProcessDatabase.GetPrfCntrProcAS(iPrcASIndex, &m_ProcessAddrSpcInfo);
		pszExe = (LPWSTR)ProcessDatabase.GetProcessName(iPrcIndex);
		m_strExecutable = wcsupr(pszExe);
	}
	else
	{
		// Chicago
		HANDLE hProcessList;
		PROCESSENTRY32 pe32;

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if ((hProcessList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0)) != NULL)
		{
			if ((*theApp.pfnProcess32First)(hProcessList, &pe32))
			{
				if (pe32.th32ProcessID == m_pid)
				{
					memcpy((void *)&m_ProcessEntry, (void *)&pe32, sizeof(PROCESSENTRY32));
					// GetTimesForProcess();
				}
				else
				{
					while ((*theApp.pfnProcess32Next)(hProcessList, &pe32))
					{
						if (pe32.th32ProcessID == m_pid)
						{
							memcpy((void *)&m_ProcessEntry, (void *)&pe32, sizeof(PROCESSENTRY32));
							// GetTimesForProcess();
							break;
						}
					}
				}
			}

			if (pe32.dwSize >= sizeof(PROCESSENTRY32))
			{
				char szFileName[_MAX_FNAME];
				_splitpath(m_ProcessEntry.szExeFile, NULL, NULL, szFileName, NULL);
				m_strExecutable = szFileName;
			}
			else
				m_strExecutable = ids(IDS_UNAVAILABLE);

			CloseHandle(hProcessList);
		}
	}

	return(TRUE);
}

void CPropertyInspector::GetTimesForProcess()
{
	HANDLE hProcess;
	
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_pid)) != NULL)
	{
		GetSystemTime(&m_stPrcSnapshotTime);
		GetProcessTimes(hProcess, &m_ftPrcCreationTime, &m_ftPrcExitTime, &m_ftPrcKernelTime, &m_ftPrcUserTime);
	}
}

BOOL CPropertyInspector::SetThreadObject(DWORD tid)
{
	BOOL fRet = SetObjectType(OT_THREAD);
	m_tid = tid;
	return(fRet);
}

BOOL CPropertyInspector::RefreshThreadObjectData()
{
	if (!theApp.IsChicago())
	{
		// Win32
		WORD wQueryType = (PROCDBQ_THREAD | PROCDBQ_THREADDET);
		CPROCDB ProcessDatabase(wQueryType/*, &error*/);
		int iThdIndex;
		LPWSTR pszExe;

		if ((iThdIndex = ProcessDatabase.GetThrdIndexFromId(m_tid)) == -1)
			return(FALSE);
		memset((void *)&m_ThreadInfo, 0, sizeof(CPROCDB::PRFCNTRTHRD));
		memset((void *)&m_ThreadDetInfo, 0, sizeof(CPROCDB::PRFCNTRTHRDDET));
		ProcessDatabase.GetPrfCntrThrd(iThdIndex, &m_ThreadInfo);
		ProcessDatabase.GetPrfCntrThrdDet(iThdIndex, &m_ThreadDetInfo);
		pszExe = (LPWSTR)ProcessDatabase.GetThreadName(iThdIndex);
		m_strExecutable = wcsupr(pszExe);
	}
	else
	{
		// Chicago
		HANDLE hThreadList;
		THREADENTRY32 te32;

		te32.dwSize = sizeof(THREADENTRY32);

		if ((hThreadList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPTHREAD, 0)) != NULL)
		{
			if ((*theApp.pfnThread32First)(hThreadList, &te32))
			{
				if (te32.th32ThreadID == m_tid)
				{
					memcpy((void *)&m_ThreadEntry, (void *)&te32, sizeof(THREADENTRY32));
					// GetTimesForThread();
				}
				else
				{
					while ((*theApp.pfnThread32Next)(hThreadList, &te32))
					{
						if (te32.th32ThreadID == m_tid)
						{
							memcpy((void *)&m_ThreadEntry, (void *)&te32, sizeof(THREADENTRY32));
							// GetTimesForThread();
							break;
						}
					}
				}
			}

			CloseHandle(hThreadList);

			HANDLE hProcessList;
			PROCESSENTRY32 pe32;

			pe32.dwSize = sizeof(PROCESSENTRY32);

			if ((hProcessList = (*theApp.pfnCreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0)) != NULL)
			{
				if ((*theApp.pfnProcess32First)(hProcessList, &pe32))
				{
					if (pe32.th32ProcessID == m_ThreadEntry.th32OwnerProcessID)
					{
						memcpy((void *)&m_ProcessEntry, (void *)&pe32, sizeof(PROCESSENTRY32));
						// GetTimesForProcess();
					}
					else
					{
						while ((*theApp.pfnProcess32Next)(hProcessList, &pe32))
						{
							if (pe32.th32ProcessID == m_ThreadEntry.th32OwnerProcessID)
							{
								memcpy((void *)&m_ProcessEntry, (void *)&pe32, sizeof(PROCESSENTRY32));
								// GetTimesForProcess();
								break;
							}
						}
					}
				}

				if (pe32.dwSize >= sizeof(PROCESSENTRY32))
				{
					char szFileName[_MAX_FNAME];
					_splitpath(m_ProcessEntry.szExeFile, NULL, NULL, szFileName, NULL);
					m_strExecutable = szFileName;
				}
				else
					m_strExecutable = ids(IDS_UNAVAILABLE);

				CloseHandle(hProcessList);
			}
		}
	}

	return(TRUE);
}

void CPropertyInspector::GetTimesForThread()
{
	HANDLE hThread;
	
	if ((hThread = OpenProcess(THREAD_QUERY_INFORMATION, FALSE, m_tid)) != NULL)
	{
		GetSystemTime(&m_stThdSnapshotTime);
		GetThreadTimes(hThread, &m_ftThdCreationTime, &m_ftThdExitTime, &m_ftThdKernelTime, &m_ftThdUserTime);
	}
}

BOOL CPropertyInspector::SetMessageObject(PMSGSTREAMDATA2 pmsd2)
{
	BOOL fRet = SetObjectType(OT_MESSAGE);
	m_msd2 = *pmsd2;
	return(fRet);
}

BOOL CPropertyInspector::RefreshMessageObjectData()
{
	CHAR szBuf[256];
	CHAR szBuf2[128];
	LPSTR pszPostType;

	switch (m_msd2.fPostType)
	{
		case POSTTYPE_SENT:
			pszPostType = ids(IDS_SENT);
			break;

		case POSTTYPE_POSTED:
			pszPostType = ids(IDS_POSTED);
			break;

		case POSTTYPE_RETURN:
			pszPostType = ids(IDS_RETURN);
			break;

		case POSTTYPE_SENTNORETURN:
			pszPostType = ids(IDS_SENT_RETURN_UNAVAIL);
			break;

		default:
			//
			// Bogus value for fPostType!
			//
			ASSERT(FALSE);
			break;
	}

	sprintf(szBuf, "%4.4X  %s", m_msd2.msg, pszPostType);
	m_strMessage1 = szBuf;

	switch (m_msd2.mtMsgType)
	{
		case MT_NORMAL:
			m_strMessage2 = CMsgDoc::m_apmdLT[m_msd2.msg]->pszMsg;
			break;

		case MT_DIALOG:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdDlgLT[m_msd2.msg - WM_USER]->pszMsg, m_msd2.msg - WM_USER);
			m_strMessage2 = szBuf;
			break;
#ifndef DISABLE_WIN95_MESSAGES
		case MT_ANIMATE:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdAniLT[m_msd2.msg - (WM_USER + 100)]->pszMsg, m_msd2.msg - (WM_USER + 100));
			m_strMessage2 = szBuf;
			break;

		case MT_HEADER:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdHdrLT[m_msd2.msg - HDM_FIRST]->pszMsg, m_msd2.msg - HDM_FIRST);
			m_strMessage2 = szBuf;
			break;

		case MT_HOTKEY:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdHKLT[m_msd2.msg - (WM_USER + 1)]->pszMsg, m_msd2.msg - (WM_USER + 1));
			m_strMessage2 = szBuf;
			break;

		case MT_LISTVIEW:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdLVLT[m_msd2.msg - LVM_FIRST]->pszMsg, m_msd2.msg - LVM_FIRST);
			m_strMessage2 = szBuf;
			break;

		case MT_PROGRESS:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdProgLT[m_msd2.msg - (WM_USER + 1)]->pszMsg, m_msd2.msg - (WM_USER + 1));
			m_strMessage2 = szBuf;
			break;

		case MT_STATUSBAR:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdStatLT[m_msd2.msg - (WM_USER + 1)]->pszMsg, m_msd2.msg - (WM_USER + 1));
			m_strMessage2 = szBuf;
			break;

		case MT_TOOLBAR:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdTBLT[m_msd2.msg - (WM_USER + 1)]->pszMsg, m_msd2.msg - (WM_USER + 1));
			m_strMessage2 = szBuf;
			break;

		case MT_TRACKBAR:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdTrkLT[m_msd2.msg - WM_USER]->pszMsg, m_msd2.msg - WM_USER);
			m_strMessage2 = szBuf;
			break;

		case MT_TABCTRL:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdTabLT[m_msd2.msg - TCM_FIRST]->pszMsg, m_msd2.msg - TCM_FIRST);
			m_strMessage2 = szBuf;
			break;

		case MT_TOOLTIP:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdTTLT[m_msd2.msg - (WM_USER + 1)]->pszMsg, m_msd2.msg - (WM_USER + 1));
			m_strMessage2 = szBuf;
			break;

		case MT_TREEVIEW:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdTVLT[m_msd2.msg - TV_FIRST]->pszMsg, m_msd2.msg - TV_FIRST);
			m_strMessage2 = szBuf;
			break;

		case MT_UPDOWN:
			sprintf(szBuf, "%s", CMsgDoc::m_apmdUpDnLT[m_msd2.msg - (WM_USER + 101)]->pszMsg, m_msd2.msg - (WM_USER + 101));
			m_strMessage2 = szBuf;
			break;
#endif	// DISABLE_WIN95_MESSAGES
		case MT_UNDOCUMENTED:
			m_strMessage2 = ids(IDS_UNDOCUMENTED);
			break;

		case MT_REGISTERED:
			if (GetClipboardFormatName(m_msd2.msg, szBuf2,
				sizeof(szBuf2) / sizeof(CHAR)))
			{
				wsprintf(szBuf, ids(IDS_REGISTERED_PARAM), szBuf2);
				m_strMessage2 = szBuf;
			}
			else
			{
				m_strMessage2 = ids(IDS_REGISTERED);
			}

			break;

		case MT_USER:
			wsprintf(szBuf, "WM_USER+%d", m_msd2.msg - WM_USER);
			m_strMessage2 = szBuf;
			break;
	}

	return TRUE;
}

void CPropertyInspector::OnRefresh()
{
	BOOL fValidObject;
	BOOL fEnableRefresh = TRUE;

	switch (m_nObjectType)
	{
		case OT_WINDOW:
			fValidObject = RefreshWindowObjectData();
			break;

		case OT_PROCESS:
			fValidObject = RefreshProcessObjectData();
			break;

		case OT_THREAD:
			fValidObject = RefreshThreadObjectData();
			break;

		case OT_MESSAGE:
			fValidObject = RefreshMessageObjectData();
			fEnableRefresh = FALSE;
			break;

		case OT_NONE:
			fValidObject = FALSE;
			fEnableRefresh = FALSE;
			break;

		default:
			ASSERT(FALSE);
			break;
	}

	SetValidObjectFlag(fValidObject);

	if (fValidObject)
	{
		for (int nTab = 0; nTab < m_tabs.GetSize(); nTab++)
		{
			((CPropertyTab*)GetTab(nTab))->SetRefreshFlag(TRUE);
		}

		if (m_nTabCur != -1)
		{
			CPropertyTab* pTab = (CPropertyTab*)GetTab(m_nTabCur);
			if (pTab->GetSafeHwnd())
			{
				pTab->UpdateFields();
				pTab->SetRefreshFlag(FALSE);
			}
		}
		m_StaticMessage.ShowWindow(SW_HIDE);
	}
	else
	{
		UINT idsInvalid;
		DWORD dwObject;
		CHAR szMsg[100];

		ClearAllTabs();

		switch (m_nObjectType)
		{
			case OT_WINDOW:
				idsInvalid = IDS_INVALIDWINDOW;
				dwObject = (DWORD)m_hwnd;
				break;

			case OT_PROCESS:
				idsInvalid = IDS_INVALIDPROCESS;
				dwObject = m_pid;
				break;

			case OT_THREAD:
				idsInvalid = IDS_INVALIDTHREAD;
				dwObject = m_tid;
				break;

			case OT_MESSAGE:
				//
				// Messages cannot become invalid...
				//
				ASSERT(FALSE);
				break;

			case OT_NONE:
				break;

			default:
				ASSERT(FALSE);
				break;
		}

		if (m_nObjectType == OT_NONE)
		{
			m_StaticMessage.SetWindowText(ids(IDS_NOTHINGSELECTED));
		}
		else
		{
			wsprintf(szMsg, "%s (%08X)", ids(idsInvalid), dwObject);
			m_StaticMessage.SetWindowText(szMsg);
		}
		m_StaticMessage.ShowWindow(SW_SHOW);
		fEnableRefresh = FALSE;
	}

	m_btnRefresh.EnableWindow(fEnableRefresh);

	if (m_fObjectTypeChanged)
	{
		SetWindowText(m_strCaption);
		m_fObjectTypeChanged = FALSE;
	}

	//
	// Force the properties window to be shown.  This is needed
	// the first time that it is created, because we create it
	// invisible to avoid some flashing of the fields as they
	// are updated.
	//
	ShowWindow(SW_SHOW);
}

void CPropertyInspector::CreateButtons()
{
	CTabbedDialog::CreateButtons();

	CString str;
	CRect rcEmpty;
	rcEmpty.SetRectEmpty();

	str.LoadString(IDS_REFRESH);
	m_btnRefresh.Create(str, WS_CHILD | WS_VISIBLE, rcEmpty, this, IDC_REFRESH);

	//
	// Calculate the height of the std bold font.
	//
	TEXTMETRIC tm;
	CDC dc;
	CFont* pFont = GetStdFont(font_NormalBold);
	CFont* pFontOld;
	dc.Attach(::GetDC(NULL));
	pFontOld = dc.SelectObject(pFont);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pFontOld);
	dc.Detach();

	//
	// Calculate the area where the tabs go.
	//
	CRect rectPage;
	CRect margins;
	GetClientRect(rectPage);
	GetMargins(margins);

	rectPage.left += margins.left;
	rectPage.right -= margins.right;
	rectPage.bottom -= margins.bottom;
	rectPage.InflateRect(-6, -6);

	//
	// Adjust the top down so that the static control will be centered
	// vertically.
	//
	rectPage.top += (rectPage.Height() - tm.tmHeight) / 2;

	//
	// Create a horizontally centering text control.
	// It is not initially visible.
	//
	m_StaticMessage.Create(NULL, WS_CHILD | SS_CENTER, rectPage, this);
	m_StaticMessage.SetFont(pFont);
}

CButton* CPropertyInspector::GetButtonFromIndex(int index)
{
	//
	// This determines the order that the buttons are shown
	// left to right in the dialog.
	//
	switch (index)
	{
		case 0:
			return &m_btnOk;			// The Close button, actually...

		case 1:
			return &m_btnRefresh;	   // Our Refresh button.

		case 2:
			return &m_btnHelp;		  // Help button is always last.

		default:
			return NULL;
	}
}

BEGIN_MESSAGE_MAP(CPropertyInspector, CTabbedDialog)
	//{{AFX_MSG_MAP(CPropertyInspector)
	ON_COMMAND(IDC_REFRESH, OnRefresh)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CPropertyInspector::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabbedDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//
	// Post a message that causes a refresh to be done.  This
	// has to be done in this fashion so that the window can
	// get all of the other messages (specifically WM_SIZE)
	// that it needs to get before we try and access the
	// data and refresh the current tab.  The problem was this:
	// We create the property inspector, then set the type of
	// object.  This causes the different tabs to be added.
	// Then we call DoModal on it, which causes the main window
	// and the first tab to get created.  But suppose that the
	// object we set is invalid?  What would happen is that
	// the OnRefresh handler clears all the tabs and shows
	// the "Invalid xxx" message in the middle, then the code
	// continued and when the OnSize handler in CTabbedDialog
	// got called, it tried to get the size of the current
	// tab so that it can size the dialog, but we just cleared
	// all tabs and everything crashes.
	//
	// By posting the refresh, we avoid clearing the tabs until
	// after the inspector has gotten WM_SIZE.  To avoid flashing,
	// however, we make the inspector invisible and only show
	// it after the refresh has been done.
	//
	PostMessage(WM_COMMAND, IDC_REFRESH);

	return 0;
}

BOOL CPropertyInspector::PreCreateWindow(CREATESTRUCT& cs)
{
	//
	// Turn off the WS_VISIBLE style for the property inspector.
	// This avoids a flash when it initially comes up, because
	// of the roundabout way that we have to cause the fields
	// to be refreshed.
	//
	cs.style &= ~WS_VISIBLE;

	return CTabbedDialog::PreCreateWindow(cs);
}

void CPropertyInspector::ChangeObject(DWORD dwObject, int nObjectType)
{
	if (m_dwObject == dwObject && m_nObjectType == nObjectType)
	{
		//
		// Nothing changed...
		//
		return;
	}

	if (m_nTabCur != -1)
	{
		CString strCaption = m_tabRow.GetTabString(m_nTabCur);
		m_DialogMap.RememberTab(nCaptionID, strCaption);
	}

	BOOL fNew = SetObject(dwObject, nObjectType);

	CString strTabCaption;
	int iInitialTab = 0;
	if (m_DialogMap.LookupTab(nCaptionID, strTabCaption))
	{
		for (int i = 0; i <= m_tabRow.MaxTab(); i++)
		{
			if (m_tabRow.GetTabString(i) == strTabCaption)
			{
				iInitialTab = i;
				break;
			}
		}
	}

	SelectTab(iInitialTab);
	// use fNew here to know whether to refresh m_ProcessDatabase...
	OnRefresh();
}
