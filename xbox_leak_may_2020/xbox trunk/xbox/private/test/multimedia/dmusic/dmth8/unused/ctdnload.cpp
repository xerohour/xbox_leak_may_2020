//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctdnload.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctdnload.cpp
//
// Test harness implementation of IDirectMusicDownload
//
// Functions:
//  CtIDirectMusicDownload::CtIDirectMusicDownload()
//  CtIDirectMusicDownload::~CtIDirectMusicDownload()
//  CtIDirectMusicDownload::InitTestClass()
//  CtIDirectMusicDownload::GetRealObjPtr()
//  CtIDirectMusicDownload::GetId()
//
// History:
//  01/13/1998 - a-llucar - created
//  01/13/1998 - a-llucar - removed GetBuffer()
//  03/30/1998 - davidkl - brave new world of inheritance
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicDownload::CtIDirectMusicDownload()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/13/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicDownload::CtIDirectMusicDownload()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicDownload";

} //*** end CtIDirectMusicDownload::CtIDirectMusicDownload()


//===========================================================================
// CtIDirectMusicDownload::~CtIDirectMusicDownload()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/13/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicDownload::~CtIDirectMusicDownload()
{
    // nothing to do

} //*** emd CtIDirectMusicDownload::~CtIDirectMusicDownload()


//===========================================================================
// CtIDirectMusicDownload::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicDownload object pointer for future use.
//
// Parameters:
//  IDirectMusicDownload *pdmDownload - pointer to real object
//
// Returns: 
//
// History:
//  01/13/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicDownload::InitTestClass(IDirectMusicDownload *pdmDownload)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmDownload, sizeof(IDirectMusicDownload), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmDownload));

} //*** end CtIDirectMusicDownload::InitTestClass()


//===========================================================================
// CtIDirectMusicDownload::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicDownload **ppdmDownload - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//	01/13/1998 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicDownload::GetRealObjPtr(IDirectMusicDownload **ppdmDownload)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmDownload, sizeof(IDirectMusicDownload*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmDownload));

} //*** end CtIDirectMusicDownload::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicDownload::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//  HRESULT - return value from actual call to QueryInterface
//
// History:
//  01/13/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicDownload::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicDownload::QueryInterface()


//===========================================================================
// CtIDirectMusicDownload::GetBuffer
//
// Encapsulates calls to GetBuffer
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetBuffer
//
// History:
//  01/13/1998 - a-llucar - created
//  03/03/1998 - davidkl - added parameter validation and fixed logging bugs
//===========================================================================
HRESULT CtIDirectMusicDownload::GetBuffer(void** ppvBuffer, DWORD* dwSize)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_ppvBuffer    = TRUE;
    BOOL    fValid_dwSize		= TRUE;

    // validate dwSize
    if(!helpIsValidPtr(ppvBuffer, 1, FALSE))
    {
        fValid_ppvBuffer = FALSE;
    }

    // validate dwSize
    if(!helpIsValidPtr(dwSize, sizeof(DWORD), FALSE))
    {
        fValid_dwSize = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppvBuffer == %p   %s",
            ppvBuffer,
            (fValid_ppvBuffer) ? "" : "BAD");
    // NOTE: dwSize is really a *
    fnsLog(PARAMLOGLEVEL, "dwSize    == %08Xh   %s",
            dwSize,
            fValid_dwSize ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicDownload*)m_pUnk)->GetBuffer(ppvBuffer, dwSize);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes   == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppvBuffer)
    {
        // BUGBUG log returned ppvBuffer data (@ MAXLOGLEVEL)
        // in banks of bytes like so:
        // 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    }
    if(fValid_dwSize)
    {
        // NOTE: dwSize is really a *
        fnsLog(PARAMLOGLEVEL, "*dwSize == %08Xh",
                *dwSize);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicDownload::GetBuffer()




//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================








