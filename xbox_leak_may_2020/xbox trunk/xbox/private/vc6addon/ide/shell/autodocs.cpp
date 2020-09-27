// AutoDocs.cpp : implementation file
//

#include "stdafx.h"

#include <utilauto.h>
#include "autoguid.h"
#include "AutoDocs.h"
#include "resource.h"

#include "autostat.h" // g_AutomationState - Internal automation code.


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Implementation of CAutoDocuments' dual interface

DELEGATE_GET(CAutoDocuments, Documents, Count, long)
DELEGATE_GET(CAutoDocuments, Documents, Application, IDispatch*)
DELEGATE_GET(CAutoDocuments, Documents, Parent, IDispatch*)

DELEGATE_ONE_PARAM_WITH_RETVAL(CAutoDocuments, Documents, SaveAll,  VARIANT, DsSaveStatus)
DELEGATE_ONE_PARAM_WITH_RETVAL(CAutoDocuments, Documents, CloseAll, VARIANT, DsSaveStatus)

STDMETHODIMP CAutoDocuments::XDocuments::Add(BSTR docType, VARIANT vtReserved, IDispatch ** ppDocument)
{
	METHOD_PROLOGUE(CAutoDocuments, Dispatch)
	
	TRY_DUAL(IID_IDocuments)
	{
		CString strDocType(docType);
		*ppDocument= pThis->Add(strDocType,vtReserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL

}


STDMETHODIMP CAutoDocuments::XDocuments::get__NewEnum(IUnknown** _NewEnum)
{
	METHOD_PROLOGUE(CAutoDocuments, Dispatch)
	
	TRY_DUAL(IID_IDocuments)
	{
		*_NewEnum = pThis->_NewEnum();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CAutoDocuments::XDocuments::Item(VARIANT index, IDispatch* FAR* Item)
{
	METHOD_PROLOGUE(CAutoDocuments, Dispatch)
	
	TRY_DUAL(IID_IDocuments)
	{
		*Item = pThis->Item(index);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CAutoDocuments::XDocuments::Open(	BSTR filename, 
												VARIANT vtDocType, 
												VARIANT vtBoolReadOnly, 
												IDispatch** ppDocument) 
{
	METHOD_PROLOGUE(CAutoDocuments, Dispatch)
	
	TRY_DUAL(IID_IDocuments)
	{
		CString strFilename(filename);
		*ppDocument = pThis->Open(strFilename, vtDocType, vtBoolReadOnly);
		return NOERROR;
	}
	CATCH_ALL_DUAL

}




/////////////////////////////////////////////////////////////////////////////
// CAutoDocuments

//IMPLEMENT_DYNCREATE(CAutoDocuments, CAutoObj)
IMPLEMENT_DYNAMIC(CAutoDocuments, CAutoObj)

CAutoDocuments::CAutoDocuments() 
{
	EnableDualAutomation();
//	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();

	// We do not need to call Sync here, because the first
	// time that someone gets the item we will Sync.
	m_pPtrlData = NULL ;
}

CAutoDocuments::~CAutoDocuments()
{
	DeleteCollection() ; //[@sync]

	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


//
// Delete the contents of the collection. [@sync]
//
void CAutoDocuments::DeleteCollection()
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

void CAutoDocuments::OnFinalRelease()
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
void CAutoDocuments::Sync()
{
	// Fill a pointer list with IDispatch* for each document.
	CDispatchList* pDispatchPtrList = new CDispatchList;

	// Iterate through the Doc Templates.
	POSITION posTemplate = theApp.GetFirstDocTemplatePosition() ;
	while (posTemplate != NULL)
	{
		CDocTemplate* pTemplate = theApp.GetNextDocTemplate(posTemplate);
		ASSERT(pTemplate != NULL) ;

		// Iterate through templates for the current Docs 
		POSITION posDoc = pTemplate->GetFirstDocPosition() ;
		while ( posDoc != NULL)
		{
			CPartDoc* pPartDoc = (CPartDoc*)pTemplate->GetNextDoc(posDoc) ;
			ASSERT(pPartDoc->IsKindOf(RUNTIME_CLASS(CPartDoc))) ;

			// Get the automation object for this document.
			IDispatch* pDispatch = pPartDoc->GetAutomationObject(TRUE) ;
			if (pDispatch != NULL)
			{
				pDispatchPtrList->AddTail(pDispatch);
			}
		}
	}

	// If there is already a collection, delete it.
	DeleteCollection() ;

	// Assign the new pointer.
	m_pPtrlData = pDispatchPtrList ;
}


BEGIN_MESSAGE_MAP(CAutoDocuments, CAutoObj)
	//{{AFX_MSG_MAP(CAutoDocuments)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoDocuments, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoDocuments)
	DISP_PROPERTY_EX(CAutoDocuments, "Count", GetCount, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoDocuments, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDocuments, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoDocuments, "Item", Item, VT_DISPATCH, VTS_VARIANT)
	DISP_FUNCTION(CAutoDocuments, "SaveAll", SaveAll, VT_I4, VTS_VARIANT)
	DISP_FUNCTION(CAutoDocuments, "CloseAll", CloseAll, VT_I4, VTS_VARIANT)
	DISP_FUNCTION(CAutoDocuments, "Add", Add, VT_DISPATCH, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CAutoDocuments, "Open", Open, VT_DISPATCH, VTS_BSTR VTS_VARIANT VTS_VARIANT)
	DISP_DEFVALUE(CAutoDocuments, "Item")
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoDocuments, "_NewEnum", DISPID_NEWENUM, _NewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

/* REMOVE per DualImpl.Doc
// Note: we add support for IID_IAutoDocuments to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {E9EB1AE1-89A8-11CF-9BE8-00A0C90A632C}
static const IID IID_IAutoDocuments =
{ 0xe9eb1ae1, 0x89a8, 0x11cf, { 0x9b, 0xe8, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c } };

*/

BEGIN_INTERFACE_MAP(CAutoDocuments, CAutoObj)
	INTERFACE_PART(CAutoDocuments, IID_IDispDocuments, Dispatch)
	INTERFACE_PART(CAutoDocuments, IID_IDocuments, Dispatch)
    DUAL_ERRORINFO_PART(CAutoDocuments)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CAutoDocuments, IID_IDocuments)
DS_IMPLEMENT_ENABLE_DUAL(CAutoDocuments, Documents)
DS_DELEGATE_DUAL_INTERFACE(CAutoDocuments, Documents)


// {E9EB1AE2-89A8-11CF-9BE8-00A0C90A632C}
// Removed See DualImpl.doc | IMPLEMENT_OLECREATE(CAutoDocuments, "SHELL.AUTODOCUMENTS", 0xe9eb1ae2, 0x89a8, 0x11cf, 0x9b, 0xe8, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c)

/////////////////////////////////////////////////////////////////////////////
// CAutoDocuments message handlers

long CAutoDocuments::GetCount() 
{
	Sync() ;
	return m_pPtrlData->GetCount();
}

LPUNKNOWN CAutoDocuments::_NewEnum() 
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

LPDISPATCH CAutoDocuments::Item(const VARIANT FAR& vtIndex) 
{
	Sync() ;

	BOOL bFoundDoc = FALSE ;
	IDispatch* pIDispatch;
	
	IDispatch* pDispatchNoPath = NULL ; // If we compare two filenames and they make by name but not fullname, we set this.

	// Covert the index to a filename.
	CString strFilename;
	CString strDefault("") ;
	HRESULT hr = ::ConvertVariantToCString(vtIndex, strFilename, strDefault, FALSE ) ;
	if (hr == S_FALSE)
	{
		// Parameter is optional and is not supplied, so use the default.
		// Return a pointer to this collection.
		return GetIDispatch(TRUE) ;
	}
	else if (hr == S_OK)
	{

		// We have a filename. There are several possibilities:
		// 1. A fullpath.
		// 2. Just a fileName.
		// 3. A relative path.
		
		// Convert the filename/relative filename into a full pathname.
 		CPath pathname;
		pathname.Create(strFilename); 

		// Get just the filename part.
		CString strName(pathname.GetFileName()) ;

		// Set up for the loop
		IGenericDocument* pIDocument;

		// Cycle through the 
		POSITION pos = m_pPtrlData->GetHeadPosition();
		while ( (pos != NULL) && (bFoundDoc == FALSE))
		{
			// Get a pointer to one of our documents.
			IDispatch* pPosDispatch = m_pPtrlData->GetNext(pos);
			ASSERT (pPosDispatch != NULL);

			// Ask the document for its IDocument interface.			 
			HRESULT hr = pPosDispatch->QueryInterface(IID_IGenericDocument, (void**)&pIDocument) ; 
			ASSERT(SUCCEEDED(hr)) ;

			// Ask for the fullname!
			BSTR bstrPosFullname = SysAllocStringLen(NULL, 256) ;
			hr = pIDocument->get_FullName(&bstrPosFullname);
			ASSERT(SUCCEEDED(hr)) ;

			// 
			if (::SysStringLen(bstrPosFullname) == 0)
			{
				// If there is no title, go to next document.
				// Release the string.
				::SysFreeString(bstrPosFullname) ;
				pIDocument->Release();
				continue ;
			}

			// Put it into a CString.
			CString strPosFullname ;
			strPosFullname = bstrPosFullname ;
			
			// Release the string.
			::SysFreeString(bstrPosFullname) ;
	
			// Release the pointer.
			pIDocument->Release() ;

			// Put it into a path...
			CPath PosPathname;
			PosPathname.Create(strPosFullname); 

			// Are they equal?
			if (pathname == PosPathname)
			{
				// We have found our document.
				pIDispatch = pPosDispatch ;
				bFoundDoc = TRUE;
			}
			else if (pDispatchNoPath == NULL)
			{
				// The two documents do not make by Fullname. However, they may
				// match by name alone. If they do match by name, set pDispatchNoPath.
				// However, keep looking for an exact match.
				if (strName.CompareNoCase(PosPathname.GetFileName()) == 0)
				{
					pDispatchNoPath = pPosDispatch ;
				}
			}			
		}
	}
	if (pDispatchNoPath && !bFoundDoc)
	{
		// So, the fullname didn't match, but a filename did.
		pIDispatch = pDispatchNoPath ;
		bFoundDoc = TRUE;
	}

	if (!bFoundDoc)
	{
		// Converting to a BSTR failed or the filename didn't exist.

		// So attempt converting to an index.
		long lIndex ;
		hr = ::ConvertVariantToLong(vtIndex, lIndex, 0, FALSE) ;
		if ( hr == S_FALSE || lIndex == 0)
		{
			// 0 is an illegal value.
			DsThrowShellOleDispatchException(DS_E_CANNOT_FIND_DOCUMENT) ;
		}


	    // Find the IDispatch pointer in the array.
		POSITION pos = m_pPtrlData->FindIndex(lIndex - 1);
		if (pos == NULL)
		{
			DsThrowShellOleDispatchException(DS_E_CANNOT_FIND_DOCUMENT) ;
		}
		// Get item from array
		pIDispatch = m_pPtrlData->GetAt(pos);
		ASSERT (pIDispatch != NULL);

		bFoundDoc = TRUE ;
	}

	pIDispatch->AddRef();
	return pIDispatch;
}

LPDISPATCH CAutoDocuments::GetApplication() 
{
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}



LPDISPATCH CAutoDocuments::GetParent() 
{
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}


DsSaveStatus CAutoDocuments::SaveAll(const VARIANT FAR& vtBoolPrompt) 
{
	// Set up the automation state.
	g_AutomationState.SetPromptState(vtBoolPrompt) ;

	// Save all of the documents.
	BOOL bResult = theApp.SaveAll(FALSE, FILTER_DEBUG);

	// Return if the save was canceled. (HandleError resets status.)
	DsSaveStatus status = g_AutomationState.SavePromptResult() ;		
	
	// Handle errors caught by the automation state. Cleans up the state.
	g_AutomationState.HandleError() ;
	
	// Handle errors not placed in the automation state.
	if (!bResult && status == dsSaveSucceeded)
	{
		// No error, but no document.
		TRACE0("CAutoDocuments::SaveAll - No Error reported.") ;
		DsThrowShellOleDispatchException(DS_E_UNKNOWN) ;
	}

	return status ;

}

DsSaveStatus CAutoDocuments::CloseAll(const VARIANT FAR& vtLongSaveChanges) 
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

//
// The Add method creates a new file and adds it to the current
// Documents collection.
//
LPDISPATCH CAutoDocuments::Add(LPCTSTR docType, const VARIANT & vtReserved) 
{
	CDocTemplate* pTemplate = theApp.GetTemplate(docType);
	if (pTemplate == NULL)
	{
		DsThrowCannedOleDispatchException(E_INVALIDARG);
	}

	// Turn UI off.
	g_AutomationState.Reset() ;
	g_AutomationState.DisableUI();

	// Attempt to create the document.
	CDocument* pDoc = pTemplate->OpenDocumentFile(NULL);	

	// Handle errors caught by the automation state. Cleans up the state.
	g_AutomationState.HandleError() ;

	// Handle silient errors.
	if (pDoc == NULL)
	{
		// No error, but no document.
		TRACE0("CAutoDocuments::Add - No Error reported.") ;
		DsThrowShellOleDispatchException(DS_E_UNKNOWN) ;
	}
	
	// Get the automation object for this doc.  AddRef
	//  once to put it in our list
	ASSERT_KINDOF(CPartDoc, pDoc);
	LPDISPATCH pDispDoc = ((CPartDoc*)pDoc)->GetAutomationObject(TRUE);

	return pDispDoc;
}

//
// The Open member function Opens a file and adds it to the currenct
// Documents collection.
//
LPDISPATCH CAutoDocuments::Open(LPCTSTR filename,			// Filename of file to open.
								const VARIANT FAR& vstrDocType, // String representing the type of document to open.
								const VARIANT FAR& vboolReadonly) // File is opened readonly if True.
{
	// Covert the vstrDoctype to a string.
	CString doctype ;
	CString strDefault("Auto") ;
	ConvertVariantToCString(vstrDocType, doctype, strDefault) ;

	// Covert the varient vboolReadonly to a real bool.
	BOOL readonly;
	::ConvertVariantToBOOL(vboolReadonly, readonly, FALSE) ;

	CDocTemplate* pTemplate = NULL ;
	if (doctype.CompareNoCase(_T("Auto")) != 0)
	{
		// Only get the template if the doctype is NOT "auto".
	 	pTemplate = theApp.GetTemplate(doctype);
		if (pTemplate == NULL)
		{
			DsThrowCannedOleDispatchException(E_INVALIDARG);
		}
	}

	//
	// The start of the real work.
	//
	// Turn UI off.
	g_AutomationState.Reset() ;
	g_AutomationState.DisableUI();

	// We do not use OpenDocumentFile, because pTemplate may be NULL if
	// the user specifies "Auto" for docType.
	CDocument* pDoc = theApp.OpenFile(pTemplate, filename, readonly) ;

	// Handle errors caught by the automation state. Cleans up the state.
	g_AutomationState.HandleError() ;

	if (pDoc == NULL)
	{
		// No error, but no document.
		TRACE0("CAutoDocuments::Open - No Error reported.") ;
		DsThrowShellOleDispatchException(DS_E_UNKNOWN); 
	}

	// Get the automation object for this doc.  AddRef
	//  once to put it in our list
	ASSERT_KINDOF(CPartDoc, pDoc);
	LPDISPATCH pDispDoc = ((CPartDoc*)pDoc)->GetAutomationObject(TRUE);

	return pDispDoc;
}


