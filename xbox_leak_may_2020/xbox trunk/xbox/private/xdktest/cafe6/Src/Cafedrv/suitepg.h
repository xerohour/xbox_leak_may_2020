/////////////////////////////////////////////////////////////////////////////
// suitepg.h
//
// email	date		change
// briancr	06/15/95	created
//
// copyright 1994 Microsoft

// Interface of the CSuitePage class

#ifndef __SUITEPG_H__
#define __SUITEPG_H__

#include "caferes.h"
#include "suitedoc.h"

/////////////////////////////////////////////////////////////////////////////
// CSuitePage dialog

class CSuitePage: public CPropertyPage
{
	DECLARE_DYNCREATE(CSuitePage)

// Construction
public:
	CSuitePage(CSuiteDoc* pSuiteDoc = NULL);
	~CSuitePage();

// Dialog Data
	//{{AFX_DATA(CSuitePage)
	enum { IDD = IDD_SuiteTab };
	CButton	m_checkLoopTests;
	int		m_bLoopTests;
	int		m_bCleanUpBefore;
	int		m_bCleanUpAfter;
	int		m_bRandomize;
	int		m_nRandomSeed;
	int		m_nLimit;
	int		m_nLimitNum;
	int		m_nCycleNumber;
	//}}AFX_DATA
	CTimeSpan m_LimitTime;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSuitePage)
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnSetActive(void);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSuitePage)
	afx_msg void UpdateUI(void);
	afx_msg void OnContinuallyLoop(void);
	afx_msg void OnViewGraph();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void AFXAPI DDX_TimeSpan(CDataExchange* pDX, int nIDC, CTimeSpan& timespan);
	void GetTimeSpan(HWND hWnd, CTimeSpan& timespan);
	void SetTimeSpan(HWND hWnd, CTimeSpan& timespan);
	
protected:

protected:
	CSuiteDoc* m_pSuiteDoc;
};

#endif // __SUITEPG_H__
