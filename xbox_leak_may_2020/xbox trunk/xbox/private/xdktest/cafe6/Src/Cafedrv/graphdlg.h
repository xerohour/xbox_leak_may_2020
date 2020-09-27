///////////////////////////////////////////////////////////////////////////////
//	graphdlg.h
//
//	Created by :			Date :
//		BrianCr				06/30/95
//
//	Description :
//		Definition of the CGraphDlg class
//

#ifndef __GRAPHDLG_H__
#define __GRAPHDLG_H__

#include "caferes.h"
#include "suitedoc.h"
#include "graphwnd.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg dialog

class CGraphDlg: public CDialog
{
// Construction
public:
	CGraphDlg(CWnd* pParent = NULL, CSuiteDoc* pSuiteDoc = NULL);   // standard constructor
	~CGraphDlg();

// Dialog Data
	//{{AFX_DATA(CGraphDlg)
	enum { IDD = IDD_Graph };
/*
	CComboBox	m_cbGraphs;
	CListBox	m_lbTests;
*/	
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGraphDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeGraphList();
	afx_msg void OnHScroll(UINT, UINT, CScrollBar *);
	afx_msg void OnVScroll(UINT, UINT, CScrollBar *);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void FillGraphList(void);
	void CalcScrollMetrics(void);
	void OnScroll(UINT nScrollCode, UINT nPos);


protected:
	CSuiteDoc* m_pSuiteDoc;
	CGraphWnd* m_pwndGraph;
	CSize m_sizeScroll;
	CSize m_sizePage;
	CSize m_sizeLine;
};

#endif // __GRAPHDLG_H__
