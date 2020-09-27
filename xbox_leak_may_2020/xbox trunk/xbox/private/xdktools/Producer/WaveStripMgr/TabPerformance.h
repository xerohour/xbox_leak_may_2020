#if !defined(AFX_TABPERFORMANCE_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_TABPERFORMANCE_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_

// TabPerformance.h : header file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include <afxtempl.h>
#include "MySlider.h"

class CPropPageMgrItem;
class CPropTrackItem;
class CTabOnReturnEdit;


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance dialog

class CTabPerformance : public CPropertyPage, CSliderCollection
{
	friend CPropPageMgrItem;
	DECLARE_DYNCREATE(CTabPerformance)

// Construction
public:
	CTabPerformance();
	~CTabPerformance();

// Dialog Data
	//{{AFX_DATA(CTabPerformance)
	enum { IDD = IDD_PROPPAGE_WAVE_PERFORMANCE };
	CEdit	m_dbAttenuationRange;
	CSliderCtrl	m_AttenuationRange;
	CButton	m_checkIgnoreLooping;
	CEdit	m_editLogicalBeat;
	CSpinButtonCtrl	m_spinLogicalBeat;
	CSpinButtonCtrl	m_spinLogicalMeasure;
	CEdit	m_editLogicalMeasure;
	CButton	m_checkNoInvalidate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabPerformance)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabPerformance)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCheckNoInvalidate();
	afx_msg void OnDoubleClickedCheckNoInvalidate();
	afx_msg void OnSpinLogicalMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinLogicalBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusLogicalMeasure();
	afx_msg void OnKillFocusLogicalBeat();
	afx_msg void OnCheckIgnoreLooping();
	afx_msg void OnDoubleclickedCheckIgnoreLooping();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetPropTrackItem( const CPropTrackItem* pPropItem );

protected:
	void UpdateObject();
	void EnableControls( BOOL fEnable );
	bool HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );
	bool HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset );
	void SetEditControl( CEdit& edit, long lValue, int nOffset );

	void SetLogicalTime();

	void SetControlRanges();
	void SetMusicTimeRanges( DWORD dwGroupBits );
	void UpdateAttenuation();
	void UpdateFineTune();
	void UpdateAttenuationRange();
	void UpdateFineTuneRange();
	// CSliderCollection override
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);
	
	IDMUSProdTimeline*		m_pTimeline;		// Weak timeline reference
	CPropPageMgrItem*		m_pPropPageMgr;

	CPropTrackItem			m_PropItem;

	BOOL					m_fNeedToDetach;
	BOOL					m_fValidTrackItem;

	MySlider*				m_pmsAttenuation;
	long					m_lAttenuation;

	MySlider*				m_pmsFineTune;
	long					m_lFineTune;

	MySlider*				m_pmsAttenuationRange;
	long					m_lAttenuationRange;

	MySlider*				m_pmsFineTuneRange;
	long					m_lFineTuneRange;

    long					m_lLogicalMeasure;
	long					m_lLogicalBeat;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABPERFORMANCE_H__1118E501_E93F_11D0_89AB_00A0C9054129__INCLUDED_)
