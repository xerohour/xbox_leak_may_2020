#if !defined(CNTRITEM_H__B6AED015_3BDF_11D0_89AC_00A0C9054129__INCLUDED_)
#define CNTRITEM_H__B6AED015_3BDF_11D0_89AC_00A0C9054129__INCLUDED_

// CntrItem.h : interface of the CJazzCntrItem class
//

class CJazzDoc;
class CJazzView;

class CJazzCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CJazzCntrItem)

// Constructors
public:
	CJazzCntrItem(CJazzDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CJazzDoc* GetDocument()
		{ return (CJazzDoc*)COleClientItem::GetDocument(); }
	CJazzView* GetActiveView()
		{ return (CJazzView*)COleClientItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJazzCntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CJazzCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CNTRITEM_H__B6AED015_3BDF_11D0_89AC_00A0C9054129__INCLUDED)
