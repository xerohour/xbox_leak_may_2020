// XODashBuilderDoc.h : interface of the CXODashBuilderDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XODASHBUILDERDOC_H__C0195150_01F6_4BF0_9636_CD2CA79F24D0__INCLUDED_)
#define AFX_XODASHBUILDERDOC_H__C0195150_01F6_4BF0_9636_CD2CA79F24D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CXODashBuilderDoc : public COleDocument
{
protected: // create from serialization only
	CXODashBuilderDoc();
	DECLARE_DYNCREATE(CXODashBuilderDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXODashBuilderDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXODashBuilderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CXODashBuilderDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XODASHBUILDERDOC_H__C0195150_01F6_4BF0_9636_CD2CA79F24D0__INCLUDED_)
