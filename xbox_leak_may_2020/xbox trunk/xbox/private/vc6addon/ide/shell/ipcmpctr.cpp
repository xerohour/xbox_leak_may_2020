// ipcmpctr.cpp : implementation of the CIPCompContainerItem class
//

#include "stdafx.h"
#pragma hdrstop

#include "docobvw.h"
#include "shldocs_.h"
#include "ipcmpctr.h"
#include "ipcfrmhk.h"
#include "ipcundo.h"
#include "resource.h"

#include <oleipcid.h>
#include "bwizmgr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPCompContainerItem implementation

IMPLEMENT_SERIAL(CIPCompContainerItem, CDocObjectContainerItem, 0)

BEGIN_INTERFACE_MAP(CIPCompContainerItem, CDocObjectContainerItem)
	INTERFACE_PART(CIPCompContainerItem, IID_IOleInPlaceComponentSite, OleIPComponentSite)
	INTERFACE_PART(CIPCompContainerItem, IID_IDispatch, AmbientDispatch)
	INTERFACE_PART(CIPCompContainerItem, IID_IOleClientSite, MyOleClientSite)
	INTERFACE_PART(CIPCompContainerItem, IID_IOleInPlaceSite, MyOleIPSite)
END_INTERFACE_MAP()

CIPCompContainerItem::CIPCompContainerItem(COleDocument * pContainer)
	: CDocObjectContainerItem(pContainer)
{
	m_fmodeComponentUI = GRPID_UNDEFINED;
}

CIPCompContainerItem::~CIPCompContainerItem()
{
}

BOOL CIPCompContainerItem::OnCompCmdMsg(GUID *pguid, DWORD cmdid, DWORD rgf,
		int nCode, void* pExtra)
{
	if (m_pInPlaceFrame)
	{
		return ((CIPCompFrameHook*)m_pInPlaceFrame)->
						   OnCompCmdMsg(pguid, cmdid, rgf, nCode, pExtra);
	}
	return FALSE;
}

IDispatch * CIPCompContainerItem::GetIDispatch()
{
	return &m_xAmbientDispatch;
}

IOleInPlaceComponentSite * CIPCompContainerItem::GetIOleInPlaceComponentSite()
{
	return &m_xOleIPComponentSite;
}

IOleInPlaceSite* CIPCompContainerItem::GetIOleInPlaceSite()
{
	return &m_xMyOleIPSite;
}

VOID CIPCompContainerItem::OnActivateMe()
{
	ASSERT(m_pOleDocumentView != NULL);
	ASSERT(m_pInPlaceFrame != NULL);
	((CIPCompFrameHook * )m_pInPlaceFrame)->SetInPlaceComponent(m_pOleDocumentView);
}

HRESULT CIPCompContainerItem::GetService(REFGUID guidService, REFIID riid, void **ppvObj)
{
	if(NULL == ppvObj)
		return E_INVALIDARG;

	*ppvObj = NULL;

	// get the doc and see if it wants to allow the
	// component to have the service it is asking for
	CIPCompDoc *pDoc = GetDocument();
	// the doc is null if we are in our shutdown sequence
	if(NULL == pDoc)
		return E_NOINTERFACE;

	ASSERT(NULL != pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CIPCompDoc)));
	// give the doc a shot at disallowing the request
	// S_OK or any E_* cause us to veto the service
	if(S_FALSE != pDoc->VetoService(guidService, riid))
		return E_NOINTERFACE;

	// give the container/site a shot at honoring the service request
	if(SID_OleComponentUIManager == guidService)
	{
		IUnknown *punk = NULL;
		if(m_srpCompUIMgr != NULL)
			punk = m_srpCompUIMgr;
		else
			punk = m_pInPlaceFrame->GetInterface(&IID_IOleComponentUIManager);

		if(NULL != punk)
			return punk->QueryInterface(riid, ppvObj);
	}
	else if(SID_SBuilderWizardManager == guidService)
	{
		if(m_srpBuilderWizardManager == NULL)
		{
			CIPCompBuilderWizardManager *pBWM = new CIPCompBuilderWizardManager(this);
			HRESULT hr = pBWM->ExternalQueryInterface(&IID_IBuilderWizardManager, 
					(void**)&m_srpBuilderWizardManager);

			if(FAILED(hr))
			{
				// its an ole interface, so delete it
				// this way, not by using delete
				pBWM->ExternalRelease();
				return hr;
			}
			// MFC starts ref counting at 1
			m_srpBuilderWizardManager->Release();
		}
		ASSERT(m_srpBuilderWizardManager != NULL);
		return m_srpBuilderWizardManager->QueryInterface(riid, ppvObj);
	}

	// give the document a shot at honoring the service request
	// the doc is null if we are in our shutdown sequence
	if(NULL == pDoc)
		return E_NOINTERFACE;
	ASSERT(NULL != pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CIPCompDoc)));
	HRESULT hr = pDoc->GetService(guidService, riid, ppvObj);
	if(SUCCEEDED(hr))
		return hr;

	// call the base call which performs all the necessary routing
	return CDocObjectContainerItem::GetService(guidService, riid, ppvObj);
}
/////////////////////////////////////////////////////////////////////////////
// Implementation of IOleInplaceComponentSite 

HRESULT CIPCompContainerItem::XOleIPComponentSite::SetUIMode(DWORD dwUIMode)
{
	METHOD_PROLOGUE(CIPCompContainerItem, OleIPComponentSite)

#ifdef UNDONE
		GetToolMgr()->OnNotify(m_iguidMain, m_hWnd, tmNotifyActivate);
#endif

	if (pThis->m_fmodeComponentUI != dwUIMode)
	{
		pThis->m_fmodeComponentUI = dwUIMode;

	//$UNDONE, put up the new toolbars and menus according to the new mode
	}
	
	return NOERROR;
}

IMPLEMENT_ADDREF		(CIPCompContainerItem, OleIPComponentSite)
IMPLEMENT_RELEASE		(CIPCompContainerItem, OleIPComponentSite)
IMPLEMENT_QUERYINTERFACE(CIPCompContainerItem, OleIPComponentSite)

/////////////////////////////////////////////////////////////////////////////
// Implementation of IDispatch

HRESULT CIPCompContainerItem::XAmbientDispatch::GetTypeInfoCount(unsigned int * pctinfo)
{
	METHOD_PROLOGUE(CIPCompContainerItem, AmbientDispatch)
	if (pctinfo == NULL	)
		return E_INVALIDARG;

	*pctinfo = 0;
	return S_OK;
}
HRESULT CIPCompContainerItem::XAmbientDispatch::GetTypeInfo(unsigned int itinfo,
			LCID lcid,ITypeInfo **pptinfo)
{
	METHOD_PROLOGUE(CIPCompContainerItem, AmbientDispatch)
	*pptinfo = NULL;
	return E_NOTIMPL;
}

HRESULT CIPCompContainerItem::XAmbientDispatch::GetIDsOfNames(REFIID riid,
			LPOLESTR *rgszNames, unsigned int cNames,LCID lcid,
	      	DISPID *rgdispid)
{
	METHOD_PROLOGUE(CIPCompContainerItem, AmbientDispatch)
	if (rgszNames == NULL || rgdispid == NULL)
		return E_INVALIDARG;

	for (unsigned int i = 0; i < cNames; i++)
		rgdispid[i] = DISPID_UNKNOWN;

	return E_NOTIMPL;
}

HRESULT CIPCompContainerItem::XAmbientDispatch::Invoke(DISPID dispidMember, 
			REFIID riid,LCID lcid, unsigned short wFlags,
			DISPPARAMS *pdispparams, VARIANT *pvarResult,
			EXCEPINFO *pexcepinfo, unsigned int *puArgErr)
{
	METHOD_PROLOGUE(CIPCompContainerItem, AmbientDispatch)
	
	HRESULT hr	= S_OK;

	if (puArgErr)
		*puArgErr = 0;

	if (pexcepinfo)
		memset(pexcepinfo, 0, sizeof(*pexcepinfo));

	if (!pdispparams)
		return(E_INVALIDARG);

	if (!pvarResult)
		return(E_INVALIDARG);

	ASSERT(pThis->GetDocument() != NULL);
	return pThis->GetDocument()->OnAmbientInvoke(dispidMember, riid, lcid, wFlags, 
				pdispparams, pvarResult, pexcepinfo, puArgErr);
}

IMPLEMENT_ADDREF		(CIPCompContainerItem, AmbientDispatch)
IMPLEMENT_RELEASE		(CIPCompContainerItem, AmbientDispatch)
IMPLEMENT_QUERYINTERFACE(CIPCompContainerItem, AmbientDispatch)

/////////////////////////////////////////////////////////////////////////////
// CIPCompContainerItem::XMyOleClientSite implementation

IMPLEMENT_ADDREF		(CIPCompContainerItem, MyOleClientSite)
IMPLEMENT_RELEASE		(CIPCompContainerItem, MyOleClientSite)
IMPLEMENT_QUERYINTERFACE(CIPCompContainerItem, MyOleClientSite)


STDMETHODIMP CIPCompContainerItem::XMyOleClientSite::SaveObject()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleClientSite)

	return pThis->m_xOleClientSite.SaveObject();
}

STDMETHODIMP CIPCompContainerItem::XMyOleClientSite::GetMoniker(
	DWORD dwAssign, DWORD dwWhichMoniker, LPMONIKER* ppMoniker)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleClientSite)

	if (NULL == ppMoniker)
		return E_INVALIDARG;

	*ppMoniker = NULL;

	// Documents that derive from CIPCompDoc are top level documents and
	// their container (the shell) only contributes a viewport to display that
	// document. Thus, the object full moniker logically is a concatenation
	// of a NULL container moniker and an absolute object relative moniker.
	// Thus we consider it an error to ask for the container moniker.
	if (dwWhichMoniker == OLEWHICHMK_CONTAINER)
		return E_UNEXPECTED;

	if (dwWhichMoniker == OLEWHICHMK_OBJFULL ||
		dwWhichMoniker == OLEWHICHMK_OBJREL)
	{
		CIPCompDoc *pDoc = pThis->GetDocument();
		if (NULL == pDoc)
			return E_OUTOFMEMORY;

		*ppMoniker = pDoc->GetMoniker((OLEGETMONIKER)dwAssign);
		if (NULL == *ppMoniker)
			return E_OUTOFMEMORY;

		return S_OK;
	}

	return E_INVALIDARG;
}

STDMETHODIMP CIPCompContainerItem::XMyOleClientSite::GetContainer(LPOLECONTAINER* ppContainer)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleClientSite)

	return pThis->m_xOleClientSite.GetContainer(ppContainer);
}

STDMETHODIMP CIPCompContainerItem::XMyOleClientSite::ShowObject()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleClientSite)

	return pThis->m_xOleClientSite.ShowObject();
}

STDMETHODIMP CIPCompContainerItem::XMyOleClientSite::OnShowWindow(BOOL fShow)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleClientSite)

	return pThis->m_xOleClientSite.OnShowWindow(fShow);
}

STDMETHODIMP CIPCompContainerItem::XMyOleClientSite::RequestNewObjectLayout()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleClientSite)

	return pThis->m_xOleClientSite.RequestNewObjectLayout();
}

/////////////////////////////////////////////////////////////////////////////
// CIPCompContainerItem::XMyOleIPSite implementation

IMPLEMENT_ADDREF		(CIPCompContainerItem, MyOleIPSite)
IMPLEMENT_RELEASE		(CIPCompContainerItem, MyOleIPSite)
IMPLEMENT_QUERYINTERFACE(CIPCompContainerItem, MyOleIPSite)


STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::GetWindow(HWND* lphwnd)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.GetWindow(lphwnd);
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::ContextSensitiveHelp(
	BOOL fEnterMode)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.ContextSensitiveHelp(fEnterMode);
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::CanInPlaceActivate()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.CanInPlaceActivate();
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::OnInPlaceActivate()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.OnInPlaceActivate();
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::OnUIActivate()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.OnUIActivate();
}

// the following won't be necessary when the MFC team uses a virtual function in the 
// IOleInPlaceSite::GetWindowContext method to allow one to use a derived class of 
// COleFrameHook.
STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::GetWindowContext(
	LPOLEINPLACEFRAME* lplpFrame,
	LPOLEINPLACEUIWINDOW* lplpDoc,
	LPRECT lpPosRect, LPRECT lpClipRect,
	LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)
	ASSERT_VALID(pThis);

	*lplpFrame = NULL;  // init these in-case of mem-alloc failure
	*lplpDoc = NULL;

	CFrameWnd* pMainFrame = NULL;
	CFrameWnd* pDocFrame = NULL;

	SCODE sc = E_UNEXPECTED;
	TRY
	{
		// get position of the item relative to activation view
		CRect rect;
		pThis->OnGetItemPosition(rect);
		::CopyRect(lpPosRect, &rect);
		pThis->OnGetClipRect(rect);
		::CopyRect(lpClipRect, &rect);

		// get the window context information
		if (pThis->OnGetWindowContext(&pMainFrame, &pDocFrame, lpFrameInfo))
		{
			// hook IOleInPlaceFrame interface to pMainFrame, and pDocFrame
			ASSERT(pDocFrame != NULL);
			
			if (pThis->m_pInPlaceFrame == NULL)
			{
				pThis->m_pInPlaceFrame = new CIPCompFrameHook(pMainFrame, pDocFrame, pThis);
				// this data member will hold CIPCompFrameHook alive until CIPCompContainerItem is destroyed	
				pThis->m_srpCompUIMgr = ((CIPCompFrameHook *)pThis->m_pInPlaceFrame)->GetIOleComponentUIManager();
				pThis->m_srpCompUIMgr->AddRef();
			
			}
			pThis->m_pInPlaceFrame->InternalAddRef();
			*lplpFrame = (LPOLEINPLACEFRAME)pThis->m_pInPlaceFrame->
				GetInterface(&IID_IOleInPlaceFrame);

			if (pDocFrame != NULL)
			{
				if (pThis->m_pInPlaceDoc == NULL)
					pThis->m_pInPlaceDoc = pThis->m_pInPlaceFrame;

				pThis->m_pInPlaceDoc->InternalAddRef(); // holds onto the CIPCompFrameHook
				*lplpDoc = NULL;
			}
			
			// save accel table for IOleInPlaceFrame::TranslateAccelerators
			pThis->m_pInPlaceFrame->m_hAccelTable = lpFrameInfo->haccel;

			sc = S_OK;
		}
	}
	CATCH_ALL(e)
	{
		// cleanup memory that may be partially allocated
		delete *lplpFrame;
		ASSERT(*lplpDoc == NULL);
		do { e->Delete(); } while (0);
	}
	END_CATCH_ALL

	return sc;
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::Scroll(SIZE scrollExtent)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.Scroll(scrollExtent);
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::OnUIDeactivate(BOOL fUndoable)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.OnUIDeactivate(fUndoable);
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::OnInPlaceDeactivate()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.OnInPlaceDeactivate();
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::DiscardUndoState()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.DiscardUndoState();
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::DeactivateAndUndo()
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)

	return pThis->m_xOleIPSite.DeactivateAndUndo();
}

STDMETHODIMP CIPCompContainerItem::XMyOleIPSite::OnPosRectChange(
	LPCRECT lpPosRect)
{
	METHOD_PROLOGUE(CIPCompContainerItem, MyOleIPSite)
	ASSERT(lpPosRect != NULL);
	
	// Give the document a chance to resize itself
	ASSERT(pThis->GetDocument() != NULL);
	ASSERT_KINDOF(CIPCompDoc, pThis->GetDocument());
	HRESULT hr = pThis->GetDocument()->OnPosRectChange((RECT*)lpPosRect);
	if(FAILED(hr))
		return hr;
		
	// Now resize the in place object	
	return pThis->m_xOleIPSite.OnPosRectChange(lpPosRect);
}
/////////////////////////////////////////////////////////////////////////////

BOOL CIPCompContainerItem::CreateNewFromClsid(CLSID clsid, IStorage * pStg)
{
	ASSERT_VALID(this);
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(m_pDocument != NULL);
	ASSERT(clsid != GUID_NULL);
	// pStg can be NULL (htm package requires this)
	// ASSERT(pStg != NULL);
	m_dwItemNumber = GetNewItemNumber();

	// Try to create a new storage file.
	//
	USES_CONVERSION; 
	DWORD dwStatus = 0;
	COleRef<IPersistStorage> srpPersistStorage;

	if (FAILED(::CoCreateInstance(clsid, NULL /*punkOuter*/,
		CLSCTX_INPROC_SERVER, IID_IOleObject, (void **)&m_lpObject)))
		goto ErrRtn;
		
	ASSERT(m_lpObject != NULL);
		
	// Retrieve the misc status bits for the object.
	if (FAILED(m_lpObject->GetMiscStatus(DVASPECT_CONTENT, &dwStatus)))
		goto ErrRtn;
		
	// Set the client site first, if we're supposed to.
	//
	if (dwStatus & OLEMISC_SETCLIENTSITEFIRST)
		{
		if (FAILED(m_lpObject->SetClientSite(&m_xMyOleClientSite)))
			goto ErrRtn;
		}

	::OleSetContainedObject(m_lpObject, TRUE);	
	m_lpObject->SetHostNames(T2COLE(theApp.m_pszAppName), NULL);

//	m_lpStorage = pStg;
//	m_lpStorage->AddRef();
	return TRUE;

ErrRtn:
	if(m_lpObject != NULL)
	{
		m_lpObject->Release();
		m_lpObject = NULL;
	}
	return FALSE;
}

HRESULT CIPCompContainerItem::ActivateDocObjectView(CView *pView)
{
	//$UNDONE, this should really take a IMsoView, and if the view already exist, we just need
	// to Activate the view.
	
	ASSERT(HeapValidate(GetProcessHeap(), 0, NULL));
	ASSERT(m_pOleDocumentView == NULL);	// this operation should only be done once
	VERIFY(GetOleDocument());			// make sure we're connected to a doc object

	// Quietly hook up our view to the COleClientItem.  This is normally done by its ::Activate()
	// method, but we can't do that because we don't want to DoVerb() it.
	//
	m_pView = pView;

	// Create a view connected to our IOleInPlaceSite.
	HRESULT hresult = NOERROR;
	if (FAILED(hresult = m_pOleDocument->CreateView(GetIOleInPlaceSite(), NULL, 0, &m_pOleDocumentView)))
	{
		::ShowErrorForInterface(IID_IMsoDocument, m_pOleDocument, hresult);

		// Since we've displayed error information, set hresult to suppress
		// further error messages:
		//
		hresult = OLE_E_PROMPTSAVECANCELLED;
		goto ErrExit;
	}
	
	if (FAILED(hresult = m_pOleDocumentView->Show(TRUE)))
		goto ErrExit;

	if (FAILED(hresult = ((CIPCompFrameHook * )m_pInPlaceFrame)->SetInPlaceComponent(m_pOleDocumentView)))
		goto ErrExit;
	
	// Activate the server UI and get the view window created.
	//
	if (FAILED(hresult = m_pOleDocumentView->UIActivate(TRUE)))
		goto ErrExit;

	OnSizeChange();		// setup initial rectangle for view window

ErrExit:
	ASSERT(HeapValidate(GetProcessHeap(), 0, NULL));	// check whether we're toast
	return hresult;
}

void CIPCompContainerItem::OnDeactivate()
{
	ASSERT_VALID(this);

	ASSERT(m_pInPlaceFrame != NULL);
	((CIPCompFrameHook * )m_pInPlaceFrame)->DoClose();

	// cleanup document interfaces allocated in GetWindowContext
	if (m_pInPlaceDoc != NULL)
	{
		// release in place document
		ASSERT (((CIPCompFrameHook *)m_pInPlaceDoc)->m_pDocFrameWnd->m_pNotifyHook == m_pInPlaceDoc);
		((CIPCompFrameHook *)m_pInPlaceDoc)->m_pDocFrameWnd->m_pNotifyHook = NULL;
		m_pInPlaceDoc->InternalRelease();
		m_pInPlaceDoc = NULL;
	}
	
	CDocObjectContainerItem::OnDeactivate();
	
}

void CIPCompContainerItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	// During in-place activation, CDocObjectContainerItem::OnGetItemPosition
	//  will be called to determine the location of this item.  Usually, this 
	//  rectangle would reflect the current position of the item relative to the 
	//  view used for activation.
	//  You can obtain the view by calling CDocObjectContainerItem::GetActiveView.

	CView * pView = GetActiveView();
	if (pView)
		pView->GetClientRect(&rPosition);
}


BOOL CIPCompContainerItem::CanClose()
{
	COleRef<IOleInPlaceObject> srpInPlaceObject;

	BOOL fRet = TRUE; // failed the QI is considered success on CanClose
	if (SUCCEEDED(m_lpObject->QueryInterface(IID_IOleInPlaceObject, (void **)&srpInPlaceObject)))
	{
		fRet = FALSE;
		ASSERT(srpInPlaceObject != NULL);
		if (SUCCEEDED(srpInPlaceObject->UIDeactivate()) && 
			SUCCEEDED(srpInPlaceObject->InPlaceDeactivate()))
		    fRet = TRUE;
	}
	return fRet;
}

void CIPCompContainerItem::DoClose()
{
	if (m_pDocument != NULL)
		((CIPCompDoc *)m_pDocument)->SetDocObjViewActivate(FALSE);
		
	// remove external lock placed on item during in-place activation
	if (m_bLocked)  //COleClientItem::OnActivate calls OleLockRunning TRUE.
	{
		OleLockRunning(m_lpObject, FALSE, TRUE);
		m_bLocked = FALSE;
	}

	if (m_pOleDocumentView != NULL)
	{
		m_pOleDocumentView->CloseView(0);
		OleRelease(m_pOleDocumentView);
	}
	if (m_pOleDocument != NULL)
		OleRelease(m_pOleDocument);

	Release(OLECLOSE_NOSAVE); 

	// need to Remove ourselves from the Document's item collection, otherwise,
	// we will get deleted in COleDocument destructor.
	if (m_pDocument != NULL)
		m_pDocument->RemoveItem(this);

	LPUNKNOWN lpUnknown = (LPUNKNOWN)GetInterface(&IID_IUnknown);
	ASSERT(lpUnknown != NULL);

	// disconnect the object
	::CoDisconnectObject(lpUnknown, 0);
}

