#if !defined(AFX_PROJECTPROPTABGENERAL_H__AE0BAF02_A119_11D0_89AD_00A0C9054129__INCLUDED_)
#define AFX_PROJECTPROPTABGENERAL_H__AE0BAF02_A119_11D0_89AD_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProjectPropTabGeneral.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabGeneral dialog

class CProjectPropTabGeneral : public CPropertyPage
{
// Construction
public:
	CProjectPropTabGeneral( CProjectPropPageManager* pPageManager );
	virtual ~CProjectPropTabGeneral();
	virtual void SetProject( CProject* pProject );

// Dialog Data
	//{{AFX_DATA(CProjectPropTabGeneral)
	enum { IDD = IDD_TAB_PROJECT_GENERAL };
	CStatic	m_staticLastModified;
	CStatic	m_staticLastModifiedBy;
	CStatic	m_staticProjectFileName;
	CEdit	m_editDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProjectPropTabGeneral)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CProject*					m_pProject;
	CProjectPropPageManager*	m_pPageManager;

// Implementation
protected:
	void EnableControls( BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CProjectPropTabGeneral)
	afx_msg void OnChangeDescription();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTPROPTABGENERAL_H__AE0BAF02_A119_11D0_89AD_00A0C9054129__INCLUDED_)
