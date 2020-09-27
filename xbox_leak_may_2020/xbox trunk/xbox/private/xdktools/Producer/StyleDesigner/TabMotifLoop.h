#if !defined(AFX_TABMOTIFLOOP_H__6B0A7543_2AF4_11D2_89B4_00C04FD912C8__INCLUDED_)
#define AFX_TABMOTIFLOOP_H__6B0A7543_2AF4_11D2_89B4_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabMotifLoop.h : header file
//

class CMotifPropPageManager;

/////////////////////////////////////////////////////////////////////////////
// CTabMotifLoop dialog

class CTabMotifLoop : public CPropertyPage
{
// Construction
public:
	CTabMotifLoop( CMotifPropPageManager* pMotifPropPageManager );
	~CTabMotifLoop();
	void RefreshTab( IDMUSProdPropPageObject* pIPropPageObject );

// Dialog Data
	//{{AFX_DATA(CTabMotifLoop)
	enum { IDD = IDD_TAB_MOTIF_LOOP };
	CButton	m_checkInfinite;
	CButton	m_btnResetLoop;
	CEdit	m_editRepeats;
	CEdit	m_editStartBar;
	CEdit	m_editStartBeat;
	CEdit	m_editStartGrid;
	CEdit	m_editStartTick;
	CEdit	m_editEndBar;
	CEdit	m_editEndBeat;
	CEdit	m_editEndGrid;
	CEdit	m_editEndTick;
	CSpinButtonCtrl	m_spinRepeats;
	CSpinButtonCtrl	m_spinStartBar;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinEndBar;
	CSpinButtonCtrl	m_spinEndBeat;
	CSpinButtonCtrl	m_spinEndGrid;
	CSpinButtonCtrl	m_spinEndTick;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabMotifLoop)
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
	DWORD					m_dwLastLoopRepeatCount;

// Implementation
protected:
	void EnableControls( BOOL fEnable );
	void UpdateControls();
	void UpdateLoopStartEndControls();

	// Generated message map functions
	//{{AFX_MSG(CTabMotifLoop)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusEditRepeats();
	afx_msg void OnDeltaPosSpinRepeats(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusEditLoopStartBar();
	afx_msg void OnKillFocusEditLoopStartBeat();
	afx_msg void OnKillFocusEditLoopStartGrid();
	afx_msg void OnKillFocusEditLoopStartTick();
	afx_msg void OnDeltaPosSpinLoopStartBar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLoopStartBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLoopStartGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLoopStartTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusEditLoopEndBar();
	afx_msg void OnKillFocusEditLoopEndBeat();
	afx_msg void OnKillFocusEditLoopEndGrid();
	afx_msg void OnKillFocusEditLoopEndTick();
	afx_msg void OnDeltaPosSpinLoopEndBar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLoopEndBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLoopEndGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinLoopEndTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnResetLoop();
	afx_msg void OnDoubleClickedResetLoop();
	afx_msg void OnCheckInfinite();
	afx_msg void OnDoubleClickedInfinite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABMOTIFLOOP_H__6B0A7543_2AF4_11D2_89B4_00C04FD912C8__INCLUDED_)
