//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 2000
//
//  File:       ctdmusi8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctdmusi8.cpp
//
// Test harness implementation of IDirectMusic8
//
// Functions:
//  CtIDirectMusic::CtIDirectMusic8()
//  CtIDirectMusic::~CtIDirectMusic8()
//  CtIDirectMusic::InitTestClass()
//  CtIDirectMusic::GetRealObjPtr()
//  CtIDirectMusic::QueryInterface()
//
// History:
//  10/15/1997 - davidkl - created
//  05/01/2000 - kcraven - converted to dx8
//===========================================================================

#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusic::CtIDirectMusic8()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  05/01/2000 - kcraven - created
//===========================================================================
CtIDirectMusic8::CtIDirectMusic8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusic8";

} // ** emd CtIDirectMusic8::CtIDirectMusic8()


//===========================================================================
// CtIDirectMusic8::~CtIDirectMusic8()
//
// Default destructor
//
// Parameters: none
//
// Returns:
//
// History:
//  05/01/2000 - kcraven - created
//===========================================================================
CtIDirectMusic8::~CtIDirectMusic8(void)
{
    // nothing to do

} // ** end CtIDirectMusic8::~CtIDirectMusic8()


//===========================================================================
// CtIDirectMusic8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusic8 object pointer for future use.
//
// Parameters:
//  IDirectMusic8    *pdm    - pointer to real IDirectMusic8 object
//
// Returns:
//
// History:
//  05/01/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusic8::InitTestClass(IDirectMusic8 *pdm)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdm, sizeof(IDirectMusic8), FALSE))
	{
        m_pUnk = NULL;
        return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdm));

} // ** end CtIDirectMusic8::InitTestClass()


//===========================================================================
// CtIDirectMusic8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusic8 **ppdm - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  05/01/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusic8::GetRealObjPtr(IDirectMusic8 **ppdm)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdm, sizeof(IDirectMusic8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdm));

} // ** end CtIDirectMusic8::GetRealObjPtr()


//===========================================================================
// CtIDirectMusic8::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  05/01/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusic8::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hRes        = E_NOTIMPL;
    LPVOID  *ppvTemp    = NULL;
    BOOL    fMatchFound = FALSE;

    // use the base class to perform the actual QI
    hRes = (CtIUnknown::QueryInterface(riid, ppvObj));

    // done
    return hRes;

} // ** end CtIDirectMusic8::QueryInterface()






//===========================================================================
// CtIDirectMusic8::SetExternalMasterClock
//
// Encapsulates calls to IDirectMusic8::SetExternalMasterClock
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real SetExternalMasterClock
//
// History:
//  05/01/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusic8::SetExternalMasterClock
(
	IReferenceClock* pClock
)
{
    HRESULT         hRes            = E_NOTIMPL;
	BOOL	        fValid_pClock	= TRUE;

    // validate pClock
    if(!helpIsValidPtr((void*)pClock, sizeof(IReferenceClock), FALSE))
    {
        fValid_pClock = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetExternalMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pClock == %p   %s",
            pClock,
            fValid_pClock ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusic8*)m_pUnk)->SetExternalMasterClock(pClock);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetExternalMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done

    return hRes;

} // ** end CtIDirectMusic8::SetExternalMasterClock()






//===========================================================================
//===========================================================================
