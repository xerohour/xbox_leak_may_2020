#if !defined(AFX_PROPPAGECURVE_H__3251F4C3_DA91_11D1_89B1_00C04FD912C8__INCLUDED_)
#define AFX_PROPPAGECURVE_H__3251F4C3_DA91_11D1_89B1_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropPageCurve.h : header file
//

#include "resource.h"
#include <afxdlgs.h>
#include "CurvePropPageMgr.h"
#include "SequenceStripMgrApp.h"

extern CSequenceStripMgrApp theApp;

class CCurvePropPageMgr;
class CPropCurve;

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
	CStatic	m_staticPBRSemitones;
	CStatic	m_staticPBRCents;
	CSpinButtonCtrl	m_spinPBREndCents;
	CSpinButtonCtrl	m_spinPBRStartCents;
	CEdit	m_editPBRStartCents;
	CEdit	m_editPBREndCents;
	CStatic	m_staticMergeIndex;
	CSpinButtonCtrl	m_spinMergeIndex;
	CEdit	m_editMergeIndex;
	CButton	m_checkStartCurrent;
	CSpinButtonCtrl	m_spinChordBeat;
	CSpinButtonCtrl	m_spinChordBar;
	CEdit	m_editChordBeat;
	CEdit	m_eidtChordBar;
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

// Implementation
protected:
	void EnableItem( int nID, BOOL fEnable );
	void EnableControls( BOOL fEnable );
	void SetControlRanges( CPropCurve* pPropCurve );
	void SetStartDurRanges( CPropCurve* pPropCurve );
	void SetEndRanges( CPropCurve* pPropCurve );
	void SetChordRanges( CPropCurve* pPropCurve );
	void StartEndValueToString( int nValue, CString& strValue ); 
	int StringToStartEndValue( LPCTSTR pszNewStartValue );
	void StartEndValueToCentsString( int nValue, CString& strValue ); 
	void SetSpinStartValuePos( int nStartValue );
	void SetSpinEndValuePos( int nEndValue );
	void SetSpinStartCentsValuePos( int nStartValue );
	void SetSpinEndCentsValuePos( int nEndValue );
	int GetSpinStartValuePos();
	int GetSpinEndValuePos();
	void HandleEditChange(CSpinButtonCtrl& spin, DWORD dwChg, long& lUpdateVal);
	void HandleKillFocus(CSpinButtonCtrl& spin,	DWORD dwChg, long& lUpdateVal);
	void HandleShortEditChange(CSpinButtonCtrl& spin, DWORD dwChg, short& nUpdateVal);
	void HandleShortKillFocus(CSpinButtonCtrl& spin,	DWORD dwChg, short& nUpdateVal);
	void SetItemWidth( int nItem, int nWidth);
	void HandleKillFocusStartPBRange( void );
	void HandleKillFocusEndPBRange( void );

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
	afx_msg void OnKillFocusStartValue();
	afx_msg void OnKillFocusEndValue();
	afx_msg void OnChangeStartBar();
	afx_msg void OnKillFocusStartBar();
	afx_msg void OnChangeStartBeat();
	afx_msg void OnKillFocusStartBeat();
	afx_msg void OnChangeStartGrid();
	afx_msg void OnKillFocusStartGrid();
	afx_msg void OnChangeStartTick();
	afx_msg void OnKillFocusStartTick();
	afx_msg void OnChangeEndBar();
	afx_msg void OnKillFocusEndBar();
	afx_msg void OnChangeEndBeat();
	afx_msg void OnKillFocusEndBeat();
	afx_msg void OnChangeEndGrid();
	afx_msg void OnKillFocusEndGrid();
	afx_msg void OnChangeEndTick();
	afx_msg void OnKillFocusEndTick();
	afx_msg void OnChangeDurBar();
	afx_msg void OnKillFocusDurBar();
	afx_msg void OnChangeDurBeat();
	afx_msg void OnKillFocusDurBeat();
	afx_msg void OnChangeDurGrid();
	afx_msg void OnKillFocusDurGrid();
	afx_msg void OnChangeDurTick();
	afx_msg void OnKillFocusDurTick();
	afx_msg void OnDeltaPosSpinEndValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinStartValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditEndValue();
	afx_msg void OnChangeEditStartValue();
	afx_msg void OnChangeEditMergeIndex();
	afx_msg void OnKillfocusEditMergeIndex();
	afx_msg void OnCheckStartCurrent();
	afx_msg void OnChangeEditPbrStartCents();
	afx_msg void OnKillfocusEditPbrStartCents();
	afx_msg void OnChangeEditPbrEndCents();
	afx_msg void OnKillfocusEditPbrEndCents();
	afx_msg void OnDeltaposSpinPbrEndCents(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinPbrStartCents(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGECURVE_H__3251F4C3_DA91_11D1_89B1_00C04FD912C8__INCLUDED_)
