//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       ctsynth8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctsynth8.cpp
//
// Test harness implementation of IDirectMusicSynth8
//
// Functions:
//  CtIDirectMusicSynth8::CtIDirectMusicSynth8()
//  CtIDirectMusicSynth8::~CtIDirectMusicSynth8()
//  CtIDirectMusicSynth8::InitTestClass()
//  CtIDirectMusicSynth8::GetRealObjPtr()
//  CtIDirectMusicSynth8::QueryInterface()
//  CtIDirectMusicSynth8::Open()
//  CtIDirectMusicSynth8::Close()
//  CtIDirectMusicSynth8::SetNumChannelGroups()
//  CtIDirectMusicSynth8::Download()
//  CtIDirectMusicSynth8::Unload()
//  CtIDirectMusicSynth8::PlayBuffer()
//  CtIDirectMusicSynth8::GetRunningStats()
//  CtIDirectMusicSynth8::GetPortCaps()
//  CtIDirectMusicSynth8::SetMasterClock()
//  CtIDirectMusicSynth8::GetLatencyClock()
//  CtIDirectMusicSynth8::Activate()
//  CtIDirectMusicSynth8::Render()
//  CtIDirectMusicSynth8::SetSynthSink()
// DX8
//  CtIDirectMusicSynth8::PlayVoice()
//  CtIDirectMusicSynth8::StopVoice()
//  CtIDirectMusicSynth8::GetVoicePosition()
//  CtIDirectMusicSynth8::Refresh()
//  CtIDirectMusicSynth8::AssignChannelToBuses()

// History:
//  10/18/1999 - kcraven - created
//===========================================================================


#include "dmth.h"
#include "dmthp.h"


//===========================================================================
// CtIDirectMusicSynth8::CtIDirectMusicSynth8()
//
// Default constructor
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSynth8::CtIDirectMusicSynth8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicSynth8";

} // *** end CtIDirectMusicSynth8::CtIDirectMusicSynth8()


//===========================================================================
// CtIDirectMusicSynth8::~CtIDirectMusicSynth8()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSynth8::~CtIDirectMusicSynth8(void)
{
    // nothing to do

} // *** end CtIDirectMusicSynth8::~CtIDirectMusicSynth8()


//===========================================================================
// CtIDirectMusicSynth8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmSynth - pointer to real
//                                      IDirectMusic object
//
// Returns:
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::InitTestClass(IDirectMusicSynth8 *pdmSynth8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmSynth8, sizeof(IDirectMusicSynth8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmSynth8));

} // *** end CtIDirectMusicSynth8::InitTestClass()


//===========================================================================
// CtIDirectMusicSynth8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicSynth8 **ppdmSynth - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::GetRealObjPtr(IDirectMusicSynth8 **ppdmSynth8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmSynth8, sizeof(IDirectMusicSynth8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmSynth8));

} // *** end CtIDirectMusicSynth8::GetRealObjPtr()

/*

//===========================================================================
// CtIDirectMusicSynth8::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::QueryInterface(REFIID riid,
												LPVOID *ppvObj)
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

} // *** end CtIDirectMusicSynth8::QueryInterface


//===========================================================================
// CtIDirectMusicSynth8::Open()
//
// Encapsulates calls to Open()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::Open(LPDMUS_PORTPARAMS pPortParams)
{
    HRESULT             hRes                = E_NOTIMPL;
	BOOL				fValid_pPortParams	= TRUE;

    // validate pPortParams
    if(!helpIsValidPtr((void*)pPortParams, sizeof(DMUS_PORTPARAMS), TRUE))
    {
        fValid_pPortParams = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Open()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPortParams == %08Xh   %s",
            pPortParams,
            fValid_pPortParams ? "" : "BAD");
    if(fValid_pPortParams)
    {
        if(!pPortParams)
        {
	        // BUGBUG
            // log default port parameters
        }
        else
        {
	        // BUGBUG
            // log actual port parameters passed in
        }
    }

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->Open(pPortParams);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Open()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::Open()


//===========================================================================
// CtIDirectMusicSynth8::Close()
//
// Encapsulates calls to Close()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::Close(void)
{
    HRESULT hRes     = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Close()",
            m_szInterfaceName);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->Close();

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Close()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::Close()


//===========================================================================
// CtIDirectMusicSynth8::SetNumChannelGroups()
//
// Encapsulates calls to SetNumChannelGroups()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::SetNumChannelGroups(DWORD dwGroups)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetNumChannelGroups()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwGroups == %08Xh",
            dwGroups);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->SetNumChannelGroups(dwGroups);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetNumChannelGroups()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::SetNumChannelGroups()


//===========================================================================
// CtIDirectMusicSynth8::Download()
//
// Encapsulates calls to Download()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::Download(LPHANDLE phDownload,
									  LPVOID pvData, LPBOOL pbFree)
{
    HRESULT             hRes                = E_NOTIMPL;
	BOOL				fValid_phDownload	= TRUE;
    BOOL                fValid_pvData	    = TRUE;
	BOOL				fValid_pbFree		= TRUE;

    // validate phDownload
    if(!helpIsValidPtr((void*)phDownload, sizeof(LPHANDLE), FALSE))
    {
        fValid_phDownload = FALSE;
    }

	// validate pvData
    if(!helpIsValidPtr(pvData, sizeof(LPVOID), FALSE))
    {
        fValid_pvData = FALSE;
    }

	// validate pbFree
    if(!helpIsValidPtr(pbFree, sizeof(LPBOOL), FALSE))
    {
        fValid_pbFree = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Download()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "phDownload == %p   %s",
            phDownload,
            fValid_phDownload ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pvData == %p   %s",
            pvData,
            fValid_pvData ? "" : "BAD");
    if(fValid_pvData)
    {
        // BUGBUG - log contents of pvData @ MAX_LOGLEVEL (in byte format)
    }
	fnsLog(PARAMLOGLEVEL, "pbFree == %p   %s",
            pbFree,
            fValid_pbFree ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->Download(phDownload, pvData, pbFree);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Download()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes        == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_phDownload)
    {
        fnsLog(PARAMLOGLEVEL, "*phDownload == %08Xh",
                *phDownload);
    }
    if(fValid_pbFree)
    {
        fnsLog(PARAMLOGLEVEL, "*pbFree     == %s",
                (*pbFree) ? "TRUE" : "FALSE");
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::Download()


//===========================================================================
// CtIDirectMusicSynth8::Unload()
//
// Encapsulates calls to Unload()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::Unload(HANDLE hDownload,
									HRESULT ( CALLBACK *lpFreeHandle)(HANDLE,HANDLE),
									HANDLE hUserData)
{
    HRESULT hRes = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Unload()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hDownload == %08Xh",
            hDownload);
    // BUGBUG how to properly validate this??
    fnsLog(PARAMLOGLEVEL, "lpFreeHandle == %p",
            lpFreeHandle);
	fnsLog(PARAMLOGLEVEL, "hUserData == %08Xh",
            hUserData);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->Unload(hDownload, lpFreeHandle, hUserData);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Unload()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::Unload()


//===========================================================================
// CtIDirectMusicSynth8::PlayBuffer()
//
// Encapsulates calls to PlayBuffer()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::PlayBuffer(REFERENCE_TIME rt,
								  LPBYTE pbBuffer, DWORD cbBuffer)
{
    HRESULT             hRes                = E_NOTIMPL;
	BOOL				fValid_pbBuffer		= TRUE;

    // validate pbBuffer
    if(!helpIsValidPtr((void*)pbBuffer, cbBuffer, FALSE))
    {
        fValid_pbBuffer = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::PlayBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rt == %016Xh",
            rt);
    fnsLog(PARAMLOGLEVEL, "pbBuffer == %p   %s",
            pbBuffer,
            fValid_pbBuffer ? "" : "BAD");
    if(fValid_pbBuffer)
    {
        // BUGBUG log contents of pbBuffer @ MAXLOGLEVEL (as bytes)
    }
    fnsLog(PARAMLOGLEVEL, "cbBuffer == %08Xh",
            cbBuffer);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->PlayBuffer(rt, pbBuffer, cbBuffer);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::PlayBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::PlayBuffer()


//===========================================================================
// CtIDirectMusicSynth8::GetRunningStats()
//
// Encapsulates calls to GetRunningStats()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::GetRunningStats(LPDMUS_SYNTHSTATS pStats)
{
    HRESULT             hRes                = E_NOTIMPL;
	BOOL				fValid_pStats		= TRUE;

    // validate pStats
    if(!helpIsValidPtr((void*)pStats, sizeof(DMUS_SYNTHSTATS), FALSE))
    {
        fValid_pStats = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetRunningStats()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pStats == %p   %s",
            pStats,
            fValid_pStats ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->GetRunningStats(pStats);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetRunningStats()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pStats)
    {
        // BUGBUG log contents of pStats (STRUCTLOGLEVEL)
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::GetRunningStats()


//===========================================================================
// CtIDirectMusicSynth8::GetPortCaps()
//
// Encapsulates calls to GetPortCaps()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::GetPortCaps(LPDMUS_PORTCAPS pCaps)
{
    HRESULT hRes            = E_NOTIMPL;
	BOOL	fValid_pCaps    = TRUE;

    // validate pCaps
    if(!helpIsValidPtr((void*)pCaps, sizeof(DMUS_PORTCAPS), FALSE))
    {
        fValid_pCaps = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(PARAMLOGLEVEL, TEXT("--- Calling %s::GetPortCaps()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("pCaps == %p   %s"),
            pCaps,
            fValid_pCaps ? TEXT("") : TEXT("BAD"));

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->GetPortCaps(pCaps);

    // log results
    fnsLog(PARAMLOGLEVEL, TEXT("--- Returned from %s::GetPortCaps()"),
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, TEXT("hRes == %s (%08Xh)"),
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pCaps)
    {
        tdmLogDMUS_PORTCAPS(STRUCTLOGLEVEL, pCaps);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::GetPortCaps()


//===========================================================================
// CtIDirectMusicSynth8::SetMasterClock()
//
// Encapsulates calls to SetMasterClock()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::SetMasterClock(CtIReferenceClock *ptClock)
{
    HRESULT         hRes            = E_NOTIMPL;
	BOOL	        fValid_ptClock	= TRUE;
    IReferenceClock *pRefClock      = NULL;

    // validate pClock
    if(!helpIsValidPtr((void*)ptClock, sizeof(CtIReferenceClock), FALSE))
    {
        fValid_ptClock = FALSE;
        pRefClock = (IReferenceClock*)ptClock;
    }
    else
    {
        // good pointer, get the real object
        hRes = ptClock->GetRealObjPtr(&pRefClock);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pClock == %p   %s",
            pRefClock,
            fValid_ptClock ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->SetMasterClock(pRefClock);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetMasterClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptClock && pRefClock)
    {
        pRefClock->Release();
    }
    return hRes;

} // *** end CtIDirectMusicSynth8::SetMasterClock()


//===========================================================================
// CtIDirectMusicSynth8::GetLatencyClock()
//
// Encapsulates calls to GetLatencyClock()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::GetLatencyClock(CtIReferenceClock **pptClock)
{
    HRESULT         hRes            = E_NOTIMPL;
	BOOL			fValid_pptClock	= TRUE;
    IReferenceClock *pRefClock      = NULL;
    IReferenceClock **ppRefClock    = NULL;

    // validate pptClock
    // NOTE:
    //	If we are passed an invalid CtIReferenceClock pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicSynth8:GetLatencyClock().
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
    hRes = ((IDirectMusicSynth8*)m_pUnk)->GetLatencyClock(ppRefClock);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetLatencyClock()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptClock)
    {
        fnsLog(PARAMLOGLEVEL, "*ppClock == %p",
                pRefClock);
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

} // *** end CtIDirectMusicSynth8::GetLatencyClock()


//===========================================================================
// CtIDirectMusicSynth8::Activate()
//
// Encapsulates calls to Activate()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::Activate(BOOL fEnable)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Activate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "fEnable == %d",
            fEnable);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->Activate(fEnable);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Activate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::Activate()


//===========================================================================
// CtIDirectMusicSynth8::Render()
//
// Encapsulates calls to Render()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::Render(short *pBuffer, DWORD dwLength,
                                DWORD dwPosition)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_pBuffer  = TRUE;

    // validate pBuffer
    if(!helpIsValidPtr((void*)pBuffer, sizeof(short), FALSE))
    {
        fValid_pBuffer = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Render()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pBuffer   == %p   %s",
            pBuffer,
            fValid_pBuffer ? "" : "BAD");
    if(fValid_pBuffer)
    {
        // BUGBUG log contents of pBuffer @ MAXLOGLEVEL
    }
    fnsLog(PARAMLOGLEVEL, "dwLength   == %08Xh",
            dwLength);
    fnsLog(PARAMLOGLEVEL, "dwPosition == %08Xh",
            dwPosition);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->Render(pBuffer, dwLength, dwPosition);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Render()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::Render()


//===========================================================================
// CtIDirectMusicSynth8::SetSynthSink
//
// Encapsulates calls to SetSynthSink()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::SetSynthSink(CtIDirectMusicSynthSink *ptdmSynSnk)
{
    HRESULT                 hRes                = E_NOTIMPL;
    BOOL                    fValid_ptdmSynSnk   = TRUE;
    IDirectMusicSynthSink   *pdmSink            = NULL;

    // validate ptdmSynSnk
    if(!helpIsValidPtr((void*)ptdmSynSnk, sizeof(CtIDirectMusicSynthSink),
                        FALSE))
    {
        fValid_ptdmSynSnk = FALSE;
        pdmSink = (IDirectMusicSynthSink*)ptdmSynSnk;
    }
    else
    {
        // good pointer, get the real object
        hRes = ptdmSynSnk->GetRealObjPtr(&pdmSink);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SetSynthSink()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSynthSink == %p   %s",
            pdmSink,
            fValid_ptdmSynSnk ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->SetSynthSink(pdmSink);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetSynthSink()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmSynSnk && pdmSink)
    {
        pdmSink->Release();
    }
    return hRes;

} // *** end CtIDirectMusicSynth8::SetSynthSink()


//===========================================================================
// CtIDirectMusicSynth8::SetChannelPriority()
//
// Encapsulates calls to SetChannelPriority()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::SetChannelPriority(
	DWORD dwChannelGroup,
	DWORD dwChannel,
	DWORD dwPriority)
{
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetChannelPriority()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh",
            dwChannelGroup);
    fnsLog(PARAMLOGLEVEL, "dwChannel == %08Xh",
            dwChannel);
    fnsLog(PARAMLOGLEVEL, "dwPriority == %08Xh",
            dwPriority);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->SetChannelPriority(
		dwChannelGroup,
		dwChannel,
		dwPriority);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetChannelPriority()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::SetChannelPriority()


//===========================================================================
// CtIDirectMusicSynth8::GetChannelPriority()
//
// Encapsulates calls to GetChannelPriority()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::GetChannelPriority(
	DWORD dwChannelGroup,
	DWORD dwChannel,
	LPDWORD pdwPriority)
{
    HRESULT hRes				= E_NOTIMPL;
	BOOL	fValid_pdwPriority	= TRUE;

    fnsIncrementIndent();

    // validate pdwPriority
    if(!helpIsValidPtr((void*)pdwPriority, sizeof(LPDWORD), FALSE))
    {
        fValid_pdwPriority = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetChannelPriority()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh",
            dwChannelGroup);
    fnsLog(PARAMLOGLEVEL, "dwChannel == %08Xh",
            dwChannel);
    fnsLog(PARAMLOGLEVEL, "pdwPriority == %p   %s",
            pdwPriority,
            fValid_pdwPriority ?  "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->GetChannelPriority(
		dwChannelGroup,
		dwChannel,
		pdwPriority);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetChannelPriority()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_pdwPriority)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwPriority == %08Xh",
                *pdwPriority);
    }
    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::SetChannelPriority()


//===========================================================================
// CtIDirectMusicSynth8::GetFormat()
//
// Encapsulates calls to GetFormat()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::GetFormat(
	LPWAVEFORMATEX pWaveFormatEx,
	LPDWORD pdwWaveFormatExSize)
{
    HRESULT hRes						= E_NOTIMPL;
	BOOL	fValid_pWaveFormatEx		= TRUE;
	BOOL	fValid_pdwWaveFormatExSize	= TRUE;

    fnsIncrementIndent();

    // validate pWaveFormatEx
    if(!helpIsValidPtr((void*)pWaveFormatEx, sizeof(LPWAVEFORMATEX), FALSE))
    {
        fValid_pWaveFormatEx = FALSE;
    }
    // validate pdwWaveFormatExSize
    if(!helpIsValidPtr((void*)pdwWaveFormatExSize, sizeof(LPDWORD), FALSE))
    {
        fValid_pdwWaveFormatExSize = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetFormat()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pWaveFormatEx == %p   %s",
            pWaveFormatEx,
            fValid_pWaveFormatEx ?  "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pdwWaveFormatExSize == %p   %s",
            pdwWaveFormatExSize,
            fValid_pdwWaveFormatExSize ?  "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->GetFormat(
		pWaveFormatEx,
		pdwWaveFormatExSize);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetFormat()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_pWaveFormatEx)
    {
        fnsLog(PARAMLOGLEVEL, "*pWaveFormatEx == %08Xh",
                *pWaveFormatEx);
    }
    if(fValid_pdwWaveFormatExSize)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwWaveFormatExSize == %08Xh",
                *pdwWaveFormatExSize);
    }
    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::GetFormat()


//===========================================================================
// CtIDirectMusicSynth8::GetAppend()
//
// Encapsulates calls to GetAppend()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::GetAppend(
	DWORD* pdwAppend)
{
    HRESULT hRes				= E_NOTIMPL;
	BOOL	fValid_pdwAppend	= TRUE;

    fnsIncrementIndent();

    // validate pdwAppend
    if(!helpIsValidPtr((void*)pdwAppend, sizeof(DWORD*), FALSE))
    {
        fValid_pdwAppend = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetAppend()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwAppend == %p   %s",
            pdwAppend,
            fValid_pdwAppend ?  "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->GetAppend(
		pdwAppend);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetAppend()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_pdwAppend)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwAppend == %08Xh",
                *pdwAppend);
    }
    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::GetAppend()


*/

//===========================================================================
// CtIDirectMusicSynth8::PlayVoice()
//
// Encapsulates calls to PlayVoice()
//
// History:
//  10/18/1999 - kcraven - created
//  04/20/2000 - kcraven - changed to match dx8 headers
//===========================================================================
HRESULT CtIDirectMusicSynth8::PlayVoice
(
	REFERENCE_TIME rt, 
	DWORD dwVoiceId, 
	DWORD dwChannelGroup, 
	DWORD dwChannel, 
	DWORD dwDLId, 
	long prPitch,			//PREL not defined here
	long vrVolume,          //VREL not defined here 
	SAMPLE_TIME stVoiceStart,
	SAMPLE_TIME stLoopStart,
	SAMPLE_TIME stLoopEnd)
{
    HRESULT             hRes                = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::PlayVoice()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rt             == %016Xh",
            rt);
    fnsLog(PARAMLOGLEVEL, "dwVoiceId      == %08Xh",
            dwVoiceId);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh",
            dwChannelGroup);
    fnsLog(PARAMLOGLEVEL, "dwChannel      == %08Xh",
            dwChannel);
    fnsLog(PARAMLOGLEVEL, "dwDLId         == %08Xh",
            dwDLId);
    fnsLog(PARAMLOGLEVEL, "prPitch        == %ld",
            prPitch);
    fnsLog(PARAMLOGLEVEL, "vrVolume       == %ld",
            vrVolume);
    fnsLog(PARAMLOGLEVEL, "stVoiceStart   == %016Xh",
            stVoiceStart);
    fnsLog(PARAMLOGLEVEL, "stLoopStart    == %016Xh",
            stLoopStart);
    fnsLog(PARAMLOGLEVEL, "stLoopEnd      == %016Xh",
            stLoopEnd);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->PlayVoice(
												rt,
												dwVoiceId,
												dwChannelGroup,
												dwChannel,
												dwDLId,
												prPitch,
												vrVolume,
												stVoiceStart,
												stLoopStart,
												stLoopEnd);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::PlayVoice()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::PlayVoice()




//===========================================================================
// CtIDirectMusicSynth8::StopVoice()
//
// Encapsulates calls to StopVoice()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::StopVoice
(
	REFERENCE_TIME rt,
	DWORD dwVoiceId
)
{
    HRESULT             hRes                = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::StopVoice()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rt == %016Xh",
            rt);
    fnsLog(PARAMLOGLEVEL, "dwVoiceId == %08Xh",
            dwVoiceId);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->StopVoice(rt,dwVoiceId);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::StopVoice()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::StopVoice()




//===========================================================================
// CtIDirectMusicSynth8::GetVoicePosition()
//
// Encapsulates calls to GetVoicePosition()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
//PHOOPHOO
/*
HRESULT CtIDirectMusicSynth8::GetVoicePosition
(
	DWORD dwVoice[],
	DWORD cbVoice,
	SAMPLE_POSITION dwVoicePos[]
)
{
    HRESULT hRes                = E_NOTIMPL;
	BOOL	fValid_dwVoice		= TRUE;
	BOOL	fValid_dwVoicePos	= TRUE;

    fnsIncrementIndent();

    // validate pdwAppend
    if(!helpIsValidPtr((void*)dwVoice, sizeof(DWORD*), FALSE))
    {
        fValid_dwVoice = FALSE;
    }
    // validate pdwAppend
    if(!helpIsValidPtr((void*)dwVoicePos, sizeof(SAMPLE_POSITION*), FALSE))
    {
        fValid_dwVoicePos = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::GetVoicePosition()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwVoice == %08Xh   %s",
            dwVoice,
            fValid_dwVoice ?  "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "cbVoice == %08Xh",
            cbVoice);
    fnsLog(PARAMLOGLEVEL, "dwVoicePos == %08Xh   %s",
            dwVoicePos,
            fValid_dwVoicePos ?  "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->GetVoicePosition(dwVoice,cbVoice,dwVoicePos);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetVoicePosition()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::GetVoicePosition()
*/



//===========================================================================
// CtIDirectMusicSynth8::Refresh()
//
// Encapsulates calls to Refresh()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::Refresh
(
	DWORD dwDownloadID,
	DWORD dwFlags
)
{
    HRESULT             hRes                = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Refresh()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwDownloadID == %08Xh",
            dwDownloadID);
    fnsLog(PARAMLOGLEVEL, "dwFlags == %08Xh",
            dwFlags);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->Refresh(dwDownloadID,dwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Refresh()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::Refresh()




//===========================================================================
// CtIDirectMusicSynth8::AssignChannelToBuses()
//
// Encapsulates calls to AssignChannelToBuses()
//
// History:
//  10/18/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynth8::AssignChannelToBuses
(
	DWORD dwChannelGroup,
	DWORD dwChannel,
	LPDWORD pdwBusses,
	DWORD cBusses
)
{
    HRESULT             hRes                = E_NOTIMPL;
	BOOL	fValid_pdwBusses		= TRUE;

    fnsIncrementIndent();

    // validate pdwAppend
    if(!helpIsValidPtr((void*)pdwBusses, sizeof(LPDWORD), FALSE))
    {
        fValid_pdwBusses = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::AssignChannelToBuses()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwChannelGroup == %08Xh",
            dwChannelGroup);
    fnsLog(PARAMLOGLEVEL, "dwChannel == %08Xh",
            dwChannel);
    fnsLog(PARAMLOGLEVEL, "pdwBusses == %p   %s",
            pdwBusses,
            fValid_pdwBusses ?  "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "cBusses == %08Xh",
            cBusses);

    // call the real function
    hRes = ((IDirectMusicSynth8*)m_pUnk)->AssignChannelToBuses(dwChannelGroup,dwChannel,pdwBusses,cBusses);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::AssignChannelToBuses()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicSynth8::AssignChannelToBuses()




//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================







