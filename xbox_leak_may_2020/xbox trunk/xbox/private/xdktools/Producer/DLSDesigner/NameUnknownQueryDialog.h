#if !defined(AFX_NAMEUNKNOWNQUERYDIALOG_H__8F804FE4_569F_4338_A09D_D91E0DC4DF93__INCLUDED_)
#define AFX_NAMEUNKNOWNQUERYDIALOG_H__8F804FE4_569F_4338_A09D_D91E0DC4DF93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"

// NameUnknownQueryDialog.h : header file
//
class CDLSComponent;

/////////////////////////////////////////////////////////////////////////////
// CNameUnknownQueryDialog dialog

class CNameUnknownQueryDialog : public CDialog
{
// Construction
public:
	CNameUnknownQueryDialog(CWnd* pParent = NULL, CDLSComponent* pComponent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNameUnknownQueryDialog)
	enum { IDD = IDD_NAME_UNNKNOWNQUERY_DIALOG };
	CEdit	m_QueryNameEdit;
	CStatic	m_QueryGUIDStatic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameUnknownQueryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void SetQueryGUID(const GUID& guidQuery);
	CString GetQueryName()
	{
		return m_sQueryName;
	};

private:
	void SetGUIDString();	// Sets the GUID on the static control

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNameUnknownQueryDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDLSComponent*	m_pComponent;
	GUID			m_guidQuery;
	CString			m_sQueryName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAMEUNKNOWNQUERYDIALOG_H__8F804FE4_569F_4338_A09D_D91E0DC4DF93__INCLUDED_)
