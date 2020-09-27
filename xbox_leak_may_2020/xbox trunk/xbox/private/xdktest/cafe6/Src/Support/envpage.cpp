/////////////////////////////////////////////////////////////////////////////
// envpage.cpp
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Implementation of the CEnvPage class

#include "stdafx.h"
#include "toolset.h"
#include "envpage.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnvPage property page

IMPLEMENT_DYNCREATE(CEnvPage, CPropertyPage)

CEnvPage::CEnvPage(CSettings* pSettings /*= NULL*/)
: CPropertyPage(CEnvPage::IDD),
  m_pSettings(pSettings)
{
	//{{AFX_DATA_INIT(CEnvPage)
		m_strEnvvar = _T("");
	//}}AFX_DATA_INIT
}

CEnvPage::~CEnvPage()
{
}

void CEnvPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnvPage)
	DDX_Control(pDX, IDL_Envvars, m_lbEnv);
	DDX_Control(pDX, IDC_Envvar, m_cbEnvvars);
	DDX_CBString(pDX, IDC_Envvar, m_strEnvvar);
	//}}AFX_DATA_MAP
}


BOOL CEnvPage::OnSetActive(void)
{
	int i;

	// call the base class
	if (!CPropertyPage::OnSetActive()) {
		return FALSE;
	}

	// the settings object must exist
	ASSERT(m_pSettings);

	// fill the environment var combo box
	for (i = 0; !CToolset::m_strEnvvars[i].IsEmpty(); i++) {
		m_cbEnvvars.AddString(CToolset::m_strEnvvars[i]);
	}

	// select the first item in this combo box
	m_cbEnvvars.SetCurSel(0);

	// copy the settings into the appropriate variables
	UpdateData(TRUE);

	// fill the envvar list box with the settings of this envvar
	FillEnvvarList();

	return TRUE;
}

void CEnvPage::OnOK(void)
{
	// call the base class
	CPropertyPage::OnOK();

	// the settings object must exist
	ASSERT(m_pSettings);

	// set envvar setting to current envvar list box
	SetEnvvar();

	// write settings to the registry
	m_pSettings->WriteRegistry();
}

BOOL CEnvPage::FillEnvvarList(void)
{
	CString strValue;
	int nSeparate;

	// clear the envvar list box
	m_lbEnv.ResetContent();

	// get the envvar value from the settings object
	strValue = m_pSettings->GetTextValue(m_strEnvvar);

	// separate the value at ;
	for (BOOL bDone = FALSE; !bDone; ) {
		// find the separating ;
		nSeparate = strValue.Find(';');
		// if there is no ;, then assume the end of the string and we're done
		if (nSeparate == -1) {
			nSeparate = strValue.GetLength();
		}
		// only insert if the substring is not empty (iSeparate > 0)
		if (nSeparate > 0) {
			// insert the string in the list box
			m_lbEnv.AddString(strValue.Left(nSeparate));
		}

		// if the separator is at the end of the string, then we're done
		if (nSeparate == strValue.GetLength()) {
			bDone = TRUE;
		}
		// remove the inserted string from the value
		else {
			// remove the inserted envvar from the string
			// +1 to skip the ;
			strValue = strValue.Mid(nSeparate+1);
		}
	}

	// add a blank line to the end of the envvar list box
	m_lbEnv.AddString("");

	// select the first line in the list box
	m_lbEnv.SetCurSel(0);

	// update UI
	OnSelchangeEnvvars();
	
	return TRUE;
}

BOOL CEnvPage::SetEnvvar(void)
{
	CString strValue;
	CString strTemp;
	int i;

	// build the value of the envvar from the list box
	for (i = 0; i < m_lbEnv.GetCount()-1; i++) {
		m_lbEnv.GetText(i, strTemp);
		strValue += strTemp + ";";
	}

	// remove the trailing ;
	if (!strValue.IsEmpty()) {
		strValue = strValue.Left(strValue.GetLength()-1);
	}

	// store this envvar in the settings object
	m_pSettings->SetTextValue(m_strEnvvar, strValue);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CEnvPage, CPropertyPage)
	//{{AFX_MSG_MAP(CEnvPage)
	ON_BN_CLICKED(IDB_EnvvarEdit, OnEnvvarEdit)
	ON_BN_CLICKED(IDB_EnvvarsAdd, OnEnvvarsAdd)
	ON_BN_CLICKED(IDB_EnvvarsMoveUp, OnEnvvarsMoveUp)
	ON_BN_CLICKED(IDB_EnvvarsRemove, OnEnvvarsRemove)
	ON_BN_CLICKED(IDC_EnvvarsMoveDown, OnEnvvarsMoveDown)
	ON_CBN_SELCHANGE(IDC_Envvar, OnSelchangeEnvvar)
	ON_LBN_SELCHANGE(IDL_Envvars, OnSelchangeEnvvars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEnvPage message handlers


void CEnvPage::OnEnvvarEdit() 
{
	CAddDlg dlgAddValue;
	int nSel;

	nSel = m_lbEnv.GetCurSel();
	if(nSel<0 || nSel>=m_lbEnv.GetCount()-1) //can't deal with the last one
		return; //no selection
	m_lbEnv.GetText(nSel, dlgAddValue.m_strNewValue);
	if (dlgAddValue.DoModal() == IDOK) {
		// update the string
		m_lbEnv.DeleteString(nSel);
		nSel = m_lbEnv.InsertString(nSel, dlgAddValue.m_strNewValue);

		// update UI
		OnSelchangeEnvvars();
	}	
}

void CEnvPage::OnEnvvarsAdd() 
{
	CAddDlg dlgAddValue;
	int nSel;
	
	// display the add new directory dialog
	if (dlgAddValue.DoModal() == IDOK) {

		// get the currently selected dir
		nSel = m_lbEnv.GetCurSel();

		// insert the string
		nSel = m_lbEnv.InsertString(nSel, dlgAddValue.m_strNewValue);

		// select the next item
		m_lbEnv.SetCurSel(nSel+1);
		
		// update UI
		OnSelchangeEnvvars();
	}
}

void CEnvPage::OnEnvvarsMoveUp() 
{
	int nSel;
	CString strSel;

	// get the currently selected item in the envvar list box
	nSel = m_lbEnv.GetCurSel();
	m_lbEnv.GetText(nSel, strSel);

	// remove the selected item from the list box
	m_lbEnv.DeleteString(nSel);

	// insert the string up one position
	nSel = m_lbEnv.InsertString(nSel-1, strSel);

	// select the item
	m_lbEnv.SetCurSel(nSel);

	// update UI
	OnSelchangeEnvvars();
}

void CEnvPage::OnEnvvarsRemove() 
{
	int nSel;

	// get the currently selected item in the dir list box
	nSel = m_lbEnv.GetCurSel();

	// remove the selected item from the list box
	m_lbEnv.DeleteString(nSel);

	// select the item in the list box (which was the next item)
	m_lbEnv.SetCurSel(nSel);

	// update UI
	OnSelchangeEnvvars();
}

void CEnvPage::OnEnvvarsMoveDown() 
{
	int nSel;
	CString strSel;

	// get the currently selected item in the envvar list box
	nSel = m_lbEnv.GetCurSel();
	m_lbEnv.GetText(nSel, strSel);

	// remove the selected item from the list box
	m_lbEnv.DeleteString(nSel);

	// insert the string down one position
	nSel = m_lbEnv.InsertString(nSel+1, strSel);

	// select the item
	m_lbEnv.SetCurSel(nSel);

	// update UI
	OnSelchangeEnvvars();
}

void CEnvPage::OnSelchangeEnvvar() 
{
	// write previous envvar to the registry
	SetEnvvar();

	// update data
	UpdateData(TRUE);

	// fill the envvar list from the registry
	FillEnvvarList();
}

void CEnvPage::OnSelchangeEnvvars() 
{
	// get pointers to the Move Up, Move Down, and Delete buttons
	CWnd *pMoveUp = GetDlgItem(IDB_EnvvarsMoveUp);
	CWnd *pMoveDown = GetDlgItem(IDC_EnvvarsMoveDown);
	CWnd *pDelete = GetDlgItem(IDB_EnvvarsRemove);

	// enable the buttons
	pMoveUp->EnableWindow();
	pMoveDown->EnableWindow();
	pDelete->EnableWindow();

	// if the first item in the envvar list box is selected, disable the Move Up button
	if (m_lbEnv.GetCurSel() == 0) {
		pMoveUp->EnableWindow(FALSE);
	}

	// if the last item (the blank line) in the envvar list box
	// is selected, disable Move Up, Move Down, and Delete
	// -1 because GetCurSel() is zero-based
	if (m_lbEnv.GetCurSel() == m_lbEnv.GetCount()-1) {
		pMoveUp->EnableWindow(FALSE);
		pMoveDown->EnableWindow(FALSE);
		pDelete->EnableWindow(FALSE);
	}

	// if the second to the last item in the directory list box
	// is selected, disable Move Down
	if (m_lbEnv.GetCurSel() == m_lbEnv.GetCount()-2) {
		pMoveDown->EnableWindow(FALSE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAddDlg dialog


CAddDlg::CAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddDlg)
	m_strNewValue = _T("");
	//}}AFX_DATA_INIT
}

void CAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddDirDlg)
	DDX_Text(pDX, IDE_AddValue, m_strNewValue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddDlg, CDialog)
	//{{AFX_MSG_MAP(CAddDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddDlg message handlers

BOOL CAddDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// set the focus to the new directory edit box
	GotoDlgCtrl(GetDlgItem(IDE_AddValue));
		
	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
