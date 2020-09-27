#if !defined(AFX_COMMANDPPG_H__1A2A29E6_EF4E_11D0_BAD9_00805F493F43__INCLUDED_)
#define AFX_COMMANDPPG_H__1A2A29E6_EF4E_11D0_BAD9_00805F493F43__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CommandPPG.h : header file
//

#include <afxctl.h>
#include "resource.h"
#include "stdafx.h"
#include "CommandPPGMgr.h"
#include "CommandMgr.h"
#include "FileIO.h"

class CCommandPPGMgr;

/////////////////////////////////////////////////////////////////////////////
// CCommandPPG dialog

class CCommandPPG : public CPropertyPage
{
	DECLARE_DYNCREATE(CCommandPPG)

// Construction
public:
	CCommandPPG();
	~CCommandPPG();

// Attributes
	CCommandPPGMgr*	m_pPageManager;

// Functions
	HRESULT SetData( PPGCommand* pCommand );

// Dialog Data
	//{{AFX_DATA(CCommandPPG)
	enum { IDD = IDD_PROPPAGE_COMMANDS };
	CComboBox	m_comboRepeatPattern;
	CStatic	m_staticEmbCustom;
	CSpinButtonCtrl	m_spinGrooveRange;
	CSpinButtonCtrl	m_spinEmbCustom;
	CEdit	m_editGrooveRange;
	CEdit	m_editEmbCustom;
	CButton	m_CheckGrooveLevel;
	CSpinButtonCtrl	m_spinGrooveLevel;
	CEdit	m_editGrooveLevel;
	CListBox	m_listboxEmbellishment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCommandPPG)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCommandPPG)
	afx_msg void OnSelchangeCommandEmbellishmentlist();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditGrooveLevel();
	afx_msg void OnCheckGrooveLevel();
	afx_msg void OnChangeEditGrooveRange();
	afx_msg void OnSelchangeComboRepeatPattern();
	afx_msg void OnKillfocusEditEmbCustom();
	afx_msg void OnDeltaposSpinEmbCustom(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void			EnableItems( BOOL fEnable );

	PPGCommand		m_PPGCommandData;
	CString			m_cstrOldLevel;
	BYTE			m_bEmbCustom;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMANDPPG_H__1A2A29E6_EF4E_11D0_BAD9_00805F493F43__INCLUDED_)
