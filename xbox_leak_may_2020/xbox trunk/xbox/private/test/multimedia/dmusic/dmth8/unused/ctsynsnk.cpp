//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctsynsnk.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctsynsnk.cpp
//
// Test harness implementation of IDirectMusicSynthSink
//
// Functions:
//  CtIDirectMusicSynthSink::CtIDirectMusicSynthSink()
//  CtIDirectMusicSynthSink::~CtIDirectMusicSynthSink()
//  CtIDirectMusicSynthSink::InitTestClass()
//  CtIDirectMusicSynthSink::GetRealObjPtr()
//  CtIDirectMusicSynthSink::QueryInterface()
//  CtIDirectMusicSynthSink::Init()
//  CtIDirectMusicSynthSink::SetFormat()
//  CtIDirectMusicSynthSink::SetMasterClock()
//  CtIDirectMusicSynthSink::GetLatencyClock()
//  CtIDirectMusicSynthSink::Activate()
//  CtIDirectMusicSynthSink::SampleToRefTime()
//  CtIDirectMusicSynthSink::RefTimeToSample()
//  CtIDirectMusicSynthSink::SetDirectSound()
//  CtIDirectMusicSynthSink::GetDesiredBufferSize()
//
// History:
//  03/16/1998 - davidkl - created
//  08/21/1998 - davidkl - added SetDirectSound() / removed SetFormat
//  09/24/1998 - kcraven - added GetDesiredBufferSize()
//===========================================================================

#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicSynthSink::CtIDirectMusicSynthSink()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  03/16/1998 - davidkl - created
//  03/17/1998 - davidkl - now inherits from CtIUnknown
//===========================================================================
CtIDirectMusicSynthSink::CtIDirectMusicSynthSink(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicSynthSink";
} //*** emd CtIDirectMusicSynthSink::CtIDirectMusicSynthSink()


//===========================================================================
// CtIDirectMusicSynthSink::~CtIDirectMusicSynthSink()
//
// Default destructor
//
// Parameters: none
//
// Returns:
//
// History:
//  03/16/1998 - davidkl - created
//===========================================================================
CtIDirectMusicSynthSink::~CtIDirectMusicSynthSink(void)
{
    // nothing to do

} //*** end CtIDirectMusicSynthSink::~CtIDirectMusicSynthSink()


//===========================================================================
// CtIDirectMusicSynthSink::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicSynthSink object pointer for future use.
//
// Parameters:
//  IDirectMusicSynthSink *pdmSink - pointer to real object
//
// Returns:
//
// History:
//  03/16/1998 - davidkl - created
//  03/17/1998 - davidkl - reimplemented as overloaded version
//===========================================================================
HRESULT CtIDirectMusicSynthSink::InitTestClass(IDirectMusicSynthSink *pdmSink)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmSink, sizeof(IDirectMusicSynthSink), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmSink));

} //*** end CtIDirectMusicSynthSink::InitTestClass()


//===========================================================================
// CtIDirectMusicSynthSink::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicSynthSink **ppdmSink - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  03/16/1998 - davidkl - created
//  03/17/1998 - davidkl - reimplemented as overloaded version
//===========================================================================
HRESULT CtIDirectMusicSynthSink::GetRealObjPtr(IDirectMusicSynthSink **ppdmSink)
{
	// validate ppdm
	if(!helpIsValidPtr(ppdmSink, sizeof(IDirectMusicSynthSink*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmSink));

} //*** end CtIDirectMusicSynthSink::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicSynthSink::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// Parameters:
//
// Returns:
//
// History:
//  03/16/1998 - davidkl - created
//  03/17/1998 - davidkl - reimplemented as replacement version
//===========================================================================
HRESULT CtIDirectMusicSynthSink::QueryInterface(REFIID riid, LPVOID *ppvObj)
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

} //*** end CtIDirectMusicSynthSink::QueryInterface()


//===========================================================================
// CtIDirectMusicSynthSink::Init
//
// Encapsulates calls to Init
//
// Parameters:
//
// Returns:
//
// History:
//  03/17/1998 - davidkl - stubbed
//  03/25/1998 - davidkl - implemented
//===========================================================================
HRESULT CtIDirectMusicSynthSink::Init(CtIDirectMusicSynth *ptdmSynth)
{
    HRESULT             hRes                = E_NOTIMPL;
    BOOL                fValid_ptdmSynth    = TRUE;
    IDirectMusicSynth   *pdmSynth           = NULL;

    // validate ptClock
    if(!helpIsValidPtr((void*)ptdmSynth, sizeof(CtIDirectMusicSynth),
                        FALSE))
    {
        fValid_ptdmSynth   = FALSE;

        // we have a bogus pointer, use it as the real thing
        pdmSynth = (IDirectMusicSynth*)ptdmSynth;

    }
    else
    {
        // we have a valid pointer, get the real one from our test object
        hRes = ptdmSynth->GetRealObjPtr(&pdmSynth);
        if(FAILED(hRes))
        {
            // BUGBUG
			// add logging
            goto END;
        }
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Init()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSynth == %p   %s",
            pdmSynth,
            fValid_ptdmSynth ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynthSink*)m_pUnk)->Init(pdmSynth);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Init()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    // done
END:
    if(fValid_ptdmSynth && pdmSynth)
    {
        pdmSynth->Release();
    }
    return hRes;

} //*** end CtIDirectMusicSynthSink::Init()


//===========================================================================
// CtIDirectMusicSynthSink::SetMasterClock
//
// Encapsulates calls to SetMasterClock
//
// Parameters:
//
// Returns:
//
// History:
//  03/17/1998 - davidkl - stubbed
//  03/25/1998 - davidkl - implemented
//===========================================================================
HRESULT CtIDirectMusicSynthSink::SetMasterClock(CtIReferenceClock *ptClock)
{
    HRESULT         hRes            = E_NOTIMPL;
    BOOL            fValid_ptClock  = TRUE;
    IReferenceClock *pRefClock      = NULL;

    // validate ptClock
    if(!helpIsValidPtr((void*)ptClock, sizeof(CtIReferenceClock),
                        FALSE))
    {
        fValid_ptClock   = FALSE;

        // we have a bogus pointer, use it as the real thing
        pRefClock = (IReferenceClock*)ptClock;

    }
    else
    {
        // we have a valid pointer, get the real one from our test object
        hRes = ptClock->GetRealObjPtr(&pRefClock);
        if(FAILED(hRes))
        {
            // BUGBUG
			// add logging
            goto END;
        }
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SetMasterClock()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pClock == %p   %s",
            pRefClock,
            fValid_ptClock ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynthSink*)m_pUnk)->SetMasterClock(pRefClock);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetMasterClock()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    // done
END:
    if(fValid_ptClock && pRefClock)
    {
        pRefClock->Release();
    }
    return hRes;

} //*** end CtIDirectMusicSynthSink::SetMasterClock()


//===========================================================================
// CtIDirectMusicSynthSink::GetLatencyClock
//
// Encapsulates calls to GetLatencyClock
//
// Parameters:
//
// Returns:
//
// History:
//  03/17/1998 - davidkl - stubbed
//  03/25/1998 - davidkl - implemented
//===========================================================================
HRESULT CtIDirectMusicSynthSink::GetLatencyClock(CtIReferenceClock **pptClock)
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
    hRes = ((IDirectMusicSynthSink*)m_pUnk)->GetLatencyClock(ppRefClock);

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

} //*** end CtIDirectMusicSynthSink::GetLatencyClock()


//===========================================================================
// CtIDirectMusicSynthSink::Activate
//
// Encapsulates calls to Activate
//
// Parameters:
//
// Returns:
//
// History:
//  03/17/1998 - davidkl - stubbed
//  03/18/1998 - davidkl - implemented
//  08/21/1998 - davidkl - removed hWnd
//===========================================================================
HRESULT CtIDirectMusicSynthSink::Activate(BOOL fEnable)
{
    HRESULT hRes    = E_NOTIMPL;

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s:Activate()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "fEnable == %s",
            fEnable ? "TRUE" : "FALSE");

    // call the real function
    hRes = ((IDirectMusicSynthSink*)m_pUnk)->Activate(fEnable);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s:Activate()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    // done
    return hRes;

} //*** end CtIDirectMusicSynthSink::Activate()


//===========================================================================
// CtIDirectMusicSynthSink::SampleToRefTime
//
// Encapsulates calls to SampleToRefTime
//
// Parameters:
//
// Returns:
//
// History:
//  03/17/1998 - davidkl - stubbed
//===========================================================================
HRESULT CtIDirectMusicSynthSink::SampleToRefTime(DWORD dwSampleTime,
                                                REFERENCE_TIME *prfTime)
{
    HRESULT hRes            = E_NOTIMPL;
    BOOL    fValid_prfTime  = TRUE;

    // validate prfTime
    if(!helpIsValidPtr((void*)prfTime, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prfTime = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s:SampleToRefTime()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwSampleTime == %08Xh",
            dwSampleTime);
    fnsLog(PARAMLOGLEVEL, "prfTime      == %p   %s",
            prfTime,
            fValid_prfTime ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynthSink*)m_pUnk)->SampleToRefTime(dwSampleTime,
                                                            prfTime);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s:SampleToRefTime()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prfTime)
    {
        fnsLog(PARAMLOGLEVEL, "*prfTime == %016Xh",
                *prfTime);
    }

    // done
    return hRes;

} //*** end CtIDirectMusicSynthSink::SampleToRefTime()


//===========================================================================
// CtIDirectMusicSynthSink::RefTimeToSample
//
// Encapsulates calls to RefTimeToSample
//
// Parameters:
//
// Returns:
//
// History:
//  03/17/1998 - davidkl - stubbed
//===========================================================================
HRESULT CtIDirectMusicSynthSink::RefTimeToSample(REFERENCE_TIME rfTime, 
                                                REFERENCE_TIME *prtSampleTime)
{
    HRESULT hRes                    = E_NOTIMPL;
    BOOL    fValid_pdwSampleTime    = TRUE;

    // validate pdwSampleTime
    if(!helpIsValidPtr((void*)prtSampleTime, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_pdwSampleTime = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s:RefTimeToSample()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rfTime        == %016Xh",
            rfTime);
    fnsLog(PARAMLOGLEVEL, "pdwSampleTime == %p   %s",
            prtSampleTime,
            fValid_pdwSampleTime ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynthSink*)m_pUnk)->RefTimeToSample(rfTime,
                                                            prtSampleTime);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s:RefTimeToSample()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pdwSampleTime)
    {
        fnsLog(PARAMLOGLEVEL, "*pdwSampleTime == %016Xh",
                *prtSampleTime);
    }

    // done
    return hRes;

} //*** end CtIDirectMusicSynthSink::RefTimeToSample()


//===========================================================================
// CtIDirectMusicSynthSink::SetDirectSound
//
// Encapsulates calls to SetDirectSound
//
// Parameters:
//
// Returns:
//
// History:
//  08/21/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicSynthSink::SetDirectSound(LPDIRECTSOUND pDirectSound,
                                        LPDIRECTSOUNDBUFFER pDirectSoundBuffer)
{
    HRESULT hRes                        = E_NOTIMPL;
    BOOL    fValid_pDirectSound         = TRUE;
    BOOL    fValid_pDirectSoundBuffer   = TRUE;

    // validate pDirectSound
    if(!helpIsValidPtr((void*)pDirectSound, sizeof(IDirectSound), TRUE))
    {
        fValid_pDirectSound = FALSE;
    }

    // validate pDirectSoundBuffer
    if(!helpIsValidPtr((void*)pDirectSoundBuffer, sizeof(IDirectSoundBuffer), TRUE))
    {
        fValid_pDirectSoundBuffer = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s:SetDirectSound()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pDirectSound       == %p   %s",
            pDirectSound,
            fValid_pDirectSound ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pDirectSoundBuffer == %p   %s",
            pDirectSoundBuffer,
            fValid_pDirectSoundBuffer ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynthSink*)m_pUnk)->SetDirectSound(pDirectSound,
                                                            pDirectSoundBuffer);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s:SetDirectSound()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    // done
    return hRes;

} //*** end CtIDirectMusicSynthSink::SetDirectSound()


//===========================================================================
//===========================================================================
//===========================================================================
// CtIDirectMusicSynthSink::GetDesiredBufferSize
//
// Encapsulates calls to GetDesiredBufferSize
//
// Parameters:
//
// Returns:
//
// History:
//  08/21/1998 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicSynthSink::GetDesiredBufferSize(LPDWORD pdwBufferSizeInSamples)
{
    HRESULT hRes                        = E_NOTIMPL;
    BOOL    fValid_pdwBufferSizeInSamples    = TRUE;

    // validate pdwBufferSizeInSamples
    if(!helpIsValidPtr((void*)pdwBufferSizeInSamples, sizeof(DWORD), FALSE))
    {
        fValid_pdwBufferSizeInSamples = FALSE;
    }

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s:GetDesiredBufferSize()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pdwBufferSizeInSamples       == %p   %s",
            pdwBufferSizeInSamples,
            fValid_pdwBufferSizeInSamples ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicSynthSink*)m_pUnk)->GetDesiredBufferSize(pdwBufferSizeInSamples);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s:GetDesiredBufferSize()",
        m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes     == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    // done
    return hRes;

} //*** end CtIDirectMusicSynthSink::GetDesiredBufferSize()


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================

