#if !defined(AFX_BANDABOUTBOX_H__1B9DF6D3_020A_11D1_BAF1_00805F493F43__INCLUDED_)
#define AFX_BANDABOUTBOX_H__1B9DF6D3_020A_11D1_BAF1_00805F493F43__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BandAboutBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBandAboutBox dialog

class CBandAboutBox : public CDialog
{
// Construction
public:
	CBandAboutBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBandAboutBox)
	enum { IDD = IDD_ABOUTBOX_BAND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBandAboutBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBandAboutBox)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BANDABOUTBOX_H__1B9DF6D3_020A_11D1_BAF1_00805F493F43__INCLUDED_)
