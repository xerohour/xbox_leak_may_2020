#if !defined(AFX_TABEFFECTINFO_H__3E290DA9_992B_4E96_9FA6_109C750AEA33__INCLUDED_)
#define AFX_TABEFFECTINFO_H__3E290DA9_992B_4E96_9FA6_109C750AEA33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabEffectInfo.h : header file
//

#include "resource.h"
#include "EffectInfo.h"


/////////////////////////////////////////////////////////////////////////////
// CTabEffectInfo dialog

class CTabEffectInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CTabEffectInfo)

// Construction
public:
	CTabEffectInfo();
	~CTabEffectInfo();

// Dialog Data
	//{{AFX_DATA(CTabEffectInfo)
	enum { IDD = IDD_TAB_EFFECT };
	CComboBox	m_comboSend;
	CEdit	m_editType;
	CEdit	m_editCustomName;
	CButton	m_checkSoftware;
	CButton	m_btnEditProperties;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabEffectInfo)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Access functions
public:
	void SetEffect( EffectInfoForPPG* pEffectInfo, IDMUSProdPropPageObject* pIPropPageObject );

// Attributes
protected:
	EffectInfoForPPG	m_EffectInfoForPPG;
	IDMUSProdPropPageObject* m_pIPropPageObject;
	bool				m_fNeedToDetach;
	bool				m_fHaveValidData;

// Implementation
protected:
	void UpdateControls();
	void EnableControls( bool fEnable );
	void EnableDlgItem( int nItem, BOOL fEnable );
	void ShowDlgItem( int nItem, int nCmdShow );

	// Generated message map functions
	//{{AFX_MSG(CTabEffectInfo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckSoftware();
	afx_msg void OnButtonEditProperties();
	afx_msg void OnKillfocusEditCustomname();
	afx_msg void OnSelchangeComboSend();
	afx_msg void OnButtonSendGuid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABEFFECTINFO_H__3E290DA9_992B_4E96_9FA6_109C750AEA33__INCLUDED_)
