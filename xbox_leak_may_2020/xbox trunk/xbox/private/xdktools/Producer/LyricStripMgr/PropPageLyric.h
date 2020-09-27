/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#if !defined(AFX_PROPPAGELYRIC_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGELYRIC_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"

class CLyricPropPageMgr;
class CTabOnReturnEdit;

// PropPageLyric.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageLyric dialog
class CLyricPropPageMgr;

class PropPageLyric : public CPropertyPage
{
	friend CLyricPropPageMgr;
	DECLARE_DYNCREATE(PropPageLyric)

// Construction
public:
	PropPageLyric();
	~PropPageLyric();

// Dialog Data
	//{{AFX_DATA(PropPageLyric)
	enum { IDD = IDD_LYRIC_PROPPAGE };
	CSpinButtonCtrl	m_spinLogicalMeasure;
	CSpinButtonCtrl	m_spinLogicalBeat;
	CEdit	m_editLogicalMeasure;
	CEdit	m_editLogicalBeat;
	CButton	m_radioTimingQuick;
	CButton	m_radioTimingBeforeTime;
	CButton	m_radioTimingAtTime;
	CSpinButtonCtrl	m_spinTick;
	CEdit	m_editTick;
	CEdit	m_editBeat;
	CEdit	m_editLyric;
	CEdit	m_editMeasure;
	CSpinButtonCtrl	m_spinMeasure;
	CSpinButtonCtrl	m_spinBeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageLyric)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPageLyric)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditBeat();
	afx_msg void OnKillfocusEditLyric();
	afx_msg void OnKillfocusEditMeasure();
	virtual void OnOK();
	afx_msg void OnKillfocusEditTick();
	afx_msg void OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimingQuick();
	afx_msg void OnDoubleClickedTimingQuick();
	afx_msg void OnTimingBeforeTime();
	afx_msg void OnDoubleClickedTimingBeforeTime();
	afx_msg void OnTimingAtTime();
	afx_msg void OnDoubleClickedTimingAtTime();
	afx_msg void OnDeltaPosSpinLogicalBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLogicalMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusEditLogicalBeat();
	afx_msg void OnKillFocusEditLogicalMeasure();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetLyric( const CLyricItem * pLyric );

protected:
	void UpdateObject( void );
	void EnableControls( BOOL fEnable );
	void HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal );
	void HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );

	BOOL					m_fNeedToDetach;
	BOOL					m_fValidLyric;
	CLyricItem				m_Lyric;
	CLyricPropPageMgr *		m_pPropPageMgr;
	IDMUSProdTimeline*		m_pTimeline; // Weak timeline reference
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGELYRIC_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
