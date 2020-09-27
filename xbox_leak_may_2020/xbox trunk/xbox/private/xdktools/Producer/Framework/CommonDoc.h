#ifndef __COMMONDOC_H__
#define __COMMONDOC_H__

// CommonDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommonDoc document

class CCommonDoc : public COleDocument
{
protected:
	CCommonDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCommonDoc)

// Attributes
public:
	IDMUSProdNode*	m_pIDocRootNode;

// Operations
public:
	virtual BOOL SaveAndCloseDoc();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCommonDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CCommonDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __COMMONDOC_H__
