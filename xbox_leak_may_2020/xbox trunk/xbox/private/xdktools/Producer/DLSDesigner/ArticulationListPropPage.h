#if !defined(AFX_ARTICULATIONLISTPROPPAGE_H__0DE09698_933A_4187_A3FF_156620DE483A__INCLUDED_)
#define AFX_ARTICULATIONLISTPROPPAGE_H__0DE09698_933A_4187_A3FF_156620DE483A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArticulationListPropPage.h : header file
//

#include "resource.h"

class CArticulationList;

/////////////////////////////////////////////////////////////////////////////
// CArticulationListPropPage dialog

class CArticulationListPropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CArticulationListPropPage)

// Construction
public:
	CArticulationListPropPage();
	~CArticulationListPropPage();

	void SetObject(CArticulationList* pArtList){m_pArtList = pArtList;}

// Dialog Data
	//{{AFX_DATA(CArticulationListPropPage)
	enum { IDD = IDD_ARTLIST_PROP_PAGE };
	CButton	m_DLS1Check;
	CButton	m_EditButton;
	CComboBox	m_ConditionCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CArticulationListPropPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CArticulationListPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnConditionEditButton();
	afx_msg void OnSelchangeArtlistConditionCombo();
	afx_msg void OnDropdownArtlistConditionCombo();
	afx_msg void OnDls1Check();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void EnableControls(BOOL bEnable = TRUE);
	void InitConditionsCombo();

private:
	CArticulationList* m_pArtList;
	BOOL m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARTICULATIONLISTPROPPAGE_H__0DE09698_933A_4187_A3FF_156620DE483A__INCLUDED_)
