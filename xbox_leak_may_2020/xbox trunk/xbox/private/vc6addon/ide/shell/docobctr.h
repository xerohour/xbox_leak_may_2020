// docobctr.h : interface of the CDocObjectContainerItem class
//

#ifndef __DOCOBCTR_H__
#define __DOCOBCTR_H__

#include <objext.h>
#include <docobj.h>

class CDocObjectDoc;
class CDocObjectView;

/////////////////////////////////////////////////////////////////////////////
// CDocObjectContainerItem

class CDocObjectContainerItem : public COleClientItem
{
	DECLARE_SERIAL(CDocObjectContainerItem)

// Constructors
public:
	CDocObjectContainerItem(COleDocument *pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

protected:
	DECLARE_INTERFACE_MAP()

	// we implement IOleDocumentSite in addition to the normal container interfaces
	BEGIN_INTERFACE_PART(OleDocumentSite, IOleDocumentSite)
		INIT_INTERFACE_PART(CDocObjectContainerItem, OleDocumentSite)
		STDMETHOD(ActivateMe)(IOleDocumentView *pMsoView);
	END_INTERFACE_PART(OleDocumentSite)

	// we implement IOleInPlaceSite as an override for COleClientItem::XOleClientSite
	BEGIN_INTERFACE_PART(OleInPlaceSite, IOleInPlaceSite)
		INIT_INTERFACE_PART(CDocObjectContainerItem, OleInPlaceSite)
		STDMETHOD(GetWindow)(HWND*);
		STDMETHOD(ContextSensitiveHelp)(BOOL);
		STDMETHOD(CanInPlaceActivate)();
		STDMETHOD(OnInPlaceActivate)();
		STDMETHOD(OnUIActivate)();
		STDMETHOD(GetWindowContext)(LPOLEINPLACEFRAME*,
			LPOLEINPLACEUIWINDOW*, LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO);
		STDMETHOD(Scroll)(SIZE);
		STDMETHOD(OnUIDeactivate)(BOOL);
		STDMETHOD(OnInPlaceDeactivate)();
		STDMETHOD(DiscardUndoState)();
		STDMETHOD(DeactivateAndUndo)();
		STDMETHOD(OnPosRectChange)(LPCRECT);
	END_INTERFACE_PART(OleInPlaceSite)

	BEGIN_INTERFACE_PART(ServiceProvider, IServiceProvider)
		STDMETHOD(QueryService) (REFGUID guidService, REFIID iid, void ** ppvObj);
	END_INTERFACE_PART(ServiceProvider)

	virtual BOOL CanActivate() { return TRUE; }

	virtual void OnGetItemPosition(CRect& rPosition);
		// implement OnGetItemPosition if you support in-place activation and we do!

public:
	CDocObjectDoc *GetDocument()
		{ return (CDocObjectDoc *)COleClientItem::GetDocument(); }
	CDocObjectView *GetActiveView()
		{ return (CDocObjectView *)COleClientItem::GetActiveView(); }
	IServiceProvider *GetIServiceProvider()
		{ return &m_xServiceProvider; }
	virtual HRESULT GetService(REFGUID guidService, REFIID riid, void **ppvObj);

	virtual BOOL CreateNewUntitledFile(CLSID clsid);
	virtual BOOL CreateFromExternalFile(LPCTSTR lpszFileName, CLSID clsid);
	virtual BOOL SaveToExternalFile(LPCTSTR lpszFileName);
	virtual BOOL IsModified();
	virtual BOOL IsReadOnlyOnDisk();
	virtual BOOL GetOleDocument();
	virtual void Activate(LONG lVerb, CView *pView);
	virtual void OnSizeChange();
	virtual void OnActivateMe() { }	// default implementation does nothing...
	virtual void DoClose();

protected:
	BOOL m_bReadOnlyOnDisk;

public:
	LPOLEDOCUMENT     m_pOleDocument;		// IOleDocument interface for m_lpObject
	LPOLEDOCUMENTVIEW m_pOleDocumentView;	// IOleDocumentView interface

	IOleInPlaceSite* CDocObjectContainerItem::GetIOleInPlaceSite();

public:
	~CDocObjectContainerItem();
	virtual BOOL OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow);
	virtual void OnInsertMenus(CMenu* pMenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
	virtual void OnSetMenu(CMenu* pMenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CString           m_strCurFileName;		// current file name storage for doc object
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __DOCOBCTR_H__
