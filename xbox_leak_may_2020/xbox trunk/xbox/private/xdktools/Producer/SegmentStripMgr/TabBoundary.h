#if !defined(AFX_TABBOUNDARY_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_TABBOUNDARY_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

// TabBoundary.h : header file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>

class CPropPageMgrItem;
class CTabOnReturnEdit;


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary dialog

class CTabBoundary : public CPropertyPage
{
	friend CPropPageMgrItem;

// Construction
public:
	CTabBoundary();
	~CTabBoundary();

// Dialog Data
	//{{AFX_DATA(CTabBoundary)
	enum { IDD = IDD_PROPPAGE_BOUNDARY };
	CButton	m_radioInvalidate;
	CButton	m_radioInvalidatePri;
	CButton	m_radioNoInvalidate;
	CStatic	m_staticPrompt;
	CStatic	m_staticNoMarkers;
	CStatic	m_staticAlignPrompt;
	CButton	m_checkMarker;
	CComboBox	m_comboBoundary;
	CButton	m_radioSwitchOnBoundary;
	CButton	m_radioSwitchAnyTime;
	CButton	m_radioSwitchAnyGrid;
	CButton	m_radioSwitchAnyBeat;
	CButton	m_radioSwitchAnyBar;
	CButton	m_radioTimingQuick;
	CButton	m_radioTimingAfterPrepareTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabBoundary)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabBoundary)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckMarker();
	afx_msg void OnDoubleClickedCheckMarker();
	afx_msg void OnSelChangeComboBoundary();
	afx_msg void OnRadioSwitchNextBoundary();
	afx_msg void OnDoubleClickedRadioSwitchNextBoundary();
	afx_msg void OnRadioSwitchAnyTime();
	afx_msg void OnDoubleClickedRadioSwitchAnyTime();
	afx_msg void OnRadioSwitchAnyGrid();
	afx_msg void OnDoubleClickedRadioSwitchAnyGrid();
	afx_msg void OnRadioSwitchAnyBeat();
	afx_msg void OnDoubleClickedRadioSwitchAnyBeat();
	afx_msg void OnRadioTimingAfterPrepareTime();
	afx_msg void OnDoubleClickedRadioTimingAfterPrepareTime();
	afx_msg void OnRadioTimingQuick();
	afx_msg void OnDoubleClickedRadioTimingQuick();
	afx_msg void OnRadioSwitchAnyBar();
	afx_msg void OnDoubleclickedRadioSwitchAnyBar();
	afx_msg void OnRadioInvalidate();
	afx_msg void OnRadioInvalidatepri();
	afx_msg void OnRadioNoinvalidate();
	afx_msg void OnDoubleclickedRadioInvalidate();
	afx_msg void OnDoubleclickedRadioInvalidatepri();
	afx_msg void OnDoubleclickedRadioNoinvalidate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetTrackItem( const CTrackItem * pItem );

protected:
	void UpdateObject();
	void EnableControls( BOOL fEnable );
	void SetPlayFlags();

	IDMUSProdTimeline*		m_pTimeline;		// Weak timeline reference
	CPropPageMgrItem*		m_pPropPageMgr;

	CTrackItem				m_TrackItem;

	BOOL					m_fNeedToDetach;
	BOOL					m_fValidTrackItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBOUNDARY_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
