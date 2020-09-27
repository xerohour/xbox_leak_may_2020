#if !defined(AFX_PERSONALITYDLG_H__E39DBB02_B70C_11D0_9EDC_00AA00A21BA9__INCLUDED_)
#define AFX_PERSONALITYDLG_H__E39DBB02_B70C_11D0_9EDC_00AA00A21BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PersonalityDlg.h : header file
//

#ifndef	__PERSONALITYDLG_H__
#define	__PERSONALITYDLG_H__

/////////////////////////////////////////////////////////////////////////////
// CPersonalityDlg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPersonalityCtrl;

class CPersonalityDlg : public CFormView
{
public:
	CPersonalityDlg();           // protected constructor used by dynamic creation
	virtual ~CPersonalityDlg();

protected:
	DECLARE_DYNCREATE(CPersonalityDlg)

// Form Data
public:
	//{{AFX_DATA(CPersonalityDlg)
	enum { IDD = IDD_DLG_PERSONALITY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CPersonalityCtrl *m_pPersonalityCtrl;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPersonalityDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPersonalityDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERSONALITYDLG_H__E39DBB02_B70C_11D0_9EDC_00AA00A21BA9__INCLUDED_)
