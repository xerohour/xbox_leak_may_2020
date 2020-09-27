//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctinstr.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctinstr.cpp
//
// Test harness implementation of IDirectMusicInstrument
//
// Functions:
//  CtIDirectMusicInstrument::CtIDirectMusicInstrument()
//  CtIDirectMusicInstrument::~CtIDirectMusicInstrument()
//  CtIDirectMusicInstrument::InitTestClass()
//  CtIDirectMusicInstrument::GetRealObjPtr()
//  CtIDirectMusicInstrument::GetPatch()
//  CtIDirectMusicInstrument::SetPatch()
//
// History:
//  10/24/1997 - davidkl - created
//  03/30/1998 - davidkl - brave new world of inheritance
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicInstrument::CtIDirectMusicInstrument()
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
CtIDirectMusicInstrument::CtIDirectMusicInstrument()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicInstrument";

} //*** end CtIDirectMusicInstrument::CtIDirectMusicInstrument()


//===========================================================================
// CtIDirectMusicInstrument::~CtIDirectMusicInstrument()
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
CtIDirectMusicInstrument::~CtIDirectMusicInstrument()
{
    // nothing to do

} //*** emd CtIDirectMusicInstrument::~CtIDirectMusicInstrument()


//===========================================================================
// CtIDirectMusicInstrument::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicInstrument object pointer for future use.
//
// Parameters:
//  IDirectMusicInstrument *pdmInstr - pointer to real object
//
// Returns: 
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicInstrument::InitTestClass(IDirectMusicInstrument *pdmInstr)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmInstr, sizeof(IDirectMusicInstrument), FALSE))
	{
        m_pUnk = NULL;
        return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmInstr));

} //*** end CtIDirectMusicInstrument::InitTestClass()


//===========================================================================
// CtIDirectMusicInstrument::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicInstrument **ppdmInstr - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//	10/24/1997 - davidkl - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicInstrument::GetRealObjPtr(IDirectMusicInstrument **ppdmInstr)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmInstr, sizeof(IDirectMusicInstrument*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmInstr));

} //*** end CtIDirectMusicInstrument::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicInstrument::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//  HRESULT - return value from actual call to QueryInterface
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicInstrument::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicInstrument::QueryInterface()


//===========================================================================
// CtIDirectMusicInstrument::GetPatch
//
// Encapsulates calls to GetPatch
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetPatch
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicInstrument::GetPatch(DWORD* pdwPatch)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_pdwPatch = TRUE;

    // validate pdwPatch
    if(!helpIsValidPtr(pdwPatch, sizeof(DWORD), FALSE))
    {
        fValid_pdwPatch = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetPatch()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwPatch == %p   %s",
            pdwPatch,
            fValid_pdwPatch ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicInstrument*)m_pUnk)->GetPatch(pdwPatch);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetPatch()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes      == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwPatch)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwPatch == %08Xh",
                *pdwPatch);
    }       

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicInstrument::GetPatch()


//===========================================================================
// CtIDirectMusicInstrument::SetPatch
//
// Encapsulates calls to SetPatch
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to SetPatch
//
// History:
//  10/24/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicInstrument::SetPatch(DWORD dwPatch)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetPatch()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwPatch == %08Xh",
            dwPatch);

    // call the real function
    hRes = ((IDirectMusicInstrument*)m_pUnk)->SetPatch(dwPatch);

    // log results
    fnsLog(PARAMLOGLEVEL, "--- Returned from %s::SetPatch()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicInstrument::SetPatch()


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================








