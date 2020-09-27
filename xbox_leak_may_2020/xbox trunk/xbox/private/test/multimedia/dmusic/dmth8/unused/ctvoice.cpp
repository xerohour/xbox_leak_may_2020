//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctvoice.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctvoice.cpp
//
// Test harness implementation of IDirectMusicVoice
//
// Functions:
//    CtIDirectMusicVoice::CtIDirectMusicVoice()
//    CtIDirectMusicVoice::~CtIDirectMusicVoice()
//    CtIDirectMusicVoice::InitTestClass()
//    CtIDirectMusicVoice::GetRealObjPtr()
//  IUnknown
//    CtIDirectMusicVoice::QueryInterface()
//    CtIDirectMusicVoice::Play()
//  DX8
//    CtIDirectMusicVoice::Stop()
//
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicVoice::CtIDirectMusicVoice()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
CtIDirectMusicVoice::CtIDirectMusicVoice()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicVoice";

} //*** end CtIDirectMusicVoice::CtIDirectMusicVoice()




//===========================================================================
// CtIDirectMusicVoice::~CtIDirectMusicVoice()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
CtIDirectMusicVoice::~CtIDirectMusicVoice(void)
{
    // nothing to do

} //*** emd CtIDirectMusicVoice::~CtIDirectMusicVoice()




//===========================================================================
// CtIDirectMusicVoice::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicVoice object pointer for future use.
//
// Parameters:
//  CtIDirectMusicVoice    *pdmDSDLWave    - pointer to real IDirectMusicVoice object
//
// Returns: 
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicVoice::InitTestClass(IDirectMusicVoice *pdmVoice)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmVoice, sizeof(IDirectMusicVoice), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmVoice));

} //*** end CtIDirectMusicVoice::InitTestClass()




//===========================================================================
// CtIDirectMusicVoice::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicVoice **ppdmVoice - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicVoice::GetRealObjPtr(IDirectMusicVoice **ppdmVoice)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmVoice, sizeof(IDirectMusicVoice*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmVoice));

} //*** end CtIDirectMusicVoice::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicVoice::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicVoice::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicVoice::QueryInterface()




//===========================================================================
// CtIDirectMusicVoice::Play()
//
// Encapsulates calls to Play
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicVoice::Play
(
	REFERENCE_TIME rtStart,
	LONG prPitch,
	LONG vrVolume
)
{
    HRESULT     hRes						= E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Play()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rtStart == %016Xh",
            rtStart);
    fnsLog(PARAMLOGLEVEL, "prPitch == %ld",
            prPitch);
    fnsLog(PARAMLOGLEVEL, "vrVolume == %ld",
            vrVolume);
    
    // call the real function
    hRes = ((IDirectMusicVoice*)m_pUnk)->Play(rtStart,prPitch,vrVolume);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Play()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicVoice::Play()




//===========================================================================
// CtIDirectMusicVoice::Stop()
//
// Encapsulates calls to Stop
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicVoice::Stop(REFERENCE_TIME rtStop)
{
    HRESULT     hRes						= E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Stop()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rtStop == %016Xh",
            rtStop);
    
    // call the real function
    hRes = ((IDirectMusicVoice*)m_pUnk)->Stop(rtStop);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Stop()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicVoice::Stop()


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




