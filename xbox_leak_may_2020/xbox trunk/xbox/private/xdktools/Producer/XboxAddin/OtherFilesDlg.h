#if !defined(AFX_OTHERFILESDLG_H__131CA465_3D4E_4550_A14D_3FE531767B80__INCLUDED_)
#define AFX_OTHERFILESDLG_H__131CA465_3D4E_4550_A14D_3FE531767B80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OtherFilesListBox.h"

// OtherFilesDlg.h : header file
//

class CFileItem;
class COtherFile;
interface IDMUSProdNode;

/////////////////////////////////////////////////////////////////////////////
// COtherFilesDlg dialog

class COtherFilesDlg : public CDialog
{
// Construction
public:
	COtherFilesDlg(CWnd* pParent = NULL);   // standard constructor
    ~COtherFilesDlg();

	void OnConnectionStateChanged( void );
	bool IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode );
	bool IsFileInUse( CFileItem *pFileItem );
	HRESULT AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode );
	void CleanUpDisplay( void );
	void DeleteAll( void );

// Dialog Data
	//{{AFX_DATA(COtherFilesDlg)
	enum { IDD = IDD_OTHER_FILES };
	CStatic	m_staticOtherFiles;
	COtherFilesListBox	m_listFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COtherFilesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTypedPtrList< CPtrList, COtherFile *> *m_plstOtherFiles;

	HRESULT AddFileToList( COtherFile *pOtherFile );
	bool RemoveFile( CFileItem *pFileItem );

	// Generated message map functions
	//{{AFX_MSG(COtherFilesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERFILESDLG_H__131CA465_3D4E_4550_A14D_3FE531767B80__INCLUDED_)
