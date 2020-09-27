// ComponentCntrItem.cpp : implementation of the CComponentCntrItem class
//

#include "stdafx.h"
#include "JazzApp.h"
#include "ComponentCntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem Interface map

BEGIN_INTERFACE_MAP(CComponentCntrItem, COleClientItem)
    INTERFACE_PART(CComponentCntrItem, IID_IServiceProvider, ServiceProvider)
    INTERFACE_PART(CComponentCntrItem, IID_IDispatch, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem implementation

IMPLEMENT_SERIAL(CComponentCntrItem, COleClientItem, 0)

CComponentCntrItem::CComponentCntrItem(CComponentDoc* pContainer)
	: COleClientItem(pContainer)
{
	// TODO: add one-time construction code here
	
}

CComponentCntrItem::~CComponentCntrItem()
{
	// TODO: add cleanup code here
	
}

void CComponentCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	// When an item is being edited (either in-place or fully open)
	//  it sends OnChange notifications for changes in the state of the
	//  item or visual appearance of its content.

	// TODO: invalidate the item by calling UpdateAllViews
	//  (with hints appropriate to your application)

	GetDocument()->UpdateAllViews(NULL);
		// for now just update ALL views/no hints
}

BOOL CComponentCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// During in-place activation CComponentCntrItem::OnChangeItemPosition
	//  is called by the server to change the position of the in-place
	//  window.  Usually, this is a result of the data in the server
	//  document changing such that the extent has changed or as a result
	//  of in-place resizing.
	//
	// The default here is to call the base class, which will call
	//  COleClientItem::SetItemRects to move the item
	//  to the new position.

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	// TODO: update any cache you may have of the item's rectangle/extent

	return TRUE;
}

void CComponentCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	// During in-place activation, CComponentCntrItem::OnGetItemPosition
	//  will be called to determine the location of this item.  The default
	//  implementation created from AppWizard simply returns a hard-coded
	//  rectangle.  Usually, this rectangle would reflect the current
	//  position of the item relative to the view used for activation.
	//  You can obtain the view by calling CComponentCntrItem::GetActiveView.

	// TODO: return correct rectangle (in pixels) in rPosition

	rPosition.SetRect(10, 10, 210, 210);
}

void CComponentCntrItem::OnActivate()
{
    // Allow only one inplace activate item per frame
    CComponentView* pView = GetActiveView();
    ASSERT_VALID(pView);
    COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
    if (pItem != NULL && pItem != this)
        pItem->Close();
    
    COleClientItem::OnActivate();
}

void CComponentCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

    // Hide the object if it is not an outside-in object
    DWORD dwMisc = 0;
    m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
    if (dwMisc & OLEMISC_INSIDEOUT)
        DoVerb(OLEIVERB_HIDE, NULL);
}

void CComponentCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// Call base class first to read in COleClientItem data.
	// Since this sets up the m_pDocument pointer returned from
	//  CComponentCntrItem::GetDocument, it is a good idea to call
	//  the base class Serialize first.
	COleClientItem::Serialize(ar);

	// now store/retrieve data specific to CComponentCntrItem
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem diagnostics

#ifdef _DEBUG
void CComponentCntrItem::AssertValid() const
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	COleClientItem::AssertValid();
}

void CComponentCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem IServiceProvider implementation

/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem::XServiceProvider::QueryInterface

STDMETHODIMP CComponentCntrItem::XServiceProvider::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, ServiceProvider )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem::XServiceProvider::AddRef

STDMETHODIMP_(ULONG) CComponentCntrItem::XServiceProvider::AddRef()
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, ServiceProvider )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem::XServiceProvider::Release

STDMETHODIMP_(ULONG) CComponentCntrItem::XServiceProvider::Release()
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, ServiceProvider )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem::XServiceProvider::QueryService implementation

STDMETHODIMP CComponentCntrItem::XServiceProvider::QueryService( REFGUID rsid, REFIID riid, void **ppvObj )
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, ServiceProvider )

	if( ppvObj == NULL )
	{
		return E_POINTER;
	}

	*ppvObj = NULL;

	if( !IsEqualGUID ( rsid, SID_IBindHost ) )
	{
        return E_FAIL;	// should be SVC_E_UNKNOWNSERVICE, which I can't find
	}

	if( theApp.m_pIServiceProvider == NULL )
	{
		return E_FAIL;
	}

    return theApp.m_pIServiceProvider->QueryService( rsid, riid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem IDispatch implementation	

/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem IDispatch::QueryInterface

STDMETHODIMP CComponentCntrItem::XDispatch::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, Dispatch )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem IDispatch::AddRef

STDMETHODIMP_(ULONG) CComponentCntrItem::XDispatch::AddRef()
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, Dispatch )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem::XDispatch::Release

STDMETHODIMP_(ULONG) CComponentCntrItem::XDispatch::Release()
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, Dispatch )

	return (ULONG)pThis->ExternalRelease();
}
	
/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem IDispatch::GetTypeInfoCount

HRESULT CComponentCntrItem::XDispatch::GetTypeInfoCount( UINT* pctInfo )
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, Dispatch )

	*pctInfo = 0;

	return S_OK;
}

	
/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem IDispatch::GetTypeInfo

HRESULT CComponentCntrItem::XDispatch::GetTypeInfo( UINT itinfo, LCID lcid, ITypeInfo** ppITypeInfo )
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, Dispatch )

	*ppITypeInfo = NULL;

	return E_NOTIMPL;
}

	
/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem IDispatch::GetIDsOfNames

HRESULT CComponentCntrItem::XDispatch::GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames,
													  UINT cNames, LCID lcid, DISPID* rgDispID )
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, Dispatch )

	*rgszNames = NULL;
	*rgDispID = NULL;

	return E_NOTIMPL;
}

	
/////////////////////////////////////////////////////////////////////////////
// CComponentCntrItem IDispatch::Invoke
	
HRESULT CComponentCntrItem::XDispatch::Invoke( DISPID dispIDMember, REFIID riid, LCID lcid,
											   unsigned short wFlags, DISPPARAMS* pDispParams,
											   VARIANT* pVarResult, EXCEPINFO* pExcepInfo,
											   UINT* puArgErr)
{
	METHOD_PROLOGUE_EX_( CComponentCntrItem, Dispatch )

	HRESULT hr;
	VARIANT varResult;

	if( riid != IID_NULL )
	{
		return E_INVALIDARG;
	}

	if( pVarResult == NULL )
	{
		pVarResult = &varResult;
	}

	VariantInit( pVarResult );

	//The most common case is Boolean; use as an initial type.
	V_VT(pVarResult) = VT_BOOL;

	//Anything but a property get is invalid.
	if( !(DISPATCH_PROPERTYGET & wFlags) )
	{
		return DISP_E_MEMBERNOTFOUND;
	}

	hr = S_OK;

	switch( dispIDMember )
	{
		case DISPID_AMBIENT_LOCALEID:
			V_I4(pVarResult) = GetThreadLocale();
			V_VT(pVarResult) = VT_I4;
			break;

		case DISPID_AMBIENT_USERMODE:
			V_BOOL(pVarResult) = TRUE;
			break;

		case DISPID_AMBIENT_UIDEAD:
			V_BOOL(pVarResult) = FALSE;
			break;

		case DISPID_AMBIENT_SUPPORTSMNEMONICS:
			V_BOOL(pVarResult) = TRUE;
			break;

		case DISPID_AMBIENT_SHOWGRABHANDLES:
			V_BOOL(pVarResult) = FALSE;
			break;

		case DISPID_AMBIENT_SHOWHATCHING:
			V_BOOL(pVarResult) = FALSE;
			break;

		case DISPID_AMBIENT_MESSAGEREFLECT:
			V_BOOL(pVarResult) = FALSE;
			break;

		case DISPID_AMBIENT_AUTOCLIP:
			V_BOOL(pVarResult) = FALSE;
			break;

		default:
			hr = DISP_E_MEMBERNOTFOUND;
			break;
	}

	return hr;
}
