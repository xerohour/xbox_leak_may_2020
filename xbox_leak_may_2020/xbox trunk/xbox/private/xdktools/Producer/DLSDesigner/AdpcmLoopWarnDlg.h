#if !defined(AFX_ADPCMLOOPWARNDLG_H__1C883E89_70C1_4235_95F7_D99E74525578__INCLUDED_)
#define AFX_ADPCMLOOPWARNDLG_H__1C883E89_70C1_4235_95F7_D99E74525578__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdpcmLoopWarnDlg.h : header file
//

#ifdef DMP_XBOX

#include "resource.h"

class CDLSComponent;


typedef enum {
	XBADPCM_ORIG,
	XBADPCM_UPSAMPLE,
	XBADPCM_DOWNSAMPLE
} XBADPCM_CHANGE;

/////////////////////////////////////////////////////////////////////////////
// CAdpcmLoopWarnDlg dialog

class CAdpcmLoopWarnDlg : public CDialog
{
// Construction
public:
	CAdpcmLoopWarnDlg(CDLSComponent* pComponent, const DWORD dwSampleRate, const DWORD dwLoopStart, const DWORD dwLoopLength, CWnd* pParent = NULL);   // standard constructor

	DWORD m_dwOrigSampleRate;
	DWORD m_dwOrigLoopStart;
	DWORD m_dwOrigLoopLength;

	DWORD m_dwUpsampleSampleRate;
	DWORD m_dwUpsampleLoopStart;
	DWORD m_dwUpsampleLoopLength;
	DWORD m_dwUpsampleInserted;

	DWORD m_dwDownsampleSampleRate;
	DWORD m_dwDownsampleLoopStart;
	DWORD m_dwDownsampleLoopLength;
	DWORD m_dwDownsampleInserted;

	XBADPCM_CHANGE m_xbChange;

// Dialog Data
	//{{AFX_DATA(CAdpcmLoopWarnDlg)
	enum { IDD = IDD_ADPCM_LOOP_WARNING };
	CButton	m_checkNotAgain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdpcmLoopWarnDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdpcmLoopWarnDlg)
	afx_msg void OnCheckNotAgain();
	afx_msg void OnDoubleclickedCheckNotAgain();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDLSComponent* m_pComponent;
};

#endif // DMP_XBOX

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADPCMLOOPWARNDLG_H__1C883E89_70C1_4235_95F7_D99E74525578__INCLUDED_)
