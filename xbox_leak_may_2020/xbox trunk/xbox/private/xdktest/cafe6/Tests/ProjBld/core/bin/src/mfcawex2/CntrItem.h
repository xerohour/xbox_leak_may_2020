// CntrItem.h : interface of the CMfcawex2CntrItem class
//

#if !defined(AFX_CNTRITEM_H__1DEE14BD_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_CNTRITEM_H__1DEE14BD_CC4B_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMfcawex2Doc;
class CMfcawex2View;

class CMfcawex2CntrItem : public COleClientItem
{
	DECLARE_SERIAL(CMfcawex2CntrItem)

// Constructors
public:
	CMfcawex2CntrItem(CMfcawex2Doc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CMfcawex2Doc* GetDocument()
		{ return (CMfcawex2Doc*)COleClientItem::GetDocument(); }
	CMfcawex2View* GetActiveView()
		{ return (CMfcawex2View*)COleClientItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcawex2CntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL CanActivate();
	//}}AFX_VIRTUAL

// Implementation
public:
	~CMfcawex2CntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__1DEE14BD_CC4B_11D0_84BF_00AA00C00848__INCLUDED_)
