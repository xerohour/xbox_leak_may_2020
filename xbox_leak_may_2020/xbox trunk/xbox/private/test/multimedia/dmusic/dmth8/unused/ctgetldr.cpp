//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctgetldr.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctgetldr.cpp
//
// Test harness implementation of IDirectMusicGetLoader
//
// Functions:
//  CtIDirectMusicGetLoader::CtIDirectMusicGetLoader()
//  CtIDirectMusicGetLoader::~CtIDirectMusicGetLoader()
//  CtIDirectMusicGetLoader::InitTestClass()
//  CtIDirectMusicGetLoader::GetRealObjPtr()
//  CtIDirectMusicGetLoader::QueryInterface()
//  CtIDirectMusicGetLoader::GetLoader()
//
// History:
//  04/21/1998 - davidkl - created
//===========================================================================

#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"

//---------------------------------------------------------------------------

//===========================================================================
// CtIDirectMusicGetLoader::CtIDirectMusicGetLoader()
//
// Default constructor
//
// History:
//  04/21/1998 - davidkl - created
//===========================================================================
CtIDirectMusicGetLoader::CtIDirectMusicGetLoader(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicGetLoader";

} //*** end CtIDirectMusicGetLoader::CtIDirectMusicGetLoader()


//===========================================================================
// CtIDirectMusicGetLoader::~CtIDirectMusicGetLoader()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  04/21/1998 - davidkl - created
//===========================================================================
CtIDirectMusicGetLoader::~CtIDirectMusicGetLoader(void)
{
    // nothing to do

} //*** end CtIDirectMusicGetLoader::~CtIDirectMusicGetLoader()


//===========================================================================
// CtIDirectMusicLoader::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmLoader - pointer to real object
//
// Returns: 
//
// History:
//  04/21/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicGetLoader::InitTestClass(IDirectMusicGetLoader *pGetLdr)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pGetLdr, sizeof(IDirectMusicGetLoader), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pGetLdr));

} //*** end CtIDirectMusicGetLoader::InitTestClass()


//===========================================================================
// CtIDirectMusicGetLoader::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicGetLoader **ppGetLdr - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  04/21/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicGetLoader::GetRealObjPtr(IDirectMusicGetLoader **ppGetLdr)
{

	// validate ppdm
	if(!helpIsValidPtr(ppGetLdr, sizeof(IDirectMusicGetLoader*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppGetLdr));

} //*** end CtIDirectMusicGetLoader::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicGetLoader::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  04/21/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicGetLoader::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicGetLoader::QueryInterface


//===========================================================================
// CtIDirectMusicGetLoader::GetLoader()
//
// Encapsulates calls to GetLoader()
//
// History:
//  04/21/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicGetLoader::GetLoader(CtIDirectMusicLoader **pptdmLoader)
{
    HRESULT             hRes                = E_NOTIMPL;
    IDirectMusicLoader  *pLoader            = NULL;
    IDirectMusicLoader  **ppLoader          = NULL;
    BOOL                fValid_pptdmLoader  = TRUE;

	// validate pptdmLoader
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicLoader pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicGetLoader::GetLoader().  Otherwise, we are to create and
    //  return a CtIDirectLoader object
    if(!helpIsValidPtr((void*)pptdmLoader, sizeof(CtIDirectMusicLoader*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmLoader = FALSE;
        ppLoader = (IDirectMusicLoader**)pptdmLoader;
    }
    else
    {
        // valid pointer, create a real object
        ppLoader = &pLoader;

        // just in case we fail, init test object ptr to NULL
        *ppLoader = NULL;
    }

	fnsIncrementIndent();

	// log inputs
	fnsLog(CALLLOGLEVEL, "--- Calling %s::GetLoader()", 
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppLoader == %p   %s",
			ppLoader,
			fValid_pptdmLoader ? "" : "BAD");

	// Get a real IDirectMusicObject
	hRes = ((IDirectMusicGetLoader*)m_pUnk)->GetLoader(ppLoader);

	// log results
	fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetLoader()",
			m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
			tdmXlatHRESULT(hRes), hRes);
	if(fValid_pptdmLoader)
	{
        fnsLog(PARAMLOGLEVEL, "*ppLoader == %p",
				pLoader);
	}
    
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pLoader, pptdmLoader);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

	fnsDecrementIndent();

    // done
    if(fValid_pptdmLoader && pLoader)
    {
        pLoader->Release();
    }
    return hRes;

} //*** end CtIDirectMusicGetLoader::GetLoader()


