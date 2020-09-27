// BLDDLG.CPP
// -----------
// Implements the Directories dialog.
//
// History
// =======
// 20-Feb-96	karlsi		Created

////////////////////////////////////////////////////////////
// Include files

#include "stdafx.h"
#pragma hdrstop
#include "blddlg.h"
#include "dlgbase.h"
#include "resource.h"
#include "projcomp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#if 0
////////////////////////////////////////////////////////////
// CBldGrid
CBldGrid::CBldGrid()
	: CStringListGridWnd(GRIDLIST_ELLIPSE)
{
}
#endif

////////////////////////////////////////////////////////////
// Constructors, destructors

CBldOptDlg::CBldOptDlg()
	: CDlgTab(IDDP_OPTIONS_BUILD, IDS_BUILD)
{
	// m_BldGrid.m_pDlg = this;
	m_bAlreadyWarnedOfBuild = FALSE;	
	m_bAlwaysExportMakefile = FALSE;
	m_bExportDeps = FALSE;
	m_bWriteBuildLog = TRUE;
}

CBldOptDlg::~CBldOptDlg
(
)
{
}

////////////////////////////////////////////////////////////
// CBldOptDlg::Activate

BOOL CBldOptDlg::Activate
(
	CTabbedDialog *	ptd,
	CPoint			cp
)
{
	// TODO:

	// Make sure that the actual activation occurs!
	return CDlgTab::Activate( ptd, cp );
}


BOOL CBldOptDlg::ValidateTab()
{
	// currently always valid

	return TRUE;
}

////////////////////////////////////////////////////////////
// CBldOptDlg::OnInitDialog

BOOL CBldOptDlg::OnInitDialog
(
)
{
#if 0
	VERIFY(m_DirGrid.ReplaceControl(this,
									IDC_PLACEHOLDER2, IDC_DIRS_LIST,
									WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
									WS_EX_CLIENTEDGE, GRIDWND_TB_ALL));
#endif

	m_bAlwaysExportMakefile = g_bAlwaysExportMakefile;
	m_bExportDeps = g_bAlwaysExportDeps;
	m_bWriteBuildLog = g_bWriteBuildLog;

	CDlgTab::OnInitDialog();

	return(TRUE);
}

////////////////////////////////////////////////////////////
// CBldOptDlg::CheckForBuildAndWarn
void CBldOptDlg::CheckForBuildAndWarn ()
{
	if (m_bAlreadyWarnedOfBuild) return;
	m_bAlreadyWarnedOfBuild = TRUE;
	return;  // don't care at the moment

	// If there is a build in process, then warn the user that changes
	// to the directories will not take effect until the next build:

	if (g_Spawner.SpawnActive ())
	{
		MsgBox ( Information, IDS_DIR_CHANGE_IN_BUILD ); // UNDONE
	}
}

////////////////////////////////////////////////////////////
// CBldOptDlg::CommitTab

void CBldOptDlg::CommitTab()
{
	if (GetSafeHwnd() == NULL)
		return;

	UpdateData(TRUE);
	// m_BldGrid.AcceptControl();

	// TODO: update data
	g_bAlwaysExportMakefile = m_bAlwaysExportMakefile;
	g_bAlwaysExportDeps = m_bExportDeps;
	g_bWriteBuildLog = m_bWriteBuildLog;
}

////////////////////////////////////////////////////////////
// Button-clicked handlers

void CBldOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBldOptDlg)
	DDX_Check(pDX, IDC_EXPORT_DEPS, m_bExportDeps);
	DDX_Check(pDX, IDC_ALWAYS_EXPORT_MAK, m_bAlwaysExportMakefile);
	DDX_Check(pDX, IDC_WRITE_BUILD_LOG, m_bWriteBuildLog);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBldOptDlg, CDlgTab)
	//{{AFX_MSG_MAP(CBldOptDlg)
	ON_BN_CLICKED(IDC_ALWAYS_EXPORT_MAK,OnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBldOptDlg::OnClick()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_ALWAYS_EXPORT_MAK);

	if (NULL!= pBtn)
	{
		if (pBtn->GetCheck())
		{
			// this warning is gratuitous
			// AfxMessageBox( IDS_WRN_SETEXPORTMAKEFILE, MB_OK | MB_ICONINFORMATION );
		}
	}
}

#if 0
BOOL CBldOptDlg::OnAdd(int nIndex)
{
	return TRUE;
}

BOOL CBldOptDlg::OnDel(int nIndex)
{
	return TRUE;
}

BOOL CBldOptDlg::OnMove(int nSrcIndex, int nDestIndex)
{
	CheckForBuildAndWarn();
	return TRUE;
}

BOOL CBldOptDlg::OnChange(int nIndex)
{
	CheckForBuildAndWarn();
	return TRUE;
}
#endif
