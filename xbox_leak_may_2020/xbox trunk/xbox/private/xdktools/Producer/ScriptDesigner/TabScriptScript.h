#if !defined(AFX_TABSCRIPTSCRIPT_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABSCRIPTSCRIPT_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabScriptScript.h : header file
//

#include "resource.h"

class CScriptPropPageManager;
class CDirectMusicScript;

/////////////////////////////////////////////////////////////////////////////
// CTabScriptScript dialog

class CTabScriptScript : public CPropertyPage
{
// Construction
public:
	CTabScriptScript( CScriptPropPageManager* pScriptPropPageManager );
	virtual ~CTabScriptScript();
	void SetScript( CDirectMusicScript* pScript );

// Dialog Data
	//{{AFX_DATA(CTabScriptScript)
	enum { IDD = IDD_TAB_SCRIPT_SCRIPT };
	CComboBox	m_comboLanguage;
	CButton	m_checkLoadAll;
	CButton	m_checkDownLoadAll;
	CEdit	m_editName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabScriptScript)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CDirectMusicScript*		m_pScript;
	CScriptPropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;

// Implementation
protected:
	void SetModifiedFlag();
	void EnableControls( BOOL fEnable );
	void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CTabScriptScript)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	afx_msg void OnKillFocusLanguage();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckDownLoadAll();
	afx_msg void OnDoubleClickedCheckDownLoadAll();
	afx_msg void OnCheckLoadAll();
	afx_msg void OnDoubleClickedCheckLoadAll();
	afx_msg void OnSelChangeLanguage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSCRIPTSCRIPT_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
