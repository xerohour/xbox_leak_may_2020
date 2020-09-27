#if !defined(AFX_TABGRAPH_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABGRAPH_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabGraph.h : header file
//

#include "resource.h"

class CGraphPropPageManager;
class CDirectMusicGraph;

/////////////////////////////////////////////////////////////////////////////
// CTabGraph dialog

class CTabGraph : public CPropertyPage
{
// Construction
public:
	CTabGraph( CGraphPropPageManager* pGraphPropPageManager );
	virtual ~CTabGraph();
	void SetGraph( CDirectMusicGraph* pGraph );

// Dialog Data
	//{{AFX_DATA(CTabGraph)
	enum { IDD = IDD_TAB_GRAPH };
	CEdit	m_editName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabGraph)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CDirectMusicGraph*		m_pGraph;
	CGraphPropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;

// Implementation
protected:
	void SetModifiedFlag();
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CTabGraph)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABGRAPH_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
