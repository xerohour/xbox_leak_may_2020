#if !defined(AFX_SCRIPTSDLG_H__3D7391FE_C78D_4270_A011_36835091915D__INCLUDED_)
#define AFX_SCRIPTSDLG_H__3D7391FE_C78D_4270_A011_36835091915D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OtherFilesListBox.h"

// ScriptsDlg.h : header file
//

class COtherFile;
interface IDMUSProdNode;

/////////////////////////////////////////////////////////////////////////////
// CScriptsDlg dialog

class CScriptsDlg : public CDialog
{
// Construction
public:
	CScriptsDlg(CWnd* pParent = NULL);   // standard constructor
    ~CScriptsDlg();

	void OnConnectionStateChanged( void );
	bool IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode );
	void AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode );

// Dialog Data
	//{{AFX_DATA(CScriptsDlg)
	enum { IDD = IDD_SCRIPTS };
	CStatic	m_staticScripts;
	COtherFilesListBox	m_listScripts;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTypedPtrList< CPtrList, COtherFile *> m_lstOtherFiles;

	// Generated message map functions
	//{{AFX_MSG(CScriptsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTSDLG_H__3D7391FE_C78D_4270_A011_36835091915D__INCLUDED_)
