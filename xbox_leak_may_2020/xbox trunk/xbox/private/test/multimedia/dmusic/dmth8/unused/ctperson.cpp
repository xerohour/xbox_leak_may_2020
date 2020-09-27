//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctperson.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctperson.cpp
//
// Test harness implementation of IDirectMusicChordMap
//
// Functions:
//  CtIDirectMusicChordMap::CtIDirectMusicChordMap()
//  CtIDirectMusicChordMap::~CtIDirectMusicChordMap()
//  CtIDirectMusicChordMap::InitTestClass()
//  CtIDirectMusicChordMap::GetRealObjPtr()
//  CtIDirectMusicChordMap::QueryInterface()
//  CtIDirectMusicChordMap::GetScale()
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - updated to latest dmusic arch
//  03/31/1998 - davidkl - inheritance update
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicChordMap::CtIDirectMusicChordMap()
//
// Default constructor
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicChordMap::CtIDirectMusicChordMap(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicChordMap";

} //*** end CtIDirectMusicChordMap::CtIDirectMusicChordMap()


//===========================================================================
// CtIDirectMusicChordMap::~CtIDirectMusicChordMap()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicChordMap::~CtIDirectMusicChordMap(void)
{
    // nothing to do

} //*** end CtIDirectMusicChordMap::~CtIDirectMusicChordMap()


//===========================================================================
// CtIDirectMusicChordMap::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmChordMap - pointer to real 
//                                      IDirectMusic object
//
// Returns: 
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicChordMap::InitTestClass(IDirectMusicChordMap *pdmChordMap)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmChordMap, sizeof(IDirectMusicChordMap), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmChordMap));

} //*** end CtIDirectMusicChordMap::InitTestClass()


//===========================================================================
// CtIDirectMusicChordMap::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicChordMap **ppdmChordMap - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/02/1998 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicChordMap::GetRealObjPtr(IDirectMusicChordMap **ppdmChordMap)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmChordMap, sizeof(IDirectMusicChordMap*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmChordMap));

} //*** end CtIDirectMusicChordMap::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicChordMap::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  01/02/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicChordMap::QueryInterface(REFIID riid, 
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

} //*** end CtIDirectMusicChordMap::QueryInterface


//===========================================================================
// CtIDirectMusicChordMap::GetScale()
//
// Encapsulates calls to GetScale
//
// History:
//  01/02/1998 - a-llucar - created
//  03/05/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicChordMap::GetScale(DWORD* pdwScale)
{
    HRESULT hRes			= E_NOTIMPL;
	BOOL	fValid_pdwScale	= TRUE;
    
    // validate pdwScale
    if(!helpIsValidPtr((void*)pdwScale, sizeof(DWORD), FALSE))
    {
        fValid_pdwScale = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetScale()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwScale == %08Xh   %s",
            pdwScale,
            fValid_pdwScale ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicChordMap*)m_pUnk)->GetScale(pdwScale);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetScale()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwScale)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwScale == %08Xh",
                *pdwScale);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicChordMap::GetScale()


//===========================================================================
//===========================================================================









