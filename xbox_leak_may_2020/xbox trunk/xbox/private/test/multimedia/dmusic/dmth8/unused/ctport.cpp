//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctport.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctport.cpp
//
// Test harness implementation of IDirectMusicPort
//
// Functions:
//  CtIDirectMusicPort::CtIDirectMusicPort()
//  CtIDirectMusicPort::~CtIDirectMusicPort()
//  CtIDirectMusicPort::InitTestClass()
//  CtIDirectMusicPort::GetRealObjPtr()
//  CtIDirectMusicPort::QueryInterface()
//  CtIDirectMusicPort::PlayBuffer()
//  CtIDirectMusicPort::SetReadNotificationHandle()
//  CtIDirectMusicPort::Read()
//  CtIDirectMusicPort::DownloadInstrument()
//  CtIDirectMusicPort::UnloadInstrument()
//  CtIDirectMusicPort::GetLatencyClock()
//  CtIDirectMusicPort::GetRunningStats()
//	CtIDirectMusicPort::SetNumChannelGroups()
//  CtIDirectMusicPort::GetCaps()
//  CtIDirectMusicPort::DeviceIoControl()
//  CtIDirectMusicPort::Compact()
//  CtIDirectMusicPort::GetNumChannelGroups()
//  CtIDirectMusicPort::Activate()
//  CtIDirectMusicPort::SetChannelPriority()
//  CtIDirectMusicPort::GetChannelPriority()
//  CtIDirectMusicPort::SetDirectSound()
//
// History:
//  10/21/1997 - davidkl - created
//  12/23/1997 - a-llucar - Added methods to reflect new dmusic.h
//	01/19/1998 - a-llucar - Updated to latest changes in dmusic.h
//	01/30/1998 - a-llucar - added methods Compact(), GetCaps(), 
//                          GetHandle(), IsBufferFormatSupported()
//  03/03/1998 - davidkl - updated to latest dm arch
//  03/25/1998 - davidkl - renamed file.  added GetNumChannelGroups.
//                         Brave new world... inherits from CtIUnknown
//  03/27/1998 - davidkl - removed property set functions (now part of 
//                         IKsPropertySet.  updated to latest dmusicc.h
//  04/01/1998 - davidkl - minor tweaking
//	08/04/1998 - a-trevg - New Activate method
//	08/20/1998 - a-trevg - New Get-/Set- ChannelPriority methods
//  08/21/1998 - davidkl - removed GetInterfaces() / Added SetDirectSound()
//===========================================================================

#include "dmth.h"
#include "dmthp.h"
#include "dmthcom.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicPort::CtIDirectMusicPort()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/21/1997 - davidkl - created
//===========================================================================
CtIDirectMusicPort::CtIDirectMusicPort()
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicPort";

} //*** end CtIDirectMusicPort::CtIDirectMusicPort()


//===========================================================================
// CtIDirectMusicPort::~CtIDirectMusicPort()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  10/21/1997 - davidkl - created
//===========================================================================
CtIDirectMusicPort::~CtIDirectMusicPort(void)
{
    // nothing to do

} //*** emd CtIDirectMusicPort::~CtIDirectMusicPort()


//===========================================================================
// CtIDirectMusicPort::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicPort object pointer for future use.
//
// Parameters:
//  IDirectMusicPort    *pdmPort    - pointer to real IDirectMusicPort object
//
// Returns: 
//
// History:
//  10/21/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPort::InitTestClass(IDirectMusicPort *pdmPort)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmPort, sizeof(IDirectMusicPort), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmPort));

} //*** end CtIDirectMusicPort::InitTestClass()


//===========================================================================
// CtIDirectMusicPort::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicPort **ppdmPort - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//	10/21/1997 - davidkl - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicPort::GetRealObjPtr(IDirectMusicPort **ppdmPort)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmPort, sizeof(IDirectMusicPort*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmPort));

} //*** end CtIDirectMusicPort::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicPort::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  10/21/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPort::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicPort::QueryInterface()


//===========================================================================
// CtIDirectMusicPort::PlayBuffer
//
// Encapsulates calls to PlayBuffer
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to PlayBuffer
//
// History:
//  10/21/1997 - davidkl - created
//  11/15/1997 - davidkl - implemented test class version
//  03/03/1998 - davidkl - renamed to PlayBuffer
//===========================================================================
HRESULT CtIDirectMusicPort::PlayBuffer(CtIDirectMusicBuffer *ptdmBuf)
{
    HRESULT             hRes            = E_NOTIMPL;
    BOOL                fValid_ptdmBuf  = TRUE;
    IDirectMusicBuffer  *pdmBuffer      = NULL;

    // validate ptdmBuf
    if(!helpIsValidPtr((void*)ptdmBuf, sizeof(CtIDirectMusicBuffer), FALSE))
    {
        fValid_ptdmBuf = FALSE;

        // we have an invalid pointer, use it as the real thing
        pdmBuffer = (IDirectMusicBuffer *)ptdmBuf;
    }
    else
    {
        // we passed in a valid pointer, get the real buffer ptr
        //  that is stored in our test class
        hRes = ptdmBuf->GetRealObjPtr(&pdmBuffer);
        if(FAILED(hRes))
        {
            // BUGBUG
			// add logging
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::PlayBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdmBuffer == %p   %s",
            pdmBuffer,
            fValid_ptdmBuf ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->PlayBuffer(pdmBuffer);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::PlayBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmBuf && pdmBuffer)
    {
        pdmBuffer->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPort::PlayBuffer()


//===========================================================================
// CtIDirectMusicPort::SetReadNotificationHandle
//
// Encapsulates calls to SetReadNotificationHandle
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to SetReadNotificationHandle
//
// History:
//  11/16/1997 - davidkl - created
//  03/03/1998 - davidkl - renamed to SetNotificationHandle
//  03/27/1998 - davidkl - renamed again
//===========================================================================
HRESULT CtIDirectMusicPort::SetReadNotificationHandle(HANDLE hEvent)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetReadNotificationHandle()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hEvent == %08Xh",
            hEvent);

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->SetReadNotificationHandle(hEvent);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetReadNotificationHandle()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;
    
} //*** end CtIDirectMusicPort::SetReadNotificationHandle()


//===========================================================================
// CtIDirectMusicPort::Read
//
// Encapsulates calls to Read
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to Read
//
// History:
//  11/16/1997 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPort::Read(CtIDirectMusicBuffer *ptdmBuf)
{
    HRESULT             hRes            = E_NOTIMPL;
    BOOL                fValid_ptdmBuf  = TRUE;
    IDirectMusicBuffer  *pdmBuffer      = NULL;

    // validate ptdmBuf
    if(!helpIsValidPtr((void*)ptdmBuf, sizeof(CtIDirectMusicBuffer), FALSE))
    {
        fValid_ptdmBuf = FALSE;

        // we have a bogus pointer, use it as the real thing
        pdmBuffer = (IDirectMusicBuffer*)ptdmBuf;
    }
    else
    {
        // we have a valid pointer, get the real one from our test class
        hRes = ptdmBuf->GetRealObjPtr(&pdmBuffer);
        if(FAILED(hRes))
        {   
            // BUGBUG
			// add logging
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Read()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdmBuffer == %p   %s",
            pdmBuffer,
            fValid_ptdmBuf ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->Read(pdmBuffer);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Read()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmBuf && pdmBuffer)
    {
        pdmBuffer->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPort::Read()


//===========================================================================
// CtIDirectMusicPort::DownloadInstrument
//
// Encapsulates calls to DownloadInstrument
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to DownloadInstrument
//
// History:
//  11/17/1997 - davidkl - created
//  03/03/1998 - davidkl - renamed to DownloadInstrument
//  04/01/1998 - davidkl - pay attention to pdmNoteRange iff dwNumRanges != 0
//===========================================================================
HRESULT CtIDirectMusicPort::DownloadInstrument(CtIDirectMusicInstrument *ptdmInst, 
					            CtIDirectMusicDownloadedInstrument **pptdmDlInst,
						        DMUS_NOTERANGE* pdmNoteRange,
						        DWORD dwNumRanges)
{
    HRESULT                             hRes                = E_NOTIMPL;
    BOOL                                fValid_ptdmInst     = TRUE;
    BOOL                                fValid_pptdmDlInst  = TRUE;
    BOOL                                fValid_pdmNoteRange = TRUE;
    IDirectMusicInstrument              *pdmInstrument      = NULL;
    IDirectMusicDownloadedInstrument    *pdmDlInstrument    = NULL;
    IDirectMusicDownloadedInstrument    **ppdmDlInstrument  = NULL;

    // validate ptdmInst
    if(!helpIsValidPtr((void*)ptdmInst, 
                        sizeof(CtIDirectMusicInstrument),
                        FALSE))
    {
        fValid_ptdmInst = FALSE;

        // we have a bogus pointer, use as the real thing   
        pdmInstrument = (IDirectMusicInstrument*)ptdmInst;
    }
    else
    {
        // we have a valid pointer, get the real thing from our test object
        hRes = ptdmInst->GetRealObjPtr(&pdmInstrument);
        if(FAILED(hRes))
        {
            // BUGBUG
			// add logging
            goto END;
        }
    }

    // validate pptdmDlInst
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicDownloadedInstrument pointer, 
    //  we assume that we are to perform an invalid parameter test on 
    //  IDirectMusicPort::Download().  Otherwise, we are to create and
    //  return a CtIDirectMusicDownloadedInstrument object
    if(!helpIsValidPtr((void*)pptdmDlInst,
                        sizeof(CtIDirectMusicDownloadedInstrument*),
                        FALSE))
    { 
        fValid_pptdmDlInst = FALSE;

        // we have a bogus pointer, use as the real thing
        ppdmDlInstrument = (IDirectMusicDownloadedInstrument**)pptdmDlInst;

    }
    else
    {
        // valid pointer, create a real object
        ppdmDlInstrument = &pdmDlInstrument;

        // just in case we fail, init test object ptr to NULL
        *pptdmDlInst = NULL;
    }

    // validate pdmNoteRange iff dwNumRanges != 0
    if(dwNumRanges)
    {
        if(!helpIsValidPtr((void*)pdmNoteRange, sizeof(DMUS_NOTERANGE), FALSE))
        {
            fValid_pdmNoteRange = FALSE;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::DownloadInstrument",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdmInstrument    == %p   %s",
            pdmInstrument,
            fValid_ptdmInst ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppdmDlInstrument == %p   %s",
            ppdmDlInstrument,
            fValid_pptdmDlInst ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdmNoteRange     == %p   %s",
            pdmNoteRange,
            fValid_pdmNoteRange ? "" : "BAD");
    if(fValid_pdmNoteRange && dwNumRanges)
    {
        // BUGBUG - log contents of pdmNoteRange?? (loglevel == 10)
    }
    fnsLog(PARAMLOGLEVEL, "dwNumRanges      == %08Xh",
            dwNumRanges);

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->DownloadInstrument(pdmInstrument,
                                ppdmDlInstrument,
                                pdmNoteRange,
                                dwNumRanges);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::DownloadInstrument",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes              == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmDlInst)
    {
        fnsLog(PARAMLOGLEVEL, "*ppdmDlInstrument == %p",
                pdmDlInstrument);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pdmDlInstrument, pptdmDlInst);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmInst && pdmInstrument)
    {
        pdmInstrument->Release();
    }
    if(fValid_pptdmDlInst && pdmDlInstrument)
    {
        pdmDlInstrument->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPort::DownloadInstrument()


//===========================================================================
// CtIDirectMusicPort::UnloadInstrument
//
// Encapsulates calls to UnloadInstrument
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to Unload
//
// History:
//  11/16/1997 - davidkl - created
//  03/03/1998 - davidkl - renamed to UnloadInstrument
//===========================================================================
HRESULT CtIDirectMusicPort::UnloadInstrument(CtIDirectMusicDownloadedInstrument *ptdmDlInst)
{
    HRESULT                             hRes                = E_NOTIMPL;
    BOOL                                fValid_ptdmDlInst   = TRUE;
    IDirectMusicDownloadedInstrument    *pdmDlInstrument    = NULL;

    // validate ptdmDlInst
    if(!helpIsValidPtr((void*)ptdmDlInst, 
                        sizeof(CtIDirectMusicDownloadedInstrument),
                        FALSE))
    {
        fValid_ptdmDlInst   = FALSE;

        // we have a bogus pointer, use it as the real thing
        pdmDlInstrument = (IDirectMusicDownloadedInstrument*)ptdmDlInst;

    }
    else
    {
        // we have a valid pointer, get the real one from our test object
        hRes = ptdmDlInst->GetRealObjPtr(&pdmDlInstrument);
        if(FAILED(hRes))
        {
            // BUGBUG
			// add logging
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::UnloadInstrument()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdmDlInstrument == %p   %s",
            pdmDlInstrument,
            fValid_ptdmDlInst ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->UnloadInstrument(pdmDlInstrument);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::UnloadInstrument()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmDlInst && pdmDlInstrument)
    {
        pdmDlInstrument->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPort::UnloadInstrument()


//===========================================================================
// CtIDirectMusicPort::GetLatencyClock
//
// Encapsulates calls to GetLatencyClock
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetLatencyClock
//
// History:
//  12/23/1997 - a-llucar - created
//  03/06/1998 - davidkl - now uses wrapper classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicPort::GetLatencyClock(CtIReferenceClock **pptClock)
{
    HRESULT         hRes            = E_NOTIMPL;
    BOOL            fValid_pptClock	= TRUE;
    IReferenceClock *pRefClock      = NULL;
    IReferenceClock **ppRefClock    = NULL;
    
    // validate pptClock
    // NOTE:
    //	If we are passed an invalid CtIReferenceClock pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicPort::GetLatencyClock().  
    //  Otherwise, we are to create and return a CtIReferenceClock object
    if(!helpIsValidPtr((void*)pptClock, sizeof(CtIReferenceClock*), 
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pptClock = FALSE;
        ppRefClock = (IReferenceClock**)pptClock;

    }
    else
    {
        // valid pointer, create a real object
        ppRefClock = &pRefClock;

        // just in case we fail, init test object ptr to NULL
        *pptClock = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetLatencyClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppClock == %p   %s",
            ppRefClock,
            fValid_pptClock ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->GetLatencyClock(ppRefClock);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetLatencyClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptClock)
    {
        fnsLog(PARAMLOGLEVEL, "*ppClock == %p",
            *pRefClock);
    }

    // create the test object
    if(SUCCEEDED(hRes))
    {
        hRes = dmthCreateTestWrappedObject(pRefClock, pptClock);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptClock && pRefClock)
    {
        pRefClock->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPort::GetLatencyClock()


//===========================================================================
// CtIDirectMusicPort::GetRunningStats
//
// Encapsulates calls to GetRunningStats
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetRunningStats
//
// History:
//  12/23/1997 - a-llucar - created
//  03/25/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicPort::GetRunningStats(LPDMUS_SYNTHSTATS pStats)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL    fValid_pStats	= TRUE;
    
    // validate pdwCPUUsagePerVoice
    if(!helpIsValidPtr((void*)pStats, sizeof(DMUS_SYNTHSTATS), FALSE))
    {
        fValid_pStats   = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetRunningStats()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStats == %p   %s",
            pStats,
            fValid_pStats ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->GetRunningStats(pStats);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetRunningStats()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pStats)
    {
        // BUGBUG - log contents of DMUS_SYNTHSTATS
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPort::GetRunningStats()


//===========================================================================
// CtIDirectMusicPort::SetNumChannelGroups
//
// Encapsulates calls to SetNumChannelGroups
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to SetNumChannelGroups
//
// History:
//  12/23/1997 - a-llucar - created
//  03/25/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicPort::SetNumChannelGroups(DWORD dwChannelGroups)
{
    HRESULT hRes    = E_NOTIMPL;
    
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetNumChannelGroups()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroups == %08Xh",
            dwChannelGroups);

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->SetNumChannelGroups(dwChannelGroups);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetNumChannelGroups()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPort::SetNumChannelGroups()


//===========================================================================
// CtIDirectMusicPort::GetCaps
//
// Encapsulates calls to GetCaps
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetCaps
//
// History:
//  12/23/1997 - a-llucar - created
//  03/25/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicPort::GetCaps(LPDMUS_PORTCAPS pPortCaps)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pPortCaps	= TRUE;
    
    // validate pPortCaps
    if(!helpIsValidPtr((void*)pPortCaps, sizeof(DMUS_PORTCAPS), FALSE))
    {
        fValid_pPortCaps   = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetCaps()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPortCaps == %p   %s",
            pPortCaps,
            fValid_pPortCaps ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->GetCaps(pPortCaps);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetCaps()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pPortCaps)
    {
        tdmLogDMUS_PORTCAPS(STRUCTLOGLEVEL, pPortCaps);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPort::GetCaps()


//===========================================================================
// CtIDirectMusicPort::DeviceIoControl
//
// Encapsulates calls to DeviceIoControl
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to DeviceIoControl
//
// History:
//  12/23/1997 - a-llucar - created
//  03/25/1998 - davidkl - fixed logging
//  03/27/1998 - davidkl - renamed, added params
//===========================================================================
HRESULT CtIDirectMusicPort::DeviceIoControl(DWORD dwIoControlCode, 
                                            LPVOID lpInBuffer, 
                                            DWORD nInBufferSize, 
                                            LPVOID lpOutBuffer, 
                                            DWORD nOutBufferSize, 
                                            LPDWORD lpBytesReturned, 
                                            LPOVERLAPPED lpOverlapped)
{
    HRESULT hRes			        = E_NOTIMPL;
    BOOL    fValid_lpInBuffer       = TRUE;
    BOOL    fValid_lpOutBuffer      = TRUE;
    BOOL    fValid_lpBytesReturned  = TRUE;
    BOOL    fValid_lpOverlapped     = TRUE;
    
    // validate lpInBuffer
    if(!helpIsValidPtr((void*)lpInBuffer, nInBufferSize, FALSE))
    {
        fValid_lpInBuffer = FALSE;
    }

    // validate lpOutBuffer
    if(!helpIsValidPtr((void*)lpOutBuffer, nOutBufferSize, FALSE))
    {
        fValid_lpOutBuffer = FALSE;
    }

    // validate lpBytesReturned
    if(!helpIsValidPtr((void*)lpBytesReturned, sizeof(DWORD), FALSE))
    {
        fValid_lpBytesReturned = FALSE;
    }

    // validate lpOverlapped
    if(!helpIsValidPtr((void*)lpOverlapped, sizeof(OVERLAPPED), FALSE))
    {
        fValid_lpOverlapped = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::DeviceIoControl()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwIoControlCode == %08Xh",
            dwIoControlCode);
    fnsLog(PARAMLOGLEVEL, "lpInBuffer      == %p   %s",
            lpInBuffer,
            fValid_lpInBuffer ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "nInBufferSize   == %08Xh",
            nInBufferSize);
    fnsLog(PARAMLOGLEVEL, "lpOutBuffer     == %p   %s",
            lpOutBuffer,
            fValid_lpOutBuffer ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "nOutBufferSize  == %08Xh",
            nOutBufferSize);
    fnsLog(PARAMLOGLEVEL, "lpBytesReturned == %p   %s",
            lpBytesReturned,
            fValid_lpBytesReturned ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "lpOverlapped    == %p   %s",
            lpOverlapped,
            fValid_lpOverlapped ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->DeviceIoControl(dwIoControlCode, 
                                                        lpInBuffer, 
                                                        nInBufferSize, 
                                                        lpOutBuffer, 
                                                        nOutBufferSize, 
                                                        lpBytesReturned, 
                                                        lpOverlapped);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::DeviceIoControl()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPort::DeviceIoControl()


//===========================================================================
// CtIDirectMusicPort::Compact
//
// Encapsulates calls to Compact
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to Compact
//
// History:
//  03/03/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPort::Compact(void)
{
    HRESULT	hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Compact()",
            m_szInterfaceName);

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->Compact();

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Compact()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPort::Compact()


//===========================================================================
// CtIDirectMusicPort::GetNumChannelGroups
//
// Encapsulates calls to GetNumChannelGroups
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetNumChannelGroups
//
// History:
//  03/25/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPort::GetNumChannelGroups(LPDWORD pdwGroups)
{
    HRESULT hRes                = E_NOTIMPL;
    BOOL    fValid_pdwGroups    = TRUE;

    // validate pdwGroups
    if(!helpIsValidPtr((void*)pdwGroups, sizeof(DWORD), FALSE))
    {
        fValid_pdwGroups = FALSE;
    }
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetNumChannelGroups()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwGroups == %p   %s",
            pdwGroups,
            fValid_pdwGroups ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->GetNumChannelGroups(pdwGroups);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetNumChannelGroups()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwGroups)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwGroups == %08Xh",
                *pdwGroups);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPort::GetNumChannelGroups()


//===========================================================================
// CtIDirectMusicPort::Activate
//
// Encapsulates calls to Activate
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to Activate
//
// History:
//  08/04/1998 - a-trevg - created
//  08/21/1998 - davidkl - removed hWnd
//===========================================================================
HRESULT CtIDirectMusicPort::Activate(BOOL fEnable)
{
    HRESULT	hr			= E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Activate()", 
		m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "fEnable == %s", 
		fEnable ? "TRUE" : "FALSE");

    // call the real function
    hr = ((IDirectMusicPort*)m_pUnk)->Activate(fEnable);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Activate()", 
		m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hr          == %s (%08Xh)", 
		tdmXlatHRESULT(hr), hr);
    
    fnsDecrementIndent();

    // done
    return hr;

} //*** end CtIDirectMusicPort::Activate()


//===========================================================================
// CtIDirectMusicPort::SetChannelPriority
//
// Encapsulates calls to SetChannelPriority
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to SetChannelPriority
//
// History:
//  08/20/1998 - a-trevg - created
//===========================================================================
HRESULT CtIDirectMusicPort::SetChannelPriority(DWORD dwChannelGroup, 
											   DWORD dwChannel, 
											   DWORD dwPriority)
{
	HRESULT	hr	= E_NOTIMPL;

	fnsIncrementIndent();

	// log inputs
	fnsLog(CALLLOGLEVEL, "--- Calling %s::SetChannelPriority()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh", dwChannelGroup);
	fnsLog(PARAMLOGLEVEL, "dwChannel == %08Xh", dwChannel);
	fnsLog(PARAMLOGLEVEL, "dwPriority == %08Xh", dwPriority);

	// call the real function
	hr = ((IDirectMusicPort*)m_pUnk)->SetChannelPriority(dwChannelGroup, dwChannel, 
		dwPriority);

	// log results
	fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetChannelPriority()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "hr == %s (%08Xh)", tdmXlatHRESULT(hr), hr);

	fnsDecrementIndent();

	// done
	return hr;

} //*** end CtIDirectMusicPort::SetChannelPriority()


//===========================================================================
// CtIDirectMusicPort::GetChannelPriority
//
// Encapsulates calls to GetChannelPriority
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetChannelPriority
//
// History:
//  08/20/1998 - a-trevg - created
//===========================================================================
HRESULT CtIDirectMusicPort::GetChannelPriority(DWORD dwChannelGroup, 
											   DWORD dwChannel, 
											   LPDWORD pdwPriority)
{
	HRESULT	hr					= E_NOTIMPL;
    BOOL    fValid_pdwPriority	= TRUE;

    // validate pdwGroups
    if(!helpIsValidPtr((void *) pdwPriority, sizeof(DWORD), FALSE))
        fValid_pdwPriority = FALSE;

	fnsIncrementIndent();

	// log inputs
	fnsLog(CALLLOGLEVEL, "--- Calling %s::GetChannelPriority()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh", dwChannelGroup);
	fnsLog(PARAMLOGLEVEL, "dwChannel      == %08Xh", dwChannel);
	fnsLog(PARAMLOGLEVEL, "pdwPriority    == %p", pdwPriority);

	// call the real function
	hr = ((IDirectMusicPort*)m_pUnk)->GetChannelPriority(dwChannelGroup, dwChannel, pdwPriority);

	// log results
	fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetChannelPriority()", m_szInterfaceName);
	fnsLog(PARAMLOGLEVEL, "hr == %s (%08Xh)", tdmXlatHRESULT(hr), hr);

    if(fValid_pdwPriority)
        fnsLog(PARAMLOGLEVEL, "*pdwPriority == %08Xh", *pdwPriority);

	fnsDecrementIndent();

	// done
	return hr;

} //*** end CtIDirectMusicPort::GetChannelPriority()




//===========================================================================
// CtIDirectMusicPort::SetDirectSound
//
// Encapsulates calls to SetDirectSound
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to SetDirectSound
//
// History:
//  08/21/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPort::SetDirectSound(LPDIRECTSOUND pDirectSound, 
                                        LPDIRECTSOUNDBUFFER pDirectSoundBuffer)
{
    HRESULT hRes                        = E_NOTIMPL;
    BOOL    fValid_pDirectSound         = TRUE;
    BOOL    fValid_pDirectSoundBuffer   = TRUE;

    // validate pDirectSound
    if(!helpIsValidPtr((void*)pDirectSound, sizeof(IDirectSound), TRUE))
    {
        // bogus pointer
        fValid_pDirectSound = FALSE;
    }

    // validate pDirectSoundBuffer
    if(!helpIsValidPtr((void*)pDirectSoundBuffer, sizeof(IDirectSoundBuffer), TRUE))
    {
        // bogus pointer
        fValid_pDirectSoundBuffer = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SetDirectSound()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pDirectSound       == %p   %s",
            pDirectSound,
            fValid_pDirectSound ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pDirectSoundBuffer == %p   %s",
            pDirectSoundBuffer,
            fValid_pDirectSoundBuffer ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->SetDirectSound(pDirectSound,
                                                        pDirectSoundBuffer);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetDirectSound()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPort::SetDirectSound()


//===========================================================================
// CtIDirectMusicPort::GetFormat
//
// Encapsulates calls to GetFormat
//
// Parameters:
//
// Returns:
//  HRESULT - return code from actual call to GetFormat
//
// History:
//  08/21/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicPort::GetFormat(LPWAVEFORMATEX pWaveFormatEx,
                                    LPDWORD pdwWaveFormatExSize,
                                    LPDWORD pdwBufferSize)
{
    HRESULT hRes                        = E_NOTIMPL;
    BOOL    fValid_pWaveFormatEx        = TRUE;
    BOOL    fValid_pdwWaveFormatExSize  = TRUE;
    BOOL    fValid_pdwBufferSize        = TRUE;

    // validate pWaveFormatEx
    if(!helpIsValidPtr((void*)pWaveFormatEx, sizeof(LPWAVEFORMATEX), TRUE))
    {
        // bogus pointer
        fValid_pWaveFormatEx = FALSE;
    }

    // validate pdwWaveFormatExSize
    if(!helpIsValidPtr((void*)pdwWaveFormatExSize, sizeof(DWORD), FALSE))
    {
        // bogus pointer
        fValid_pdwWaveFormatExSize = FALSE;
    }

    // validate pdwWaveFormatExSize
    if(!helpIsValidPtr((void*)pdwBufferSize, sizeof(DWORD), FALSE))
    {
        // bogus pointer
        fValid_pdwBufferSize = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetFormat()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pWaveFormatEx       == %p   %s",
            pWaveFormatEx,
            fValid_pWaveFormatEx ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdwWaveFormatExSize == %p   %s",
            pdwWaveFormatExSize,
            fValid_pdwWaveFormatExSize ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdwBufferSize       == %p   %s",
            pdwBufferSize,
            fValid_pdwBufferSize ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPort*)m_pUnk)->GetFormat(pWaveFormatEx,
                                                  pdwWaveFormatExSize,
                                                  pdwBufferSize);

    //Log contents of WAVEFORMATEX, don't care about anything extra, since we don't know what that might be. -jimmo
    if (SUCCEEDED( hRes ))
    {
        if (NULL != pWaveFormatEx)
        {
            fnsLog(PARAMLOGLEVEL, "========= WFX Contents From GetFormat() call  ============");
            fnsLog(PARAMLOGLEVEL, "    WFX.wFormatTag      == %08Xh ",  pWaveFormatEx->wFormatTag);
            fnsLog(PARAMLOGLEVEL, "    WFX.nChannels       == %08Xh ",  pWaveFormatEx->nChannels);
            fnsLog(PARAMLOGLEVEL, "    WFX.nSamplesPerSec  == %08Xh ",  pWaveFormatEx->nSamplesPerSec);
            fnsLog(PARAMLOGLEVEL, "    WFX.nAvgBytesPerSec == %08Xh ",  pWaveFormatEx->nAvgBytesPerSec);
            fnsLog(PARAMLOGLEVEL, "    WFX.nBlockAlign     == %08Xh ",  pWaveFormatEx->nBlockAlign);
            fnsLog(PARAMLOGLEVEL, "    WFX.wBitsPerSample  == %08Xh ",  pWaveFormatEx->wBitsPerSample);
            fnsLog(PARAMLOGLEVEL, "    WFX.cbSize          == %08Xh ",  pWaveFormatEx->cbSize);
        }
    }

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetFormat()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPort::GetFormat()


//===========================================================================
//===========================================================================












 












