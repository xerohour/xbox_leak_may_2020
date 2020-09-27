#if !defined(AFX_BUTTONDIALOG_H__F37EEF82_6F6E_48F3_BE67_5133A4AFAB57__INCLUDED_)
#define AFX_BUTTONDIALOG_H__F37EEF82_6F6E_48F3_BE67_5133A4AFAB57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonDialog.h : header file
//

#include "scene.h"

/////////////////////////////////////////////////////////////////////////////
// CButtonDialog dialog

class CButtonDialog : public CDialog
{
// Construction
public:
	CButtonDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CButtonDialog)
	enum { IDD = IDD_BUTTONMAP };
	CButton	m_DeleteButton;
	CButton	m_EditButton;
	CButton	m_SaveButton;
	CButton	m_NewButton;
	CListBox	m_ButtonList;
	CEdit	m_ButtonHelp;
	CEdit	m_ButtonLink;
	CEdit	m_ButtonID;
	CEdit	m_ButtonName;
	CEdit		m_SceneName;
	//}}AFX_DATA

	void getSceneCollection(VSceneCollection *pScenes);
	void getSelectedScene(CString selectedScene);
	void FillButtonList(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	VSceneCollection *CurrentScenes;
	CString			 m_pzSelectedScene;
	VScene			 *m_pScene;

	
	int				 nCurEditButton;
	bool			 bButtonChange;
	bool			 bButtonNew;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CButtonDialog)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeButtonlist();
	afx_msg void OnNewbutton();
	afx_msg void OnEditbutton();
	afx_msg void OnSavebutton();
	afx_msg void OnDeletebutton();
	afx_msg void OnDebugbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONDIALOG_H__F37EEF82_6F6E_48F3_BE67_5133A4AFAB57__INCLUDED_)
