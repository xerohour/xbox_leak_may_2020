#if !defined(AFX_PROPERTYSHEETCTRL_H__0E6E1144_ACEF_11D0_89AD_00A0C9054129__INCLUDED_)
#define AFX_PROPERTYSHEETCTRL_H__0E6E1144_ACEF_11D0_89AD_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropertySheetCtrl.h : header file
//


/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER PROPERTY SHEET OBJECT
========================================================================================
@object PropSheet | Represents the <o Framework>'s global property sheet control.  
	DirectMusic Producer <o Component>s can access the Framework's property sheet control
	by calling <om IDMUSProdFramework::QueryInterface> to obtain a pointer to its
	<i IDMUSProdPropSheet> interface.

@supint IDMUSProdPropSheet | Defines interaction with the Framework's PropSheet object.
	Implementation is handled by the Framework.

@comm
	The Framework creates and manages a PropSheet object.  This object is created during
	application launch and exists throughout the entire DirectMusic Producer work session.
	Display of this property sheet is tied to the application's View/Properties menu item.

	In order to use the PropSheet object, Components must implement <i IDMUSProdPropPageManager>
	for each set of pages that will be placed into the property sheet and <i IDMUSProdPropPageObject>
	for each object that will be displayed by those pages.
	
--------------------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
@interface IDMUSProdPropSheet | 
	This interface provides access to the <o Framework>'s global property sheet control.

@comm
	<i IDMUSProdPropSheet> provides the means for a <o Component> to hook into the Framework's
	property sheet control, or <o PropSheet> object.  The Framework creates the PropSheet
	object during application launch and it exists throughout the entire DirectMusic Producer
	work session. A pointer to the Framework's IDMUSProdPropSheet interface can be obtained via
	<om IDMUSProdFramework::QueryInterface>.
	
	In order to use the global property sheet, a Component must implement <i IDMUSProdPropPageManager>.
	The object behind this interface, the <o PropPageManager> object, manages the set of pages
	placed in the property sheet as well as the data currently displayed in those pages.  
	
	The PropPageManager uses the <i IDMUSProdPropPageObject> interface to interact with the object
	whose data is being shown in its pages.  Calls to <om IDMUSProdPropPageManager::SetObject> set
	the PropPageManager's current object so that the same set of pages can be used to display
	data for different objects.

@base public | IUnknown

@xref  <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
-------------------------------------------------------------------------- */

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl

class CPropertySheetCtrl : public CPropertySheet, public IDMUSProdPropSheet
{
	DECLARE_DYNAMIC(CPropertySheetCtrl)

public:
	// IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// IDMUSProdPropSheet functions
// @meth HRESULT | SetPageManager | Sets the property sheet PageManager.  
	HRESULT STDMETHODCALLTYPE SetPageManager( IDMUSProdPropPageManager* pINewPageManager );
// @meth HRESULT | RemovePageManager | Removes the specified PageManager from the property
//		sheet.
	HRESULT STDMETHODCALLTYPE RemovePageManager( IDMUSProdPropPageManager* pIPageManager );

// @meth HRESULT | RemovePageManagerByObject | Removes the current PageManager when the specified
//		PropPageObject is being displayed.
	HRESULT STDMETHODCALLTYPE RemovePageManagerByObject( IDMUSProdPropPageObject* pIPropPageObject );
// @meth HRESULT | RefreshTitleByObject | Redraws the property sheet title when the specified
//		PropPageObject is being displayed. 
	HRESULT STDMETHODCALLTYPE RefreshTitleByObject( IDMUSProdPropPageObject* pIPropPageObject );
// @meth HRESULT | RefreshActivePageByObject | Redraws the active page when the specified
//		PropPageObject is being displayed.  
	HRESULT STDMETHODCALLTYPE RefreshActivePageByObject( IDMUSProdPropPageObject* pIPropPageObject );
// @meth HRESULT | IsEqualPageManagerObject | Determines whether the specified object is 
//		currently being displayed in the property sheet.
    HRESULT STDMETHODCALLTYPE IsEqualPageManagerObject( IDMUSProdPropPageObject* pIPropPageObject );
	
// @meth HRESULT | RefreshTitle | Redraws the title of the property sheet. 
	HRESULT STDMETHODCALLTYPE RefreshTitle();
// @meth HRESULT | RefreshActivePage | Redraws the active page of the property sheet. 
    HRESULT STDMETHODCALLTYPE RefreshActivePage();
// @meth HRESULT | GetActivePage | Returns the active page of the property sheet. 
    HRESULT STDMETHODCALLTYPE GetActivePage( short* pnIndex );
// @meth HRESULT | SetActivePage | Sets the active page of the property sheet. 
    HRESULT STDMETHODCALLTYPE SetActivePage( short nIndex );

// @meth HRESULT | Show | Shows/hides the property sheet. 
    HRESULT STDMETHODCALLTYPE Show( BOOL fShow );
// @meth HRESULT | IsShowing | Determines the visibility state of the property sheet. 
    HRESULT STDMETHODCALLTYPE IsShowing();

	// Additional methods
protected:
	void RemoveDummyPage();
	BOOL InsertThePage( HPROPSHEETPAGE hPage );
	void ChangeTitle( BSTR bstrTitle, BOOL fAddPropertiesText );
	LRESULT OnRefreshTitle( WPARAM wParam, LPARAM lParam );

public:	
	void InsertDummyPage();

public:
	void RemoveCurrentPageManager();

// Construction
public:
	CPropertySheetCtrl();


// Attributes
private:
    DWORD					m_dwRef;
	IDMUSProdPropPageManager*	m_pIPageManager;	// Page manager owning prop sheet
	CPropertyPage*			m_pDummyPage;
	BOOL					m_fDummyPageInserted;
	short					m_nNbrPages;
	short					m_nActivePage;

public:
	CTypedPtrList<CPtrList, HPROPSHEETPAGE> m_lstPages;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertySheetCtrl)
	public:
	virtual int DoModal();
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	virtual BOOL Create( CWnd* pParentWnd = NULL, DWORD dwStyle = (DWORD)-1, DWORD dwExStyle = 0 );

// Implementation
public:
	virtual ~CPropertySheetCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertySheetCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSHEETCTRL_H__0E6E1144_ACEF_11D0_89AD_00A0C9054129__INCLUDED_)
