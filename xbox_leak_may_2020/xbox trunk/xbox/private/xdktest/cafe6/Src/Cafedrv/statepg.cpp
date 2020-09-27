///////////////////////////////////////////////////////////////////////////////
//	statepg.cpp
//
//	Created by :			Date :
//		BrianCr				08/07/95
//
//	Description :
//		Implementation of the CStatePage class
//

#include "stdafx.h"
#include "statepg.h"

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CStatePage dialog

IMPLEMENT_DYNCREATE(CStatePage, CPropertyPage)

CStatePage::CStatePage(CSuiteDoc* pSuiteDoc /*= NULL*/)
	: CPropertyPage(CStatePage::IDD),
	  m_pSuiteDoc(pSuiteDoc)
{
	// the doc must be valid
	ASSERT(m_pSuiteDoc);

	//{{AFX_DATA_INIT(CStatePage)
	//}}AFX_DATA_INIT
}

CStatePage::~CStatePage()
{
}

void CStatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatePage)
	//}}AFX_DATA_MAP
}

BOOL CStatePage::OnSetActive(void)
{
	// call the base class
	if (!CPropertyPage::OnSetActive()) {
		return FALSE;
	}

	// fill the state list
	FillStateList();

	return TRUE;
}


BEGIN_MESSAGE_MAP(CStatePage, CPropertyPage)
	//{{AFX_MSG_MAP(CStatePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg message handlers

void CStatePage::OnOK() 
{
	CDialog::OnOK();
}

void CStatePage::FillStateList(void)
{
	// get the tests run list box
	CListBox* plbState = (CListBox*)GetDlgItem(IDL_CurrentState);

	// empty the list box
	plbState->ResetContent();

	// the state is only valid if we're randomizing
	if (m_pSuiteDoc->GetRandomize()) {
		// get the state
		CState* pState = m_pSuiteDoc->GetCurrentState();

		// get the name of the state
		CString strState = pState->GetName();
		strState.TrimLeft();
		strState.TrimRight();

		// add the name to the list box (each state is separated by a comma)
		while (!strState.IsEmpty()) {
			strState.TrimLeft();
			int nEndPos = strState.Find(_T(','));
			if (nEndPos == -1) {
				nEndPos = strState.GetLength();
			}
			plbState->AddString(strState.Left(nEndPos));
			strState = strState.Mid(nEndPos+1);
		}
	}
	else {
		plbState->AddString("No current state");
	}
}
