//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctdmusic.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctdmusic.cpp
//
// Test harness implementation of IDirectMusic
//
// Functions:
//  CtIDirectMusic::CtIDirectMusic()
//  CtIDirectMusic::~CtIDirectMusic()
//  CtIDirectMusic::InitTestClass()
//  CtIDirectMusic::GetRealObjPtr()
//  CtIDirectMusic::QueryInterface()
//  CtIDirectMusic::EnumPort()
//  CtIDirectMusic::CreateMusicBuffer()
//  CtIDirectMusic::CreatePort()
//  CtIDirectMusic::EnumMasterClock()
//  CtIDirectMusic::GetMasterClock()
//  CtIDirectMusic::SetMasterClock()
//  CtIDirectMusic::Activate()
//  CtIDirectMusic::GetDefaultPort()
//  CtIDirectMusic::SetDirectSound
//
// History:
//  10/15/1997 - davidkl - created
//  12/22/1997 - bthomas and a-llucar - added methods to CtIDirectMusic
//  12/23/1997 - a-llucar - added methods to CtIDirectMusic
//	01/19/1998 - a-llucar - latest Core changes fixed
//  02/25/1998 - davidkl - removed AllocateEventClass and FreeEventClass.
//                          added CreatePort
//  03/02/1998 - davidkl - ported to latest dmusic architecture changes
//  04/01/1998 - davidkl - a little maintenance
//	07/22/1998 - a-trevg - removed GetPortProperty
//  08/21/1998 - davidkl - added SetDirectSound()
//===========================================================================

#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusic::CtIDirectMusic()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  10/15/1997 - davidkl - created
//  03/18/1998 - davidkl - now inherits from CtIUnknown
//===========================================================================
CtIDirectMusic::CtIDirectMusic(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusic";

} //*** emd CtIDirectMusic::CtIDirectMusic()


//===========================================================================
// CtIDirectMusic::~CtIDirectMusic()
//
// Default destructor
//
// Parameters: none
//
// Returns:
//
// History:
//  10/15/1997 - davidkl - created
//===========================================================================
CtIDirectMusic::~CtIDirectMusic(void)
{
    // nothing to do

} //*** end CtIDirectMusic::~CtIDirectMusic()


//===========================================================================
// CtIDirectMusic::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusic object pointer for future use.
//
// Parameters:
//  IDirectMusic    *pdm    - pointer to real IDirectMusic object
//
// Returns:
//
// History:
//  10/16/1997 - davidkl - created
//  03/18/1998 - davidkl - reimplemented as overloaded version
//===========================================================================
HRESULT CtIDirectMusic::InitTestClass(IDirectMusic *pdm)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdm, sizeof(IDirectMusic), FALSE))
	{
        m_pUnk = NULL;
        return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdm));

} //*** end CtIDirectMusic::InitTestClass()


//===========================================================================
// CtIDirectMusic::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusic **ppdm - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//	10/16/1997 - davidkl - created
//  10/17/1997 - davidkl - cleaned up
//  1/15/98 - bthomas - made it more COM-like
//  03/18/1998 - davidkl - reimplemented as overloaded version
//===========================================================================
HRESULT CtIDirectMusic::GetRealObjPtr(IDirectMusic **ppdm)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdm, sizeof(IDirectMusic*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdm));

} //*** end CtIDirectMusic::GetRealObjPtr()


//===========================================================================
// CtIDirectMusic::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  10/16/1997 - davidkl - created
//  03/18/1998 - davidkl - reimplemented as replacement version
//===========================================================================
HRESULT CtIDirectMusic::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusic::QueryInterface()


//===========================================================================
// CtIDirectMusic::EnumPort
//
// Encapsulates calls to IDirectMusic::EnumPort
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real EnumPort
//
// History:
//  10/16/1997 - davidkl - created
//  12/04/1997 - davidkl - realized I forgot to implement this
//  12/23/1997 - a-llucar - updating to latest dmusic.h
//  03/18/1998 - davidkl - now inherits from CtIUnknown
//  04/01/1998 - davidkl - fixed param validation bug
//===========================================================================
HRESULT CtIDirectMusic::EnumPort(DWORD dwIdx, LPDMUS_PORTCAPS lpPortCaps)
{
    HRESULT                 hRes                    = E_NOTIMPL;
	BOOL					fValid_lpPortCaps		= TRUE;

    // validate lpPortCaps
    if(!helpIsValidPtr(lpPortCaps, sizeof(DMUS_PORTCAPS), FALSE))
    {
        fValid_lpPortCaps = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumPort()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIdx      == %08Xh",
            dwIdx);
    fnsLog(PARAMLOGLEVEL, "lpPortCaps == %p   %s",
            lpPortCaps,
            fValid_lpPortCaps ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->EnumPort(dwIdx, lpPortCaps);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumPort()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes        == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_lpPortCaps)
    {
        tdmLogDMUS_PORTCAPS(STRUCTLOGLEVEL, lpPortCaps);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusic::EnumPort()


//===========================================================================
// CtIDirectMusic::CreateMusicBuffer
//
// Encapsulates calls to IDirectMusic::CreateMusicBuffer
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real CreateMusicBuffer
//
// History:
//  10/16/1997 - davidkl - created
//  03/18/1998 - davidkl - now inherits from CtIUnknown
//  03/25/1998 - davidkl - fixed refcounting bug, simplified obj creation
//===========================================================================
HRESULT CtIDirectMusic::CreateMusicBuffer(LPDMUS_BUFFERDESC pBufferDesc,
                                    CtIDirectMusicBuffer **pptdmBuffer,
                                    IUnknown *punk)
{
    HRESULT             hRes                = E_NOTIMPL;
    IDirectMusicBuffer  *pdmBuf             = NULL;
    IDirectMusicBuffer  **ppdmBuf           = NULL;
    BOOL                fValid_punk         = TRUE;
    BOOL                fValid_pptdmBuffer  = TRUE;
	BOOL				fValid_pBufferDesc	= TRUE;

	// validate pBufferDesc
    if(!helpIsValidPtr(pBufferDesc, sizeof(DMUS_BUFFERDESC), FALSE))
    {
        fValid_pBufferDesc = FALSE;
    }

	// validate punk
    // NOTE: NULL is valid for this parameter.  It indicates that aggregation
    //  is not desired.
    if(!helpIsValidPtr(punk, sizeof(IUnknown), TRUE))
    {
        fValid_punk = FALSE;
    }

    // validate pptdmBuffer
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicBuffer pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusic::CreateMusicBuffer().  Otherwise, we are to create and
    //  return a CtIDirectMusicBuffer object
    if(!helpIsValidPtr((void*)pptdmBuffer, sizeof(CtIDirectMusicBuffer*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmBuffer = FALSE;
        ppdmBuf = (IDirectMusicBuffer**)pptdmBuffer;

    }
    else
    {
        // valid pointer, create a real object
        ppdmBuf = &pdmBuf;

        // just in case we fail, init test object ptr to NULL
        *pptdmBuffer = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::CreateMusicBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pBufferDesc == %p   %s",
            pBufferDesc,
			fValid_pBufferDesc ? "" : "BAD");
    // BUGBUG log pBufferDesc contents (if valid) @ level MAXLOGLEVEL
    fnsLog(PARAMLOGLEVEL, "ppdmBuffer  == %p   %s",
            ppdmBuf,
            fValid_pptdmBuffer ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "punk        == %p   %s",
            punk,
            fValid_punk ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->CreateMusicBuffer(pBufferDesc, ppdmBuf, punk);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::CreateMusicBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes        == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmBuffer)
    {
        fnsLog(PARAMLOGLEVEL, "*ppdmBuffer == %p",
                pdmBuf);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmBuf, pptdmBuffer);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmBuffer && pdmBuf)
    {
        pdmBuf->Release();
    }
    return hRes;

} //*** end CtIDirectMusic::CreateMusicBuffer()


//===========================================================================
// CtIDirectMusic::CreatePort
//
// Encapsulates calls to IDirectMusic::CreatePort
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real CreatePort
//
// History:
//  02/25/1998 - davidkl - created
//  03/18/1998 - davidkl - now inherits from CtIUnknown
//  03/25/1998 - davidkl - fixed refcounting bug, simplified obj creation
//  04/01/1998 - davidkl - fixed param validation, enhanced GUID logging
//  08/21/1998 - davidkl - removed rguidSink
//===========================================================================
HRESULT CtIDirectMusic::CreatePort(REFGUID rguidPort,
                                   LPDMUS_PORTPARAMS pPortParams,
                                   CtIDirectMusicPort **pptdmPort,
                                   IUnknown *punk)
{
	HRESULT				hRes						= E_NOTIMPL;
	IDirectMusicPort    *pdmPort					= NULL;
    IDirectMusicPort    **ppdmPort					= NULL;
    BOOL                fValid_punk					= TRUE;
    BOOL                fValid_pPortParams			= TRUE;
	BOOL				fValid_pptdmPort			= TRUE;
    char                szguidPort[MAX_LOGSTRING];
	HRESULT				hResCreate					= E_FAIL;

	// validate pPortParams
    // NOTE: NULL is ok here... be careful later in the fn!
    if(!helpIsValidPtr(pPortParams, sizeof(DMUS_PORTPARAMS), TRUE))
    {
        fValid_pPortParams = FALSE;
    }

	// validate punk
    if(!helpIsValidPtr(punk, sizeof(IUnknown), TRUE))
    {
        fValid_punk = FALSE;
    }

    // validate pptdmPort
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicPort pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusic::CreatePort().  Otherwise, we are to create and
    //  return a CtIDirectMusic object
    if(!helpIsValidPtr((void*)pptdmPort, sizeof(CtIDirectMusicPort*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmPort = FALSE;
        ppdmPort = (IDirectMusicPort**)pptdmPort;

    }
    else
    {
        // valid pointer, create a real object
        ppdmPort = &pdmPort;

        // just in case we fail, init test object ptr to NULL
        *pptdmPort = NULL;
    }

    // convert GUIDs to readable strings
    tdmGUIDtoString(rguidPort, szguidPort);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::CreatePort()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidPort   == %s (%s)",
            szguidPort, tdmXlatGUID(rguidPort));
    fnsLog(PARAMLOGLEVEL, "pPortParams == %p   %s",
            pPortParams,
            fValid_pPortParams ? "" : "BAD");
    if(fValid_pPortParams && pPortParams)
    {
        // log pPortParams contents
        tdmLogDMUS_PORTPARAMS(STRUCTLOGLEVEL, pPortParams);
    }
    fnsLog(PARAMLOGLEVEL, "ppdmPort    == %p   %s",
            ppdmPort,
            fValid_pptdmPort ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "punk        == %p   %s",
            punk,
            fValid_punk ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->CreatePort(rguidPort, 
                                            pPortParams, 
                                            ppdmPort, 
                                            punk);

    // log results
    fnsLog(PARAMLOGLEVEL, "--- Returned from %s::CreatePort()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes        == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmPort)
    {
        fnsLog(CALLLOGLEVEL, "*ppdmPort == %p",
                pdmPort);
    }
    if(fValid_pPortParams && pPortParams)
    {
        // log pPortParams contents
        tdmLogDMUS_PORTPARAMS(STRUCTLOGLEVEL, pPortParams);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hResCreate = dmthCreateTestWrappedObject(pdmPort, pptdmPort);
        if(FAILED(hResCreate))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hResCreate);
			hRes = hResCreate;
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmPort && pdmPort)
    {
        pdmPort->Release();
    }
    return hRes;

} //*** end CtIDirectMusic::CreatePort()




//===========================================================================
// CtIDirectMusic::EnumMasterClock
//
// Encapsulates calls to IDirectMusic::EnumMasterClock
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real EnumMasterClock
//
// History:
//  12/22/1997 - bthomas and a-llucar - created
//  03/18/1998 - davidkl - now inherits from CtIUnknown
//  04/01/1998 - davidkl - fixed param validation
//===========================================================================
HRESULT CtIDirectMusic::EnumMasterClock(DWORD dwIdx, LPDMUS_CLOCKINFO lpClockCaps)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_lpClockCaps		= TRUE;

    // validate lpClockCaps
    if(!helpIsValidPtr(lpClockCaps, sizeof(DMUS_CLOCKINFO), FALSE))
    {
        fValid_lpClockCaps = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::EnumMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIdx       == %08Xh",
            dwIdx);
    fnsLog(PARAMLOGLEVEL, "lpClockCaps == %p",
            lpClockCaps);

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->EnumMasterClock(dwIdx, lpClockCaps);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::EnumMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_lpClockCaps)
    {
        // BUGBUG log lpClockCaps contents @ level STRUCTLOGLEVEL
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusic::EnumMasterClock()


//===========================================================================
// CtIDirectMusic::GetMasterClock
//
// Encapsulates calls to IDirectMusic::GetMasterClock
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real GetMasterClock
//
// History:
//  12/23/1997 - a-llucar - created
//  03/06/1998 - now uses wrapper classes, enhanced logging
//  03/18/1998 - davidkl - now inherits from CtIUnknown
//  04/01/1998 - davidkl - enhanced GUID logging
//===========================================================================
HRESULT CtIDirectMusic::GetMasterClock(GUID *guidClock,
									   CtIReferenceClock **pptReferenceClock)
{
    HRESULT         hRes                        = E_NOTIMPL;
    BOOL            fValid_guidClock		    = TRUE;
	BOOL	        fValid_pptReferenceClock	= TRUE;
    char            szGuid[MAX_LOGSTRING];
    IReferenceClock *pRefClock                  = NULL;
    IReferenceClock **ppRefClock                = NULL;

    // validate guidClock
    // guidClock is allowed to be NULL! be careful here!!
    if(!helpIsValidPtr(guidClock, sizeof(GUID), TRUE))
    {
        fValid_guidClock = FALSE;
    }

    // validate pptReferenceClock
    // NOTE:
    //	If we are passed an invalid CtIReferenceClock pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusic:GetMasterClock().  
    //  Otherwise, we are to create and return a CtIReferenceClock object
    if(!helpIsValidPtr((void*)pptReferenceClock, sizeof(CtIReferenceClock*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pptReferenceClock = FALSE;
        ppRefClock = (IReferenceClock**)pptReferenceClock;

    }
    else
    {
        // valid pointer, create a real object
        ppRefClock = &pRefClock;

        // just in case we fail, init test object ptr to NULL
        *pptReferenceClock = NULL;
    }

	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "guidClock        == %p   %s",
            // NOTE: guidClock is really a *
            guidClock,
            fValid_guidClock ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppReferenceClock == %p   %s",
            ppRefClock,
            (fValid_pptReferenceClock) ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->GetMasterClock(guidClock, ppRefClock);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes              == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(SUCCEEDED(hRes))
    {
        if(fValid_guidClock && guidClock) 
        {
            tdmGUIDtoString(*guidClock, szGuid);
            fnsLog(PARAMLOGLEVEL, "*guidClock        == %s (%s)",
                szGuid, tdmXlatGUID(*guidClock));
        }
        if(fValid_pptReferenceClock)
        {
            fnsLog(PARAMLOGLEVEL, "*ppReferenceClock == %p",
                *pRefClock);
        }
    }

    // create the test object
    if(SUCCEEDED(hRes) && fValid_pptReferenceClock)
    {
        hRes = dmthCreateTestWrappedObject(pRefClock, pptReferenceClock);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptReferenceClock && pRefClock)
    {
        pRefClock->Release();

    }
    return hRes;

} //*** end CtIDirectMusic::GetMasterClock()


//===========================================================================
// CtIDirectMusic::SetMasterClock
//
// Encapsulates calls to IDirectMusic::SetMasterClock
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real SetMasterClock
//
// History:
//  12/23/1997 - a-llucar - created
//  03/18/1998 - davidkl - now inherits from CtIUnknown
//  04/01/1998 - davidkl - enhanced GUID logging
//===========================================================================
HRESULT CtIDirectMusic::SetMasterClock(REFGUID guidClock)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_guidClock		= TRUE;
    char    szGuid[MAX_LOGSTRING];

	tdmGUIDtoString(guidClock, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "guidClock == %s (%s)",
            szGuid, tdmXlatGUID(guidClock));

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->SetMasterClock(guidClock);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusic::SetMasterClock()


//===========================================================================
// CtIDirectMusic::Activate
//
// Encapsulates calls to IDirectMusic::Activate
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real Activate
//
// History:
//  12/23/1997 - a-llucar - created
//  03/18/1998 - davidkl - now inherits from CtIUnknown
//  04/01/1998 - davidkl - enhanced logging
//  08/21/1998 - davidkl - removed hWnd
//===========================================================================
HRESULT CtIDirectMusic::Activate(BOOL fEnable)
{
    HRESULT hRes                    = E_NOTIMPL;

	fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Activate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "fEnable == %s",
            fEnable ? "TRUE" : "FALSE");

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->Activate(fEnable);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Activate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusic::Activate()


//===========================================================================
// CtIDirectMusic::GetDefaultPort
//
// Encapsulates calls to IDirectMusic::GetDefaultPort
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real GetDefaultPort
//
// History:
//  03/27/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusic::GetDefaultPort(GUID *pguidPort)
{
    HRESULT hRes                = E_NOTIMPL;
    BOOL    fValid_pguidPort    = TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pguidPort
    if(!helpIsValidPtr((void*)pguidPort, sizeof(GUID), FALSE))
    {
        fValid_pguidPort = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetDefaultPort()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pguidPort == %p   %s",
            pguidPort,
            fValid_pguidPort ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->GetDefaultPort(pguidPort);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetDefaultPort()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pguidPort)
    {
        tdmGUIDtoString(*pguidPort, szGuid);
        fnsLog(PARAMLOGLEVEL, "*pguidPort == %s (%s)",
                szGuid, tdmXlatGUID(*pguidPort));
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusic::GetDefaultPort()



//===========================================================================
// CtIDirectMusic::SetDirectSound
//
// Encapsulates calls to IDirectMusic::SetDirectSound
//
// Parameters:
//
// Returns:
//  HRESULT - return code from the real SetDirectSound
//
// History:
//  08/21/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusic::SetDirectSound(LPDIRECTSOUND pDirectSound,
                                        HWND hWnd)
{
    HRESULT hRes                = E_NOTIMPL;
    BOOL    fValid_pDirectSound =TRUE;

    // validate lpPortCaps
    if(!helpIsValidPtr(pDirectSound, sizeof(IDirectSound), TRUE))
    {
        fValid_pDirectSound = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetDirectSound()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pDirectSound == %p   %s",
            pDirectSound,
            fValid_pDirectSound ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "hWnd         == %08Xh",
            hWnd);

    // call the real function
    hRes = ((IDirectMusic*)m_pUnk)->SetDirectSound(pDirectSound, hWnd);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetDirectSound()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes        == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusic::SetDirectSound()


//===========================================================================
//===========================================================================
