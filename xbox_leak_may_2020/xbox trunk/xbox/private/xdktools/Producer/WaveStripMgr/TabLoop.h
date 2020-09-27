#if !defined(AFX_TABLOOP_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_TABLOOP_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

// TabLoop.h : header file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>

class CPropPageMgrItem;
class CPropTrackItem;
class CTabOnReturnEdit;


/////////////////////////////////////////////////////////////////////////////
// CTabLoop dialog

class CTabLoop : public CPropertyPage
{
	friend CPropPageMgrItem;
	DECLARE_DYNCREATE(CTabLoop)

// Construction
public:
	CTabLoop();
	~CTabLoop();

// Dialog Data
	//{{AFX_DATA(CTabLoop)
	enum { IDD = IDD_PROPPAGE_WAVE_LOOP };
	CButton	m_checkLoop;
	CButton	m_btnLockLength;
	CStatic	m_staticPromptGrid;
	CStatic	m_staticPromptTick;
	CStatic	m_staticPromptBeat;
	CStatic	m_staticPromptBar;
	CEdit	m_editStartTick;
	CEdit	m_editStartGrid;
	CEdit	m_editStartBeat;
	CEdit	m_editStartMeasure;
	CEdit	m_editStartSample;
	CEdit	m_editEndTick;
	CEdit	m_editEndGrid;
	CEdit	m_editEndBeat;
	CEdit	m_editEndMeasure;
	CEdit	m_editEndSample;
	CEdit	m_editLengthTick;
	CEdit	m_editLengthGrid;
	CEdit	m_editLengthBeat;
	CEdit	m_editLengthMeasure;
	CEdit	m_editLengthNbrSamples;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartMeasure;
	CSpinButtonCtrl	m_spinStartSample;
	CSpinButtonCtrl	m_spinEndTick;
	CSpinButtonCtrl	m_spinEndGrid;
	CSpinButtonCtrl	m_spinEndBeat;
	CSpinButtonCtrl	m_spinEndMeasure;
	CSpinButtonCtrl	m_spinEndSample;
	CSpinButtonCtrl	m_spinLengthTick;
	CSpinButtonCtrl	m_spinLengthGrid;
	CSpinButtonCtrl	m_spinLengthBeat;
	CSpinButtonCtrl	m_spinLengthMeasure;
	CSpinButtonCtrl	m_spinLengthNbrSamples;
	CStatic	m_staticSourceStartTick;
	CStatic	m_staticSourceStartGrid;
	CStatic	m_staticSourceStartBeat;
	CStatic	m_staticSourceStartMeasure;
	CStatic	m_staticSourceStartSample;
	CStatic	m_staticSourceEndTick;
	CStatic	m_staticSourceEndGrid;
	CStatic	m_staticSourceEndBeat;
	CStatic	m_staticSourceEndMeasure;
	CStatic	m_staticSourceEndSample;
	CStatic	m_staticSourceLengthTick;
	CStatic	m_staticSourceLengthGrid;
	CStatic	m_staticSourceLengthBeat;
	CStatic	m_staticSourceLengthMeasure;
	CStatic	m_staticSourceLengthNbrSamples;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabLoop)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabLoop)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSpinStartTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartSample(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinEndSample(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLengthNbrSamples(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusStartTick();
	afx_msg void OnKillFocusStartGrid();
	afx_msg void OnKillFocusStartBeat();
	afx_msg void OnKillFocusStartMeasure();
	afx_msg void OnKillFocusStartSample();
	afx_msg void OnKillFocusEndTick();
	afx_msg void OnKillFocusEndGrid();
	afx_msg void OnKillFocusEndBeat();
	afx_msg void OnKillFocusEndMeasure();
	afx_msg void OnKillFocusEndSample();
	afx_msg void OnKillFocusLengthTick();
	afx_msg void OnKillFocusLengthGrid();
	afx_msg void OnKillFocusLengthBeat();
	afx_msg void OnKillFocusLengthMeasure();
	afx_msg void OnKillFocusLengthNbrSamples();
	virtual void OnOK();
	afx_msg void OnCheckLoop();
	afx_msg void OnDoubleClickedCheckLoop();
	afx_msg void OnDoubleClickedLockLength();
	afx_msg void OnLockLength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetPropTrackItem( const CPropTrackItem* pPropItem );

protected:
	void UpdateObject();
	void EnableControls( BOOL fEnable );
	bool HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );
	bool HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, DWORD& dwUpdateVal );
	bool HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset );
	bool HandleKillFocus( CEdit& edit, DWORD& dwUpdateVal, int nOffset );
	void SetEditControl( CEdit& edit, long lValue, int nOffset );
	void SetStaticControl( CStatic& stat, long lValue, int nOffset );

	void SetLoopStart( DWORD dwNewLoopStart );
	void SetLoopEnd( DWORD dwNewLoopEnd );
	void SetLoopLength( DWORD dwNewLoopLength );
	void EnforceLoopBoundaries( DWORD& dwNewLoopStart, DWORD& dwNewLoopEnd );

	DWORD StartTimeToSample();
	DWORD EndTimeToSample();
	DWORD LengthTimeToNbrSamples();
	
	void RecomputeLoopTimes();
	void SampleToRefTime();
	void SampleToMusicTime();
	
	void SetControlRanges();
	void SetMusicTimeRanges( DWORD dwGroupBits );
	void SetRefTimeRanges();

	IDMUSProdTimeline*			m_pTimeline;		// Weak timeline reference
	CPropPageMgrItem*			m_pPropPageMgr;
	IDMUSProdWaveTimelineDraw*	m_pIWaveTimelineDraw;

	CPropTrackItem				m_PropItem;

	BOOL		m_fNeedToDetach;
	BOOL		m_fValidTrackItem;

	long		m_lStartMeasure;
	long		m_lStartBeat;
	long		m_lStartGrid;
	long		m_lStartTick;

	long		m_lEndMeasure;
	long		m_lEndBeat;
	long		m_lEndGrid;
	long		m_lEndTick;

	long		m_lLengthMeasure;
	long		m_lLengthBeat;
	long		m_lLengthGrid;
	long		m_lLengthTick;
	DWORD		m_dwLengthNbrSamples;

	long		m_lSourceStartMeasure;
	long		m_lSourceStartBeat;
	long		m_lSourceStartGrid;
	long		m_lSourceStartTick;

	long		m_lSourceEndMeasure;
	long		m_lSourceEndBeat;
	long		m_lSourceEndGrid;
	long		m_lSourceEndTick;

	long		m_lSourceLengthMeasure;
	long		m_lSourceLengthBeat;
	long		m_lSourceLengthGrid;
	long		m_lSourceLengthTick;
	DWORD		m_dwSourceLengthNbrSamples;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLOOP_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
