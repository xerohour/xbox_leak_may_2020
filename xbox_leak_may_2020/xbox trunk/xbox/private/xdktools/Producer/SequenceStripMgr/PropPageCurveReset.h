#if !defined(AFX_PROPPAGECURVERESET_H__14597682_0522_11D2_850D_00A0C99F7E74__INCLUDED_)
#define AFX_PROPPAGECURVERESET_H__14597682_0522_11D2_850D_00A0C99F7E74__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropPageCurveReset.h : header file
//

class CPropCurve;

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset dialog

class CPropPageCurveReset : public CPropertyPage
{
	// DECLARE_DYNCREATE(CPropPageCurveReset)

// Construction
public:
	CPropPageCurveReset(CCurvePropPageMgr* pCurvePropPageMgr);
	~CPropPageCurveReset();

// Dialog Data
	//{{AFX_DATA(CPropPageCurveReset)
	enum { IDD = IDD_PROPPAGE_CURVE_RESET };
	CSpinButtonCtrl	m_spinPBRResetCents;
	CEdit	m_editPBRResetCents;
	CStatic	m_staticPBRSemitones;
	CStatic	m_staticPBRCents;
	CEdit	m_editResetValue;
	CEdit	m_editDurationTick;
	CEdit	m_editDurationGrid;
	CEdit	m_editDurationBeat;
	CEdit	m_editDurationBar;
	CButton	m_checkEnableReset;
	CSpinButtonCtrl	m_spinDurationTick;
	CSpinButtonCtrl	m_spinDurationGrid;
	CSpinButtonCtrl	m_spinDurationBeat;
	CSpinButtonCtrl	m_spinDurationBar;
	CSpinButtonCtrl	m_spinResetValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPageCurveReset)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Member Variables
protected:
	CCurvePropPageMgr*		m_pPageManager;
	BOOL					m_fInUpdateControls;
	BOOL					m_fNeedToDetach;
	BOOL					m_fInOnKillFocusResetValue;

// Implementation
protected:
	void EnableItem( int nID, BOOL fEnable );
	void EnableControls( BOOL fEnable );
	void SetControlRanges( CPropCurve* pPropCurve );
	void UpdateEdit(CPropCurve* pPropCurve, CSpinButtonCtrl& spin, long lValue,	DWORD dwUndt);
	void HandleEditChange(CSpinButtonCtrl& spin, DWORD dwChg, long& lUpdateVal);
	void HandleKillFocus(CSpinButtonCtrl& spin,	DWORD dwChg, long& lUpdateVal);
	void SetSpinResetValuePos( int nResetValue );
	void SetSpinResetCentsValuePos( int nResetValue );
	int GetSpinResetValuePos( void );
	void ResetValueToString( int nValue, CString& strValue ) ;
	int	StringToResetValue( LPCTSTR pszNewValue );
	void ResetValueToCentsString( int nValue, CString& strValue ) ;
	void HandleKillFocusResetPBRange();

public:
	void UpdateControls( CPropCurve* pPropCurve );

	// Generated message map functions
	//{{AFX_MSG(CPropPageCurveReset)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnChangeDurationbar();
	afx_msg void OnKillfocusDurationbar();
	afx_msg void OnChangeDurationbeat();
	afx_msg void OnKillfocusDurationbeat();
	afx_msg void OnChangeDurationgrid();
	afx_msg void OnKillfocusDurationgrid();
	afx_msg void OnChangeDurationtick();
	afx_msg void OnKillfocusDurationtick();
	afx_msg void OnChangeResetValue();
	afx_msg void OnKillfocusResetValue();
	afx_msg void OnEnableResetClicked();
	afx_msg void OnDeltaPosSpinPbrResetCents(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinResetValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditPbrResetCents();
	afx_msg void OnKillfocusEditPbrResetCents();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGECURVERESET_H__14597682_0522_11D2_850D_00A0C99F7E74__INCLUDED_)
