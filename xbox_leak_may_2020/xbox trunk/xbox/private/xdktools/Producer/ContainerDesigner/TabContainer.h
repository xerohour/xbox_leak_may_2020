#if !defined(AFX_TABCONTAINER_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABCONTAINER_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabContainer.h : header file
//

#include "resource.h"

class CContainerPropPageManager;
class CDirectMusicContainer;

/////////////////////////////////////////////////////////////////////////////
// CTabContainer dialog

class CTabContainer : public CPropertyPage
{
// Construction
public:
	CTabContainer( CContainerPropPageManager* pContainerPropPageManager );
	virtual ~CTabContainer();
	void SetContainer( CDirectMusicContainer* pContainer );

// Dialog Data
	//{{AFX_DATA(CTabContainer)
	enum { IDD = IDD_TAB_CONTAINER };
	CButton	m_btnContents;
	CEdit	m_editName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabContainer)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CDirectMusicContainer*		m_pContainer;
	CContainerPropPageManager*	m_pPageManager;
	BOOL						m_fNeedToDetach;

// Implementation
protected:
	void SetModifiedFlag();
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CTabContainer)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonContents();
	afx_msg void OnDoubleClickedButtonContents();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABCONTAINER_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
