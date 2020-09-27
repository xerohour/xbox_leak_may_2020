#ifndef __COMPONENTCNTRITEM_H__
#define __COMPONENTCNTRITEM_H__

// ComponentCntrItem.h : interface of the CComponentCntrItem class
//

class CComponentDoc;
class CComponentView;

class CComponentCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CComponentCntrItem)

// Constructors
public:
	CComponentCntrItem(CComponentDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CComponentDoc* GetDocument()
		{ return (CComponentDoc*)COleClientItem::GetDocument(); }
	CComponentView* GetActiveView()
		{ return (CComponentView*)COleClientItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComponentCntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	//}}AFX_VIRTUAL

// Interface Maps
public:
	// IServiceProvider
	BEGIN_INTERFACE_PART(ServiceProvider, IServiceProvider)
		INIT_INTERFACE_PART(COleClientItem, ServiceProvider)
		STDMETHOD(QueryService)(REFGUID rsid, REFIID iid, void **ppvObj);
	END_INTERFACE_PART(ServiceProvider)

	// IDispatch for ambient properties
    BEGIN_INTERFACE_PART(Dispatch, IDispatch)
		INIT_INTERFACE_PART(COleClientItem, Dispatch)
		STDMETHOD(GetTypeInfoCount)(UINT*);
		STDMETHOD(GetTypeInfo)(UINT, LCID, LPTYPEINFO*);
		STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, UINT, LCID, DISPID*);
		STDMETHOD(Invoke)(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
    END_INTERFACE_PART(Dispatch)

	DECLARE_INTERFACE_MAP()

// Implementation
public:
	~CComponentCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


#endif // __COMPONENTCNTRITEM_H__
