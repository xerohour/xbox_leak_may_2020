//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctscripterr.cpp
//
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
// ctscripterr.cpp
//
// Test harness implementation of IDirectMusicScriptError
//
// Functions:
//    CtIDirectMusicScriptError::CtIDirectMusicScriptError()
//    CtIDirectMusicScriptError::~CtIDirectMusicScriptError()
//    CtIDirectMusicScriptError::InitTestClass()
//    CtIDirectMusicScriptError::GetRealObjPtr()
//  IUnknown
//    CtIDirectMusicScriptError::QueryInterface()
//
//    CtIDirectMusicScriptError::GetError()
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================


#if 0 //  Removed will clean up later

#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicScriptError::CtIDirectMusicScriptError()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
CtIDirectMusicScriptError::CtIDirectMusicScriptError()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicScriptError";

} //*** end CtIDirectMusicScriptError::CtIDirectMusicScriptError()




//===========================================================================
// CtIDirectMusicScriptError::~CtIDirectMusicScriptError()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
CtIDirectMusicScriptError::~CtIDirectMusicScriptError(void)
{
    // nothing to do

} //*** emd CtIDirectMusicScriptError::~CtIDirectMusicScriptError()




//===========================================================================
// CtIDirectMusicScriptError::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicScriptError object pointer for future use.
//
// Parameters:
//  CtIDirectMusicScriptError    *pdmScriptError    - pointer to real IDirectMusicScriptError object
//
// Returns: 
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicScriptError::InitTestClass(IDirectMusicScriptError *pdmScriptError)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmScriptError, sizeof(IDirectMusicScriptError), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmScriptError));

} //*** end CtIDirectMusicScriptError::InitTestClass()




//===========================================================================
// CtIDirectMusicScriptError::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicScriptError **ppdmScriptError - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicScriptError::GetRealObjPtr(IDirectMusicScriptError **ppdmScriptError)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmScriptError, sizeof(IDirectMusicScriptError*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmScriptError));

} //*** end CtIDirectMusicScriptError::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicScriptError::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicScriptError::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicScriptError::QueryInterface()




//===========================================================================
// CtIDirectMusicScript::GetError()
//
// Encapsulates calls to Init
//
// History:
//  10/26/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicScriptError::GetError(DMUS_SCRIPT_ERRORINFO *pErrorInfo)
{
    HRESULT                 hRes				= E_NOTIMPL;
    BOOL				    fValid_pErrorInfo	= TRUE;

    // validate pPMSG
    if(!helpIsValidPtr((void*)pErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO), FALSE))
    {
        fValid_pErrorInfo = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetError()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pErrorInfo == %p   %s",
            pErrorInfo,
            fValid_pErrorInfo ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicScriptError*)m_pUnk)->GetError(pErrorInfo);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetError()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done

    return hRes;

} //*** end CtIDirectMusicScriptError::GetError()






#endif // #if 0


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




