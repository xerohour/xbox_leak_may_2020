#if !defined(AFX_DLGMIDI_H__07F96B02_35F9_11D2_8900_00C04FBF8D15__INCLUDED_)
#define AFX_DLGMIDI_H__07F96B02_35F9_11D2_8900_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DlgMIDI.h : header file
//

#include "resource.h"
#include "OutputTool.h"

//#define USE_LATENCY_HELPTEXT 1

/////////////////////////////////////////////////////////////////////////////
// CROEdit window

/*
class CROEdit : public CEdit
{
// Construction
public:
	CROEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CROEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CROEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CROEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
*/

/////////////////////////////////////////////////////////////////////////////
// MIDI / synth options dialog

class DlgMIDI : public CDialog
{
// Construction
public:
	DlgMIDI(CWnd* pParent = NULL);	 // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgMIDI)
	enum { IDD = IDD_SETUP_MIDI };
	CEdit	m_editOther;
	CComboBox	m_comboLowLatDevice;
	CComboBox	m_comboDefaultSynth;
	CEdit	m_editVoices;
	CSpinButtonCtrl	m_spinVoices;
	CEdit	m_editLatency;
	CSpinButtonCtrl	m_spinLatency;
	CButton	m_btnEchoMidi;
	CComboBox	m_comboEchoInput;
	CSpinButtonCtrl	m_spinEchoPChannels;
	CComboBox	m_comboInput;
	//}}AFX_DATA

	//CEdit	m_editEchoPChannels;
	CEdit			m_editEchoPChannels;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgMIDI)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgMIDI)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDeltaposSpinEchoPchannels(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnKillfocusEditEchoPchannels();
	afx_msg void OnSelchangeComboEchoInput();
	afx_msg void OnButtonLatencyDefault();
	afx_msg void OnCheckLowLatency();
	afx_msg void OnSelchangeComboLowLatDevice();
	afx_msg void OnSelchangeDefaultSynth();
	afx_msg void On1122444896();
	afx_msg void OnOther();
	afx_msg void OnEchoAdvanced();
	//}}AFX_MSG
#ifdef USE_LATENCY_HELPTEXT
	afx_msg void OnChangeEditLatency();
#endif // USE_LATENCY_HELPTEXT
	DECLARE_MESSAGE_MAP()

protected:
	void UpdatePChannelEdit( int nBlock );
	void DisplayPortCreationError( HRESULT hr, CComboBox &comboBox );
	void EnableDlgItem( int nID, bool fEnable );
	bool IsDlgItemEnabled( int nID );
	void RestrictFrequencyRange( const DWORD dwLowFreq, const DWORD dwHighFreq );

#ifdef USE_LATENCY_HELPTEXT
	void UpdateLatencyText( void );

	DWORD	m_dwBaseLatency;
	bool	m_fOrigUsePhoneyDSound;
#endif // USE_LATENCY_HELPTEXT

	int		m_nInitialMIDIInIndex;
	int		m_nInitialEchoMIDIInIndex;

public:
	DWORD	m_dwLatency;
	bool	m_fLatencyAppliesToAllAudioPaths;

	DWORD	m_dwSampleRate;
	DWORD	m_dwOtherSampleRate;
	DWORD	m_dwVoices;
	CLSID	m_clsidDefaultSynth;

	bool	m_fUsePhoneyDSound;
	TCHAR	m_strPhoneyFilterName[MAX_PATH];

	bool	m_fDownloadGM;
	bool	m_fDownloadDLS;

	int		m_anCurrentBlock[ECHO_ADVANCED_PORTS];
	GUID	m_aguidEchoMIDIInPort[ECHO_ADVANCED_PORTS];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMIDI_H__07F96B02_35F9_11D2_8900_00C04FBF8D15__INCLUDED_)
