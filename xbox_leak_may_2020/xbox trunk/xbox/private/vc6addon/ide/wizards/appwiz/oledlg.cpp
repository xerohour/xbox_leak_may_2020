// oledlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfcappwz.h"
#include "symbols.h"
#include "oleutil.h"
#include "oledlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COleDlg dialog

COleDlg::COleDlg(CWnd* pParent /*=NULL*/)
	: CSeqDlg(COleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COleDlg)
	m_bActiveDocServer = FALSE;
	m_nOleType = -1;
	m_nCompFile = -1;
	m_bAutomation = FALSE;
	m_bOcx = TRUE;
	m_bActiveDocContainer = FALSE;
	//}}AFX_DATA_INIT
	m_nLastCompFile = 1;

	// If the ActiveX doc checkbox is disabled and becomes enabled
	// later, its default is TRUE.
	//--------------REVIEW!!!
	// Changed to FALSE in order to not break the sniffs until the MFC docobj
	// support is actually in a drop.
	// Once MFC supports docobjs in 4.2, change back to TRUE and delete this comment.
	m_bActiveDocServerSave = FALSE;
	m_bActiveDocContainerSave = FALSE;
}

void COleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COleDlg)
	DDX_Control(pDX, IDC_XDOCCONTAINER, m_btnActiveDocContainer);
	DDX_Control(pDX, IDCD_DOCOBJ, m_btnActiveDocServer);
	DDX_Check(pDX, IDCD_DOCOBJ, m_bActiveDocServer);
	DDX_Radio(pDX, IDC_NO_INPLACE, m_nOleType);
	DDX_Radio(pDX, IDC_COMPFILE_RADIO, m_nCompFile);
	DDX_Check(pDX, IDCD_AUTOMATION, m_bAutomation);
	DDX_Check(pDX, IDCD_OCX, m_bOcx);
	DDX_Check(pDX, IDC_XDOCCONTAINER, m_bActiveDocContainer);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COleDlg, CSeqDlg)
	//{{AFX_MSG_MAP(COleDlg)
	ON_COMMAND_EX(IDC_NO_INPLACE, OnClickedRadio)
	ON_COMMAND_EX(IDC_CONTAINER, OnClickedRadio)
	ON_COMMAND_EX(IDC_MSERVER, OnClickedRadio)
	ON_COMMAND_EX(IDC_FSERVER, OnClickedRadio)
	ON_COMMAND_EX(IDC_CSERVER, OnClickedRadio)
	ON_BN_CLICKED(IDCD_DOCOBJ, OnClickedDocServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COleDlg message handlers

BOOL COleDlg::OnClickedRadio(UINT nID)
{
	CMainDlg* pMainDlg = (CMainDlg*) GetParent();
	pMainDlg->RedrawBitmap(BMPSIZE_SUM);
	UpdateCompFile();
	UpdateActiveDocServer(FALSE);
	return TRUE;
}

void COleDlg::OnClickedDocServer() 
{
	// don't allow user to 'finish' from here if ActiveX doc server AND no file extension
	GetParent()->GetDlgItem(IDC_END)->EnableWindow(!(m_btnActiveDocServer.GetCheck() && projOptions.m_names.strDocFileExt.IsEmpty()));
}

void COleDlg::UpdateActiveDocServer(BOOL fInitDialog)
{
	BOOL bOleServer = (m_nOleType != OLE_NO && m_nOleType != OLE_CONTAINER);
	BOOL bOleContainer =  (m_nOleType == OLE_CONTAINER || m_nOleType == OLE_CSERVER );
	if (bOleServer && DoesSymbolExist("TARGET_MAC"))
	{
		bOleServer = FALSE;	// with Mac targets, force it to be turned off
		m_bActiveDocServer = FALSE;
	}
	BOOL bActiveDocCEnabled;
	BOOL bActiveDocSEnabled;
	if (fInitDialog)
	{
		// get in a consistent state to begin with
		ASSERT(bOleServer || !m_bActiveDocServer);	// inconsistent initial state
		m_btnActiveDocServer.EnableWindow(bActiveDocSEnabled = bOleServer);
		m_btnActiveDocContainer.EnableWindow(bActiveDocCEnabled = bOleContainer);
		m_btnActiveDocServer.SetCheck(m_bActiveDocServer);
		m_btnActiveDocContainer.SetCheck(m_bActiveDocContainer);
	}
	else
	{
		bActiveDocSEnabled = m_btnActiveDocServer.IsWindowEnabled();
		bActiveDocCEnabled = m_btnActiveDocContainer.IsWindowEnabled();
	}

	if (bOleServer != bActiveDocSEnabled)
	{
		// must change enabledness of "activex doc server" button.
		if (bOleServer)
		{
			m_bActiveDocServer = m_bActiveDocServerSave;
		}
		else
		{
			m_bActiveDocServerSave = m_bActiveDocServer;
			m_bActiveDocServer = FALSE;
		}
		m_btnActiveDocServer.SetCheck(m_bActiveDocServer);
		m_btnActiveDocServer.EnableWindow(bOleServer);
		OnClickedDocServer();	// may need to reset 'Finish' button
	}
	if (bOleContainer != bActiveDocCEnabled)
	{
		// must change enabledness of "activex doc server" button.
		if (bOleContainer)
		{
			m_bActiveDocContainer = m_bActiveDocContainerSave;
		}
		else
		{
			m_bActiveDocContainerSave = m_bActiveDocContainer;
			m_bActiveDocContainer = FALSE;
		}
		m_btnActiveDocContainer.SetCheck(m_bActiveDocContainer);
		m_btnActiveDocContainer.EnableWindow(bOleContainer);
		// OnClickedDocServer();	// may need to reset 'Finish' button
	}
}

static UINT Obmp[] =
{
	IDB_B13NONE, IDB_B131000, IDB_B130100, IDB_B130010, IDB_B130001,
};

UINT COleDlg::ChooseBitmap()
{
	if (((CButton*) GetDlgItem(IDC_NO_INPLACE))->GetCheck())
		return Obmp[0];
	for (int i=0; i <= OLELAST-OLEBASE; i++)
	{
		if (((CButton*) GetDlgItem(OLEBASE+i))->GetCheck())
			break;
	}
	ASSERT (((CButton*) GetDlgItem(OLEBASE+i))->GetCheck());
	return Obmp[i+1];
}

void COleDlg::ReadValues()
{
	GetOleSymbols();
	m_nOleType = projOptions.m_nOleType;
	m_nCompFile = projOptions.m_bOleCompFile ? 1 : 0;
	m_bAutomation = projOptions.m_bOleAutomation;
	m_bOcx = projOptions.m_bOcx;
	m_bActiveDocServer = projOptions.m_bActiveDocServer;
	m_bActiveDocContainer = projOptions.m_bActiveDocContainer;
	UpdateData(FALSE);
}

void COleDlg::WriteValues()
{
	UpdateData(TRUE);
	projOptions.m_nOleType = m_nOleType;
	projOptions.m_bOleCompFile = (m_nCompFile == 1 ? TRUE : FALSE);
	projOptions.m_bOleAutomation = m_bAutomation;
	projOptions.m_bOcx = m_bOcx;
	projOptions.m_bActiveDocServer = m_bActiveDocServer;
	projOptions.m_bActiveDocContainer = m_bActiveDocContainer;
	
	// Record settings in symbol table
	SetOleSymbols();

	if (m_nOleType == OLE_MSERVER)
	{
		// If user selected mini-server, we must become SDI
		SetMDI(FALSE);
	}

}

BOOL COleDlg::CanDestroy()
{
	WriteValues();
	// If user selected a container or container-server, make sure
	//  CEditView isn't selected.
	if ((m_nOleType == OLE_CONTAINER || m_nOleType == OLE_CSERVER)
		&& projOptions.m_names.strBaseClass[classView] == "CEditView")
	{
		int nResult = AfxMessageBox(IDP_EDITVIEW_THEN_OLE, MB_OKCANCEL);
		if (nResult == IDCANCEL)
			return FALSE;
		projOptions.m_names.strBaseClass[classView] = "CView";
		SetBaseViewSymbols();
	}

	// If container/server support enabled, make sure m_bDBSimple is unset
	if (m_nOleType != OLE_NO && projOptions.m_bDBSimple)
	{
		int nResult = AfxMessageBox(IDP_SIMPLE_THEN_OLE, MB_OKCANCEL);
		if (nResult == IDCANCEL)
			return FALSE;
		projOptions.m_bDBSimple = FALSE;
		SetDBSymbols();
	}

	// If container unset, make sure "CRichEditView" is unset
	if (projOptions.m_names.strBaseClass[classView] == "CRichEditView"
		&& m_nOleType != OLE_CONTAINER && m_nOleType != OLE_CSERVER)
	{
		if (AfxMessageBox(IDP_RICHEDIT_THEN_NOOLE, MB_OKCANCEL) == IDCANCEL)
			return FALSE;
		projOptions.m_names.strBaseClass[classView] = "CView";
		SetBaseViewSymbols();
	}
	return TRUE;
}	

BOOL COleDlg::OnInitDialog()
{
	CSeqDlg::OnInitDialog();
	CenterWindow();
	CString tmp;
	if (projOptions.m_bNoDocView == TRUE)
	{
		// Turn off everything for no doc view
		ReadValues();
		//EnableOcx(FALSE);
		UpdateCompFile(FALSE);
		UpdateActiveDocServer(FALSE);
		GetDlgItem(IDC_NO_INPLACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONTAINER)->EnableWindow(FALSE);
		GetDlgItem(IDC_MSERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_FSERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CSERVER)->EnableWindow(FALSE);
		GetDlgItem(IDCD_AUTOMATION)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_NO_INPLACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONTAINER)->EnableWindow(TRUE);
		GetDlgItem(IDC_MSERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_FSERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_CSERVER)->EnableWindow(TRUE);
		GetDlgItem(IDCD_AUTOMATION)->EnableWindow(TRUE);
		projOptions.m_bOcx = TRUE;
		EnableOcx(TRUE);
		ReadValues();
		UpdateCompFile(TRUE);
		UpdateActiveDocServer(TRUE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COleDlg::UpdateCompFile(BOOL bCalledFromInit /* = FALSE */)
{
	UpdateData(TRUE);	// Get values
	CWnd* pCompFileRadio = GetDlgItem(IDC_COMPFILE_RADIO);
	CWnd* pCompFileRadio2 = GetDlgItem(IDC_COMPFILE_RADIO2);
	if (m_nOleType == OLE_NO && pCompFileRadio->IsWindowEnabled())
	{
		ASSERT (pCompFileRadio2->IsWindowEnabled());

		// No OLE selected.  Disable compound files, but remember its
		//  last setting
		if (!bCalledFromInit)
		{
			m_nLastCompFile = m_nCompFile;
			m_nCompFile = 0;
			UpdateData(FALSE);		// Set compfile to no
		}
		pCompFileRadio->EnableWindow(FALSE);
		pCompFileRadio2->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC3)->EnableWindow(FALSE);
	}
	else if (m_nOleType != OLE_NO && !pCompFileRadio->IsWindowEnabled())
	{
		ASSERT (!pCompFileRadio2->IsWindowEnabled());

		// OLE is selected, and compfile is currently disabled.  Thus,
		//  re-enable it with the previous setting.
		pCompFileRadio->EnableWindow(TRUE);
		pCompFileRadio2->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC3)->EnableWindow(TRUE);
		m_nCompFile = m_nLastCompFile;
		UpdateData(FALSE);		// Set compfile to prev. setting
	}
}


void COleDlg::EnableOcx(BOOL bEnable)
{
	GetDlgItem(IDCD_OCX)->EnableWindow(bEnable);
}
