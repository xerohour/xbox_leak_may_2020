////////////////////////////////////////////////////////////////////////////
//
// Docking windows and default toolbars for the super simple package
//

#include "stdafx.h"		// Windows, MFC, and Shell headers (pre-compiled)
#pragma hdrstop

void GetDebugDockInit(int winType, DOCKINIT FAR* lpdi);

BOOL g_bDockAvailable = TRUE;

/*
CDebugDockWnd* g_pTheDebugDockWnd = NULL;

CDebugDockWnd::CDebugDockWnd()
{
	m_pPacket = theApp.GetPacket(PACKAGE_DEBUG, PACKET_DEBUG);
}

LRESULT CDebugDockWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	if ( DkPreHandleMessage(m_hWnd, message, wParam, lParam, &lResult) )
		return( lResult );

	return( CDockablePartView::WindowProc(message, wParam, lParam) );
}

void CDebugDockWnd::OnNcDestroy()
{
	ASSERT(this == g_pTheDebugDockWnd);

	g_pTheDebugDockWnd = NULL;

	delete this;
}
*/

///////////////////////////////////////////////////////////////////////////////
//	Dockable windows interface
//
static const DOCK_IDS_TAB g_rgDockViewIDs[] =
{
	IDDW_VCPP_CPU_WIN,     PACKET_MULTI, IDR_CPU_TYPE,	   SYS_CpuWin_Title,     CPU_WIN,
	IDDW_VCPP_MEMORY_WIN,  PACKET_MULTI, IDR_MEMORY_TYPE,  SYS_MemoryWin_Title,  MEMORY_WIN,
	IDDW_VCPP_CALLS_WIN,   PACKET_MULTI, IDR_CALLS_TYPE,   SYS_CallsWin_Title,   CALLS_WIN,
	IDDW_VCPP_DISASSY_WIN, PACKET_MULTI, IDR_DISASSY_TYPE, SYS_DisassyWin_Title, DISASSY_WIN,
};

const int g_cDebugDockables = sizeof(g_rgDockViewIDs) / sizeof(DOCK_IDS_TAB);


const DOCK_IDS_TAB* DockTabFromID(UINT nID)
{
	for (int i = 0; i < g_cDebugDockables; i++)
	{
		if (g_rgDockViewIDs[i].nID == nID)
			return &g_rgDockViewIDs[i];
	}

	return NULL;
}

const DOCK_IDS_TAB* DockTabFromType(DOCTYPE type)
{
	for (int i = 0; i < g_cDebugDockables; i++)
	{
		if (g_rgDockViewIDs[i].type == type)
			return &g_rgDockViewIDs[i];
	}

	return NULL;
}

// Dock related information for the variables windows.

class CDockWndInfo
{
    public:
    	UINT nID;
    	UINT nIDPacket;
    	UINT nIDResource;
    	UINT nIDString;

    	static RECT s_rectInit;

    	// member functions.
    	HICON GetIcon( );
    	CString GetTitle( );
    	VOID GetDebugDockInit(DOCKINIT * );
};

HICON CDockWndInfo::GetIcon( )
{
	return ::LoadIcon(hInst, MAKEINTRESOURCE(nIDResource));
}

CString CDockWndInfo::GetTitle( )
{
	CString strTitle;
	
	strTitle.LoadString(nIDString);
	return strTitle;
}			

VOID CDockWndInfo::GetDebugDockInit( DOCKINIT * lpdi )
{
	lpdi->dt = dtEdit;
	lpdi->nIDWnd = nID;
	lpdi->nIDPackage = PACKAGE_DEBUG;
	lpdi->nIDPacket = nIDPacket;
	lpdi->dpInit = dpNil;
	lpdi->dpPrev = dpNil;
	lpdi->rectFloat = s_rectInit;

	lpdi->fInit = INIT_CANVERT | INIT_CANHORZ | INIT_ASKAVAILABLE;
	if ( gpIBldSys == NULL)
		lpdi->fInit |= INIT_REMOVE;

	lstrcpy(lpdi->szCaption, GetTitle( ));
	
	OffsetRect(&s_rectInit, 20, 20); // Offset the next one.
}

// Currently only the watch and variables window use this structure.
static CDockWndInfo s_rgDockWndInfo[ ] =
{
	{ IDDW_VCPP_WATCH_WIN,  PACKET_BASE, IDR_WATCH_TYPE, SYS_WatchWin_Title },
	{ IDDW_VCPP_VARIABLES_WIN, PACKET_BASE, IDR_LOCALS_TYPE, SYS_VarsWin_Title},
};	


// Default position for the first dockable window in our list.
RECT CDockWndInfo::s_rectInit = { 100, 100, 450, 250 };

static const int s_cVarsDockable = sizeof(s_rgDockWndInfo)/sizeof(s_rgDockWndInfo[0]);
	
CDockWndInfo * GetDockWndInfo(UINT nID)
{
	CDockWndInfo * pInfo = NULL;
	
	for ( int i = 0 ; i < s_cVarsDockable ; i++ )
	{
		if ( s_rgDockWndInfo[i].nID == nID )
			pInfo = &s_rgDockWndInfo[i];
	}
	
	return pInfo;
}					


CView *CreateView(int type)
{
	int                 nIDWnd = 0;
	const DOCK_IDS_TAB* pTab   = DockTabFromType((DOCTYPE) type);
	DWORD               style  = WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_BORDER;

	if (pTab != NULL)
		nIDWnd = pTab->nID;

	if (type != DOC_WIN)
		style |= DWS_STRETCHY;

	// Create the appropriate kind of CView subclass here.
	// [19-Jul-93 mattg]

	HWND       hwndEdit;
	CPartView* pPartView;
	
	switch (type)
	{
		case DOC_WIN:
            ASSERT(FALSE);
			break;

		case CPU_WIN:
			pViewCpu = (CIDEView *)(pPartView = new CCpuView() );
			break;
		case DISASSY_WIN:
			pViewDisassy = (CIDEView *)(pPartView = new CDAMView() );
			break;
		case CALLS_WIN:
			pViewCalls = (CIDEView *)(pPartView = new CCallsView() );
			break;
		case MEMORY_WIN:
			pViewMemory = (CIDEView *)( pPartView = new CMemoryView() );
			break;

		default:
			ASSERT(FALSE);
			break;
	}

	ASSERT(pPartView->IsKindOf(RUNTIME_CLASS(CPartView)));

    ASSERT(IsWindow(theApp.m_pMainWnd->m_hWnd));

	if (!((CView *)pPartView)->Create(AfxRegisterWndClass(CS_DBLCLKS),
							          NULL, style, CRect(0, 0, 0, 0),
							          theApp.m_pMainWnd, nIDWnd))
		return(NULL);

	hwndEdit = pPartView->m_hWnd;

	if ( FMultiEditView(pPartView) && DebuggeeAlive( ) )
	{
		// We should not be opening the debugger windows if the debuggee
		// is not alive. If we ever decide to change this behavior, we need to
		// find another place to send the WU_INITDEBUGWIN after the
		// debugging actually starts. -- sanjays --

		pPartView->SendMessage(WU_INITDEBUGWIN, 0, 0);
	}

	return (CView *)pPartView;
}

HGLOBAL CDebugPackage::GetDockInfo()
{
	// DkWMapDockingCmd(MAKEDOCKID(PACKAGE_VCPP, IDDW_OUTPUT), IDM_ACTIVATE_OUTPUT);
	// DkWMapDockingCmd(MAKEDOCKID(PACKAGE_VCPP, IDDW_OUTPUT), IDM_TOGGLE_OUTPUT);

	if (gpIBldSys != NULL )
	{
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_WATCH_WIN),       IDM_ACTIVATE_WATCH);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_WATCH_WIN),       IDM_TOGGLE_WATCH);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_VARIABLES_WIN),   IDM_ACTIVATE_VARIABLES);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_VARIABLES_WIN),   IDM_TOGGLE_VARIABLES);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_CPU_WIN),         IDM_ACTIVATE_REGISTERS);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_CPU_WIN),         IDM_TOGGLE_REGISTERS);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_MEMORY_WIN),      IDM_ACTIVATE_MEMORY);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_MEMORY_WIN),      IDM_TOGGLE_MEMORY);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_CALLS_WIN),       IDM_ACTIVATE_CALLSTACK);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_CALLS_WIN),       IDM_TOGGLE_CALLSTACK);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_DISASSY_WIN),     IDM_ACTIVATE_DISASSEMBLY);
		DkWMapDockingCmd(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_DISASSY_WIN),     IDM_TOGGLE_DISASSEMBLY);
	}

	// When adding enties to rgdiStat[] make sure to add corresponding entries in
	// rgCaptionID[].
	static const UINT rgCaptionID[] = { IDS_DEBUG };

    static DOCKINIT BASED_CODE rgdiStat[] =
    {
		{ IDTB_VCPP_DEBUG, PACKAGE_DEBUG, PACKET_NIL, "",
         INIT_CANHORZ | INIT_CANVERT | INIT_POSITION | INIT_ASKAVAILABLE,
    	 dtToolbar, dpNil, dpNil, {0,0,0,0} }
    };

	// These arrays need to match.
	ASSERT(sizeof(rgdiStat) / sizeof(DOCKINIT) ==
		sizeof(rgCaptionID) / sizeof(UINT));

	const int cStatDockables = sizeof(rgdiStat) / sizeof(DOCKINIT);

	CString str;

	for (int i = 0; i < cStatDockables; i++)
	{
		VERIFY(str.LoadString(rgCaptionID[i]));
		ASSERT(str.GetLength() <= _MAX_DOCK_CAPTION);
		lstrcpy(rgdiStat[i].szCaption, str);
	}

	DOCKINIT    rgdi[g_cDebugDockables + s_cVarsDockable + cStatDockables];

	for (i = 0; i < g_cDebugDockables; i++)
		GetDebugDockInit(g_rgDockViewIDs[i].type, &rgdi[i]);

	for ( i = 0; i < s_cVarsDockable; i++)
		s_rgDockWndInfo[i].GetDebugDockInit(&rgdi[i + g_cDebugDockables]);

	for (i = 0; i < cStatDockables; i++)
		rgdi[i + g_cDebugDockables + s_cVarsDockable] = rgdiStat[i];

	return( DkInfoData(sizeof(rgdi) / sizeof(DOCKINIT), rgdi) );
}

HWND CDebugPackage::GetDockableWindow(UINT nID, HGLOBAL hglob)
{
	// Docking views.
	const DOCK_IDS_TAB* pTab = DockTabFromID(nID);

	if ( pTab != NULL )
	{
		EnsureDebugWindow(pTab->type);

		HWND    hwnd = GetDebugWindowHandle(pTab->type);

		switch (pTab->type)
		{
			case MEMORY_WIN:
				hwnd = ::GetParent(hwnd);

			default:
				break;
		}
		
		return( hwnd );
	}
	else
	{
		CBaseVarsView * pBaseVarsView = NULL;

		// One of the new watch or variables windows
		switch ( nID )
		{
			case IDDW_VCPP_WATCH_WIN:
				pBaseVarsView = g_pWatchView = new CGridWatchView;
				break;

			case IDDW_VCPP_VARIABLES_WIN:
				pBaseVarsView = g_pVarsView = new CVarsView;
				break;

			default:
				break;
		}

		if ( pBaseVarsView != NULL )
		{
			pBaseVarsView->EnsureWindow( );
			return( pBaseVarsView->GetSafeHwnd( ) );
		}
	}

	return( CPackage::GetDockableWindow(nID, hglob) );
}

HGLOBAL CDebugPackage::GetToolbarData(UINT nID)
{
	static const UINT BASED_CODE debug_buttons[] =
	{
		IDM_RUN_RESTART,
		IDM_RUN_STOPDEBUGGING,
		IDM_RUN_BREAK,
		IDM_DEBUG_UPDATEIMAGE,
	    0,
		IDM_DEBUG_SHOWIP,
	    IDM_RUN_TRACEINTO,
		// FUTURE:  Give this a glyph in vcpp.cmd before adding
		// 	to a toolbar!
	    //IDM_RUN_TRACEFUNCTION,
	    IDM_RUN_STEPOVER,
	    IDM_RUN_STOPAFTERRETURN,
		IDM_RUN_TOCURSOR,
	    0,
		IDM_RUN_QUICKWATCH,
	    0,
		IDM_TOGGLE_WATCH,
		IDM_TOGGLE_VARIABLES,
		IDM_TOGGLE_REGISTERS,
		IDM_TOGGLE_MEMORY,
		IDM_TOGGLE_CALLSTACK,
		IDM_TOGGLE_DISASSEMBLY,
	};

	TOOLBARINIT tbi;

	tbi.nIDWnd = nID;

	if ( nID == IDTB_VCPP_DEBUG )
	{
        tbi.nIDCount = sizeof(debug_buttons)/sizeof(UINT);
        return( DkToolbarData(&tbi, debug_buttons) );
	}

	return CPackage::GetToolbarData(nID);
}

HICON CDebugPackage::GetDockableIcon(UINT nID)
{
	const DOCK_IDS_TAB* pTab = DockTabFromID(nID);

	if ( pTab != NULL )
		return ::LoadIcon(hInst, MAKEINTRESOURCE(pTab->nIDResource));
	else {
		CDockWndInfo *pDockWndInfo = GetDockWndInfo(nID);
	 	ASSERT(pDockWndInfo != NULL);

		return pDockWndInfo->GetIcon( );
	}
}

BOOL CDebugPackage::AskAvailable(UINT nID)
{
	if ( nID == IDTB_VCPP_DEBUG )
		return( gpIBldSys != NULL || fVisTestPresent );
	else
	{
		// The registers and memory windows are always disabled for Java.
		if ((nID == IDDW_VCPP_CPU_WIN || nID == IDDW_VCPP_MEMORY_WIN) && pDebugCurr && pDebugCurr->IsJava())
		{
			return FALSE;
		}
		return( IS_STATE_DEBUG(DkGetDockState()) );
	}
}

void GetDebugDockInit(int winType, DOCKINIT FAR* lpdi)
{
	static RECT rectInit = { 100, 100, 450, 250 };

	const DOCK_IDS_TAB* pTab = DockTabFromType((DOCTYPE) winType);
	ASSERT(pTab != NULL);

	lpdi->dt = dtEdit;
	lpdi->nIDWnd = pTab->nID;
	lpdi->nIDPackage = PACKAGE_DEBUG;
	lpdi->nIDPacket = pTab->nIDPacket;
	lpdi->dpInit = dpNil;
	lpdi->dpPrev = dpNil;
	lpdi->rectFloat = rectInit;

	lpdi->fInit = INIT_CANVERT | INIT_CANHORZ;

    if (winType == DISASSY_WIN)
        lpdi->fInit |= INIT_MDI;

    lpdi->fInit |= INIT_ASKAVAILABLE;

    // Debugger UI should not be enabled.
    if (gpIBldSys == NULL)
        lpdi->fInit |= INIT_REMOVE;

	CString str;

	str.LoadString(pTab->nIDString);
	ASSERT(str.GetLength() <= _MAX_DOCK_CAPTION);
	lstrcpy(lpdi->szCaption, str);

	OffsetRect(&rectInit, 20, 20);	// Offset the next one.
}


