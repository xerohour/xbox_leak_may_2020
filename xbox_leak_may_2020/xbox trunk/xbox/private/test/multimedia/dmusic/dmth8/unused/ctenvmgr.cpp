//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctenvmgr.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctenvmgr.cpp
//
// Test harness implementation of IDirectMusicEnvelopeManager
//
// Functions:
//    CtIDirectMusicEnvelopeManager::CtIDirectMusicEnvelopeManager()
//    CtIDirectMusicEnvelopeManager::~CtIDirectMusicEnvelopeManager()
//    CtIDirectMusicEnvelopeManager::InitTestClass()
//    CtIDirectMusicEnvelopeManager::GetRealObjPtr()
//  IUnknown
//    CtIDirectMusicEnvelopeManager::QueryInterface()
//  DX8
//    CtIDirectMusicEnvelopeManager::Init()
//    CtIDirectMusicEnvelopeManager::GetValue()
//
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicEnvelopeManager::CtIDirectMusicEnvelopeManager()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
CtIDirectMusicEnvelopeManager::CtIDirectMusicEnvelopeManager()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicEnvelopeManager";

} //*** end CtIDirectMusicEnvelopeManager::CtIDirectMusicEnvelopeManager()




//===========================================================================
// CtIDirectMusicEnvelopeManager::~CtIDirectMusicEnvelopeManager()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
CtIDirectMusicEnvelopeManager::~CtIDirectMusicEnvelopeManager(void)
{
    // nothing to do

} //*** emd CtIDirectMusicEnvelopeManager::~CtIDirectMusicEnvelopeManager()




//===========================================================================
// CtIDirectMusicEnvelopeManager::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicEnvelopeManager object pointer for future use.
//
// Parameters:
//  CtIDirectMusicEnvelopeManager    *pdmManager    - pointer to real IDirectMusicEnvelopeManager object
//
// Returns: 
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicEnvelopeManager::InitTestClass(IDirectMusicEnvelopeManager *pdmManager)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmManager, sizeof(IDirectMusicEnvelopeManager), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmManager));

} //*** end CtIDirectMusicEnvelopeManager::InitTestClass()




//===========================================================================
// CtIDirectMusicEnvelopeManager::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicEnvelopeManager **ppdmManager - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicEnvelopeManager::GetRealObjPtr(IDirectMusicEnvelopeManager **ppdmManager)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmManager, sizeof(IDirectMusicEnvelopeManager*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmManager));

} //*** end CtIDirectMusicEnvelopeManager::GetRealObjPtr()




//===========================================================================
// CtIDirectMusicEnvelopeManager::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicEnvelopeManager::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicEnvelopeManager::QueryInterface()




//===========================================================================
// CtIDirectMusicEnvelopeManager::Init()
//
// Encapsulates calls to Init
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicEnvelopeManager::Init(DWORD dwNumParams)
{
    HRESULT     hRes						= E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwNumParams  == %08Xh",
            dwNumParams);
    
    // call the real function
    hRes = ((IDirectMusicEnvelopeManager*)m_pUnk)->Init(dwNumParams);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Init()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicEnvelopeManager::Init()




//===========================================================================
// CtIDirectMusicEnvelopeManager::GetValue()
//
// Encapsulates calls to GetValue
//
// History:
//  11/02/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicEnvelopeManager::GetValue
(
	DWORD dwIndex,
	REFERENCE_TIME refTime,
	long *pValue, 
	REFERENCE_TIME *prefNextPoint
)
{
    HRESULT	hRes					= E_NOTIMPL;
    BOOL	fValid_pValue			= TRUE;
    BOOL	fValid_prefNextPoint	= TRUE;
    
    // validate pValue
    if(!helpIsValidPtr((void*)pValue, sizeof(long), FALSE))
    {
        fValid_pValue	= FALSE;
    }

    // validate prefNextPoint
    if(!helpIsValidPtr((void*)prefNextPoint, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prefNextPoint	= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetValue()",
            m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "dwIndex       == %08Xh",
            dwIndex);
    fnsLog(PARAMLOGLEVEL, "refTime       == %016Xh",
            refTime);
    fnsLog(PARAMLOGLEVEL, "pValue        == %p   %s",
            pValue,
            fValid_pValue? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "prefNextPoint == %p   %s",
            prefNextPoint,
            fValid_prefNextPoint? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicEnvelopeManager*)m_pUnk)->GetValue(dwIndex,refTime,pValue,prefNextPoint);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetValue()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicEnvelopeManager::Stop()


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




