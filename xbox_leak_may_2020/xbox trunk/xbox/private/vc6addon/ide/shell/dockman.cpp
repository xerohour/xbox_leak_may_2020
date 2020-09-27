///////////////////////////////////////////////////////////////////////////////
//  DOCKMAN.CPP
//      Contains most of the general dock managenment code for a CFrameWnd,
//      implemented in two classes: CDockWorker, and CDockManager.
//      See also DOCKWNDS, and DOCKAPI.
///////////////////////////////////////////////////////////////////////////////

// Implementation of embedded toolbars. Because we don't have time to implement a full-blown embedded toolbar,
// we're making an embedded toolbar a pseudo-docking window.

#include "stdafx.h"

#include "shell.h"
#include "resource.h"
#include "dockman.h"
#include "barbdr.h"
#include "bardockx.h"
#include "bardlgs.h"
#include "about.h"
#include "prxycust.h"

//[multimon] 11 mar 97 - der
#include "mmonitor.h"

IMPLEMENT_SERIAL(CDockWorker, CObject, 1);
IMPLEMENT_SERIAL(CDockManager, CObject, 1)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Worker messages

UINT DMM_CHILDSETFOCUS = RegisterMessage("DockManChildSetFocus");
UINT DMM_CHILDKILLFOCUS = RegisterMessage("DockManChildKillFocus");
UINT DMM_NCLBUTTONDOWN = RegisterMessage("DockManNcLButtonDown");
UINT DMM_NCLBUTTONDBLCLK = RegisterMessage("DockManNcLButtonDblClk");

// Value of s_nInsertTolerance when insertion is allowed/disallowed
static const int s_cnAllowInsert=4;
static const int s_cnDisallowInsert=0;

// current insert tolerance
int CDockWorker::s_nInsertTolerance=s_cnDisallowInsert;


///////////////////////////////////////////////////////////////////////////////
//  CDockWorker
//      The dock worker is the odject that does most of the work for
//      a dockable HWND.  All DMM_* eventually end up here, and most of
//      docking information for a dockable HWND is stored in this object.
//

static int DkCompareIDs(UINT nID1, UINT nID2)
{
	if (LOWORD(nID1) > LOWORD(nID2))
		return 1;
	else if (LOWORD(nID1) < LOWORD(nID2))
		return -1;
	else if (HIWORD(nID1) > HIWORD(nID2))
		return 1;
	else if (HIWORD(nID1) < HIWORD(nID2))
		return -1;

	return 0;
}

CWnd* g_pActivePopup = NULL;

// Really sizes (point for compatibility with MINMAXINFO).
CPoint CDockWorker::GetDefMin(void)
{
	return CPoint(g_mt.cxToolCapButton+g_mt.cxToolCapBorder, g_mt.cyToolCapButton+g_mt.cyToolCapBorder);
}

CPoint CDockWorker::s_ptDefMax(500, 500);

LPCSTR CDockWorker::s_lpszLayoutSection = NULL;

BOOL CDockWorker::s_bLockDocker = FALSE;
CDockWorker* CDockWorker::s_pActiveDocker = NULL;

//	CDockWorker::CDockWorker
//		Construction.  For deserialization.

CDockWorker::CDockWorker()
{
	m_bAvailable = FALSE;
	m_bMDIBound = FALSE;
	m_bLoading = TRUE;
	m_pWnd = NULL;
	m_hglobInit = NULL;
	m_pDeskPlace = NULL;
	m_pDockPlace = NULL;
	m_pDockSerial = NULL;
}

#ifdef _DEBUG
static void DebugCheckDockID(UINT nID, DOCKTYPE dt)
{
	// Make sure things are in the right ranges.
	WORD wPkg = HIWORD(nID);
	WORD wID = LOWORD(nID);

	// Note: we have a few historical hacks for the project and edit toolbars.
	if ((wPkg == PACKAGE_SUSHI && wID != 0x75FF) ||
		(wPkg == PACKAGE_VPROJ && wID == IDTB_PROJECT))
	{
		if (dt == dtToolbar || dt == dtEmbeddedToolbar)
			ASSERT(wID >= IDTB_CUSTOM_BASE ||
				(wID >= IDTB_SHELL_BASE && wID <= IDTB_SHELL_MAX));
		else if (dt == dtEdit)
			ASSERT(wID >= IDDW_SHELL_BASE && wID <= IDDW_SHELL_MAX);
		else
			ASSERT((wID >= IDDW_SHELL_BASE && wID <= IDDW_SHELL_MAX) ||
				(wID >= IDTB_SHELL_BASE && wID <= IDTB_SHELL_MAX));
	}
	else
	{
		if (dt == dtToolbar || dt == dtEmbeddedToolbar)
			ASSERT(wID >= IDTB_PACKAGE_BASE && wID <= IDTB_PACKAGE_MAX);
		else if (dt == dtEdit) 
			ASSERT(wID >= IDDW_PACKAGE_BASE && wID <= IDDW_PACKAGE_MAX);
		else
			ASSERT((wID >= IDTB_PACKAGE_BASE && wID <= IDTB_PACKAGE_MAX) ||
				(wID >= IDDW_PACKAGE_BASE && wID <= IDDW_PACKAGE_MAX));
	}
}
#endif

//	CDockWorker::Serialize
//		Load/Save of dock worker to disk.

void CDockWorker::Serialize(CArchive& ar)
{
	DWORD fwInit;

	if (ar.IsStoring())
	{
		BOOL bTempMDIInfo = FALSE, bTempDockInfo = FALSE;
		if (m_pWnd != NULL)
		{
			if (m_bMDIBound)
			{
				if (IsVisible())
				{
					bTempMDIInfo = TRUE;
					SaveMDIPos();
				}
			}
			else if (m_dp == dpNil || m_dp==dpEmbedded)
			{
				SaveFloatPos();
			}
			else if (IsVisible())
			{
				bTempDockInfo = TRUE;
				SaveDockPos(FALSE);	// Absolute position.
				SaveDockPos(TRUE);	// Relative position.
			}
		}

		fwInit = m_bCanHorz ? INIT_CANHORZ : 0;
		fwInit |= m_bCanVert ? INIT_CANVERT : 0;
		fwInit |= m_bPosition ? INIT_POSITION : 0;
		fwInit |= (m_bVisible && m_dp!=dpEmbedded) ? INIT_VISIBLE : 0;
		fwInit |= m_bTranspose ? INIT_TRANSPOSE_SIZE : 0;
		fwInit |= m_bAsk ? INIT_ASKAVAILABLE : 0;
		fwInit |= m_bLastAvailable ? INIT_LASTAVAILABLE : 0;
		fwInit |= m_bDestroyOnHide ? 0 : INIT_NODESTROY;
		fwInit |= m_bAvailable ? INIT_AVAILABLE : 0;
		fwInit |= m_bMDIBound ? INIT_MDI : 0;
		fwInit |= (m_pDeskPlace != NULL) ? INIT_DESKPLACE : 0;
		fwInit |= (m_pDockPlace != NULL) ? INIT_DOCKPLACE : 0;
		fwInit |= (m_pDockSerial != NULL) ? INIT_DOCKSERIAL : 0;
		fwInit |= m_bHost ? INIT_HOST : 0;

		ar << m_strTitle <<	fwInit << (WORD) m_dtWnd << (WORD) m_dp
			<< (WORD) m_nIDPackage << (WORD) m_nIDPacket
			<< (WORD) m_nIDWnd << m_rectFloat << m_sizeLastStretch << m_nIDAssociate;

		if (m_pDockSerial != NULL)
		{
			ar.Write(m_pDockSerial, sizeof(DOCKPLACEMENT));
			delete m_pDockSerial;
			m_pDockSerial = NULL;
		}

		if (m_pDockPlace != NULL)
		{
			ar.Write(m_pDockPlace, sizeof(DOCKPLACEMENT));
			if (bTempDockInfo)
			{
				delete m_pDockPlace;
				m_pDockPlace = NULL;
			}
		}

		if (m_pDeskPlace != NULL)
		{
			ar.Write(m_pDeskPlace, sizeof(DESKPLACEMENT));
			if (bTempMDIInfo)
			{
				delete m_pDeskPlace;
				m_pDeskPlace = NULL;
			}
		}
	}
	else
	{
		WORD wdt, wdp, wIDGroup, wIDPart, wIDWnd;

		ar >> m_strTitle >>	fwInit >> wdt >> wdp
			>> wIDGroup >> wIDPart >> wIDWnd
			>> m_rectFloat >> m_sizeLastStretch >> m_nIDAssociate;

		m_dtWnd = (DOCKTYPE) wdt;
		m_dp = (DOCKPOS) wdp;

		// REVIEW(BrendanX): Remove this the next time we redo default layouts.
		if (wIDGroup == PACKAGE_SUSHI && wIDWnd == 0x75FF)
			wIDGroup = PACKAGE_VCPP;

		m_nIDPackage = wIDGroup;
		m_nIDPacket = wIDPart;
		m_nIDWnd = MAKEDOCKID(wIDGroup, wIDWnd);

		m_bCanHorz = ((fwInit & INIT_CANHORZ) != 0);
		m_bCanVert = ((fwInit & INIT_CANVERT) != 0);
		m_bPosition = ((fwInit & INIT_POSITION) != 0);
		m_bVisible = ((fwInit & INIT_VISIBLE) != 0);
		m_bTranspose = ((fwInit & INIT_TRANSPOSE_SIZE) != 0);
		m_bAsk = ((fwInit & INIT_ASKAVAILABLE) != 0);
		m_bLastAvailable = ((fwInit & INIT_LASTAVAILABLE) != 0);
		m_bDestroyOnHide = ((fwInit & INIT_NODESTROY) == 0);
		m_bAvailable = m_bAsk ? ((fwInit & INIT_AVAILABLE) != 0) : TRUE;
		m_bMDIBound = ((fwInit & INIT_MDI) != 0);
		m_bHost=((fwInit & INIT_HOST) != 0);

		if ((fwInit & INIT_DOCKSERIAL) != 0)
		{
			m_pDockSerial = new DOCKPLACEMENT;
			ar.Read(m_pDockSerial, sizeof(DOCKPLACEMENT));
		}

		if ((fwInit & INIT_DOCKPLACE) != 0)
		{
			m_pDockPlace = new DOCKPLACEMENT;
			ar.Read(m_pDockPlace, sizeof(DOCKPLACEMENT));
		}

		if ((fwInit & INIT_DESKPLACE) != 0)
		{
			m_pDeskPlace = new DESKPLACEMENT;
			ar.Read(m_pDeskPlace, sizeof(DESKPLACEMENT));
		}

#ifdef _DEBUG
		DebugCheckDockID(m_nIDWnd, m_dtWnd);
#endif
	}
}

//	CDockWorker::CDockWorker
//		Construction.  Initialized with DOCKINIT, and not from disk.

CDockWorker::CDockWorker(DOCKINIT FAR* lpdi)
{
	ASSERT(lpdi != NULL);
	ASSERT(HIWORD(lpdi->nIDPackage) == 0);
	ASSERT(HIWORD(lpdi->nIDWnd) == 0);

	m_strTitle = lpdi->szCaption;

    if (!(m_bPosition = (lpdi->fInit & INIT_POSITION) != 0))
		m_rectFloat = lpdi->rectFloat;
	if ((lpdi->fInit & INIT_STRETCHSIZE) != 0)
		m_sizeLastStretch = lpdi->sizeStretch;
	else
	{
		m_sizeLastStretch.cx = DEFAULT_STRETCH_WIDTH;
		m_sizeLastStretch.cy = DEFAULT_STRETCH_HEIGHT;
	}

    m_bCanHorz = (lpdi->fInit & INIT_CANHORZ) != 0;
    m_bCanVert = (lpdi->fInit & INIT_CANVERT) != 0;
	m_bMDIBound = (lpdi->fInit & INIT_MDI) != 0;
    m_bVisible = (lpdi->fInit & INIT_VISIBLE) != 0;
	m_bTranspose = (lpdi->fInit & INIT_TRANSPOSE_SIZE) != 0;
	m_bAsk = (lpdi->fInit & INIT_ASKAVAILABLE) != 0;
	// INIT_AVAILABLE  was previously unimplemented for inital usage, for some reason
	m_bAvailable = (!m_bAsk) || ((lpdi->fInit & INIT_AVAILABLE) != 0);
	m_bLastAvailable = (lpdi->fInit & INIT_LASTAVAILABLE) != 0;
	m_bDestroyOnHide = (lpdi->fInit & INIT_NODESTROY) == 0;
    m_bLoading = FALSE;
    m_dtWnd = lpdi->dt;
    m_dp = lpdi->dpInit;
    m_nIDPackage = lpdi->nIDPackage;
	m_nIDPacket = lpdi->nIDPacket;
    m_nIDWnd = MAKEDOCKID(lpdi->nIDPackage, lpdi->nIDWnd);
    m_pWnd = NULL;
	m_hglobInit = NULL;
	m_pDeskPlace = NULL;
	m_pDockPlace = NULL;
	m_pDockSerial = NULL;
	m_bHost=FALSE;
	m_nIDAssociate=0;

#ifdef _DEBUG
	DebugCheckDockID(m_nIDWnd, m_dtWnd);
#endif
}

//	CDockWorker::Reset
//		If a Package registers a DockWorker that we have not yet
//		created, we should trust it for some of the data.

void CDockWorker::Reset(DOCKINIT FAR* lpdi)
{
	ASSERT(lpdi != NULL);

	m_strTitle = lpdi->szCaption;
    m_bCanHorz = (lpdi->fInit & INIT_CANHORZ) != 0;
    m_bCanVert = (lpdi->fInit & INIT_CANVERT) != 0;
	m_bAsk = (lpdi->fInit & INIT_ASKAVAILABLE) != 0;
	m_bLastAvailable = (lpdi->fInit & INIT_LASTAVAILABLE) != 0;
	m_bDestroyOnHide = (lpdi->fInit & INIT_NODESTROY) == 0;
    m_dtWnd = lpdi->dt;
    m_nIDPackage = lpdi->nIDPackage;
	m_nIDPacket = lpdi->nIDPacket;
    m_nIDWnd = MAKEDOCKID(lpdi->nIDPackage, lpdi->nIDWnd);

	if (m_hglobInit != NULL)
	{
		::GlobalFree(m_hglobInit);
		m_hglobInit = NULL;
	}
}

//	CDockWorker::Reset
//		Used during CDockManager::Create to update workers in the new
//		manager with information from the old.

void CDockWorker::Reset(CDockWorker* pDockerOld)
{
	ASSERT_VALID(pDockerOld);

	// MDI state is transfered across managers, but if the state is changing
	// we do not want to take dock position information to be taken literally
	// during creation.  A lot may have changed since we last had valid info
	// for the dock.
	//
	DOCKPOS dp = pDockerOld->GetDock();

	// If we were embedded, we still are.
	if(dp==dpEmbedded)
	{
		m_dp=dpEmbedded;
	}

	if (m_bMDIBound != pDockerOld->m_bMDIBound)
	{
		m_bLoading = FALSE;
		m_bMDIBound = pDockerOld->m_bMDIBound;
	}

	// MDI windows and embedded bars do not change visibility during a layout switch
	if (m_bMDIBound || dp==dpEmbedded)
		m_bVisible = pDockerOld->m_bVisible;

	if (dp == dpNil || (dp == dpMDI && pDockerOld->IsVisible()))
	{
		// Note: Doing a DeparentWindow here for docked windows
		//       would be expensive.

		if (dp == GetDock())
			pDockerOld->DeparentWindow(NULL, m_pManager);
		else
			pDockerOld->DeparentWindow(m_pManager->GetDock(dpHide));
	}

	m_strTitle = pDockerOld->m_strTitle;	// Don't change text across layouts.
//	m_sizeLastStretch = pDockerOld->m_sizeLastStretch;
	m_rectFloat = pDockerOld->m_rectFloat;

	ASSERT(m_pWnd == NULL);	// Otherwise we leak.
	m_pWnd = pDockerOld->m_pWnd;
	pDockerOld->m_pWnd = NULL;
	ASSERT(m_hglobInit == NULL);	// Otherwise we leak.
	m_hglobInit = pDockerOld->m_hglobInit;
	pDockerOld->m_hglobInit = NULL;

	delete m_pDeskPlace;
	m_pDeskPlace = pDockerOld->m_pDeskPlace;
	pDockerOld->m_pDeskPlace = NULL;

	if (CDockWorker::s_pActiveDocker == pDockerOld)
		CDockWorker::s_pActiveDocker = this;
}

//	CDockWorker::PreTransfer
//		Called for workers during a manager change when a worker with
//		the same ID does not exist in new manager.

void CDockWorker::PreTransfer(CDockManager* pManagerNew)
{
	DOCKPOS dp = GetDock();
	if (dp == dpNil || (dp == dpMDI && IsVisible()))
		DeparentWindow(NULL, pManagerNew);

	// Dock placement info is manager specific.
	delete m_pDockPlace;
	m_pDockPlace = NULL;
	delete m_pDockSerial;
	m_pDockSerial = NULL;

	// Default for new dockables in full screen mode is to be hidden.
	if (DkGetDockState() & MASK_FULLSCREEN)
		m_bVisible = FALSE;
}

//	CDockWorker::Create
//		Makes sure we have a window for the dock worker, and correctly places
//		it within the docks.

BOOL CDockWorker::Create(CDockManager* pManager, CWnd* pOwner /*=NULL*/,
	BOOL bForceVisible /*=FALSE*/)
{
	ASSERT(pManager != NULL);
    m_pManager = pManager;
    m_bVisible = m_bVisible || bForceVisible;

	// If we are already have window in a floating or MDI window,
	// then we are done.
	CWnd* pParent;
	if (m_pWnd != NULL && (pParent = m_pWnd->GetParent()) != NULL)
	{
		if(	m_dp==dpEmbedded)
		{
			return TRUE;
		}

		if (pParent->IsKindOf(RUNTIME_CLASS(CPaletteWnd)))
			return TRUE;

		if (pParent->IsKindOf(RUNTIME_CLASS(CMDIChildDock)))
		{
			// MDI windows don't get hidden.
			if (!IsVisible())
				DeparentWindow();
			return TRUE;
		}
	}

    if (m_pWnd == NULL)
    	m_pWnd = pOwner;

    if (m_pWnd == NULL)
    {
    	ASSERT(m_nIDWnd);
    	HWND hwnd = GetPackageWindow();

    	if (hwnd == NULL)
    	{
    		// Window gets hidden status, so it won't get messages.
			m_bVisible = FALSE;

			if (m_bLoading && !m_bMDIBound && m_dp != dpNil)
			{
				// Get rid of absolute dock placement information.  It is
				// only valid for the first creat call.
				//
				delete m_pDockSerial;
				m_pDockSerial = NULL;
			}

    		return FALSE;
    	}

    	m_pWnd = CWnd::FromHandlePermanent(hwnd);
    	ASSERT(m_pWnd != NULL);	// Must be a CWnd.
    }

    ASSERT((UINT) LOWORD(m_nIDWnd) == (UINT) m_pWnd->GetDlgCtrlID());

    CDockWnd* pDock = m_pManager->GetDock(dpHide);
    if (pDock == NULL || m_pWnd->SetParent(pDock) == NULL)
    	return FALSE;

    // Try to follow the init request for this window, but don't worry
    // about failure; we still have a valid worker.
	if (m_bMDIBound)
	{
		if (IsVisible())
			DeskWnd();
	}
	else if (m_dp == dpEmbedded)
	{
        CRect rect;
        GetFloatRect(rect);
		CWnd *pParent=m_pWnd->GetParent();

		FloatWnd(rect);
		DeparentWindow(pParent, NULL);
	}
    else if (m_dp == dpNil)
    {
        CRect rect;
        GetFloatRect(rect);
        FloatWnd(rect);
    }
    else if (IsVisible())
    {
    	if ((m_pWnd->GetStyle() & WS_VISIBLE) == 0)
    		m_pWnd->ShowWindow(SW_SHOWNOACTIVATE);

		DockWnd(NULL, !m_bLoading);
    }

	EndLoad();

	// if we already have focus, then set the active docker)
	if(CWnd::GetFocus()==m_pWnd && m_pWnd!=NULL)
	{
		s_pActiveDocker=this;
	}

    return TRUE;
}

CDockWorker::~CDockWorker()
{
	// Destroy the window if one exists.
	if (m_pWnd != NULL)
		DestroyWindow(FALSE);

	// Free init data.
	if (m_hglobInit != NULL)
		::GlobalFree(m_hglobInit);

	delete m_pDeskPlace;	// Okay to delete NULL.
	delete m_pDockPlace;
	delete m_pDockSerial;
}

HWND CDockWorker::GetPackageWindow()
{
	if (m_pWnd != NULL)
		return m_pWnd->m_hWnd;

	HWND hwnd = NULL;

	CPackage* pGrp = theApp.GetPackage(m_nIDPackage);
	if (pGrp == NULL)
		return NULL;

	switch (m_dtWnd)
	{
	case dtHwnd:
		hwnd = pGrp->GetDockableWindow(LOWORD(m_nIDWnd), m_hglobInit);
		break;

	case dtEmbeddedToolbar:
	case dtToolbar:
		//BLOCK:
		{
			// This code is now complicated by the possibility that we might try to load a 
			// toolbar from disk and fail. Because we are changing the data format, this is
			// a likely contingency, at least until we write (hypothetical) toolbar migration
			// code. 
			// For this reason, if a loaded toolbar fails, we'll try to construct a default
			// toolbar instead. martynl 27Mar96
			BOOL bFinished=FALSE;
			BOOL bLoadFailed=FALSE;

			while(!bFinished)
			{
				TOOLBARINIT tbi;
				HGLOBAL hglob = NULL;
				// if we've failed a load, then hide the hglobinit
				HGLOBAL hglobInitMasked=bLoadFailed ? NULL : m_hglobInit;

				if (hglobInitMasked == NULL)
				{
					hglob = pGrp->GetToolbarData(LOWORD(m_nIDWnd));
					if(hglob==NULL)
					{
						bFinished=TRUE;
					}
				}

				if (hglob != NULL || hglobInitMasked != NULL)
				{
					TOOLBARINIT FAR* lptbi = &tbi;
					UINT FAR* lpIDArray = NULL;

					if (hglob != NULL) {
						lptbi = (TOOLBARINIT FAR*) ::GlobalLock(hglob);
						lpIDArray = (UINT FAR*) (lptbi + 1);
					}
					
					/* Toolbars are now storing their full m_nIDWnd, to allow them to know their 
					own package id. 
					To avoid confusing the packages, CPackage::GetToolbarData (called above)
					is still sent a window id stripped of its package id (above). However, we 
					modify the hglob that it returns so that the TOOLBARINIT structure contains
					the	full m_nIDWnd. So the lptbi->nIDWnd field is initialised the same way,
					whether or not there is an available hglob. martynl 05Mar96
					*/
					lptbi->nIDWnd = m_nIDWnd;

					CASBar* pBar = new CASBar;
					if (!pBar->Create(NULL, m_pManager, WS_BORDER, lptbi,
							lpIDArray, hglobInitMasked))
					{
						// Don't delete pBar here, because Create is a self-delete-on-fail function
						pBar=NULL;

						// if load already failed, then now we have to give up
						if(bLoadFailed)
						{
							return NULL;
						}
						else
						{
							bLoadFailed=TRUE;
						}
					}
					else
					{
						bFinished=TRUE;
					}

					hwnd = pBar->GetSafeHwnd();

					if (hglob != NULL)
					{
						::GlobalUnlock(hglob);
						::GlobalFree(hglob);
					}
				}
			}
		}
		break;

	case dtBorder:
	case dtEdit:
		hwnd = pGrp->GetDockableWindow(LOWORD(m_nIDWnd), m_hglobInit);

		if (hwnd != NULL)
		{
			CBorderBar* pBar = new CBorderBar;
			DWORD dwStyle = 0xF & ::GetWindowLong((HWND) hwnd, GWL_STYLE);
			if (!pBar->Create(NULL, m_pManager, dwStyle, hwnd, m_nIDWnd))
			{
				delete pBar;
				return NULL;
			}

			hwnd = pBar->GetSafeHwnd();
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	// If window was created successfully, then get rid of init data.
	if (hwnd != NULL && m_hglobInit != NULL)
	{
		::GlobalFree(m_hglobInit);
		m_hglobInit = NULL;
	}

	return hwnd;
}

static char BASED_CODE szWorkerKey[] = "Window%x";

void CDockWorker::LoadData()
{
	ASSERT(s_lpszLayoutSection != NULL);

	CString strKey;
	wsprintf(strKey.GetBuffer(sizeof(szWorkerKey) + 10), szWorkerKey, m_nIDWnd);
	strKey.ReleaseBuffer();

	m_hglobInit = GetRegData(s_lpszLayoutSection, strKey, NULL);
}

void CDockWorker::SaveData()
{
	ASSERT(s_lpszLayoutSection != NULL);

	if (m_pWnd != NULL)
	{
		ASSERT(m_hglobInit == NULL);	// Otherwise we leak.
		m_hglobInit = (HGLOBAL) m_pWnd->SendMessage(DWM_GETDATA);
	}

	CString strKey;
	wsprintf(strKey.GetBuffer(sizeof(szWorkerKey) + 10), szWorkerKey, m_nIDWnd);
	strKey.ReleaseBuffer();

	if (m_hglobInit != NULL)
	{
		WriteRegData(s_lpszLayoutSection, strKey, m_hglobInit);
	}
	else
	{
		// erase any previous saved version of this window
		DeleteRegData(s_lpszLayoutSection, strKey);
	}

	// If we have a window, this data is only useful for the save.
	if (m_pWnd != NULL && m_hglobInit != NULL)
	{
		::GlobalFree(m_hglobInit);
		m_hglobInit = NULL;
	}
}

//  CDockWorker::OnMessage
//      Distibution point for DMM_*'s.

LONG CDockWorker::OnMessage(UINT message, LPARAM lParam)
{
	// Can't use a switch, because these are not constants.
	if (message == DMM_CHILDSETFOCUS)
	{
		OnChildSetFocus(CWnd::FromHandle((HWND) lParam));
    	return 0L;
	}
	else if (message == DMM_CHILDKILLFOCUS)
	{
		OnChildKillFocus(CWnd::FromHandle((HWND) lParam));
    	return 0L;
	}
    if (message == DMM_NCLBUTTONDOWN)
	{
		HGLOBAL hglob = (HGLOBAL) lParam;
		MOUSESTRUCT FAR* pmst = (MOUSESTRUCT FAR*) ::GlobalLock(hglob);
		LONG lResult = OnNcLButtonDown(pmst->nHitTest, pmst->pt);
		::GlobalUnlock(hglob);

		return lResult;
    }
    else if (message == DMM_NCLBUTTONDBLCLK)
	{
		HGLOBAL hglob = (HGLOBAL) lParam;
		MOUSESTRUCT FAR* pmst = (MOUSESTRUCT FAR*) ::GlobalLock(hglob);
		LONG lResult = OnNcLButtonDblClk(pmst->nHitTest, pmst->pt);
		::GlobalUnlock(hglob);

		return lResult;
    }
	else
	{
        ASSERT(FALSE);  // No other messages.
    }

    return 0L;
}

void CDockWorker::OnChildSetFocus(CWnd* pWndLoseFocus)
{
	if (s_pActiveDocker!=this ||			// activation may have happened early
		(m_pWnd != pWndLoseFocus && !m_pWnd->IsChild(pWndLoseFocus)))
		OnWindowActivate(TRUE, pWndLoseFocus);
}

void CDockWorker::OnChildKillFocus(CWnd* pWndGetFocus)
{
	if (m_pWnd != pWndGetFocus && !m_pWnd->IsChild(pWndGetFocus))
		OnWindowActivate(FALSE, pWndGetFocus);
}

// return true if window is of the type that should lock the doc window
BOOL CDockWorker::FWndLockWorker(CWnd* pWndOther)
{
	BOOL fRet = FALSE;

	if (pWndOther != NULL)
		{
		if (pWndOther == GetPropertyBrowser())
			fRet = TRUE;
		else if (pWndOther->IsKindOf(RUNTIME_CLASS(CGoToDialog)))
			fRet = TRUE;
		}
	return fRet;
}

void CDockWorker::OnWindowActivate(BOOL bActive, CWnd* pOtherWnd)
{
	ASSERT(m_pWnd != NULL);

	CMDIChildWnd* pFrame;
	CPartView* pView = NULL;

	// Only dtEdit types have Packets.
	if (m_dtWnd != dtEdit)
		return;
	else if	(CDockWorker::s_bLockDocker)
	{
		if (bActive)
			CDockWorker::s_bLockDocker = FALSE;

		if (s_pActiveDocker == this)
			return;

		// Warning:  If this happens unexplained, it should be considered, a bug.
		// Known cases when this happens:
		//		(1)	Clicking on floating dockable to activate the app,
		//			after deactivation with another dockable window focused.
		//
		ASSERT(bActive);
		TRACE2("CDockWorker::OnWindowActivate '%s' with locked worker '%s'.\n",
			(const char*) m_strTitle, s_pActiveDocker == NULL ?
			"" : (const char*) s_pActiveDocker->m_strTitle);

		if (s_pActiveDocker != NULL)
			s_pActiveDocker->OnWindowActivate(!bActive, m_pWnd);
	}
	else
	{
		// Handle the view activation.  This is pretty ugly, because
		// we have to take into account times when the view of a CMDIChildWnd
		// should be getting the message, but the MDI wnd will not get a
		// WM_MDIACTIVATE.
		//
		CWnd *pParent;
		while (pOtherWnd != NULL && !FWndLockWorker(pOtherWnd))
		{
			pParent = pOtherWnd->GetParent();
			if (pParent != NULL &&
				(pParent->IsKindOf(RUNTIME_CLASS(CDockWnd)) ||
				pParent->IsKindOf(RUNTIME_CLASS(CPaletteWnd))))
				break;

			pOtherWnd = pParent;
		}

		CDockWorker* pDocker = m_pManager->WorkerFromWindow(pOtherWnd);

		// If the OtherWnd is in a docking view, then it will be handled
		// in its own WindowActivate call.
		//
		if (pDocker == NULL || pDocker->m_dtWnd != dtEdit)
		{
			// Otherwise if the OtherWnd is not associated with a
			// dockable window, or we are being activated, then we
			// will need to notify the active MDI child.
			//
			if (bActive || pOtherWnd == NULL ||
				(pDocker == NULL && !FWndLockWorker(pOtherWnd)))
			{
				pFrame = ((CMDIFrameWnd*) theApp.m_pMainWnd)->MDIGetActive();
				if (pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)))
					pView = (CPartView*) pFrame->GetActiveView();
				ASSERT(pView == NULL || pView->IsKindOf(RUNTIME_CLASS(CPartView)));
			}
			// This window is being deactivated to give focus to another
			// dockable window (probably the Find Combo), so lock this as the
			// active worker.  (To allow us to find in dtEdit windows.)
			// Also lock for property page.
			else
			{
				LockWorker();
				return;
			}
		}
	}

	CPartView* pViewDock = GetView();
	ASSERT_VALID(pViewDock);

	if (bActive)
	{
		s_pActiveDocker = this;

		theApp.ActivatePacket(pViewDock->GetPacket());

		if (pView != NULL)
			pView->OnActivateView(FALSE, pView, pView);
	}
	else
	{
		s_pActiveDocker = NULL;

		// If there is no MDI child, we just deactivate the packet,
		// otherwise we update the menu to the new MDI child.
		//
		if (pView == NULL)
		{
			theApp.DeactivatePacket();
			theApp.m_theAppSlob.SetSlobProp(P_CurrentSlob, NULL);
		}
		else
		{
			ASSERT(pFrame != NULL);
			pFrame->OnUpdateFrameMenu(TRUE, pFrame, NULL);

			// Force update of available windows. This fixes a problem
			// when there is a switch from the Dialog editor (MDI) to the output
			// window (docking) to a regular text window (MDI): the last switch
			// would not update the toolbars and avail wnds because the packet
			// for the output window is the same as the packet for the MDI text Wnds.
			DkUpdateAvailableWnds(FALSE);
		}

		// Focus change will cause the necessary OnActivateView() for
		// MDI windows.
	}

	// s_pActiveDocker must be reset before we call OnActivateView for the
	// docking view, so that theApp.GetActiveView() can be used to test
	// active state.
	//
	pViewDock->OnActivateView(bActive, pViewDock, pViewDock);
}

//	CDockWorker::OnNcLButtonDown
//		Distribution point for the NcLButtonDown handling, to either size
//		or move, and returns FALSE on the HT's we don't handle.

BOOL CDockWorker::OnNcLButtonDown(UINT nHitTest, CPoint pt)
{
	switch(nHitTest)
	{
	case HTCAPTION:
		DoMove(pt);
		return TRUE;

	case HTLEFT:
	case HTTOP:
	case HTTOPLEFT:
	case HTTOPRIGHT:
	case HTRIGHT:
	case HTBOTTOM:
	case HTBOTTOMLEFT:
	case HTBOTTOMRIGHT:
		DoSize(pt, nHitTest);
		return TRUE;
	}

	return FALSE;
}

//	CDockWorker::OnNcLButtonDblClk
//		Toggles float on a HTCAPTION, and returns FALSE for other HT's.

BOOL CDockWorker::OnNcLButtonDblClk(UINT nHitTest, CPoint pt)
{
	if (nHitTest == HTCAPTION)
	{
		ToggleFloat();
		return TRUE;
	}

	return FALSE;
}

//  CDockWorker::ToggleFloat
//      Handles switching a dockable window between floating and docked
//      states.

void CDockWorker::ToggleFloat()
{
	ASSERT(m_pWnd != NULL);

    if (m_dp != dpNil)
    {
    	CRect rect;
    	GetFloatRect(rect);
        FloatWnd(rect);
    }
    else
    {
		DockWnd();
    }
}

//  CDockWorker::FloatWnd
//      Given a rect, creates a new palette around the rect, and sets this
//      as the parent of the dock worker's window.  Also handles removing
//      the window from its current DockWnd.

BOOL CDockWorker::FloatWnd(const CRect& rect)
{
	ASSERT(m_pWnd != NULL);

	DeparentWindow();

    m_dp = dpNil;

    CFrameWnd* pFrame = m_pWnd->GetParentFrame();
    ASSERT(pFrame != NULL);

	CRect rectCreate = rect;	// Make sure titlebar will be visible.
	CPaletteWnd::GetPaletteRect(rectCreate);
	//[multimon] 11 mar 97 - der : ForceRectBelowTop modified for MultiMon.
	::ForceRectBelowTop(rectCreate);

	CPaletteWnd::GetChildRect(rectCreate);

    CPaletteWnd* pPalette = new CPaletteWnd(m_pManager);

	BOOL bHasClose=(LOWORD(m_nIDWnd) != IDTB_MENUBAR) || IS_STATE_FULLSCREEN(DkGetDockState());

    if (!pPalette->Create(NULL, m_strTitle, m_pWnd, pFrame, rectCreate, m_nIDWnd, bHasClose))
    {
        delete pPalette;
        return FALSE;
    }

	// SetParent does not change ActiveWindow, so we need to make sure we
	// maintain Active->IsChild(Focus)
	//
	CWnd* pWndFocus = CWnd::GetFocus();
	if (pWndFocus != NULL && pPalette->IsChild(pWndFocus) &&
		pPalette != CWnd::GetActiveWindow())
	{
		pPalette->SetActiveWindow();
	}

    if (IsVisible() && m_pManager->IsVisible())
    {
    	pPalette->ShowWindow(SW_SHOWNOACTIVATE);
    	pPalette->UpdateWindow();
    }

    return TRUE;
}

BOOL CDockWorker::DockWnd(DOCKPLACEMENT* pDockPlace /*=NULL*/,
	BOOL bRelative /*=TRUE*/)
{
	ASSERT(m_pWnd != NULL);

	// This is a workaround for a dud-registry loading problem. Where the dud registry data comes
	// from is another question...
	if(!bRelative)
	{
		if(m_dp==dpNil || m_pDockSerial==NULL)
		{
			bRelative=TRUE;
		}
	}

	ASSERT(bRelative || (m_dp != dpNil && m_pDockSerial != NULL));

	BOOL bFloating = (m_dp == dpNil);

	DOCKPOS dpTmp = m_dp;
	if (bRelative && pDockPlace != NULL)
		dpTmp = pDockPlace->dp;
	if (dpTmp == dpNil)
	{
		if (m_pDockPlace != NULL)
			dpTmp = m_pDockPlace->dp;
		else if (m_bCanHorz)
	    	dpTmp = dpTop;
	    else if (m_bCanVert)
	    	dpTmp = dpLeft;
	    else
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}
	ASSERT(dpTmp != dpNil);

    CDockWnd* pDock = m_pManager->GetDock(dpTmp);
    if (pDock == NULL)
		return FALSE;

	CSize size;
	DOCKPOS dpInitSize = (dpTmp == dpLeft || dpTmp == dpRight) ?
		dpVertInit : dpHorzInit;
	if (m_bTranspose)
		dpInitSize = (dpInitSize == dpVertInit) ? dpHorzInit : dpVertInit;

	if (bRelative)
	{
		// Hack: set m_dp so we will get the focus back when the frame
		// gets the WM_ACTIVATE caused by RemovePalette.
		//
		m_dp = dpTmp;

		if (bFloating)
		{
			// Can't use DeparentWindow because we have already changed m_dp.
	        SaveFloatPos();
	        CPaletteWnd* pPalette = (CPaletteWnd*) m_pWnd->GetParent();
			ASSERT(pPalette != NULL &&
				pPalette->IsKindOf(RUNTIME_CLASS(CPaletteWnd)));
	        m_pManager->RemovePalette(pPalette);
		}

		if (pDockPlace != NULL)
			pDock->AddWnd(m_pWnd, &pDockPlace->rectAbsolute);
		else if (m_pDockPlace != NULL)
		{
			// Make sure the size is current, and add the window.
			size.cx = m_pDockPlace->drRelative.cAlong;
			size.cy = m_pDockPlace->drRelative.cAcross;
			DockTranspose(m_dp, &size);
			size = GetMoveSize(dpInitSize, size);
			DockTranspose(m_dp, &size);
			m_pDockPlace->drRelative.cAlong = size.cx;
			m_pDockPlace->drRelative.cAcross = size.cy;

			pDock->AddWnd(m_pWnd, &m_pDockPlace->drRelative);
		}
		else
		{
		    size = GetMoveSize(dpInitSize, GetFloatSize());
	    	pDock->AddWnd(m_pWnd, size);
		}

		// SetParent does not change ActiveWindow, so we need to make sure we
		// maintain Active->IsChild(Focus)
		//
		CWnd* pWndFocus = CWnd::GetFocus();
		CWnd* pFrame = AfxGetApp()->m_pMainWnd;
		if (pWndFocus != NULL && pFrame->IsChild(pWndFocus) &&
			 pFrame != CWnd::GetActiveWindow())
		{
			pFrame->SetActiveWindow();
		}

	}
	else
	{
		ASSERT(m_dp == dpTmp);

		CRect rect = m_pDockSerial->rectAbsolute;
		size = GetMoveSize(dpInitSize, rect.Size());

		m_pWnd->SetParent(pDock);
		m_pWnd->SetWindowPos(NULL,
			rect.left, rect.top, size.cx, size.cy,
			SWP_NOACTIVATE | SWP_NOZORDER);

		delete m_pDockSerial;
		m_pDockSerial = NULL;
	}

	delete m_pDockPlace;
	m_pDockPlace = NULL;

	return TRUE;
}

//	CDockWorker::ToggleMDIState
//		Toggles the current MDI state, insuring that the docked view
//		remains active.

void CDockWorker::ToggleMDIState()
{
	HWND hwndSaveActiveView = theApp.GetActiveView();
	SetMDIState(!m_bMDIBound);
	if (hwndSaveActiveView != theApp.GetActiveView())
		theApp.SetActiveView(hwndSaveActiveView);
}

//	CDockWorker::SetMDIState
//		This is the interface for toggling windows between MDI and docking
//		views.

void CDockWorker::SetMDIState(BOOL bMDIBound)
{
	if (m_dtWnd != dtEdit ||
		(bMDIBound && m_bMDIBound) ||
		(!bMDIBound && !m_bMDIBound))
		return;

	if (!IsVisible())
	{
		m_bMDIBound = bMDIBound;
		return;
	}

	if (bMDIBound)
	{
		if (!DeskWnd())
			return;
		m_bMDIBound = TRUE;
	}
	else
	{
		DeparentWindow();
		m_bMDIBound = FALSE;
		Create(m_pManager);
	}
}

//	CDockWorker::DeskWnd
//      Given a rect, creates a new MDI child, and sets this
//      as the parent of the dock worker's window.  Also handles removing
//      the window from its current Parent.

BOOL CDockWorker::DeskWnd(const CRect& rect /*=CFrameWnd::rectDefault*/)
{
	ASSERT(m_pWnd != NULL);

	DeparentWindow();

	m_bMDIBound = TRUE;

	ASSERT(m_dtWnd == dtEdit);
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*) AfxGetApp()->m_pMainWnd;
    ASSERT(pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));

    CMDIChildDock* pMDIWnd = new CMDIChildDock(m_pManager);

	CPackage* pGrp = theApp.GetPackage(m_nIDPackage);
	ASSERT(pGrp != NULL);

    if (!pMDIWnd->Create(AfxRegisterWndClass(0, NULL, NULL,
    										 pGrp->GetDockableIcon(LOWORD(m_nIDWnd))),
    					 m_strTitle, m_pWnd, pFrame, rect))
    {
        delete pMDIWnd;
        return FALSE;
    }

	// SetParent does not change ActiveWindow, so we need to make sure we
	// maintain Active->IsChild(Focus)
	//
	CWnd* pWndFocus = CWnd::GetFocus();
	if (pWndFocus != NULL && pFrame->IsChild(pWndFocus) &&
		 pFrame != CWnd::GetActiveWindow())
	{
		pFrame->SetActiveWindow();
	}

	if (m_pDeskPlace != NULL)
	{
		m_pDeskPlace->place.length = sizeof(WINDOWPLACEMENT);	// Fix for NT bug.
		m_pDeskPlace->place.showCmd = SW_HIDE;
		pMDIWnd->SetWindowPlacement(&m_pDeskPlace->place);
		delete m_pDeskPlace;
		m_pDeskPlace = NULL;
	}

	ASSERT_VALID(GetView()) ; // Get that its okay before we dereference.
	CPartTemplate* pTemplate = GetView()->GetDocTemplate();
	if (pTemplate == NULL)
		pMDIWnd->InitialUpdateFrame(NULL, TRUE);
	else
		pTemplate->InitialUpdateFrame(pMDIWnd, NULL);

	// Make sure this doesn't stay the active worker.  MDI windows do not
	// get worker activation.
	//
	if (s_pActiveDocker == this)
	{
		s_pActiveDocker = NULL;
		s_bLockDocker = FALSE;
	}

    return TRUE;
}

//	CDockWorker::DeparentWindow
//		Removes the dockable window from its current parent, setting the
//		parent to the hidden dock, and destroying the old parent if
//		necessary.

void CDockWorker::DeparentWindow(CWnd* pWndNewParent /*=NULL*/,
	CDockManager* pManagerNew /*=NULL*/)
{
	if (m_pWnd == NULL)
		return;

	if (pWndNewParent == NULL)
		pWndNewParent = m_pManager->GetDock(dpHide);

	CWnd* pWndParent = m_pWnd->GetParent();
	if (pWndParent == pWndNewParent)
		return;

	ASSERT_VALID(pWndParent);

	if (pWndParent->IsKindOf(RUNTIME_CLASS(CMDIChildDock)))
	{
		ASSERT(m_dtWnd == dtEdit);

		if (pManagerNew == NULL)
			SaveMDIPos();

        m_pManager->RemoveMDIChild((CMDIChildDock*) pWndParent,
			pWndNewParent, pManagerNew);
	}
    else if (pWndParent->IsKindOf(RUNTIME_CLASS(CPaletteWnd)))
	{
		if (pManagerNew == NULL)
			SaveFloatPos();

        m_pManager->RemovePalette((CPaletteWnd*) pWndParent,
			pWndNewParent, pManagerNew);
	}
	else if (pManagerNew == NULL)
    {
		// We don't do this if we are swtiching managers, because it would
		// really expensive with all the layout recalc'ing.

		if(m_dp!=dpEmbedded)
		{
			SaveDockPos();

			CDockWnd* pDock = m_pManager->GetDock(m_dp);
			ASSERT(pDock != NULL);	// The dock exists, so this should work.
			pDock->RemoveWnd(m_pWnd, pWndNewParent);
		}
    }
}

void CDockWorker::SetText(const CString& str)
{
	m_strTitle = str;

	if ((IsVisible() && m_bMDIBound) || (m_pWnd != NULL && m_dp == dpNil))
	{
		ASSERT(m_pWnd != NULL);
		m_pWnd->GetParent()->SetWindowText(str);
	}
}

//	CDockWorker::GetView
//		Gets the CPartView associated with a dtEdit docking view.

CPartView* CDockWorker::GetView() const
{
	if (!m_pWnd || m_dtWnd != dtEdit)
		return NULL;

	// Orion 96 Bug 15325 - GetDescendantWindow is finding an IV window with the id E900
	// which happens to be AFX_IDW_PANE_FIRST. Replaced with another function which 
	// returns the first window with an ID which is also a CPartView.
	CPartView* pView = GetFirstChildWindowWithID(m_pWnd->m_hWnd, AFX_IDW_PANE_FIRST);
	if (pView == NULL)
	{
		pView = GetFirstChildWindowWithID(m_pWnd->m_hWnd, LOWORD(m_nIDWnd));
	}
	ASSERT(pView != NULL && pView->IsKindOf(RUNTIME_CLASS(CPartView)));
	return pView;
}

//	CDockWorker::ActivateView
//		Activates a worker's view.

void CDockWorker::ActivateView()
{
	if (m_dtWnd != dtEdit)
		return;

	if (!IsVisible())
		ShowWindow(TRUE);
	else
	{
		CView* pView = GetView();
		if (pView->GetSafeHwnd() != NULL)
			theApp.SetActiveView(pView->GetSafeHwnd());
	}
}

//	CDockWorker::ShowWindow
//		Dockable window version of ShowWindow.  Takes care of hiding the
//		the palette if window is floating, or re-calc'ing layout if it
//		was docked.

void CDockWorker::ShowWindow(BOOL bShow)
{
	// This and creation should be the only places we handle visibility.
	BOOL bVisible = IsVisible();

	// if it's embedded, this is it's creation call
	if(m_dp==dpEmbedded && bShow && (m_pWnd==NULL || m_pWnd->IsWindowVisible()==FALSE))
	{
		bVisible=FALSE;
	}

	if ((bShow && bVisible) || (!bShow && !bVisible))
	{
		if(m_dp==dpEmbedded)
		{
			m_bVisible=bShow;
		}
		return;
	}

	BOOL bCustomize = (CASBar::s_pCustomizer != NULL &&
		CASBar::s_pCustomizer->m_pDialog != NULL);

	if (bShow)
	{
		// May need to create window.  Creation of hidden windows is defered.
		if (m_pWnd == NULL && !Create(m_pManager))
			return;

		// Make sure we refuse input in customize mode.
		if (bCustomize)
    		m_pWnd->SendMessage(DWM_ENABLECHILD, FALSE);
		else if (m_dtWnd == dtEdit)
		{
			CPartView* pView = GetView();
			ASSERT_VALID(pView);
			theApp.SetActiveView(pView->m_hWnd);
		}

		if (m_bMDIBound)
		{
			DeskWnd();
		}
		else if (m_dp != dpNil && m_dp != dpEmbedded)
		{
			DockWnd();
		}
		else
		{
			CPaletteWnd* pPalette = (CPaletteWnd*) m_pWnd->GetParent();
			if (!pPalette->IsKindOf(RUNTIME_CLASS(CPaletteWnd)))
			{
				CRect rect;
				GetFloatRect(rect);
				FloatWnd(rect);
				
				pPalette = (CPaletteWnd*) m_pWnd->GetParent();
			}

			ASSERT(pPalette->IsKindOf(RUNTIME_CLASS(CPaletteWnd)));

			// Make sure we appear on screen.
			CRect rectPalette;
			pPalette->GetWindowRect(rectPalette);
			ForceRectOnScreen(rectPalette);

			pPalette->ActivateNoFocus();
			pPalette->SetWindowPos(NULL, rectPalette.left, rectPalette.top,
				rectPalette.Width(), rectPalette.Height(),
				SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
			pPalette->UpdateWindow();
		}

		// Window owner may have asked to show a window before we reach
		// UpdateAvailable().  In order to avoid having this code execute
		// twice in this event, we assume m_bAvailable to be true.

		m_bAvailable = TRUE;

#ifdef _DEBUG
		if (m_bAsk)
		{
			// Make sure the window really is available.
			CPackage* pGrp = theApp.GetPackage(m_nIDPackage);
			ASSERT(pGrp == NULL || pGrp->AskAvailable(LOWORD(m_nIDWnd)));
		}
#endif
	}
	else
	{
		// Safeguard against nasty crashes, if by some quirk we get in
		// a state we should really never be in.  But let's be safe.
		//
		if (m_pWnd == NULL)
		{
			m_bVisible = FALSE;
			return;
		}

		ASSERT(m_pWnd->m_hWnd != NULL);

		if (m_bMDIBound)
		{
			if (!m_bDestroyOnHide)
				DeparentWindow();
		}
		else if (m_dp == dpEmbedded)
		{
			m_pWnd->ShowWindow(SW_HIDE);
		}
		else if (m_dp != dpNil)
		{
			DeparentWindow();
		}
		else
		{
			CWnd* pPalette = m_pWnd->GetParent();
			ASSERT(pPalette->IsKindOf(RUNTIME_CLASS(CPaletteWnd)));
			pPalette->ShowWindow(SW_HIDE);
			pPalette->UpdateWindow();
		}

		// If we had the focus, make sure we don't keep it.
		CWnd* pWnd = CWnd::GetFocus();
		if (m_pWnd == pWnd || m_pWnd->IsChild(pWnd))
			m_pManager->m_pFrame->SetFocus();

		if (s_pActiveDocker == this)
		{
			// Focus handling for this window is broken in some way, really.
			// The above SetFocus should have deactivated this window.

			// Apparently a longer explanation of this is in order, due to the
			// number of questions this generates.  Do not even think about
			// removing this assert; it has found a ton of bugs.

			// The above SetFocus() will cause Windows to send two important
			// messages:
			// (1) WM_KILLFOCUS to the dockable window child that has the focus.
			//     If you are calling DkPreHandleMessage for this child,
			//     s_pActiveDocker will be set to NULL in CDockWorker::OnWindowActivate.
			// (2) WM_SETFOCUS to the main frame.  If your window has no bugs,
			//     s_pActiveDocker will still be NULL, and the normal focus routing
			//     will occur.

			// Thus far 2 types of bugs have caused this assert to fire:
			// (1) The dockable child window with the focus does not call
			//     DkPreHandleMessage.
			// (2) Some hidden sub-function has the side-effect of setting the
			//     focus back to the dockable window of one of its children.
			//     To catch the culprit, set a breakpoint on CDockWorker::OnChildSetFocus,
			//     and watch how it gets called during the hide.

			ASSERT(FALSE);
			s_pActiveDocker = NULL;
		}

		// Destroy dtEdit windows, since they have more overhead, but
		// only destroy them when they have m_bDestroyOnHide set.
		if (m_dtWnd == dtEdit && m_bDestroyOnHide)
			DestroyWindow();
	}

	m_bVisible = (bShow != 0);	// Bit bool.

	// Are we in customize mode?
	if (bCustomize)
	{
		// If we have a CToolbar dialog, update the checklist.
		if (CASBar::s_pCustomizer->m_pDialog->
			IsKindOf(RUNTIME_CLASS(CToolbarDialog)))
		{
			((CToolbarDialog*) CASBar::s_pCustomizer->m_pDialog)->
				UpdateCheck(m_nIDWnd);
		}
	}

	// Notify the window of the change.  This is where windows that want to
	// unregister themselves when closed should call DkUnregister, since
	// unregistering will cause this worker to be deleted.
	//
	// Warning: Do not add code to this function below this call, or you may
	// crash accessing a deleted this pointer.

	if (m_pWnd != NULL)
		m_pWnd->SendMessage(DWM_SHOWWINDOW, m_bVisible);
}

void CDockWorker::DestroyWindow(BOOL bInitData /*=TRUE*/)
{
	if (m_pWnd == NULL)
		return;

	// cache it here in case it gets corrupted during destruction
	DOCKPOS dp=m_dp;

	if (bInitData)
	{
		ASSERT(m_hglobInit == NULL);	// Otherwise we leak.
		m_hglobInit = (HGLOBAL) m_pWnd->SendMessage(DWM_GETDATA);
	}

	// Make sure we don't stay the active worker.
	if (s_pActiveDocker == this)
	{
		s_bLockDocker = FALSE;
		s_pActiveDocker->OnWindowActivate(FALSE, m_pManager->m_pFrame);

		CWnd* pWndFocus = CWnd::GetFocus();
		if (m_pWnd == pWndFocus || m_pWnd->IsChild(pWndFocus))
			m_pManager->m_pFrame->SetFocus();
	}

	CWnd* pParent = m_pWnd->GetParent();
	if (pParent != NULL && !pParent->IsKindOf(RUNTIME_CLASS(CDockWnd)))
		DeparentWindow();

	if(dp!=dpEmbedded)
	{
		m_pWnd->DestroyWindow();	// Must be auto-deleting.
		m_pWnd = NULL;
	}
	else
	{
		// embeddee will do it.
	}
}

//	CDockWorker::UpdateAvailable
//		Updates the m_bAvailable bool, and hide/shows the window if appropriate.
//		The bool is used to allow us to hide unavailable windows in one loop,
//		end then show available ones after.

void CDockWorker::UpdateAvailable(BOOL bShow)
{
	if(m_dp==dpEmbedded)
	{
		// let the host do the talking
		return;
	}

	BOOL bAvailable = FALSE;
	if(m_bAsk)
	{
		CPackage* pGrp = theApp.GetPackage(m_nIDPackage);
		if (pGrp == NULL)
			return;

		bAvailable = m_pManager->IsVisible() &&
			pGrp->AskAvailable(LOWORD(m_nIDWnd));
	}
	else bAvailable = m_pManager->IsVisible();

	if(bAvailable && m_pManager->IsInDocObjectMode())
	{
		switch(m_nIDWnd)
		{
			case MAKEDOCKID(PACKAGE_PRJSYS, IDDW_PROJECT):
			case MAKEDOCKID(PACKAGE_PRJSYS, IDDW_GLOBAL):
			case MAKEDOCKID(PACKAGE_IV,IDDW_IV):
				bAvailable = TRUE;
				break;
			default:
				bAvailable = FALSE;
		}
	}
	BOOL bVisible = m_bVisible;	// May change in ShowWindow.

	if ((bShow && bAvailable) || (!bShow && !bAvailable))
	{
		ShowWindow(bAvailable && bVisible);

		if(bAvailable && bVisible && m_nIDWnd==MAKEDOCKID(PACKAGE_SUSHI, IDTB_MENUBAR))
		{
			CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();
			pFrame->RebuildMenus();
		}

		m_bAvailable = bAvailable;
		m_bVisible = bVisible;	// May have been changed.
	}
}

//	CDockWorker::DoSize
//		Snap sizing for floating windows.

void CDockWorker::DoSize(CPoint pt, UINT nHitTest)
{
	ASSERT(m_pWnd != NULL);
	ASSERT(m_dp == dpNil);

    CRect rect;

    if (!TrackSize(pt, nHitTest, rect))
        return;

	Move(m_dp, rect);
}

//	CDockWorker::TrackSize
//		Tracks cursor movement in snap sizing operation, displaying tracker.

BOOL CDockWorker::TrackSize(CPoint ptStart, UINT nHitTest, CRect& rectFinal)
{
	// No tracking when we aren't really visible.  Might end up
	// tracking for during OLE activation.
	if (m_dp != dpNil || !m_pManager->IsVisible())
		return FALSE;

    BOOL bCancel = FALSE;

    CRect rect, rectOld, rectWnd;
    CRect rectBound(g_mt.cxFrame, g_mt.cyFrame,
    	::GetSystemMetrics(SM_CXSCREEN) - g_mt.cxFrame,
    	::GetSystemMetrics(SM_CYSCREEN) - g_mt.cyFrame);
    CPoint pt, ptOld;

#if 1
    CLockDC dc(NULL);
#else
	CClientDC dc(NULL);
#endif

    MINMAXINFO mmi;
	mmi.ptMaxTrackSize = s_ptDefMax;
    mmi.ptMinTrackSize = CDockWorker::GetDefMin();

    m_pWnd->GetWindowRect(rectWnd);
    rect = rectWnd;

    m_pWnd->SendMessage(WM_GETMINMAXINFO, 0, (LPARAM) (LPVOID) &mmi);

    ptOld = pt = ptStart;

    // Loop while the mouse is down.
    CWnd* pWndTrack = CWnd::GetFocus();	// So we stay the active window.
    if (pWndTrack == NULL)
	{
		// Try to get a valid focused window.
		m_pManager->m_pFrame->SetFocus();
		if ((pWndTrack = CWnd::GetFocus()) == NULL)
			return FALSE;
	}


    pWndTrack->SetCapture();

    // Turn on the drag rectangle.
    rectOld = rect;
    DrawTrackRect(&dc, m_dp, &rect);

    while (!PollForMouseChange(pt, FALSE, bCancel))
    {
        if (pt == ptOld)
        	continue;

        switch (nHitTest)
        {
    	case HTTOP:
    	case HTTOPLEFT:
    	case HTTOPRIGHT:
    		rect.top = rectWnd.top + pt.y - ptStart.y;
    		rect.top = rect.bottom -
    			min(max(rect.Height(), mmi.ptMinTrackSize.y),
    				mmi.ptMaxTrackSize.y);
    		break;

    	case HTBOTTOM:
    	case HTBOTTOMLEFT:
    	case HTBOTTOMRIGHT:
    		rect.bottom = rectWnd.bottom + pt.y - ptStart.y;
    		rect.bottom = rect.top +
    			min(max(rect.Height(), mmi.ptMinTrackSize.y),
    				mmi.ptMaxTrackSize.y);
    		break;
        }

        switch (nHitTest)
        {
    	case HTLEFT:
    	case HTTOPLEFT:
    	case HTBOTTOMLEFT:
    		rect.left = rectWnd.left + pt.x - ptStart.x;
    		rect.left = rect.right -
    			min(max(rect.Width(), mmi.ptMinTrackSize.x),
    				mmi.ptMaxTrackSize.x);
    		break;

    	case HTRIGHT:
    	case HTTOPRIGHT:
    	case HTBOTTOMRIGHT:
    		rect.right = rectWnd.right + pt.x - ptStart.x;
    		rect.right = rect.left +
    			min(max(rect.Width(), mmi.ptMinTrackSize.x),
    				mmi.ptMaxTrackSize.x);
    		break;
		}

		// Give the window a chance to update this rectangle
		// for snapping drag.
		if (m_pWnd->GetStyle() & (DWS_SIZABLE | DWS_8SIZABLE))
		{
			CSize size = GetTrackSize(nHitTest, rect.Size());

#ifdef _DEBUG
			if (size.cx > mmi.ptMaxTrackSize.x ||
				size.cy > mmi.ptMaxTrackSize.y ||
				size.cx < mmi.ptMinTrackSize.x ||
				size.cy < mmi.ptMinTrackSize.y)
			{
				TRACE0("Warning: Returned track size outside min/max bounds.\n	Using last rect.\n");
			}
#endif

			size.cx = min(max(size.cx, mmi.ptMinTrackSize.x),
				mmi.ptMaxTrackSize.x);
			size.cy = min(max(size.cy, mmi.ptMinTrackSize.y),
				mmi.ptMaxTrackSize.y);

			// Adjust, anchored to a point that is not changing.
			switch (nHitTest)
			{
	    	case HTTOP:
	    	case HTTOPLEFT:
			case HTLEFT:
				rect.left = rect.right - size.cx;
				rect.top = rect.bottom - size.cy;
				break;

	    	case HTBOTTOM:
	    	case HTBOTTOMRIGHT:
			case HTRIGHT:
				rect.right = rect.left + size.cx;
				rect.bottom = rect.top + size.cy;
				break;

	    	case HTTOPRIGHT:
				rect.right = rect.left + size.cx;
				rect.top = rect.bottom - size.cy;
				break;

	    	case HTBOTTOMLEFT:
				rect.left = rect.right - size.cx;
				rect.bottom = rect.top + size.cy;
				break;
			}

			if (rect.left > rectBound.right || rect.top > rectBound.bottom ||
				rect.right < rectBound.left || rect.bottom < rectBound.top)
			{
				rect = rectOld;
			}
		}

		ptOld = pt;

        if (rect != rectOld)
        {
        	DrawTrackRect(&dc, m_dp, &rect, TRUE, m_dp, &rectOld, TRUE);
        	rectOld = rect;
        }
    } // while

    // Turn off drag rectangle.
    DrawTrackRect(&dc, m_dp, &rect);
    ::ReleaseCapture();

    if (bCancel || rect == rectWnd)
        return FALSE;

    rectFinal = rect;

    return TRUE;
}

//  CDockWorker::DoMove
//      Given a point in screen coordinates of the location of the mouse down
//      that initiated the move.  Handles the dragging, and all necessary
//      dock changing to complete a move on a dockable window.

void CDockWorker::DoMove(CPoint pt)
{
	ASSERT(m_pWnd != NULL);

    DOCKPOS dp;
    CRect rect;

	// Give the user a hint about pressing the Ctrl key.
    SetPrompt(IDS_NODOCK_WINDOW, TRUE);

    // Drag the window while mouse is down.
    if (TrackMove(pt, dp, rect))
    	Move(dp, rect);

    SetPrompt();
}

//	CDockWorker::Move
//		Called both for the DoMove UI, as well as for DkMoveWindow moves a
//		window within the docking model.

void CDockWorker::Move(DOCKPOS dp, CRect& rect)
{
	ASSERT(m_pWnd != NULL);
	ASSERT(dp != dpHide);

	// an Embedded bar will be controlled by its parent
	if(dp==dpEmbedded)
	{
		m_dp=dp;
		return;
	}

    if ((IsVisible() || m_dp == dpNil) && (dp == dpCurrent || dp == m_dp))
    {
    	CRect rectWnd;
    	m_pWnd->GetWindowRect(rectWnd);
    	if (rect == rectWnd)
    		return;

        if (m_dp == dpNil)
        {
			rectWnd = rect;
        	CPaletteWnd::GetPaletteRect(rectWnd);
			//[multimon] 11 mar 97 - der : ForceRectBelowTop modified for MultiMon.
			::ForceRectBelowTop(rectWnd); 

        	ASSERT(m_pWnd->GetParent() != NULL);
            m_pWnd->GetParent()->MoveWindow(rectWnd);
        }
        else if(m_dp==dpEmbedded)
		{
			m_pWnd->Invalidate();
			// do nothing
		}
		else // moved within dock
        {
            CDockWnd* pDock = m_pManager->GetDock(m_dp);
            pDock->MoveWnd(m_pWnd, rect);
        }
    }
    else if (dp == dpCurrent)
    {
    	// Just size the window, but leave it hidden.
    	m_pWnd->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
    		SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
    }
    else    // dp is changing
    {
        if (dp == dpNil || dp == dpEmbedded)
            FloatWnd(rect);
        else    // need to dock the window.
        {
            if (m_dp != dpNil && IsVisible()) // was docked before
            {
	            CDockWnd* pDock;
				CRect rectDock, rectDockOld;

                pDock = m_pManager->GetDock(m_dp);
				pDock->GetWindowRect(rectDockOld);

                pDock->RemoveWnd(m_pWnd, m_pManager->GetDock(dpHide));

				pDock->GetWindowRect(rectDock);

				// Compensate for possible shift of docks.
				if (m_dp == dpTop && (dp == dpLeft || dp == dpRight))
					rect.OffsetRect(0, rectDock.Height() - rectDockOld.Height());
            }

			DOCKPLACEMENT place;
			place.dp = dp;
			place.rectAbsolute = rect;

			DockWnd(&place);
            m_bVisible = TRUE;
        }
    }
}

//  CDockWorker::TrackMove
//      Handles dragging of dockable window during a move operation.  Given,
//      the start point in screen coordinates, returns true if user didn't
//      cancel the drag, as well as a final DOCKPOS, and window rect.

BOOL CDockWorker::TrackMove(CPoint pt, DOCKPOS& dpFinal, CRect& rectFinal)
{
	// No tracking when we aren't really visible.  Might end up
	// tracking for during OLE activation.
	if (!m_pManager->IsVisible())
		return FALSE;

    BOOL bWasOutside;
    BOOL bCancel = FALSE;

	// Stretchy window variables.
	BOOL bStretchy = (m_pWnd->GetStyle() & DWS_STRETCHY) != 0;
	SHELL_DRAGINFO drag;
	SHELL_DRAGINFO* pDrag = (bStretchy && m_dp != dpNil ? &drag : NULL);
	CRect rectDraw, rectDrawOld;

	BOOL bGotRect, bGotNewRect, bNewRect = FALSE;
    BOOL bControl, bControlOld, bShift, bShiftOld;
    CRect rect, rectOld;
	CRect rectHorz, rectVert;
    DOCKPOS dp, dpOld;
    CPoint ptOld;

    CSize sizeHorz, sizeVert, sizeFloat;
    CRect rectTDock, rectLDock, rectBDock, rectRDock;
    CRect rectClient, rectDesk;

#if 1
	CLockDC dc(NULL);
#else
	CClientDC dc(NULL);
#endif

    m_pManager->GetDeskRects(rectDesk, rectClient);

    dp = m_dp;

	// Update size variables used in tracking move.
	if (dp == dpNil)
		SaveFloatPos();
	else
		SaveStretchSize();

    m_pWnd->GetWindowRect(rect);

    sizeHorz = GetMoveSize(dpTop, GetFloatSize());
    sizeVert = GetMoveSize(dpLeft, GetFloatSize());

    if (bStretchy && dp != dpNil)
    {
		CDockWnd* pDock = m_pManager->GetDock(dp);

		drag.dp = dp;
		VERIFY(!pDock->IsInsert(rect, &drag.iRow, FALSE));

    	if (m_dp == dpLeft || m_dp == dpRight)
		{
			drag.cAlongRow = rect.Height();
    		drag.cAlongOther = sizeVert.cy;
		}
    	else if (m_dp == dpTop || m_dp == dpBottom)
		{
			drag.cAlongRow = rect.Width();
    		drag.cAlongOther = sizeHorz.cx;
		}
    }

    if (dp == dpNil)
    {
    	ASSERT(m_pWnd->GetParent() != NULL);
        m_pWnd->GetParent()->GetWindowRect(rect);
        sizeFloat = rect.Size();
    }
    else
    {
    	CRect rectFloat;
    	GetFloatRect(rectFloat);
    	CPaletteWnd::GetPaletteRect(rectFloat);
    	sizeFloat = rectFloat.Size();
    }

#if 0
    // Make sure the initial drag rect matches the size for the DOCKPOS,
    // important for STRETCHY windows.
    //
    if (dp == dpTop || dp == dpBottom)
    {
        rect.right = rect.left + sizeHorz.cx;
        rect.bottom = rect.top + sizeHorz.cy;
    }
    else if (dp == dpLeft || dp == dpRight)
    {
        rect.right = rect.left + sizeVert.cx;
        rect.bottom = rect.top + sizeVert.cy;
    }
#endif

    // Calculate the snapping rects.
    if (m_bCanHorz)
    {
//		CSize sizeBar((sizeHorz.cx - g_mt.cxFrame) / 2, sizeHorz.cy - g_mt.cyFrame);
		CSize sizeBar(0, sizeHorz.cy - g_mt.cyFrame);
        rectTDock.SetRect(rectClient.left - sizeBar.cx, rectClient.top - sizeBar.cy,
            rectClient.right + sizeBar.cx, rectDesk.top + sizeBar.cy);
        rectBDock.SetRect(rectClient.left - sizeBar.cx, rectDesk.bottom - sizeBar.cy,
            rectClient.right + sizeBar.cx, rectClient.bottom + sizeBar.cy);
    }

    if (m_bCanVert)
    {
//		CSize sizeBar(sizeVert.cx - g_mt.cxFrame, (sizeVert.cy - g_mt.cyFrame) / 2);
		CSize sizeBar(sizeVert.cx - g_mt.cxFrame, 0);

		// Orion 96 Bug 17212 - The size of the docking areas was changed so that the
		// user couldn't get a floating window into a position where it could not be moved.
        rectLDock.SetRect(rectClient.left /*- sizeBar.cx*/, rectDesk.top - sizeBar.cy,
            rectDesk.left + sizeBar.cx, rectDesk.bottom + sizeBar.cy);
        rectRDock.SetRect(rectDesk.right - sizeBar.cx, rectDesk.top - sizeBar.cy,
            rectClient.right + sizeBar.cx - g_mt.cxFrame, rectDesk.bottom + sizeBar.cy);
    }

    // Loop while the mouse is down.
    CWnd* pWndTrack = CWnd::GetFocus();	// So we stay the active window.
    if (pWndTrack == NULL)
	{
		// Try to get a valid focused window.
		m_pManager->m_pFrame->SetFocus();
		if ((pWndTrack = CWnd::GetFocus()) == NULL)
			return FALSE;
	}

    pWndTrack->SetCapture();
    ptOld = pt;
    bControlOld = FALSE;
	bShiftOld = FALSE;

    // Turn on the drag rectangle.
    DrawTrackRect(&dc, dp, &rect, FALSE);

	rectDrawOld = rect;	// Save for stretchy windows.

    while (!PollForMouseChange(pt, FALSE, bCancel))
    {
        bControl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
		bShift = dp != dpNil && !bStretchy &&
			(GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

        if (pt == ptOld && bControl == bControlOld && bShift == bShiftOld)
        	continue;

        dpOld = dp;
        rectOld = rect;

        rect.OffsetRect(pt.x - ptOld.x, pt.y - ptOld.y);

        // Find out which dock position we are in.
        if (bControl)
            dp = dpNil;
        else
        {
        	// Bellow tests cursor and rect locations to determine the
        	// dock we are over, if any.  If the window could be in more
        	// than one dock, the dock with the center line closest to
        	// the center line of the window rect gets it.

            bWasOutside = !rectClient.PtInRect(ptOld);

			CRect rectTrack, rectTmp;
			CPoint ptMid((rect.left + rect.right) / 2,
					     (rect.top + rect.bottom) / 2);
			int nMinDelta = INT_MAX;
			int nCurDelta;

            dp = dpNil;
			bGotNewRect = FALSE;

            if (m_bCanHorz)
            {
				if(bShift && !m_bCanVert)
				{
					bShift=FALSE;
				}

                if (rect.top < rectDesk.top &&
                    rect.bottom > rectClient.top &&
                    rectTDock.PtInRect(pt))
                {
					rectTmp = rect;
					bGotRect = GetTrackRect(rectTmp, rectOld, pt,
						bShift ? sizeVert : sizeHorz, dpTop, dpOld,	bWasOutside,
						rectDesk, rectClient, bShift != bShiftOld, pDrag);

					if (rectTmp.left < rectClient.right &&
						rectTmp.right > rectClient.left)
					{
	                	nMinDelta = abs((rectTmp.top + rectTmp.bottom) / 2 -
	                		(rectTDock.top + rectTDock.bottom) / 2);
	                    dp = dpTop;
						rectTrack = rectTmp;
						bGotNewRect = bGotRect;
					}
                }
                else if (rect.bottom > rectDesk.bottom &&
                    rect.top < rectClient.bottom &&
                    rectBDock.PtInRect(pt))
                {
					rectTmp = rect;
					bGotRect = GetTrackRect(rectTmp, rectOld, pt,
						bShift ? sizeVert : sizeHorz, dpBottom, dpOld, bWasOutside,
						rectDesk, rectClient, bShift != bShiftOld, pDrag);

					if (rectTmp.left < rectClient.right &&
						rectTmp.right > rectClient.left)
					{
	                	nMinDelta = abs((rectTmp.top + rectTmp.bottom) / 2 -
	                		(rectBDock.top + rectBDock.bottom) / 2);
                        dp = dpBottom;
						rectTrack = rectTmp;
						bGotNewRect = bGotRect;
					}
				}
            }

            if (m_bCanVert)
            {
				if(bShift && !m_bCanHorz)
				{
					bShift=FALSE;
				}

                if (rect.left < rectDesk.left &&
                    rect.right > rectClient.left &&
                    rectLDock.PtInRect(pt))
                {
					rectTmp = rect;
					bGotRect = GetTrackRect(rectTmp, rectOld, pt,
						bShift ? sizeHorz : sizeVert, dpLeft, dpOld, bWasOutside,
						rectDesk, rectClient, bShift != bShiftOld, pDrag);

					if (rectTmp.top < rectDesk.bottom &&
						rectTmp.bottom > rectDesk.top)
					{
	                	nCurDelta = abs((rectTmp.left + rectTmp.right) / 2 -
	                		(rectLDock.left + rectLDock.right) / 2);

						if (dp == dpNil || nCurDelta < nMinDelta)
						{
	                        dp = dpLeft;
							rectTrack = rectTmp;
							bGotNewRect = bGotRect;
	                    }
					}
				}
                else if (rect.right > rectDesk.right &&
                    rect.left < rectClient.right &&
                    rectRDock.PtInRect(pt))
                {
					rectTmp = rect;
					bGotRect = GetTrackRect(rectTmp, rectOld, pt,
						bShift ? sizeHorz : sizeVert, dpRight, dpOld, bWasOutside,
						rectDesk, rectClient, bShift != bShiftOld, pDrag);

					if (rectTmp.top < rectDesk.bottom &&
						rectTmp.bottom > rectDesk.top)
					{
	                	nCurDelta = abs((rectTmp.left + rectTmp.right) / 2 -
	                		(rectRDock.left + rectRDock.right) / 2);

						if (dp == dpNil || nCurDelta < nMinDelta)
						{
	                        dp = dpRight;
							rectTrack = rectTmp;
							bGotNewRect = bGotRect;
						}
					}
				}
            }

			if (dp != dpNil)
				rect = rectTrack;
        }	// if(bControl)

        if (dp == dpNil)
		{
			GetTrackRect(rect, rectOld, pt, sizeFloat, dp, dpOld,
				bWasOutside, rectDesk, rectClient);

			// No new rect, so that dragging a transposed window to a floating
			// position will not cause the transpose bool to be cleared.
			//
			bGotNewRect = FALSE;
		}

		bNewRect = bNewRect || bGotNewRect;

		if (!bStretchy)
        	DrawTrackRect(&dc, dp, &rect, FALSE, dpOld, &rectOld, FALSE);
		else
		{
			rectDraw = rect;

			if (dp != dpNil)
			{
				BOOL bInsert = !m_pManager->HasDock(dp);

				if (!bInsert)
				{
					CDockWnd* pDock = m_pManager->GetDock(dp);
					bInsert = pDock->IsInsert(rect, NULL, FALSE);
				}

				if (dp == dpLeft || dp == dpRight)
				{
					if (bInsert || rectDraw.Height() >= rectDesk.Height())
					{
						rectDraw.top = rectDesk.top;
						rectDraw.bottom = rectDesk.bottom;
					}
				}
				else if (dp == dpTop || dp == dpBottom)
				{
					if (bInsert || rectDraw.Width() >= rectClient.Width())
					{
						rectDraw.left = rectClient.left;
						rectDraw.right = rectClient.right;
					}
				}
			}

	        DrawTrackRect(&dc, dp, &rectDraw, FALSE, dpOld, &rectDrawOld, FALSE);
			rectDrawOld = rectDraw;
		}

        ptOld = pt;
        bControlOld = bControl;
		bShiftOld = bShift;
    } // while

    // Turn of drag rectangle.
    DrawTrackRect(&dc, dp, bStretchy ? &rectDrawOld : &rect, FALSE);
    ::ReleaseCapture();

    if (bCancel)
        return FALSE;
    else
    {
        dpFinal = dp;
        if (dp == dpNil)
        	CPaletteWnd::GetChildRect(rect);
        rectFinal = rect;
		if (bNewRect)
			m_bTranspose = bShift;
        return TRUE;
    }
}

//	CDockWorker::GetTrackRect
//		Returns TRUE if a new rect was calculated, and FALSE otherwise.

BOOL CDockWorker::GetTrackRect(CRect& rect, const CRect& rectOld, CPoint pt,
	CSize size, DOCKPOS dp,	DOCKPOS dpOld, BOOL bWasOutside,
	const CRect& rectDesk, const CRect& rectClient,
	BOOL bForce /*=FALSE*/, SHELL_DRAGINFO* pDrag /*=NULL*/)
{
	if (!bForce && dp == dpOld && (pDrag == NULL || pDrag->dp != dp))
		return FALSE;

	if (pDrag != NULL && pDrag->dp == dp)
	{
		// If we are within the starting dock, we want to change the size
		// along the row dependant on whether we are in the starting row.
		// If we do not keep the size the same in the starting row, the window
		// will resize on NCLButtonDown in the caption.

		int iRow;
		CSize sizeCurrent = rect.Size();
		CDockWnd* pDock = m_pManager->GetDock(dp);

		DockTranspose(dp, &sizeCurrent);
		if (!pDock->IsInsert(rect, &iRow, FALSE) && iRow == pDrag->iRow)
			sizeCurrent.cx = pDrag->cAlongRow;
		else
			sizeCurrent.cx = pDrag->cAlongOther;
		DockTranspose(dp, &sizeCurrent);

		rect.right = rect.left + sizeCurrent.cx;
		rect.bottom = rect.top + sizeCurrent.cy;
	}

	if (bForce || dp != dpOld)
	{
	    switch (dp)
	    {
	    case dpNil:
	    default:
	        ASSERT(dp == dpNil);
	        rect.bottom = rect.top + size.cy;
	        rect.right = rect.left + size.cx;
	        if ((dpOld == dpTop && rect.top < rectClient.top) ||
	            (dpOld == dpBottom && rect.top < rectClient.bottom))
	        {
	            ASSERT(m_bCanHorz);
	            rect.bottom = rectOld.bottom;
	            rect.top = rect.bottom - size.cy;
	        }
	        else if ((dpOld == dpLeft && rect.left < rectClient.left) ||
	            (dpOld == dpRight && rect.left < rectClient.right))
	        {
	            ASSERT(m_bCanVert);
	            rect.right = rectOld.right;
	            rect.left = rect.right - size.cx;
	        }
	        break;

	    case dpTop:
	    case dpBottom:
	        if (bWasOutside == (dp == dpTop))
	            rect.top = rect.bottom - size.cy;
	        else
	            rect.bottom = rect.top + size.cy;

	        rect.right = rect.left + size.cx;

	        if (pt.y < rect.top)
	        {
	            rect.top = pt.y - g_mt.cyFrame;
	            rect.bottom = rect.top + size.cy;
	        }
	        if (pt.y > rect.bottom)
	        {
	            rect.bottom = pt.y + g_mt.cyFrame;
	            rect.top = rect.bottom - size.cy;
	        }
	        break;

	    case dpLeft:
	    case dpRight:
	        if (bWasOutside == (dp == dpLeft))
	            rect.left = rect.right - size.cx;
	        else
	            rect.right = rect.left + size.cx;

	        rect.bottom = rect.top + size.cy;

	        if (pt.x < rect.left)
	        {
	            rect.left = pt.x - g_mt.cxFrame;
	            rect.right = rect.left + size.cx;
	        }
	        if (pt.x > rect.right)
	        {
	            rect.right = pt.x + g_mt.cxFrame;
	            rect.left = rect.right - size.cx;
	        }
	        break;
	    } // switch (dp)
	}

    if (pt.x < rect.left || pt.x > rect.right)
    {
        int cx = rect.Width();

        rect.left = pt.x - cx / 2;
        rect.right = rect.left + cx;
    }

    if (pt.y < rect.top || pt.y > rect.bottom)
    {
        int cy = rect.Height();

        rect.top = pt.y - cy / 2;
        rect.bottom = rect.top + cy;
    }

	return TRUE;
}

//  CDockWorker::DrawTrackRect
//      Given a DC, and one or two rect-DOCKPOS pair(s), inverts the
//      specified drag rect(s) (with width dependant on DOCKPOS).  Two
//      rects for minimal flicker.

void CDockWorker::DrawTrackRect(CDC* pdc,
	DOCKPOS dp, const CRect* prect, BOOL bChildRect /*=TRUE*/,
	DOCKPOS dpLast /*=dpNil*/, const CRect* prectLast /*=NULL*/,
	BOOL bChildRectLast /*=TRUE*/)
{
    CSize size(g_mt.cxBorder, g_mt.cyBorder);
    CSize sizeLast(g_mt.cxBorder, g_mt.cyBorder);
	CBrush *pbr;

	CBrush* pbrSolid = GetSysBrush(CMP_COLOR_HILITE);
	CBrush* pbrHalf = CDC::GetHalftoneBrush();
	if (pbrSolid == NULL || pbrHalf == NULL)
		return;

	pbr = pbrSolid;
    CRect rect = *prect;
    if (dp == dpNil)
    {
		pbr = pbrHalf;
    	if (bChildRect)
    		CPaletteWnd::GetPaletteRect(rect);

        size.cx = g_mt.cxFrame - g_mt.cxBorder;
        size.cy = g_mt.cyFrame - g_mt.cyBorder;
    }
	prect = &rect;

    CRect	rectLast;
	CBrush	*pbrLast = NULL;

    if (prectLast != NULL)
    {
		pbrLast = pbrSolid;
        rectLast = *prectLast;
        if (dpLast == dpNil)
        {
			pbrLast = pbrHalf;
	    	if (bChildRectLast)
	    		CPaletteWnd::GetPaletteRect(rectLast);

            sizeLast.cx = g_mt.cxFrame - g_mt.cxBorder;
            sizeLast.cy = g_mt.cyFrame - g_mt.cyBorder;
        }
		prectLast = &rectLast;
    }

	pdc->DrawDragRect(prect, size, prectLast, sizeLast, pbr, pbrLast);
}

//	CDockWorker::GetMoveSize
//		Calls the window for the dock snapping sizes.

CSize CDockWorker::GetMoveSize(DOCKPOS dp, CSize size)
{
	ASSERT(m_pWnd != NULL);

	if ((m_pWnd->GetStyle() & DWS_STRETCHY) != 0)
	{
		if (dp == dpLeft || dp == dpRight || dp == dpVertInit)
			size.cx = m_sizeLastStretch.cx;
		else if (dp == dpTop || dp == dpBottom || dp == dpHorzInit)
			size.cy = m_sizeLastStretch.cy;

		return size;
	}
	else
	{
		LONG lSize = m_pWnd->SendMessage(DWM_GETMOVESIZE, (UINT) dp,
			MAKELPARAM(size.cx, size.cy));

		if (lSize)
			return CSize((short) LOWORD(lSize), (short) HIWORD(lSize));
		else
			return size;
	}
}

//	CDockWorker::GetTrackSize
//		Calls the window for the sizing snap sizes.

CSize CDockWorker::GetTrackSize(UINT nHitTest, CSize size)
{
	ASSERT(m_pWnd != NULL);

	LONG lSize = m_pWnd->SendMessage(DWM_GETTRACKSIZE, nHitTest,
		MAKELPARAM(size.cx, size.cy));

	if (lSize)
		return CSize((short) LOWORD(lSize), (short) HIWORD(lSize));
	else
		return size;
}

//	CDockWorker::SaveStretchSize
//		Saves the current window size into m_sizeLastStretch.

void CDockWorker::SaveStretchSize()
{
	ASSERT(m_pWnd != NULL);
#ifdef _DEBUG
	CWnd* pWndParent = m_pWnd->GetParent();
 	ASSERT(pWndParent != NULL &&
		pWndParent->IsKindOf(RUNTIME_CLASS(CDockWnd)));
#endif

	CRect rect;
	m_pWnd->GetWindowRect(rect);
	if (m_dp == dpLeft || m_dp == dpRight)
		m_sizeLastStretch.cx = rect.Width();
	else if (m_dp == dpTop || m_dp == dpBottom)
		m_sizeLastStretch.cy = rect.Height();
}

//	CDockWorker::SaveFloatPos
//		Saves the current window position into the m_rectFloat.

void CDockWorker::SaveFloatPos()
{
	if (m_pWnd == NULL)
		return;

#ifdef _DEBUG
	if(m_dp!=dpEmbedded)
	{
		CWnd* pWndParent = m_pWnd->GetParent();
		ASSERT(pWndParent != NULL &&
			pWndParent->IsKindOf(RUNTIME_CLASS(CPaletteWnd)));
	}
#endif

	// Never save positions the leave us off screen.
    m_pWnd->GetWindowRect(m_rectFloat);
	if(m_dp!=dpEmbedded)
	{
		CPaletteWnd::GetPaletteRect(m_rectFloat);
		ForceRectOnScreen(m_rectFloat);
		CPaletteWnd::GetChildRect(m_rectFloat);
	}

    m_bPosition = FALSE;
}

//	CDockWorker::SaveMDIPos
//		Saves the current desk window position into m_pDeskPlace.

void CDockWorker::SaveMDIPos()
{
	// Do nothing if there is no window, or we already have place info.
	// Thus it is necessary to clear the old information before calling
	// this.

	if (m_pWnd == NULL)
		return;

	ASSERT(m_pDeskPlace == NULL);

	CMDIChildDock* pMDIChild = (CMDIChildDock*) m_pWnd->GetParent();
	ASSERT(pMDIChild != NULL &&
		pMDIChild->IsKindOf(RUNTIME_CLASS(CMDIChildDock)));

	m_pDeskPlace = new DESKPLACEMENT;
	m_pDeskPlace->place.length = sizeof(WINDOWPLACEMENT);
	pMDIChild->GetWindowPlacement(&m_pDeskPlace->place);
}

void CDockWorker::SaveDockPos(BOOL bRelative /*=TRUE*/)
{
	// Do nothing if there is no window, or we already have place info.
	// Thus it is necessary to clear the old information before calling
	// this.

	if (m_pWnd == NULL)
		return;

	SaveStretchSize();

	ASSERT((bRelative && m_pDockPlace == NULL) ||
		   (!bRelative && m_pDockSerial == NULL));	// otherwise we leak.

	CDockWnd* pDock = (CDockWnd*) m_pWnd->GetParent();
	ASSERT(pDock != NULL &&
		pDock->IsKindOf(RUNTIME_CLASS(CDockWnd)));

	if (bRelative)
	{
		m_pDockPlace = new DOCKPLACEMENT;
		m_pDockPlace->dp = m_dp;
		pDock->GetDockRestore(&m_pDockPlace->drRelative, m_pWnd);
	}
	else
	{
		m_pDockSerial = new DOCKPLACEMENT;
		m_pDockSerial->dp = dpNil;
		m_pWnd->GetWindowRect(&m_pDockSerial->rectAbsolute);
		pDock->ScreenToClient(&m_pDockSerial->rectAbsolute);
	}
}

//	CDockWorker::GetFloatSize
//		Returns the size in m_rectFloat, unless the window still has the
//		position bit set, in which case it uses the current size.

CSize CDockWorker::GetFloatSize()
{
	ASSERT(m_pWnd != NULL);

	if (!m_bPosition || !IsVisible())
		return m_rectFloat.Size();
	else
	{
		CRect rect;
		m_pWnd->GetWindowRect(rect);
		return rect.Size();
	}
}

//	CDockWorker::GetFloatRect
//		Returns the rect in m_rectFloat, unless the window still has the
//		position bit set, in which case it uses the current window rect.

void CDockWorker::GetFloatRect(CRect& rect)
{
	ASSERT(m_pWnd != NULL);

	if (m_bPosition)
	{
		if (IsVisible())
			m_pWnd->GetWindowRect(m_rectFloat);
		else
		{
			CRect rectDummy;
			m_pManager->GetDeskRects(m_rectFloat, rectDummy);
			m_rectFloat.OffsetRect(20, 20);	// Semi-random choice.
			// This thing had better be able to size itself!
		}
	}

	CSize size = GetMoveSize(dpNil, GetFloatSize());
	m_rectFloat.right = m_rectFloat.left + size.cx;
	m_rectFloat.bottom = m_rectFloat.top + size.cy;
	rect = m_rectFloat;
}

//	CDockWorker::GetMaxState

MAX_STATE CDockWorker::GetMaxState()
{
	DOCKPOS dp = GetDock();
	switch (dp)
	{
	case dpLeft:
	case dpRight:
	case dpTop:
	case dpBottom:
		//BLOCK:
		{
			CDockWnd* pDock = m_pManager->GetDock(dp);
			if (pDock != NULL)
			{
				ASSERT_VALID(m_pWnd);
				return pDock->GetMaxState(m_pWnd);
			}
		}
		break;
	}

	return stateDisabled;
}

//	CDockWorker::MaximizeRestore

void CDockWorker::MaximizeRestore()
{
	DOCKPOS dp = GetDock();
	switch (dp)
	{
	case dpLeft:
	case dpRight:
	case dpTop:
	case dpBottom:
		//BLOCK:
		{
			CDockWnd* pDock = m_pManager->GetDock(dp);
			if (pDock != NULL)
			{
				ASSERT_VALID(m_pWnd);
				pDock->MaximizeRestore(m_pWnd);
			}
		}
		break;
	}
}

void CDockWorker::Associate(BOOL bHost, UINT nID)
{
	m_bHost=bHost;
	m_nIDAssociate=nID;
}

///////////////////////////////////////////////////////////////////////////////
//  CDockManager
//      The dock manager is responsible for the overall dock management
//      associated with a CFrameWnd that requires docking services.  It
//      keeps track of all dockable windows associated with a frame, as
//      well as all floating palettes, and the docks.  The manager also
//      provides a forwarding point for messages between dockable windows
//      and their dock workers (mostly for external users of the docks).

//	CDockManager::Serialize
//		Load/Saves a dock manager to disk.

void CDockManager::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		m_listWorker.Serialize(ar);

	    for (int i = 0; i < dpCount - 1; i++)
	    {
    		ar << (DWORD) (LPVOID) m_rgpDock[i];

	    	if (m_rgpDock[i] != NULL)
	    		ar << m_rgpDock[i];
	    }
	}
	else
	{
		DWORD dw;

		m_listWorker.Serialize(ar);

	    for (int i = 0; i < dpCount - 1; i++)
	    {
	    	ar >> dw;
	    	m_rgpDock[i] = (CDockWnd*) (LPVOID) dw;

	    	if (m_rgpDock[i] != NULL)
	    		ar >> m_rgpDock[i];
	    }
	}
}

//  CDockManager::CDockManager
//		Construction.
//      Note: rect is not actually a rect, it specifies the  border on
//      each side of pFrame's client area, used by border windows other
//      than the CDockWnds (e.g. status bar).

CDockManager::CDockManager()
{
	m_pFrame = NULL;
	m_pView = NULL;
    m_bVisible = FALSE;
    m_bDelayedShow = FALSE;
	m_bInDocObjectMode = FALSE;
	m_bAvailableWndDirty = FALSE;

    for (int i = 0; i < dpCount; i++)
        m_rgpDock[i] = NULL;    // defer creation.

	m_mapCmdToWorker.InitHashTable(29);
}

//	CDockManager::~CDockManager
//		Destructor.  Removes all windows, and destroys all docks.

CDockManager::~CDockManager()
{
	ShowManager(FALSE);

	CObArray aWorkers;
	ArrayOfType(dtEmbeddedToolbar, &aWorkers, FALSE, FALSE);

	for(int i=0; i<aWorkers.GetSize(); ++i)
	{
		CDockWorker *pWorker=(CDockWorker*)aWorkers[i];
		POSITION pos;
		WorkerFromID(pWorker->m_nIDWnd, &pos);
		RemoveWorker(pWorker, pos);
	}

	while (!m_listWorker.IsEmpty())
	{
		RemoveWorker((CDockWorker*) m_listWorker.GetHead(),
			m_listWorker.GetHeadPosition());
	}

    ASSERT(m_listFloat.IsEmpty());
	ASSERT(m_listDesk.IsEmpty());

    for (i = 0; i < dpCount; i++)
        delete m_rgpDock[i];
}

//	CDockManager::Create
//		Initializes the dock manager for the information loaded from
//		deserialization, or let clients register defaults if we have
//		no workers.

BOOL CDockManager::Create(CFrameWnd* pFrame, CWnd* pView,
	CDockManager* pManagerOld /*=NULL*/)
{
	ASSERT(pFrame != NULL);
    m_pFrame = pFrame;
	m_pView = pView;

	// Make sure we are z-ordered below bottom-most CControlBar.
	const CWnd* pWndAfter = &CWnd::wndTop;
	for (CWnd* pWndNext = m_pFrame->GetTopWindow(); pWndNext != NULL;
		pWndNext = pWndNext->GetWindow(GW_HWNDNEXT))
	{
		if (pWndNext->IsKindOf(RUNTIME_CLASS(CControlBar)))
			pWndAfter = pWndNext;
	}

	// Create the docks.
    for (int i = 0; i < dpCount; i++)
    {
    	if (m_rgpDock[i] != NULL && m_rgpDock[i]->m_nRows > 0)
		{
    		if (m_rgpDock[i]->Create(m_pFrame, IDR_DOCKBASE + i, this))
			{
				m_rgpDock[i]->SetWindowPos(pWndAfter, 0, 0, 0, 0,
					SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
				pWndAfter = m_rgpDock[i];
			}
		}
    }

	POSITION pos1, pos2, posTmp;
	CDockWorker *pDocker1, *pDocker2;

	// If we are transfering from an existing manager, transfer any already
	// existing windows.

	if (pManagerOld == NULL)
	{
		// Get info from packages.
		HGLOBAL hglob;

		POSITION pos = theApp.m_packages.GetHeadPosition();
		while (pos != NULL)
		{
			CPackage* pGrp = (CPackage*)theApp.m_packages.GetNext(pos);
			if ((hglob = pGrp->GetDockInfo()) == NULL)
				continue;

			UINT FAR* lpnInfoCount = (UINT FAR*)GlobalLock(hglob);
			DOCKINIT FAR* lpdiArray = (DOCKINIT FAR*)(lpnInfoCount + 1);

			RegisterInfo(*lpnInfoCount, lpdiArray);

			GlobalUnlock(hglob);
			GlobalFree(hglob);
		}
	}
	else
	{
		pManagerOld->ShowManager(FALSE, updNoRecalc);

		// Transfer windows between managers, and make sure the new manager
		// has the same workers as the old manager.  User may have made custom
		// toolbar changes in the old manager.
		//
		pos1 = m_listWorker.GetHeadPosition();
		pos2 = pManagerOld->m_listWorker.GetHeadPosition();

		while (pos1 != NULL && pos2 != NULL)
		{
		    pDocker1 = (CDockWorker*) m_listWorker.GetAt(pos1);
		    pDocker2 = (CDockWorker*) pManagerOld->m_listWorker.GetAt(pos2);

        	pDocker1->SetManager(this);	// So DeparentWindow works.

		    if (DkCompareIDs(pDocker1->m_nIDWnd, pDocker2->m_nIDWnd) < 0)
		    {
				// Remove new worker with no match in current layout.
		    	posTmp = pos1;
		    	m_listWorker.GetNext(pos1);
		    	m_listWorker.RemoveAt(posTmp);
				delete pDocker1;
		    }
		    else
		    {
		    	if (pDocker1->m_nIDWnd != pDocker2->m_nIDWnd)
				{
					pDocker2->PreTransfer(this);
		    		m_listWorker.InsertBefore(pos1, pDocker2);
				}
				else
				{
					m_listWorker.GetNext(pos1);
					pDocker1->Reset(pDocker2);
			    	delete pDocker2;
			    }

		    	posTmp = pos2;
		    	pManagerOld->m_listWorker.GetNext(pos2);
		    	pManagerOld->m_listWorker.RemoveAt(posTmp);
		    }
		}

		// Remove remaining new workers with no match in the current layout.
		while (pos1 != NULL)
		{
	    	posTmp = pos1;
	    	delete m_listWorker.GetNext(pos1);
	    	m_listWorker.RemoveAt(posTmp);
		}

		// Add to new layout remaining workers in current layout with no match
		// in new layout.
		while (pos2 != NULL)
		{
	    	posTmp = pos2;
		    pDocker2 = (CDockWorker*) pManagerOld->m_listWorker.GetNext(pos2);

			pDocker2->PreTransfer(this);

	    	pManagerOld->m_listWorker.RemoveAt(posTmp);
	    	m_listWorker.AddTail(pDocker2);
	    }
	}

	// Now Create the visible dock workers to ensure their m_pWnds, and
	// position them.

	CObArray aWnd;
	CWordArray anShow;
	CWnd* pWndMDITop = ((CMainFrame*) m_pFrame)->MDIGetActive();
	CPartFrame::LockWorkspace(TRUE);

	pos1 = m_listWorker.GetHeadPosition();
    while (pos1 != NULL)
    {
        pDocker1 = (CDockWorker*) m_listWorker.GetAt(pos1);
		ASSERT(pDocker1 != NULL);

		// If this is our first manager, load data from the Reg DB.  Also,
		// remove any dockable windows whose packages no longer exist.
		if (pManagerOld == NULL)
		{
			if (theApp.GetPackage(pDocker1->m_nIDPackage) == NULL)
			{
				posTmp = pos1;
				delete m_listWorker.GetNext(pos1);
				m_listWorker.RemoveAt(posTmp);
				continue;
			}

			pDocker1->LoadData();
		}

		if (pDocker1->m_pWnd == NULL &&	pDocker1->m_hglobInit == NULL &&
			pDocker1->m_nIDPackage == PACKAGE_SUSHI &&
			LOWORD(pDocker1->m_nIDWnd) >= IDTB_CUSTOM_BASE)
		{
			// Remove non-existent custom toolbars.
	    	posTmp = pos1;
    		delete m_listWorker.GetNext(pos1);
	    	m_listWorker.RemoveAt(posTmp);
		}
        else
		{
			pDocker1->SetManager(this);	// Just to be sure this happens.

			if (!pDocker1->IsVisible() && pDocker1->m_pWnd != NULL &&
				pDocker1->m_dtWnd == dtEdit)
			{
				if (pDocker1->m_pWnd->GetParent() == pWndMDITop)
					pWndMDITop = NULL;

				if (pDocker1->IsDestroyOnHide())
				{
					pDocker1->DestroyWindow();
					ASSERT(pDocker1->m_pWnd == NULL);
				}
			}
			
			// If the window was available before, check again now to avoid window flash.
			if (pDocker1->IsVisible() && pDocker1->m_bAsk)
			{
				CPackage* pGrp = theApp.GetPackage(pDocker1->m_nIDPackage);
				if (pGrp != NULL)
					pDocker1->m_bAvailable = pGrp->AskAvailable(LOWORD(pDocker1->m_nIDWnd));
			}

			if (pDocker1->IsVisible() || pDocker1->m_pWnd != NULL)
			{
				WORD wShow = (pDocker1->m_pDeskPlace == NULL) ?
					 (WORD) -1 : (WORD) pDocker1->m_pDeskPlace->place.showCmd;

				if (pDocker1->Create(this) &&
					pDocker1->GetDock() == dpMDI &&	pDocker1->IsVisible())
				{
					aWnd.Add(pDocker1->m_pWnd);
					anShow.Add(wShow);
				}
			}

			pDocker1->EndLoad();

    		m_listWorker.GetNext(pos1);
		}
    }

	// Deal with MDI windows.
	CPartFrame::LockWorkspace(FALSE);

	ASSERT(anShow.GetSize() == aWnd.GetSize());
	int nCount = anShow.GetSize();
	if (nCount > 0)
	{
		((CMainFrame*) m_pFrame)->m_pWndMDIClient->ShowWindow(SW_HIDE);

		// Show the windows.
		CWnd* pMDIChild = NULL;

		for (i = 0; i < nCount; i++)
		{
			if (aWnd[i] == NULL || anShow[i] == (WORD) -1)
				continue;

			pMDIChild = ((CWnd*) aWnd[i])->GetParent();
			ASSERT(pMDIChild != NULL &&
				pMDIChild->IsKindOf(RUNTIME_CLASS(CMDIChildDock)));

			if (anShow[i] == SW_SHOWMAXIMIZED && !theApp.m_bMaximizeDoc)
				pMDIChild->ShowWindow(SW_SHOW);
			else
			{
				// DS96 #17641 [CFlaat]: need to override recorded placement in the maximization case as well as the non-maximization case!
				if (theApp.m_bMaximizeDoc)
					pMDIChild->ShowWindow(SW_SHOWMAXIMIZED);
				else
					pMDIChild->ShowWindow(anShow[i]);
			}
		}

		if (pWndMDITop != NULL)
			pWndMDITop->BringWindowToTop();
		else if (pMDIChild != NULL)
			pMDIChild->BringWindowToTop();

		((CMainFrame*) m_pFrame)->m_pWndMDIClient->ShowWindow(SW_SHOWNA);
	}

	// Make sure a window that was hidden by the switch does not
	// stay the active worker.
	pDocker1 = CDockWorker::s_pActiveDocker;
	if (pDocker1 != NULL && !pDocker1->IsVisible())
	{
		CDockWorker::s_bLockDocker = FALSE;
		CDockWorker::s_pActiveDocker->OnWindowActivate(FALSE, m_pFrame);

		// Set focus to the main frame only if we had the focus.
		CWnd* pWndFocus = CWnd::GetFocus();
		CWnd* pWndDocker = pDocker1->m_pWnd;
		if (pWndDocker == pWndFocus || pWndDocker->IsChild(pWndFocus))
			m_pFrame->SetFocus();

	}

	m_bAvailableWndDirty = TRUE;

	return TRUE;
}

void CDockManager::SaveWorkers()
{
	CDockWorker* pDocker;

    POSITION pos = m_listWorker.GetHeadPosition();
    while (pos != NULL)
    {
        pDocker = (CDockWorker*) m_listWorker.GetNext(pos);
		pDocker->SaveData();
    }
}

//  CDockManager::GetDock
//      Given a DOCKPOS, returns a pointer the CDockWnd for that pos.  If the
//      CDockWnd does not yet exist it is created.

CDockWnd* CDockManager::GetDock(DOCKPOS dp)
{
	ASSERT(m_pFrame != NULL);
    ASSERT(dpFirst <= dp && dp < dpFirst + dpCount);
    int iDock = dp - dpFirst;

    if (m_rgpDock[iDock] == NULL)
        m_rgpDock[iDock] = new CDockWnd(dp);

	if (m_rgpDock[iDock]->m_hWnd == NULL)
	{
        if (!m_rgpDock[iDock]->Create(m_pFrame, IDR_DOCKBASE + iDock, this))
            return NULL;

		// Make sure we are z-ordered below bottom-most CControlBar.
		const CWnd* pWndAfter = &CWnd::wndTop;
		for (CWnd* pWndNext = m_pFrame->GetTopWindow(); pWndNext != NULL;
			pWndNext = pWndNext->GetWindow(GW_HWNDNEXT))
		{
			if (pWndNext->IsKindOf(RUNTIME_CLASS(CControlBar)))
				pWndAfter = pWndNext;
		}

        for (int i = 0; i < iDock; i++)
        {
            if (m_rgpDock[i]->GetSafeHwnd() != NULL)
				pWndAfter = m_rgpDock[i];
        }

		// place new dock in correct z-order.
		m_rgpDock[iDock]->SetWindowPos(pWndAfter, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

        // position new dock
        if (dp != dpHide)
        {
	        m_pFrame->RecalcLayout();
	        if (!m_bVisible)
				m_rgpDock[iDock]->ShowWindow(SW_HIDE);	// Created visible for repositioning.
		}
    }

    return m_rgpDock[iDock];
}

//  CDockManager::GetDeskRects
//      For CDockWnd::TrackMove, this function returns two rectangles used
//      to calculate the snapping rects for dragging.

void CDockManager::GetDeskRects(CRect& rectDesk, CRect& rectClient)
{
    m_pView->GetWindowRect(rectDesk);

    rectClient = rectDesk;

    // displace client rect with any dockwnds
    CRect rect;
    if (m_rgpDock[dpLeft - dpFirst]->GetSafeHwnd() != NULL)
    {
        m_rgpDock[dpLeft - dpFirst]->GetWindowRect(rect);
        rectClient.left = rect.left + g_mt.cxBorder;
    }
    if (m_rgpDock[dpTop - dpFirst]->GetSafeHwnd() != NULL)
    {
        m_rgpDock[dpTop - dpFirst]->GetWindowRect(rect);
        rectClient.top = rect.top + g_mt.cxBorder;
    }
    if (m_rgpDock[dpRight - dpFirst]->GetSafeHwnd() != NULL)
    {
        m_rgpDock[dpRight - dpFirst]->GetWindowRect(rect);
        rectClient.right = rect.right - g_mt.cxBorder;
    }
    if (m_rgpDock[dpBottom - dpFirst]->GetSafeHwnd() != NULL)
    {
        m_rgpDock[dpBottom - dpFirst]->GetWindowRect(rect);
        rectClient.bottom = rect.bottom - g_mt.cxBorder;
    }
}

UINT CDockManager::GetCustomID()
{
	CDockWorker* pDocker = (CDockWorker*) m_listWorker.GetTail();

	return max(IDTB_CUSTOM_BASE, LOWORD(pDocker->m_nIDWnd)) + 1;
}

//	CDockManager::RegisteInfo
//		Used for client to register a block of dock workers.

void CDockManager::RegisterInfo(UINT nInfoCount, DOCKINIT FAR* lpdi)
{
	for (UINT i = 0; i < nInfoCount; i++, lpdi++)
		RegisterWnd((CWnd*) NULL, lpdi);
}

CDockWorker* CDockManager::RegisterToolbar(DOCKINIT FAR* lpdi,
	TOOLBARINIT FAR* lptbi, UINT FAR* lpIDArray)
{
	ASSERT(lpdi != NULL && lptbi != NULL);

	CASBar* pBar = new CASBar;
	if (!pBar->Create(NULL, this, WS_BORDER, lptbi,
			lpIDArray, NULL, TRUE, lpdi))
	{
		delete pBar;
		return NULL;
	}

	return WorkerFromID(lpdi->nIDWnd);
}

CDockWorker* CDockManager::RegisterWnd(HWND hwnd, DOCKINIT FAR* lpdi)
{
	ASSERT(lpdi != NULL);

	CDockWorker* pDocker = NULL;
	DOCKTYPE dt = lpdi->dt;

	switch (dt)
	{
	case dtHwnd:
		return RegisterWnd(CWnd::FromHandlePermanent(hwnd), lpdi);

	case dtBorder:
	case dtEdit:
		if (hwnd == NULL)
			 return RegisterWnd((CWnd*) NULL, lpdi);
		else
		{
			CBorderBar* pBar = new CBorderBar;
			DWORD dwStyle = 0x0F & ::GetWindowLong(hwnd, GWL_STYLE);
			if (!pBar->Create(NULL, this, dwStyle, hwnd,
				MAKEDOCKID(lpdi->nIDPackage, lpdi->nIDWnd),
				TRUE, lpdi))
			{
				delete pBar;
				return NULL;
			}

			if (pDocker == NULL)
			{
				ASSERT(lpdi != NULL);
				pDocker = WorkerFromWindow(pBar);
			}
		}
		break;

	default:
		// Use other functions for other types.
		ASSERT(FALSE);
		break;
	}

    return pDocker;
}

//  CDockManager::RegisterWnd
//      For internal use (CWnd already created), creates a DockWorker
//      for the window, and returns a pointer.

CDockWorker* CDockManager::RegisterWnd(CWnd* pWnd, DOCKINIT FAR* lpdi)
{
	ASSERT(lpdi != NULL);

	POSITION pos;
	CDockWorker* pDocker = WorkerFromID(MAKEDOCKID(lpdi->nIDPackage,
		lpdi->nIDWnd), &pos);

	if ((lpdi->fInit & INIT_REMOVE) != 0)
	{
		if (pDocker != NULL)
			RemoveWorker(pDocker, pos);

		return NULL;
	}
	else if (pDocker == NULL)
	{
		ASSERT(lpdi != NULL);
		pDocker = new CDockWorker(lpdi);
	}
	else if (pDocker->m_pWnd == NULL)
	{
		pDocker->Reset(lpdi);
	}

	if (pDocker->m_pWnd != NULL)
		return pDocker;

	if (pWnd == NULL)
		pDocker->SetManager(this);
	else if (!pDocker->Create(this, pWnd, (lpdi->fInit & INIT_VISIBLE) != 0))
	{
		if (pos != NULL)
			m_listWorker.RemoveAt(pos);

		delete pDocker;
		return NULL;
	}

	if (pos == NULL)
		InsertWorker(pDocker);

    return pDocker;
}

//	CDockManager::InsertWorker
//		Inserts a worker into the list in sorted order.

void CDockManager::InsertWorker(CDockWorker* pDocker)
{
	CDockWorker* pDockerNext;
    POSITION pos = m_listWorker.GetHeadPosition();
    while (pos != NULL)
    {
        pDockerNext = (CDockWorker*) m_listWorker.GetAt(pos);
        if (DkCompareIDs(pDockerNext->m_nIDWnd, pDocker->m_nIDWnd) > 0)
            break;

        m_listWorker.GetNext(pos);
    }

	if (pos == NULL)
		m_listWorker.AddTail(pDocker);
	else
		m_listWorker.InsertBefore(pos, pDocker);
}

//  CDockManager::RemoveWnd
//      Given window pointer, removes the window from the registered list,
//		destroying its dock worker.

void CDockManager::RemoveWnd(CWnd* pWnd)
{
    POSITION pos;
    CDockWorker* pDocker = WorkerFromWindow(pWnd, &pos);
    if (pDocker == NULL)
    {
    	TRACE0("Warning: Attemping to remove unregistered dockable window.\n");
    	return;
    }

    RemoveWorker(pDocker, pos);
}

//  CDockManager::RemoveWnd
//      Given window ID, removes the window from the registered list,
//		destroying its dock worker.

void CDockManager::RemoveWnd(UINT nID)
{
    POSITION pos;
    CDockWorker* pDocker = WorkerFromID(nID, &pos);
    if (pDocker == NULL)
    {
    	TRACE0("Warning: Attemping to remove unregistered dockable window.\n");
    	return;
    }

    RemoveWorker(pDocker, pos);
}

//	CDockManager::RemoveWorker
//		Given a pointer to a worker, and its position in the list, removes
//		the worker from the list, also handling destruction of the window,
//		and palette (if floating).

void CDockManager::RemoveWorker(CDockWorker* pDocker, POSITION pos)
{
	ASSERT_VALID(pDocker);

	if (CASBar::s_pCustomizer != NULL &&
		CASBar::s_pCustomizer->m_pDialog != NULL)
	{
		// If we have a CToolbar dialog, update the checklist.
		if (CASBar::s_pCustomizer->m_pDialog->
			IsKindOf(RUNTIME_CLASS(CToolbarDialog)))
		{
			ASSERT(pDocker->m_nIDPackage == PACKAGE_SUSHI &&
				LOWORD(pDocker->m_nIDWnd) >= IDTB_CUSTOM_BASE);
			((CToolbarDialog*) CASBar::s_pCustomizer->m_pDialog)->
				RemoveToolbar(pDocker);
		}
	}

    m_listWorker.RemoveAt(pos);

	if (m_bVisible)
		pDocker->ShowWindow(FALSE);

    delete pDocker;
}

// returns 0 if message is successfully processed, and -1 if not.
LONG CDockManager::WorkerMessage(UINT message, HWND hwnd, LPARAM lParam)
{
    ASSERT(hwnd != NULL);

    POSITION pos;
    CDockWorker* pDocker;
    CWnd* pWnd = CWnd::FromHandlePermanent(hwnd);

    if ((pDocker = WorkerFromWindow(pWnd, &pos)) == NULL)
    {
    	TRACE0("Warning: Unable to find worker to handle message.\n");
        return 0L;
    }

    return pDocker->OnMessage(message, lParam);
}

//  CDockManager::WorkerFromWindow
//      Given a window pointer, returns a pointer to the dock worker that
//      corresponds to the window, as well as the postition of the window in
//      the registration list.

CDockWorker* CDockManager::WorkerFromWindow(CWnd* pWnd,
	POSITION* posFinal /*=NULL*/)
{
	if (pWnd == NULL)
		return NULL;

    CDockWorker* pDocker;

    POSITION pos = m_listWorker.GetHeadPosition();
    while (pos != NULL)
    {
        pDocker = (CDockWorker*) m_listWorker.GetAt(pos);
        if (pDocker->m_pWnd == pWnd)
            break;

        m_listWorker.GetNext(pos);
    }

    if (posFinal != NULL)
    	*posFinal = pos;

    return (pos == NULL) ? NULL : pDocker;
}

//  CDockManager::WorkerFromID
//      Given a window ID, returns a pointer to the dock worker that
//      corresponds to the window, as well as the postition of the window in
//      the registration list.

CDockWorker* CDockManager::WorkerFromID(UINT nID,
		POSITION* posFinal /*=NULL*/)
{
    CDockWorker* pDocker;

    POSITION pos = m_listWorker.GetHeadPosition();
    while (pos != NULL)
    {
        pDocker = (CDockWorker*) m_listWorker.GetAt(pos);
        if (pDocker->m_nIDWnd == nID)
            break;

        m_listWorker.GetNext(pos);
    }

    if (posFinal != NULL)
    	*posFinal = pos;

    return (pos == NULL) ? NULL : pDocker;
}

//	CDockManager::ShowMenu
//		Shows the manager popup (with all available, registered windows)
//		at the specied point on the screen.

void CDockManager::ShowMenu(CPoint pt)
{
	CString str;

	CObArray aWorkers;
	ArrayOfType(dtEmbeddedToolbar, &aWorkers, TRUE, TRUE);

	int nWorkers = min(aWorkers.GetSize(),
		IDM_DOCKSHOW_LAST - IDM_DOCKSHOW_BASE + 1);

	if (nWorkers > 0)
	{
        CBMenuPopup menuPopup ;
		CDockWorker* pDocker;
		BOOL bSeparate = FALSE;

		for (int i = 0; i < nWorkers; i++)
		{
			pDocker = (CDockWorker*) aWorkers[i];
			if (pDocker->GetDock() == dpMDI)
				continue;

			// don't add the menu bar to the popup menu, unless we are in full screen mode
			if (LOWORD(((CDockWorker*) aWorkers[i])->m_nIDWnd) == IDTB_MENUBAR &&
				!IS_STATE_FULLSCREEN(DkGetDockState()))
				continue;

			// Separate windows from toolbars.
			if (LOWORD(((CDockWorker*) aWorkers[i])->m_nIDWnd) < IDTB_SHELL_BASE)
				bSeparate = TRUE;
			if (bSeparate && (LOWORD(((CDockWorker*) aWorkers[i])->m_nIDWnd) >= IDTB_SHELL_BASE))
			{
				menuPopup.AppendMenu(MF_SEPARATOR);
				bSeparate = FALSE;
			}

			pDocker->GetText(str);

			ASSERT(!str.IsEmpty());

			menuPopup.AppendMenu((pDocker->IsVisible() ? MF_CHECKED : MF_STRING),
				IDM_DOCKSHOW_BASE + i, str);
		}

		menuPopup.AppendMenu(MF_SEPARATOR);

		str.LoadString(IDS_POPUP_CUSTOMIZE);
		menuPopup.AppendMenu(CASBar::s_pCustomizer ? MF_GRAYED : MF_STRING,
			IDM_TOOLBAR_CUSTOMIZE, str);

		menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			pt.x, pt.y, m_pFrame, NULL);
	}
}

//	CDockManager::ArrayOfType
//		Given a DOCKTYPE, and a pointer to an existing ObArray, adds pointers
//		to all existing windows of the type to the array.

void CDockManager::ArrayOfType(DOCKTYPE dt, CObArray* paWorkers,
	BOOL bExclude /*=FALSE*/, BOOL bAvailable /*=FALSE*/, DOCKTYPE dtAnother /*=dtNil*/)
{
	if (bAvailable && m_bAvailableWndDirty)
		IdleUpdateAvailableWnds();

    POSITION pos = m_listWorker.GetHeadPosition();
    while (pos != NULL)
    {
        CDockWorker* pDocker = (CDockWorker*) m_listWorker.GetNext(pos);
        if ((!bAvailable || pDocker->IsAvailable()) &&
        	(dt == dtNil ||
        	(!bExclude && (pDocker->m_dtWnd == dt || pDocker->m_dtWnd == dtAnother) ) ||
        	(bExclude && (pDocker->m_dtWnd != dt && pDocker->m_dtWnd != dtAnother))))
		{
    		paWorkers->Add(pDocker);
		}
    }
}

void CDockManager::MapCmdToWorker(UINT nCmd, UINT nIDWnd)
{
	CDockWorker* pDocker = WorkerFromID(nIDWnd);
	if (pDocker != NULL)
		m_mapCmdToWorker.SetAt((WORD) nCmd, pDocker);
}

void CDockManager::HandleWorkerCmd(UINT nCmd, BOOL bToggle,
	CCmdUI* pCmdUI /*=NULL*/)
{
	CDockWorker* pDocker;
	if (!m_mapCmdToWorker.Lookup((WORD)nCmd, (CObject*&)pDocker))
	{
		if (pCmdUI != NULL)
			pCmdUI->Enable(FALSE);
	}
	else if (pCmdUI != NULL)
	{
		if (bToggle)
			pCmdUI->SetCheck(pDocker->IsVisible());
		pCmdUI->Enable(pDocker->IsAvailable());
	}
	else if (bToggle)
		pDocker->ShowWindow(!pDocker->IsVisible());
	else
		pDocker->ActivateView();
}

//	CDockManager::AddMDIChild
//		Adds a MDI child pointer to the palette list.

void CDockManager::AddMDIChild(CMDIChildDock* pMDIWnd)
{
    m_listDesk.AddHead(pMDIWnd);
}

//  CDockManager::RemoveMDIChild
//      Removes the specified MDI child from the palette list, setting its
//      child's parent to the hidden dock. (Also deletes the palette)

void CDockManager::RemoveMDIChild(CMDIChildDock* pMDIWnd,
	CWnd* pWndNewParent /*=NULL*/, CDockManager* pManagerNew /*=NULL*/)
{
    POSITION pos = m_listDesk.GetHeadPosition();
    while (pos != NULL)
    {
        CMDIChildDock* pDeskItem = (CMDIChildDock*) m_listDesk.GetAt(pos);

        if (pMDIWnd == pDeskItem)
        {
			if (pManagerNew != NULL)
			{
				pMDIWnd->SetManager(pManagerNew);
				pManagerNew->AddMDIChild(pMDIWnd);
			}
			else
			{
				if (pWndNewParent == NULL)
	            	pWndNewParent = GetDock(dpHide);
	            pMDIWnd->ParentChild(pWndNewParent);

	            pMDIWnd->DestroyWindow();  // Auto-deleting.
			}

            m_listDesk.RemoveAt(pos);
            break;
        }

        m_listDesk.GetNext(pos);
    }
}

//	CDockManager::AddPalette
//		Adds a palette pointer to the palette list.

void CDockManager::AddPalette(CPaletteWnd* pPalette)
{
    m_listFloat.AddHead(pPalette);
}

//  CDockManager::RemovePalette
//      Removes the specified palette from the palette list, setting its
//      child's parent to the hidden dock. (Also deletes the palette)

void CDockManager::RemovePalette(CPaletteWnd* pPalette,
	CWnd* pWndNewParent /*=NULL*/, CDockManager* pManagerNew /*=NULL*/)
{
    POSITION pos = m_listFloat.GetHeadPosition();
    while (pos != NULL)
    {
        CPaletteWnd* pPalItem = (CPaletteWnd*) m_listFloat.GetAt(pos);

        if (pPalette == pPalItem)
        {
			if (pManagerNew != NULL)
			{
				pPalette->SetManager(pManagerNew);
				pManagerNew->AddPalette(pPalette);
			}
			else
			{
	        	pPalette->ShowWindow(SW_HIDE);
	            CWnd* pChild = pPalette->GetTopWindow();
	            ASSERT(pChild != NULL);

				if (pWndNewParent == NULL)
	            	pWndNewParent = GetDock(dpHide);
	            pChild->SetParent(pWndNewParent);

	            pPalette->DestroyWindow();  // Auto-deleting.
			}

            m_listFloat.RemoveAt(pos);
            break;
        }

        m_listFloat.GetNext(pos);
    }
}

//	CDockManager::ActivatePalettes
//		Activates/Deactivates all palette captions.

void CDockManager::ActivatePalettes(BOOL bActive)
{
    POSITION pos = m_listFloat.GetHeadPosition();
    while (pos != NULL)
    {
        CPaletteWnd* pPalette = (CPaletteWnd*) m_listFloat.GetNext(pos);
        pPalette->ActivateWindow(bActive);
    }
}

//	CDockManager::EnableManagerInput
//		Notifies all worker windows to disable children for toolbar customize
//		mode.

void CDockManager::EnableManagerInput(BOOL bEnable)
{
    POSITION pos = m_listWorker.GetHeadPosition();
    while (pos != NULL)
    {
        CDockWorker* pDocker = (CDockWorker*) m_listWorker.GetNext(pos);

        if (pDocker->m_pWnd != NULL)
        	pDocker->m_pWnd->SendMessage(DWM_ENABLECHILD, bEnable);
    }
}

///////////////////////////////////////////////////////////
//
//	CDockManager::ShowManager
//		Hide/Show all manager windows -- palettes & docks.
//
// bForceUpdate -	Execute the function even if we don't
//					think is is needed
//
void CDockManager::ShowManager(BOOL bShow, UPDATE_SHOW update /*=updNow*/, BOOL bForceUpdate /*= FALSE*/)
{
	if (!bForceUpdate && // Bug 14790 --- CApplication::SetVisible needs to run this function even if the current value should be correct.
		((bShow && m_bVisible) || (!bShow && !m_bVisible)))
	{
		if (update == updDelay)
			m_bDelayedShow = FALSE;

		return;
	}
	else if (update == updDelay)
	{
		m_bDelayedShow = TRUE;
		return;
	}

	m_bVisible = bShow;

    for (int i = 0; i < dpCount - 1; i++)
    {
    	if (m_rgpDock[i]->GetSafeHwnd() != NULL)
        	m_rgpDock[i]->ShowWindow(bShow ? SW_SHOWNOACTIVATE : SW_HIDE);
    }

    if (update == updNow)
    {
		ASSERT(m_pFrame != NULL);

		if (!m_pFrame->IsWindowVisible() || m_pFrame->IsIconic())
		{
  			// User not watching. Recalc twice, since first recalc may
  			// cause row removal in OnWindowPosChanged.
			//
			m_pFrame->RecalcLayout();
			m_pFrame->RecalcLayout();
		}
    	else if (!bShow)
    	{
		    m_pFrame->RecalcLayout();
		    m_pFrame->UpdateWindow();
    	}
    	else
    	{
    		// Might involve resizing of stretchy windows, which we don't want
    		// the user to see.  Also we recalc twice, since first recalc may
			// cause row removal in OnWindowPosChanged.
			//
	    	m_pFrame->SetRedraw(FALSE);
		    m_pFrame->RecalcLayout();
			m_pFrame->RecalcLayout();
		    m_pFrame->SetRedraw(TRUE);
	        m_pFrame->RedrawWindow(NULL, NULL,
	            RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);

			// Sprites for active view won't be painted in the RedrawWindow.
			CSlobWnd* pWnd = GetActiveSlobWnd();
			if (pWnd != NULL)
				pWnd->ObscureSprites();	// Force a redraw.
	    }
	}

	// If the frame is iconic, do not show the palettes.  We will
	// need to do this when the window is restored.
	
	if (!m_pFrame->IsIconic() 
		&& !theApp.m_bRunInvisibly) // Bug 14790 --- We don't want the floating windows appearing if the app is automated.
	{
		//Orion Bug #
		ShowPalettes(bShow, bForceUpdate);
	}
}

///////////////////////////////////////////////////////////
//
//	ShowPalettes -	Hides or shows the floating versions
//					of the docking windows.
//
// bForceUpdate -	Execute the function even if we don't
//					think is is needed
//
void CDockManager::ShowPalettes(BOOL bShow, BOOL bForceUpdate /*= FALSE*/)
{
	if (!bForceUpdate && // Bug 14790 --- CApplication::SetVisible needs to run this function even if the current value should be correct.
		((m_bVisible && !bShow) || (!m_bVisible && bShow)))
	{
		return;
	}

	// Show/hide the palettes.
	const CWnd* pWndZOrder = &CWnd::wndTop;
	if (PwndSplashScreen() != NULL)
		pWndZOrder = PwndSplashScreen();

    POSITION pos = m_listWorker.GetHeadPosition();
    while (pos != NULL)
    {
        CDockWorker* pDocker = (CDockWorker*) m_listWorker.GetNext(pos);
        if (pDocker->GetDock() == dpNil && pDocker->IsVisible())
        {
        	ASSERT(pDocker->m_pWnd != NULL);
        	ASSERT(pDocker->m_pWnd->GetParent() != NULL);

        	CWnd* pPalette = pDocker->m_pWnd->GetParent();
        	pPalette->SetWindowPos(pWndZOrder, 0, 0, 0, 0,
        		(bShow ? SWP_SHOWWINDOW : (SWP_HIDEWINDOW | SWP_NOZORDER)) |
        		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	        pPalette->UpdateWindow();

			pWndZOrder = pPalette;
		}
    }
}

BOOL CDockManager::DocObjectMode(BOOL bShow)
{
	if(!bShow == !m_bInDocObjectMode)
		return bShow;

	m_bInDocObjectMode = bShow;
	m_bAvailableWndDirty = TRUE; //Force update.

	if(bShow)	// If we're switching to a Doc Object
		((CMainFrame *)AfxGetMainWnd())->SaveManager();	// Save the current layout
	return !bShow; // Return value of m_bInDocObjectMode before the call
}

//  CDockManager::IdleUpdate
//      Updates command UIs for floating windows.

void CDockManager::IdleUpdate()
{
    POSITION pos = m_listFloat.GetHeadPosition();
    while (pos != NULL)
    {
        ((CPaletteWnd*) m_listFloat.GetNext(pos))->
            SendMessageToDescendants(WM_IDLEUPDATECMDUI,
				(WPARAM)TRUE, 0, TRUE, TRUE);
    }
}

//	CDockManager::UpdateAvailableWnds
//		Called at beginning of idle loop to hide/show windows that have
//		changed availability.

void CDockManager::IdleUpdateAvailableWnds()
{

	// These tests are duplicated in CDockManager::NeedIdle, we have to make sure that
	// they remain in ssync so that the Idle loop processing can remain efficient
	if (m_pFrame == NULL || m_pFrame->IsIconic())
		return;

	if (m_bAvailableWndDirty)
	{
		CMainFrame *pFrame=((CMainFrame *)theApp.m_pMainWnd);

		BOOL bLocked=pFrame->m_bLockLayout;
		((CMainFrame *)theApp.m_pMainWnd)->LockLayout(TRUE);

		m_bAvailableWndDirty = FALSE;

		// Hide unavailable LastAvailable windows first.
	    POSITION pos = m_listWorker.GetHeadPosition();
	    while (pos != NULL)
	    {
	        CDockWorker* pDocker = (CDockWorker*) m_listWorker.GetNext(pos);

			if (pDocker->IsLastAvailable())
	        	pDocker->UpdateAvailable(FALSE);
	    }

		// Hide all other unavailable windows first; then show available ones.
		for (int i = 0; i < 2; i++)
		{
		    pos = m_listWorker.GetHeadPosition();
		    while (pos != NULL)
		    {
		        CDockWorker* pDocker = (CDockWorker*) m_listWorker.GetNext(pos);

				if (!pDocker->IsLastAvailable())
			        pDocker->UpdateAvailable(i);
		    }
		}

		// Show available LastAvailable windows last, and in reverse order.
	    pos = m_listWorker.GetTailPosition();
	    while (pos != NULL)
	    {
	        CDockWorker* pDocker = (CDockWorker*) m_listWorker.GetPrev(pos);

			if (pDocker->IsLastAvailable())
	        	pDocker->UpdateAvailable(TRUE);
	    }

		// restore lock status
		((CMainFrame *)theApp.m_pMainWnd)->LockLayout(bLocked);
		if(!bLocked)
		{
			// if we locked it, we must force the repaint
			((CMainFrame *)theApp.m_pMainWnd)->RecalcLayout();
		}

	}

	// These tests are duplicated in CDockManager::NeedIdle, we have to make sure that
	// they remain in ssync so that the Idle loop processing can remain efficient
	if (m_bDelayedShow)
	{
		m_bDelayedShow = FALSE;
		ShowManager(!m_bVisible);
	}
}

//  CDockManager::SysColorChange
//      Updates colors for floating windows.

void CDockManager::SysColorChange()
{
    POSITION pos = m_listFloat.GetHeadPosition();
    while (pos != NULL)
    {
        ((CPaletteWnd*) m_listFloat.GetNext(pos))->
			SendMessageToDescendants(WM_SYSCOLORCHANGE, 0, 0L, TRUE, TRUE);
    }
}

