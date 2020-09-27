// msgdoc.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"
#include "mainfrm.h"
#include "findtool.h"
#include "filtrdlg.h"
#include "srchdlgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgDoc

IMPLEMENT_DYNCREATE(CMsgDoc, CDocument)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMsgDoc::CMsgDoc()
{
	//
	// This variable is indirectly referenced in the destructor,
	// so we must initialize it early in case the object gets
	// destroyed before the bulk of initialization is done in
	// OnNewDocument.



	m_fLogging = FALSE;
	m_hwndSelected = NULL;
	m_wMsgSelected = 0;
	m_uTypeSelected = 0;
	m_fSearchUp = FALSE;
}

CMsgDoc::~CMsgDoc()
{
	//
	// Be sure to turn off logging!
	//
	if (m_fLogging)
	{
		OnMessagesStartStop();
	}

	POSITION pos;
	if (m_fLogToFile && ((pos = theApp.m_strlistLogFiles.Find(m_strLogFileName)) != NULL))
	{
		theApp.m_strlistLogFiles.RemoveAt(pos);
	}
}

BOOL CMsgDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//
	// Cache the first (and only) view for this doc.
	//
	POSITION pos = GetFirstViewPosition();
	ASSERT(pos);
	m_pMsgView = (CMsgView *)GetNextView(pos);
	ASSERT(m_pMsgView);

	//
	// Be sure there really is only one view.
	//
	ASSERT(!pos);

	//
	// Attach the view to the CMsgStream object.
	//
	m_mout.SetView(m_pMsgView);

	m_fMsgsUser = m_fMsgsUserDef;
	m_fMsgsRegistered = m_fMsgsRegisteredDef;
	m_fMsgsUndocumented = m_fMsgsUndocumentedDef;

	memcpy(m_afSelected, m_afSelectedDef, MAX_MESSAGES * sizeof(BYTE));
	memcpy(m_afDlgSelected, m_afDlgSelectedDef, MAX_DLG_MESSAGES * sizeof(BYTE));
#ifndef DISABLE_WIN95_MESSAGES
	memcpy(m_afAniSelected,  m_afAniSelectedDef,  MAX_ANI_MESSAGES * sizeof(BYTE));
	memcpy(m_afHdrSelected,  m_afHdrSelectedDef,  MAX_HDR_MESSAGES * sizeof(BYTE));
	memcpy(m_afTBSelected,   m_afTBSelectedDef,   MAX_TB_MESSAGES * sizeof(BYTE));
	memcpy(m_afTTSelected,   m_afTTSelectedDef,   MAX_TT_MESSAGES * sizeof(BYTE));
	memcpy(m_afStatSelected, m_afStatSelectedDef, MAX_STAT_MESSAGES * sizeof(BYTE));
	memcpy(m_afTrkSelected,  m_afTrkSelectedDef,  MAX_TRK_MESSAGES * sizeof(BYTE));
	memcpy(m_afUpDnSelected, m_afUpDnSelectedDef, MAX_UD_MESSAGES * sizeof(BYTE));
	memcpy(m_afProgSelected, m_afProgSelectedDef, MAX_PROG_MESSAGES * sizeof(BYTE));
	memcpy(m_afHKSelected,   m_afHKSelectedDef,   MAX_HK_MESSAGES * sizeof(BYTE));
	memcpy(m_afLVSelected,   m_afLVSelectedDef,   MAX_LV_MESSAGES * sizeof(BYTE));
	memcpy(m_afTVSelected,   m_afTVSelectedDef,   MAX_TV_MESSAGES * sizeof(BYTE));
	memcpy(m_afTabSelected,  m_afTabSelectedDef,  MAX_TAB_MESSAGES * sizeof(BYTE));
#endif	// DISABLE_WIN95_MESSAGES

	m_fAllWindows = FALSE;
	m_fIncludeParent = m_fIncludeParentDef;
	m_fIncludeChildren = m_fIncludeChildrenDef;
	m_fIncludeThread = m_fIncludeThreadDef;
	m_fIncludeProcess = m_fIncludeProcessDef;

	m_fShowNestingLevel = m_fShowNestingLevelDef;
	m_fShowRawParms = m_fShowRawParmsDef;
	m_fShowDecodedParms = m_fShowDecodedParmsDef;
	m_fShowRawReturn = m_fShowRawReturnDef;
	m_fShowDecodedReturn = m_fShowDecodedReturnDef;
	m_fShowOriginTime = m_fShowOriginTimeDef;
	m_fShowMousePosition = m_fShowMousePositionDef;
	m_fLogToFile = m_fLogToFileDef;
	m_strLogFileName = m_strLogFileNameDef;

	//
	// Enable hook subclassing if this view wants to see return values.
	//
	if (m_fShowRawReturn)
		m_cShowReturnUseCount++;

	if (m_fShowDecodedReturn)
		m_cShowReturnUseCount++;

	if (m_cShowReturnUseCount)
		EnableSubclass(TRUE);

	SetMaxLines(m_cLinesMaxDef);

	//
	// Figure out what was last selected in one of the tree
	// views and set the window filters to that object as
	// appropriate.  This will be the object that we initially
	// start spying on.
	//
	switch (GetLastSelectedObjectType())
	{
		case OT_WINDOW:
			SetWindowToSpyOn((HWND)GetLastSelectedObject());
			break;

		case OT_THREAD:
			SetThreadToSpyOn(GetLastSelectedObject());
			break;

		case OT_PROCESS:
			SetProcessToSpyOn(GetLastSelectedObject());
			break;

		case OT_NONE:
			SetNothingToSpyOn();
			break;
	}

	UpdateTitle();

	//
	// Start viewing messages now, unless nothing is selected.
	//
	if (IsSomethingSelected() && GetSpyImmediate())
	{
		OnMessagesStartStop();
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// CMsgDoc::UpdateTitle
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::UpdateTitle()
{
	CHAR szTitle[256];
	LPSTR psz;

	if (m_fAllWindows)
	{
		psz = ids(IDS_MESSAGES_ALL_WINDOWS);
	}
	else
	{
		switch (m_nObjectType)
		{
			case OT_WINDOW:
				wsprintf(szTitle, ids(IDS_MESSAGES_WINDOW), m_hwndToSpyOn);
				break;

			case OT_THREAD:
				wsprintf(szTitle, ids(IDS_MESSAGES_THREAD), m_tidToSpyOn);
				break;

			case OT_PROCESS:
				wsprintf(szTitle, ids(IDS_MESSAGES_PROCESS), m_pidToSpyOn);
				break;

			case OT_NONE:
				wsprintf(szTitle, ids(IDS_MESSAGES_NOTHING_SELECTED));
				break;
		}

		psz = szTitle;
	}

	SetTitle(psz);
}

//-----------------------------------------------------------------------------
// CMsgDoc::EnableHook
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::EnableHook(BOOL fEnable, BOOL fForce)
{
	if (fForce || !m_fTestMode)
	{
		//
		// Set the flag in shared memory to the desired state.
		//
		gfHookEnabled = fEnable;
	}
}

//-----------------------------------------------------------------------------
// CMsgDoc::EnableSubclass
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::EnableSubclass(BOOL fEnable, BOOL fForce)
{
	if (fForce || !m_fTestMode)
	{
		//
		// Set the flag in shared memory to the desired state.
		//
		gfEnableSubclass = fEnable;
	}
}

//-----------------------------------------------------------------------------
// CMsgDoc::SetWindowToSpyOn
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::SetWindowToSpyOn(HWND hwnd)
{
	m_hwndToSpyOn = hwnd;
	m_tidToSpyOn = ::GetWindowThreadProcessId(hwnd, &m_pidToSpyOn);
	m_nObjectType = OT_WINDOW;
}

//-----------------------------------------------------------------------------
// CMsgDoc::SetThreadToSpyOn
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::SetThreadToSpyOn(DWORD tid)
{
	m_hwndToSpyOn = NULL;
	m_tidToSpyOn = tid;
	m_pidToSpyOn = (DWORD)-1;
	m_nObjectType = OT_THREAD;
}

//-----------------------------------------------------------------------------
// CMsgDoc::SetProcessToSpyOn
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::SetProcessToSpyOn(DWORD pid)
{
	m_hwndToSpyOn = NULL;
	m_tidToSpyOn = (DWORD)-1;
	m_pidToSpyOn = pid;
	m_nObjectType = OT_PROCESS;
}

//-----------------------------------------------------------------------------
// CMsgDoc::SetProcessToSpyOn
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::SetNothingToSpyOn()
{
	m_hwndToSpyOn = NULL;
	m_tidToSpyOn = (DWORD)-1;
	m_pidToSpyOn = (DWORD)-1;
	m_nObjectType = OT_NONE;
}

void CMsgDoc::LogMsg(PMSGSTREAMDATA pmsd)
{
	//
	// Check the window filters to see if this is a message for
	// a window we are interested in.
	// Be sure to filter out return messages if the user
	// does not want to see them.
	//
	if (IsFilteredHwnd(pmsd->hwnd) && (pmsd->fPostType != POSTTYPE_RETURN || m_fShowRawReturn || m_fShowDecodedReturn))
	{
		MSGTYPE mt = GetMsgType(pmsd->msg, pmsd->hwnd);

		switch (mt)
		{
			case MT_NORMAL:
				if (m_afSelected[pmsd->msg])
				{
					PrintMsg(MT_NORMAL, pmsd);
				}

				break;

			case MT_DIALOG:
				if (m_afDlgSelected[pmsd->msg - WM_USER])
				{
					PrintMsg(MT_DIALOG, pmsd);
				}

				break;

#ifndef DISABLE_WIN95_MESSAGES
			case MT_ANIMATE:
				if (m_afAniSelected[pmsd->msg - (WM_USER + 100)])
				{
					PrintMsg(MT_ANIMATE, pmsd);
				}

				break;

			case MT_HEADER:
				if (m_afHdrSelected[pmsd->msg - HDM_FIRST])
				{
					PrintMsg(MT_HEADER, pmsd);
				}

				break;

			case MT_HOTKEY:
				if (m_afHKSelected[pmsd->msg - (WM_USER + 1)])
				{
					PrintMsg(MT_HOTKEY, pmsd);
				}

				break;

			case MT_LISTVIEW:
				if (m_afLVSelected[pmsd->msg - LVM_FIRST])
				{
					PrintMsg(MT_LISTVIEW, pmsd);
				}

				break;

			case MT_PROGRESS:
				if (m_afProgSelected[pmsd->msg - (WM_USER + 1)])
				{
					PrintMsg(MT_PROGRESS, pmsd);
				}

				break;

			case MT_STATUSBAR:
				if (m_afStatSelected[pmsd->msg - (WM_USER + 1)])
				{
					PrintMsg(MT_STATUSBAR, pmsd);
				}

				break;

			case MT_TOOLBAR:
				if (m_afTBSelected[pmsd->msg - (WM_USER + 1)])
				{
					PrintMsg(MT_TOOLBAR, pmsd);
				}

				break;

			case MT_TRACKBAR:
				if (m_afTrkSelected[pmsd->msg - WM_USER])
				{
					PrintMsg(MT_TRACKBAR, pmsd);
				}

				break;

			case MT_TABCTRL:
				if (m_afTabSelected[pmsd->msg - TCM_FIRST])
				{
					PrintMsg(MT_TABCTRL, pmsd);
				}

				break;

			case MT_TOOLTIP:
				if (m_afTTSelected[pmsd->msg - (WM_USER + 1)])
				{
					PrintMsg(MT_TOOLTIP, pmsd);
				}

				break;

			case MT_TREEVIEW:
				if (m_afTVSelected[pmsd->msg - TV_FIRST])
				{
					PrintMsg(MT_TREEVIEW, pmsd);
				}

				break;

			case MT_UPDOWN:
				if (m_afUpDnSelected[pmsd->msg - (WM_USER + 101)])
				{
					PrintMsg(MT_UPDOWN, pmsd);
				}

				break;
#endif	// DISABLE_WIN95_MESSAGES

			case MT_UNDOCUMENTED:
				if (m_fMsgsUndocumented)
				{
					PrintMsg(MT_UNDOCUMENTED, pmsd);
				}

				break;

			case MT_REGISTERED:
				if (m_fMsgsRegistered)
				{
					PrintMsg(MT_REGISTERED, pmsd);
				}

				break;

			case MT_USER:
				if (m_fMsgsUser)
				{
					PrintMsg(MT_USER, pmsd);
				}

				break;
		}
	}
}

//-----------------------------------------------------------------------------
// CMsgDoc::IsFilteredHwnd
//
// Returns TRUE if the specified hwnd passes the window filters
// in effect for this CMsgDoc.
//
// Arguments:
//  HWND hwnd - The window handle to check.
//
// Returns:
//  TRUE (actually, non-zero) if the hwnd passes the filters, FALSE if not.
//
//-----------------------------------------------------------------------------

BOOL CMsgDoc::IsFilteredHwnd(HWND hwnd)
{
	if (m_fAllWindows)
	{
		return TRUE;
	}

	switch (m_nObjectType)
	{
		case OT_WINDOW:
			return((m_hwndToSpyOn == hwnd) ||
				(m_fIncludeChildren && ::IsChild(m_hwndToSpyOn, hwnd)) ||
				(m_fIncludeParent && hwnd == ::GetParent(m_hwndToSpyOn)) ||
				(m_fIncludeThread && IsSameThreadWindow(hwnd)) ||
				(m_fIncludeProcess && IsSameProcessWindow(hwnd)));

		case OT_THREAD:
			return IsSameThreadWindow(hwnd);

		case OT_PROCESS:
			return IsSameProcessWindow(hwnd);
	}

	//
	// We should not get here.  If the object type is
	// not found in the above case statement (like
	// OT_NONE), then m_fAllWindows should be TRUE.
	//
	ASSERT(0);
	return FALSE;
}

//-----------------------------------------------------------------------------
// FormatDWORDTime
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

static CHAR szStringBuf[32];

LPSTR FormatDwordTime(DWORD dwTime)
{
	int msec, sec, min, hr;

    hr = (int)(dwTime / 3600000);

    dwTime = dwTime - (hr * 3600000);
    min = (int)(dwTime / 60000);

    dwTime = dwTime - (min * 60000);
    sec = (int)(dwTime / 1000);

    dwTime = dwTime - (sec * 1000);
    msec = (int)(dwTime);

	_stprintf(szStringBuf, "%d:%02d:%02d.%03d", hr, min, sec, msec);

	return szStringBuf;
}

//-----------------------------------------------------------------------------
// FormatPoint
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

LPSTR FormatPoint(LONG ptX, LONG ptY)
{
	_stprintf(szStringBuf, "(%ld, %ld)", ptX, ptY);

	return szStringBuf;
}

//-----------------------------------------------------------------------------
// CMsgDoc::PrintMsg
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::PrintMsg(MSGTYPE mt, PMSGSTREAMDATA pmsd)
{
	INT i;
	LPSTR pszPostType;

	g_pmout = &m_mout;

	m_mout.SetMSDPointer(pmsd);

	switch (pmsd->fPostType)
	{
		case POSTTYPE_SENT:
			pszPostType = " S ";
			break;

		case POSTTYPE_POSTED:
			pszPostType = " P ";
			break;

		case POSTTYPE_RETURN:
			pszPostType = " R ";
			break;

		case POSTTYPE_SENTNORETURN:
			pszPostType = " s ";
			break;

		default:
			ASSERT(FALSE);
			break;
	}

	m_mout << pmsd->hwnd << pszPostType;

	if (m_fShowNestingLevel)
	{
		i = pmsd->nLevel;
		while (i--)
			m_mout << '.';
	}

	if (mt != MT_USER && mt != MT_UNDOCUMENTED && mt != MT_REGISTERED)
	{
#ifdef ONESHOTFILTER
		//
		// If the message was sent, we want to wait for the
		// return value before filtering out the message.
		//
		if (pmsd->fPostType != POSTTYPE_SENT)
		{
			//
			// Turn off the selected flag so that all following
			// occurences of this message will be filtered out.
			//
			if (mt == MT_DIALOG)
				m_afDlgSelected[pmsd->msg - WM_USER] = FALSE;
#ifndef DISABLE_WIN95_MESSAGES
			else if (mt == MT_ANIMATE)
				m_afAniSelected[pmsd->msg - (WM_USER + 100)] = FALSE;
			else if (mt == MT_HEADER)
				m_afHdrSelected[pmsd->msg - HDM_FIRST] = FALSE;
			else if (mt == MT_HOTKEY)
				m_afHKSelected[pmsd->msg - (WM_USER + 1)] = FALSE;
			else if (mt == MT_LISTVIEW)
				m_afLVSelected[pmsd->msg - LVM_FIRST] = FALSE;
			else if (mt == MT_PROGRESS)
				m_afProgSelected[pmsd->msg - (WM_USER + 1)] = FALSE;
			else if (mt == MT_STATUSBAR)
				m_afStatSelected[pmsd->msg - (WM_USER + 1)] = FALSE;
			else if (mt == MT_TOOLBAR)
				m_afTBSelected[pmsd->msg - (WM_USER + 1)] = FALSE;
			else if (mt == MT_TRACKBAR)
				m_afTrkSelected[pmsd->msg - WM_USER] = FALSE;
			else if (mt == MT_TABCTRL)
				m_afTabSelected[pmsd->msg - TCM_FIRST] = FALSE;
			else if (mt == MT_TOOLTIP)
				m_afTTSelected[pmsd->msg - (WM_USER + 1)] = FALSE;
			else if (mt == MT_TREEVIEW)
				m_afTVSelected[pmsd->msg - TV_FIRST] = FALSE;
			else if (mt == MT_UPDOWN)
				m_afUpDnSelected[pmsd->msg - (WM_USER + 101)] = FALSE;
#endif	// DISABLE_WIN95_MESSAGES
			else
				m_afSelected[pmsd->msg] = FALSE;
		}
#endif	// ONESHOTFILTER

		if (mt == MT_DIALOG)
			m_mout << m_apmdDlgLT[pmsd->msg - WM_USER]->pszMsg;
#ifndef DISABLE_WIN95_MESSAGES
		else if (mt == MT_ANIMATE)
			m_mout << m_apmdAniLT[pmsd->msg - (WM_USER + 100)]->pszMsg;
		else if (mt == MT_HEADER)
			m_mout << m_apmdHdrLT[pmsd->msg - HDM_FIRST]->pszMsg;
		else if (mt == MT_HOTKEY)
			m_mout << m_apmdHKLT[pmsd->msg - (WM_USER + 1)]->pszMsg;
		else if (mt == MT_LISTVIEW)
			m_mout << m_apmdLVLT[pmsd->msg - LVM_FIRST]->pszMsg;
		else if (mt == MT_PROGRESS)
			m_mout << m_apmdProgLT[pmsd->msg - (WM_USER + 1)]->pszMsg;
		else if (mt == MT_STATUSBAR)
			m_mout << m_apmdStatLT[pmsd->msg - (WM_USER + 1)]->pszMsg;
		else if (mt == MT_TOOLBAR)
			m_mout << m_apmdTBLT[pmsd->msg - (WM_USER + 1)]->pszMsg;
		else if (mt == MT_TRACKBAR)
			m_mout << m_apmdTrkLT[pmsd->msg - WM_USER]->pszMsg;
		else if (mt == MT_TABCTRL)
			m_mout << m_apmdTabLT[pmsd->msg - TCM_FIRST]->pszMsg;
		else if (mt == MT_TOOLTIP)
			m_mout << m_apmdTTLT[pmsd->msg - (WM_USER + 1)]->pszMsg;
		else if (mt == MT_TREEVIEW)
			m_mout << m_apmdTVLT[pmsd->msg - TV_FIRST]->pszMsg;
		else if (mt == MT_UPDOWN)
			m_mout << m_apmdUpDnLT[pmsd->msg - (WM_USER + 101)]->pszMsg;
#endif	// DISABLE_WIN95_MESSAGES
		else
			m_mout << m_apmdLT[pmsd->msg]->pszMsg;

		if (pmsd->fPostType == POSTTYPE_RETURN)
		{
			BOOL fDecoded;

			if (m_fShowDecodedReturn)
			{
				if (mt == MT_DIALOG)
					fDecoded = (*m_apmdDlgLT[pmsd->msg - WM_USER]->pfnDecodeRet)(pmsd);
#ifndef DISABLE_WIN95_MESSAGES
				else if (mt == MT_ANIMATE)
					fDecoded = (*m_apmdAniLT[pmsd->msg - (WM_USER + 100)]->pfnDecodeRet)(pmsd);
				else if (mt == MT_HEADER)
					fDecoded = (*m_apmdHdrLT[pmsd->msg - HDM_FIRST]->pfnDecodeRet)(pmsd);
				else if (mt == MT_HOTKEY)
					fDecoded = (*m_apmdHKLT[pmsd->msg - (WM_USER + 1)]->pfnDecodeRet)(pmsd);
				else if (mt == MT_LISTVIEW)
					fDecoded = (*m_apmdLVLT[pmsd->msg - LVM_FIRST]->pfnDecodeRet)(pmsd);
				else if (mt == MT_PROGRESS)
					fDecoded = (*m_apmdProgLT[pmsd->msg - (WM_USER + 1)]->pfnDecodeRet)(pmsd);
				else if (mt == MT_STATUSBAR)
					fDecoded = (*m_apmdStatLT[pmsd->msg - (WM_USER + 1)]->pfnDecodeRet)(pmsd);
				else if (mt == MT_TOOLBAR)
					fDecoded = (*m_apmdTBLT[pmsd->msg - (WM_USER + 1)]->pfnDecodeRet)(pmsd);
				else if (mt == MT_TRACKBAR)
					fDecoded = (*m_apmdTrkLT[pmsd->msg - WM_USER]->pfnDecodeRet)(pmsd);
				else if (mt == MT_TABCTRL)
					fDecoded = (*m_apmdTabLT[pmsd->msg - TCM_FIRST]->pfnDecodeRet)(pmsd);
				else if (mt == MT_TOOLTIP)
					fDecoded = (*m_apmdTTLT[pmsd->msg - (WM_USER + 1)]->pfnDecodeRet)(pmsd);
				else if (mt == MT_TREEVIEW)
					fDecoded = (*m_apmdTVLT[pmsd->msg - TV_FIRST]->pfnDecodeRet)(pmsd);
				else if (mt == MT_UPDOWN)
					fDecoded = (*m_apmdUpDnLT[pmsd->msg - (WM_USER + 101)]->pfnDecodeRet)(pmsd);
#endif	// DISABLE_WIN95_MESSAGES
				else
					fDecoded = (*m_apmdLT[pmsd->msg]->pfnDecodeRet)(pmsd);
			}
			else
			{
				fDecoded = FALSE;
			}

			if (m_fShowRawReturn)
			{
				if (fDecoded)
				{
					m_mout << " [lResult:" << (DWORD)pmsd->lResult << ']';
				}
				else
				{
					m_mout << " lResult:" << (DWORD)pmsd->lResult;
				}
			}

			m_mout.EndLine();
		}
		else
		{
			BOOL fDecoded;

			if (m_fShowDecodedParms)
			{
				if (mt == MT_DIALOG)
					fDecoded = (*m_apmdDlgLT[pmsd->msg - WM_USER]->pfnDecode)(pmsd);
#ifndef DISABLE_WIN95_MESSAGES
				else if (mt == MT_ANIMATE)
					fDecoded = (*m_apmdAniLT[pmsd->msg - (WM_USER + 100)]->pfnDecode)(pmsd);
				else if (mt == MT_HEADER)
					fDecoded = (*m_apmdHdrLT[pmsd->msg - HDM_FIRST]->pfnDecode)(pmsd);
				else if (mt == MT_HOTKEY)
					fDecoded = (*m_apmdHKLT[pmsd->msg - (WM_USER + 1)]->pfnDecode)(pmsd);
				else if (mt == MT_LISTVIEW)
					fDecoded = (*m_apmdLVLT[pmsd->msg - LVM_FIRST]->pfnDecode)(pmsd);
				else if (mt == MT_PROGRESS)
					fDecoded = (*m_apmdProgLT[pmsd->msg - (WM_USER + 1)]->pfnDecode)(pmsd);
				else if (mt == MT_STATUSBAR)
					fDecoded = (*m_apmdStatLT[pmsd->msg - (WM_USER + 1)]->pfnDecode)(pmsd);
				else if (mt == MT_TOOLBAR)
					fDecoded = (*m_apmdTBLT[pmsd->msg - (WM_USER + 1)]->pfnDecode)(pmsd);
				else if (mt == MT_TRACKBAR)
					fDecoded = (*m_apmdTrkLT[pmsd->msg - WM_USER]->pfnDecode)(pmsd);
				else if (mt == MT_TABCTRL)
					fDecoded = (*m_apmdTabLT[pmsd->msg - TCM_FIRST]->pfnDecode)(pmsd);
				else if (mt == MT_TOOLTIP)
					fDecoded = (*m_apmdTTLT[pmsd->msg - (WM_USER + 1)]->pfnDecode)(pmsd);
				else if (mt == MT_TREEVIEW)
					fDecoded = (*m_apmdTVLT[pmsd->msg - TV_FIRST]->pfnDecode)(pmsd);
				else if (mt == MT_UPDOWN)
					fDecoded = (*m_apmdUpDnLT[pmsd->msg - (WM_USER + 101)]->pfnDecode)(pmsd);
#endif	// DISABLE_WIN95_MESSAGES
				else
					fDecoded = (*m_apmdLT[pmsd->msg]->pfnDecode)(pmsd);
			}
			else
			{
				fDecoded = FALSE;
			}

			if (m_fShowRawParms)
			{
				if (fDecoded)
				{
					m_mout << " [wParam:" << (DWORD)pmsd->wParam << " lParam:" << (DWORD)pmsd->lParam;

					if (m_fShowOriginTime && pmsd->fPostType == POSTTYPE_POSTED)
					{
						m_mout << " time:" << FormatDwordTime((DWORD)pmsd->time);
					}

					if (m_fShowMousePosition && pmsd->fPostType == POSTTYPE_POSTED)
					{
						m_mout << " point:" << FormatPoint(pmsd->ptX, pmsd->ptY);
					}

					m_mout << ']';
				}
				else
				{
					m_mout << " wParam:" << (DWORD)pmsd->wParam << " lParam:" << (DWORD)pmsd->lParam;

					if (m_fShowOriginTime && pmsd->fPostType == POSTTYPE_POSTED)
					{
						m_mout << " time:" << FormatDwordTime((DWORD)pmsd->time);
					}

					if (m_fShowMousePosition && pmsd->fPostType == POSTTYPE_POSTED)
					{
						m_mout << " point:" << FormatPoint(pmsd->ptX, pmsd->ptY);
					}
				}
			}

			m_mout.EndLine();
		}
	}
	else
	{
		//
		// Not a normal message.  First print the message number
		// (may include a descriptive string for its type).
		//
		switch (mt)
		{
			case MT_USER:
				m_mout << "message:0x" << (WORD)pmsd->msg << ids(IDS_USERDEFINED) << "WM_USER+" << pmsd->msg - WM_USER << ']';
				break;

			case MT_UNDOCUMENTED:
				m_mout << "message:0x" << (WORD)pmsd->msg << ids(IDS_UNDOCUMENTED);
				break;

			case MT_REGISTERED:
				{
					CHAR sz[256];

					m_mout << "message:0x" << (WORD)pmsd->msg << ids(IDS_REGISTERED);

					if (GetClipboardFormatName(pmsd->msg, sz, sizeof(sz) / sizeof(CHAR)))
					{
						m_mout << ":\"" << sz << '"';
					}

					m_mout << ']';
				}

				break;
		}

		//
		// For return values, print the lResult.  Otherwise print
		// the raw wParam and lParam values.
		//
		if (pmsd->fPostType == POSTTYPE_RETURN)
		{
			m_mout << " lResult:" << (DWORD)pmsd->lResult;
		}
		else
		{
			if (m_fShowDecodedParms || m_fShowRawParms)
			{
				m_mout << " wParam:" << (DWORD)pmsd->wParam << " lParam:" << (DWORD)pmsd->lParam;

				if (m_fShowOriginTime && pmsd->fPostType == POSTTYPE_POSTED)
				{
					m_mout << " time:" << FormatDwordTime((DWORD)pmsd->time);
				}

				if (m_fShowMousePosition && pmsd->fPostType == POSTTYPE_POSTED)
				{
					m_mout << " point:" << FormatPoint(pmsd->ptX, pmsd->ptY);
				}
			}
		}

		m_mout.EndLine();
	}
}


BEGIN_MESSAGE_MAP(CMsgDoc, CDocument)
	//{{AFX_MSG_MAP(CMsgDoc)
	ON_COMMAND(ID_MESSAGES_STARTSTOP, OnMessagesStartStop)
	ON_UPDATE_COMMAND_UI(ID_MESSAGES_STARTSTOP, OnUpdateMessagesStartStop)
	ON_COMMAND(ID_MESSAGES_OPTIONS, OnMessagesOptions)
	ON_UPDATE_COMMAND_UI(ID_MESSAGES_OPTIONS, OnUpdateMessagesOptions)
	ON_COMMAND(ID_MESSAGES_CLEAR, OnMessagesClear)
	ON_UPDATE_COMMAND_UI(ID_MESSAGES_CLEAR, OnUpdateMessagesClear)
	ON_COMMAND(ID_SEARCH_FIND, OnSearchFind)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FIND, OnUpdateSearchFind)
	ON_COMMAND(ID_SEARCH_FINDNEXT, OnSearchFindNext)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FINDNEXT, OnUpdateSearchFindNext)
	ON_COMMAND(ID_SEARCH_FINDPREV, OnSearchFindPrev)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FINDPREV, OnUpdateSearchFindPrev)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgDoc commands

//-----------------------------------------------------------------------------
// CMsgDoc::OnMessagesStartStop
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

void CMsgDoc::OnMessagesStartStop()
{
	if (m_fLogging)
	{
		m_fLogging = FALSE;

		//
		// Disable the hook if there are no other viewers logging.
		//
		m_nLogging--;
		if (m_nLogging == 0)
			EnableHook(FALSE);

		//
		// if logging to a file, close the file handle.
		//
		if (m_fLogToFile && m_hLoggingFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hLoggingFile);
		}
	}
	else
	{
		m_fLogging = TRUE;

		//
		// Enable the hook if there were no previous views logging.
		//
		m_nLogging++;
		if (m_nLogging == 1)
			EnableHook(TRUE);

		//
		// if logging to a file, open the file and set file pointer to end
		// of file so as to continue the log where it left off previously.
		//
		if (m_fLogToFile)
		{
			m_hLoggingFile = CreateFile((LPCSTR)m_strLogFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (m_hLoggingFile != INVALID_HANDLE_VALUE)
				SetFilePointer(m_hLoggingFile, 0, NULL, FILE_END);
			else
			{
				DWORD dwError = GetLastError();
				SpyMessageBox(IDS_CANNOTLOGTOFILE2);
				m_fLogToFile = FALSE;
			}
		}
	}
}

void CMsgDoc::OnUpdateMessagesStartStop(CCmdUI* pCmdUI)
{
	int iDesiredImage;

	if (m_fLogging)
	{
		pCmdUI->SetText(ids(IDS_STOPLOGGINGMENU));
		pCmdUI->Enable(TRUE);
		iDesiredImage = INDEX_STOP_IMAGE;
	}
	else
	{
		pCmdUI->SetText(ids(IDS_STARTLOGGINGMENU));
		pCmdUI->Enable(m_nObjectType != OT_NONE || m_fAllWindows);
		iDesiredImage = INDEX_START_IMAGE;
	}

	CToolBar* pToolBar = &((CMainFrame*)theApp.m_pMainWnd)->m_wndToolBar;
	int iToolBtn = pToolBar->CommandToIndex(ID_MESSAGES_STARTSTOP);
	UINT nID;
	UINT nStyle;
	int iImage;
	pToolBar->GetButtonInfo(iToolBtn, nID, nStyle, iImage);

	if (iImage != iDesiredImage)
	{
		pToolBar->SetButtonInfo(iToolBtn, nID, nStyle, iDesiredImage);
	}
}

void CMsgDoc::OnMessagesOptions()
{
	ShowMessagesOptions(m_iInitialTab);
}

void CMsgDoc::OnUpdateMessagesOptions(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_fLogging || !m_fLogToFile);
}

int CMsgDoc::ShowMessagesOptions(UINT iInitialTab)
{
	CFiltersTabbedDialog dlg(IDS_MESSAGESTREAMFILTERS, NULL, iInitialTab);

	dlg.AddTab(new CFiltersWindowDlgTab(IDD_FILTERSWINDOWTAB, IDS_WINDOWSTAB));
	dlg.AddTab(new CFiltersMessageDlgTab(IDD_FILTERSMESSAGETAB, IDS_MESSAGESTAB));
	dlg.AddTab(new CFiltersOutputDlgTab(IDD_FILTERSOUTPUTTAB, IDS_OUTPUTTAB));

	dlg.SetDocPointer(this);

	return(dlg.DoModal());
}

void CMsgDoc::OnMessagesClear()
{
	//
	// Delete all the lines in the view
	//
	m_pMsgView->m_MsgLog.ClearAll();

	//
	// if logging to a file, clear the file as well...
	//
	if (m_fLogToFile)
	{
		if (IsLogging() && m_hLoggingFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hLoggingFile);
			m_hLoggingFile = CreateFile((LPCSTR)m_strLogFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		else
		{
			m_hLoggingFile = CreateFile((LPCSTR)m_strLogFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			CloseHandle(m_hLoggingFile);
		}
	}
}

void CMsgDoc::OnUpdateMessagesClear(CCmdUI* pCmdUI)
{
	//
	// Enable only if there are lines to clear
	//
	pCmdUI->Enable(!m_pMsgView->m_MsgLog.IsEmpty());
}

void CMsgDoc::OnSearchFind()
{
	DoFind();
	return;
}

void CMsgDoc::OnUpdateSearchFind(CCmdUI* pCmdUI)
{
	pCmdUI->SetText(ids(IDS_MENU_FINDMESSAGE));
	pCmdUI->Enable(!m_pMsgView->m_MsgLog.IsEmpty());
	return;
}

void CMsgDoc::OnSearchFindNext()
{
	DoFindNext();
	return;
}

void CMsgDoc::OnUpdateSearchFindNext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_pMsgView->m_MsgLog.IsEmpty() && HasValidFind());
	return;
}

void CMsgDoc::OnSearchFindPrev()
{
	DoFindPrev();
	return;
}

void CMsgDoc::OnUpdateSearchFindPrev(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_pMsgView->m_MsgLog.IsEmpty() && HasValidFind());
	return;
}

BOOL CMsgDoc::DoFind()
{
#ifdef USE_TABBED_SEARCH_DIALOGS
	CSearchTabbedDialog dlgTest(IDS_SEARCHCAPTION, NULL, (UINT)-1);

	dlgTest.AddTab(new CSearchWindowDlgTab(IDD_SEARCHMESSAGETAB, IDS_SEARCHMESSAGETAB));

	dlgTest.DoModal();
#else	// USE_TABBED_SEARCH_DIALOGS
	CSearchMessageDlg dlg;
	CMsgView *pmvActive = (CMsgView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());
	LINE *pLineArray = pmvActive->m_MsgLog.GetLinesArray();
	int iCur = pmvActive->m_MsgLog.GetCurSel(),
		iFirst = pmvActive->m_MsgLog.GetFirstIndex(), 
		iMax = pmvActive->m_MsgLog.GetNumLines();
	LINE *pLine = &pLineArray[(iCur + iFirst) % iMax];

	m_hwndSelected = pLine->msd2.hwnd;
	m_wMsgSelected = pLine->msd2.msg;
	m_uTypeSelected = pLine->msd2.fPostType;

	dlg.SetWindow(m_hwndSelected); 
	dlg.SetMessage(m_wMsgSelected);
	dlg.SetType(m_uTypeSelected);
	dlg.SetDirection(m_fSearchUp);

	if (dlg.DoModal() == TRUE)
	{
		m_wSearchFlags	= dlg.GetSearchFlags();
		m_hwndSelected	= dlg.GetWindow();
		m_wMsgSelected	= dlg.GetMessage();
		m_uTypeSelected	= dlg.GetType();
		m_fSearchUp		= dlg.GetDirection();

		if (m_fSearchUp)
		{
			if (!FindPreviousMatchingLine() && !FindLastMatchingLine())
			{
				// notify user that no matching node found
				SpyMessageBox(IDS_NO_MATCHING_MSG);
			}
		}
		else
		{
			if (!FindNextMatchingLine() && !FindFirstMatchingLine())
			{
				// notify user that no matching node found
				SpyMessageBox(IDS_NO_MATCHING_MSG);
			}
		}
	}
#endif	// USE_TABBED_SEARCH_DIALOGS

	return(TRUE);
}

BOOL CMsgDoc::DoFindNext()
{
	if (!FindNextMatchingLine() && !FindFirstMatchingLine())
	{
		// notify user that no next matching node found
		SpyMessageBox(IDS_NO_NEXT_MATCH_MSG);
	}

	return(TRUE);
}

BOOL CMsgDoc::DoFindPrev()
{
	if (!FindPreviousMatchingLine() && !FindLastMatchingLine())
	{
		// notify user that no previous matching node found
		SpyMessageBox(IDS_NO_PREV_MATCH_MSG);
	}

	return(TRUE);
}

BOOL CMsgDoc::HasValidFind()
{
	BOOL fRet = (m_wSearchFlags != 0);

	if (fRet && m_wSearchFlags & SRCHFLAG_MESSAGE_USEHANDLE)
	{
		fRet &= m_hwndSelected != NULL;
	}

	if (fRet && m_wSearchFlags & SRCHFLAG_MESSAGE_USEMSG)
	{
		fRet &= m_wMsgSelected != 0;
	}

	if (fRet && m_wSearchFlags & SRCHFLAG_MESSAGE_USEPOSTTYPE)
	{
		fRet &= (m_uTypeSelected >= 0 && m_uTypeSelected <= 3);
	}

	return(fRet);
}

BOOL CMsgDoc::CompareLine(LINE *pLine)
{
	// do comparison of node contents with search criteria
	switch (m_wSearchFlags)
	{
		case SRCHFLAG_MESSAGE_USEHANDLE:

			if (pLine->msd2.hwnd == m_hwndSelected)
			{
				return(TRUE);
			}
			break;

		case SRCHFLAG_MESSAGE_USEMSG:

			if ((pLine->msd2.hwnd != NULL) && 
				(pLine->msd2.msg == m_wMsgSelected))
			{
				return(TRUE);
			}
			break;

		case SRCHFLAG_MESSAGE_USEPOSTTYPE:

			if ((pLine->msd2.hwnd != NULL) && 
				(pLine->msd2.fPostType == m_uTypeSelected))
			{
				return(TRUE);
			}
			break;

		case (SRCHFLAG_MESSAGE_USEHANDLE | SRCHFLAG_MESSAGE_USEMSG):

			if ((pLine->msd2.hwnd == m_hwndSelected) && 
				(pLine->msd2.msg == m_wMsgSelected))
			{
				return(TRUE);
			}
			break;

		case (SRCHFLAG_MESSAGE_USEHANDLE | SRCHFLAG_MESSAGE_USEPOSTTYPE):

			if ((pLine->msd2.hwnd == m_hwndSelected) &&
				(pLine->msd2.fPostType == m_uTypeSelected))
			{
				return(TRUE);
			}
			break;

		case (SRCHFLAG_MESSAGE_USEPOSTTYPE | SRCHFLAG_MESSAGE_USEMSG):

			if ((pLine->msd2.hwnd != NULL) && 
				(pLine->msd2.fPostType == m_uTypeSelected) &&
				(pLine->msd2.msg == m_wMsgSelected))
			{
				return(TRUE);
			}
			break;

		case (SRCHFLAG_MESSAGE_USEHANDLE | SRCHFLAG_MESSAGE_USEPOSTTYPE | SRCHFLAG_MESSAGE_USEMSG):

			if ((pLine->msd2.hwnd == m_hwndSelected) &&
				(pLine->msd2.msg == m_wMsgSelected) &&
				(pLine->msd2.fPostType == m_uTypeSelected))
			{
				return(TRUE);
			}
			break;

		default:
			return(FALSE);
	}

	return(FALSE);
}

BOOL CMsgDoc::FindFirstMatchingLine()
{
	CMsgView *pmvActive;
	
	pmvActive = (CMsgView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	LINE *pLine;
	LINE *pLineArray = pmvActive->m_MsgLog.GetLinesArray();
	int iFirst = pmvActive->m_MsgLog.GetFirstIndex(), 
		iMax = pmvActive->m_MsgLog.GetNumLines();

	for (int i = 0; i < iMax; i++)
	{
		pLine = &pLineArray[(i + iFirst) % iMax];

		// do comparison of node contents with search criteria
		if (CompareLine(pLine))
		{
			pmvActive->m_MsgLog.SelectLine(i);
			pmvActive->m_MsgLog.ShowSelection();
			return(TRUE);
		}
	}

	return(FALSE);
}

BOOL CMsgDoc::FindNextMatchingLine()
{
	CMsgView *pmvActive;
	
	pmvActive = (CMsgView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	LINE *pLine;
	LINE *pLineArray = pmvActive->m_MsgLog.GetLinesArray();
	int iFirst = pmvActive->m_MsgLog.GetFirstIndex(), 
		iCur = pmvActive->m_MsgLog.GetCurSel(),
		iMax = pmvActive->m_MsgLog.GetNumLines();

	for (int i = iCur + 1; i < iMax; i++)
	{
		pLine = &pLineArray[(i + iFirst) % iMax];

		// do comparison of node contents with search criteria
		// do comparison of node contents with search criteria
		if (CompareLine(pLine))
		{
			pmvActive->m_MsgLog.SelectLine(i);
			pmvActive->m_MsgLog.ShowSelection();
			return(TRUE);
		}
	}

	return(FALSE);
}

BOOL CMsgDoc::FindPreviousMatchingLine()
{
	CMsgView *pmvActive;
	
	pmvActive = (CMsgView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	LINE *pLine;
	LINE *pLineArray = pmvActive->m_MsgLog.GetLinesArray();
	int iFirst = pmvActive->m_MsgLog.GetFirstIndex(), 
		iCur = pmvActive->m_MsgLog.GetCurSel(),
		iMax = pmvActive->m_MsgLog.GetNumLines();

	for (int i = iCur - 1; i >= 0; i--)
	{
		pLine = &pLineArray[(i + iFirst) % iMax];

		// do comparison of node contents with search criteria
		// do comparison of node contents with search criteria
		if (CompareLine(pLine))
		{
			pmvActive->m_MsgLog.SelectLine(i);
			pmvActive->m_MsgLog.ShowSelection();
			return(TRUE);
		}
	}

	return(FALSE);
}

BOOL CMsgDoc::FindLastMatchingLine()
{
	CMsgView *pmvActive;
	
	pmvActive = (CMsgView *)(((CMDIFrameWnd *)(theApp.m_pMainWnd))->GetActiveFrame()->GetActiveView());

	LINE *pLine;
	LINE *pLineArray = pmvActive->m_MsgLog.GetLinesArray();
	int iFirst = pmvActive->m_MsgLog.GetFirstIndex(), 
		iMax = pmvActive->m_MsgLog.GetNumLines();

	for (int i = iMax - 1; i >= 0; i--)
	{
		pLine = &pLineArray[(i + iFirst) % iMax];

		// do comparison of node contents with search criteria
		// do comparison of node contents with search criteria
		if (CompareLine(pLine))
		{
			pmvActive->m_MsgLog.SelectLine(i);
			pmvActive->m_MsgLog.ShowSelection();
			return(TRUE);
		}
	}

	return(FALSE);
}





