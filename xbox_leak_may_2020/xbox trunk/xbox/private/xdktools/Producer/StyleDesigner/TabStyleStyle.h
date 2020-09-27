#if !defined(AFX_TABSTYLESTYLE_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABSTYLESTYLE_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabStyleStyle.h : header file
//

#include "resource.h"

class CStylePropPageManager;
class CDirectMusicStyle;

/////////////////////////////////////////////////////////////////////////////
// CTabStyleStyle dialog

class CTabStyleStyle : public CPropertyPage
{
// Construction
public:
	CTabStyleStyle( CStylePropPageManager* pStylePropPageManager );
	virtual ~CTabStyleStyle();
	void SetStyle( CDirectMusicStyle* pStyle );

// Dialog Data
	//{{AFX_DATA(CTabStyleStyle)
	enum { IDD = IDD_TAB_STYLE_STYLE };
	CButton	m_btnTimeSignature;
	CSpinButtonCtrl	m_spinTempo;
	CEdit	m_editTempo;
	CComboBox	m_cmbxCategory;
	CEdit	m_editName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabStyleStyle)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CDirectMusicStyle*		m_pStyle;
	CStylePropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;

// Implementation
protected:
	void SetTempoControlText();
	void SetModifiedFlag();
	void EnableControls( BOOL fEnable );
	void UpdateControls();
	void SetTimeSignatureBitmap();

	// Generated message map functions
	//{{AFX_MSG(CTabStyleStyle)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	afx_msg void OnKillFocusTempo();
	afx_msg void OnDeltaPosTempoSpin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusCategory();
	afx_msg void OnTimeSignature();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSTYLESTYLE_H__7060E0E4_B63A_11D0_89AE_00A0C9054129__INCLUDED_)
