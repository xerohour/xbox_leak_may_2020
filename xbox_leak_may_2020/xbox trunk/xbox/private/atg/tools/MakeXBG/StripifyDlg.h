//-----------------------------------------------------------------------------
// File: StripifyDlg.h
//
// Desc: Code to implement a dialog to let the user stripify a mesh.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Name: class StripifyDlg
// Desc: A dialog to let the user stripify a mesh
//-----------------------------------------------------------------------------
class StripifyDlg : public CDialog
{
    DECLARE_DYNAMIC(StripifyDlg)
// Construction
public:
    StripifyDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    //{{AFX_DATA(StripifyDlg)
    enum { IDD = IDD_STRIPIFY };
    CD3DFrame* m_pFrame;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(StripifyDlg)
    afx_msg void OnStrip();
	afx_msg void OnOptForIndices();
	afx_msg void OnOptForCache();
	afx_msg void OnOutputTriList();
	afx_msg void OnOutputTriStrip();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

	DWORD m_dwOptimizeFlag;
	DWORD m_dwOutputFlag;
};




