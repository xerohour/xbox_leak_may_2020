#if !defined(AFX_DLGEDITSYNTH_H__51ACE707_7179_4F5C_AB26_67BB6DFB6037__INCLUDED_)
#define AFX_DLGEDITSYNTH_H__51ACE707_7179_4F5C_AB26_67BB6DFB6037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditSynth.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgEditSynth dialog

class CDlgEditSynth : public CDialog
{
// Construction
public:
	CDlgEditSynth(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditSynth)
	enum { IDD = IDD_DLG_SYNTH };
	CSpinButtonCtrl	m_spinVoices;
	CEdit	m_editVoices;
	CEdit	m_editSynthName;
	CButton	m_checkReverb;
	CButton	m_checkChorus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditSynth)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	DWORD	m_dwEffects;
	DWORD	m_dwSupportedEffects;
	DWORD	m_dwSampleRate;
	DWORD	m_dwVoices;
	DWORD	m_dwMaxVoices;
	CString	m_strName;

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditSynth)
	afx_msg void OnKillfocusEditVoices();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITSYNTH_H__51ACE707_7179_4F5C_AB26_67BB6DFB6037__INCLUDED_)
