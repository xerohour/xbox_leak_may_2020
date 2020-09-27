#if !defined(AFX_TABMOTIFMOTIF_H__819A7F03_7332_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABMOTIFMOTIF_H__819A7F03_7332_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabMotifMotif.h : header file
//

class CMotifPropPageManager;

/////////////////////////////////////////////////////////////////////////////
// CTabMotifMotif dialog

class CTabMotifMotif : public CPropertyPage
{
// Construction
public:
	CTabMotifMotif( CMotifPropPageManager* pMotifPropPageManager );
	virtual ~CTabMotifMotif();
	void RefreshTab( IDMUSProdPropPageObject* pIPropPageObject );

// Dialog Data
	//{{AFX_DATA(CTabMotifMotif)
	enum { IDD = IDD_TAB_MOTIF_MOTIF };
	CButton	m_btnResetVarOrder;
	CButton	m_btnTimeSignature;
	CEdit	m_editName;
	CEdit	m_editStartBar;
	CEdit	m_editStartBeat;
	CEdit	m_editStartGrid;
	CEdit	m_editStartTick;
	CButton	m_btnLength;
	CSpinButtonCtrl	m_spinStartBar;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartTick;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabMotifMotif)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CDirectMusicPattern*	m_pMotif;
	CMotifPropPageManager*	m_pPageManager;
	BOOL					m_fNeedToDetach;

// Implementation
protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();
	void UpdatePlayStartControls();
	void SetTimeSignatureBitmap();

	// Generated message map functions
	//{{AFX_MSG(CTabMotifMotif)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocusName();
	afx_msg void OnLength();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusEditPlayStartBar();
	afx_msg void OnKillFocusEditPlayStartBeat();
	afx_msg void OnKillFocusEditPlayStartGrid();
	afx_msg void OnKillFocusEditPlayStartTick();
	afx_msg void OnDeltaPosSpinPlayStartBar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinPlayStartBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinPlayStartGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinPlayStartTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimeSignature();
	afx_msg void OnCheckResetVarOrder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABMOTIFMOTIF_H__819A7F03_7332_11D1_89AE_00A0C9054129__INCLUDED_)
