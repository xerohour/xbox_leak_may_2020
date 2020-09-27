#if !defined(AFX_BUTTONPICKER_H__B2FC0106_A3F0_4E1A_B41A_AF315901C11C__INCLUDED_)
#define AFX_BUTTONPICKER_H__B2FC0106_A3F0_4E1A_B41A_AF315901C11C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonPicker.h : header file
//

#include "scene.h"

/////////////////////////////////////////////////////////////////////////////
// CButtonPicker dialog

class CButtonPicker : public CDialog
{
// Construction
public:
	CButtonPicker(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CButtonPicker)
	enum { IDD = IDD_BUTTONPICK };
	CListBox	m_ButtonList;
	//}}AFX_DATA

	void getSceneCollection(VSceneCollection *pScenes);
    char m_returnButton[255];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonPicker)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	VSceneCollection *CurrentScenes;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CButtonPicker)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDblclkButtonlist();
	afx_msg void OnCancelbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONPICKER_H__B2FC0106_A3F0_4E1A_B41A_AF315901C11C__INCLUDED_)
