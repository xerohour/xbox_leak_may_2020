//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctportdl.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctportdl.cpp
//
// Test harness implementation of IDirectMusicPortDownload
//
// Functions:
//  CtIDirectMusicPortDownload::CtIDirectMusicPortDownload()
//  CtIDirectMusicPortDownload::~CtIDirectMusicPortDownload()
//  CtIDirectMusicPortDownload::InitTestClass()
//  CtIDirectMusicPortDownload::GetRealObjPtr()
//  CtIDirectMusicPortDownload::QueryInterface()
//  CtIDirectMusicPortDownload::GetBuffer()
//  CtIDirectMusicPortDownload::AllocateBuffer()
//  CtIDirectMusicPortDownload::GetDLId()
//  CtIDirectMusicPortDownload::Append()
//  CtIDirectMusicPortDownload::Download()
//  CtIDirectMusicPortDownload::Unload()
//
// History:
//  11/17/1997 - davidkl - created
//	12/23/1997 - a-llucar - Added new methods
//  03/30/1998 - davidkl - welcome to the brave new world of inheritance
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicPortDownload::CtIDirectMusicPortDownload()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
CtIDirectMusicPortDownload::CtIDirectMusicPortDownload()
{
    // initialize our member variables
	m_pUnk = NULL;
//	m_pdmPortDl = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicPortDownload";

} //*** end CtIDirectMusicPortDownload::CtIDirectMusicPortDownload()


//===========================================================================
// CtIDirectMusicPortDownload::~CtIDirectMusicPortDownload()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
CtIDirectMusicPortDownload::~CtIDirectMusicPortDownload(void)
{
    // nothing to do

} //*** emd CtIDirectMusicPortDownload::~CtIDirectMusicPortDownload()


//===========================================================================
// CtIDirectMusicPortDownload::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicPortDownload object pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmPortDl - pointer to real 
//                                      IDirectMusicPortDownload object
//
// Returns: 
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPortDownload::InitTestClass(IDirectMusicPortDownload *pdmPortDl)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmPortDl, sizeof(IDirectMusicPortDownload), FALSE))
	{
        m_pUnk = NULL;
//        m_pdmPortDl = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmPortDl));

/*
	// if we get here, we succeeded
	pdmPortDl->AddRef();
    m_pdmPortDl = pdmPortDl;
	return  S_OK;
*/
} //*** end CtIDirectMusicPortDownload::InitTestClass()


//===========================================================================
// CtIDirectMusicPortDownload::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicPortDownload **ppdmPortDl - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  11/17/1997 - davidkl - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicPortDownload::GetRealObjPtr(IDirectMusicPortDownload **ppdmPortDl)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmPortDl, sizeof(IDirectMusicPortDownload*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmPortDl));

/*
	// if we get here, return m_pdm
    *ppdmPortDl = m_pdmPortDl;
	((IDirectMusicPortDownload*)m_pUnk)->AddRef();
    return S_OK;
*/
} //*** end CtIDirectMusicPortDownload::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicPortDownload::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPortDownload::QueryInterface(REFIID riid, 
                                                    LPVOID *ppvObj)
{
    HRESULT hRes        = E_NOTIMPL;
    LPVOID  *ppvTemp    = NULL;
    BOOL    fMatchFound = FALSE;

    // BUGBUG - validate ppvObj

    // BUGBUG - this will need to do some wrapper class creation
    // based on test harness specific IIDs

    // use the base class to perform the actual QI
    hRes = (CtIUnknown::QueryInterface(riid, ppvObj));

    // BUGBUG - iff harness IID, wrap the interface returned

    // BUGBUG - otherwise, copy ppvTemp to ppvObj

    // done
    return hRes;

/*
    return tdmQueryInterface((IUnknown*)m_pdmPortDl,
                            (LPCTSTR)m_szInterfaceName,
                            riid,
                            ppvObj);
*/
} //*** end CtIDirectMusicPortDownload::QueryInterface()


//===========================================================================
// CtIDirectMusicPortDownload::AddRef
//
// Encapsulates calls to AddRef
//
// Parameters:
//
// Returns:
//  DWORD - new reference count
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
/*
DWORD CtIDirectMusicPortDownload::AddRef(void)
{

    // increment our object's refcount
    m_dwRefCount++;

    // call the real AddRef
    tdmAddRef((IUnknown*)m_pdmPortDl, (LPCTSTR)m_szInterfaceName);

    // return >our< refcount
    return m_dwRefCount;

} //*** end CtIDirectMusicPortDownload::AddRef()
*/

//===========================================================================
// CtIDirectMusicPortDownload::Release
//
// Encapsulates calls to Release
//
// Parameters:
//
// Returns:
//  DWORD - new reference count
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
/*
DWORD CtIDirectMusicPortDownload::Release(void)
{

    // decrement our object's refcount
    m_dwRefCount--;

    // call the real Release
    tdmRelease((IUnknown*)m_pdmPortDl, (LPCTSTR)m_szInterfaceName);

    // clean ourselves up
    if(0 >= m_dwRefCount)
    {
        delete this;
    }

    // return >our< refcount
    return m_dwRefCount;

} //*** end CtIDirectMusicPortDl::Release()
*/


//===========================================================================
// CtIDirectMusicPortDownload::GetBuffer
//
// Encapsulates calls to CtIDirectMusicPortDownload::GetBuffer
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetBuffer
//
// History:
//  12/23/1997 - a-llucar - created
//  03/03/1998 - davidkl - modified to pass a wrapped version of IDMDownload
//  03/30/1998 - davidkl - fixed logging
//===========================================================================
DWORD CtIDirectMusicPortDownload::GetBuffer(DWORD dwId, 
                                    CtIDirectMusicDownload **pptdmDownload)
{
    HRESULT                 hRes					= E_NOTIMPL;
    BOOL                    fValid_pptdmDownload    = TRUE;
    IDirectMusicDownload    *pdmDL                  = NULL;
    IDirectMusicDownload    **ppdmDL                = NULL;
    
    // validate pptdmDownload
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicDownload pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicPortDownload::GetBuffer().  Otherwise, we are to create and
    //  return a CtIDirectMusicDownload object
    if(!helpIsValidPtr((void*)pptdmDownload, sizeof(CtIDirectMusicDownload*),
                        FALSE))
    {
        fValid_pptdmDownload   = FALSE;

        // we have a bogus pointer, use it as such
        ppdmDL = (IDirectMusicDownload**)pptdmDownload;
    }
    else
    {
        // valid pointer, we need to use a real object
        ppdmDL = &pdmDL;

        // just in case we fail, init test object ptr to NULL
        *pptdmDownload = NULL;

    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwId         == %08Xh",
			dwId);
	// note, pIDMDownload is really a **
    fnsLog(PARAMLOGLEVEL, "pIDMDownload == %p   %s",
            ppdmDL,
            (fValid_pptdmDownload) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPortDownload*)m_pUnk)->GetBuffer(dwId, ppdmDL);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes          == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmDownload)
    {
    	// note, pIDMDownload is really a **
        fnsLog(PARAMLOGLEVEL, "*pIDMDownload == %p",
            pdmDL);       
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmDL, pptdmDownload);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmDownload && pdmDL)
    {
        pdmDL->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPortDownload::GetBuffer()


//===========================================================================
// CtIDirectMusicPortDownload::AllocateBuffer
//
// Encapsulates calls to CtIDirectMusicPortDownload::AllocateBuffer
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to AllocateBuffer
//
// History:
//  12/23/1997 - a-llucar - created
//  03/03/1998 - davidkl - modified to pass a wrapped version of IDMDownload
//  03/30/1998 - davidkl - fixed logging
//===========================================================================
DWORD CtIDirectMusicPortDownload::AllocateBuffer(DWORD dwSize, 
                                        CtIDirectMusicDownload **pptdmDownload)
{
    HRESULT                 hRes					= E_NOTIMPL;
    BOOL                    fValid_pptdmDownload    = TRUE;
    IDirectMusicDownload    *pdmDL                  = NULL;
    IDirectMusicDownload    **ppdmDL                = NULL;
    
    // validate pptdmDownload
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicDownload pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicPortDownload::AllocateBuffer().  Otherwise, we are to 
    //  create and return a CtIDirectMusicDownload object
    if(!helpIsValidPtr((void*)pptdmDownload, sizeof(CtIDirectMusicDownload*),
                        FALSE))
    {
        fValid_pptdmDownload   = FALSE;

        // we have a bogus pointer, use it as such
        ppdmDL = (IDirectMusicDownload**)pptdmDownload;
    }
    else
    {
        // valid pointer, we need to use a real object
        ppdmDL = &pdmDL;

        // just in case we fail, init test object ptr to NULL
        *pptdmDownload = NULL;

    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::AllocateBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwSize == %08Xh",
            dwSize);
	// note, pIDMDownload is really a **
    fnsLog(PARAMLOGLEVEL, "pIDMDownload == %p   %s",
            ppdmDL,
            (fValid_pptdmDownload) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPortDownload*)m_pUnk)->AllocateBuffer(dwSize, ppdmDL);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::AllocateBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes          == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmDownload)
    {
    	// note, pIDMDownload is really a **
        fnsLog(PARAMLOGLEVEL, "*pIDMDownload == %p",
            pdmDL);       
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmDL, pptdmDownload);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmDownload && pdmDL)
    {
        pdmDL->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPortDownload::AllocateBuffer()


//===========================================================================
// CtIDirectMusicPortDownload::FreeBuffer
//
// Encapsulates calls to CtIDirectMusicPortDownload::FreeBuffer
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to FreeBuffer
//
// History:
//  12/23/1997 - a-llucar - created
//  03/03/1998 - davidkl - modified to pass a wrapped version of IDMDownload
//  03/30/1998 - davidkl - fixed logging
//  05/11/1998 - davidkl - Removed to reflect interface change
//===========================================================================
/* 
DWORD CtIDirectMusicPortDownload::FreeBuffer(CtIDirectMusicDownload *ptdmDownload)
{
    HRESULT                 hRes				= E_NOTIMPL;
    BOOL                    fValid_ptdmDownload	= TRUE;
    IDirectMusicDownload    *pdmDL              = NULL;
    
    // validate ptdmDownload
    if(!helpIsValidPtr((void*)ptdmDownload, sizeof(CtIDirectMusicDownload),
                        FALSE))
    {
        fValid_ptdmDownload   = FALSE;

        // we have a bogus pointer, use it as such
        pdmDL = (IDirectMusicDownload*)ptdmDownload;
    }
    else
    {
        // we have a good pointer, get the real object
        hRes = ptdmDownload->GetRealObjPtr(&pdmDL);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::FreeBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pIDMDownload == %p   %s",
            pdmDL,
            fValid_ptdmDownload ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPortDownload*)m_pUnk)->FreeBuffer(pdmDL);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::FreeBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(pdmDL)
    {
        pdmDL->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPortDownload::FreeBuffer()
*/


//===========================================================================
// CtIDirectMusicPortDownload::GetDLId
//
// Encapsulates calls to CtIDirectMusicPortDownload::GetDLId
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetDLId
//
// History:
//  12/23/1997 - a-llucar - created
//  03/03/1998 - davidkl - fixed logging bugs and added pointer validation
//  03/30/1998 - davidkl - fixed logging (for real this time :)
//===========================================================================
DWORD CtIDirectMusicPortDownload::GetDLId(DWORD *pdwStartDLId, DWORD dwCount)
{
    HRESULT hRes				= E_NOTIMPL;   
    BOOL    fValid_dwStartDLId	= TRUE;
 
    // validate dwStartDLId
    if(!helpIsValidPtr((void*)pdwStartDLId, sizeof(DWORD),
                        FALSE))
    {
        fValid_dwStartDLId  = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetDLId()",
            m_szInterfaceName);
    // NOTE dwStartDLId is a *
    fnsLog(PARAMLOGLEVEL, "pdwStartDLId == %p   %s",
            pdwStartDLId,
            (fValid_dwStartDLId) ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "dwCount == %08Xh",
            dwCount);

    // call the real function
    hRes = ((IDirectMusicPortDownload*)m_pUnk)->GetDLId(pdwStartDLId, dwCount);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetDLId()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes         == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_dwStartDLId)
    {
        // NOTE dwStartDLId is a *
        fnsLog(PARAMLOGLEVEL, "*pdwStartDLId == %08Xh",
                *pdwStartDLId);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPortDownload::GetDLId()


//===========================================================================
// CtIDirectMusicPortDownload::GetAppend
//
// Encapsulates calls to CtIDirectMusicPortDownload::GetAppend
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetAppend
//
// History:
//	10/13/1998 - a-trevg - created
//===========================================================================
DWORD CtIDirectMusicPortDownload::GetAppend(DWORD* pdwAppend)
{
    HRESULT	hRes	= E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetAppend()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwAppend == %08Xh",
            pdwAppend);

    // call the real function
    hRes = ((IDirectMusicPortDownload*)m_pUnk)->GetAppend(pdwAppend);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetAppend()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
	fnsLog(PARAMLOGLEVEL, "*pdwAppend == %08Xh",
            *pdwAppend);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPortDownload::Download()


//===========================================================================
// CtIDirectMusicPortDownload::Download
//
// Encapsulates calls to CtIDirectMusicPortDownload::Download
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to Download
//
// History:
//  12/23/1997 - a-llucar - created
//  03/03/1998 - davidkl - modified to pass a wrapped version of IDMDownload
//  03/30/1998 - davidkl - fixed logging
//===========================================================================
DWORD CtIDirectMusicPortDownload::Download(CtIDirectMusicDownload* ptdmDownload)
{
    HRESULT                 hRes				= E_NOTIMPL;
    BOOL                    fValid_ptdmDownload	= TRUE;
    IDirectMusicDownload    *pdmDL              = NULL;
    
    // validate ptdmDownload
    if(!helpIsValidPtr((void*)ptdmDownload, sizeof(CtIDirectMusicDownload),
                        FALSE))
    {
        fValid_ptdmDownload   = FALSE;

        // we have a bogus pointer, use it as such
        pdmDL = (IDirectMusicDownload*)ptdmDownload;
    }
    else
    {
        // we have a good pointer, get the real object
        hRes = ptdmDownload->GetRealObjPtr(&pdmDL);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Download()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pIDMDownload == %p   %s",
            pdmDL,
            fValid_ptdmDownload ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPortDownload*)m_pUnk)->Download(pdmDL);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Download()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmDownload && pdmDL)
    {
        pdmDL->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPortDownload::Download()


//===========================================================================
// CtIDirectMusicPortDownload::Unload
//
// Encapsulates calls to CtIDirectMusicPortDownload::Unload
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to Unload
//
// History:
//  12/23/1997 - a-llucar - created
//  03/03/1998 - davidkl - modified to pass a wrapped version of IDMDownload
//  03/30/1998 - davidkl - fixed logging
//===========================================================================
DWORD CtIDirectMusicPortDownload::Unload(CtIDirectMusicDownload* ptdmDownload)
{
    HRESULT                 hRes				= E_NOTIMPL;
    BOOL                    fValid_ptdmDownload	= TRUE;
    IDirectMusicDownload    *pdmDL              = NULL;
    
    // validate ptdmDownload
    if(!helpIsValidPtr((void*)ptdmDownload, sizeof(CtIDirectMusicDownload),
                        FALSE))
    {
        fValid_ptdmDownload   = FALSE;

        // we have a bogus pointer, use it as such
        pdmDL = (IDirectMusicDownload*)ptdmDownload;
    }
    else
    {
        // we have a good pointer, get the real object
        hRes = ptdmDownload->GetRealObjPtr(&pdmDL);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Unload()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pIDMDownload == %p   %s",
            pdmDL,
            fValid_ptdmDownload ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPortDownload*)m_pUnk)->Unload(pdmDL);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Unload()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmDownload && pdmDL)
    {
        pdmDL->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPortDownload::Unload()


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================















