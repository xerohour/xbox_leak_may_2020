// projtype.cpp : implementation file
//

#include "stdafx.h"
#include <ver.h>
#include "mfcappwz.h"
#include "projtype.h"
#include "lang.h"
#include "symbols.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CProjDlg dialog

CProjDlg::CProjDlg(CWnd* pParent /*=NULL*/)
    : CSeqDlg(CProjDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CProjDlg)
    m_nPTRadio = -1;
	m_bDocView = TRUE;
	//}}AFX_DATA_INIT
}

void CProjDlg::DoDataExchange(CDataExchange* pDX)
{
    CSeqDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProjDlg)
    DDX_Radio(pDX, IDC_PTRADIO, m_nPTRadio);
	DDX_Check(pDX, IDC_DOCVIEW, m_bDocView);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProjDlg, CSeqDlg)
    //{{AFX_MSG_MAP(CProjDlg)
	ON_COMMAND_EX(IDC_PTRADIO, OnClickedRadio)
    ON_WM_DESTROY()
	ON_COMMAND_EX(IDC_PTMDI, OnClickedRadio)
	ON_COMMAND_EX(IDC_PTDLG, OnClickedRadio)
	ON_COMMAND_EX(IDC_DOCVIEW, OnClickedRadio)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjDlg message handlers



BOOL CProjDlg::OnInitDialog()
{
    CSeqDlg::OnInitDialog();
    CenterWindow();
	// LATER: when multiple langs are allowed, uncomment
	//VERIFY(m_chklstLang.SubclassDlgItem(IDC_RSC_LANG, this));

    ReadValues();
	
	/*if (m_chklstLang.GetCount() == 1)
	{
		// If there's only one item, check it & disable the listbox
		m_chklstLang.SetItemData(0, 1);
		m_chklstLang.EnableWindow(FALSE);
	}*/

	if( projOptions.m_nProjType !=  PROJTYPE_DLG )
		GetDlgItem(IDC_DOCVIEW)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_DOCVIEW)->EnableWindow(FALSE);
    
	// this is for the Forms AppWizard. A lot simpler to do if we put the code here. bobz 6/6/97
	CString strExplanation;
	GetSymbol("EXPLAIN_PROJTYPE", strExplanation);

	if (!strExplanation.IsEmpty())
	{
		// move controls to make room for explanation
		CRect crect;
		CSize csize;
		CWnd *pwnd1;
		CWnd *pwnd2;

		pwnd1 = GetDlgItem(IDC_LBL_LANG);
		ASSERT(pwnd1);
		pwnd1->GetWindowRect(&crect);
		csize = crect.Size();
		pwnd2 = GetDlgItem(IDC_MOVE_LBL_LANG);
		ASSERT(pwnd2);
		pwnd2->GetWindowRect(&crect);
		ScreenToClient(&crect); //top and left ok, reset size
		crect.SetRect(crect.left, crect.top, 
			crect.left + csize.cx, crect.top + csize.cy);
		pwnd1->MoveWindow(&crect);

		pwnd1 = GetDlgItem(IDC_RSC_LANG);
		ASSERT(pwnd1);
		pwnd1->GetWindowRect(&crect);
		csize = crect.Size();
		pwnd2 = GetDlgItem(IDC_MOVE_RSC_LANG);
		ASSERT(pwnd2);
		pwnd2->GetWindowRect(&crect);
		ScreenToClient(&crect); //top and left ok, reset size
		crect.SetRect(crect.left, crect.top, 
			crect.left + csize.cx, crect.top + csize.cy);
		pwnd1->MoveWindow(&crect);

		// now fill explain window text
		pwnd1 = GetDlgItem(IDC_EXPLAIN);
		ASSERT(pwnd1);
		pwnd1->SetWindowText(strExplanation);
		pwnd1->ShowWindow(SW_SHOWNOACTIVATE);	
	}


    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProjDlg::WriteProjType()
{
	projOptions.m_nProjType = m_nPTRadio;
	projOptions.m_bNoDocView = !m_bDocView;
	if(!m_bDocView)
	{
		// No Doc View architecture support means no printing
		projOptions.m_options &= ~(1 << (IDCD_POPRINT-IDCD_POBASE) );
		projOptions.m_nOleType = OLE_NO;
		projOptions.m_bOleAutomation = FALSE;
		projOptions.m_bSplitter = FALSE;
    	projOptions.m_nProjStyle = PS_MFCNORMAL;       // Explorer or Normal.
		projOptions.m_bDBSimple = FALSE;
		projOptions.m_bDBHeader = FALSE;
		projOptions.m_options &= ~(1 << IDCD_POMAPI-IDCD_POBASE);
		projOptions.m_options &= ~(1 << IDCD_POSOCKETS-IDCD_POBASE);

		RemoveSymbol("MAPI");
		RemoveSymbol("SOCKETS");

		RemoveSymbol("DB_COLSINFO");
		RemoveSymbol("DB_VARSINFO");
		RemoveSymbol("DB_SOURCE");
		RemoveSymbol("DB_DSN");

		SetProjOptionsSymbols();
		SetSplitterSymbols();
		SetDBSymbols();
		SetOleSymbols();
	}
}

BOOL CProjDlg::OnClickedRadio(UINT nID)
{
    UpdateData(TRUE);
	WriteProjType();

	if( projOptions.m_nProjType !=  PROJTYPE_DLG )
		GetDlgItem(IDC_DOCVIEW)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_DOCVIEW)->EnableWindow(FALSE);

	CMainDlg* pMainDlg = (CMainDlg*) GetParent();
	pMainDlg->RedrawBitmap(BMPSIZE_SUM);
	return TRUE;
}

static UINT Pbmp[] =
{
	IDB_B11100, IDB_B11010, IDB_B11001, IDB_B110001,
};

UINT CProjDlg::ChooseBitmap()
{
	for (int i=0; i < 4; i++)
	{
		if (((CButton*) GetDlgItem(IDC_PTRADIO+i))->GetCheck())
			break;
	}
	ASSERT (((CButton*) GetDlgItem(IDC_PTRADIO+i))->GetCheck());
	return Pbmp[i];
}

void CProjDlg::OnDestroy()
{
    WriteValues();
    CSeqDlg::OnDestroy();
}

BOOL CProjDlg::CanDestroy()
{
    WriteValues();

	if (!langDlls.LoadLibs())
		return FALSE;

	if (!langDlls.LoadLangSpecificDefaults())
		return FALSE;

    // If MDI selected, make sure m_bDBSimple is unset & we're not a mini-server.
    if (projOptions.m_nProjType == PROJTYPE_MDI)
    {
        if (projOptions.m_bDBSimple)
        {
            int nResult = AfxMessageBox(IDP_SIMPLE_THEN_MDI, MB_OKCANCEL);
            if (nResult == IDCANCEL)
                return FALSE;
            
            projOptions.m_bDBSimple = FALSE;
			SetDBSymbols();
        }

        // If we were a mini-server, must now become a full-server
        if (projOptions.m_nOleType == OLE_MSERVER)
        {
            int nResult = AfxMessageBox(IDP_MSERVER_THEN_MDI, MB_OKCANCEL);
            if (nResult == IDCANCEL)
                return FALSE;
            
            projOptions.m_nOleType = OLE_FSERVER;
			SetOleSymbols();
        }
    }

	return TRUE;
}

    

void CProjDlg::WriteValues()
{
    UpdateData(TRUE);
	projOptions.m_nProjType = m_nPTRadio;
	projOptions.m_bNoDocView = !m_bDocView;
	WriteProjType();
	SetProjectTypeSymbol();

    // If we're now dlg-based, unset some symbols
    if (projOptions.m_nProjType == PROJTYPE_DLG)
    {
        ResetOleSymbols();
        RemoveSymbol("CRecordView");
		RemoveSymbol("DB");

		// Set the default to OLE control hosting ... the user gets to change
		// it in later dialogs, if desired.
		projOptions.m_bOcx = TRUE;
		SetSymbol("OLECTL", "1");
    }
    else
    {
        // Otherwise, set the ole symbols again.
        SetOleSymbols();
        if (projOptions.m_names.strBaseClass[classView] == "CRecordView")
            SetSymbol("CRecordView", "1");
    }
	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_RSC_LANG);
	langDlls.WriteLangDlls(pCombo);
	SetLanguageSymbols();
}

void CProjDlg::ReadValues()
{
	GetProjectTypeSymbol();
	GetLanguageSymbols();
    m_nPTRadio = projOptions.m_nProjType;
    m_bDocView = !projOptions.m_bNoDocView;
    UpdateData(FALSE);

	// Initialize language dll check list
	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_RSC_LANG);
	if (!langDlls.m_AvailableLangDlls.InitLangDllList(pCombo))
	{
		PostMessage(WM_GOTO_BEGINNING, 0, 0);
		return;
	}
}

