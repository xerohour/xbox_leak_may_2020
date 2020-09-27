//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctwave.cpp
//
//===========================================================================
//===========================================================================
//===========================================================================
//===========================================================================
// ctwave.cpp
//
// Test harness implementation of IDirectSoundDownloadedWave
//
// Functions:
//    CtIDirectSoundDownloadedWave::CtIDirectSoundDownloadedWave()
//    CtIDirectSoundDownloadedWave::~CtIDirectSoundDownloadedWave()
//    CtIDirectSoundDownloadedWave::InitTestClass()
//    CtIDirectSoundDownloadedWave::GetRealObjPtr()
//  IUnknown
//    CtIDirectSoundDownloadedWave::QueryInterface()
//
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectSoundDownloadedWave::CtIDirectSoundDownloadedWave()
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
CtIDirectSoundDownloadedWave::CtIDirectSoundDownloadedWave()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectSoundDownloadedWave";

} //*** end CtIDirectSoundDownloadedWave::CtIDirectSoundDownloadedWave()




//===========================================================================
// CtIDirectSoundDownloadedWave::~CtIDirectSoundDownloadedWave()
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
CtIDirectSoundDownloadedWave::~CtIDirectSoundDownloadedWave(void)
{
    // nothing to do

} //*** emd CtIDirectSoundDownloadedWave::~CtIDirectSoundDownloadedWave()




//===========================================================================
// CtIDirectSoundDownloadedWave::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectSoundDownloadedWave object pointer for future use.
//
// Parameters:
//  CtIDirectSoundDownloadedWave    *pdmDSDLWave    - pointer to real CtIDirectSoundDownloadedWave object
//
// Returns: 
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectSoundDownloadedWave::InitTestClass(IDirectSoundDownloadedWave *pdmDSDLWave)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmDSDLWave, sizeof(IDirectSoundDownloadedWave), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmDSDLWave));

} //*** end CtIDirectSoundDownloadedWave::InitTestClass()




//===========================================================================
// CtIDirectSoundDownloadedWave::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectSoundDownloadedWave **ppdmDSDLWave - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/14/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectSoundDownloadedWave::GetRealObjPtr(IDirectSoundDownloadedWave **ppdmDSDLWave)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmDSDLWave, sizeof(IDirectSoundDownloadedWave*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmDSDLWave));

} //*** end CtIDirectSoundDownloadedWave::GetRealObjPtr()




//===========================================================================
// CtIDirectSoundDownloadedWave::QueryInterface
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
HRESULT CtIDirectSoundDownloadedWave::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectSoundDownloadedWave::QueryInterface()
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




