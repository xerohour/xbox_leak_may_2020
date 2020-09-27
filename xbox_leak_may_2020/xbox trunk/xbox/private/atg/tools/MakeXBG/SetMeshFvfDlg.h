//-----------------------------------------------------------------------------
// File: SetMeshFVFDlg.h
//
// Desc: Code to implement a dialog to let the user modify the FVF code of a
//       mesh.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Name: class CSetMeshFvfDlg
// Desc: A dialog to let the user modify the FVF code of a mesh
//-----------------------------------------------------------------------------
class CSetMeshFvfDlg : public CDialog
{
    DECLARE_DYNAMIC(CSetMeshFvfDlg)
// Construction
public:
    CSetMeshFvfDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    //{{AFX_DATA(CSetMeshFvfDlg)
    enum { IDD = IDD_SETMESHFVF };
    DWORD      m_dwFVF;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CSetMeshFvfDlg)
    afx_msg void OnControl();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




