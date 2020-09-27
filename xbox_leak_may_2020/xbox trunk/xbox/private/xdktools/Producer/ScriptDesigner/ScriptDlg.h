#if !defined(AFX_SCRIPTDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_)
#define AFX_SCRIPTDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_

// ScriptDlg.h : header file
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EditSource.h"
#include "EditValue.h"


#define SSE_SOURCE		0x00000001
#define SSE_ROUTINES	0x00000002
#define SSE_VARIABLES	0x00000004
#define SSE_ALL			0xFFFFFFFF

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CScriptDlg : public CFormView
{
friend class CScriptCtrl;
friend class CEditSource;
friend class CEditValue;

public:
	CScriptDlg();           // protected constructor used by dynamic creation
	virtual ~CScriptDlg();

private:
	void FillRoutineListBox();
	void FillVariableListBox();
	void FormatValueText( WCHAR* awchVariableName, CString& strValue );
	int FindVariableAtYPos( int nYPos );

	// Additional methods
public:
	void SyncSource();
	void RefreshControls( DWORD dwFlags );
	void UpdateStatusBar();
	void OnUpdateEditCut( CCmdUI* pCmdUI );
	void OnEditCut();
	void OnUpdateEditCopy( CCmdUI* pCmdUI );
	void OnEditCopy();
	void OnUpdateEditPaste( CCmdUI* pCmdUI );
	void OnEditPaste();
	void OnUpdateEditInsert( CCmdUI* pCmdUI );
	void OnEditInsert();
	void OnUpdateEditDelete( CCmdUI* pCmdUI );
	void OnEditDelete();
	void OnEditSelectAll();
	BOOL OnViewProperties();
	void CallSelectedRoutines();
	void SetSelectedVariable();
	BOOL IsInEditVariable();

private:
	int					m_nLastStartChar;		// Used to display status bar text
	int					m_nLastEndChar;			// Used to display status bar text
	int					m_nLastPos;				// Used to display status bar text

	CString				m_strEditVariableName;	// Variable being edited
	VARIANT				m_EditVariant;			// VARIANT for variable being edited
	CEditValue*			m_pEditValueCtrl;		// Edit control used to edit variable		

public:
	CScriptCtrl*		m_pScriptCtrl;
	CDirectMusicScript*	m_pScript;
	CEditSource			m_editSource;		// Subclasses IDC_SOURCE

// Form Data
public:
	//{{AFX_DATA(CScriptDlg)
	enum { IDD = IDD_DLG_SCRIPT };
	CButton	m_btnInitialize;
	CListCtrl	m_lstbxVariables;
	CStatic	m_staticSourcePrompt;
	CButton	m_btnRefresh;
	CStatic	m_staticVariablePrompt;
	CStatic	m_staticRoutinePrompt;
	CListBox	m_lstbxRoutines;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptDlg)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	LRESULT OnEndEditValue( WPARAM wParam, LPARAM lParam );
	LRESULT OnCancelEditValue( WPARAM wParam, LPARAM lParam );

	// Generated message map functions
	//{{AFX_MSG(CScriptDlg)
	afx_msg void OnDestroy();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblClkRoutineList();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRefresh();
	afx_msg void OnDblClkVariableList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickVariableList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInitVariables();
	afx_msg void OnKeyDownVariableList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturnVariableList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnSetFocusVariableList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTDLG_H__E2ED8AF6_6AD9_11D3_B45F_00105A2796DE__INCLUDED_)
