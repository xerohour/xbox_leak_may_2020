//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctcollec.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctcollec.cpp
//
// Test harness implementation of IDirectMusicCollection
//
// Functions:
//  CtIDirectMusicCollection::CtIDirectMusicCollection()
//  CtIDirectMusicCollection::~CtIDirectMusicCollection()
//  CtIDirectMusicCollection::InitTestClass()
//  CtIDirectMusicCollection::GetRealObjPtr()
//  CtIDirectMusicCollection::QueryInterface()
//  CtIDirectMusicCollection::AddRef()
//  CtIDirectMusicCollection::Release()
//  CtIDirectMusicCollection::GetInstrument()
//  CtIDirectMusicCollection::EnumInstrument()
//
// History:
//  10/24/1997 - davidkl - created
//  12/23/1997 - a-llucar - added GetInstrumentPatch, GetNumInstruments 
//  01/09/1998 - a-llucar - removed GetInstrumentPatch, GetNumInstrument, 
//                          added EnumInstrument
//  03/29/1998 - davidkl - brave new world... implemented as a derrived class
//===========================================================================

#include "globals.h"
//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicCollection::CtIDirectMusicCollection()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
CtIDirectMusicCollection::CtIDirectMusicCollection()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicCollection";

} //*** end CtIDirectMusicCollection::CtIDirectMusicCollection()


//===========================================================================
// CtIDirectMusicCollection::~CtIDirectMusicCollection()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
CtIDirectMusicCollection::~CtIDirectMusicCollection(void)
{
    // nothing to do

} //*** emd CtIDirectMusicCollection::~CtIDirectMusicCollection()


//===========================================================================
// CtIDirectMusicCollection::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicCollection object pointer for future use.
//
// Parameters:
//  IDirectMusicCollection *pdmCollect - pointer to real IDirectMusicCollection 
//                                      object
//
// Returns: 
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicCollection::InitTestClass(IDirectMusicCollection *pdmCollect)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmCollect, sizeof(IDirectMusicCollection), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmCollect));

} //*** end CtIDirectMusicCollection::InitTestClass()


//===========================================================================
// CtIDirectMusicCollection::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicCollection **ppdmCollect - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//	10/24/1997 - davidkl - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicCollection::GetRealObjPtr(IDirectMusicCollection **ppdmCollect)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmCollect, sizeof(IDirectMusicCollection*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmCollect));

} //*** end CtIDirectMusicCollection::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicCollection::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicCollection::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicCollection::QueryInterface()


//===========================================================================
// CtIDirectMusicCollection::GetInstrument
//
// Encapsulates calls to GetInstrument
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetInstrument
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicCollection::GetInstrument(DWORD dwPatch, 
                                            CtIDirectMusicInstrument **pptdmInst)
{
    HRESULT                 hRes                = E_NOTIMPL;
    BOOL                    fValid_pptdmInst    = TRUE;
    IDirectMusicInstrument  *pdmInstrument      = NULL;
    IDirectMusicInstrument  **ppdmInstrument    = NULL;

    // validate pptdmInst
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicInstrument pointer, 
    //  we assume that we are to perform an invalid parameter test on 
    //  IDirectMusicCollection::GetInstrument().  Otherwise, we are to 
    //  create and return a CtIDirectMusicInstrument object
    if(!helpIsValidPtr((void*)pptdmInst, 
                        sizeof(CtIDirectMusicInstrument*),
                        FALSE))
    {
        fValid_pptdmInst = FALSE;

        // we have a bad pointer, use as the real thing
        ppdmInstrument = (IDirectMusicInstrument**)pptdmInst;
    }
    else
    {
        // valid pointer, create a real object
        ppdmInstrument = &pdmInstrument;

        // just in case we fail, init test object ptr to NULL
        *pptdmInst = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetInstrument",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwPatch           == %08Xh",
            dwPatch);
    fnsLog(PARAMLOGLEVEL, "ppdmInstrument    == %p   %s",
            ppdmInstrument,
            fValid_pptdmInst ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicCollection*)m_pUnk)->GetInstrument(dwPatch, ppdmInstrument);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetInstrument",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes            == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmInst)
    {
        fnsLog(PARAMLOGLEVEL, "*ppdmInstrument == %p",
            pdmInstrument);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmInstrument, pptdmInst);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmInst && pdmInstrument)
    {
        pdmInstrument->Release();
    }
    return hRes;

} //*** end CtIDirectMusicCollection::GetInstrument()


//===========================================================================
// CtIDirectMusicCollection::EnumInstrument
//
// Encapsulates calls to EnumInstrument
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to EnumInstrument
//
// History:
//  10/24/1997 - davidkl - created
//  03/29/1998 - davidkl - fixed ptr validation bug
//===========================================================================
HRESULT CtIDirectMusicCollection::EnumInstrument(DWORD dwIndex, DWORD* pdwPatch, 
												 LPWSTR pName, DWORD cwchName)
{
    HRESULT                 hRes                = E_NOTIMPL;
    BOOL                    fValid_pdwPatch	    = TRUE;
	BOOL					fValid_pName		= TRUE;
    
	if(!helpIsValidPtr((void*)pdwPatch, sizeof(DWORD), FALSE))
    {
        fValid_pdwPatch = FALSE;
	}

//	can be NULL - don't check
//	if(!helpIsValidPtr((void*)pName, sizeof(WCHAR), FALSE))
//	{
//		fValid_pName = FALSE;
//	}

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumInstrument",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIndex == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "pdwPatch == %p   %s",
            pdwPatch,
            fValid_pdwPatch ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pName == %08Xh",pName);
//    fnsLog(PARAMLOGLEVEL, "pName == %p   %s",
//          pName,
//          fValid_pName ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "cwchName == %08Xh",
            cwchName);

    // call the real function
    hRes = ((IDirectMusicCollection*)m_pUnk)->EnumInstrument(dwIndex, 
                                                            pdwPatch, 
                                                            pName, 
                                                            cwchName);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumInstrument",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicCollection::EnumInstrument()


