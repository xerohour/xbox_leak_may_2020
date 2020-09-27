#if !defined(AFX_CHECKEDOUT_H__02FBA4AC_B3A0_4ADC_B190_CD6CFE733650__INCLUDED_)
#define AFX_CHECKEDOUT_H__02FBA4AC_B3A0_4ADC_B190_CD6CFE733650__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckedOut.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCheckedOut dialog

class CCheckedOut : public CDialog
{
// Construction
public:
	CCheckedOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCheckedOut)
	enum { IDD = IDD_CHECKEDOUT };
	CListBox	m_FilesCheckedIn;
	//}}AFX_DATA

    void CheckFile(char *szFilePath, char *szFileName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckedOut)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCheckedOut)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKEDOUT_H__02FBA4AC_B3A0_4ADC_B190_CD6CFE733650__INCLUDED_)
