// docobctr.cpp : implementation of the CDocObjectContainerItem class
//

#include "stdafx.h"
#pragma hdrstop

#include "docobctr.h"
#include "docobtpl.h"
#include "docobdoc.h"
#include "docobfhk.h"
#include "oleref.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// REVIEW(scotg)  this needs to be removed when we pick up a new
// docobj.h header that will include this guid.
DEFINE_GUID(SID_SContainerDispatch, 0xb722be00, 0x4e68, 0x101b, 0xa2, 0xbc,0x0, 0xaa, 0x0, 0x40, 0x47,0x70);
DEFINE_GUID(SID_SContainer,			0x0000011b, 0x0,    0x0,    0xc0, 0x0, 0x0, 0x0,  0x0, 0x0,  0x0, 0x46);

/////////////////////////////////////////////////////////////////////////////
// CDocObjectContainerItem implementation

IMPLEMENT_SERIAL(CDocObjectContainerItem, COleClientItem, 0)

BEGIN_INTERFACE_MAP(CDocObjectContainerItem, COleClientItem)
	 INTERFACE_PART(CDocObjectContainerItem, IID_IOleDocumentSite, OleDocumentSite)
	 INTERFACE_PART(CDocObjectContainerItem, IID_IOleInPlaceSite, OleInPlaceSite)
	 INTERFACE_PART(CDocObjectContainerItem, IID_IServiceProvider, ServiceProvider)
END_INTERFACE_MAP()

CDocObjectContainerItem::CDocObjectContainerItem(COleDocument * pContainer)
	: COleClientItem(pContainer)
{
	m_pOleDocument = NULL;
	m_pOleDocumentView = NULL;
	m_lpObject = NULL;
	m_lpStorage = NULL;
	HRESULT hr = ::OleInitialize(NULL);
	// S_OK means initialization succeeded, S_FALSE means already initialized.
	ASSERT(hr == S_OK || hr == S_FALSE);
	m_strCurFileName.Empty();

	m_bReadOnlyOnDisk = FALSE ;
}

CDocObjectContainerItem::~CDocObjectContainerItem()
{
	USES_CONVERSION;

	if (m_pOleDocumentView != NULL)
	{
		m_pOleDocumentView->CloseView(0);
		m_pOleDocumentView->Release();
		m_pOleDocumentView = NULL;
	}

	if (m_pOleDocument != NULL)
	{
		m_pOleDocument->Release();
		m_pOleDocument = NULL;
	}

	::OleUninitialize();
}

BOOL CDocObjectContainerItem::IsModified()
{
	COleRef<IPersistStorage> srpPersistStorage;
	VERIFY(SUCCEEDED(m_lpObject->QueryInterface(IID_IPersistStorage, (LPVOID *)&srpPersistStorage)));
	BOOL bDirty = (srpPersistStorage->IsDirty() != S_FALSE);
	return(bDirty);
}

BOOL CDocObjectContainerItem::IsReadOnlyOnDisk()
{
	return m_bReadOnlyOnDisk ;
}

void CDocObjectContainerItem::OnSizeChange()
{
	ASSERT(m_pView != NULL);

	RECT rectViewClient;
	m_pView->GetClientRect(&rectViewClient);
	if(m_pOleDocumentView)
		VERIFY(SUCCEEDED(m_pOleDocumentView->SetRect(&rectViewClient)));
}

BOOL CDocObjectContainerItem::CreateNewUntitledFile(CLSID clsid)
{
	USES_CONVERSION;

	ASSERT_VALID(this);
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(m_pDocument != NULL);
	ASSERT(clsid != GUID_NULL);

	m_dwItemNumber = GetNewItemNumber();

	// Create a storage for the file.
	if (FAILED(StgCreateDocfile(NULL, STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE |
			STGM_CREATE | STGM_DELETEONRELEASE, 0, &m_lpStorage)))
		return FALSE;


	if(FAILED(::OleCreate(clsid, IID_IOleObject,
				OLERENDER_NONE, NULL,
				&m_xOleClientSite, m_lpStorage, (void **)&m_lpObject)))
		return FALSE;

	m_lpObject->Advise(&m_xAdviseSink, &m_dwConnection);

	ASSERT_VALID(this);
	// save the storage location file name
	m_strCurFileName = "Untitled";
	return TRUE;
}

#include <initguid.h>
DEFINE_GUID (CLSID_Word97, 0x00020906, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

// Creation of an item whose object storage is not embedded in the container storage, but instead is
// in a separate file.
//
// We do this slightly unusual thing because of the unusual container model of VC++ projects,
// in which a project points to a bunch of other things which live as separate files, not embedded
// within the project file.
//
// Also this is what you probably want for doing File/Open on a doc object.
//
// NOTE: this is not a link, because a link still needs there to be a container storage for the link
//		to be stored in.
BOOL CDocObjectContainerItem::CreateFromExternalFile(LPCTSTR lpszFileName, CLSID clsid)
{
	USES_CONVERSION;

	ASSERT_VALID(this);
	ASSERT(m_lpObject == NULL);     // one time only
	ASSERT(m_pDocument != NULL);

	m_dwItemNumber = GetNewItemNumber();

	// Open a storage on the file.
	if(::StgIsStorageFile(T2W(lpszFileName)) == S_OK)
	{
		if( FAILED(::StgOpenStorage(T2W(lpszFileName), NULL, STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_DENY_WRITE, NULL, 0, &m_lpStorage)))
		{
			if(FAILED(::StgOpenStorage(T2W(lpszFileName), NULL, STGM_TRANSACTED | STGM_READ | STGM_SHARE_DENY_WRITE, NULL, 0, &m_lpStorage)))
				return FALSE;

			m_bReadOnlyOnDisk = TRUE ;
		}
		// Load the file into the storage.
		if (FAILED(::OleLoad(m_lpStorage, IID_IOleObject, &m_xOleClientSite, (void **)&m_lpObject)))
			return FALSE;

		if (NULL == m_lpObject)
			return FALSE;
	}
	else
	{
		CPath path;
		path.Create(lpszFileName);

		if ((IsEqualCLSID(clsid, CLSID_Word97)) && (!_tcsicmp(path.GetExtension(), _T(".RTF"))))
		{
			// if the server is Word97 and the file is RTF then open in-place...
			if (FAILED(::CoCreateInstance(clsid, NULL, (CLSCTX_INPROC_SERVER| CLSCTX_LOCAL_SERVER),
											IID_IOleObject, (void **)&m_lpObject)))
				return FALSE;

			if (NULL == m_lpObject)
				return FALSE;

			DWORD dwStatus = 0;
			m_lpObject->GetMiscStatus(DVASPECT_CONTENT, &dwStatus);

			if ((dwStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 &&
				FAILED(m_lpObject->SetClientSite(&m_xOleClientSite)))
			{
				return FALSE;
			}

			COleRef<IPersistFile> pPersistFile;
			VERIFY(SUCCEEDED(m_lpObject->QueryInterface(IID_IPersistFile, (LPVOID *)&pPersistFile)));
			if (FAILED(pPersistFile->Load(T2W(lpszFileName), STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_DENY_WRITE)))
				if (FAILED(pPersistFile->Load(T2W(lpszFileName), STGM_TRANSACTED | STGM_READ | STGM_SHARE_DENY_WRITE)))
					return FALSE;

			if ((dwStatus & OLEMISC_SETCLIENTSITEFIRST) == 0 &&
				FAILED(m_lpObject->SetClientSite(&m_xOleClientSite)))
			{
				return FALSE;
			}
		}
		else
		{
			// otherwise, attempt to open the file with the server application standalone.
			::ShellExecute(NULL, NULL, lpszFileName, NULL, NULL, SW_SHOWDEFAULT);

			// [patbr] hack! flag our document that open was standalone.
			m_pOleDocument = (LPOLEDOCUMENT)0xFFFFFFFF;
			m_pOleDocumentView = (LPOLEDOCUMENTVIEW)0xFFFFFFFF;

			// return FALSE to indicate that no document or view should be created.
			return FALSE;
		}
	}

	m_lpObject->Advise(&m_xAdviseSink, &m_dwConnection);

	ASSERT_VALID(this);
	// save the storage location file name
	m_strCurFileName = lpszFileName;
	return TRUE;
}

#define SAVE_AS_RTF 6
#define CMD_SAVE_AS 102

BOOL CDocObjectContainerItem::SaveToExternalFile(LPCTSTR lpszFileName)
{
	USES_CONVERSION;

	// if the file is not dirty and the specified storage location
	// has not been changed then we don't have to do anything...
	if (!IsModified() && !m_strCurFileName.CompareNoCase(lpszFileName))
		return(TRUE);

	BOOL bRet;
	
	if(m_lpStorage == NULL)
	{
		BOOL bHasName;
		LPOLESTR pwszFileName = NULL;
		COleRef<IPersistFile> pPersistFile;
		VERIFY(SUCCEEDED(m_lpObject->QueryInterface(IID_IPersistFile, (LPVOID *)&pPersistFile)));
		bHasName = (pPersistFile->GetCurFile(&pwszFileName) == S_OK);
		if (!m_strCurFileName.CompareNoCase(lpszFileName) && bHasName)
		{
			// File.Save
			HRESULT hr = pPersistFile->Save(NULL, TRUE);
			if (SUCCEEDED(hr))
			{
				pPersistFile->SaveCompleted(pwszFileName);
				bRet = TRUE;
			}
			else
			{
				// this failure occurs when the file cannot be saved to a
				// storage--for example, when the file is RTF opened by Word.
				// here we do specific handling for the Word/RTF case.
				CPath path;
				path.Create(lpszFileName);
				CDocObjectDoc *pDoc = GetDocument();
				CLSID clsid = pDoc->m_clsid;
				if ((IsEqualCLSID(clsid, CLSID_Word97)) && (!_tcsicmp(path.GetExtension(), _T(".RTF"))))
				{
					COleRef<IDispatch> pDispatch;
					if (SUCCEEDED(m_lpObject->QueryInterface(IID_IDispatch, (LPVOID *)&pDispatch)))
					{
						VARIANTARG rgvarg[2];
						rgvarg[0].vt = VT_I4;
						rgvarg[0].lVal = SAVE_AS_RTF;
						rgvarg[1].vt = VT_BSTR;
						BSTR bstr = SysAllocString(T2W(lpszFileName));
						ASSERT(bstr);
						rgvarg[1].bstrVal = bstr;
						DISPPARAMS dispparams;
						dispparams.rgvarg = rgvarg;
						dispparams.cArgs = 2;
						dispparams.cNamedArgs = 0;
						dispparams.rgdispidNamedArgs = 0;
						VARIANT varResult;
						EXCEPINFO excepInfo;
						UINT uArgErr = 0;
						TRY
						{
							hr = pDispatch->Invoke(CMD_SAVE_AS, IID_NULL, LOCALE_SYSTEM_DEFAULT,
								DISPATCH_METHOD, &dispparams, &varResult, &excepInfo, &uArgErr);
						}
						CATCH_ALL(e)
						{
							ASSERT(FALSE);
						}
						END_CATCH_ALL
						SysFreeString(bstr);
						bRet = SUCCEEDED(hr);
					}
				}
			}
			if(pwszFileName)
				CoTaskMemFree(pwszFileName);
		}
		else
		{
			// File.SaveAs
			if(pwszFileName)
				CoTaskMemFree(pwszFileName);
			pwszFileName = T2W(lpszFileName);
			bRet = SUCCEEDED(pPersistFile->Save(pwszFileName, TRUE));
			if(bRet)
				pPersistFile->SaveCompleted(pwszFileName);
		}
		return bRet;
	}

	if (!m_strCurFileName.CompareNoCase(lpszFileName))
	{
		// File.Save
		COleRef<IPersistStorage> srpPersistStorage;
		VERIFY(SUCCEEDED(m_lpObject->QueryInterface(IID_IPersistStorage, (LPVOID *)&srpPersistStorage)));
		if ((bRet = (SUCCEEDED(srpPersistStorage->Save(m_lpStorage, TRUE)))) == TRUE)
		{
			VERIFY(SUCCEEDED(srpPersistStorage->SaveCompleted(m_lpStorage)));
			VERIFY(SUCCEEDED(m_lpStorage->Commit(STGC_OVERWRITE)));
		}
	}
	else
	{
		// File.SaveAs
		COleRef<IStorage> srpStorage;
		if (FAILED(StgCreateDocfile(NULL, STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, &srpStorage)))
			return FALSE;

		COleRef<IPersistStorage> srpPersistStorage;
		VERIFY(SUCCEEDED(m_lpObject->QueryInterface(IID_IPersistStorage, (LPVOID *)&srpPersistStorage)));
		if ((bRet = (SUCCEEDED(::OleSave(srpPersistStorage, srpStorage, FALSE)))) == TRUE)
		{
			VERIFY(SUCCEEDED(srpPersistStorage->HandsOffStorage()));

			// make sure that the file does not exist on disk--the user has already
			// indicated that he wants to overwrite so delete the file if necessary.
			if (_access(lpszFileName, 0) != -1)
				VERIFY(remove(lpszFileName) == 0);
			
			COleRef<IRootStorage> srpRootStorage;
			VERIFY(SUCCEEDED(srpStorage->QueryInterface(IID_IRootStorage, (LPVOID *)&srpRootStorage)));
			VERIFY(SUCCEEDED(srpRootStorage->SwitchToFile(T2W(lpszFileName))));

			VERIFY(SUCCEEDED(srpPersistStorage->SaveCompleted(srpStorage)));
			m_lpStorage->Release();
			m_lpStorage = srpStorage.Disown();
			VERIFY(SUCCEEDED(m_lpStorage->Commit(STGC_OVERWRITE)));

			// NOTE: IPersistFile::GetCurFile() may return temp file name,
			// so we save away the file name on load and on each save-as.
			m_strCurFileName = lpszFileName;
		}
		else
		{
			VERIFY(SUCCEEDED(srpPersistStorage->SaveCompleted(NULL)));
		}

	}

	return(bRet);
}

void CDocObjectContainerItem::DoClose()
{
	// If the object never got created, do nothing.
	if(m_lpObject == NULL)
		return;

	COleRef<IOleInPlaceObject> srpInPlaceObject;

	BOOL fRet = FALSE;
	if (SUCCEEDED(m_lpObject->QueryInterface(IID_IOleInPlaceObject, (void **)&srpInPlaceObject)))
	{
		ASSERT(srpInPlaceObject != NULL);
		VERIFY(SUCCEEDED(srpInPlaceObject->UIDeactivate()));
		VERIFY(SUCCEEDED(srpInPlaceObject->InPlaceDeactivate()));
	}
	srpInPlaceObject.SRelease();

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

void CDocObjectContainerItem::Activate(LONG lVerb, CView *pView)
{
	RECT rc;
	CWaitCursor curWait;

	// Quietly hook up our view to the COleClientItem.
	m_pView = pView;
	pView->GetClientRect(&rc);
	m_lpObject->DoVerb(lVerb, NULL, &m_xOleClientSite, 0, pView->m_hWnd, &rc);
}

BOOL CDocObjectContainerItem::GetOleDocument()
{
	if (m_pOleDocument != NULL)
		return TRUE;	// already have it

	// Get the object into the running state if it isn't already (otherwise QI will fail).
	if (OleRun(m_lpObject) != S_OK)
		return FALSE;	// couldn't start the server

	HRESULT hresult = m_lpObject->QueryInterface(IID_IOleDocument, (void **)&m_pOleDocument);
	ASSERT((hresult == NOERROR) == (m_pOleDocument != NULL));
	return(hresult == NOERROR);
}

IOleInPlaceSite* CDocObjectContainerItem::GetIOleInPlaceSite()
{
	return &m_xOleIPSite;
}

void CDocObjectContainerItem::OnGetItemPosition(CRect &rrcPosition)
{
	ASSERT_VALID(this);
	ASSERT(AfxIsValidAddress(&rrcPosition, sizeof(RECT)));

	ASSERT_VALID(m_pView);
	m_pView->GetClientRect(&rrcPosition);
}

BOOL CDocObjectContainerItem::OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow)
{
	BOOL bRet = FALSE;
	ASSERT_VALID(pFrameWnd);
	
	if (pFrameWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
		bRet = !DkDocObjectMode(!bShow);

	COleClientItem::OnShowControlBars(pFrameWnd, bShow);
	return bRet;
}

void CDocObjectContainerItem::OnInsertMenus(CMenu* pMenuShared,
	LPOLEMENUGROUPWIDTHS lpMenuWidths)
{

	// Make sure the inplace menu is initialized.
	CPartTemplate* pTemplate = (CPartTemplate*) GetDocument()->GetDocTemplate();
	ASSERT(pTemplate != NULL && pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)));
	pTemplate->SetContainerInfo();

	COleClientItem::OnInsertMenus(pMenuShared, lpMenuWidths);
}

void CDocObjectContainerItem::OnSetMenu(CMenu* pMenuShared, HOLEMENU holemenu,
	HWND hwndActiveObject)
{

	COleClientItem::OnSetMenu(pMenuShared, holemenu, hwndActiveObject);

	 // [fabriced] 20 jun 96 -- Force repaint (MFC bug DevStud'96 #4354)
	theApp.CWinApp::OnIdle(0);
	
	if(NULL != pMenuShared)
		theApp.m_bMenuDirty = FALSE;
}

// This routine contains the IServiceProvider/service routing logic.  We search for 
// services as follows:
//	container/site
//	document
//	package that owns the document
//	application, which in turn searches as follows
//		if CLSID_NULL == guidService, call the shell's FindInterface()
//		mainframe
//		itself, if it ever need to provide services
//		round robin through the packages
HRESULT CDocObjectContainerItem::GetService(REFGUID guidService, REFIID riid, void **ppvObj)
{
	IUnknown *punk = NULL;
	HRESULT hr = E_NOINTERFACE;

	if (ppvObj == NULL)
		return E_INVALIDARG;

	*ppvObj = NULL;

	CPartDoc *pDoc = GetDocument();

	if(SID_SContainerDispatch == guidService)
	{
		// get the container object that gets the dispatch
		// don't continue service routing in this case
		// always return
		ASSERT(NULL != pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));
		IDispatch *pDisp = pDoc->GetAutomationObject(FALSE);	// no addref
		if(NULL != pDisp)
			hr = pDisp->QueryInterface(riid, ppvObj);
		return hr;					// always return
	}

	if(SID_SContainer == guidService)
	{
		// we don't support an IOleContainer COM abstraction
		return E_NOINTERFACE;		// always return
	}

	// give the package a shot at honoring the service request
	CPackage *pPackage = pDoc->GetPackage();
	ASSERT(NULL != pPackage);
	hr = pPackage->GetService(guidService, riid, ppvObj);
	if(SUCCEEDED(hr))
		return hr;

	// give the app (which will ask the mainframe) and all packages
	// if necessary
	return theApp.GetService(guidService, riid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of IOleDocumentSite

IMPLEMENT_ADDREF		(CDocObjectContainerItem, OleDocumentSite)
IMPLEMENT_RELEASE		(CDocObjectContainerItem, OleDocumentSite)
IMPLEMENT_QUERYINTERFACE(CDocObjectContainerItem, OleDocumentSite)

STDMETHODIMP CDocObjectContainerItem::XOleDocumentSite::ActivateMe(IOleDocumentView *pOleView)
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleDocumentSite)

	RECT rc;
	// If we're passed a NULL view pointer, then try to get one from
	// the document object (the object within us).
	if (NULL == pOleView)
	{
		if (FAILED(pThis->m_pOleDocument->CreateView(pThis->GetIOleInPlaceSite(), NULL, 0, &pOleView)))
			return E_OUTOFMEMORY;
		pThis->m_pOleDocumentView = pOleView;
	}

	ASSERT(pOleView == pThis->m_pOleDocumentView);
	// Make sure that the view has our client site.
	pOleView->SetInPlaceSite(pThis->GetIOleInPlaceSite());

	// Activation steps, now that we have a view:
	// 1.  Call IOleDocumentView::SetInPlaceSite (assume done since
	//     either the view already knows, or IOleDocument::CreateView
	//     has done it already.
	// 2.  Call IOleDocumentView::Show to make the thing visible.
	// 3.  Call IOleDocumentView::UIActivate to finish the job.
	// 4.  Call IOleDocumentView::SetRect to give a bunch of space to
	//     the view.  In our case this is the whole client area of
	//     the CPages window.  (Patron doesn't use SetRectComplex)

	// Make it active
	pOleView->Show(TRUE);    

	// virtual call-back for derived classes
	pThis->OnActivateMe();
	
	// This sets up toolbars and menus
	VERIFY(SUCCEEDED(pOleView->UIActivate(TRUE)));

	// Set the window size sensitive to new toolbars
	pThis->m_pView->GetClientRect(&rc);
	pOleView->SetRect(&rc);

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of IOleInPlaceSite

IMPLEMENT_ADDREF		(CDocObjectContainerItem, OleInPlaceSite)
IMPLEMENT_RELEASE		(CDocObjectContainerItem, OleInPlaceSite)
IMPLEMENT_QUERYINTERFACE(CDocObjectContainerItem, OleInPlaceSite)


STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::GetWindow(HWND* lphwnd)
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.GetWindow(lphwnd);
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::ContextSensitiveHelp(
	BOOL fEnterMode)
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.ContextSensitiveHelp(fEnterMode);
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::CanInPlaceActivate()
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.CanInPlaceActivate();
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::OnInPlaceActivate()
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.OnInPlaceActivate();
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::OnUIActivate()
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.OnUIActivate();
}

// the following won't be necessary when the MFC team uses a virtual function in the 
// IOleInPlaceSite::GetWindowContext method to allow one to use a derived class of 
// COleFrameHook.
STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::GetWindowContext(
	LPOLEINPLACEFRAME* lplpFrame, LPOLEINPLACEUIWINDOW* lplpDoc,
	LPRECT lpPosRect, LPRECT lpClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	METHOD_PROLOGUE_EX(CDocObjectContainerItem, OleInPlaceSite)
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
			// hook IOleInPlaceFrame interface to pMainFrame
			if (pThis->m_pInPlaceFrame == NULL)
				pThis->m_pInPlaceFrame = new CDocObjectFrameHook(pMainFrame, pThis);
			pThis->m_pInPlaceFrame->InternalAddRef();
			*lplpFrame = (LPOLEINPLACEFRAME)pThis->m_pInPlaceFrame->
				GetInterface(&IID_IOleInPlaceFrame);

			// save accel table for IOleInPlaceFrame::TranslateAccelerators
			pThis->m_pInPlaceFrame->m_hAccelTable = lpFrameInfo->haccel;

			// hook IOleInPlaceUIWindow to pDocFrame
			if (pDocFrame != NULL)
			{
				if (pThis->m_pInPlaceDoc == NULL)
					pThis->m_pInPlaceDoc = new CDocObjectFrameHook(pDocFrame, pThis);
				pThis->m_pInPlaceDoc->InternalAddRef();
				*lplpDoc = (LPOLEINPLACEUIWINDOW)pThis->m_pInPlaceDoc->
					GetInterface(&IID_IOleInPlaceUIWindow);
			}
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

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::Scroll(SIZE scrollExtent)
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.Scroll(scrollExtent);
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::OnUIDeactivate(BOOL fUndoable)
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.OnUIDeactivate(fUndoable);
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::OnInPlaceDeactivate()
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.OnInPlaceDeactivate();
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::DiscardUndoState()
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.DiscardUndoState();
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::DeactivateAndUndo()
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.DeactivateAndUndo();
}

STDMETHODIMP CDocObjectContainerItem::XOleInPlaceSite::OnPosRectChange(LPCRECT lpPosRect)
{
	METHOD_PROLOGUE(CDocObjectContainerItem, OleInPlaceSite)
	return pThis->m_xOleIPSite.OnPosRectChange(lpPosRect);
}

/////////////////////////////////////////////////////////////////////////////
// CDocObjectContainerItem diagnostics

#ifdef _DEBUG
void CDocObjectContainerItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CDocObjectContainerItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Implementation of IServiceProvider

IMPLEMENT_ADDREF		(CDocObjectContainerItem, ServiceProvider)
IMPLEMENT_RELEASE		(CDocObjectContainerItem, ServiceProvider)
IMPLEMENT_QUERYINTERFACE(CDocObjectContainerItem, ServiceProvider)

HRESULT CDocObjectContainerItem::XServiceProvider::QueryService(REFGUID guidService, REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(CDocObjectContainerItem, ServiceProvider)

	if(NULL == ppvObj)
		return E_INVALIDARG;

	*ppvObj = NULL;

	return pThis->GetService(guidService, riid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
