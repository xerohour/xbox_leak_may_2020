#if !defined(AFX_TABVIRTUALSEGMENT_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABVIRTUALSEGMENT_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TabVirtualSegment.h : header file
//

typedef struct _PPGTabVirtualSegment
{
	_PPGTabVirtualSegment( void )
	{
		dwPageIndex = 1;
		dwNbrMeasures = 0;
		lStartMeasure = 0;
		lStartBeat = 0;
		lStartGrid = 0;
		lStartTick = 0;
		dwTimeSigGroupBits = 0;
		dwResolution = 0;
		dwFlagsUI = 0;
	}

	DWORD				dwPageIndex;
	CString				strName;
	DWORD				dwNbrMeasures;
	long				lStartMeasure;
	long				lStartBeat;
	long				lStartGrid;
	long				lStartTick;
	DWORD				dwTimeSigGroupBits;
	DWORD				dwResolution;
	DWORD				dwFlagsUI;
} PPGTabVirtualSegment;


#define TAB_VSEG_RESOLUTION_FLAGS	(DMUS_SEGF_TIMESIG_ALWAYS | DMUS_SEGF_USE_AUDIOPATH)


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment dialog

class CTabVirtualSegment : public CPropertyPage
{
// Construction
public:
	CTabVirtualSegment( IDMUSProdPropPageManager* pIPageManager );
	~CTabVirtualSegment();

// Dialog Data
	//{{AFX_DATA(CTabVirtualSegment)
	enum { IDD = IDD_TAB_VIRTUAL_SEGMENT };
	CEdit	m_editName;
	CButton	m_btnLength;
	CEdit	m_editStartMeasure;
	CEdit	m_editStartBeat;
	CEdit	m_editStartGrid;
	CEdit	m_editStartTick;
	CEdit	m_editTrackGroup;
	CSpinButtonCtrl	m_spinStartMeasure;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinTrackGroup;
	CButton	m_checkExtendTimeSig;
	CButton	m_checkAudioPath;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabVirtualSegment)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTabVirtualSegment)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocusEditName();
	afx_msg void OnCheckAudioPath();
	afx_msg void OnDoubleClickedCheckAudioPath();
	afx_msg void OnCheckExtendTimeSig();
	afx_msg void OnDoubleClickedCheckExtendTimeSig();
	afx_msg void OnSpinStartMeasure(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartBeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinStartTick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinTrackGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusStartMeasure();
	afx_msg void OnKillFocusStartBeat();
	afx_msg void OnKillFocusStartGrid();
	afx_msg void OnKillFocusStartTick();
	afx_msg void OnKillFocusTrackGroup();
	afx_msg void OnButtonLength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( BOOL fEnable );
	void UpdateObject();
	void SetEditControl( CEdit& edit, long lValue, int nOffset );
	bool HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset );
	bool HandleKillFocus( CSpinButtonCtrl& spin, DWORD& dwUpdateVal, int nOffset );
	bool HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );

public:
	void SetObject( IDMUSProdPropPageObject* pIPropPageObject );
	void RefreshTab( void );

protected:
	IDMUSProdPropPageManager*		m_pIPageManager;
	IDMUSProdPropPageObject*		m_pIPropPageObject;
	PPGTabVirtualSegment			m_PPGTabVirtualSegment;
	long							m_lTrackGroupBits;
	BOOL							m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABVIRTUALSEGMENT_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
