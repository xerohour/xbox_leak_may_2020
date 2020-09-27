#ifndef __LOOPPPG_H__
#define __LOOPPPG_H__ 1

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )

class CSegmentPPGMgr;
struct PPGSegment;

// LoopPPG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoopPPG dialog

class CLoopPPG : public CPropertyPage
{
friend class CSegmentPPGMgr;

	DECLARE_DYNCREATE(CLoopPPG)

// Construction
public:
	CLoopPPG();
	~CLoopPPG();

// Dialog Data
	//{{AFX_DATA(CLoopPPG)
	enum { IDD = IDD_PROPPAGE_LOOP };
	CButton	m_btnResetLoop;
	CEdit	m_editStartTick;
	CEdit	m_editStartGrid;
	CEdit	m_editStartBeat;
	CEdit	m_editStartBar;
	CEdit	m_editLoopRepeat;
	CEdit	m_editEndTick;
	CEdit	m_editEndGrid;
	CEdit	m_editEndBeat;
	CEdit	m_editEndBar;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartBar;
	CSpinButtonCtrl	m_spinRepeat;
	CSpinButtonCtrl	m_spinEndTick;
	CSpinButtonCtrl	m_spinEndGrid;
	CSpinButtonCtrl	m_spinEndBeat;
	CSpinButtonCtrl	m_spinEndBar;
	UINT	m_dwClockLoopStart;
	UINT	m_dwClockLoopEnd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLoopPPG)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	PPGSegment		*m_pPPGSegment;
	CSegmentPPGMgr	*m_pPageManager;
	WORD			m_wMaxStartBeat;
	WORD			m_wMaxStartGrid;
	DWORD			m_dwMaxStartTick;
	WORD			m_wMaxEndBeat;
	WORD			m_wMaxEndGrid;
	DWORD			m_dwMaxEndTick;
	BOOL			m_fNeedToDetach;
	DWORD			m_dwLastLoopRepeatCount;

// Implementation
public:
	HRESULT	SetData(const struct PPGSegment *pSegment);

// Helper functions
private:
	void EnableItem(int nItem, BOOL fEnable);
	void ResetStartRanges( void );
	void ResetEndRanges( void );
	BOOL GetTimeSig( DMUS_TIMESIGNATURE &TimeSig, DWORD dwMeasure );
	void UpdateSegment( void );
	BOOL ValidateEndTime();
	BOOL ValidateStartTime();
	void SetEndLoopToNextAvailableTick( void );
	void SetStartLoopToNextAvailableTick( void );

protected:
	void	EnableControls(BOOL fEnable);
	void	ResetLoopControls();

	// Generated message map functions
	//{{AFX_MSG(CLoopPPG)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillfocusEditLoopEndBar();
	afx_msg void OnKillfocusEditLoopEndBeat();
	afx_msg void OnKillfocusEditLoopEndGrid();
	afx_msg void OnKillfocusEditLoopEndTick();
	afx_msg void OnKillfocusEditLoopRepeat();
	afx_msg void OnKillfocusEditLoopStartBar();
	afx_msg void OnKillfocusEditLoopStartBeat();
	afx_msg void OnKillfocusEditLoopStartGrid();
	afx_msg void OnKillfocusEditLoopStartTick();
	afx_msg void OnDeltaposSpinLoopEndBar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLoopEndBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLoopEndGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLoopEndTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLoopRepeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLoopStartBar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLoopStartBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLoopStartGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLoopStartTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnResetLoop();
	afx_msg void OnDoubleClickedResetLoop();
	afx_msg void OnChangeEditLoopRepeat();
	afx_msg void OnCheckRepeatForever();
	afx_msg void OnCheckLoopClocktime();
	afx_msg void OnKillfocusEditLoopClockend();
	afx_msg void OnKillfocusEditLoopClockstart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //__LOOPPPG_H__
