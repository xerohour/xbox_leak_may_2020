#if !defined(AFX_RESAMPLEDLG_H__7E1F5F2F_0BB7_4A93_93DC_8D591F4D658F__INCLUDED_)
#define AFX_RESAMPLEDLG_H__7E1F5F2F_0BB7_4A93_93DC_8D591F4D658F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

// ResampleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResampleDlg dialog

class CResampleDlg : public CDialog
{
// Construction
public:
	CResampleDlg(DWORD dwSampleRate, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResampleDlg)
	enum { IDD = IDD_RESAMPLE };
	CEdit	m_editNewSamplerate;
	//}}AFX_DATA

	DWORD GetSampleRate() { return m_dwNewSampleRate; }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResampleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResampleDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DWORD	m_dwCurrentSampleRate;
	DWORD	m_dwNewSampleRate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESAMPLEDLG_H__7E1F5F2F_0BB7_4A93_93DC_8D591F4D658F__INCLUDED_)
