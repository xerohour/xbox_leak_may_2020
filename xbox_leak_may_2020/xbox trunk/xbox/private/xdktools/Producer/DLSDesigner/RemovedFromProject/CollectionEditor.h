#if !defined(AFX_COLLECTIONEDITOR_H__FA332522_C73E_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_COLLECTIONEDITOR_H__FA332522_C73E_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//  : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCollectionEditor 

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCollectionCtrl;

class CCollectionEditor : public CFormView
{
friend class CCollectionCtrl;

protected:
	CCollectionEditor(CCollectionCtrl* parent = NULL);         
	DECLARE_DYNCREATE(CCollectionEditor)

// Form Data
public:
	//{{AFX_DATA(CCollectionEditor)
	enum { IDD = IDD_COLLECTION };
	DWORD	m_dwVersion1;
	DWORD	m_dwVersion2;
	DWORD	m_dwVersion3;
	DWORD	m_dwVersion4;
	DWORD	m_dwSampleCount;
	CString	m_csSubject;
	CString	m_csName;
	CString	m_csEngineer;
	CString	m_csCopyright;
	CString	m_csComment;
	DWORD	m_dwArticCount;
	DWORD	m_dwInstCount;
	DWORD	m_dwRegionCount;
	DWORD	m_dwWaveCount;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCollectionEditor)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCollectionEditor();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CCollectionCtrl* m_parent;

	// Generated message map functions
	//{{AFX_MSG(CCollectionEditor)
	afx_msg void OnChangeArticCount();
	afx_msg void OnChangeComment();
	afx_msg void OnChangeCopyright();
	afx_msg void OnChangeEngineer();
	afx_msg void OnChangeInstCount();
	afx_msg void OnChangeName();
	afx_msg void OnChangeRegionCount();
	afx_msg void OnChangeSampleCount();
	afx_msg void OnChangeSubject();
	afx_msg void OnChangeVer1();
	afx_msg void OnChangeVer2();
	afx_msg void OnChangeVer3();
	afx_msg void OnChangeVer4();
	afx_msg void OnChangeWaveCount();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLLECTIONEDITOR_H__FA332522_C73E_11D0_876A_00AA00C08146__INCLUDED_)
