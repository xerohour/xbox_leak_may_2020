// mfcawex2Doc.h : interface of the CMfcawex2Doc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCAWEX2DOC_H__1DEE14B4_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAWEX2DOC_H__1DEE14B4_CC4B_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define TESTTEST

class CMfcawex2SrvrItem;

class CMfcawex2Doc : public COleServerDoc
{
protected: // create from serialization only
	CMfcawex2Doc();
	DECLARE_DYNCREATE(CMfcawex2Doc)

// Attributes
public:
	CMfcawex2SrvrItem* GetEmbeddedItem()
		{ return (CMfcawex2SrvrItem*)COleServerDoc::GetEmbeddedItem(); }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawex2Doc)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMfcawex2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfcawex2Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMfcawex2Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWEX2DOC_H__1DEE14B4_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
