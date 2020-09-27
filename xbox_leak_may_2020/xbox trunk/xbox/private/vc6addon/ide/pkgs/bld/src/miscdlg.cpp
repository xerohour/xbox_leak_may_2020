//
// Implementation for CRemoteTargetDlg and CCallingExeDlg classes
//
// History:
// Date				Who			What
// 03/23/94			colint			created
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard AFX include
#pragma hdrstop
#include "miscdlg.h"
#include "resource.h"
#include "msgboxes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const char szDbgFilter[] =	"Executable Files (*.exe;*.com)\0*.exe;*.com\0All Files (*.*)\0*.*\0\0";

///////////////////////////////////////////////////////////////////////////////
// CRemoteTargetDlg implementation
///////////////////////////////////////////////////////////////////////////////

CRemoteTargetDlg::CRemoteTargetDlg(BOOL bBuild /* = FALSE */,
				   CWnd * pParent /* = NULL */)
	: C3dDialog(CRemoteTargetDlg::IDD, pParent)
{
	m_bBuild = bBuild;
}

BEGIN_MESSAGE_MAP(CRemoteTargetDlg, C3dDialog)
	//{{AFX_MSG_MAP(CRemoteTargetDlg)
		ON_EN_CHANGE(IDC_REMOTE_TARGET, OnNameChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// BOOL CRemoteTargetDlg::OnInitDialog()
///////////////////////////////////////////////////////////////////////////////
BOOL CRemoteTargetDlg::OnInitDialog()
{
	CString strText;
	strText.LoadString(m_bBuild ? IDS_GET_REMTARG_BUILD : IDS_GET_REMTARG);	
	((CStatic *)GetDlgItem(IDC_REMOTE_TARGET_TXT))->SetWindowText(strText);
	((CEdit *)GetDlgItem(IDC_REMOTE_TARGET))->LimitText(255);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// void CRemoteTargetDlg::OnNameChange()
///////////////////////////////////////////////////////////////////////////////
void CRemoteTargetDlg::OnNameChange()
{
	BOOL bOK = ((CEdit *)GetDlgItem(IDC_REMOTE_TARGET))->LineLength() > 0;
	((CButton *)GetDlgItem(IDOK))->EnableWindow(bOK);
}

///////////////////////////////////////////////////////////////////////////////
// void CRemoteTargetDlg::OnOK()
///////////////////////////////////////////////////////////////////////////////
void CRemoteTargetDlg::OnOK()
{	
	CPlatform * pPlatform;
	CString str;
	BOOL bMacTarget;

	// Must have an active project!
	ASSERT(g_pActiveProject);
	pPlatform = g_pActiveProject->GetCurrentPlatform();
	bMacTarget = (pPlatform->GetUniqueId() == mac68k && pPlatform->IsSupported());

	if (bMacTarget)
	{
		// We must force macintosh path and file name
		// validation on the field as we have at least
		// one selected macintosh target in the tree
		// control.
		CString strRemote;
		BOOL bValidated = FALSE;
			
		CWnd * pWnd = GetDlgItem(IDC_REMOTE_TARGET);
		ASSERT(pWnd != (CWnd *)NULL);
		pWnd->GetWindowText(strRemote);

		if (!strRemote.IsEmpty())
		{
			// Must have a colon but cannot be the
			// first character in the string as this
			// indicates we have a relative pathname.
			int nPos = strRemote.Find(_T(':'));
			if (nPos == -1)
			{
				// No colon - must specify a volume
				// name
				InformationBox(IDS_MUST_SPECIFY_VOLUME_NAME, strRemote);
			}
			if (nPos == 0)
			{
				// Colon as first character - no
				// partial pathnames allowed.
				InformationBox(IDS_NO_PARTIAL_PATHNAMES, strRemote);
			}
			if (nPos > 0)
				// Pathname ok.
				bValidated = TRUE;
			
			if (!bValidated)
			{
				pWnd->SetFocus();
				((CEdit *)pWnd)->SetSel(0, -1);
				return;
			}
		}
	}

	((CEdit *)GetDlgItem(IDC_REMOTE_TARGET))->GetWindowText(str);

	// Must have an active project!
	ASSERT(g_pActiveProject);
	g_pActiveProject->SetStrProp(P_RemoteTarget, str);

	CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
// void CRemoteTargetDlg::OnCancel()
///////////////////////////////////////////////////////////////////////////////
void CRemoteTargetDlg::OnCancel()
{
	CDialog::OnCancel();
}


///////////////////////////////////////////////////////////////////////////////
// CCallingExeDlg implementation
///////////////////////////////////////////////////////////////////////////////

CCallingExeDlg::CCallingExeDlg(BOOL bExecute /* = FALSE */,
			       CWnd * pParent /* = NULL */)
	: C3dDialog(CCallingExeDlg::IDD, pParent)
{
	m_bExecute = bExecute;
}

BEGIN_MESSAGE_MAP(CCallingExeDlg, C3dDialog)
	//{{AFX_MSG_MAP(CCallingExeDlg)
		ON_EN_CHANGE(IDC_CALLING_PROG, OnNameChange)
		ON_COMMAND(IDM_BROWSE_FOR_DEBUG_EXE, OnDebugBrowse)
		ON_COMMAND(IDM_USE_TESTCONTAINER, OnUseTestContainer)
		ON_COMMAND(IDM_USE_WEBBROWSER, OnUseWebBrowser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_POPUP_MENU(ExeFinder)
	MENU_ITEM(IDM_BROWSE_FOR_DEBUG_EXE)
	MENU_ITEM(IDM_USE_TESTCONTAINER)
	MENU_ITEM(IDM_USE_WEBBROWSER)
END_POPUP_MENU()

///////////////////////////////////////////////////////////////////////////////
// BOOL CCallingExeDlg::OnInitDialog()
///////////////////////////////////////////////////////////////////////////////
BOOL CCallingExeDlg::OnInitDialog()
{
	// call the base class
	C3dDialog::OnInitDialog();

	CString strText;
	strText.LoadString(m_bExecute ? IDS_GET_CALLING_PROG : IDS_GET_CALLING_PROG_DEBUG);
	((CStatic *)GetDlgItem(IDC_CALLING_PROG_TXT2))->SetWindowText(strText);

	if (!m_btnExeFinder.SubclassDlgItem(IDC_DEBUG_BROWSE, this))
		return FALSE;	// not ok

	m_btnExeFinder.SetPopup(MENU_CONTEXT_POPUP(ExeFinder));

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// void CCallingExeDlg::OnNameChange()
///////////////////////////////////////////////////////////////////////////////
void CCallingExeDlg::OnNameChange()
{
	BOOL bOK = ((CEdit *)GetDlgItem(IDC_CALLING_PROG))->LineLength() > 0;
	((CButton *)GetDlgItem(IDOK))->EnableWindow(bOK);
}

///////////////////////////////////////////////////////////////////////////////
// void CCallingExeDlg::OnOK()
///////////////////////////////////////////////////////////////////////////////
void CCallingExeDlg::OnOK()
{	
	CString str;

	((CEdit *)GetDlgItem(IDC_CALLING_PROG))->GetWindowText(str);

	CPath ExePath;
	CString strMsg;

	ExePath.Create(str);
	if(!FileExists(ExePath) && (MsgBox(Information, MsgText(strMsg,
				IDS_NO_DEBUG_EXE,  (const TCHAR *)ExePath))))
	{
		((CEdit *)GetDlgItem(IDC_CALLING_PROG))->SetFocus();
		return;
	}

	// Must have an active project!
	ASSERT(g_pActiveProject);
	g_pActiveProject->SetStrProp(P_Caller, str);

	CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
// void CCallingExeDlg::OnCancel()
///////////////////////////////////////////////////////////////////////////////
void CCallingExeDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CCallingExeDlg::OnDebugBrowse() 
{
	// TODO: Add your control notification handler code here
	CDbgBrowseDlg fDlg;

	if(fDlg.DoModal() == IDOK)
	{
		CEdit* ceBrowse = (CEdit *) GetDlgItem(IDC_CALLING_PROG);
		ceBrowse->SetWindowText(fDlg.GetFileName());
	}
}

void CCallingExeDlg::OnUseWebBrowser()
{
	CEdit* ceBrowse = (CEdit *) GetDlgItem(IDC_CALLING_PROG);
	ceBrowse->SetWindowText(CProject::s_strWebBrowser);
}

void CCallingExeDlg::OnUseTestContainer()
{
	CEdit* ceBrowse = (CEdit *) GetDlgItem(IDC_CALLING_PROG);
	ceBrowse->SetWindowText(CProject::s_strTestContainer);
}

////////////////////////////////////////////////////////////////////
// DebBrowse dialog class
// CDbgBrowseDlg
////////////////////////////////////////////////////////////////////

CDbgBrowseDlg::CDbgBrowseDlg() 
	: C3dFileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHOWHELP, NULL, 
		NULL, 0, 0)
{
	 m_szFileBuff[0] = _T('\0');
}

CDbgBrowseDlg::~CDbgBrowseDlg()
{
}

CString CDbgBrowseDlg::GetFileName()
{
	CString strFileName = m_szFileBuff;
	return strFileName;
}

int CDbgBrowseDlg::DoModal()
{
	CString	strTitle;

	strTitle.LoadString(IDS_DEBUG_BROWSE_TITLE);

	m_ofn.lpstrFilter = szDbgFilter;
	m_ofn.nMaxCustFilter = sizeof(szDbgFilter);
	m_ofn.lpstrTitle = strTitle;

	m_ofn.lpstrFile = m_szFileBuff;
	m_ofn.nMaxFile = MAX_PATH-1;
	m_ofn.lpstrFileTitle = NULL;
	m_ofn.nMaxFileTitle = 0;	 
	m_ofn.nFileOffset = 0;
	m_ofn.nFileExtension = 0;
	m_ofn.lpstrDefExt = NULL;

	return C3dFileDialog::DoModal(); 
}


///////////////////////////////////////////////////////////////////////////////
// CJavaDebugInfoDlg implementation
///////////////////////////////////////////////////////////////////////////////
CJavaDebugInfoDlg::CJavaDebugInfoDlg(BOOL bExecute /*= FALSE*/, CWnd * pParent /*= NULL*/)
: C3dDialog(CJavaDebugInfoDlg::IDD, pParent),
  m_bExecute(bExecute)
{
}

BEGIN_MESSAGE_MAP(CJavaDebugInfoDlg, C3dDialog)
	//{{AFX_MSG_MAP(CJavaDebugInfoDlg)
		ON_EN_CHANGE(IDC_JAVA_CLASS_FILE_NAME, OnChange)
		ON_BN_CLICKED(IDC_JAVA_USE_BROWSER, OnChangeDebugUsing)
		ON_BN_CLICKED(IDC_JAVA_USE_STANDALONE, OnChangeDebugUsing)
		ON_EN_CHANGE(IDC_JAVA_BROWSER, OnChange)
		ON_EN_CHANGE(IDC_JAVA_STANDALONE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// CJavaDebugInfoDlg::DoDataExchange
void CJavaDebugInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJavaDebugInfoDlg)
	DDX_Control(pDX, IDC_JAVA_CLASS_FILE_NAME, m_editClassFileName);
	DDX_Control(pDX, IDC_JAVA_USE_BROWSER, m_btnBrowser);
	DDX_Control(pDX, IDC_JAVA_USE_STANDALONE, m_btnStandalone);
	DDX_Control(pDX, IDC_JAVA_BROWSER, m_editBrowser);
	DDX_Control(pDX, IDC_JAVA_STANDALONE, m_editStandalone);
	//}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
// CJavaDebugInfoDlg::OnInitDialog
BOOL CJavaDebugInfoDlg::OnInitDialog()
{
	// call the base class
	C3dDialog::OnInitDialog();

	// change title if we're executing instead of debugging
	// change description text if we're executing instead of debugging
	if (m_bExecute) {

		// change title
		CString strTitle;
		VERIFY(strTitle.LoadString(IDS_JAVA_CLASS_INFO_TITLE));
		SetWindowText(strTitle);

		// change group box text
		CWnd* pWnd = GetDlgItem(IDC_JAVA_DEBUGUSING_GRP);
		ASSERT(pWnd != NULL);
		if (pWnd != NULL) {
			CString strGroup;
			VERIFY(strGroup.LoadString(IDS_JAVA_CLASS_INFO_GRP_RUN));
			pWnd->SetWindowText(strGroup);
		}
	}

	// must have an active project
	ASSERT(g_pActiveProject != NULL);

	// load the class file name, if it's already set
	CString strClassFileName;
	if (g_pActiveProject->GetStrProp(P_Java_ClassFileName, strClassFileName) == valid)
		m_editClassFileName.SetWindowText(strClassFileName);

	// load debug using, if it's already set
	int nDebugUsing = Java_DebugUsing_Unknown;
	if (g_pActiveProject->GetIntProp(P_Java_DebugUsing, nDebugUsing) == valid) {
		switch (nDebugUsing) {
			case Java_DebugUsing_Browser:
				m_btnBrowser.SetCheck(1);
				m_btnStandalone.SetCheck(0);
				break;
			case Java_DebugUsing_Standalone:
				m_btnBrowser.SetCheck(0);
				m_btnStandalone.SetCheck(1);
				break;
		}
	}

	// load the browser, if it's already set
	CString strBrowser;
	g_pActiveProject->GetStrProp(P_Java_Browser, strBrowser);
	m_editBrowser.SetWindowText(strBrowser);

	// load the stand-alone interpreter, if it's already set
	CString strStandalone;
	g_pActiveProject->GetStrProp(P_Java_Standalone, strStandalone);
	m_editStandalone.SetWindowText(strStandalone);

	UpdateData(FALSE);

	// update the HTML viewer controls
	OnChangeDebugUsing();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CJavaDebugInfoDlg::OnChangeDebugUsing
void CJavaDebugInfoDlg::OnChangeDebugUsing()
{
	UpdateData(TRUE);

	// enable/disable the browser edit control
	m_editBrowser.EnableWindow(m_btnBrowser.GetCheck() == 1);

	// enable/disable the stand-alone edit control
	m_editStandalone.EnableWindow(m_btnStandalone.GetCheck() == 1);

	// update the OK button
	OnChange();
}

///////////////////////////////////////////////////////////////////////////////
// CJavaDebugInfoDlg::OnChange
void CJavaDebugInfoDlg::OnChange()
{
	UpdateData(TRUE);

	BOOL bClassFileName = (m_editClassFileName.LineLength() > 0);
	BOOL bDebugUsing = (m_btnBrowser.GetCheck() == 1 || m_btnStandalone.GetCheck() == 1);
	BOOL bBrowser = (m_btnBrowser.GetCheck() == 0 || m_editBrowser.LineLength() > 0);
	BOOL bStandalone = (m_btnStandalone.GetCheck() == 0 || m_editStandalone.LineLength() > 0);

	// enable the OK button if a class file name, debug using, and browser and/or stand-alone
	// have been entered
	CButton* pBtn = (CButton*)GetDlgItem(IDOK);
	ASSERT(pBtn != NULL);
	pBtn->EnableWindow(bClassFileName && bDebugUsing && bBrowser && bStandalone);
}

///////////////////////////////////////////////////////////////////////////////
// CJavaDebugInfoDlg::OnOK
void CJavaDebugInfoDlg::OnOK()
{
	UpdateData(TRUE);

	// must have an active project
	ASSERT(g_pActiveProject != NULL);

	// set the class file name
	CString strClassFileName;
	m_editClassFileName.GetWindowText(strClassFileName);

	// we don't allow the user to enter a path or .class extension
	// (other extensions are allowed because we can't tell the
	// difference between the extension in java.lang.String and
	// an invalid extension)

	// split the class name into parts
	// (and eliminate any path that's entered)
	TCHAR szFileName[_MAX_PATH];
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(strClassFileName, NULL, NULL, szFileName, szExt);

	// remove the extension if it's .class
	CString strNewClassFileName;
	if (_tcscmp(szExt, _T(".class")) == 0) {

		strNewClassFileName = CString(szFileName);
	}
	else {

		strNewClassFileName = CString(szFileName) + CString(szExt);
	}

	// set the class file name
	g_pActiveProject->SetStrProp(P_Java_ClassFileName, strNewClassFileName);

	// set debug using
	int nDebugUsing = (m_btnBrowser.GetCheck() == 1) ? Java_DebugUsing_Browser : Java_DebugUsing_Standalone;
	g_pActiveProject->SetIntProp(P_Java_DebugUsing, nDebugUsing);

	// set the browser
	CString strBrowser;
	m_editBrowser.GetWindowText(strBrowser);
	g_pActiveProject->SetStrProp(P_Java_Browser, strBrowser);

	// set the stand-alone interpreter
	CString strStandalone;
	m_editStandalone.GetWindowText(strStandalone);
	g_pActiveProject->SetStrProp(P_Java_Standalone, strStandalone);

	CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
// CPlatformsDlg implementation
///////////////////////////////////////////////////////////////////////////////

CPlatformsDlg::CPlatformsDlg(CStringList * plstPlatforms, CWnd * pParent /* = NULL */)
	: C3dDialog(CPlatformsDlg::IDD, pParent)
{
	m_plstPlatforms = plstPlatforms;
}

BEGIN_MESSAGE_MAP(CPlatformsDlg, C3dDialog)
	//{{AFX_MSG_MAP(CPlatformsDlg)
 	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// BOOL CPlatformsDlg::OnInitDialog()
///////////////////////////////////////////////////////////////////////////////
BOOL CPlatformsDlg::OnInitDialog()
{
	// Subclass the check listbox
	VERIFY(m_lbPlatforms.SubclassDlgItem(IDC_PLATFORMS, this));
	m_lbPlatforms.SetRedraw(FALSE);

	// call base class OnInitDialog
	C3dDialog::OnInitDialog() ;

	// Set the text to use the thin font
	GetDlgItem(IDC_PLATFORM_TEXT)->SetFont(GetStdFont(font_Normal));

	CPlatform * pPlatform;	CString strPlatformName;
	g_prjcompmgr.InitPlatformEnum();
	while (g_prjcompmgr.NextPlatform(pPlatform))
	{
        if (pPlatform->IsSupported() && pPlatform->GetBuildable())
        {
		    strPlatformName = *(pPlatform->GetUIDescription());
			if (pPlatform->GetUniqueId () == java)
				continue;

		    m_lbPlatforms.AddString(strPlatformName);
        }
	}

	// Check all the platforms as the default
	int nPlatforms = m_lbPlatforms.GetCount();
	for (int i = 0; i < nPlatforms; i++)
		m_lbPlatforms.SetCheck(i, TRUE);

	m_lbPlatforms.SetRedraw(TRUE);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// void CPlatformsDlg::OnOK()
///////////////////////////////////////////////////////////////////////////////
void CPlatformsDlg::OnOK()
{	
	// Construct the list of platforms
	int nItem, nPlatforms;

	// Construct a CStringList of the platforms
	m_plstPlatforms->RemoveAll();
	nPlatforms = m_lbPlatforms.GetCount();
	for (nItem = 0; nItem < nPlatforms; nItem++)
	{
		if (m_lbPlatforms.GetCheck(nItem))
		{	
			CString strPlatformName;

			m_lbPlatforms.GetText(nItem, strPlatformName);
			m_plstPlatforms->AddTail(strPlatformName);
		}
	}

	// We must have at least one platform selected.
	if (m_plstPlatforms->IsEmpty())
	{
		MsgBox(Information, IDS_BAD_PLATFORMS);
		return;
	}

	C3dDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
// void CPlatformsDlg::OnCancel()
///////////////////////////////////////////////////////////////////////////////
void CPlatformsDlg::OnCancel()
{
	C3dDialog::OnCancel();
}
