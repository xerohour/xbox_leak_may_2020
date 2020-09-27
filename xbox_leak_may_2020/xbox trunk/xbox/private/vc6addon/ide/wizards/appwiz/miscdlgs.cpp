// miscdlgs.cpp : implementation file
//

#include "stdafx.h"
#include "mfcappwz.h"
#include "miscdlgs.h"
#include "ddxddv.h"
#include "advanced.h"
#include "symbols.h"
#include "lang.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// This is used by the DocAppOptionsDlg & DlgAppOptionsDlg to verify
//  the string entered as the app's title.

#define MAX_MAC_TITLE 27   // Max no. chars. in Mac titlebar.

BOOL IsValidTitle(const char* szTitle)
{
	int nLen = strlen(szTitle);
	if (nLen > MAX_MAC_TITLE)
		return FALSE;
	while (*szTitle)
	{
		//this won't go off the end of the string because even if you see a '\' as
		//the last char, checking the \0 against 'n' won't hurt anything
		if (*szTitle == _T('\"') || (*szTitle == _T('\\') && *(szTitle + 1) == _T('n')))
		{
			return FALSE;
		}
		szTitle = _tcsinc(szTitle);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDocAppOptionsDlg dialog

CDocAppOptionsDlg::CDocAppOptionsDlg(CWnd* pParent /*=NULL*/)
	: CSeqDlg(CDocAppOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDocAppOptionsDlg)
	m_nSizeMRU = 0;
	m_nMenus = 0;
	//}}AFX_DATA_INIT
}

void CDocAppOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CSeqDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocAppOptionsDlg)
	DDX_Text(pDX, IDC_MRU, m_nSizeMRU);
	DDV_MinMaxUInt(pDX, m_nSizeMRU, 0, 16);
	DDX_Radio(pDX, IDC_NORMALMENU, m_nMenus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDocAppOptionsDlg, CSeqDlg)
	//{{AFX_MSG_MAP(CDocAppOptionsDlg)
	ON_BN_CLICKED(IDC_ADVANCED, OnAdvanced)
	ON_COMMAND_EX(IDCD_POTOOLBAR, OnClickedCheckbox)
	ON_COMMAND_EX(IDCD_POSTATUSBAR, OnClickedCheckbox)
	ON_COMMAND_EX(IDCD_POPRINT, OnClickedCheckbox)
	ON_COMMAND_EX(IDCD_POHELP, OnClickedCheckbox)
	ON_COMMAND_EX(IDCD_PO3D, OnClickedCheckbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDocAppOptionsDlg::CanDestroy()
{
	if (!UpdateData(TRUE))
		return FALSE;

	if (projOptions.m_bActiveDocServer && projOptions.m_names.strDocFileExt.IsEmpty())
	{
		AfxMessageBox(IDP_NOEXT_DOCSVR);
		return FALSE;
	}

	WriteValues();		
	return TRUE;
}

void CDocAppOptionsDlg::WriteValues()
{
	UpdateData(TRUE);
	projOptions.m_nSizeMRU = m_nSizeMRU;
    for (int i=0; i < PO_OPTION_COUNT; i++)
    {
        CButton* pCheck = (CButton*)GetDlgItem(IDCD_POBASE + i);
        ASSERT(pCheck != NULL);
        if (pCheck->GetCheck())
            projOptions.m_options |= (1 << i);
        else
            projOptions.m_options &= ~(1 << i);
    }
	projOptions.m_nMenuStyle = m_nMenus;
	SetAppOptionsSymbols();
	SetSizeMRUSymbols(m_nSizeMRU);
	SetDocumentSymbols();
	SetFrameSymbols();
	SetMenuSymbols();
	SetSplitterSymbols();
	SetChildFrameSymbols();
}

void CDocAppOptionsDlg::ReadValues()
{
	GetAppOptionsSymbols();
	GetSizeMRUSymbols();
	GetDocumentSymbols();
	GetFrameSymbols();
	GetMenuSymbols();
	GetSplitterSymbols();
	GetChildFrameSymbols();
	m_nSizeMRU = projOptions.m_nSizeMRU;
	m_nMenus = projOptions.m_nMenuStyle;
	UpdateData(FALSE);
    for (int i=0; i < PO_OPTION_COUNT; i++)
    {
        CButton* pCheck = (CButton*)GetDlgItem(IDCD_POBASE + i);
        ASSERT(pCheck != NULL);
        if (projOptions.m_options & (1 << i))
            pCheck->SetCheck(1);
        else
            MyResetCheck(pCheck);
    }
	CButton* pCheck = (CButton*)GetDlgItem(IDCD_POTOOLBAR);
	ASSERT(pCheck != NULL);
	GetDlgItem(IDC_REBARMENU)->EnableWindow(pCheck->GetCheck());
	GetDlgItem(IDC_NORMALMENU)->EnableWindow(pCheck->GetCheck());
}

BOOL CDocAppOptionsDlg::OnClickedCheckbox(UINT nID)
{
	CMainDlg* pMainDlg = (CMainDlg*) GetParent();
	pMainDlg->RedrawBitmap(BMPSIZE_SUM);
	pMainDlg->RedrawBitmap(BMPSIZE_3D);

	if(nID == IDCD_POTOOLBAR)
	{
		CButton* pCheck = (CButton*)GetDlgItem(IDCD_POTOOLBAR);
		ASSERT(pCheck != NULL);
		GetDlgItem(IDC_REBARMENU)->EnableWindow(pCheck->GetCheck());
		GetDlgItem(IDC_NORMALMENU)->EnableWindow(pCheck->GetCheck());
	}

	return TRUE;
}

static UINT DAObmp[] =
{
	IDB_B140000, IDB_B140001, IDB_B140010, IDB_B140011,
	IDB_B140100, IDB_B140101, IDB_B140110, IDB_B140111,
	IDB_B141000, IDB_B141001, IDB_B141010, IDB_B141011,
	IDB_B141100, IDB_B141101, IDB_B141110, IDB_B141111,
};

#define NUM_PICTURED_OPTIONS 4
UINT CDocAppOptionsDlg::ChooseBitmap()
{
	int nIndex = 0;
	for (int i=0; i < NUM_PICTURED_OPTIONS; i++)
	{
		CButton* pCheck = (CButton*)GetDlgItem(IDCD_PICBASE + ((NUM_PICTURED_OPTIONS-1) - i));
        ASSERT(pCheck != NULL);
		if (pCheck->GetCheck())
			nIndex += (1 << i);
	}
	return DAObmp[nIndex];
}

static UINT DAO3Dbmp[] =
{
	IDB_B143DNO, IDB_B143DON,
};

UINT CDocAppOptionsDlg::ChooseBitmap2()
{
	CButton* pCheck = (CButton*) GetDlgItem(IDCD_PO3D);
	ASSERT(pCheck != NULL);
	if (pCheck->GetCheck())
		return DAO3Dbmp[1];
	else
		return DAO3Dbmp[0];
}

BOOL IsMacWOSA();		// defined in mfcappwz.cpp

void CDocAppOptionsDlg::EnablePrint(BOOL bEnable)
{
	GetDlgItem(IDCD_POPRINT)->EnableWindow(bEnable);
}

BOOL CDocAppOptionsDlg::OnInitDialog()
{
    CSeqDlg::OnInitDialog();
	VERIFY(m_btnAdvanced.SubclassDlgItem(IDC_ADVANCED, this));

	// Init MRU Spin control
	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*) GetDlgItem(IDC_SPINMRU);
	pSpin->SetRange(0, 16);


    CenterWindow();

	if (projOptions.m_bNoDocView)
	{
		// No Doc View architecture support means no printing
		projOptions.m_options &= ~(1 << (IDCD_POPRINT-IDCD_POBASE) );
		SetAppOptionsSymbols();
		EnablePrint(FALSE);
		GetDlgItem(IDCD_POMAPI)->EnableWindow(FALSE);
	}
	else
	{
		EnablePrint(TRUE);
		GetDlgItem(IDCD_POMAPI)->EnableWindow(TRUE);
	}

    ReadValues();
    return TRUE;
}

// We override PreTranslate to make sure that if a button is default,
//  <return> causes it to be activated.
BOOL CDocAppOptionsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN
		&& ::GetFocus() == GetDlgItem(IDC_ADVANCED)->GetSafeHwnd())
	{
		SendMessage(WM_COMMAND,
			MAKEWPARAM(IDC_ADVANCED, BN_CLICKED),
			(LPARAM) (GetDlgItem(IDC_ADVANCED)->GetSafeHwnd()));
		return TRUE;
	}

	return CSeqDlg::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAppOptionsDlg dialog


CDlgAppOptionsDlg::CDlgAppOptionsDlg(CWnd* pParent /*=NULL*/)
	: CSeqDlg(CDlgAppOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAppOptionsDlg)
	m_bOleAutomation = FALSE;
	m_bOcx = TRUE;
	//}}AFX_DATA_INIT
}

void CDlgAppOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CSeqDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAppOptionsDlg)
	DDX_Check(pDX, IDCD_AUTOMATION, m_bOleAutomation);
	DDX_Check(pDX, IDCD_OCX, m_bOcx);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAppOptionsDlg, CSeqDlg)
	//{{AFX_MSG_MAP(CDlgAppOptionsDlg)
    ON_WM_DESTROY()
	ON_COMMAND_EX(IDCD_POABOUT, OnClickedCheckbox)
	ON_COMMAND_EX(IDCD_POHELP, OnClickedCheckbox)
	ON_COMMAND_EX(IDCD_PO3D, OnClickedCheckbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgAppOptionsDlg::GetTitle(CString& strTitle)
{
	CWnd* pEdit = GetDlgItem(IDC_APP_TITLE);
	ASSERT_VALID(pEdit);
	pEdit->GetWindowText(strTitle);
	strTitle.ReleaseBuffer();
}

BOOL CDlgAppOptionsDlg::CanDestroy()
{
	CString strTitle;
	GetTitle(strTitle);
	if (!(strTitle))
	{
		AfxMessageBox(IDP_BAD_TITLE);
		CEdit* pEdit = (CEdit*) GetDlgItem(IDC_APP_TITLE);
		pEdit->SetSel(0, -1);
		pEdit->SetFocus();
		return FALSE;
	}
	projOptions.m_names.strTitle = strTitle;
	return TRUE;
}
		
void CDlgAppOptionsDlg::WriteValues()
{
	VERIFY(UpdateData());
	projOptions.m_bOleAutomation = m_bOleAutomation;
	projOptions.m_bOcx = m_bOcx;
	GetTitle(langDlls.m_astrTitle[0]);
    for (int i=0; i < PODLG_OPTION_COUNT; i++)
    {
        CButton* pCheck = (CButton*)GetDlgItem(IDCD_PODLGBASE + i);
        ASSERT(pCheck != NULL);
        if (pCheck->GetCheck())
            projOptions.m_options |= (1 << (i + IDCD_PODLGBASE-IDCD_POBASE));
        else
            projOptions.m_options &= ~(1 << (i + IDCD_PODLGBASE-IDCD_POBASE));
    }
	SetAppOptionsSymbols();
	SetOleSymbols();
}

void CDlgAppOptionsDlg::ReadValues()
{
	GetAppOptionsSymbols();
	GetOleSymbols();
    for (int i=0; i < PODLG_OPTION_COUNT; i++)
    {
        CButton* pCheck = (CButton*)GetDlgItem(IDCD_PODLGBASE + i);
        ASSERT(pCheck != NULL);
        if (projOptions.m_options & (1 << (i + IDCD_PODLGBASE-IDCD_POBASE)))
            pCheck->SetCheck(1);
        else
            MyResetCheck(pCheck);
    }

	CWnd* pEdit = GetDlgItem(IDC_APP_TITLE);
	ASSERT_VALID(pEdit);
	pEdit->SetWindowText(projOptions.m_names.strTitle);	
	m_bOleAutomation = projOptions.m_bOleAutomation;
	m_bOcx = projOptions.m_bOcx;
	UpdateData(FALSE);
}

// Enable/disable WOSA & OCX options
void CDlgAppOptionsDlg::EnableVarious(BOOL bEnableWOSA, BOOL bEnableOCX)
{
	// WOSA
	GetDlgItem(IDCD_POSOCKETS)->EnableWindow(bEnableWOSA);
	GetDlgItem(IDC_WOSA_QUESTION)->EnableWindow(bEnableWOSA);
	GetDlgItem(IDC_WOSA_STATIC)->ShowWindow(bEnableWOSA ? SW_HIDE : SW_SHOW);

	// OCX
	GetDlgItem(IDCD_OCX)->EnableWindow(bEnableOCX);
	GetDlgItem(IDC_OCX_STATIC)->ShowWindow(bEnableOCX ? SW_HIDE : SW_SHOW);
}

BOOL CDlgAppOptionsDlg::OnInitDialog()
{
    CSeqDlg::OnInitDialog();
    CenterWindow();
    
	CString tmp;
	if (DoesSymbolExist("TARGET_MAC") && !IsMacWOSA())
	{
		// MacWOSA is not around, so select no WOSA support & disable the controls
		projOptions.m_options &= ~(1 << IDCD_POMAPI-IDCD_POBASE);
		projOptions.m_options &= ~(1 << IDCD_POSOCKETS-IDCD_POBASE);
		projOptions.m_bOcx = IsMacOcx();
		EnableVarious(FALSE, IsMacOcx());
	}
	else
	{
		// WOSA is a-ok to be enabled.
		EnableVarious(TRUE, TRUE);
	}

    ReadValues();
    return TRUE;
}

BOOL CDlgAppOptionsDlg::OnClickedCheckbox(UINT nID)
{
	CMainDlg* pMainDlg = (CMainDlg*) GetParent();
	pMainDlg->RedrawBitmap(BMPSIZE_SUM);
	pMainDlg->RedrawBitmap(BMPSIZE_3D);
	return TRUE;
}

static UINT DgAObmp[] =
{
	IDB_B2200, IDB_B2201, IDB_B2210, IDB_B2211,
};

UINT CDlgAppOptionsDlg::ChooseBitmap()
{
	int nIndex = 0;
	CButton* pCheck = (CButton*)GetDlgItem(IDCD_POABOUT);
	ASSERT(pCheck != NULL);
	if (pCheck->GetCheck())
		nIndex += (1 << 1);
	pCheck = (CButton*)GetDlgItem(IDCD_POHELP);
	ASSERT(pCheck != NULL);
	if (pCheck->GetCheck())
		nIndex += (1 << 0);

	return DgAObmp[nIndex];
}

UINT CDlgAppOptionsDlg::ChooseBitmap2()
{
	CButton* pCheck = (CButton*) GetDlgItem(IDCD_PO3D);
	ASSERT(pCheck != NULL);
	if (pCheck->GetCheck())
		return DAO3Dbmp[1];
	else
		return DAO3Dbmp[0];
}

void CDlgAppOptionsDlg::OnDestroy()
{
    WriteValues();
    CSeqDlg::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CProjOptionsDlg dialog

CProjOptionsDlg::CProjOptionsDlg(UINT nID, CWnd* pParent)
	: CSeqDlg(nID, pParent)
{
}

CProjOptionsDlg::CProjOptionsDlg(CWnd* pParent /*=NULL*/)
	: CSeqDlg(CProjOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjOptionsDlg)
	m_nAtlSupport = 1;
	m_nProjStyle = PS_MFCNORMAL;
	//}}AFX_DATA_INIT
}

void CProjOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CSeqDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjOptionsDlg)
	DDX_Radio(pDX, IDC_MFCSTANDARD, m_nProjStyle);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProjOptionsDlg, CSeqDlg)
	//{{AFX_MSG_MAP(CProjOptionsDlg)
	ON_COMMAND_EX(IDCD_POVERBOSE, OnClickedRadio)
    ON_WM_DESTROY()
	ON_COMMAND_EX(IDC_RADIO2, OnClickedRadio)
	ON_COMMAND_EX(IDC_RADIO3, OnClickedRadio)
	ON_COMMAND_EX(IDC_RADIO5, OnClickedRadio)
	ON_COMMAND_EX(IDC_MFCSTANDARD, OnClickedRadio)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CProjOptionsDlg::OnClickedRadio(UINT nID)
{
	UpdateData(TRUE);	// Force a DDX to Get values
	CMainDlg* pMainDlg = (CMainDlg*) GetParent();
	pMainDlg->RedrawBitmap(BMPSIZE_SUM);
	return TRUE;
}

static UINT PObmp[] =
{
	IDB_B150110, IDB_B150101, IDB_B151010, IDB_B151001,
};

UINT CProjOptionsDlg::ChooseBitmap()
{
	int nIndex = 0;
	//for (int i=0; i < PO_PROJ_COUNT-1; i++)
	const int i = 0;
	{
        CButton* pCheck = (CButton*)GetDlgItem(IDCD_POPROJBASE + i);
        ASSERT(pCheck != NULL);
        if (pCheck->GetCheck())
			nIndex = 2;
		else
			nIndex = 0;
	}
	return PObmp[nIndex];
	// TODO: Fix me

}

void CProjOptionsDlg::WriteValues()
{
    for (int i=0; i < PO_PROJ_COUNT; i++)
    {
        CButton* pCheck = (CButton*)GetDlgItem(IDCD_POPROJBASE + i);
        ASSERT(pCheck != NULL);
        if (pCheck->GetCheck())
            projOptions.m_projOptions |= (1 << i);
        else
            projOptions.m_projOptions &= ~(1 << i);
    }
    projOptions.m_bATLSupport= !m_nAtlSupport;
    projOptions.m_nProjStyle = m_nProjStyle;
	SetProjOptionsSymbols();
}

BOOL CProjOptionsDlg::CanDestroy()
{
    WriteValues();
	return TRUE;
}

void CProjOptionsDlg::ReadValues()
{
	GetProjOptionsSymbols();
    for (int i=0; i < PO_PROJ_COUNT; i++)
    {
        CButton* pCheck = (CButton*)GetDlgItem(IDCD_POPROJBASE + i);
        ASSERT(pCheck != NULL);
        if (projOptions.m_projOptions & (1 << i))
            pCheck->SetCheck(1);
        else
            MyResetCheck(pCheck);
    }
    if( projOptions.m_bATLSupport ) 
		m_nAtlSupport = 0;
	else 
		m_nAtlSupport = 1;

    m_nProjStyle = projOptions.m_nProjStyle;
}

BOOL CProjOptionsDlg::OnInitDialog()
{
	// we don't give the user an option to change how we link to MFC
	// for the learning edition.  The GetDlgItem(IDC_MFC_LIB) call
	// in the if is to make sure we're working with the dialog resource
	// that belongs to CProjOptionsDlg.  CDllProjOptionsDlg is derived
	// from this class and handles the MFC linkage a bit differently.
	if (IsLearningBox() && GetDlgItem(IDC_MFC_LIB))
	{
		GetDlgItem(IDC_MFC_LIB)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCD_PODLL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCD_PONOTDLL)->ShowWindow(SW_HIDE);
	}
	if( GetDlgItem(IDC_RADIO5) )
	{
		if( projOptions.m_bNoDocView || (projOptions.m_nProjType == PROJTYPE_DLG) )
			GetDlgItem(IDC_RADIO5)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_RADIO5)->EnableWindow(TRUE);
	}

    CSeqDlg::OnInitDialog();
    CenterWindow();

	// If we're not targeting something that can use MFC in a DLL,
	//  force user to use static MFC
	CString tmp;
	if (!DoesSymbolExist("TARGET_MIPS")
		&& !DoesSymbolExist("TARGET_ALPHA")
		&& !DoesSymbolExist("TARGET_INTEL")
		&& !DoesSymbolExist("TARGET_POWERMAC"))

	{
		projOptions.m_projOptions &= ~(1 << (IDCD_PODLL-IDCD_POPROJBASE));
		SetProjOptionsSymbols();
		ReadValues();
		GetDlgItem(IDCD_PODLL)->EnableWindow(FALSE);
		GetDlgItem(IDCD_PONOTDLL)->EnableWindow(FALSE);
	}
	else
		ReadValues();

	UpdateData(FALSE);
    return TRUE;
}

void CProjOptionsDlg::OnDestroy()
{
    WriteValues();
    CSeqDlg::OnDestroy();
}

void CDocAppOptionsDlg::OnAdvanced()
{
	CAdvancedDlg dlg(GetParent());
	dlg.DoModal();

	// don't allow user to 'finish' from here if ActiveX doc server AND no file extension
	GetParent()->GetDlgItem(IDC_END)->EnableWindow(!(projOptions.m_bActiveDocServer && projOptions.m_names.strDocFileExt.IsEmpty()));
}

/////////////////////////////////////////////////////////////////////////////
// CDllProjOptionsDlg dialog


CDllProjOptionsDlg::CDllProjOptionsDlg(CWnd* pParent /*=NULL*/)
	: CProjOptionsDlg(CDllProjOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDllProjOptionsDlg)
	m_nDllType = -1;
	m_nVerbose = -1;
	//}}AFX_DATA_INIT
}


void CDllProjOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CProjOptionsDlg::DoDataExchange(pDX);
	if (IsLearningBox())
	{
		// m_nDllType is based on IDC_DLLTYPE as position 0
		// since we're getting rid of it for the learning box,
		// we need to adjust accordingly
		if (m_nDllType > 0)
			m_nDllType--;
		DDX_Radio(pDX, IDCD_PODLL, m_nDllType);
		m_nDllType++;
	}
	else
		DDX_Radio(pDX, IDC_DLLTYPE, m_nDllType);
	//{{AFX_DATA_MAP(CDllProjOptionsDlg)
		DDX_Radio(pDX, IDCD_POVERBOSE, m_nVerbose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDllProjOptionsDlg, CProjOptionsDlg)
	//{{AFX_MSG_MAP(CDllProjOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDllProjOptionsDlg::OnInitDialog() 
{
	if (IsLearningBox())
	{
		// this little exercise gets rid of the static linkage option and moves 
		// the other two options up on the screen
		CWnd* wnd = GetDlgItem(IDC_DLLTYPE);
		RECT rect1, rect2;
		if (wnd)
		{
			wnd->GetWindowRect(&rect1);
			ScreenToClient(&rect1);
			wnd->ShowWindow(SW_HIDE);
		}
		wnd = GetDlgItem(IDCD_PODLL);
		if (wnd)
		{
			wnd->GetWindowRect(&rect2);
			ScreenToClient(&rect2);
			wnd->ModifyStyle(0L, WS_GROUP);
			wnd->MoveWindow(&rect1);
			if (GetDlgItem(IDC_RADIO1))
				GetDlgItem(IDC_RADIO1)->MoveWindow(&rect2);
		}
	}

	CProjOptionsDlg::OnInitDialog();

	if (DoesSymbolExist("TARGET_MAC") && !IsMacWOSA())
	{
		// MacWOSA is not around, so select no WOSA support & disable the controls
		projOptions.m_options &= ~(1 << IDCD_POSOCKETS-IDCD_POBASE);
		RemoveSymbol("SOCKETS");
		EnableSockets(FALSE);
	}
	else
	{
		// WOSA is a-ok to be enabled.
		EnableSockets(TRUE);
	}

	ReadValues();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDllProjOptionsDlg::EnableSockets(BOOL bEnable)
{
	GetDlgItem(IDCD_SOCKETS)->EnableWindow(bEnable);
	GetDlgItem(IDC_SOCKETS_STATIC)->ShowWindow(bEnable ? SW_HIDE : SW_SHOW);
}

void CDllProjOptionsDlg::ReadValues()
{
	// DLL Type
	GetProjOptionsSymbols();
	GetDllSymbol();
	if (projOptions.m_bExtdll)
	{
		if (!(projOptions.m_projOptions & (1 << (IDCD_PODLL-IDCD_POPROJBASE))))
		{
			ASSERT(FALSE);
			projOptions.m_projOptions |= (1 << IDCD_PODLL-IDCD_POPROJBASE);
		}
		m_nDllType = DT_EXT;
	}
	else
	{
		if (projOptions.m_projOptions & (1 << IDCD_PODLL-IDCD_POPROJBASE))
			m_nDllType = DT_REGSHARED;
		else
			m_nDllType = DT_REGSTATIC;
	}
 
	// automation
	GetOleSymbols();
	((CButton*) GetDlgItem(IDCD_AUTOMATION))->SetCheck(projOptions.m_bOleAutomation);
	
	// sockets
	GetAppOptionsSymbols();
	if (projOptions.m_options & (1 << (IDCD_POSOCKETS-IDCD_POBASE)))
		((CButton*) GetDlgItem(IDCD_SOCKETS))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDCD_SOCKETS))->SetCheck(0);

	// verbose
	if (projOptions.m_projOptions & (1 << (IDCD_POVERBOSE-IDCD_POPROJBASE)))
		m_nVerbose = 0;
	else
		m_nVerbose = 1;

	UpdateData(FALSE);
}

void CDllProjOptionsDlg::WriteValues()
{
	VERIFY(UpdateData(TRUE));

	// DLL Type
	switch(m_nDllType)
	{
	case DT_REGSTATIC:
		projOptions.m_bExtdll = FALSE;
		projOptions.m_projOptions &= ~(1 << (IDCD_PODLL-IDCD_POPROJBASE));
		break;
	case DT_REGSHARED:
		projOptions.m_bExtdll = FALSE;
		projOptions.m_projOptions |= (1 << (IDCD_PODLL-IDCD_POPROJBASE));
		break;
	case DT_EXT:
		projOptions.m_bExtdll = TRUE;
		projOptions.m_projOptions |= (1 << (IDCD_PODLL-IDCD_POPROJBASE));
		break;
	}
	SetDllSymbol();

	// automation
	projOptions.m_nOleType = OLE_NO;	
	projOptions.m_bOleAutomation = ((CButton*) GetDlgItem(IDCD_AUTOMATION))->GetCheck();
	SetOleSymbols();
	
	// sockets
	if (((CButton*) GetDlgItem(IDCD_SOCKETS))->GetCheck())
		projOptions.m_options |= (1 << (IDCD_POSOCKETS-IDCD_POBASE));
	else
		projOptions.m_options &= ~(1 << (IDCD_POSOCKETS-IDCD_POBASE));
	SetAppOptionsSymbols();

	// verbose
	if (m_nVerbose == 0)
		projOptions.m_projOptions |= (1 << (IDCD_POVERBOSE-IDCD_POPROJBASE));
	else
		projOptions.m_projOptions &= ~(1 << (IDCD_POVERBOSE-IDCD_POPROJBASE));
	SetProjOptionsSymbols();
}

