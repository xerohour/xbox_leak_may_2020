#if !defined(AFX_SEGMENTPPG_H__6CEFCD03_E1CB_11D0_BACD_00805F493F43__INCLUDED_)
#define AFX_SEGMENTPPG_H__6CEFCD03_E1CB_11D0_BACD_00805F493F43__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )

class CSegmentPPGMgr;

// SegmentPPG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPG dialog

class CSegmentPPG : public CPropertyPage
{
friend class CSegmentPPGMgr;

	DECLARE_DYNCREATE(CSegmentPPG)

// Construction
public:
	CSegmentPPG();
	~CSegmentPPG();

// Dialog Data
	//{{AFX_DATA(CSegmentPPG)
	enum { IDD = IDD_PROPPAGE_SEGMENT };
	CButton	m_buttonExtendTimeSig;
	CButton	m_buttonAudioPath;
	CEdit	m_editName;
	CEdit	m_editTrackGroup;
	CEdit	m_editStartTick;
	CEdit	m_editStartGrid;
	CEdit	m_editStartBeat;
	CEdit	m_editStartBar;
	CSpinButtonCtrl	m_spinTrackGroup;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartBar;
	CButton	m_buttonLength;
	short	m_nNumMeasures;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSegmentPPG)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CSegmentPPGMgr	*m_pPageManager;
	struct PPGSegment	*m_pPPGSegment;
	WORD			m_wMaxBeat;
	WORD			m_wMaxGrid;
	DWORD			m_dwMaxTick;
	BOOL			m_fNeedToDetach;

// Implementation
public:
	HRESULT	SetData(const struct PPGSegment *pSegment);
	//void	SetWarnUser(BOOL bWarnUser);

// Helper functions
private:
	void	UpdateSegment();
	void	UpdateControls();
	void	ResetStartRanges();
	void	EnableControls(BOOL fEnable);
	BOOL	GetTimeSig(DMUS_TIMESIGNATURE &TimeSig);
	void	UpdateLengthButtonText();

protected:
	// Generated message map functions
	//{{AFX_MSG(CSegmentPPG)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLength();
	afx_msg void OnKillfocusEditPlayStartBar();
	afx_msg void OnKillfocusEditPlayStartBeat();
	afx_msg void OnKillfocusEditPlayStartGrid();
	afx_msg void OnKillfocusEditPlayStartTick();
	afx_msg void OnKillfocusEditTrackGroup();
	afx_msg void OnDeltaposSpinPlayStartBar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinPlayStartBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinPlayStartGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinPlayStartTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinTrackGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditName();
	afx_msg void OnCheckAudiopath();
	afx_msg void OnCheckExtendTimesig();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTPPG_H__6CEFCD03_E1CB_11D0_BACD_00805F493F43__INCLUDED_)
