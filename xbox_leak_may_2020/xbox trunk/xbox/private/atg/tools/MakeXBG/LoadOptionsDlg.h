//-----------------------------------------------------------------------------
// File: LoadOptionsDlg.h
//
// Desc: Code to implement a dialog to let the user specifiy load options.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Name: class CLoadOptionsDlg
// Desc: A dialog to let the user specifiy load options.
//-----------------------------------------------------------------------------
class CLoadOptionsDlg : public CDialog
{
    DECLARE_DYNAMIC(CLoadOptionsDlg)
// Construction
public:
    CLoadOptionsDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    //{{AFX_DATA(CLoadOptionsDlg)
    enum { IDD = IDD_XFILELOADOPTIONS };
    BOOL       m_bCollapseMesh;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CLoadOptionsDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




