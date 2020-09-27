#if !defined(AFX_TABMIXGROUP_H__A37594BC_7317_4208_9B0E_A53431BE06A8__INCLUDED_)
#define AFX_TABMIXGROUP_H__A37594BC_7317_4208_9B0E_A53431BE06A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabMixGroup.h : header file
//

#include "resource.h"
#include "MixGroupPPGMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CTabMixGroup dialog

class CTabMixGroup : public CPropertyPage
{
	DECLARE_DYNCREATE(CTabMixGroup)

// Construction
public:
	CTabMixGroup();
	~CTabMixGroup();

// Dialog Data
	//{{AFX_DATA(CTabMixGroup)
	enum { IDD = IDD_TAB_MIXGROUP };
	CButton	m_groupSynth;
	CButton	m_buttonSynth;
	CEdit	m_editMixGroupName;
	CComboBox	m_comboSynth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabMixGroup)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Access functions
public:
	void SetItem( MixGroupInfoForPPG* pMixGroupInfoForPPG, IDMUSProdPropPageObject* pINewPropPageObject );

// Attributes
protected:
	MixGroupInfoForPPG	m_MixGroupInfoForPPG;
	IDMUSProdPropPageObject* m_pIPropPageObject;
	bool			m_fNeedToDetach;

// Implementation
protected:
	void UpdateControls();
	void EnableControls( bool fEnable );

	// Generated message map functions
	//{{AFX_MSG(CTabMixGroup)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSynth();
	afx_msg void OnKillfocusEditMixgroupName();
	afx_msg void OnSelchangeComboSynth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABMIXGROUP_H__A37594BC_7317_4208_9B0E_A53431BE06A8__INCLUDED_)
