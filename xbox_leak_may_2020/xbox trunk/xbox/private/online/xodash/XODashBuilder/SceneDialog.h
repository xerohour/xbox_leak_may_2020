#if !defined(AFX_SCENEDIALOG_H__6155FBD3_1474_4AF5_9BF7_6BCEBDE24A1A__INCLUDED_)
#define AFX_SCENEDIALOG_H__6155FBD3_1474_4AF5_9BF7_6BCEBDE24A1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneDialog.h : header file
//
#include "Scene.h"

/////////////////////////////////////////////////////////////////////////////
// CSceneDialog dialog

class CSceneDialog : public CDialog
{
// Construction
public:
	CSceneDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSceneDialog)
	enum { IDD = IDD_SCENEDESC };
	CButton	m_DeleteButton;
	CButton	m_ButtonsButton;
	CButton	m_EditButton;
	CButton	m_NewSceneButton;
	CButton	m_SaveButton;
	CEdit	m_SceneName;
	CEdit	m_FieldSceneID;
	CButton	m_ShowOnTop;
	CEdit	m_TextName;
	CEdit	m_FilePath;
	CListBox	m_SceneList;
	CString	m_SceneIDField;
	//}}AFX_DATA

	VSceneCollection CurrentScenes;
    bool             bSceneChange;
    bool             bSceneNew;
    int              nCurEditScene;

    void FillSceneList(void);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSceneDialog)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeSceneList();
	afx_msg void OnEditscene();
	afx_msg void OnNewscene();
	afx_msg void OnSavebutton();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtons();
	afx_msg void OnDeletebutton();
	virtual BOOL OnInitDialog();
	afx_msg void OnDebugbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEDIALOG_H__6155FBD3_1474_4AF5_9BF7_6BCEBDE24A1A__INCLUDED_)
