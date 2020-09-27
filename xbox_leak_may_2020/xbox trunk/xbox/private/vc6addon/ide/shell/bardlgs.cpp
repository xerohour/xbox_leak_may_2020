///////////////////////////////////////////////////////////////////////////////
//  BARDLGS.CPP
//      Contains implementations for the major classes relavant to Sushi
//              toolbars CToolbarDialog, and CCustomDialog.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bardockx.h"
#include "bardlgs.h"
#include "resource.h"
#include "barcust.h"
#include "menuprxy.h"
#include "prxycust.h"
#include "btnctl.h"
#include "customiz.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolbarDialog dialog
//              Toolbar management dialog, for hiding/showing docked windows
//              ("toolbars"), reseting/deleting toolbars, creating new custom
//              toolbars.

//      CToolbarCheckList::SetCheck
//              Override CCheckList for check notification to the dialog.

void CToolbarCheckList::SetCheck(int nItem, BOOL bCheck)
{
	// Allow parent to veto action
	CToolbarDialog* pParent = (CToolbarDialog*) GetParent();

	CDockWorker* pDocker = pParent->GetWorker(nItem);
	
	// if it's the menu bar, and we are not full screen, then you can't hide it
	if (LOWORD(pDocker->m_nIDWnd) == IDTB_MENUBAR &&
		!IS_STATE_FULLSCREEN(DkGetDockState()) &&
		bCheck==FALSE)
		return;

	CCheckList::SetCheck(nItem, bCheck);
	UpdateWindow();

	ASSERT(pParent != NULL);
	pParent->UpdateVisible(nItem);
}

IMPLEMENT_DYNAMIC(CToolbarDialog, CDlgTab)

//      CToolbarDialog::CToolbarDialog
//              Construction.  Creates a CToolCustomizer.  

CToolbarDialog::CToolbarDialog(CDockManager* pManager, CWnd* pParent /*=NULL*/)
	: CDlgTab(CToolbarDialog::IDD, IDS_TOOLBARS)
{
	m_pManager = pManager;
	LockManager(TRUE);

	m_nSel = LB_ERR;
	//{{AFX_DATA_INIT(CToolbarDialog)
	m_strName = "";
	m_bToolTips = theApp.m_bToolTips;
	m_bKeys = theApp.m_bToolTipKeys;
	m_bLargeToolBars = theApp.m_bLargeToolBars;
	//}}AFX_DATA_INIT
}

CToolbarDialog::~CToolbarDialog()
{
	LockManager(FALSE);
}

BOOL CToolbarDialog::Create()
{
	return CDlgTab::Create(MAKEINTRESOURCE(CToolbarDialog::IDD), m_pParentWnd);
}

void CToolbarDialog::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolbarDialog)
	DDX_Control(pDX, IDC_NAME_TEXT, m_statName);
	DDX_Control(pDX, IDC_NAME_EDIT, m_edName);
	DDX_Control(pDX, IDC_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_RESET, m_btnReset);
	DDX_Control(pDX, IDC_RESET_ALL_BARS, m_btnResetAllBars);
	DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
	DDV_MaxChars(pDX, m_strName, 100);
	DDX_Check(pDX, IDC_TOOLTIPS, m_bToolTips);
	DDX_Check(pDX, IDC_KEYS, m_bKeys);
	DDX_Check(pDX, IDC_LARGETOOLBARS, m_bLargeToolBars);
	//}}AFX_DATA_MAP
}

//      CToolbarDialog::LockManager
//              Make sure the CDockManager that we reference does not go away.

void CToolbarDialog::LockManager(BOOL bLock)
{
	CMainFrame* pFrame = (CMainFrame*) m_pManager->m_pFrame;
	ASSERT(pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

	pFrame->LockDockState(bLock);
}

//      CToolbarDialog::UpdateVisible
//              For real time hide/show of toolbars in synch with checkboxes.

BOOL CToolbarDialog::UpdateVisible(int nItem)
{
	ASSERT(nItem >= 0 && nItem < m_aToolWorkers.GetSize());
	
	CDockWorker* pDocker = GetWorker(nItem);
	
	// Low overhead if check and visible same
	pDocker->ShowWindow(m_lbToolbars.GetCheck(nItem));

	return TRUE;
}

//      CToolbarDialog::UpdateCheck
//              Called in CDockWorker::ShowWindow to keep check list in synch with
//              user actions hide/showing toolbars.

void CToolbarDialog::UpdateCheck(UINT nIDWnd)
{
	int nBars = m_aToolWorkers.GetSize();
	CDockWorker* pDocker;
	
	for (int i = 0; i < nBars; i++)
	{
		pDocker = GetWorker(i);
		
		if (pDocker->m_nIDWnd == nIDWnd)
			m_lbToolbars.SetCheck(i, pDocker->IsVisible());
	}
}

//      CToolbarDialog::AddToolbar
//              Allows adding of toolbar to the list in case user customization creates
//              new toolbar.

void CToolbarDialog::AddToolbar(CDockWorker* pDocker)
{
	m_aToolWorkers.Add(pDocker);
	
	CString str;
	pDocker->GetText(str);
	int nBar = m_lbToolbars.AddString(str);
	
	if (nBar != LB_ERR)
		m_lbToolbars.SetCheck(nBar, pDocker->IsVisible());
}

//      CToolbarDialog::RemoveToolbar
//              Allows adding of toolbar to the list in case user customization creates
//              new toolbar.

void CToolbarDialog::RemoveToolbar(CDockWorker* pDocker)
{
	int nCount = m_aToolWorkers.GetSize();
	for (int i = 0; i < nCount && m_aToolWorkers[i] != pDocker; i++)
		;

	if (i < nCount)
	{
		m_aToolWorkers.RemoveAt(i);
		m_lbToolbars.DeleteString(i);

		if (m_lbToolbars.SetCurSel(m_nSel) == LB_ERR)
		{
			m_lbToolbars.SetCurSel(m_nSel - 1);
		}
		
		m_nSel = LB_ERR;
		
		OnSelChangeToolbar();
	}
}

BEGIN_MESSAGE_MAP(CToolbarDialog, CDlgTab)
	//{{AFX_MSG_MAP(CToolbarDialog)
	ON_BN_CLICKED(IDC_DELETE, OnClickedDelete)
	ON_BN_CLICKED(IDC_RESET, OnClickedReset)
	ON_BN_CLICKED(IDC_RESET_ALL_BARS, OnClickedResetAllBars)
	ON_BN_CLICKED(IDC_NEW, OnClickedNew)
	ON_BN_CLICKED(IDC_TOOLTIPS, OnClickedToolTips)
	ON_BN_CLICKED(IDC_KEYS, OnClickedKeys)
	ON_LBN_SELCHANGE(IDC_TOOLBAR, OnSelChangeToolbar)
	ON_WM_SYSCOMMAND()
	ON_WM_ENABLE()
	ON_BN_CLICKED(IDC_LARGETOOLBARS, OnClickedLargeToolBars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolbarDialog message handlers

//      CToolbarDialog::OnInitDialog
//              Initializes list, and calls OnSelChangeToolbar to init selection
//              dependant states.

BOOL CToolbarDialog::OnInitDialog()
{
	// Initialize listbox first to avoid flashing.
	CDlgTab::OnInitDialog();
	
	VERIFY(m_lbToolbars.SubclassDlgItem(IDC_TOOLBAR, this));

	if (!m_bToolTips)
		GetDlgItem(IDC_KEYS)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CToolbarDialog::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	if (!CDlgTab::Activate(pParentWnd, position))
		return FALSE;

	// Initialize the listbox and corresponding worker array.
	m_aToolWorkers.RemoveAll();
	m_pManager->ArrayOfType(dtEdit, &m_aToolWorkers, TRUE, TRUE, dtEmbeddedToolbar);

	m_lbToolbars.ResetContent();
	m_lbToolbars.SetRedraw(FALSE);
	
	CString str;
	int nBars = m_aToolWorkers.GetSize();
	CDockWorker* pDocker;
	
	int iBar = 0;
	for (int i = 0; i < nBars; i++)
	{
		pDocker = GetWorker(i);

		if (!pDocker->IsAvailable())
			m_aToolWorkers.RemoveAt(iBar);
		else
		{
			pDocker->GetText(str);
			m_lbToolbars.AddString(str);
			m_lbToolbars.SetCheck(iBar, pDocker->IsVisible());
			iBar++;
		}
	}
	
	m_lbToolbars.SetRedraw(TRUE);

	m_lbToolbars.SetCurSel(0);
	
	OnSelChangeToolbar();

	theApp.ActivateDraggableCustomizePage(this);

	return TRUE;
}

void CToolbarDialog::Deactivate(CTabbedDialog* pParentWnd)
{
	UpdateData();

	CDockWorker* pDocker = GetWorker(m_nSel);
	if (!m_strName.IsEmpty() && pDocker != NULL)
		pDocker->SetText(m_strName);
	
	CDlgTab::Deactivate(pParentWnd);

	theApp.DeactivateDraggableCustomizePage(pParentWnd);

}

//      CToolbarDialog::OnClickedNew
//              Allows the creation of a new toolbar, bringing up the New
//              Toolbar dialog.

void CToolbarDialog::OnClickedNew()
{
	CNewBarDialog dlg(this);
	
	if (dlg.DoModal() == IDOK)
	{
		CASBar* pBar = new CASBar;
		if (!pBar->Create(m_pManager, TBB_NIL, NULL, dlg.m_strName))
		{
			delete pBar;
			AfxMessageBox (IDS_ERROR_TOOLBAR_CREATE);
			return;
		}
	}
}

//      CToolbarDialog::OnClickedDelete
//              Deletes the toolbar, if it is a custom toolbar.

void CToolbarDialog::OnClickedDelete()
{
	CDockWorker* pDocker = GetWorker(m_nSel);
	
	if (LOWORD(pDocker->m_nIDWnd) < IDTB_SHELL_BASE)
		return;
	
	if (pDocker->m_nIDPackage == PACKAGE_SUSHI &&
		LOWORD(pDocker->m_nIDWnd) >= IDTB_CUSTOM_BASE)
	{
		// Delete the toolbar.
		ASSERT(CASBar::s_pCustomizer != NULL);
		if (pDocker->m_pWnd == CASBar::s_pCustomizer->m_pSelectBar->GetRealObject())
			CASBar::s_pCustomizer->SetSelection(NULL, 0);
			
		// RemoveWnd calls our RemoveBar function to update the dialog.
		m_pManager->RemoveWnd(pDocker->m_nIDWnd);
	}
}

// resets the nIndex bar in the list
void CToolbarDialog::ResetBar(int nIndex)
{
	CDockWorker* pDocker = GetWorker(nIndex);
	
	if (LOWORD(pDocker->m_nIDWnd) < IDTB_SHELL_BASE)
		return;
	
	if (pDocker->m_nIDPackage != PACKAGE_SUSHI ||
		LOWORD(pDocker->m_nIDWnd) < IDTB_CUSTOM_BASE)
	{
		// Reset the toolbar to the standard.

		CASBar* pBar = (CASBar*) pDocker->m_pWnd;
		if (pBar != NULL)
		{
			pBar->Reset();
		}
	}
}

//      CToolbarDialog::OnClickedResetAllBars
//              Resets all toolbars
void CToolbarDialog::OnClickedResetAllBars()
{
	int nBars=m_lbToolbars.GetCount();

	for(int iBar=0; iBar<nBars; ++iBar)
	{
		ResetBar(iBar);
	}
}

//      CToolbarDialog::OnClickedReset
//              Resets the toolbar
void CToolbarDialog::OnClickedReset()
{
	ResetBar(m_nSel);
}

void CToolbarDialog::OnClickedToolTips()
{
	UpdateData();
	theApp.m_bToolTips = m_bToolTips;
	GetDlgItem(IDC_KEYS)->EnableWindow(m_bToolTips);
}

void CToolbarDialog::OnClickedKeys()
{
	UpdateData();
	theApp.m_bToolTipKeys = m_bKeys;
}

// fixes bug in SDK headers. This function declaration was missing
#undef BroadcastSystemMessage
extern "C" WINUSERAPI long  WINAPI  BroadcastSystemMessage(DWORD, LPDWORD, UINT, WPARAM, LPARAM);

void CToolbarDialog::OnClickedLargeToolBars() 
{
	// update the dialog information
	UpdateData();
	if(theApp.m_bLargeToolBars!=m_bLargeToolBars) {
		theApp.m_bLargeToolBars = m_bLargeToolBars;

		CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;
		pMainWnd->SetRedraw(FALSE);

		DWORD dwDestination=BSM_APPLICATIONS;

		// NOTE: This function is ripe for cleanup.

		// notify others that bar sizes are changing
		// Orion Bug #: 14130 - BroadcastSystemMessage crashes on Win95. So don't use it.
		//BroadcastSystemMessage(BSF_NOHANG	| BSF_NOTIMEOUTIFNOTHUNG, &dwDestination ,DSM_BARSIZECHANGING,theApp.m_bLargeToolBars,_getpid());
		pMainWnd->SendMessage(DSM_BARSIZECHANGING,theApp.m_bLargeToolBars,_getpid());

		// Also need to notify all of the windows associated with a CDockWorker.
		// This is needed so that the Controls toolbar used by the Dialog editor is updated.

		CObArray aWorkers ;
		CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();
		pFrame->m_pManager->ArrayOfType(dtNil, &aWorkers);

		int nViews = aWorkers.GetSize();
		CDockWorker* pDocker;
		for (int i = 0; i < nViews; i++)
		{
			pDocker = (CDockWorker*) aWorkers.GetAt(i);
			if (pDocker!= NULL && pDocker->m_pWnd != NULL)
			{
				pDocker->m_pWnd->SendMessageToDescendants(DSM_BARSIZECHANGING, theApp.m_bLargeToolBars, _getpid(), TRUE, FALSE); 
			}
		}

		// now cycle thru the visible toolbars
		int barIndex=0;
		while(barIndex<CASBar::s_aUsage.GetSize()) {
			CASBar *pBar=(CASBar *)(CASBar::s_aUsage[barIndex]);
		
			ASSERT(pBar!=NULL);

			pBar->SetButtonSize(m_bLargeToolBars);

			++barIndex;
		}

		pMainWnd->SetRedraw(TRUE);
	    pMainWnd->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
	}
}

//      CToolbarDialog::OnSelChangeToolbar
//              Handles updating the name edit control (and name string), and the
//              Delete/reset text.

void CToolbarDialog::OnSelChangeToolbar()
{
	int nCurSel = m_lbToolbars.GetCurSel();
	
	CDockWorker* pDockerOld = GetWorker(m_nSel);
	CDockWorker* pDocker = GetWorker(nCurSel);
	
	if (pDockerOld != NULL)
	{
		// Set the CDockWorker text.
		CString strOldTitle;
		pDockerOld->GetText(strOldTitle);
		UpdateData();
		
		if (!m_strName.IsEmpty() && strOldTitle != m_strName)
		{
			pDockerOld->SetText(m_strName);
			
			BOOL bCheckOld = m_lbToolbars.GetCheck(m_nSel);
			m_lbToolbars.SetRedraw(FALSE);
			m_lbToolbars.DeleteString(m_nSel);
			m_lbToolbars.InsertString(m_nSel, m_strName);
			m_lbToolbars.SetCheck(m_nSel, bCheckOld);
			m_lbToolbars.SetCurSel(nCurSel);
			m_lbToolbars.SetRedraw(TRUE);
		}
	}
	
	m_nSel = nCurSel;

	BOOL bEnableDelete=FALSE;
	BOOL bEnableReset=FALSE;
		
	// Update the edit control text, and delete/reset button.
	if (pDocker == NULL)
	{
		m_strName.Empty();
	}
	else
	{
		pDocker->GetText(m_strName);
	
		BOOL bCustom = (pDocker->m_nIDPackage == PACKAGE_SUSHI &&
			LOWORD(pDocker->m_nIDWnd) >= IDTB_CUSTOM_BASE);
		m_statName.EnableWindow(bCustom);
		m_edName.EnableWindow(bCustom);
		if(LOWORD(pDocker->m_nIDWnd) >= IDTB_SHELL_BASE)
		{
			if (bCustom)
			{
				bEnableDelete=TRUE;
			}
			else
			{
				bEnableReset=TRUE;
			}
		}
	}

	// ensure that the default button isn't disabled
	DWORD def=GetDefID();
	BOOL bMoveDef=FALSE;
	if(HIWORD(def)==DC_HASDEFID)
	{
		int id=LOWORD(def);

		if((id==IDC_DELETE && !bEnableDelete) ||
			(id==IDC_RESET && !bEnableReset))
		{
			// make the new button the default
			SetDefID(IDC_NEW);
			m_lbToolbars.SetFocus();
		}
	}

	m_btnDelete.EnableWindow(bEnableDelete);
	m_btnReset.EnableWindow(bEnableReset);

	UpdateData(FALSE);
}

//      CToolbarDialog::OnSysCommand
//              Disallow switching to the main window.

void CToolbarDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
    switch (nID & 0xfff0)
    {
    case SC_PREVWINDOW:
    case SC_NEXTWINDOW:
	if (LOWORD(lParam) == VK_F6)
		return;
	break;
    }
	
	CDlgTab::OnSysCommand(nID, lParam);
}

//      CToolbarDialog::OnEnable
//              Enable/disable the main window when we get enabled/disabled.

void CToolbarDialog::OnEnable(BOOL bEnable)
{
	CDlgTab::OnEnable(bEnable);

	ASSERT(m_pManager->m_pFrame != NULL);
	m_pManager->m_pFrame->EnableWindow(bEnable);
}

//      CToolbarDialog::GetWorker
//      Gets the CDockWorker for the given index from m_aToolWorkers, or for
//              the current selection if no index is given.

CDockWorker* CToolbarDialog::GetWorker(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aToolWorkers.GetSize())
		return NULL;
	else
		return (CDockWorker*) m_aToolWorkers.GetAt(nIndex);
}

/////////////////////////////////////////////////////////////////////////////
// CNewBarDialog dialog
//              Allows user to name a new toolbar.  Brought up via the New button
//              in the CToolbarDialog.

//      CNewBarDialog::CNewBarDialog
//              Cunstruction.

CNewBarDialog::CNewBarDialog(CWnd* pParent /*=NULL*/)
	: C3dDialog(CNewBarDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewBarDialog)
	m_strName = "";
	//}}AFX_DATA_INIT
}

void CNewBarDialog::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewBarDialog)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
	DDV_MaxChars(pDX, m_strName, _MAX_DOCK_CAPTION);
}

BEGIN_MESSAGE_MAP(CNewBarDialog, C3dDialog)
	//{{AFX_MSG_MAP(CNewBarDialog)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewBarDialog message handlers

//      CNewBarDialog::OnChangedName
//              Do not allow unnamed toolbars.

void CNewBarDialog::OnChangeName()
{
	UpdateData();
	m_btnOK.EnableWindow(!m_strName.IsEmpty());
}

//      CToolbarDialog::OnOK
//              Ends modeless dialog.

void CNewBarDialog::OnOK()
{
	UpdateData();

	CDockManager* pManager = ((CMainFrame *)AfxGetMainWnd())->m_pManager;

	CObArray aWorkers;
	pManager->ArrayOfType(dtNil, &aWorkers, FALSE, TRUE);

	// check for duplicates with existing docking windows
	CDockWorker* pDocker;
	CString str;

	int nWorkers=aWorkers.GetSize();

	for (int i = 0; i < nWorkers; i++)
	{
		pDocker = (CDockWorker*) aWorkers[i];

		if (LOWORD(((CDockWorker*) aWorkers[i])->m_nIDWnd) < IDTB_SHELL_BASE)
			continue;

		pDocker->GetText(str);

		if(str.CompareNoCase(m_strName)==0)
		{
			// duplicates existing bar name
			AfxMessageBox(IDS_ERROR_DUPLICATE_TOOLBAR_NAME, MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}

	C3dDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CCustomDialog dialog
//              Displays available controls for customizing, allowing the user
//              to drag displayed controls onto toolbars.

IMPLEMENT_DYNAMIC(CCustomDialog, CCommandTab)

//      CCustomDialog::CCustomDialog
//              Construction.  Creates a CToolCustomizer. 

CCustomDialog::CCustomDialog(CDockManager* pManager, CWnd* pParent /*=NULL*/)
	: CCommandTab(CCustomDialog::IDD, IDS_COMMANDS)
{
	m_pManager = pManager;
	LockManager(TRUE);

	m_pGroups=NULL;
	m_nSel = LB_ERR;
	m_lfCurrent=cListFillNone;
	m_iTextListBase=-1;
	//{{AFX_DATA_INIT(CCustomDialog)
	//}}AFX_DATA_INIT
}

CCustomDialog::~CCustomDialog()
{
	if(m_pGroups)
	{
		CAppToolGroups::ReleaseAppToolGroups();
	}

	DestroyToolbars();

	LockManager(FALSE);

	// These won't be referenced until customization begins (if ever), so they're just wasted memory
	theApp.UnloadPackageBitmaps();
}

void CCustomDialog::DestroyToolbars(void)
{
	int nCount = m_aToolbars.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		if(m_aToolbars[i])
		{
			((CASBar*) m_aToolbars[i])->DestroyWindow();    // Auto-deleting.
		}
	}

	m_aToolbars.RemoveAll();

	if(m_cbCategories.GetSafeHwnd())
	{
		m_cbCategories.ResetContent();
	}

	m_aIds.RemoveAll();
}

BOOL CCustomDialog::Create()
{
	return CCommandTab::Create(CCustomDialog::IDD, m_pParentWnd);
}

void CCustomDialog::DoDataExchange(CDataExchange* pDX)
{
	CCommandTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomDialog)
	DDX_Control(pDX, IDC_TEXTLIST, m_lbTextList);
	DDX_Control(pDX, IDC_CATEGORIES_COMBO, m_cbCategories);
	DDX_Control(pDX, IDC_DESCRIPTION, m_statDescription);
	DDX_Control(pDX, IDC_TOOLS, m_statTools);
	DDX_Control(pDX, IDC_RESET_ALL_MENUS, m_btnResetAllMenus);
	DDX_Control(pDX, IDC_MENU_FILTER, m_cbMenuFilter);
	//}}AFX_DATA_MAP
}

//      CCustomDialog::LockManager
//              Make sure the CDockManager that we reference does not go away.

void CCustomDialog::LockManager(BOOL bLock)
{
	CMainFrame* pFrame = (CMainFrame*) m_pManager->m_pFrame;
	ASSERT(pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

	pFrame->LockDockState(bLock);
}

//      CCustomDialog::SetDescriptionText
//              For displaying the status line of the selected button.

void CCustomDialog::SetDescriptionText(UINT nID)
{
	CString strPrompt;
	LPCTSTR lpsz = NULL;
	if (nID != 0)
	{
		BOOL fRet = theCmdCache.GetCommandString(nID, STRING_PROMPT, &lpsz);
		if (fRet)
			strPrompt = lpsz;
		else
			TRACE1("No String for ID = 0x%x.\n", nID);
	}

	m_statDescription.SetWindowText(strPrompt);
}

BEGIN_MESSAGE_MAP(CCustomDialog, CCommandTab)
	//{{AFX_MSG_MAP(CCustomDialog)
	ON_CBN_SELCHANGE(IDC_CATEGORIES_COMBO, OnSelChangeCategories)
	ON_CBN_SELCHANGE(IDC_MENU_FILTER, OnSelChangeMenuFilter)
	ON_WM_SYSCOMMAND()
	ON_REGISTERED_MESSAGE(DSM_BARSELECTCHANGING, OnBarSelectChanging)
	ON_REGISTERED_MESSAGE(DSM_COMMANDREMOVED, OnCommandRemoved)
	ON_LBN_SELCHANGE(IDC_TEXTLIST, OnSelChangeTextList)
	ON_BN_CLICKED(IDC_RESET_ALL_MENUS, OnClickedResetAllMenus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCustomDialog::CreateToolbars(void)
{
	CStringArray rgTitles;

	// REVIEW: Optimize, somehow.
	m_pGroups->CreateCustomizeToolbars(this, m_pManager, &m_aToolbars, &rgTitles, &m_aIds);
		
	CRect rectTools;
	m_statTools.GetWindowRect(rectTools);
	ScreenToClient(rectTools);
	
	CString strTitle;
	CASBar* pBar;
	int nCount = m_aToolbars.GetSize();

	for (int i = 0; i < nCount; i++)
	{
		pBar = GetToolbar(i);
		if(pBar)
		{
			pBar->SetParent(this);
			pBar->SetWindowPos(&wndTop, rectTools.left, rectTools.top,
				rectTools.Width(), rectTools.Height(), SWP_NOACTIVATE);

			// if relevant, set up scroll bar info
			SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS | SIF_RANGE;
			si.nMin=si.nMax=0;
			si.nPos=0;
			si.nTrackPos=0;

			CSize statSize=rectTools.Size();
			statSize.cx-=::GetSystemMetrics(SM_CXVSCROLL);
			CSize barFit=pBar->GetSize(HTRIGHT, statSize);

			if(rectTools.Size().cy < barFit.cy) {
				// scroll bar will be required
				si.nMax=barFit.cy-rectTools.Size().cy;
			}

			pBar->SetScrollInfo(SB_VERT,&si);
			
		}
		m_cbCategories.AddString(rgTitles[i]);
	}

	ASSERT(IDS_ALLCOMMANDS+1==IDS_DELETEDCOMMANDS);
	ASSERT(IDS_DELETEDCOMMANDS+1==IDS_MENUS);
	ASSERT(IDS_MENUS+1==IDS_NEWMENU);

	// point at first text entry
	m_iTextListBase=m_cbCategories.GetCount();

	// add the textual entries
	CString textList;
	for(listFill lf=cListFillFirst; lf< cListFillFirst+cListFillCount; lf=(listFill)((int)lf+1))
	{
		textList.LoadString(cTextListStringBase+lf-cListFillFirst);
		m_cbCategories.AddString(textList);
	}

	//hide the all commands list
	m_lbTextList.ShowWindow(SW_HIDE);

	m_cbCategories.SetCurSel(0);
	OnSelChangeCategories();

}

/////////////////////////////////////////////////////////////////////////////
// CCustomDialog message handlers

//      CCustomDialog::OnInitDialog
//              Create the custom toolbars to be displayed in this dialog, and
//              fill the list with their names.

BOOL CCustomDialog::OnInitDialog()
{
	CCommandTab::OnInitDialog();
	
	SetDescriptionText(0);

	ReloadMacros();
	m_pGroups  = CAppToolGroups::GetAppToolGroups(TRUE);

	// Fill the menu filters list
	ASSERT(IDS_MENUFILTER_CURRENT+1==IDS_MENUFILTER_ALL);

	CString strFilter;
	strFilter.LoadString(IDS_MENUFILTER_CURRENT);
	m_cbMenuFilter.AddString(strFilter);
	strFilter.LoadString(IDS_MENUFILTER_ALL);
	m_cbMenuFilter.AddString(strFilter);

	m_cbMenuFilter.SetCurSel(0);

	CreateToolbars();

	OnSelChangeMenuFilter();

	m_btnModifySelection.SubclassDlgItem(IDC_MODIFY, this);
	m_btnModifySelection.SetPopup(CToolCustomizer::GetButtonMenu());
	m_btnModifySelection.m_pMenuParent=AfxGetMainWnd();

	m_statDescription.SetFont(GetStdFont(font_Normal));
	GetDlgItem(IDC_INFORMATION)->SetFont(GetStdFont(font_Normal));

	OnBarSelectChanging(0,0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//      CCustomDialog::OnSelChangeCategories
//              Change the visible button group.
void CCustomDialog::OnSelChangeCategories()
{
	int nCurSel=m_cbCategories.GetCurSel();
	CWnd *pButtonGroup=GetDlgItem(IDC_BUTTONGROUP);
	CWnd *pCommands=GetDlgItem(IDC_COMMANDS_TEXT);

	ASSERT(pButtonGroup!=NULL);

	ASSERT(m_iTextListBase!=-1);

	if (m_nSel == nCurSel)
		return;

	// first show the new
	CASBar* pBar = GetToolbar(nCurSel);
	if (pBar != NULL)
	{
		pBar->ShowWindow(SW_SHOWNOACTIVATE);
	}
	else
	{
		// do we need to fill the list?
		listFill lfNew;
		if(nCurSel>=m_iTextListBase)
		{
			lfNew=(listFill) (nCurSel-(int)m_iTextListBase+cListFillFirst);
		}
		else
		{
			lfNew=(listFill)nCurSel;
		}
		if(lfNew!=m_lfCurrent)
		{
			m_lbTextList.SetRedraw(FALSE);
			m_lbTextList.ResetContent();

			switch(lfNew)
			{
				case cListFillCommands:
					theCmdCache.FillAllCommandsList(&m_lbTextList);
					break;

				case cListFillDeletedCommands:
					theCmdCache.FillDeletedCommandsList(&m_lbTextList);
					break;

				case cListFillMenus:
					theCmdCache.FillMenuList(&m_lbTextList);
					break;

				case cListFillNewMenu:
					theCmdCache.FillNewMenuList(&m_lbTextList);
					break;

				default:
					{
						// We must be a text category in the bars section, so fill
						for(int i=0;i<m_pGroups->m_nGroups; ++i)
						{
							if(m_pGroups->m_rgGroups[i].m_nId==m_aIds[nCurSel])
							{
								// found the group
								m_pGroups->m_rgGroups[i].FillCommandList(&m_lbTextList,TRUE);
								break;
							}
						}
					}
					break;
					
			}
			m_lbTextList.SetRedraw(TRUE);
			m_lbTextList.Invalidate();

			m_lfCurrent=lfNew;
		}
		m_lbTextList.SetCurSel(-1);
		OnSelChangeTextList();
		m_lbTextList.ShowWindow(SW_SHOWNOACTIVATE);
	}
	
	CASBar* pBarOld = GetToolbar(m_nSel);
	if (pBarOld != NULL)
	{
		pBarOld->ShowWindow(SW_HIDE);

		if(CASBar::s_pCustomizer)
		{
			// Clear customizer selection.
			if (CASBar::s_pCustomizer->m_pSelectBar &&
				pBarOld == CASBar::s_pCustomizer->m_pSelectBar->GetRealObject())
				CASBar::s_pCustomizer->SetSelection(NULL, 0);
		}

		if(pBar==NULL)
		{
			pButtonGroup->ShowWindow(SW_HIDE);
			pCommands->ShowWindow(SW_SHOWNOACTIVATE);
		}
	}
	else
	{
		// if we've moved from a text item to a button item, hide the list
		if(pBar!=NULL)
		{
			m_lbTextList.ShowWindow(SW_HIDE);
			pButtonGroup->ShowWindow(SW_SHOWNOACTIVATE);
			pCommands->ShowWindow(SW_HIDE);
		}
	}

	m_nSel = m_cbCategories.GetCurSel();
	SetDescriptionText(0);
}

//      CCustomDialog::OnSysCommand
//              Disallow switching to the main window.

void CCustomDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
    switch (nID & 0xfff0)
    {
    case SC_PREVWINDOW:
    case SC_NEXTWINDOW:
	if (LOWORD(lParam) == VK_F6)
	    return;
	break;
    }
	
	CCommandTab::OnSysCommand(nID, lParam);
}

LRESULT CCustomDialog::OnBarSelectChanging(WPARAM wParam, LPARAM lParam)
{
	// update description
	UINT nIndex=(UINT)wParam;
	CToolBarProxy* pBar= (CToolBarProxy*) lParam;

	UINT nID = 0;
	if (pBar != NULL)
	{
		UINT nStyle; int iImage;
		pBar->GetButtonInfo(nIndex, nID, nStyle, iImage);
	}

	SetDescriptionText(nID);

	// update availability of buttons
	BOOL bEnableChoose=FALSE;
	if (CASBar::s_pCustomizer!=NULL &&
		CASBar::s_pCustomizer->m_pSelectBar != NULL)
	{
		bEnableChoose=TRUE;
	}
	CButton *pChoose=(CButton *)GetDlgItem(IDC_MODIFY);
	ASSERT(pChoose!=NULL);
	pChoose->EnableWindow(bEnableChoose);

	return 0;
}

//      CCustomDialog::GetToolbar
//      Gets the CASBar for the given index from m_aToolbars, or for
//              the current selection if no index is given.
CASBar* CCustomDialog::GetToolbar(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aToolbars.GetSize())
		return NULL;
	else
		return (CASBar*) m_aToolbars[nIndex];
}

BOOL CCustomDialog::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	if (!CCommandTab::Activate(pParentWnd, position))
		return FALSE;

	// TRUE if a command list needs to be rebuilt
	BOOL bReList=FALSE;

	if (m_pGroups != NULL)
	{
		if (m_pGroups->ScanCommandCache())
		{
			DestroyToolbars();
			CreateToolbars();

			if(GetToolbar()==NULL)
			{
				bReList=TRUE;
			}
		}
	}

	theApp.ActivateDraggableCustomizePage(this);

	// If the deleted or all commands list are visible, refill them to allow for any 
	// customization that happened in other tabs
	if(bReList ||
		m_lfCurrent==cListFillCommands ||
		m_lfCurrent==cListFillDeletedCommands)
	{
		// fake change to force refill
		if(	m_lfCurrent==cListFillNewMenu)
		{
			m_lfCurrent=cListFillDeletedCommands;
		}
		else
		{
			m_lfCurrent=cListFillNewMenu;
		}
		m_nSel=cListFillNewMenu+m_iTextListBase-cListFillFirst;
		OnSelChangeCategories();
	}

	// If we're supposed to auto-select a command (launched from
	//  Tools.Macro), then do it now.  In the key customize dlg this
	//  is done in OnInitDialog.  In CCustomDlg, though, we must do
	//  this in Activate, since it must happen after the call to
	//  ActivateDraggableCustomizePage; otherwise, we'll assert
	//  in OnSelChangeCategories, that CASBar::s_pCustomizer is NULL.
	AutoSelectInitialCommand();

	OnSelChangeMenuFilter();

	return TRUE;
}

void CCustomDialog::AutoSelectInitialCommand()
{
	if (m_strInitialCommand.IsEmpty())
		return;

	CToolGroup* pToolGroup = m_pGroups->GroupFromCommandName(m_strInitialCommand);
	if (pToolGroup == NULL)
	{
		ASSERT(FALSE);
		m_strInitialCommand.Empty();
		return;
	}

	// Found the right group.  Select it in the list
	int nCategoryToSelect = 
		m_cbCategories.FindStringExact(-1, pToolGroup->m_strGroup);
	if (nCategoryToSelect == CB_ERR)
	{
		// The command's category isn't in the combo.  Can't be!
		ASSERT(FALSE);
		m_strInitialCommand.Empty();
		return;
	}

	m_cbCategories.SetCurSel(nCategoryToSelect);
	OnSelChangeCategories();

	if (GetToolbar(nCategoryToSelect) == NULL)
	{
		// Select the command passed in
		m_lbTextList.SelectString(-1, m_strInitialCommand);
		OnSelChangeTextList();
	}

	// Done with m_strInitialCommand, so empty it out
	m_strInitialCommand.Empty();
}

void CCustomDialog::Deactivate(CTabbedDialog* pParentWnd)
{
	CCommandTab::Deactivate(pParentWnd);

	theApp.DeactivateDraggableCustomizePage(pParentWnd);

	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
	// revert menus to their normal state
	pFrame->RebuildMenus();
}

// Update the command description text after the selection changes
void CCustomDialog::OnSelChangeTextList() 
{
	UINT nIndex=m_lbTextList.GetCurSel();
	
	// currently, we show no description for menus
	if(nIndex!=LB_ERR)
	{
		UINT nID = m_lbTextList.GetItemData(nIndex);

		SetDescriptionText(nID);

		CTE *pCTE=theCmdCache.GetCommandEntry(nID);
		if(pCTE)
		{
			if(pCTE->flags & CT_MENU)
			{
				CASBar::HideInvalidDestination(nID);
			}
		}
	}
	else
	{
		SetDescriptionText(0);
	}
}

// Update the menus after the menu filter is changed.
void CCustomDialog::OnSelChangeMenuFilter() 
{
	UINT nIndex=m_cbMenuFilter.GetCurSel();

	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();

	if(nIndex==cMenuFilterCurrent)
	{
		// hide transients, but show only for this editor
		pFrame->RebuildMenus(FALSE, TRUE, TRUE);
	}
	else
	{
		// hide transients, and show for all editors
		pFrame->RebuildMenus(TRUE, TRUE, TRUE);
	}
}

//      CCustomDialog::OnClickedResetAllMenus
//              Resets all menus
void CCustomDialog::OnClickedResetAllMenus()
{
	// ask mainframe to reset all menus
	CMainFrame*pFrame=(CMainFrame*)AfxGetMainWnd();

	pFrame->ResetAllMenus();

	if(	CASBar::s_pMenuBar)
	{
		CASBar::s_pMenuBar->Reset();
	}

	// get the menus rebuilt into customized form
	OnSelChangeMenuFilter();
}

// Called when a command was removed by customization
LRESULT CCustomDialog::OnCommandRemoved(WPARAM wParam, LPARAM lParam)
{
	// only add to the list if the current fill is deleted commands. Doesn't matter if the list is actually visible
	if(m_lfCurrent==cListFillDeletedCommands)
	{
		// This is called when a command is deleted
		UINT nIDCmd=(UINT)wParam;

		// get the command table entry for the missing command 
		CTE *pCTEMissing=theCmdCache.GetCommandEntry(nIDCmd);
		
		// Add the command to the list box, if it's not already in there
		LPCTSTR lpszCmdName = pCTEMissing->szCommand;
		int nOldIndex=m_lbTextList.FindStringExact(0, lpszCmdName);
		if(nOldIndex==LB_ERR)
		{
			int index = m_lbTextList.AddString(lpszCmdName);
			if(index!=LB_ERR)
			{
				m_lbTextList.SetItemData(index, (DWORD) pCTEMissing->id);
			}
		}
	}

	return 0;
}
