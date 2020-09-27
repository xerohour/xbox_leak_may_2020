///////////////////////////////////////////////////////////////////////////////
//	testspg.h
//
//	Created by :			Date :
//		BrianCr				08/04/95
//
//	Description :
//		Definition of the CTestsRunDlg class
//

#ifndef __TESTSPG_H__
#define __TESTSPG_H__

#include "caferes.h"
#include "suitedoc.h"

/////////////////////////////////////////////////////////////////////////////
// CTestsRunDlg dialog

class CTestsRunPage: public CPropertyPage
{
	DECLARE_DYNCREATE(CTestsRunPage)
// Construction
public:
	CTestsRunPage(CSuiteDoc* pSuiteDoc = NULL);
	~CTestsRunPage();

// Dialog Data
	//{{AFX_DATA(CTestsRunPage)
	enum { IDD = IDD_TestsRun };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestsRunPage)
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnSetActive(void);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTestsRunPage)
	afx_msg void OnSelChangeTestsRunList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void FillTestsRunList(void);
	void FillPreCondList(CTest* pTest);
	void FillPostCondList(CTest* pTest);

// data
protected:
	CSuiteDoc* m_pSuiteDoc;
};

#endif // __TESTSPG_H__
