/////////////////////////////////////////////////////////////////////////////
// dualimpl.cpp

#include "stdafx.h"
#include <afxpriv.h>
#include <utilauto.h>
#include <ObjModel\appdefs.h>
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// This file implements the functions & classes
//  you need to create OLE Automation / dual interface objects that
//  expose functionality FROM Developer Studio packages TO macros
//  and addins.  The code in this file was taken from MFC and the
//  ACDUAL samples, with revisions made.  Functions revised
//  for use in Developer Studio are prefixed with "Ds"
// All declarations of these functions & classes are in
//  include\dualimpl.cpp.
// For examples of usage, see the sample package in
//  dev\ide\pkgs\pkg.  Check out auto*.*.
// For more info, see dualimpl.doc, checked in to the VBA subproject of
//  the V5Spec project.  There's a link to it on
//  http://devstudio/devstudio/automation.


// CONTENTS:
//  CAutoObj:
//		Derive your OLE Automation / dual interface objects from this.
//  CAutoWindowObj:
//		Derive your Window automation objects from this.
//  DsThrowOleDispatchException:
//		Use this instead of AfxThrowOleDispatchException for throwing
//		errors inside your dispatch handlers.
//  CMyOleDispatchImpl:
//		Used in CAutoObj.
//  CMiniOleDispatchDriver:
//		Used in firing dispatch events
//  FireDispatchEvent(V):
//		Functions that use CMiniOleDispatchDriver to fire the dispatch
//		events.  (Firing dual events is taken care of directly in the
//		the macros defined in dualimpl.h.)


////////////////////////////////////////////////////////////////////////
// CAutoObj

IMPLEMENT_DYNCREATE(CAutoObj, CCmdTarget)

CAutoObj::~CAutoObj()
{
	if (m_xDispatch.m_vtbl != 0)
	{
		ExternalDisconnect();
	}
	ASSERT(m_dwRef <= 1);
	m_pModuleState = NULL;

	// Set this to 0 so CCmdTarget's destructor doesn't try
	//  to call nonexistent Disconnect().
	m_xDispatch.m_vtbl = 0;

}

void CAutoObj::ThrowZombifiedException()
{
	DsThrowShellOleDispatchException(DS_E_SHUTDOWN_REQUESTED);
}

////////////////////////////////////////////////////////////////////////
// CAutoWindowObj

IMPLEMENT_DYNCREATE(CAutoWindowObj, CAutoObj)

// Events
/*BEGIN_CONNECTION_MAP(CAutoWindowObj, CAutoObj)
    CONNECTION_PART(CAutoWindowObj, IID_IWindowEvents, WindowCP)
    CONNECTION_PART(CAutoWindowObj, IID_IDispWindowEvents, DispWindowCP)
END_CONNECTION_MAP()*/

CAutoWindowObj::~CAutoWindowObj()
{
}

// RemoveChangeIndicatorFromWindowCaption returns whether " *" was stripped or not
BOOL CAutoWindowObj::RemoveChangeIndicatorFromWindowCaption(CString &strCaption)
{
	int nLength = strCaption.GetLength();

	if (nLength < 3) // if less than 3
		return FALSE; // obviously can't contain a 2-char change indicator!

	static CString strEnd; // to avoid needless construction/destruction overhead

	strEnd = strCaption.Right(2);
	if (strEnd != _T(" *"))
		return FALSE;

	// otherwise, we have a change indicator, so axe it & return TRUE

	strCaption = strCaption.Left(nLength - 2); // remove the trailing *
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// DsDualHandleException

// This was taken from ACDUAL's DualHandleException, but was modified to use
//  the scode instead of the wCode in the exception structure.  This way,
//  the error return is NOT DIFFERENT for dispatch clients and dual vtbl
//  clients.  Note that the LINES sample will change to do things this way,
//  and the ACDUAL sample should be changed, too.

HRESULT DsDualHandleException(REFIID riidSource, const CException* pAnyException)
{
    USES_CONVERSION;

    ASSERT_VALID(pAnyException);

    TRACE0("DsDualHandleException called\n");

    // Set ErrInfo object so that VTLB binding container
    // applications can get rich error information.
    ICreateErrorInfo* pcerrinfo;
    HRESULT hr = CreateErrorInfo(&pcerrinfo);
    if (SUCCEEDED(hr))
    {
	    TCHAR   szDescription[256];
	    LPCTSTR pszDescription = szDescription;
        GUID    guid = GUID_NULL;
        DWORD   dwHelpContext = 0;
        BSTR    bstrHelpFile = NULL;
        BSTR    bstrSource = NULL;
	    if (pAnyException->IsKindOf(RUNTIME_CLASS(COleDispatchException)))
	    {
		    // specific IDispatch style exception
		    COleDispatchException* e = (COleDispatchException*)pAnyException;

            guid = riidSource;

			// If this ASSERT fires, perhaps you used AfxThrowOleDispatchException,
			//  instead of DsThrowOleDispatchException.
			ASSERT (e->m_scError != 0);

		    hr = e->m_scError;
		    pszDescription = e->m_strDescription;
		    dwHelpContext = e->m_dwHelpContext;

		    // propagate source and help file if present
            // call ::SysAllocString directly so no further exceptions are thrown
		    if (!e->m_strHelpFile.IsEmpty())
			    bstrHelpFile = ::SysAllocString(T2COLE(e->m_strHelpFile));
		    if (!e->m_strSource.IsEmpty())
			    bstrSource = ::SysAllocString(T2COLE(e->m_strSource));

	    }
	    else if (pAnyException->IsKindOf(RUNTIME_CLASS(CMemoryException)))
	    {
		    // failed memory allocation
		    AfxLoadString(AFX_IDP_FAILED_MEMORY_ALLOC, szDescription);
		    hr = E_OUTOFMEMORY;
	    }
	    else
	    {
		    // other unknown/uncommon error
		    AfxLoadString(AFX_IDP_INTERNAL_FAILURE, szDescription);
		    hr = E_UNEXPECTED;
	    }

	    if (bstrHelpFile == NULL && dwHelpContext != 0)
		    bstrHelpFile = ::SysAllocString(T2COLE(AfxGetApp()->m_pszHelpFilePath));

	    if (bstrSource == NULL)
		    bstrSource = ::SysAllocString(T2COLE(AfxGetAppName()));

        // Set up ErrInfo object
        pcerrinfo->SetGUID(guid);
	    pcerrinfo->SetDescription(::SysAllocString(T2COLE(pszDescription)));
        pcerrinfo->SetHelpContext(dwHelpContext);
        pcerrinfo->SetHelpFile(bstrHelpFile);
        pcerrinfo->SetSource(bstrSource);

        TRACE("\tSource = %ws\n", bstrSource);
        TRACE("\tDescription = %s\n", pszDescription);
        TRACE("\tHelpContext = %lx\n", dwHelpContext);
        TRACE("\tHelpFile = %ws\n", bstrHelpFile);

        // Set the ErrInfo object for the current thread
        IErrorInfo* perrinfo;
        if (SUCCEEDED(pcerrinfo->QueryInterface(IID_IErrorInfo, (LPVOID*)&perrinfo)))
        {
            SetErrorInfo(0, perrinfo);
            perrinfo->Release();
        }

        pcerrinfo->Release();
    }

    TRACE("DsDualHandleException returning HRESULT %lx\n", hr);
    
    return hr;
}

// This was stolen from MFC's AfxThrowOleDispatchException, but was modified
//  to accept an HRESULT instead of the wCode WORD.
void DsThrowOleDispatchException(HRESULT hr, UINT nDescriptionID, UINT nHelpID)
{
	TCHAR szBuffer[256];
	VERIFY(AfxLoadString(nDescriptionID, szBuffer) != 0);
	if (nHelpID == -1)
		nHelpID = nDescriptionID;

	DsThrowOleDispatchException(hr, szBuffer, nHelpID);
}

void DsThrowOleDispatchException(HRESULT hr, LPCTSTR szPrompt, UINT nHelpID)
{
	// Pass in 0 as the wCode, since we're using the scode field instead
	//  of the wCode, for consistency between the dual HRESULT return,
	//  and the dispatch exception error code.
	COleDispatchException* e = new COleDispatchException(szPrompt, nHelpID, 0);
	e->m_scError = hr;
	THROW(e);
}

// This variation of DsThrowOleDispatchException is for taking shell-defined
// HRESULT's & returning the shell's string

void DsThrowShellOleDispatchException(HRESULT hr, UINT nHelpID)
{
  // do an ugly but fast switch stmt to get string resource ID

  UINT uMsgID;

  switch (hr)
	{
		default:
			{
				ASSERT(FALSE); // you shouldn't have called this function with any other value, dummy!
				return;
			}
		case DS_E_SHUTDOWN_REQUESTED:
			{
				uMsgID = IDS_E_SHUTDOWN_REQUESTED;
				break;
			}
		case DS_E_UNKNOWN:
			{
				uMsgID = IDS_E_UNKNOWN;
				break;
			}

		case DS_E_BAD_PARAM_VALUE:
			{
				uMsgID = IDS_E_BAD_PARAM_VALUE;
				break;
			}

		case DS_E_DOC_RELEASED:
			{
				uMsgID = IDS_E_DOC_RELEASED;
				break;
			}

		case DS_E_WINDOW_RELEASED:
			{
				uMsgID = IDS_E_WINDOW_RELEASED;
				break;
			}
	
		case DS_E_CANNOTCREATENEWWINDOW:
			{
				uMsgID = IDS_E_CANNOTCREATENEWWINDOW;
				break ;
			}

		case DS_E_CANNOT_FIND_WINDOW:
			{
				uMsgID = IDS_E_CANNOT_FIND_WINDOW ;
				break ;
			}
		case DS_E_CANNOT_FIND_DOCUMENT:
			{
				uMsgID = IDS_E_CANNOT_FIND_DOCUMENT;
				break ;
			}

			
		//
		// CFileException File I/O Errors
		//
		case DS_E_FILENOTFOUND:
			{
				uMsgID = IDS_E_FILENOTFOUND ;
				break;
			}
		case DS_E_ENDOFFILE:
			{
				uMsgID = IDS_E_ENDOFFILE ;
				break;
			}
		case DS_E_BADPATH :			//All or part of the path is invalid.
			{
				uMsgID = IDS_E_BADPATH  ;
				break;
			}
		case DS_E_ACCESSDENIED :	//The file could not be accessed.
			{
				uMsgID = IDS_E_ACCESSDENIED ;
				break;
			}
		case DS_E_INVALIDFILE :		//There was an attempt to use an invalid file handle.
			{
				uMsgID = IDS_E_INVALIDFILE ;
				break;
			}
		case DS_E_DISKFULL :		//The disk is full.
			{
				uMsgID = IDS_E_DISKFULL ;
				break;
			}
		case DS_E_SHARINGVIOLATION:
			{
				uMsgID = IDS_E_SHARINGVIOLATION;
				break;
			}
		//
		// Other file errors
		// 
		case DS_E_READONLY:
			{
				uMsgID = IDS_E_READONLY ;
				break ;
			}
		case DS_E_NOFILENAME:
			{
				uMsgID = IDS_E_NOFILENAME ;
				break ;
			}

		//
		// See project.cpp in the bld system for these errors.
		// 
		case DS_E_PROJECT_OLD_MAKEFILE_VC:
			{
				uMsgID = IDS_E_PROJECT_OLD_MAKEFILE_VC ;
				break ;
			}
		case DS_E_PROJECT_OLD_MAKEFILE_DEVSTUDIO:
			{
				uMsgID = IDS_E_PROJECT_OLD_MAKEFILE_DEVSTUDIO;
				break ;
			}
		case DS_E_PROJECT_EXTERNAL_MAKEFILE:
			{
				uMsgID = IDS_E_PROJECT_EXTERNAL_MAKEFILE;
				break ;
			}
		case DS_E_PROJECT_FUTURE_FORMAT:
			{
				uMsgID = IDS_E_PROJECT_FUTURE_FORMAT;
				break ;
			}

	}

	// delegate to regular function
  DsThrowOleDispatchException(hr, uMsgID, nHelpID);
}


// This variation of DsThrowOleDispatchException is for taking system-defined
// HRESULT's & returning the system's string under the current locale

void DsThrowCannedOleDispatchException(HRESULT hr, UINT nHelpID)
{
  LPTSTR szBuffer;

	::FormatMessage( // get a canned system string
		FORMAT_MESSAGE_FROM_SYSTEM |   // go from HRESULT to string
		  FORMAT_MESSAGE_ALLOCATE_BUFFER,  // and allocate a buffer for me
		0, // we're not passing in a message, so this is ignored
		hr, // the HRESULT to decode
		LOCALE_USER_DEFAULT, // REVIEW(CFlaat): should we use the current user locale?
		reinterpret_cast<TCHAR*>(&szBuffer), // yes, this is right -- the system allocates memory for us
		1, // this is a minimum size for our buffer
		0); // irrelevant since we're not passing in a string

	// Pass in 0 as the wCode, since we're using the scode field instead
	//  of the wCode, for consistency between the dual HRESULT return,
	//  and the dispatch exception error code.
	COleDispatchException* e = new COleDispatchException(szBuffer, nHelpID, 0);
	e->m_scError = hr;

  VERIFY(!LocalFree(szBuffer)); // the ! will get optimized away

	THROW(e);
}



////////////////////////////////////////////////////////////////////////
// CMyOleDispatchImpl

// This was stolen & modified from MFC.  The change is that this version
//  of the class receives a pThis pointer on initialization rather than
//  constructing it.  It can't construct it, since this class no longer
//  lives at CCmdTarget::m_xDispatch--the implementation
//  of the dual interface lives there instead.
// Another change is the removal of the virtual Disconnect


STDMETHODIMP_(ULONG) CMyOleDispatchImpl::AddRef()
{
	return m_pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CMyOleDispatchImpl::Release()
{
	return m_pThis->ExternalRelease();
}

STDMETHODIMP CMyOleDispatchImpl::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
	return m_pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CMyOleDispatchImpl::GetTypeInfoCount(UINT* pctinfo)
{
	*pctinfo = m_pThis->GetTypeInfoCount();
	return S_OK;
}

STDMETHODIMP CMyOleDispatchImpl::GetTypeInfo(UINT itinfo, LCID lcid,
	ITypeInfo** pptinfo)
{
	ASSERT_POINTER(pptinfo, LPTYPEINFO);

	if (itinfo != 0)
		return E_INVALIDARG;

	IID iid;
	if (!m_pThis->GetDispatchIID(&iid))
		return E_NOTIMPL;

	return m_pThis->GetTypeInfoOfGuid(lcid, iid, pptinfo);
}

STDMETHODIMP CMyOleDispatchImpl::GetIDsOfNames(
	REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid)
{
	ASSERT_POINTER(rgszNames, char*);
	ASSERT_POINTER(rgdispid, DISPID);

	USES_CONVERSION;

	// check arguments
	if (riid != IID_NULL)
		return DISP_E_UNKNOWNINTERFACE;

	SCODE sc;
	LPTYPEINFO lpTypeInfo = NULL;
	if (lcid != 0 && SUCCEEDED(sc = GetTypeInfo(0, lcid, &lpTypeInfo)))
	{
		// For non-zero lcid, let typeinfo do the work (when available)
		ASSERT(lpTypeInfo != NULL);
		sc = lpTypeInfo->GetIDsOfNames(rgszNames, cNames, rgdispid);
		lpTypeInfo->Release();
		if (sc == TYPE_E_ELEMENTNOTFOUND)
			sc = DISP_E_UNKNOWNNAME;
	}
	else
	{
		// fill in the member name
		const AFX_DISPMAP* pDerivMap = m_pThis->GetDispatchMap();
		rgdispid[0] = m_pThis->MemberIDFromName(pDerivMap, OLE2CT(rgszNames[0]));
		if (rgdispid[0] == DISPID_UNKNOWN)
			sc = DISP_E_UNKNOWNNAME;
		else
			sc = S_OK;

		// argument names are always DISPID_UNKNOWN (for this implementation)
		for (UINT nIndex = 1; nIndex < cNames; nIndex++)
			rgdispid[nIndex] = DISPID_UNKNOWN;
	}

	return sc;
}

STDMETHODIMP CMyOleDispatchImpl::Invoke(
	DISPID dispid, REFIID riid, LCID lcid,
	WORD wFlags, DISPPARAMS* pDispParams, LPVARIANT pvarResult,
	LPEXCEPINFO pexcepinfo, UINT* puArgErr)
{
	ASSERT_NULL_OR_POINTER(pvarResult, VARIANT);
	ASSERT_NULL_OR_POINTER(pexcepinfo, EXCEPINFO);
	ASSERT_NULL_OR_POINTER(puArgErr, UINT);

	// make sure pvarResult is initialized
	if (pvarResult != NULL)
		AfxVariantInit(pvarResult);

	// check arguments
	if (riid != IID_NULL)
		return DISP_E_UNKNOWNINTERFACE;

	// allow subclass to disable Invoke
	if (!m_pThis->IsInvokeAllowed(dispid))
		return E_UNEXPECTED;

	SCODE sc = S_OK;
	const AFX_DISPMAP_ENTRY* pEntry;
	UINT uArgErr = (UINT)-1;    // no error yet
	VARIANT* pvarParamSave = NULL;
	if (theApp.m_pAutoApp == NULL || theApp.m_pAutoApp->IsZombified())
	{
		TRY
		{
			// allow CAutoObj's to disable Invoke w/ specific error message
			m_pThis->ThrowZombifiedException();
		}
		CATCH(COleDispatchException, e)
		{
			if (e->IsKindOf(RUNTIME_CLASS(COleDispatchException)))
			{
				AFX_MANAGE_STATE(m_pThis->m_pModuleState);
				if (pexcepinfo != NULL)
				{
					// fill exception with translation of MFC exception
					COleDispatchException::Process(pexcepinfo, e);
				}
				sc = DISP_E_EXCEPTION;
				goto Cleanup;
			}
		}
		AND_CATCH_ALL(e)
		{
			// Only supposed to throw an OLEDispatchException from ThrowZombifiedException
			ASSERT(FALSE);
			return E_UNEXPECTED;
		}
		END_CATCH_ALL
	}

	// copy param block for safety
	DISPPARAMS params = *pDispParams;
	pDispParams = &params;

	// most of the time, named arguments are not supported
	if (pDispParams->cNamedArgs != 0)
	{
		// only special PROPERTYPUT named argument is allowed
		if (pDispParams->cNamedArgs != 1 ||
			pDispParams->rgdispidNamedArgs[0] != DISPID_PROPERTYPUT)
		{
			return DISP_E_NONAMEDARGS;
		}
	}

	// get entry for the member ID
	pEntry = m_pThis->GetDispEntry(dispid);
	if (pEntry == NULL)
		return DISP_E_MEMBERNOTFOUND;

	// treat member calls on properties just like property get/set
	if ((wFlags == DISPATCH_METHOD) &&
		((pEntry->pfn == NULL && pEntry->pfnSet == NULL) ||
		 (pEntry->pfn == NULL && pEntry->pfnSet != NULL) ||
		 (pEntry->pfn != NULL && pEntry->pfnSet != NULL)))
	{
		// the entry describes a property but a method call is being
		//  attempted -- change it to a property get/set based on the
		//  number of parameters being passed.
		wFlags &= ~DISPATCH_METHOD;
		UINT nExpectedArgs = pEntry->lpszParams != NULL ?
			(UINT)lstrlenA(pEntry->lpszParams) : 0;
		if (pDispParams->cArgs <= nExpectedArgs)
		{
			// no extra param -- so treat as property get
			wFlags |= DISPATCH_PROPERTYGET;
		}
		else
		{
			// extra params -- treat as property set
			wFlags |= DISPATCH_PROPERTYPUTREF;
			pDispParams->cNamedArgs = 1;
		}
	}

	// property puts should not require a return value
	if (wFlags & (DISPATCH_PROPERTYPUTREF|DISPATCH_PROPERTYPUT))
	{
		pvarResult = NULL;
		// catch attempt to do property set on method
		if (pEntry->pfn != NULL && pEntry->pfnSet == NULL)
			return DISP_E_TYPEMISMATCH;
	}



	// handle special cases of DISPATCH_PROPERTYPUT
	VARIANT vaParamSave;
	DISPPARAMS paramsTemp;
	VARIANT vaTemp;
	AfxVariantInit(&vaTemp);

	if (wFlags == DISPATCH_PROPERTYPUT && dispid != DISPID_VALUE)
	{
		// with PROPERTYPUT (no REF), the right hand side may need fixup
		if (pDispParams->rgvarg[0].vt == VT_DISPATCH &&
			pDispParams->rgvarg[0].pdispVal != NULL)
		{
			// remember old value for restore later
			pvarParamSave = &pDispParams->rgvarg[0];
			vaParamSave = pDispParams->rgvarg[0];
			AfxVariantInit(&pDispParams->rgvarg[0]);

			// get default value of right hand side
			memset(&paramsTemp, 0, sizeof(DISPPARAMS));
			sc = vaParamSave.pdispVal->Invoke(
				DISPID_VALUE, riid, lcid, DISPATCH_PROPERTYGET, &paramsTemp,
				&pDispParams->rgvarg[0], pexcepinfo, puArgErr);
		}

		// special handling for PROPERTYPUT (no REF), left hand side
		if (sc == S_OK && pEntry->vt == VT_DISPATCH)
		{
			memset(&paramsTemp, 0, sizeof(DISPPARAMS));

			// parameters are distributed depending on what the Get expects
			if (pEntry->lpszParams == NULL)
			{
				// paramsTemp is already setup for no parameters
				sc = Invoke(dispid, riid, lcid,
					DISPATCH_PROPERTYGET|DISPATCH_METHOD, &paramsTemp,
					&vaTemp, pexcepinfo, puArgErr);
				if (sc == S_OK &&
					(vaTemp.vt != VT_DISPATCH || vaTemp.pdispVal == NULL))
					sc = DISP_E_TYPEMISMATCH;
				else if (sc == S_OK)
				{
					ASSERT(vaTemp.vt == VT_DISPATCH && vaTemp.pdispVal != NULL);
					// we have the result, now call put on the default property
					sc = vaTemp.pdispVal->Invoke(
						DISPID_VALUE, riid, lcid, wFlags, pDispParams,
						pvarResult, pexcepinfo, puArgErr);
				}
			}
			else
			{
				// pass all but named params
				paramsTemp.rgvarg = &pDispParams->rgvarg[1];
				paramsTemp.cArgs = pDispParams->cArgs - 1;
				sc = Invoke(dispid, riid, lcid,
					DISPATCH_PROPERTYGET|DISPATCH_METHOD, &paramsTemp,
					&vaTemp, pexcepinfo, puArgErr);
				if (sc == S_OK &&
					(vaTemp.vt != VT_DISPATCH || vaTemp.pdispVal == NULL))
					sc = DISP_E_TYPEMISMATCH;
				else if (sc == S_OK)
				{
					ASSERT(vaTemp.vt == VT_DISPATCH && vaTemp.pdispVal != NULL);

					// we have the result, now call put on the default property
					paramsTemp = *pDispParams;
					paramsTemp.cArgs = paramsTemp.cNamedArgs;
					sc = vaTemp.pdispVal->Invoke(
						DISPID_VALUE, riid, lcid, wFlags, &paramsTemp,
						pvarResult, pexcepinfo, puArgErr);
				}
			}
			VariantClear(&vaTemp);

			if (sc != DISP_E_MEMBERNOTFOUND)
				goto Cleanup;
		}

		if (sc != S_OK && sc != DISP_E_MEMBERNOTFOUND)
			goto Cleanup;
	}

	// ignore DISP_E_MEMBERNOTFOUND from above
	ASSERT(sc == DISP_E_MEMBERNOTFOUND || sc == S_OK);

	// undo implied default value on right hand side on error
	if (sc != S_OK && pvarParamSave != NULL)
	{
		// default value stuff failed -- so try without default value
		pvarParamSave = NULL;
		VariantClear(&pDispParams->rgvarg[0]);
		pDispParams->rgvarg[0] = vaParamSave;
	}
	sc = S_OK;

	// check arguments against this entry
	UINT nOrigArgs; nOrigArgs = pDispParams->cArgs;
	if (wFlags & (DISPATCH_PROPERTYGET|DISPATCH_METHOD))
	{
		if (!(wFlags & DISPATCH_METHOD))
		{
			if (pEntry->vt == VT_EMPTY)
				return DISP_E_BADPARAMCOUNT;
			if (pvarResult == NULL)
				return DISP_E_PARAMNOTOPTIONAL;
		}
		if (pEntry->lpszParams == NULL && pDispParams->cArgs > 0)
		{
			if (pEntry->vt != VT_DISPATCH)
				return DISP_E_BADPARAMCOUNT;

			// it is VT_DISPATCH property/method but too many arguments supplied
			// transfer those arguments to the default property of the return value
			// after getting the return value from this call.  This is referred
			// to as collection lookup.
			pDispParams->cArgs = 0;
			if (pvarResult == NULL)
				pvarResult = &vaTemp;
		}
	}

	// make sure that parameters are not passed to a simple property
	if (pDispParams->cArgs > 1 &&
		(wFlags & (DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF)) &&
		pEntry->pfn == NULL)
	{
		sc = DISP_E_BADPARAMCOUNT;
		goto Cleanup;
	}

	// make sure that pvarResult is set for simple property get
	if (pEntry->pfn == NULL && pDispParams->cArgs == 0 && pvarResult == NULL)
	{
		sc = DISP_E_PARAMNOTOPTIONAL;
		goto Cleanup;
	}

	// make sure IsExpectingResult returns FALSE as appropriate
	BOOL bResultExpected;
	bResultExpected = m_pThis->m_bResultExpected;
	m_pThis->m_bResultExpected = pvarResult != NULL;

	TRY
	{
		if (pEntry->pfn == NULL)
		{
			// do standard property get/set
			if (pDispParams->cArgs == 0)
				m_pThis->GetStandardProp(pEntry, pvarResult, &uArgErr);
			else
				sc = m_pThis->SetStandardProp(pEntry, pDispParams, &uArgErr);
		}
		else
		{
			// do standard method call
			sc = m_pThis->CallMemberFunc(pEntry, wFlags,
				pvarResult, pDispParams, &uArgErr);
		}
	}
	CATCH(COleException, e)
	{
		sc = e->m_sc;
	}
	AND_CATCH_ALL(e)
	{
		AFX_MANAGE_STATE(m_pThis->m_pModuleState);
		if (pexcepinfo != NULL)
		{
			// fill exception with translation of MFC exception
			COleDispatchException::Process(pexcepinfo, e);
		}
		sc = DISP_E_EXCEPTION;
	}
	END_CATCH_ALL

	// restore original m_bResultExpected flag
	m_pThis->m_bResultExpected = bResultExpected;

	// handle special DISPATCH_PROPERTYGET collection lookup case
	if (sc == S_OK && nOrigArgs > pDispParams->cArgs)
	{
		ASSERT(wFlags & (DISPATCH_PROPERTYGET|DISPATCH_METHOD));
		ASSERT(pvarResult != NULL);
		// must be non-NULL dispatch, otherwise type mismatch
		if (pvarResult->vt != VT_DISPATCH || pvarResult->pdispVal == NULL)
		{
			sc = DISP_E_TYPEMISMATCH;
			goto Cleanup;
		}
		// otherwise, valid VT_DISPATCH was returned
		pDispParams->cArgs = nOrigArgs;
		LPDISPATCH lpTemp = pvarResult->pdispVal;
		if (pvarResult != &vaTemp)
			AfxVariantInit(pvarResult);
		else
			pvarResult = NULL;
		sc = lpTemp->Invoke(DISPID_VALUE, riid, lcid, wFlags,
			pDispParams, pvarResult, pexcepinfo, puArgErr);
		lpTemp->Release();
	}

Cleanup:
	// restore any arguments which were modified
	if (pvarParamSave != NULL)
	{
		VariantClear(&pDispParams->rgvarg[0]);
		pDispParams->rgvarg[0] = vaParamSave;
	}

	// fill error argument if one is available
	if (sc != S_OK && puArgErr != NULL && uArgErr != -1)
		*puArgErr = uArgErr;

	return sc;
}


////////////////////////////////////////////////////////////////////////
// EVENTS, EVENTS, EVENTS!!

// Stolen from MFC's oledisp2.cpp
#if defined(_68K_) || defined(_X86_)
	#define DOUBLE_ARG  _AFX_DOUBLE
#else
	#define DOUBLE_ARG  double
#endif


////////////////////////////////////////////////////////////////////////
// CMiniOleDispatchDriver - toned-down COleDispatchDriver, without the
//  code to convert arguments or deal with disp return values.  Suited
//  for firing potentially cancelable dispatch events.
// We can't reuse MFC's COleDispatchDriver, because it expects MFC type
//  args that it will translate (e.g., LPCTSTR instead of BSTR, or
//  BOOL instead of Boolean).  However, when firing an event to a
//  dual interface sink, it expects some real dispatch types, not these
//  girly-man MFC types.  In order to make firing events possible by
//  calling a single macro which takes only one version of these types,
//  we re-create the COleDispatchDriver to accept the actual dispatch
//  types.
// As a bonus, since we're redo-ing the COleDispatchDriver, we can make
//  its InvokeHelper return whether the event was canceled.  That way,
//  the macros will know to stop calling the sinks as soon as one
//  cancels the event.
// Changes made just for DevStudio are noted in "// DS:" comments

class CMiniOleDispatchDriver
{
// Constructors
public:
	CMiniOleDispatchDriver();

// Attributes
	LPDISPATCH m_lpDispatch;
	BOOL m_bAutoRelease;

// Operations

	void AttachDispatch(LPDISPATCH lpDispatch, BOOL bAutoRelease = TRUE);
	void ReleaseDispatch();
	LPDISPATCH DetachDispatch();

	// returns whether the event was canceled
	BOOL InvokeHelperV(DISPID dwDispID, WORD wFlags,
		const BYTE* pbParamInfo, va_list argList);
};

CMiniOleDispatchDriver::CMiniOleDispatchDriver()
{
	m_lpDispatch = NULL;
	m_bAutoRelease = TRUE;
}

void CMiniOleDispatchDriver::AttachDispatch(LPDISPATCH lpDispatch,
	BOOL bAutoRelease)
{
	ASSERT(lpDispatch != NULL);

	ReleaseDispatch();  // detach previous
	m_lpDispatch = lpDispatch;
	m_bAutoRelease = bAutoRelease;
}

void CMiniOleDispatchDriver::ReleaseDispatch()
{
	if (m_lpDispatch != NULL)
	{
		if (m_bAutoRelease)
			m_lpDispatch->Release();
		m_lpDispatch = NULL;
	}
}

LPDISPATCH CMiniOleDispatchDriver::DetachDispatch()
{
	LPDISPATCH lpDispatch = m_lpDispatch;
	m_lpDispatch = NULL;    // detach without Release
	return lpDispatch;
}

BOOL CMiniOleDispatchDriver::InvokeHelperV(DISPID dwDispID, WORD wFlags,
	const BYTE* pbParamInfo, va_list argList)
{
	USES_CONVERSION;
	VARIANT_BOOL* pBool = NULL;		// DS: Remembers last BOOL* we came across

	if (m_lpDispatch == NULL)
	{
		TRACE0("Warning: attempt to call Invoke with NULL m_lpDispatch!\n");
		return FALSE;
	}

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);

	// determine number of arguments
	if (pbParamInfo != NULL)
		dispparams.cArgs = lstrlenA((LPCSTR)pbParamInfo);

	DISPID dispidNamed = DISPID_PROPERTYPUT;
	if (wFlags & (DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF))
	{
		ASSERT(dispparams.cArgs > 0);
		dispparams.cNamedArgs = 1;
		dispparams.rgdispidNamedArgs = &dispidNamed;
	}

	if (dispparams.cArgs != 0)
	{
		// allocate memory for all VARIANT parameters
		VARIANT* pArg = new VARIANT[dispparams.cArgs];
		ASSERT(pArg != NULL);   // should have thrown exception
		dispparams.rgvarg = pArg;
		memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);

		// get ready to walk vararg list
		const BYTE* pb = pbParamInfo;
		pArg += dispparams.cArgs - 1;   // params go in opposite order

		while (*pb != 0)
		{
			// DS: Last param wasn't the last bool we found
			//  (since there's another param we're about to
			//  investigate).  So reset the bool pointer.
			pBool = NULL;		
			ASSERT(pArg >= dispparams.rgvarg);

			pArg->vt = *pb; // set the variant type
			if (pArg->vt & VT_MFCBYREF)
			{
				pArg->vt &= ~VT_MFCBYREF;
				pArg->vt |= VT_BYREF;
			}
			switch (pArg->vt)
			{
			case VT_I2:
#ifdef _MAC
				pArg->iVal = (short)va_arg(argList, int);
#else
				pArg->iVal = va_arg(argList, short);
#endif
				break;
			case VT_I4:
				pArg->lVal = va_arg(argList, long);
				break;
			case VT_R4:
				// Note: All float arguments to vararg functions are passed
				//  as doubles instead.  Thats why they are passed as VT_R8
				//  instead of VT_R4.
				pArg->vt = VT_R8;
				*(DOUBLE_ARG*)&pArg->dblVal = va_arg(argList, DOUBLE_ARG);
				break;
			case VT_R8:
				*(DOUBLE_ARG*)&pArg->dblVal = va_arg(argList, DOUBLE_ARG);
				break;
			case VT_DATE:
				*(DOUBLE_ARG*)&pArg->date = va_arg(argList, DOUBLE_ARG);
				break;
			case VT_CY:
				pArg->cyVal = *va_arg(argList, CY*);
				break;
#if !defined(_UNICODE) && !defined(OLE2ANSI)
			case VT_BSTRA:
				// DS: Unlike MFC, we'll never expect an ANSI string, so fix up
				//  the vt field to be a normal VT_BSTR
				pArg->vt = VT_BSTR;
				// Fall through to VT_BSTR case...
#endif
			case VT_BSTR:
				// DS: Unlike MFC, we already expect a BSTR, so no copying.
				pArg->bstrVal = va_arg(argList, BSTR);
				break;
			case VT_DISPATCH:
				pArg->pdispVal = va_arg(argList, LPDISPATCH);
				break;
			case VT_ERROR:
				pArg->scode = va_arg(argList, SCODE);
				break;
			case VT_BOOL:
				// DS: Unlike MFC, we already expect a VARIANT_BOOL, not BOOL
				V_BOOL(pArg) = va_arg(argList, VARIANT_BOOL);
				break;
			case VT_VARIANT:
				*pArg = *va_arg(argList, VARIANT*);
				break;
			case VT_UNKNOWN:
				pArg->punkVal = va_arg(argList, LPUNKNOWN);
				break;

			case VT_I2|VT_BYREF:
				pArg->piVal = va_arg(argList, short*);
				break;
			case VT_I4|VT_BYREF:
				pArg->plVal = va_arg(argList, long*);
				break;
			case VT_R4|VT_BYREF:
				pArg->pfltVal = va_arg(argList, float*);
				break;
			case VT_R8|VT_BYREF:
				pArg->pdblVal = va_arg(argList, double*);
				break;
			case VT_DATE|VT_BYREF:
				pArg->pdate = va_arg(argList, DATE*);
				break;
			case VT_CY|VT_BYREF:
				pArg->pcyVal = va_arg(argList, CY*);
				break;
			case VT_BSTR|VT_BYREF:
				pArg->pbstrVal = va_arg(argList, BSTR*);
				break;
			case VT_DISPATCH|VT_BYREF:
				pArg->ppdispVal = va_arg(argList, LPDISPATCH*);
				break;
			case VT_ERROR|VT_BYREF:
				pArg->pscode = va_arg(argList, SCODE*);
				break;
			case VT_BOOL|VT_BYREF:
				// DS: Unlike MFC, we already expect a VARIANT_BOOL*, not BOOL*
				// Remember this in pBool in case it's the last parameter.  If we're
				//  firing a cancelable event, this is where the sink
				//  will tell us whether it was canceled
				pBool = pArg->pboolVal = va_arg(argList, VARIANT_BOOL*);
				break;
			case VT_VARIANT|VT_BYREF:
				pArg->pvarVal = va_arg(argList, VARIANT*);
				break;
			case VT_UNKNOWN|VT_BYREF:
				pArg->ppunkVal = va_arg(argList, LPUNKNOWN*);
				break;

			default:
				ASSERT(FALSE);  // unknown type!
				break;
			}

			--pArg; // get ready to fill next argument
			++pb;
		}
	}

	// initialize return value
	VARIANT* pvarResult = NULL;
	VARIANT vaResult;
	AfxVariantInit(&vaResult);

	// initialize EXCEPINFO struct
	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);

	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	// make the call
	SCODE sc = m_lpDispatch->Invoke(dwDispID, IID_NULL, 0, wFlags,
		&dispparams, pvarResult, &excepInfo, &nArgErr);

	// DS: Unlike MFC, we don't do any cleanup of the variants, since
	//  we didn't copy any LPCTSTRs into new BSTRs.
	delete[] dispparams.rgvarg;

	// throw exception on failure
	if (FAILED(sc))
	{
		VariantClear(&vaResult);
		if (sc != DISP_E_EXCEPTION)
		{
			// non-exception error code
			AfxThrowOleException(sc);
		}

		// make sure excepInfo is filled in
		if (excepInfo.pfnDeferredFillIn != NULL)
			excepInfo.pfnDeferredFillIn(&excepInfo);

		// allocate new exception, and fill it
		COleDispatchException* pException =
			new COleDispatchException(NULL, 0, excepInfo.wCode);
		ASSERT(pException->m_wCode == excepInfo.wCode);
		if (excepInfo.bstrSource != NULL)
		{
			pException->m_strSource = excepInfo.bstrSource;
			SysFreeString(excepInfo.bstrSource);
		}
		if (excepInfo.bstrDescription != NULL)
		{
			pException->m_strDescription = excepInfo.bstrDescription;
			SysFreeString(excepInfo.bstrDescription);
		}
		if (excepInfo.bstrHelpFile != NULL)
		{
			pException->m_strHelpFile = excepInfo.bstrHelpFile;
			SysFreeString(excepInfo.bstrHelpFile);
		}
		pException->m_dwHelpContext = excepInfo.dwHelpContext;
		pException->m_scError = excepInfo.scode;

		// then throw the exception
		THROW(pException);
		ASSERT(FALSE);  // not reached
	}

	if (pBool != NULL)
		// DS: This is a cancelable event, so return whether it was canceled
		return (*pBool != 0);

	// DS: This is not a cancelable event, so just return FALSE (i.e., not canceled)
	return FALSE;
}


////////////////////////////////////////////////////////////////////////
// Dispatch event-firing functions

// This does the actual firing of the dispatch event.  Returns whether
//  the action was canceled.
BOOL FireDispatchEventV(BOOL bCancelable, CConnectionPoint* pCP,
	DISPID dispid, BYTE* pbParams, va_list argList)
{
	// Cancelable events are not supported until they can be represented
	//  in the event queue (see utilauto.h, definition of CApplication)
	ASSERT (!bCancelable);

	CMiniOleDispatchDriver driver;
	BOOL bWasCanceled = FALSE;

	const CPtrArray* pConnections = pCP->GetConnections();
	ASSERT(pConnections != NULL);
	CPtrArray ConnectionsCopy;
	ConnectionsCopy.Copy(*pConnections);

	int i;
	int cConnections = ConnectionsCopy.GetSize();
	LPDISPATCH pDispatch;
	for(i = 0; i < cConnections; i++)
	{
		pDispatch = (LPDISPATCH)(ConnectionsCopy.GetAt(i));
		ASSERT(pDispatch != NULL);
		pDispatch->AddRef();
	}

	for (i = 0; i < cConnections; i++)
	{
		pDispatch = (LPDISPATCH)(ConnectionsCopy.GetAt(i));
		ASSERT(pDispatch != NULL);
		driver.AttachDispatch(pDispatch, FALSE);
		TRY
			bWasCanceled = driver.InvokeHelperV(dispid, 
				DISPATCH_METHOD, pbParams, argList);
		END_TRY

		driver.DetachDispatch();

		// If this is a cancelable event & was canceled, stop firing!
		if (bCancelable && bWasCanceled)
			break;
	}
	for(i = 0; i < cConnections; i++)
	{
		pDispatch = (LPDISPATCH)(ConnectionsCopy.GetAt(i));
		ASSERT(pDispatch != NULL);
		pDispatch->Release();
	}
	return (bCancelable && bWasCanceled);
}

// This prepares the varargs and calls FireDispatchEventV directly.  It
//  is this function that's actually called by the macros.
BOOL FireDispatchEvent(BOOL bCancelable, CConnectionPoint* pCP,
	DISPID dispid, BYTE* pbParams, ...)
{
	va_list argList;
	va_start(argList, pbParams);
	BOOL bCanceled =
		FireDispatchEventV(bCancelable, pCP, dispid, pbParams, argList);
	va_end(argList);
	return bCanceled;
}

// Event queue functions

// Private queueing function called by the other EventQueueAdd.  This adds an
//  allocated CEventEntry to the event queue.  If the queue fills up, it
//  spills everything out.
void CApplication::EventQueueAdd(CEventEntry* pEventEntry)
{
	m_pEventQueue[m_nEventQueueTail] = pEventEntry;
	m_nEventQueueTail = (m_nEventQueueTail+1) % MAX_EVENT_QUEUE_ENTRIES;
	if ( (m_nEventQueueTail+1) % MAX_EVENT_QUEUE_ENTRIES == m_nEventQueueHead)
	{
		// Queue has filled up.  Most likely a naughty add-in or macro
		//  has screwed up calls to EnableModeless so that we think
		//  we're supposed to be disabled when we're actually enabled.
		//  To fix this, we'll
		//		(1) Fix the EnableModeless situation
		//		(2) Empty out the queue
		// (Note that another way we can get here is if too many events
		//  have been added which are fired in response to posted messages,
		//  and our queue is simply too small to hold them.  If that's true,
		//  we'll need to make MAX_EVENT_QUEUE_ENTRIES bigger.)

		while (m_nEnableModelessLevels != 0)
		{
			// If a naughty macro or add-in doesn't re-enable us enough
			//  times (i.e., not calling EnableModeless(TRUE) for each
			//  EnableModeless(FALSE)), we'll do it ourselves here
			EnableModeless(TRUE);
		}

		EventQueueRemoveAll();
	}
}

// NOTE!  The caller must delete the returned CEventEntry*
CEventEntry* CApplication::EventQueueRemove()
{
	if (m_nEventQueueHead == m_nEventQueueTail)
		return NULL;		// already empty

	CEventEntry* pEventEntry = m_pEventQueue[m_nEventQueueHead];
	m_pEventQueue[m_nEventQueueHead] = NULL;
	m_nEventQueueHead = (m_nEventQueueHead+1) % MAX_EVENT_QUEUE_ENTRIES;
	return pEventEntry;
}

// Dequeues all events and fires them in turn.  This is called after all
//  message boxes (that were displayed by a macro or add-in) are dismissed.
//  It's also called to spill out an overflowing queue (see EventQueueAdd above)
void CApplication::EventQueueRemoveAll()
{
	if (m_bEventQueueEmptying)
		return;
	m_bEventQueueEmptying = TRUE;

	CEventEntry* pEvent;
	while ((pEvent = EventQueueRemove()) != NULL)
	{
		// Fire event stored in pEvent
		pEvent->FireDispatchEvent();
		pEvent->FireDualEvent();
		pEvent->DestroyArgs();

		// Destroy
		delete pEvent;
	}

	m_bEventQueueEmptying = FALSE;
}

void CApplication::EventQueueInit()
{
	m_bEventQueueEmptying = FALSE;
	m_nEventQueueHead = 0;
	m_nEventQueueTail = 0;

	for (int i=0; i < MAX_EVENT_QUEUE_ENTRIES; i++)
		m_pEventQueue[i] = NULL;
}

void CApplication::EventQueueDestroy()
{
	for (int i=0; i < MAX_EVENT_QUEUE_ENTRIES; i++)
	{
		if (m_pEventQueue[i] != NULL)
		{
			delete m_pEventQueue[i];
			m_pEventQueue[i] = NULL;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//
// Variant Convertions helpers.
//
//

ConvertVariantToCString(/*in*/	const VARIANT &vInput, 
					   /*out*/	CString& tReturnValue, 
					   /*in*/	CString tDefaultValue, 
					   /*in*/	BOOL bThrowException /*= TRUE*/)
{
	if ( ((vInput.vt == VT_ERROR) && (vInput.scode == DISP_E_PARAMNOTFOUND))
		|| (vInput.vt == VT_EMPTY))
	{
		tReturnValue = tDefaultValue; // lDefault is our default value
		return S_FALSE;
	}

	VARIANT vTemp;
	::VariantInit(&vTemp);

	HRESULT hr = ::VariantChangeType(&vTemp, const_cast<VARIANT*>(&vInput), 0, VT_BSTR) ;
	if (FAILED(hr))
	{	
	// they gave us an unusable type, so trigger an error		
		if (bThrowException)
		{
			::DsThrowCannedOleDispatchException(hr);
		}
		else
		{
			return E_FAIL ;
		}
	}

	tReturnValue = vTemp.bstrVal;
	if (tReturnValue.IsEmpty())
	{
		tReturnValue = tDefaultValue; 
		return S_FALSE;
	}
 	return S_OK;
}


ConvertVariantToLong(/*in*/	const VARIANT &vInput, 
					   /*out*/	long& tReturnValue, 
					   /*in*/	long tDefaultValue, 
					   /*in*/	BOOL bThrowException /*= TRUE*/)
{
   	if (((vInput.vt == VT_ERROR) && (vInput.scode == DISP_E_PARAMNOTFOUND)) // if param omitted
		|| (vInput.vt == VT_EMPTY))
	{
		tReturnValue = tDefaultValue; // lDefault is our default value
		return S_FALSE;
	}

	VARIANT vTemp;
	::VariantInit(&vTemp);

	HRESULT hr = ::VariantChangeType(&vTemp, const_cast<VARIANT*>(&vInput), 0, VT_I4) ;
	if (FAILED(hr))
	{	
	// they gave us an unusable type, so trigger an error		
		if (bThrowException)
		{ 
			::DsThrowCannedOleDispatchException(hr);
		}
		else
		{
			return E_FAIL ;
		}
	}

	tReturnValue = vTemp.lVal;
 	return S_OK;
}



ConvertVariantToBOOL(/*in*/	const VARIANT &vInput, 
					   /*out*/	BOOL& tReturnValue, 
					   /*in*/	BOOL tDefaultValue, 
					   /*in*/	BOOL bThrowException /*= TRUE*/)
{
	if ( ((vInput.vt == VT_ERROR) && (vInput.scode == DISP_E_PARAMNOTFOUND)) // if param omitted
		|| (vInput.vt == VT_EMPTY))

	{
		tReturnValue = tDefaultValue; // lDefault is our default value
		return S_FALSE;
	}

	VARIANT vTemp;
	::VariantInit(&vTemp);

	HRESULT hr = ::VariantChangeType(&vTemp, const_cast<VARIANT*>(&vInput), 0, VT_BOOL) ;
	if (FAILED(hr))
	{	
	// they gave us an unusable type, so trigger an error		
		if (bThrowException)
		{
			::DsThrowCannedOleDispatchException(hr);
		}
		else
		{
			return E_FAIL ;
		}
	}

	tReturnValue = (vTemp.boolVal == VARIANT_TRUE) ? TRUE : FALSE ;
 	return S_OK;
}


// this fn is used by quoted strings for recording
void CQuotedStringArg::GetQuotedBasicString(CString &strOut, LPCTSTR szData)
{
	CString strTemp(szData);
	int i;

	static TCHAR chQuote = _T('\"');
	strOut = chQuote; // our opening quote

	while (1)
	{
		i = strTemp.Find(chQuote);

		if (-1 == i) // if no embedded double quotes
		{
			strOut += strTemp; // grab the rest of the string

#ifdef _DEBUG
			CString strData(szData);
			int iOutLen = strOut.GetLength() - 1; // to account for the opening quote
			ASSERT(strData.GetLength() <= iOutLen); // can't be smaller
			int iMaxLen = 1 + strData.GetLength() * 2;
			ASSERT(iOutLen <= iMaxLen); // can't be more than twice as large
#endif

			strOut += chQuote;
			return; // and return
		}

		// otherwise, i is the _byte_ (!) index into our character string

		strOut += strTemp.Left(i); // grab the characters before the quote
		strOut += _T("\"\""); // append two double quotes
		strTemp = strTemp.Mid(i + 1); // discard the quote & the characters before it
		// and repeat the find operation
	}
}

