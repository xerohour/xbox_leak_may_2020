#if !defined(AFX_TABWAVETRACK_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_)
#define AFX_TABWAVETRACK_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabWaveTrack.h : header file
//

#include "resource.h"
#include <MySlider.h>

/////////////////////////////////////////////////////////////////////////////
// CTabWaveTrack dialog

interface IDMUSProdPropPageObject;
interface IDMUSProdPropSheet;

class CGroupBitsPropPageMgr;

struct ioWaveTrackPPG
{
	DWORD	dwPageIndex;
	long	lVolume;		
	DWORD	dwTrackFlagsDM;
};

class CTabWaveTrack : public CPropertyPage, CSliderCollection
{
friend class CGroupBitsPropPageMgr;

// Construction
public:
	CTabWaveTrack();
	~CTabWaveTrack();

// Dialog Data
	//{{AFX_DATA(CTabWaveTrack)
	enum { IDD = IDD_PROPPAGE_TRACK };
	CButton	m_checkPersistVariationControl;
	CButton	m_checkSyncVariations;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabWaveTrack)
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
	//{{AFX_MSG(CTabWaveTrack)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSync();
	afx_msg void OnDoubleClickedSync();
	afx_msg void OnPersist();
	afx_msg void OnDoubleClickedPersist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdatePPO();

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

	// Variables for keeping track of the active property tab (if there is more than one)
	IDMUSProdPropSheet*			m_pIPropSheet;

protected:
	CGroupBitsPropPageMgr*		m_pPropPageMgr;
	IDMUSProdPropPageObject*	m_pPPO;
	ioWaveTrackPPG				m_PPGTrackParams;
	BOOL						m_fNeedToDetach;

    MySlider*					m_pmsAttenuation;
	long						m_lAttenuation;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABWAVETRACK_H__D02E70E9_E7C6_4033_AD9F_A8BDA1620F03__INCLUDED_)
