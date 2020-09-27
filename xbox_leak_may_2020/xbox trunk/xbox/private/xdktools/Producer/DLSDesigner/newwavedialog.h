#if !defined(AFX_NEWWAVEDIALOG_H__686C5727_2333_4743_BBEB_31AA5C8D6505__INCLUDED_)
#define AFX_NEWWAVEDIALOG_H__686C5727_2333_4743_BBEB_31AA5C8D6505__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// newwavedialog.h : header file
//

#include "Resource.h"
#include "afxtempl.h"

#define DEFAULT_SAMPLERATE_COUNT	8
static CString arrDefaultSampleRates[DEFAULT_SAMPLERATE_COUNT] = {"3000", "8000", "11025", "22050", "32000", "44100", "48000", "88000"};


/////////////////////////////////////////////////////////////////////////////
// CNewWaveDialog dialog

class CNewWaveDialog : public CDialog
{
// Construction
public:
	CNewWaveDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewWaveDialog)
	enum { IDD = IDD_NEW_WAVE };
	CComboBox	m_SampleRateCombo;
	CButton	m_MonoButton;
	CButton	m_8BitSampleButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewWaveDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


public:
	static CString m_sLastSelectedRate;
	static bool	m_bLastSelected8Bit;
	static bool m_bLastSelectedStereo;


private:
	BOOL IsValidSampleRate(DWORD& dwSampleRate);
	void SetSampleSizeButtons();
	void SetChannelButtons();


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewWaveDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void On16bitsampleRadiobtn();
	afx_msg void OnStereoRadiobtn();
	afx_msg void On8bitsampleRadiobtn();
	afx_msg void OnMonoRadiobtn();
	afx_msg void OnKillfocusSampleRateCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static CArray <DWORD, DWORD> m_arraySampleRate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWWAVEDIALOG_H__686C5727_2333_4743_BBEB_31AA5C8D6505__INCLUDED_)
