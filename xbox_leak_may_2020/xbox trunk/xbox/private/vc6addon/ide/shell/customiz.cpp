#include "stdafx.h"

#include "shell.h"
#include "bardlgs.h"
#include "keycust.h"
#include "workspc.h"
#include "toolcust.h"
#include "fcdialog.h"
#include <aut1api_.h>
#include <aut1gui_.h>
#include "customiz.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

class COptDlg : public CTabbedDialog
{
public:
	COptDlg(UINT nIDCaption, OPTDLG_TYPE type, UINT iSelectTab = -1, DLGPOSITION pos=POS_APPCENTER);
	~COptDlg();
	virtual void PostNcDestroy();
	// Make this public so CTheApp can access the command tabs to prime them
	//  with a command to auto-select (when launched from Tools.Macro)
	CDlgTab* GetTab(int nTab) { return CTabbedDialog::GetTab(nTab); }

	BOOL m_bModeless;
};

////////////////////////////////////////////////////////////////////////////

void CVshellPackage::OnCustomize()
{
	theApp.DoCustomizeDialog();
}

void CVshellPackage::OnToolbarCustomize()
{
	COptDlg* pDlg = new COptDlg(IDS_CUSTOMIZE, customize, 0, COptDlg::POS_AVOIDBARS);
	if (pDlg->GetTabCount() > 0)
	{
		AfxGetApp()->m_pMainWnd->EnableWindow(FALSE);
		pDlg->m_bModeless = TRUE;
		
		// ToolsCustomize is an asynchronous command.  The handler returns
		//  to the main message pump BEFORE the dialog is dismissed.  Inform
		//  the shell of this, so that automation calls to 
		//  Application::ExecuteCommand will wait until this dialog is dismissed
		//  before continuing.
		theApp.BeginAsyncCmd();

		pDlg->Create(); // self-deleting dialog
	}
}

void CVshellPackage::OnOptions()
{
	theApp.DoOptionsDialog();
}

void CTheApp::DoOptionsDialog(UINT nIDCaption /* = 0 */)
{
	COptDlg dlg(IDS_OPTIONS, options);

	if (dlg.GetTabCount() <= 0)
		return;

	// Caller may select a specific page by passing the string ID of the
	// page's caption string.  (We don't use m_nOrder because it's not
	// guaranteed to be unique)
	if (nIDCaption != 0)
	{
		CString strCaption;
		strCaption.LoadString(nIDCaption);
		ASSERT(!strCaption.IsEmpty());

		dlg.SelectTab(strCaption, TRUE);
	}

	dlg.DoModal();	// not self-deleting.
}

void CTheApp::DoCustomizeDialog(UINT nIDPageCaption /* = 0*/, 
								LPCTSTR szMacroName /*= NULL*/)
{
	COptDlg* pDlg = new COptDlg(IDS_CUSTOMIZE, customize, -1, COptDlg::POS_AVOIDBARS);
	if (pDlg->GetTabCount() > 0)
	{
		AfxGetApp()->m_pMainWnd->EnableWindow(FALSE);
		pDlg->m_bModeless = TRUE;

		// Caller may select a specific page by passing the string ID of the
		// page's caption string.  (We don't use m_nOrder because it's not
		// guaranteed to be unique)
		if (nIDPageCaption != 0)
		{
			CString strCaption;
			strCaption.LoadString(nIDPageCaption);
			ASSERT(!strCaption.IsEmpty());

			// If we're passed a command and the tab is primeable, then
			//  prime it with that command.  (This is done from Tools.Macro
			//  when the user wants to tie a selected macro to a
			//  toolbar or keystroke.)
			pDlg->SelectTab(strCaption, TRUE);
			int nTab = pDlg->GetCurrentTab();
			CDlgTab* pTab = pDlg->GetTab(nTab);
			if (szMacroName != NULL &&
				pTab->IsKindOf(RUNTIME_CLASS(CCommandTab)))
			{
				((CCommandTab*) pTab)->SelectCommand(szMacroName);
			}
		}

		// ToolsCustomize is an asynchronous command.  The handler returns
		//  to the main message pump BEFORE the dialog is dismissed.  Inform
		//  the shell of this, so that automation calls to 
		//  Application::ExecuteCommand will wait until this dialog is dismissed
		//  before continuing.
		theApp.BeginAsyncCmd();

		pDlg->Create(); // self-deleting dialog
	}
}

////////////////////////////////////////////////////////////////////////////

COptDlg::COptDlg(UINT nIDCaption, OPTDLG_TYPE type, UINT iSelectTab /*=-1*/, DLGPOSITION dlgpos)
	: CTabbedDialog(nIDCaption, NULL, iSelectTab,
	                type == options ? commitOnOk : commitOnTheFly, dlgpos)
{
	POSITION pos = theApp.m_packages.GetHeadPosition();
	while (pos != NULL)
	{
		CPackage* pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
		pPackage->AddOptDlgPages(this, type);
	}
	
	m_bModeless = FALSE;
}

COptDlg::~COptDlg()
{
}

void COptDlg::PostNcDestroy()
{
	if (m_bModeless)
	{
		AfxGetApp()->m_pMainWnd->EnableWindow(TRUE);
		AfxGetApp()->m_pMainWnd->SetFocus();
		delete this;

		// Now that the dialog is dismissed, tell the shell that this
		//  asynchronous command is comleted, so that if this was called
		//  via Application::ExecuteCommand, then ExecuteCommand can now
		//  return to its caller (e.g., a VBS macro).
		theApp.EndAsyncCmd();
	}
}

////////////////////////////////////////////////////////////////////////////
// CCommandTab mini-class

IMPLEMENT_DYNAMIC(CCommandTab, CDlgTab)

// Shared between the Commands and the Keyboard tabs of Tools.Customize.
//  Caches pointer to devaut1's IAut1Misc.  Created when the first tab
//  needs it, and released when the first tab is destroyed.
IAut1Misc* CCommandTab::m_pAut1Misc = NULL;

CCommandTab::~CCommandTab()
{
	if (m_pAut1Misc != NULL)
	{
		m_pAut1Misc->Release();
		m_pAut1Misc = NULL;
	}
}

void CCommandTab::ReloadMacros()
{
	if (m_pAut1Misc == NULL)
	{
		if (FAILED(theApp.FindInterface(IID_IAut1Misc, (LPVOID*) &m_pAut1Misc))
			|| m_pAut1Misc == NULL)
		{
			m_pAut1Misc = NULL;
			return;
		}
	}

	if(m_pAut1Misc)
	{
		m_pAut1Misc->ReloadMacros();
	}
}

////////////////////////////////////////////////////////////////////////////

void CVshellPackage::AddOptDlgPages(class CTabbedDialog* pOptDlg, OPTDLG_TYPE type)
{
	CMainFrame* pFrame = (CMainFrame*)theApp.m_pMainWnd;

	switch (type)
	{
	case customize:
		{
			if (theApp.IsUIElementEnabled(UieToolbarCustomize))
			{
				pOptDlg->AddTab(new CCustomDialog(pFrame->m_pManager, pFrame));
			}
			// for the moment, this one is always enabled, because the toolbars dialog was always enabled
			{
				CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

				// Self deleting dialog.
				pOptDlg->AddTab(new CToolbarDialog(pFrame->m_pManager, pFrame));
			}
			if (theApp.IsUIElementEnabled(UieToolsCustomize))
				pOptDlg->AddTab(new CCustomizeToolsDialog());
			if (theApp.IsUIElementEnabled(UieKeyCustomize))
				pOptDlg->AddTab(new CKeyCustDlg(NULL, NUM_ACC_RES_TABLES));
			break;
		}
		
	case options:
		{
			if (theApp.IsUIElementEnabled(UieWorkspaceOptions))
				pOptDlg->AddTab(new CWorkspaceDlg(pFrame->m_pManager, pFrame));
			if (theApp.IsUIElementEnabled(UieFormatOptions))
				pOptDlg->AddTab(new CFontColorDlg(pFrame));
		}
		break ;
	}
}

