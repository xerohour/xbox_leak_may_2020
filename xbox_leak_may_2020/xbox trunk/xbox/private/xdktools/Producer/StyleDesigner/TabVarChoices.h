#if !defined(AFX_TABVARCHOICES_H__F3571522_8865_11D1_89AF_00A0C9054129__INCLUDED_)
#define AFX_TABVARCHOICES_H__F3571522_8865_11D1_89AF_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabVarChoices.h : header file
//

class CVarChoicesPropPageManager;

/////////////////////////////////////////////////////////////////////////////
// CTabVarChoices dialog

class CTabVarChoices : public CPropertyPage
{
// Construction
public:
	CTabVarChoices( CVarChoicesPropPageManager* pVarChoicesPropPageManager );
	virtual ~CTabVarChoices();
	void SetVarChoices( CVarChoices* pVarChoices );

// Dialog Data
	//{{AFX_DATA(CTabVarChoices)
	enum { IDD = IDD_TAB_VARCHOICES };
	CSpinButtonCtrl	m_spinVariation;
	CEdit	m_editVariation;
	CButton	m_btnOther;
	CButton	m_btnMinor;
	CButton	m_btnMajor;
	CComboBox	m_comboFunction;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabVarChoices)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CVarChoices*				m_pVarChoices;
	CVarChoicesPropPageManager*	m_pPageManager;
	BOOL						m_fNeedToDetach;

// Implementation
protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CTabVarChoices)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABVARCHOICES_H__F3571522_8865_11D1_89AF_00A0C9054129__INCLUDED_)
