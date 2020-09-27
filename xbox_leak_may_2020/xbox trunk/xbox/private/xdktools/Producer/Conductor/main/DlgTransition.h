#if !defined(AFX_DLGTRANSITION_H__6E01F2D4_00B0_11D2_88D2_00C04FBF8D15__INCLUDED_)
#define AFX_DLGTRANSITION_H__6E01F2D4_00B0_11D2_88D2_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgTransition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgTransition dialog

class DlgTransition : public CDialog
{
	friend class CToolbarHandler;
// Construction
public:
	DlgTransition(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgTransition)
	enum { IDD = IDD_DIALOG_TRANSITION };
	CButton	m_buttonTransNothing;
	CButton	m_radioAfterPrepare;
	CButton	m_radioQuickResponse;
	CButton	m_buttonSegDefault;
	CComboBox	m_comboSegment;
	CStatic	m_staticNoMarkers;
	CButton	m_buttonBoundary;
	CStatic	m_staticAlignPrompt;
	CButton	m_radioSwitchAnyTime;
	CButton	m_radioSwitchAnyGrid;
	CButton	m_radioSwitchAnyBeat;
	CButton	m_radioSwitchAnyBar;
	CStatic	m_staticCustomIdPrompt;
	CButton	m_radioEntirePattern;
	CButton	m_radioEntireAdditional;
	CButton	m_radio1BarPattern;
	CButton	m_radio1BarAdditional;
	CSpinButtonCtrl	m_spinCustomId;
	CEdit	m_editCustomId;
	CComboBox	m_comboPattern;
	CComboBox	m_comboBoundary;
	CButton	m_buttonModulate;
	CButton	m_buttonLong;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgTransition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateSegmentAndPatternTransitionControls( void );

	ConductorTransitionOptions		m_TransitionOptions; // Transition options.

	// Generated message map functions
	//{{AFX_MSG(DlgTransition)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckLong();
	afx_msg void OnSelChangeComboPattern();
	afx_msg void OnKillFocusCustomId();
	afx_msg void OnDeltaPosCustomIdSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeComboBoundary();
	afx_msg void OnCheckBoundary();
	afx_msg void OnSelchangeComboTransSegment();
	afx_msg void OnRadioEntireAdditional();
	afx_msg void OnRadioEntirePattern();
	afx_msg void OnRadioOneBarAdditional();
	afx_msg void OnRadioOneBarPattern();
	afx_msg void OnCheckModulate();
	afx_msg void OnCheckSegDefault();
	afx_msg void OnCheckTransNothing();
	afx_msg void OnRadioPrepareBtn();
	afx_msg void OnRadioInvalidateBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRANSITION_H__6E01F2D4_00B0_11D2_88D2_00C04FBF8D15__INCLUDED_)
