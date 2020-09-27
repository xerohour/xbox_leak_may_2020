#if !defined(AFX_MODELESSOPS_H__828CE901_15EE_11D3_AE81_00105AA22F40__INCLUDED_)
#define AFX_MODELESSOPS_H__828CE901_15EE_11D3_AE81_00105AA22F40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OlePropSheet.h : header file
//

interface IDMUSProdUpdateObjectNow;

/////////////////////////////////////////////////////////////////////////////
// COlePropSheet

class COlePropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COlePropSheet)

// Construction
public:
	COlePropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COlePropSheet(LPUNKNOWN lpUnk, LPCTSTR pszCaption, IDMUSProdUpdateObjectNow *pIDMUSProdUpdateObjectNow, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COlePropSheet();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COlePropSheet)
	public:
	virtual int DoModal();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	LPUNKNOWN			m_pUnkServer;
	IDMUSProdUpdateObjectNow	*m_pIDMUSProdUpdateObjectNow;
	CPtrArray			m_pageArray;
	UINT				m_iPageCnt;

	// Generated message map functions
protected:
	//{{AFX_MSG(COlePropSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(COlePropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELESSOPS_H__828CE901_15EE_11D3_AE81_00105AA22F40__INCLUDED_)
