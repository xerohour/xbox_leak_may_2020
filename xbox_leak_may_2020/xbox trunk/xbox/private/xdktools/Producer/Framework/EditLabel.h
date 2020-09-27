#if !defined(AFX_EDITLABEL_H__3C8C0742_BBBC_11D0_89AE_00A0C9054129__INCLUDED_)
#define AFX_EDITLABEL_H__3C8C0742_BBBC_11D0_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EditLabel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditLabel window

class CEditLabel : public CEdit
{
// Construction
public:
	CEditLabel( IDMUSProdNode* pINode );

// Attributes
protected:
	IDMUSProdNode* m_pINode;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditLabel)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditLabel)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITLABEL_H__3C8C0742_BBBC_11D0_89AE_00A0C9054129__INCLUDED_)
