/////////////////////////////////////////////////////////////////////////////
// suitepg.cpp
//
// email	date		change
// briancr	06/15/95	created
//
// copyright 1995 Microsoft

// Implementation of the CSuitePage class

#include "stdafx.h"
#include "suitepg.h"
#include "graphdlg.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuitePage property page

IMPLEMENT_DYNCREATE(CSuitePage, CPropertyPage)

CSuitePage::CSuitePage(CSuiteDoc* pSuiteDoc)
: CPropertyPage(CSuitePage::IDD),
  m_pSuiteDoc(pSuiteDoc)
{
	// the suite doc must be valid
	ASSERT(m_pSuiteDoc);

	//{{AFX_DATA_INIT(CSuitePage)
	m_bLoopTests = FALSE;
	m_bCleanUpBefore = TRUE;
	m_bCleanUpAfter = TRUE;
	m_bRandomize = FALSE;
	m_nRandomSeed = 0;
	m_nLimit = 1;
	m_nLimitNum = 100;
	m_LimitTime = CTimeSpan(0, 1, 0, 0);
	m_nCycleNumber = 1;
	//}}AFX_DATA_INIT

	// initialize dialog with data from the doc
	m_bCleanUpBefore = m_pSuiteDoc->GetCleanUpBefore();
	m_bCleanUpAfter = m_pSuiteDoc->GetCleanUpAfter();
	m_nCycleNumber = m_pSuiteDoc->GetCycleNumber();
	m_bLoopTests = m_pSuiteDoc->GetLoopTests();
	m_bRandomize = m_pSuiteDoc->GetRandomize();
	m_nRandomSeed = m_pSuiteDoc->GetRandomSeed();
	m_nLimit = (int)m_pSuiteDoc->GetRandomLimit();
	m_nLimitNum = m_pSuiteDoc->GetRandomLimitNum();
	m_LimitTime = m_pSuiteDoc->GetRandomLimitTime();


}

CSuitePage::~CSuitePage()
{
}

void CSuitePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSuitePage)
	DDX_Control(pDX, IDC_SuiteLoopTests, m_checkLoopTests);
	DDX_Check(pDX, IDC_SuiteLoopTests, m_bLoopTests);
	DDX_Check(pDX, IDC_SuiteCleanUpBefore, m_bCleanUpBefore);
	DDX_Check(pDX, IDC_SuiteCleanUpAfter, m_bCleanUpAfter);
	DDX_Check(pDX, IDC_SuiteRandomize, m_bRandomize);
	DDX_Text(pDX, IDE_SuiteRandomSeed, m_nRandomSeed);
	DDX_Radio(pDX, IDR_RandomLimitNone, m_nLimit);
	DDX_Text(pDX, IDE_RandomLimitNum, m_nLimitNum);
	DDX_Text(pDX, IDE_CycleNumber, m_nCycleNumber);
	DDX_TimeSpan(pDX, IDE_RandomLimitTime, m_LimitTime);
	//}}AFX_DATA_MAP
}

BOOL CSuitePage::OnSetActive(void)
{
	// call the base class
	if (!CPropertyPage::OnSetActive()) {
		return FALSE;
	}

	// set up UI
	UpdateUI();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CSuitePage, CPropertyPage)
	//{{AFX_MSG_MAP(CSuitePage)
	ON_BN_CLICKED(IDC_SuiteRandomize, UpdateUI)
	ON_BN_CLICKED(IDC_SuiteLoopTests, OnContinuallyLoop)
	ON_BN_CLICKED(IDB_SuiteViewGraphs, OnViewGraph)
	ON_BN_CLICKED(IDR_RandomLimitNone, UpdateUI)
	ON_BN_CLICKED(IDR_RandomLimitNum, UpdateUI)
	ON_BN_CLICKED(IDR_RandomLimitTime, UpdateUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSuitePage::UpdateUI(void)
{
	// retrieve settings from the UI
	UpdateData(TRUE);

	// gray the randomize check box and graph button based on ability to randomize
	((CButton*)GetDlgItem(IDC_SuiteRandomize))->EnableWindow(m_pSuiteDoc->CanRandomize());
	((CButton*)GetDlgItem(IDB_SuiteViewGraphs))->EnableWindow(m_pSuiteDoc->CanRandomize());

	// gray or enable the limit number edit field based on the radio button selected
	((CWnd*)GetDlgItem(IDE_RandomLimitNum))->EnableWindow(m_nLimit==1);
	
	// gray or enable the limit time edit field based on the radio button selected
	((CWnd*)GetDlgItem(IDE_RandomLimitTime))->EnableWindow(m_nLimit==2);

	// gray (or enable) all the controls related to randomizing based on the state of the randomize check box
	for (int i = IDS_SuiteRandomSeed; i < IDB_SuiteViewGraphs; i++) {
		((CWnd*)GetDlgItem(i))->EnableWindow(m_bRandomize);
	}

	// gray all the randomization controls based on the state of the continuously loop check box
	// (we can't loop and randomize)
	if (m_bLoopTests) {
		for (i = IDC_SuiteRandomize; i < IDB_SuiteViewGraphs; i++) {
			((CWnd*)GetDlgItem(i))->EnableWindow(FALSE);
		}
	}
}

void AFXAPI CSuitePage::DDX_TimeSpan(CDataExchange* pDX, int nIDC, CTimeSpan& timespan)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate) {
		GetTimeSpan(hWndCtrl, timespan);
	}
	else {
		SetTimeSpan(hWndCtrl, timespan);
	}
}

void CSuitePage::GetTimeSpan(HWND hWnd, CTimeSpan& timespan)
{
	TCHAR szWindowText[20];
	::GetWindowText(hWnd, szWindowText, 19);
	TCHAR* psz;
	int nDays = 0;
	int nHours = 0;
	int nMinutes = 0;

	// we want to parse this string backwards
	// so start at the end
	psz = szWindowText + _tcslen(szWindowText) - 1;
	
	// skip white space
	while (*psz == ' ') {
		psz--;
	}
	*(psz+1) = '\0';
	// go to the colon for minutes
	while (psz != szWindowText && *psz != ':') {
		psz--;
	}
	if (psz == szWindowText) {
		nMinutes = _ttoi(psz);
		goto FigureTime;
	}
	else {
		nMinutes = _ttoi(psz+1);
	}
	*(psz) = '\0';
	// go to the colon for hours
	while (psz != szWindowText && *psz != ':') {
		psz--;
	}
	if (psz == szWindowText) {
		nHours = _ttoi(psz);
		goto FigureTime;
	}
	else {
		nHours = _ttoi(psz+1);
	}
	*(psz) = '\0';
	nDays = _ttoi(szWindowText);
	
FigureTime:
	timespan = CTimeSpan(nDays, nHours, nMinutes, 0);
}

void CSuitePage::SetTimeSpan(HWND hWnd, CTimeSpan& timespan)
{
	CString str = timespan.Format("%D:%H:%M");
	::SetWindowText(hWnd, str);
}

/////////////////////////////////////////////////////////////////////////////
// CSuitePage message handlers

void CSuitePage::OnOK() 
{
	// store the dialog data in the doc
	m_pSuiteDoc->SetCleanUpBefore(m_bCleanUpBefore);
	m_pSuiteDoc->SetCleanUpAfter(m_bCleanUpAfter);
	m_pSuiteDoc->SetCycleNumber(m_nCycleNumber);
	m_pSuiteDoc->SetLoopTests(m_bLoopTests);
	m_pSuiteDoc->SetRandomize(m_bRandomize);
	m_pSuiteDoc->SetRandomSeed(m_nRandomSeed);
	m_pSuiteDoc->SetRandomLimit((CSuiteDoc::RandomLimit)m_nLimit);
	m_pSuiteDoc->SetRandomLimitNum(m_nLimitNum);
	m_pSuiteDoc->SetRandomLimitTime(m_LimitTime);
	
	CDialog::OnOK();
}

void CSuitePage::OnContinuallyLoop(void)
{
	UpdateData(TRUE);
	m_bRandomize = FALSE;
	//need to disable edit box if loop checkbox enabled
	//and set cyclenumber to 1.
//	if (((CButton*)GetCheck(IDC_SuiteLoopTests)) != 0)
	if(m_checkLoopTests.GetCheck() != 0) 
//	or if((CButton*)GetDlgItem(IDC_SuiteLoopTests)) ->GetCheck !=0)
	{
		m_nCycleNumber = 1;
		UpdateData(FALSE);
		//disable cyclenumber editbox if looptest checkbox is checked
		(GetDlgItem(IDE_CycleNumber))->EnableWindow(FALSE);
	}
	else
	{
		(GetDlgItem(IDE_CycleNumber))->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
	UpdateUI();
}

void CSuitePage::OnViewGraph() 
{
	CGraphDlg dlgGraph(NULL, m_pSuiteDoc);
	
	dlgGraph.DoModal();
}

BOOL CSuitePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//if continually looping checked previously, when bring up dialog again
	//need to have cycle number editbox disabled.
	if (m_bLoopTests)
	{
		GetDlgItem(IDE_CycleNumber)->EnableWindow(FALSE);
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
