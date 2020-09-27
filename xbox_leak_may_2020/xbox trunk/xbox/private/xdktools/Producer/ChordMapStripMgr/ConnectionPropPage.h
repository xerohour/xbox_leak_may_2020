#if !defined(AFX_CONNECTIONPROPPAGE_H__B9C5C49F_8240_11D1_984C_00805FA67D16__INCLUDED_)
#define AFX_CONNECTIONPROPPAGE_H__B9C5C49F_8240_11D1_984C_00805FA67D16__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConnectionPropPage.h : header file
//

#include "resource.h"
#include "connectionproppagemgr.h"


/////////////////////////////////////////////////////////////////////////////
// CConnectionPropPage dialog

class CConnectionPropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CConnectionPropPage)

// Construction
public:
	void SetConnection(short val);
	void SetMinBeats(short val);
	void SetMaxBeats(short val);
	void SetMinMaxEdit(BOOL fEnable);
	CConnectionPropPage();
	~CConnectionPropPage();
	CConnectionPropPageMgr* m_pPropPageMgr;

	short m_nWeight;
	short m_nMinBeats;
	short m_nMaxBeats;
	BOOL  m_fVariableNotFixed;

// Dialog Data
	//{{AFX_DATA(CConnectionPropPage)
	enum { IDD = IDD_CONNECTION_PROPAGE };
	CSpinButtonCtrl	m_spinMinBeats;
	CSpinButtonCtrl	m_spinMaxBeats;
	CEdit	m_editMinBeats;
	CEdit	m_editMaxBeats;
	CSpinButtonCtrl	m_spinWeight;
	CEdit	m_editWeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectionPropPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL	m_fNeedToDetach;

	// Generated message map functions
	//{{AFX_MSG(CConnectionPropPage)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateEditweight();
	afx_msg void OnKillFocusEditMaxBeats();
	afx_msg void OnKillFocusEditMinBeats();
	afx_msg void OnDeltaPosSpinMaxBeats(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinMinBeats(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIONPROPPAGE_H__B9C5C49F_8240_11D1_984C_00805FA67D16__INCLUDED_)
