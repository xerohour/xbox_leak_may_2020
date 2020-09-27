// AutoWins.cpp : implementation file
//

#include "stdafx.h"
#include <utilauto.h>
#include "autoguid.h"
#include "resource.h"

#include "autostat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implementation of CAutoWindows' dual interface
DELEGATE_GET(CAutoWindows, Windows, Count, long)
DELEGATE_GET(CAutoWindows, Windows, Application, IDispatch*)
DELEGATE_GET(CAutoWindows, Windows, Parent, IDispatch*)

DELEGATE_ONE_PARAM(CAutoWindows, Windows, Arrange, VARIANT)
DELEGATE_ONE_PARAM_WITH_RETVAL(CAutoWindows, Windows, CloseAll, VARIANT, DsSaveStatus)


STDMETHODIMP CAutoWindows::XWindows::get__NewEnum(IUnknown** _NewEnum)
{
	METHOD_PROLOGUE(CAutoWindows, Dispatch)
	
	TRY_DUAL(IID_IWindows)
	{
		*_NewEnum = pThis->_NewEnum();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CAutoWindows::XWindows::Item(VARIANT index, IDispatch* FAR* Item)
{
	METHOD_PROLOGUE(CAutoWindows, Dispatch)
	
	TRY_DUAL(IID_IWindows)
	{
		*Item = pThis->Item(index);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


/////////////////////////////////////////////////////////////////////////////
// CAutoWindows

//IMPLEMENT_DYNCREATE(CAutoWindows, CAutoObj)
IMPLEMENT_DYNAMIC(CAutoWindows, CAutoObj)

CAutoWindows::CAutoWindows(CPartDoc* pOwnerDoc)
{
//	EnableAutomation();
	EnableDualAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
	
	// CAutoWindows has two modes: Either it is owned by a document
	// or it has all MDI windows in it. 
	// If m_pOwnerDoc is NULL, we will work with the entire MDI space.
	// If m_pOwnerDoc is non-NULL, it points to a document whose windows we will use.
	m_pOwnerDoc = pOwnerDoc ;

	// We do not need to do a Sync here, because the first
	// time that someone gets the item will sync.
	m_pPtrlData = NULL ;
}


CAutoWindows::~CAutoWindows()
{
	DeleteCollection() ; //@sync

	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}

//
// Delete the contents of the collection. [@sync]
//
void CAutoWindows::DeleteCollection()
{	
	if (m_pPtrlData != NULL)
	{
		POSITION pos = m_pPtrlData->GetHeadPosition();
		while (pos != NULL)
		{
			IDispatch* pDispatch = m_pPtrlData->GetNext(pos);
			ASSERT (pDispatch != NULL);
			pDispatch->Release() ;
		}

		// The collection is in charge of cleaning this up.
		delete m_pPtrlData ;

		m_pPtrlData = NULL ; 
	}
}

void CAutoWindows::OnFinalRelease()
{

	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CAutoObj::OnFinalRelease();
}

//
// This function is used to keep the internal pointer list in
// sync with the real world.
//
void CAutoWindows::Sync()
{
	// Fill a pointer list with IDispatch* for each document.
	CDispatchList* pDispatchPtrList = new CDispatchList;

	if (m_pOwnerDoc == NULL)
	{
		// This code originated in COpenDocsDlg (opendocs.h|cpp)

		// get a pointer to the main frame
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

		// This code is similar to the Arrange code in the Windows object.

		CPartFrame* pMDIActiveChild = (CPartFrame*)pMainFrame->MDIGetActive();
		if (pMDIActiveChild != NULL)
		{
			CPartFrame* pMDIChild =(CPartFrame *)pMDIActiveChild->GetWindow(GW_HWNDFIRST);
			while( pMDIChild != NULL)
			{
				if(IsValidMDICycleMember(pMDIChild))
				{
					// Get the automation object for this document.
					IDispatch* pDispatch = pMDIChild->GetAutomationObject(TRUE) ;
					if (pDispatch != NULL)
					{
						pDispatchPtrList->AddTail(pDispatch);
					}
				}

				// Get the next window.
				pMDIChild = (CPartFrame *)pMDIChild->GetWindow(GW_HWNDNEXT);
			}
		}
	}
	else
	{
		// Get all of the windows associated with the owner document.
		POSITION pos = m_pOwnerDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			CPartView *pView = static_cast<CPartView*>(m_pOwnerDoc->GetNextView(pos));
			ASSERT(pView);
			ASSERT_KINDOF(CPartView, pView) ;

			if (pView) // defensive code
			{
				CPartFrame* pFrame = static_cast<CPartFrame*>(pView->GetParentFrame());
				ASSERT(pFrame);
				ASSERT_KINDOF(CPartFrame, pFrame) ;
				if (pFrame) // defensive code
				{
					IDispatch* pDispatch = pFrame->GetAutomationObject(TRUE) ;
					if (pDispatch != NULL)
					{
						pDispatchPtrList ->AddTail(pDispatch);
					}
				}
			}
		}
	}

	// If there is already a collection, delete it.
	DeleteCollection() ;

	// Assign the new pointer.
	m_pPtrlData = pDispatchPtrList ;
}



BEGIN_MESSAGE_MAP(CAutoWindows, CAutoObj)
	//{{AFX_MSG_MAP(CAutoWindows)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoWindows, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoWindows)
	DISP_PROPERTY_EX(CAutoWindows, "Count", GetCount, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoWindows, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoWindows, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoWindows, "Item", Item, VT_DISPATCH, VTS_VARIANT)
	DISP_FUNCTION(CAutoWindows, "Arrange", Arrange, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CAutoWindows, "CloseAll", CloseAll, VT_I4, VTS_VARIANT)
	DISP_DEFVALUE(CAutoWindows, "Item")
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoWindows, "_NewEnum", DISPID_NEWENUM, _NewEnum, SetNotSupported, VT_UNKNOWN)
	// Not In Spec DISP_FUNCTION(CAutoWindows, "Add", Add, VT_DISPATCH, VTS_PBSTR)
END_DISPATCH_MAP()

/* REMOVED see DualImpl.doc
// Note: we add support for IID_IWindows to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {5774D191-96E1-11CF-9C00-00A0C90A632C}
static const IID IID_IWindows =
{ 0x5774d191, 0x96e1, 0x11cf, { 0x9c, 0x0, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c } };
*/

BEGIN_INTERFACE_MAP(CAutoWindows, CAutoObj)
	INTERFACE_PART(CAutoWindows, IID_IDispWindows, Dispatch)
	INTERFACE_PART(CAutoWindows, IID_IWindows, Dispatch)
    DUAL_ERRORINFO_PART(CAutoWindows)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CAutoWindows, IID_IWindows)
DS_IMPLEMENT_ENABLE_DUAL(CAutoWindows, Windows)
DS_DELEGATE_DUAL_INTERFACE(CAutoWindows, Windows)

/* See DualImpl.doc
// {5774D192-96E1-11CF-9C00-00A0C90A632C}
IMPLEMENT_OLECREATE(CAutoWindows, "VSHELL7.AUTOWINDOWS", 0x5774d192, 0x96e1, 0x11cf, 0x9c, 0x0, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c)
*/

/////////////////////////////////////////////////////////////////////////////
// CAutoWindows message handlers
long CAutoWindows::GetCount() 
{
	Sync() ;

	return m_pPtrlData->GetCount();
}

LPUNKNOWN CAutoWindows::_NewEnum() 
{
	Sync() ;

	CEnumVariantObjs* pEnumVariant = new CEnumVariantObjs(m_pPtrlData);
	
	if (pEnumVariant == NULL)
		DsThrowCannedOleDispatchException(E_OUTOFMEMORY);

	// We don't QueryInterface, since we don't want the IEnumVARIANT
	//  AddRef'd.  The "new CEnumVariantObjs" above already AddRefs it for
	//  us.
	return &pEnumVariant->m_xEnumVariant;
}


// This function takes the number index or a string 
// representing the caption.
// We decided not to take the Type of the window since this
// wasn't generally useful.
LPDISPATCH CAutoWindows::Item(const VARIANT FAR& vtIndex) 
{
	Sync() ;

	BOOL bHaveValidIndex = FALSE ;
	long lIndex ;

	// Covert the vtIndex to a string representing the caption.
	CString strCaptionWanted;
	CString strDefault("") ;
	HRESULT hr = ::ConvertVariantToCString(vtIndex, strCaptionWanted, strDefault, FALSE ) ;
	if (hr == S_FALSE)
	{
		// Parameter is optional and is not supplied, so use the default.
		// Return a pointer to this collection.
		return GetIDispatch(TRUE) ;
	}
	else if (hr == S_OK)
	{
		CString strCaption ;
		// Iterate through the windows in this collection, and return
		//  the first one we find with this type
		POSITION pos = m_pPtrlData->GetHeadPosition();
		while (pos != NULL)
		{
			LPDISPATCH pDispWnd = m_pPtrlData->GetNext(pos);
			IGenericWindow* pIWindow = NULL;
			VERIFY(SUCCEEDED(pDispWnd->QueryInterface(IID_IGenericWindow, (void**) &pIWindow)));
			if (pIWindow == NULL)
			{
				ASSERT (FALSE);
				continue;
			}
			// Get the caption.
			BSTR bszCaption;
			VERIFY(SUCCEEDED(pIWindow->get_Caption(&bszCaption)));
			
			// Convert to a CString
			strCaption = bszCaption ;

			// Free bstr
			::SysFreeString(bszCaption);

			// remove change indicators (" *") from the captions
			CAutoWindowObj::RemoveChangeIndicatorFromWindowCaption(strCaptionWanted);
			CAutoWindowObj::RemoveChangeIndicatorFromWindowCaption(strCaption);

			// Compare
			if (strCaptionWanted.CompareNoCase(strCaption) == 0)
			{
				// Found a match!
				return pIWindow;		// No need to AddRef, since QueryInterface did that for us
			}

			// Release interface and goto next one.
			pIWindow->Release();
		}
	}

	// Converting to a BSTR failed or the filename didn't exist.
	// So attempt converting to an index.
	hr = ::ConvertVariantToLong(vtIndex, lIndex, 0, FALSE) ;
	if (hr == S_FALSE || lIndex == 0)
	{
		// 0 is an illegal value.
		DsThrowShellOleDispatchException(DS_E_CANNOT_FIND_WINDOW) ;
	}

	// Get item from array
	IDispatch* pObj = NULL ;
    POSITION pos = m_pPtrlData->FindIndex(lIndex-1);
    if (pos != NULL)
	{
        pObj = m_pPtrlData->GetAt(pos);
		ASSERT (pObj != NULL);
	}
	else
	{
		DsThrowShellOleDispatchException(DS_E_CANNOT_FIND_WINDOW) ;
	}
    
	ASSERT (pObj != NULL);
	pObj->AddRef();
	return pObj ;
}

LPDISPATCH CAutoWindows::GetApplication() 
{

	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}



LPDISPATCH CAutoWindows::GetParent() 
{
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}


DsSaveStatus CAutoWindows::CloseAll(const VARIANT FAR& vtLongSaveChanges) 
{

	// Do we close all MDI windows or just those actually in the collection.
	if (m_pOwnerDoc == NULL) 
	{
		// Convert the variant into a valid automation state.
		g_AutomationState.SaveChanges(vtLongSaveChanges);
		
		// get a pointer to the main frame
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

		// Attempt to close all windows
		pMainFrame->OnWindowCloseAll() ;

		// Return if the save was canceled. (HandleError resets status.)
		DsSaveStatus status = g_AutomationState.SavePromptResult() ;		

		// Handle errors caught by the automation state. Cleans up the state.
		g_AutomationState.HandleError() ;

		// Return if the save was canceled.
		return status ;
	}
	else
	{
		Sync() ;

		// Iterate through the windows in this collection, and 
		//  close them.
		POSITION pos = m_pPtrlData->GetHeadPosition();
		while (pos != NULL)
		{
			LPDISPATCH pDispWnd = m_pPtrlData->GetNext(pos);

			// Get the window interface.
			IGenericWindow* pWindow = NULL;
			VERIFY(SUCCEEDED(pDispWnd->QueryInterface(IID_IGenericWindow, (void**) &pWindow)));
			if (pWindow == NULL)
			{
				ASSERT (FALSE);
				continue;
			}
			
			// Close the window.
			DsSaveStatus saveStatus ;
			VERIFY(SUCCEEDED(pWindow->Close(vtLongSaveChanges, &saveStatus))) ;
			pWindow->Release() ;
			if (saveStatus == dsSaveCanceled)
			{
				return dsSaveCanceled ;
			}
		}
		return dsSaveSucceeded ;
	}
} 

//
// This helper function minimizes all MDI child windows.
//
void MinimizeAllMDI()
{ 
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));
	
	CPartFrame* pMDIActiveChild = (CPartFrame*)pMainFrame->MDIGetActive();
	if (pMDIActiveChild != NULL)
	{
		CPartFrame* pMDIChild =(CPartFrame *)pMDIActiveChild->GetWindow(GW_HWNDFIRST);
		while( pMDIChild != NULL)
		{
			//if(!IsValidMDICycleMember(pMDIChild))
			CPartFrame* pTemp = (CPartFrame *)pMDIChild->GetWindow(GW_HWNDNEXT);
			pMDIChild->ShowWindow(SW_MINIMIZE ) ;
			pMDIChild = pTemp ;
		}
	}
}
//
//
//
void CAutoWindows::Arrange(const VARIANT FAR& vtArrangeStyle)
{
	BOOL bDone = FALSE ;
	
	// Convert VARIANT To a long
	const long lDefault = dsTileHorizontal;
	long lIndex ;
	::ConvertVariantToLong(vtArrangeStyle, lIndex, lDefault) ;

	// Catch the bad values.
	switch(lIndex)
	{
	// Iconify all of the windows
	case dsMinimize:
	case dsCascade:
	case dsTileVertical:
	case dsTileHorizontal:
		break ;
	default:
		::DsThrowCannedOleDispatchException(E_INVALIDARG);
		break ;
	}

	// get a pointer to the main frame
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));
	CMDIChildWnd* pMdiChild = pMainFrame->MDIGetActive() ;
	if (pMdiChild == NULL)
	{
		//There is no one to arrange
		return ;
	}
	CWnd* pMdiClient = pMdiChild->GetParent() ;
	ASSERT(pMdiClient != NULL) ;

	if (m_pOwnerDoc != NULL)
	{
		Sync() ;

		// We only want to Arrange the windows inside the collection. 
		// The easiest way to do this is to minimize everybody,
		// restore our windows, then do the function.
		// This applies to everything, but minimization.
		// If we are minimizing, we just minimize the windows and get out.


		// Turn off drawing...

		pMdiClient->SetRedraw(FALSE) ;


		if (lIndex != dsMinimize)
		{
			// Minimize Everybody - only if we are not minimizing
			MinimizeAllMDI() ;
		}

		// Restore our windows.
		POSITION pos = m_pPtrlData->GetHeadPosition();
		while (pos != NULL)
		{
			LPDISPATCH pDispWnd = m_pPtrlData->GetNext(pos);

			// Get the window interface.
			IGenericWindow* pIWindow = NULL;
			VERIFY(SUCCEEDED(pDispWnd->QueryInterface(IID_IGenericWindow, (void**) &pIWindow)));
			if (pIWindow == NULL)
			{
				ASSERT (FALSE);
				continue;
			}

			// Restore the window unless we are minimizing.
			HRESULT hr = pIWindow->put_WindowState((lIndex == dsMinimize) ? dsWindowStateMinimized  : dsWindowStateNormal) ;
			if (FAILED(hr))
			{
				::DsThrowCannedOleDispatchException(hr);
			}
		}

		// If we are minimizing we are done.
		if (lIndex == dsMinimize)
		{
			bDone = TRUE ;
		}
	}

	// Now is the time to actually arrange the windows.
	// If we are working on the entire MDI space, we have not done anything
	// yet. However, if we are only working on the subset of windows in the
	// collection, we have minimized everything else.

	if (!bDone)
	{
		switch(lIndex)
		{
		// Iconify all of the windows
		case dsMinimize:
			ASSERT(m_pOwnerDoc == NULL) ;
			MinimizeAllMDI() ;
			break ;
		// Cascade all of the windows.
		case dsCascade:
			pMainFrame->MDICascade() ;
			break ;

		// Tile the winows vertically
		case dsTileVertical:
			pMainFrame->MDITile(MDITILE_VERTICAL) ;
			break ;

		// Tile the windows horizontally. (This is the default.)
		case dsTileHorizontal:
			pMainFrame->MDITile(MDITILE_HORIZONTAL) ;
			break ;
		default:
			ASSERT(0) ;
		}
	}

	if (m_pOwnerDoc != NULL)
	{
		//Turn Redraw back on and update window.
		pMdiClient->SetRedraw(TRUE) ;

		CRect rectMdiClient ;
		pMdiClient->GetWindowRect(&rectMdiClient) ;
		::InvalidateRect(NULL, rectMdiClient, TRUE) ;
	}
}

/* Not in spec
LPDISPATCH CAutoWindows::Add(BSTR FAR* docType) 
{
	// TODO: Add your dispatch handler code here
	DsThrowCannedOleDispatchException(E_NOTIMPL);

	return NULL;
}
*/
