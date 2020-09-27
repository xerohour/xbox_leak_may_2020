// mfcawexeDoc.h : interface of the CMfcawexeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCAWEXEDOC_H__CAA16AB0_CAD7_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAWEXEDOC_H__CAA16AB0_CAD7_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CMfcawexeDoc : public CDocument
{
protected: // create from serialization only
	CMfcawexeDoc();
	DECLARE_DYNCREATE(CMfcawexeDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawexeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMfcawexeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfcawexeDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAWEXEDOC_H__CAA16AB0_CAD7_11D0_84BF_00AA00C00848__INCLUDED_)
