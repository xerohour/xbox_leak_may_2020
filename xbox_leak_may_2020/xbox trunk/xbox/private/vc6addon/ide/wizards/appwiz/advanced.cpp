// advanced.cpp : implementation file
//

#include "stdafx.h"
#include "mfcappwz.h"
#include "symbols.h"
#include "advanced.h"
#include "ddxddv.h"
#include "lang.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL IsValidTitle(const char* szTitle);

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDlg dialog

CAdvancedDlg::CAdvancedDlg(CWnd* pParent)
	: CTabbedDialog(IDS_ADVANCED, pParent, UINT(-1), CTabbedDialog::commitOnTheFly)
{
	CDocStringsDlg* pDocStringsDlg = NULL;
	if (!DoesSymbolExist("NODOCVIEW"))
	{
		pDocStringsDlg = new CDocStringsDlg;
		AddTab((CDlgTab*) pDocStringsDlg);
	}

	CFrameStylesDlg* pFrameStylesDlg = new CFrameStylesDlg;
	AddTab((CDlgTab*) pFrameStylesDlg);

	/*if (IsMDI())
	{
		CChildFrameStylesDlg* pChildFrameStylesDlg = new CChildFrameStylesDlg;
#ifdef VS_PACKAGE
		AddPage(pChildFrameStylesDlg);
#else
		AddTab((CDlgTab*) pChildFrameStylesDlg);
#endif	// VS_PACKAGE
	}*/

	CString tmp;
	if (DoesSymbolExist("TARGET_MAC"))
	{
		CMacDlg* pMacDlg = new CMacDlg;
		AddTab((CDlgTab*) pMacDlg);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFrameStylesDlg dialog

CFrameStylesDlg::CFrameStylesDlg()
	: CDlgTab(CFrameStylesDlg::IDD, IDS_FRAME_STYLES)
{
	m_nIDHelp = CFrameStylesDlg::IDD;
	//{{AFX_DATA_INIT(CFrameStylesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFrameStylesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrameStylesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFrameStylesDlg, CDlgTab)
	//{{AFX_MSG_MAP(CFrameStylesDlg)
	ON_BN_CLICKED(IDC_MAXIMIZE, OnMaximize)
	ON_BN_CLICKED(IDC_MINIMIZE, OnMinimize)
	ON_BN_CLICKED(IDC_CHILD_MINIMIZE, OnChildMinimize)
	ON_BN_CLICKED(IDC_CHILD_MAXIMIZE, OnChildMaximize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameStylesDlg message handlers

BOOL CFrameStylesDlg::ValidateTab()
{
	// Record splitter
	CButton* pCheck = (CButton*) GetDlgItem(IDC_MAIN_SPLITTER);
	ASSERT(pCheck != NULL);
	if (pCheck->GetCheck())
	{
		if (!CanViewClassBeUsedWithSplitter(projOptions.m_names.strBaseClass[classView]))
		{
			CString strMessageText;
			AfxFormatString1(strMessageText, IDP_EDITVIEW_THEN_SPLITTER,
				projOptions.m_names.strBaseClass[classView]);
			int nResult = AfxMessageBox(strMessageText, MB_OKCANCEL);
			if (nResult == IDCANCEL)
				return FALSE;
			projOptions.m_names.strBaseClass[classView] = "CView";
			SetBaseViewSymbols();
		}
		projOptions.m_bSplitter = TRUE;
	}
	else
		projOptions.m_bSplitter = FALSE;
	
	// Record main frame styles
	projOptions.m_nFrameStyles = 0;
	for (int i = FRAME_STYLES_START; i <= FRAME_STYLES_END; i++)
	{
		CButton* pCheck = (CButton*) GetDlgItem(i);
		ASSERT(pCheck != NULL);
		if (pCheck->GetCheck())
			projOptions.m_nFrameStyles |= (1 << (i - FRAME_STYLES_START));
	}

	// Record child frame styles.
	projOptions.m_nChildFrameStyles = 0;
	for (i = CHILD_FRAME_STYLES_START; i <= CHILD_FRAME_STYLES_END; i++)
	{
		CButton* pCheck = (CButton*) GetDlgItem(i);
		ASSERT(pCheck != NULL);
		if (pCheck->GetCheck())
			projOptions.m_nChildFrameStyles |= (1 << (i - CHILD_FRAME_STYLES_START));
	}

	return TRUE;
}

void CFrameStylesDlg::CommitTab()
{
	ValidateTab();
}

BOOL CFrameStylesDlg::OnInitDialog()
{
	CDlgTab::OnInitDialog();

	// Display main frame styles
	for (int i = FRAME_STYLES_START; i <= FRAME_STYLES_END; i++)
	{
		CButton* pCheck = (CButton*) GetDlgItem(i);
		ASSERT(pCheck != NULL);
		if (projOptions.m_nFrameStyles & (1 << (i - FRAME_STYLES_START)))
			pCheck->SetCheck(TRUE);
		else
			pCheck->SetCheck(FALSE);
	}
	OnMaximize();
	OnMinimize();

	CWnd *pWndTemp;

	// Display child frame styles
	if (IsMDI())
	{
		// We're MDI, so display the current child frame style settings
		for (int i = CHILD_FRAME_STYLES_START; i <= CHILD_FRAME_STYLES_END; i++)
		{
			CButton* pCheck = (CButton*) GetDlgItem(i);
			ASSERT(pCheck != NULL);
			if (pCheck != NULL)
			{
				pCheck->EnableWindow(TRUE);
				if (projOptions.m_nChildFrameStyles & (1 << (i - CHILD_FRAME_STYLES_START)))
					pCheck->SetCheck(TRUE);
				else
					pCheck->SetCheck(FALSE);
			}
		}

		pWndTemp = GetDlgItem(IDC_CHILD_STATIC);
		if (pWndTemp != NULL)
			pWndTemp->EnableWindow(TRUE);

		OnChildMaximize();
		OnChildMinimize();
		// Hide note about SDI main frame Minimized and Maximized styles

		pWndTemp = GetDlgItem(IDC_NOTE);
		if (pWndTemp != NULL)
			pWndTemp->ShowWindow(SW_HIDE);

		pWndTemp = GetDlgItem(IDC_NOTE_NOMINMAX);
		if (pWndTemp != NULL)
			pWndTemp->ShowWindow(SW_HIDE);
	}
	else
	{
		// We're SDI so disable all the child frame style controls
		for (int i = CHILD_FRAME_STYLES_START; i <= CHILD_FRAME_STYLES_END; i++)
		{
			CButton* pCheck = (CButton*) GetDlgItem(i);
			ASSERT(pCheck != NULL);
			pCheck->EnableWindow(FALSE);
		}

		pWndTemp = GetDlgItem(IDC_CHILD_STATIC);
		if (pWndTemp != NULL)
			pWndTemp->EnableWindow(FALSE);
	}

	// Display splitter option
	CButton* pCheck = (CButton*) GetDlgItem(IDC_MAIN_SPLITTER);
	ASSERT(pCheck != NULL);
	if (pCheck != NULL)
	{
		if (projOptions.m_bNoDocView)
		{
			projOptions.m_bSplitter = FALSE;
			pCheck->EnableWindow(FALSE);
		} else {
			pCheck->EnableWindow(TRUE);
		}

		if (projOptions.m_bSplitter)
			pCheck->SetCheck(TRUE);
		else
			pCheck->SetCheck(FALSE);
	}

	return TRUE;
}

// These prohibit the user from selecting Minimize & Maximize simultaneously.

void CFrameStylesDlg::OnMaximize()
{
	if (IsDlgButtonChecked(IDC_MAXIMIZE))
		GetDlgItem(IDC_MINIMIZE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_MINIMIZE)->EnableWindow(TRUE);
}

void CFrameStylesDlg::OnMinimize()
{
	if (IsDlgButtonChecked(IDC_MINIMIZE))
		GetDlgItem(IDC_MAXIMIZE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_MAXIMIZE)->EnableWindow(TRUE);
}

void CFrameStylesDlg::OnChildMinimize()
{
	if (IsDlgButtonChecked(IDC_CHILD_MINIMIZE))
		GetDlgItem(IDC_CHILD_MAXIMIZE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CHILD_MAXIMIZE)->EnableWindow(TRUE);
}

void CFrameStylesDlg::OnChildMaximize()
{
	if (IsDlgButtonChecked(IDC_CHILD_MAXIMIZE))
		GetDlgItem(IDC_CHILD_MINIMIZE)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CHILD_MINIMIZE)->EnableWindow(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CDocStringsDlg dialog

CDocStringsDlg::CDocStringsDlg()
	: CDlgTab(CDocStringsDlg::IDD, IDS_DOC_STRINGS)
{
	m_nCurrLang = 0;
	m_nIDHelp = CDocStringsDlg::IDD;
	//{{AFX_DATA_INIT(CDocStringsDlg)
	//}}AFX_DATA_INIT
}

void CDocStringsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocStringsDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_DOC_TYPE, m_strDocTag);
	m_strDocTag.ReleaseBuffer();
	DDV_SymbolCanBeginWithNumber(pDX, m_strDocTag);
	DDV_MaxChars(pDX, m_strDocTag, MAX_TAG);
	
	DDX_Text(pDX, IDC_DOC_EXT, m_strDocFileExt);
	m_strDocFileExt.ReleaseBuffer();
	if (!m_strDocFileExt.IsEmpty())
	{
		DDV_FileName(pDX, m_strDocFileExt, NULL);
	}
	
	DDX_Text(pDX, IDC_FILE_NEW, m_strDocFileNew);
	m_strDocFileNew.ReleaseBuffer();
	DDV_MaxChars(pDX, m_strDocFileNew, MAX_OLE_SHORT);
	
	DDX_Text(pDX, IDC_FILTER, m_strDocFilter);
	m_strDocFilter.ReleaseBuffer();
	DDV_MaxChars(pDX, m_strDocFilter, MAX_FILTER);
	
	DDX_Text(pDX, IDC_REG_ID, m_strDocRegID);
	m_strDocRegID.ReleaseBuffer();
	DDV_ProgID(pDX, m_strDocRegID);
	DDV_MaxChars(pDX, m_strDocRegID, MAX_PROGID);
	
	DDX_Text(pDX, IDC_REG_NAME, m_strDocRegName);
	m_strDocRegName.ReleaseBuffer();
	DDV_MaxChars(pDX, m_strDocRegName, MAX_LONGNAME);

	DDX_Text(pDX, IDC_APP_TITLE, m_strTitle);
	m_strTitle.ReleaseBuffer();
	DDV_Title(pDX, m_strTitle);
}

BEGIN_MESSAGE_MAP(CDocStringsDlg, CDlgTab)
	//{{AFX_MSG_MAP(CDocStringsDlg)
	ON_EN_CHANGE(IDC_DOC_TYPE, OnChangeDocType)
	ON_EN_CHANGE(IDC_DOC_EXT, OnChangeDocExt)
	ON_EN_CHANGE(IDC_FILE_NEW, OnChangeFileNew)
	ON_EN_CHANGE(IDC_FILTER, OnChangeFilter)
	ON_EN_CHANGE(IDC_REG_ID, OnChangeRegId)
	ON_EN_CHANGE(IDC_REG_NAME, OnChangeRegName)
	//}}AFX_MSG_MAP
	// FUTURE: Multiple langs:
	//ON_CBN_SELCHANGE(IDC_LANG_COMBO, OnSelchangeLangCombo)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDocStringsDlg message handlers

BOOL CDocStringsDlg::OnInitDialog()
{
	CDlgTab::OnInitDialog();

	// Fill in lang combo box
	// FUTURE: Multiple langs:
	/*CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_LANG_COMBO);
	ASSERT_VALID(pCombo);
	pCombo->SetRedraw(FALSE);
	int nSize = langDlls.GetSize();
	for (int i = 0; i < nSize && !langDlls.m_astrNameLang[i].IsEmpty(); i++)
		pCombo->AddString(langDlls.m_astrNameLang[i]);
	pCombo->SetRedraw(TRUE);
	pCombo->SetCurSel(m_nCurrLang);*/
	GetDlgItem(IDC_LANG_COMBO)->SetWindowText(langDlls.m_astrNameLang[0]);

	// Display the current (first) language's strings
	DisplayCurrLang();
	return TRUE;
}

BOOL CDocStringsDlg::ValidateTab()
{
	return ValidateCurrLang();
}

// FUTURE: Multiple Langs:
/*void CDocStringsDlg::OnSelchangeLangCombo() 
{
	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_LANG_COMBO);
	ASSERT_VALID(pCombo);

	int nNewSel = pCombo->GetCurSel();

	if (nNewSel == m_nCurrLang)
		// When this fcn calls SetCurSel, this will be the case, and
		//  don't want to recurse!
		return;

	if (!ValidateCurrLang())
	{
		// Data not validated correctly.  Restore previous combo selection
		pCombo->SetCurSel(m_nCurrLang);
		return;
	}

	// Data validated OK, so change to new lang
	m_nCurrLang = nNewSel;
	DisplayCurrLang();
}*/

BOOL CDocStringsDlg::ValidateCurrLang()
{
	// Do validation for all fields
	if (!UpdateData(TRUE))
		return FALSE;

	// Record nonlocalized strings
	projOptions.m_names.strDocFileExt = m_strDocFileExt;
	projOptions.m_names.strDocRegID = m_strDocRegID;

	// Record localized strings
	langDlls.m_astrTitle[m_nCurrLang] = m_strTitle;
	langDlls.m_astrDocTag[m_nCurrLang] = m_strDocTag;
	langDlls.m_astrDocFileNew[m_nCurrLang] = m_strDocFileNew;
	langDlls.m_astrDocFilter[m_nCurrLang] = m_strDocFilter;
	langDlls.m_astrDocRegName[m_nCurrLang] = m_strDocRegName;

	return TRUE;
}

void CDocStringsDlg::DisplayCurrLang()
{
	// Unlocalized strings
	m_strDocFileExt = projOptions.m_names.strDocFileExt;
	m_strDocRegID = projOptions.m_names.strDocRegID;

	// Localized strings
	m_strTitle = langDlls.m_astrTitle[m_nCurrLang];
	m_strDocTag = langDlls.m_astrDocTag[m_nCurrLang];
	m_strDocFileNew = langDlls.m_astrDocFileNew[m_nCurrLang];
	m_strDocFilter = langDlls.m_astrDocFilter[m_nCurrLang];
	m_strDocRegName = langDlls.m_astrDocRegName[m_nCurrLang];

	UpdateData(FALSE);
}

void CDocStringsDlg::CommitTab()
{
	ValidateTab();
}

void CDocStringsDlg::UpdateDocFields()
{
	if (m_strDocFileExt.Left(1) == ".")
		m_strDocFileExt = m_strDocFileExt.Right(m_strDocFileExt.GetLength()-1);

	// Special case: update Filter for ALL langs, since it tracks the extension,
	//  which is constant over all langs
	int nSize = langDlls.GetSize();
	for (int i = 0; i < nSize && langDlls.IsValidEntry(i); i++)
	{
		if (langDlls.m_abUpdateFilter[i])
		{
			if (m_strDocFileExt.IsEmpty() || langDlls.m_astrDocTag[i].IsEmpty())
				langDlls.m_astrDocFilter[i].Empty();
			else
			{
				if (langDlls.m_abFilesAfter[i])
					langDlls.m_astrDocFilter[i] = langDlls.m_astrDocTag[i] + " "
						+ langDlls.m_astrFiles[i];
				else
					langDlls.m_astrDocFilter[i] = langDlls.m_astrFiles[i] + " "
						+ langDlls.m_astrDocTag[i];
				langDlls.m_astrDocFilter[i] += " (*." + m_strDocFileExt + ")";
			}
			if (i == m_nCurrLang)
				m_strDocFilter = langDlls.m_astrDocFilter[i];
		}
	}

	if (m_strDocTag == "")
	{
		if (langDlls.m_abUpdateFileNew[m_nCurrLang])
			m_strDocFileNew = "";
		if (projOptions.m_bUpdateRegID)
			m_strDocRegID = "";
		if (langDlls.m_abUpdateRegName[m_nCurrLang])
			m_strDocRegName = "";
	}
	else
	{
		if (langDlls.m_abUpdateFileNew[m_nCurrLang])
			m_strDocFileNew = m_strDocTag;
		if (langDlls.m_abUpdateRegName[m_nCurrLang])
			m_strDocRegName = m_strDocTag + " Document";
	}
}

void CDocStringsDlg::OnChangeDocType()
{
	// Read doc tag field
	GetDlgItem(IDC_DOC_TYPE)->GetWindowText(m_strDocTag);
	m_strDocTag.ReleaseBuffer();

	// Update other fields accordingly
	UpdateDocFields();
	UpdateData(FALSE);
}

void CDocStringsDlg::OnChangeDocExt()
{
	// Read file extension field
	GetDlgItem(IDC_DOC_EXT)->GetWindowText(m_strDocFileExt);
	m_strDocFileExt.ReleaseBuffer();

	// Update other fields accordingly
	UpdateDocFields();
	UpdateData(FALSE);
}

// Four functions below: If user alters these fields, cease their
//  tracking.  Also, read the value so a future UpdateData(FALSE)
//  works as expected.

void CDocStringsDlg::OnChangeFileNew()
{
	langDlls.m_abUpdateFileNew[m_nCurrLang] = FALSE;
	GetDlgItem(IDC_FILE_NEW)->GetWindowText(m_strDocFileNew);
	m_strDocFileNew.ReleaseBuffer();
}

void CDocStringsDlg::OnChangeFilter()
{
	langDlls.m_abUpdateFilter[m_nCurrLang] = FALSE;
	GetDlgItem(IDC_FILTER)->GetWindowText(m_strDocFilter);
	m_strDocFilter.ReleaseBuffer();
}

void CDocStringsDlg::OnChangeRegId()
{
	projOptions.m_bUpdateRegID = FALSE;
	GetDlgItem(IDC_REG_ID)->GetWindowText(m_strDocRegID);
	m_strDocRegID.ReleaseBuffer();
}

void CDocStringsDlg::OnChangeRegName()
{
	langDlls.m_abUpdateRegName[m_nCurrLang] = FALSE;
	GetDlgItem(IDC_REG_NAME)->GetWindowText(m_strDocRegName);
	m_strDocRegName.ReleaseBuffer();
}


/////////////////////////////////////////////////////////////////////////////
// CMacDlg dialog


CMacDlg::CMacDlg()
	: CDlgTab(CMacDlg::IDD, IDS_MAC)
{
	m_nIDHelp = CMacDlg::IDD;
	//{{AFX_DATA_INIT(CMacDlg)
	//}}AFX_DATA_INIT
	m_strDocFileCreator = projOptions.m_names.strDocFileCreator;
	m_strDocFileType = projOptions.m_names.strDocFileType;
	m_strDocFilter = projOptions.m_names.strMacFilter;
	m_bTrackFilter = TRUE;
}

void CMacDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgTab::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMacDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_DOC_CREATOR, m_strDocFileCreator);
	DDV_MaxChars(pDX, m_strDocFileCreator, 4);
	DDX_Text(pDX, IDC_FILE_TYPE, m_strDocFileType);
	DDV_MaxChars(pDX, m_strDocFileType, 4);
	DDX_Text(pDX, IDC_FILTER, m_strDocFilter);
}

BEGIN_MESSAGE_MAP(CMacDlg, CDlgTab)
	//{{AFX_MSG_MAP(CMacDlg)
	ON_EN_CHANGE(IDC_FILE_TYPE, OnChangeFileType)
	ON_EN_CHANGE(IDC_FILTER, OnChangeFilter)
	ON_EN_CHANGE(IDC_DOC_CREATOR, OnChangeDocCreator)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMacDlg message handlers

BOOL CMacDlg::ValidateTab()
{
	if (!UpdateData(TRUE))
		return FALSE;
	
	projOptions.m_names.strDocFileCreator = m_strDocFileCreator;
	projOptions.m_names.strDocFileType = m_strDocFileType;
	projOptions.m_names.strMacFilter = m_strDocFilter;
	projOptions.m_names.strDocFileCreator.MakeUpper();
	projOptions.m_names.strDocFileType.MakeUpper();

	return TRUE;
}

void CMacDlg::CommitTab()
{
	ValidateTab();
}

void CMacDlg::OnChangeFileType() 
{
	if (!m_bTrackFilter)
		return;
	
	GetDlgItem(IDC_FILE_TYPE)->GetWindowText(m_strDocFileType);
	m_strDocFileType.ReleaseBuffer();
	
	// TODO: Use langDlls
/*	if (projOptions.m_bFilesAfter)
		m_strDocFilter = m_strDocFileType + " "
			+ projOptions.m_strFiles;
	else
		m_strDocFilter = projOptions.m_strFiles + " "
			+ m_strDocFileType;*/

	UpdateData(FALSE);
}

void CMacDlg::OnChangeFilter() 
{
	m_bTrackFilter = FALSE;	
	GetDlgItem(IDC_FILTER)->GetWindowText(m_strDocFilter);
}

void CMacDlg::OnChangeDocCreator() 
{
	GetDlgItem(IDC_DOC_CREATOR)->GetWindowText(m_strDocFileCreator);	
}

