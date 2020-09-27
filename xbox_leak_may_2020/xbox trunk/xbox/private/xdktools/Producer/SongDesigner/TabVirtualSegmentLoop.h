#if !defined(AFX_TABVIRTUALSEGMENTLOOP_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABVIRTUALSEGMENTLOOP_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TabVirtualSegmentLoop.h : header file
//

typedef struct _PPGTabVirtualSegmentLoop
{
	_PPGTabVirtualSegmentLoop( void )
	{
		dwPageIndex = 3;
		dwRepeats = 0;
		lStartMeasure = 0;
		lStartBeat = 0;
		lStartGrid = 0;
		lStartTick = 0;
		lEndMeasure = 0;
		lEndBeat = 0;
		lEndGrid = 0;
		lEndTick = 0;
		dwFlagsUI = 0;
	}

	DWORD				dwPageIndex;
    DWORD				dwRepeats;
	long				lStartMeasure;
	long				lStartBeat;
	long				lStartGrid;
	long				lStartTick;
	long				lEndMeasure;
	long				lEndBeat;
	long				lEndGrid;
	long				lEndTick;
	DWORD				dwFlagsUI;
} PPGTabVirtualSegmentLoop;


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop dialog

class CTabVirtualSegmentLoop : public CPropertyPage
{
// Construction
public:
	CTabVirtualSegmentLoop( IDMUSProdPropPageManager* pIPageManager );
	~CTabVirtualSegmentLoop();

// Dialog Data
	//{{AFX_DATA(CTabVirtualSegmentLoop)
	enum { IDD = IDD_TAB_VIRTUAL_SEGMENT_LOOP };
	CEdit	m_editLoopRepeat;
	CSpinButtonCtrl	m_spinLoopRepeat;
	CButton	m_btnLoopInfinite;
	CEdit	m_editStartMeasure;
	CEdit	m_editStartBeat;
	CEdit	m_editStartGrid;
	CEdit	m_editStartTick;
	CEdit	m_editEndMeasure;
	CEdit	m_editEndBeat;
	CEdit	m_editEndGrid;
	CEdit	m_editEndTick;
	CSpinButtonCtrl	m_spinStartMeasure;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinEndMeasure;
	CSpinButtonCtrl	m_spinEndBeat;
	CSpinButtonCtrl	m_spinEndGrid;
	CSpinButtonCtrl	m_spinEndTick;
	CButton	m_btnResetLoop;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabVirtualSegmentLoop)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTabVirtualSegmentLoop)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSpinStartMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusStartMeasure();
	afx_msg void OnKillFocusStartBeat();
	afx_msg void OnKillFocusStartGrid();
	afx_msg void OnKillFocusStartTick();
	afx_msg void OnSpinEndMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusEndMeasure();
	afx_msg void OnKillFocusEndBeat();
	afx_msg void OnKillFocusEndGrid();
	afx_msg void OnKillFocusEndTick();
	afx_msg void OnSpinRepeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusRepeat();
	afx_msg void OnCheckRepeat();
	afx_msg void OnDoubleClickedCheckRepeat();
	afx_msg void OnResetLoop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdateObject();
	void SetEditControl( CEdit& edit, long lValue, int nOffset );
	bool HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset );
	bool HandleKillFocus( CSpinButtonCtrl& spin, DWORD& dwUpdateVal, int nOffset );
	bool HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );
	bool HandleDeltaChange( CSpinButtonCtrl& spin, NMHDR* pNMHDR, LRESULT* pResult, DWORD& dwUpdateVal );

public:
	void SetObject( IDMUSProdPropPageObject* pIPropPageObject );
	void RefreshTab( void );

protected:
	IDMUSProdPropPageManager*		m_pIPageManager;
	IDMUSProdPropPageObject*		m_pIPropPageObject;
	PPGTabVirtualSegmentLoop		m_PPGTabVirtualSegmentLoop;
	BOOL							m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABVIRTUALSEGMENTLOOP_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
