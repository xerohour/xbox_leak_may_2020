#if !defined(AFX_TABTOOL_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABTOOL_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabTool.h : header file
//

#include "resource.h"

class CToolPropPageManager;
class CTool;

/////////////////////////////////////////////////////////////////////////////
// CTabTool dialog

class CTabTool : public CPropertyPage
{
// Construction
public:
	CTabTool( CToolPropPageManager* pToolPropPageManager );
	virtual ~CTabTool();
	void SetTool( CTool* pTool );
	DWORD GetBitsUI();

// Dialog Data
	//{{AFX_DATA(CTabTool)
	enum { IDD = IDD_TAB_TOOL };
	CButton	m_btnToolProperties;
	CEdit	m_editName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabTool)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CTool*					m_pTool;
	CToolPropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;
	DWORD					m_dwBitsUI;

// Implementation
protected:
	void SetModifiedFlag();
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CTabTool)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusName();
	afx_msg void OnToolProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTOOL_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
