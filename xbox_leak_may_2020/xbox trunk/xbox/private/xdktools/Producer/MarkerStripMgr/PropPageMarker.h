#if !defined(AFX_PROPPAGEMARKER_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_PROPPAGEMARKER_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"

class CMarkerPropPageMgr;
class CTabOnReturnEdit;

// PropPageMarker.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPageMarker dialog
class CMarkerPropPageMgr;

class PropPageMarker : public CPropertyPage
{
	friend CMarkerPropPageMgr;
	DECLARE_DYNCREATE(PropPageMarker)

// Construction
public:
	PropPageMarker();
	~PropPageMarker();

// Dialog Data
	//{{AFX_DATA(PropPageMarker)
	enum { IDD = IDD_MARKER_PROPPAGE };
	CEdit	m_editGrid;
	CSpinButtonCtrl	m_spinGrid;
	CSpinButtonCtrl	m_spinTick;
	CEdit	m_editTick;
	CEdit	m_editBeat;
	CEdit	m_editMarker;
	CEdit	m_editMeasure;
	CSpinButtonCtrl	m_spinMeasure;
	CSpinButtonCtrl	m_spinBeat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPageMarker)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropPageMarker)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditBeat();
	afx_msg void OnKillfocusEditMeasure();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditTick();
	afx_msg void OnKillfocusEditGrid();
	afx_msg void OnDeltaposSpinGrid(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetMarker( const CMarkerItem * pMarker );

protected:
	void UpdateObject( void );
	void EnableControls( BOOL fEnable );
	void HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal );
	void HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );

	BOOL					m_fNeedToDetach;
	BOOL					m_fValidMarker;
	CMarkerItem				m_Marker;
	CMarkerPropPageMgr*		m_pPropPageMgr;
	IDMUSProdTimeline*		m_pTimeline; // Weak timeline reference
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEMARKER_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
