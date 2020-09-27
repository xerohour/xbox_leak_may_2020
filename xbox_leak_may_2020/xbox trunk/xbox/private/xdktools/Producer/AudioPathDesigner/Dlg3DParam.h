#if !defined(AFX_DLG3DPARAM_H__CCAD8313_1528_4555_A4EF_AF3A9548FF1A__INCLUDED_)
#define AFX_DLG3DPARAM_H__CCAD8313_1528_4555_A4EF_AF3A9548FF1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg3DParam.h : header file
//

#include "resource.h"
#include "dsound.h"

/////////////////////////////////////////////////////////////////////////////
// C3DEdit class

class C3DEdit :
	public CEdit
{
	DECLARE_DYNCREATE(C3DEdit)
public:
	C3DEdit();

	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	bool	m_fMinusValid;
	bool	m_fDotValid;

protected:
};

/////////////////////////////////////////////////////////////////////////////
// CDlg3DParam dialog

class CDlg3DParam : public CDialog
{
// Construction
public:
	CDlg3DParam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg3DParam)
	enum { IDD = IDD_DLG_3D_POS };
	CEdit	m_editConeOutside;
	CEdit	m_editConeInside;
	C3DEdit	m_editConeVolume;
	C3DEdit	m_editMaxDistance;
	C3DEdit	m_editMinDistance;
	C3DEdit	m_editZVel;
	C3DEdit	m_editYVel;
	C3DEdit	m_editXVel;
	C3DEdit	m_editZPos;
	C3DEdit	m_editYPos;
	C3DEdit	m_editXPos;
	C3DEdit	m_editZCone;
	C3DEdit	m_editYCone;
	C3DEdit	m_editXCone;
	CComboBox	m_comboMode;
	//}}AFX_DATA

	DS3DBUFFER m_ds3DBuffer;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg3DParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool GetFloat( int nControlID, float *pfValue );

	// Generated message map functions
	//{{AFX_MSG(CDlg3DParam)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEditConeInside();
	afx_msg void OnKillfocusEditConeOutside();
	afx_msg void OnKillfocusEditConeVolume();
	afx_msg void OnKillfocusEditMaxDistance();
	afx_msg void OnKillfocusEditMinDistance();
	afx_msg void OnKillfocusEditXcone();
	afx_msg void OnKillfocusEditXpos();
	afx_msg void OnKillfocusEditXvel();
	afx_msg void OnKillfocusEditYcone();
	afx_msg void OnKillfocusEditYpos();
	afx_msg void OnKillfocusEditYvel();
	afx_msg void OnKillfocusEditZcone();
	afx_msg void OnKillfocusEditZpos();
	afx_msg void OnKillfocusEditZvel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG3DPARAM_H__CCAD8313_1528_4555_A4EF_AF3A9548FF1A__INCLUDED_)
