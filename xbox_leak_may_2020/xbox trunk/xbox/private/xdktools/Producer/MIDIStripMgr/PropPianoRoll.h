#if !defined(AFX_PROPPIANOROLL_H__FEAF363B_E67F_11D0_BC02_00A0C922E6EB__INCLUDED_)
#define AFX_PROPPIANOROLL_H__FEAF363B_E67F_11D0_BC02_00A0C922E6EB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <dmusprod.h>
#include "DialogNewPart.h"
// PropPianoRoll.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PropPianoRoll dialog

class PropPianoRoll : public CPropertyPage
{
	DECLARE_DYNCREATE(PropPianoRoll)

// Construction
public:
	PropPianoRoll();
	~PropPianoRoll();

// Dialog Data
	//{{AFX_DATA(PropPianoRoll)
	enum { IDD = IDD_PROPPAGE_PART };
	CStatic	m_staticExtension;
	CComboBox	m_cmbVarMode;
	CEdit	m_editPChannelName;
	CButton	m_checkAutoInvert;
	CSpinButtonCtrl	m_spinPChannel;
	CSpinButtonCtrl	m_spinInvUpper;
	CSpinButtonCtrl	m_spinInvLower;
	CNewPartEdit	m_editPChannel;
	CEdit	m_editName;
	CEdit	m_editInvUpper;
	CEdit	m_editInvLower;
	CComboBox	m_cmbVarLock;
	CComboBox	m_cmbChordLevel;
	CButton	m_btnTimeSignature;
	CButton	m_btnSelNoteColor;
	CButton	m_btnNoteColor;
	CButton	m_btnLength;
	CComboBox	m_cmbPlayMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropPianoRoll)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropPianoRoll)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPlaymode();
	afx_msg void OnButtonNotecolor();
	afx_msg void OnButtonSelectioncolor();
	afx_msg void OnChangePartEditName();
	afx_msg void OnButtonLength();
	afx_msg void OnButtonTimeSignature();
	afx_msg void OnChangePartEditPChannel();
	afx_msg void OnDeltaPosPartSpinPChannel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeComboVarLock();
	afx_msg void OnDeltaPosPartSpinInvLower(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosPartSpinInvUpper(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPartCheckInvAutomatic();
	afx_msg void OnSelchangeComboPlaymode2();
	afx_msg void OnChangePartEditPchannelname();
	afx_msg void OnSelchangeComboVarmode();
	afx_msg void OnKillfocusPartEditPchannel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void EnableControls( PianoRollData* pPRD, BOOL fEnable );
	void SetTimeSignatureBitmap( DirectMusicTimeSig* pTimeSig );
	void FillVarLockComoboBox( PianoRollData* pPRD );
	void FillChordLevelComoboBox( PianoRollData* pPRD );
	void FillInversionBoundaries( const PianoRollData* pPRD );
	CString UpdatePChannelName( DWORD dwPChannel );
	interface IDMUSProdPChannelName*	m_pIPChannelName;
	void InitializePRData( PianoRollData* pPRD );

	// Variables for keeping track of the active property tab (if there is more than one)
public:
	static short*				sm_pnActiveTab;
	IDMUSProdPropSheet*			m_pIPropSheet;

public:
	void SetObject( IDMUSProdPropPageObject* pPPO );
	void RefreshData( void );

protected:
	IDMUSProdPropPageObject*	m_pPPO;
	BOOL						m_fNeedToDetach;
	CString						m_strBroadcastSeg;
	CString						m_strBroadcastPerf;
	CString						m_strBroadcastAPath;
	CString						m_strBroadcastGrp;

private:
	COLORREF m_pcrCustomColors[16];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPIANOROLL_H__FEAF363B_E67F_11D0_BC02_00A0C922E6EB__INCLUDED_)
