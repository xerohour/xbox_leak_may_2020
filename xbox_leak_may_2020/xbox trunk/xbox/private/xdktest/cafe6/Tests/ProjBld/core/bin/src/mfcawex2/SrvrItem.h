// SrvrItem.h : interface of the CMfcawex2SrvrItem class
//

#if !defined(AFX_SRVRITEM_H__1DEE14B9_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_SRVRITEM_H__1DEE14B9_CC4B_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMfcawex2SrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(CMfcawex2SrvrItem)

// Constructors
public:
	CMfcawex2SrvrItem(CMfcawex2Doc* pContainerDoc);

// Attributes
	CMfcawex2Doc* GetDocument() const
		{ return (CMfcawex2Doc*)COleServerItem::GetDocument(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawex2SrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CMfcawex2SrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SRVRITEM_H__1DEE14B9_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
