//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctclock.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctclock.cpp
//
// Test harness implementation of IReferenceClock
//
// Functions:
//  CtIReferenceClock::CtIReferenceClock()
//  CtIReferenceClock::~CtIReferenceClock()
//  CtIReferenceClock::InitTestClass()
//  CtIReferenceClock::GetRealObjPtr()
//  CtIReferenceClock::QueryInterface()
//  CtIReferenceClock::GetTime()
//  CtIReferenceClock::AdviseTime()
//  CtIReferenceClock::AdvisePeriodic()
//  CtIReferenceClock::Unadvise()
//
// History:
//  11/17/1997 - davidkl - created
//  03/24/1998 - davidkl - brave new world...  now a derived class
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIReferenceClock::CtIReferenceClock()
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
CtIReferenceClock::CtIReferenceClock()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = TEXT("IReferenceClock");

} //*** end CtIReferenceClock::CtIReferenceClock()


//===========================================================================
// CtIReferenceClock::~CtIReferenceClock
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
CtIReferenceClock::~CtIReferenceClock()
{
    // nothing to do

} //*** end CtIReferenceClock::~CtIReferenceClock()



//===========================================================================
// CtIReferenceClock::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IReferenceClock object pointer for future use.
//
// Parameters:
//  CtIReferenceClock *pRefCount - pointer to real object
//
// Returns: 
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIReferenceClock::InitTestClass(IReferenceClock *pRefClock)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pRefClock, sizeof(IReferenceClock), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pRefClock));

} //*** end CtIReferenceClock::InitTestClass()


//===========================================================================
// CtIReferenceClock::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IReferenceClock **ppRefClock - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//	11/17/1997 - davidkl - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIReferenceClock::GetRealObjPtr(IReferenceClock **ppRefClock)
{

	// validate ppdm
	if(!helpIsValidPtr(ppRefClock, sizeof(IReferenceClock*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppRefClock));

} //*** end CtIReferenceClock::GetRealObjPtr()


//===========================================================================
// CtIReferenceClock::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//  HRESULT - return value from actual call to QueryInterface
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIReferenceClock::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIReferenceClock::QueryInterface()


//===========================================================================
// CtIReferenceClock::GetTime
//
// Encapsulates calls to GetTime
//
// Parameters:
//
// Returns:
//  HRESULT - return value from actual call to GetTime
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIReferenceClock::GetTime(REFERENCE_TIME *pTime)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_pTime    = TRUE;

    // validate pTime
    if(!helpIsValidPtr((void*)pTime, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_pTime = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, TEXT("--- Calling %s::GetTime()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("pTime == %p   %s"),
            pTime,
            fValid_pTime ? TEXT("") : TEXT("BAD"));

    // call the real function
    hRes = ((IReferenceClock*)m_pUnk)->GetTime(pTime);

    // log results
    fnsLog(CALLLOGLEVEL, TEXT("--- Returned from %s::GetTime()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("hRes   == %s (%08Xh)"),
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pTime)
    {
        fnsLog(PARAMLOGLEVEL, TEXT("*pTime == %016Xh"),
                *pTime);         
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIReferenceClock::GetTime()


//===========================================================================
// CtIReferenceClock::AdviseTime
//
// Encapsulates calls to AdviseTime
//
// Parameters:
//
// Returns:
//  HRESULT - return value from actual call to AdviseTime
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIReferenceClock::AdviseTime(REFERENCE_TIME baseTime,
                                REFERENCE_TIME streamTime,
                                HANDLE hEvent, DWORD *pdwAdviseCookie)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pdwAdviseCookie  = TRUE;

    // validate pdwAdviseCookie
    if(!helpIsValidPtr((void*)pdwAdviseCookie, sizeof(DWORD), FALSE))
    {
        fValid_pdwAdviseCookie = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, TEXT("--- Calling %s::AdviseTime()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("baseTime        == %016Xh"),
            baseTime);
    fnsLog(PARAMLOGLEVEL, TEXT("streamTime      == %016Xh"),
            streamTime);
    fnsLog(PARAMLOGLEVEL, TEXT("hEvent          == %08Xh"),
            hEvent);
    fnsLog(PARAMLOGLEVEL, TEXT("pdwAdviseCookie == %p   %s"),
            pdwAdviseCookie,
            fValid_pdwAdviseCookie ? TEXT("") : TEXT("BAD"));

    // call the real function
    hRes = ((IReferenceClock*)m_pUnk)->AdviseTime(baseTime, streamTime, hEvent,
                                    pdwAdviseCookie);

    // log results
    fnsLog(CALLLOGLEVEL, TEXT("--- Returned from %s::AdviseTime()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("hRes             == %s (%08Xh)"),
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwAdviseCookie)
    {
	    fnsLog(PARAMLOGLEVEL, TEXT("*pdwAdviseCookie == %08Xh   %s"),
            *pdwAdviseCookie);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIReferenceClock::AdviseTime()


//===========================================================================
// CtIReferenceClock::AdvisePeriodic
//
// Encapsulates calls to AdvisePeriodic
//
// Parameters:
//
// Returns:
//  HRESULT - return value from actual call to AdvisePeriodic
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIReferenceClock::AdvisePeriodic(REFERENCE_TIME startTime,
                                    REFERENCE_TIME periodTime,
                                    HANDLE hSemaphore,
                                    DWORD * pdwAdviseCookie)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pdwAdviseCookie  = TRUE;

    // validate pdwAdviseCookie
    if(!helpIsValidPtr((void*)pdwAdviseCookie, sizeof(DWORD), FALSE))
    {
        fValid_pdwAdviseCookie = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, TEXT("--- Calling %s::AdvisePeriodic()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("startTime       == %016Xh"),
            startTime);
    fnsLog(PARAMLOGLEVEL, TEXT("periodTime      == %016Xh"),
            periodTime);
    fnsLog(PARAMLOGLEVEL, TEXT("hSemaphore      == %08Xh"),
            hSemaphore);
    fnsLog(PARAMLOGLEVEL, TEXT("pdwAdviseCookie == %p   %s"),
            pdwAdviseCookie,
            fValid_pdwAdviseCookie ? TEXT("") : TEXT("BAD"));

    // call the real function
    hRes = ((IReferenceClock*)m_pUnk)->AdvisePeriodic(startTime, periodTime, hSemaphore,
                                    pdwAdviseCookie);

    // log results
    fnsLog(CALLLOGLEVEL, TEXT("--- Returned from %s::AdvisePeriodic()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("hRes             == %s (%08Xh)"),
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwAdviseCookie)
    {
    fnsLog(PARAMLOGLEVEL, TEXT("*pdwAdviseCookie == %08Xh   %s"),
            *pdwAdviseCookie);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIReferenceClock::AdvisePeriodic()


//===========================================================================
// CtIReferenceClock::Unadvise
//
// Encapsulates calls to Unadvise
//
// Parameters:
//
// Returns:
//  HRESULT - return value from actual call to Unadvise
//
// History:
//  11/17/1997 - davidkl - created
//===========================================================================
HRESULT CtIReferenceClock::Unadvise(DWORD dwAdviseCookie)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, TEXT("--- Calling %s::Unadvise()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("dwAdviseCookie == %08Xh"),
            dwAdviseCookie);

    // call the real function
    hRes = ((IReferenceClock*)m_pUnk)->Unadvise(dwAdviseCookie);

    // log results
    fnsLog(CALLLOGLEVEL, TEXT("--- Returned from %s::Unadvise()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("hRes == %s (%08Xh)"),
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIReferenceClock::Unadvise()


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


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================















