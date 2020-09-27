#if !defined(AFX_PCHANNELPROPPAGE_H__D5C695D2_2636_11D2_9A65_006097B01078__INCLUDED_)
#define AFX_PCHANNELPROPPAGE_H__D5C695D2_2636_11D2_9A65_006097B01078__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PChannelPropPage.h : header file
//


#define PRIORITY_LEVELS	4
static UINT arrPriorityLevelNames[PRIORITY_LEVELS] = { IDS_CRITICAL_VOICE_PRIORITY, IDS_HIGH_VOICE_PRIORITY,
													   IDS_STANDARD_VOICE_PRIORITY, IDS_LOW_VOICE_PRIORITY };
				

class CPChannelPropPageManager;
class CCommonInstrumentPropertyObject;

/////////////////////////////////////////////////////////////////////////////
// CPChannelPropPage dialog

class CPChannelPropPage : public CPropertyPage
{

// Construction
public:
	CPChannelPropPage(CPChannelPropPageManager* pPageManager);
	~CPChannelPropPage();
	
	void SetPChannel(CCommonInstrumentPropertyObject* pCommonInstrumentObject);

// Dialog Data
	//{{AFX_DATA(CPChannelPropPage)
	enum { IDD = IDD_TAB_PCHANNEL };
	CSpinButtonCtrl	m_PChNumberSpin;
	CButton	m_IgnorePitchBendRange;
	CSpinButtonCtrl	m_PBTransposeSpin;
	CEdit	m_PBTransposeEdit;
	CSpinButtonCtrl	m_PBOctaveSpin;
	CEdit	m_PBOctaveEdit;
	CStatic	m_InstrumentInfoStatic;
	CComboBox	m_PriorityCombo;
	CButton	m_SoundCanvasCheck;
	CButton	m_IgnoreNoteRange;
	CSpinButtonCtrl	m_HighNoteSpin;
	CEdit	m_HighNoteEdit;
	CSpinButtonCtrl	m_LowNoteSpin;
	CEdit	m_LowNoteEdit;
	CSpinButtonCtrl	m_VolumeSpin;
	CEdit			m_VolumeEdit;
	CSpinButtonCtrl	m_TransposeSpin;
	CEdit			m_TransposeEdit;
	CSpinButtonCtrl	m_PrioritySpin;
	CEdit			m_PriorityEdit;
	CEdit			m_PChNumberEdit;
	CEdit			m_PChNameEdit;
	CSpinButtonCtrl	m_PanSpin;
	CEdit			m_PanEdit;
	CEdit			m_OctaveEdit;
	CSpinButtonCtrl	m_OctaveSpin;
	CButton			m_SelectInstrument;
	CButton			m_IgnoreVolume;
	CButton			m_IgnorePriority;
	CButton			m_IgnorePan;
	CButton			m_IgnoreOctaveTranspose;
	CButton			m_IgnoreInstrument;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPChannelPropPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

//Attributes
public:
	static HMENU  m_hmenuPatch;
	static HMENU  m_hmenuDrums;
	static void CreatePatchPopupMenu();

protected:
		CCommonInstrumentPropertyObject* m_pCommonInstrumentObject;
		CPChannelPropPageManager* m_pPageManager;

// Implementation
protected:
	void LoadIntervals();
	void InitPriorityCombo();

	void EnableItem(int nID, BOOL fEnable);
	void CheckForIgnores(CCommonInstrumentPropertyObject* pCommonInstrumentObject);
	void CheckAndEnableControl(int nCheckState, CWnd& control);
	void EnableControls(BOOL fEnable);
	void UpdateControls(CCommonInstrumentPropertyObject* pCommonInstrumentObject);
	CString GetPanString(BYTE bPan);
	int GetPanValue(CString sPanString);
	void SetPriorityLevel(DWORD lPriorityLevel, DWORD dwPriorityOffset);

	
	// Generated message map functions
	//{{AFX_MSG(CPChannelPropPage)
	afx_msg void OnInstrumentButton();
	afx_msg void OnIgnoreInstrumentCheck();
	afx_msg void OnIgnoreOctavetransposeCheck();
	afx_msg void OnIgnorePanCheck();
	afx_msg void OnIgnorePriorityCheck();
	afx_msg void OnIgnoreVolumeCheck();
	afx_msg void OnDeltaposPanSpin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusOctaveEdit();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillfocusPanEdit();
	afx_msg void OnKillfocusTransposeEdit();
	afx_msg void OnKillfocusVolumeEdit();
	afx_msg void OnKillfocusPriorityEdit();
	afx_msg void OnDeltaposVolumeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposTransposeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposOctaveSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposPrioritySpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIgnoreNoteRangeCheck();
	afx_msg void OnKillfocusHighNoteEdit();
	afx_msg void OnKillfocusLowNoteEdit();
	afx_msg void OnDeltaposLowNoteSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposHighNoteSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUseSoundcanvasCheck();
	afx_msg void OnSelchangePriorityCombo();
	afx_msg void OnChangePchannelNameEdit();
	afx_msg void OnIgnorePitchBendRangeCheck();
	afx_msg void OnKillfocusPitchBendOctaveEdit();
	afx_msg void OnDeltaposPitchBendOctaveSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusPitchBendTransposeEdit();
	afx_msg void OnDeltaposPitchBendTransposeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposPChannelNumberSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusPChannelNumberEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	bool m_fInChange;
	BOOL m_fNeedToDetach;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCHANNELPROPPAGE_H__D5C695D2_2636_11D2_9A65_006097B01078__INCLUDED_)
