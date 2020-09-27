#if !defined(AFX_PROPPAGECURVE_H__3251F4C3_DA91_11D1_89B1_00C04FD912C8__INCLUDED_)
#define AFX_PROPPAGECURVE_H__3251F4C3_DA91_11D1_89B1_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropPageCurve.h : header file
//

#include "CurvePropPageMgr.h"
#include "ParamStripMgrApp.h"

extern class CParamStripMgrApp theApp;

#define DIALOG_EDIT_LEN 15

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve dialog

class CPropPageCurve : public CPropertyPage
{
// Construction
public:
	CPropPageCurve( CCurvePropPageMgr* pCurvePropPageMgr );
	~CPropPageCurve();

// Dialog Data
	//{{AFX_DATA(CPropPageCurve)
	enum { IDD = IDD_PROPPAGE_CURVE };
	CComboBox	m_comboEnumStartValue;
	CComboBox	m_comboEnumEndValue;
	CButton	m_btnStartFromCurrent;
	CStatic	m_staticPromptTick;
	CStatic	m_staticPromptGrid;
	CStatic	m_staticPromptBeat;
	CStatic	m_staticPromptBar;
	CSpinButtonCtrl	m_spinStartValue;
	CSpinButtonCtrl	m_spinStartTick;
	CSpinButtonCtrl	m_spinStartGrid;
	CSpinButtonCtrl	m_spinStartBeat;
	CSpinButtonCtrl	m_spinStartBar;
	CSpinButtonCtrl	m_spinEndValue;
	CSpinButtonCtrl	m_spinEndTick;
	CSpinButtonCtrl	m_spinEndGrid;
	CSpinButtonCtrl	m_spinEndBeat;
	CSpinButtonCtrl	m_spinEndBar;
	CSpinButtonCtrl	m_spinDurTick;
	CSpinButtonCtrl	m_spinDurGrid;
	CSpinButtonCtrl	m_spinDurBeat;
	CSpinButtonCtrl	m_spinDurBar;
	CEdit	m_editStartValue;
	CEdit	m_editStartTick;
	CEdit	m_editStartGrid;
	CEdit	m_editStartBeat;
	CEdit	m_editStartBar;
	CEdit	m_editEndValue;
	CEdit	m_editEndTick;
	CEdit	m_editEndGrid;
	CEdit	m_editEndBeat;
	CEdit	m_editEndBar;
	CEdit	m_editDurTick;
	CEdit	m_editDurGrid;
	CEdit	m_editDurBeat;
	CEdit	m_editDurBar;
	CComboBox	m_comboShape;
	CButton	m_btnFlipVert;
	CButton	m_btnFlipHorz;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPageCurve)
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
	BOOL					m_fInOnKillFocusStartValue;
	BOOL					m_fInOnKillFocusEndValue;
	bool					m_bEnumType;
	bool					m_bBoolType; 
	bool					m_bFloatType;
	bool					m_bIntType;


	CPropCurve				m_PropCurve;

	long		m_lStartBar;
	long		m_lStartBeat;
	long		m_lStartGrid;
	long		m_lStartTick;

	long		m_lEndBar;
	long		m_lEndBeat;
	long		m_lEndGrid;
	long		m_lEndTick;

	long		m_lDurBar;
	long		m_lDurBeat;
	long		m_lDurGrid;
	long		m_lDurTick;


// Implementation
protected:
	void EnableItem( int nID, BOOL fEnable );
	void EnableControls( BOOL fEnable );
	void SetControlRanges( CPropCurve* pPropCurve );
	void SetRefTimeRanges( void );
	void SetMusicTimeRanges( DWORD dwGroupBits, CPropCurve* pPropCurve );
	void StartEndValueToString( float nValue, CString& strValue ); 
	float StringToStartEndValue( LPCTSTR pszNewStartValue );
	void SetSpinStartValuePos( int nStartValue );
	void SetSpinEndValuePos( int nEndValue );
	int GetSpinStartValuePos();
	int GetSpinEndValuePos();
	
	bool HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset );
	bool HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal );
	void SetEditControl( CEdit& edit, long lValue, int nOffset );

	void RecomputeTimes();
	void ComputeMinSecGridMs( void );
	void ComputeBarBeatGridTick( void );

	REFERENCE_TIME StartToUnknownTime( void );
	REFERENCE_TIME EndToUnknownTime( void );
	REFERENCE_TIME DurationToUnknownTime( void );
	REFERENCE_TIME MinTimeToUnknownTime( void );
	REFERENCE_TIME MinTimeToRefTime( void );

	void SetStart( REFERENCE_TIME rtNewStart );
	void SetEnd( REFERENCE_TIME rtNewEnd );
	void SetDuration( REFERENCE_TIME rtNewLength );

	void UpdateObject( void );

	void InitShapeCombo(CPropCurve* pPropCurve);


public:
	void UpdateControls( CPropCurve* pPropCurve );

	// Generated message map functions
	//{{AFX_MSG(CPropPageCurve)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeComboShape();
	afx_msg void OnBtnFlipVert();
	afx_msg void OnDoubleClickedBtnFlipVert();
	afx_msg void OnBtnFlipHorz();
	afx_msg void OnDoubleClickedBtnFlipHorz();
	afx_msg void OnDeltaPosSpinEndValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinStartValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditEndValue();
	afx_msg void OnChangeEditStartValue();
	afx_msg void OnKillFocusStartValue();
	afx_msg void OnKillFocusEndValue();
	afx_msg void OnKillfocusEditStartbar();
	afx_msg void OnDeltaposSpinStartbar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditStartbeat();
	afx_msg void OnKillfocusEditStartgrid();
	afx_msg void OnKillfocusEditStarttick();
	afx_msg void OnKillfocusEditEndbar();
	afx_msg void OnKillfocusEditEndbeat();
	afx_msg void OnKillfocusEditEndgrid();
	afx_msg void OnKillfocusEditEndtick();
	afx_msg void OnKillfocusEditDurbar();
	afx_msg void OnKillfocusEditDurbeat();
	afx_msg void OnKillfocusEditDurgrid();
	afx_msg void OnKillfocusEditDurtick();
	afx_msg void OnDeltaposSpinStartbeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinStartgrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinStarttick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinEndbar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinEndbeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinEndgrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinEndtick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinDurbar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinDurbeat(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinDurgrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinDurtick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckStartFromCurrent();
	afx_msg void OnSelchangeComboEnumEndValue();
	afx_msg void OnSelchangeComboEnumStartValue();
	afx_msg void OnKillfocusComboEnumStartValue();
	afx_msg void OnKillfocusComboEnumEndValue();
	afx_msg void OnKillfocusComboShape();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGECURVE_H__3251F4C3_DA91_11D1_89B1_00C04FD912C8__INCLUDED_)
