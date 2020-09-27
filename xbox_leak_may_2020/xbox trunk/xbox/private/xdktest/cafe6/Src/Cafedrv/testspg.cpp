///////////////////////////////////////////////////////////////////////////////
//	testspg.cpp
//
//	Created by :			Date :
//		BrianCr				08/04/95
//
//	Description :
//		Implementation of the CTestsRunPage class
//

#include "stdafx.h"
#include "testspg.h"
#include "randtest.h"
#include "set.h"

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTestsRunPage dialog

IMPLEMENT_DYNCREATE(CTestsRunPage, CPropertyPage)

CTestsRunPage::CTestsRunPage(CSuiteDoc* pSuiteDoc /*= NULL*/)
	: CPropertyPage(CTestsRunPage::IDD),
	  m_pSuiteDoc(pSuiteDoc)
{
	// the doc must be valid
	ASSERT(m_pSuiteDoc);

	//{{AFX_DATA_INIT(CTestsRunPage)
	//}}AFX_DATA_INIT
}

CTestsRunPage::~CTestsRunPage()
{
}

void CTestsRunPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestsRunPage)
	//}}AFX_DATA_MAP
}

BOOL CTestsRunPage::OnSetActive(void)
{
	// call the base class
	if (!CPropertyPage::OnSetActive()) {
		return FALSE;
	}

	// fill the tests run list
	FillTestsRunList();

	// select the last test

	OnSelChangeTestsRunList();

	return TRUE;
}


BEGIN_MESSAGE_MAP(CTestsRunPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTestsRunPage)
	ON_CBN_SELCHANGE(IDL_TestsRun, OnSelChangeTestsRunList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg message handlers

void CTestsRunPage::OnOK() 
{
	CDialog::OnOK();
}

void CTestsRunPage::OnSelChangeTestsRunList()
{
	// get the tests run list box
	CListBox* plbTestsRun = (CListBox*)GetDlgItem(IDL_TestsRun);

	// get the current selection
	CTest* pTest = NULL;
	int nSel = plbTestsRun->GetCurSel();
	if (nSel != LB_ERR) {
		// get a pointer to the test
		pTest = (CTest*)plbTestsRun->GetItemDataPtr(nSel);
	}

	// update the pre- and post- conditions list boxes
	FillPreCondList(pTest);
	FillPostCondList(pTest);
}

void CTestsRunPage::FillTestsRunList(void)
{
	// get the tests run list box
	CListBox* plbTestsRun = (CListBox*)GetDlgItem(IDL_TestsRun);

	// empty the list box
	plbTestsRun->ResetContent();

	// get the list of tests from the suite
	CSubSuite::TestList* plistTests = m_pSuiteDoc->GetTestsRunList();

	// iterate through the list of tests run
	for (POSITION pos = plistTests->GetHeadPosition(); pos != NULL; ) {
		// get the test
		CTest* pTest = plistTests->GetNext(pos);
		// add each test to the list
		int nElement = plbTestsRun->AddString(pTest->GetName());
		// store a pointer to the test in the list
		plbTestsRun->SetItemDataPtr(nElement, (void*)pTest);
	}
}

void CTestsRunPage::FillPreCondList(CTest* pTest)
{
	// get the pre-cond list box
	CListBox* plbPreCond = (CListBox*)GetDlgItem(IDL_PreCond);

	// empty the list box
	plbPreCond->ResetContent();

	// leave the list box empty if no test is selected
	if (pTest) {
		// determine whether this test is randomizable or not
		if (pTest->GetType() == TEST_TYPE_RANDOM) {
			// get the set of pre-conditions
			CSet<CVertex> Set = ((CRandomTest*)pTest)->GetPreCond();

			// iterate through the set and show them in the list
			for (POSITION pos = Set.GetStartPosition(); pos != NULL; ) {
				// get the pre-condition
				CVertex precond = Set.GetNext(pos);
				// add the pre-condition to the list
				plbPreCond->AddString(precond.GetName());
			}
		}
		else {
			plbPreCond->AddString("No pre-conditions");
		}
	}
}

void CTestsRunPage::FillPostCondList(CTest* pTest)
{
	// get the post-cond list box
	CListBox* plbPostCond = (CListBox*)GetDlgItem(IDL_PostCond);

	// empty the list box
	plbPostCond->ResetContent();

	// leave the list box empty if no test is selected
	if (pTest) {
		// determine whether this test is randomizable or not
		if (pTest->GetType() == TEST_TYPE_RANDOM) {

			// get the set of post-conditions
			CSet<CVertex> Set = ((CRandomTest*)pTest)->GetPostCond();

			// iterate through the set and show them in the list
			for (POSITION pos = Set.GetStartPosition(); pos != NULL; ) {
				// get the post-condition
				CVertex postcond = Set.GetNext(pos);
				// add the post-condition to the list
				plbPostCond->AddString(postcond.GetName());
			}
		}
		else {
			plbPostCond->AddString("No post-conditions");
		}
	}
}
