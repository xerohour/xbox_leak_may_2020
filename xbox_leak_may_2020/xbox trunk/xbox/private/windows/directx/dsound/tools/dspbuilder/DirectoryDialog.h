#if !defined(AFX_DIRECTORYDIALOG_H__1C8F71E4_0190_4F03_8FE1_DC34D41C1C0B__INCLUDED_)
#define AFX_DIRECTORYDIALOG_H__1C8F71E4_0190_4F03_8FE1_DC34D41C1C0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirectoryDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirectoryDialog dialog

class CDirectoryDialog : public CDialog
{
// Construction
public:

	CDirectoryDialog(CWnd* pParent = NULL);   // standard constructor
	LPCTSTR GetDirectory(void) const { return (LPCTSTR)m_directory; };

// Dialog Data
	//{{AFX_DATA(CDirectoryDialog)
	enum { IDD = IDD_DIRECTORY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	virtual BOOL OnInitDialog(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirectoryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDirectoryDialog)
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int AddDrives(void);
	BOOL AddDriveItem(LPCTSTR pszDrive);
	BOOL SetButtonState(HTREEITEM hItem, LPCTSTR pszPath);
	CString GetPathFromItem(HTREEITEM hItem);
	void DeleteFirstChild(HTREEITEM hItem);
	void DeleteAllChildren(HTREEITEM hItem);
	int AddDirectories(HTREEITEM hItem, LPCTSTR pszPath);


private:

	CImageList	m_ilDrives;
	CTreeCtrl*	m_pTree;
	CString		m_directory;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTORYDIALOG_H__1C8F71E4_0190_4F03_8FE1_DC34D41C1C0B__INCLUDED_)
