#if !defined(AFX_CONDITIONCONFIGEDITOR_H__6D512B1A_8D27_47AE_99F8_618F3CB61B71__INCLUDED_)
#define AFX_CONDITIONCONFIGEDITOR_H__6D512B1A_8D27_47AE_99F8_618F3CB61B71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// conditionconfigeditor.h : header file
//

#include "resource.h"

class CDLSComponent;
class CSystemConfiguration;

/////////////////////////////////////////////////////////////////////////////
// CConditionConfigEditor dialog

class CConditionConfigEditor : public CDialog
{
// Construction
public:
	CConditionConfigEditor(CWnd* pParent = NULL, CDLSComponent* pComponent = NULL);   // standard constructor

public:
	void SetConfig(CSystemConfiguration* pConfig);

private:
	void InitQueryList();
	void EnableControls(BOOL bEnable = TRUE);

private:
	CDLSComponent*			m_pComponent;
	CSystemConfiguration*	m_pConfig;

// Dialog Data
	//{{AFX_DATA(CConditionConfigEditor)
	enum { IDD = IDD_CONDITION_CONFIG_EDITOR };
	CButton	m_OKButton;
	CEdit	m_ValueEdit;
	CListBox	m_QueryList;
	CStatic	m_HelpStatic;
	CEdit	m_ConfigNameEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConditionConfigEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConditionConfigEditor)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusConfigNameEdit();
	virtual void OnOK();
	afx_msg void OnKillfocusValueEdit();
	afx_msg void OnSelchangeQueryList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDITIONCONFIGEDITOR_H__6D512B1A_8D27_47AE_99F8_618F3CB61B71__INCLUDED_)
