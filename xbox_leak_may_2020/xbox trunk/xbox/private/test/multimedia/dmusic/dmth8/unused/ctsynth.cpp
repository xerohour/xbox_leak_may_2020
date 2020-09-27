//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctsynth.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctsynth.cpp
//
// Test harness implementation of IDirectMusicSynth
//
// Functions:
//  CtIDirectMusicSynth::CtIDirectMusicSynth()
//  CtIDirectMusicSynth::~CtIDirectMusicSynth()
//  CtIDirectMusicSynth::InitTestClass()
//  CtIDirectMusicSynth::GetRealObjPtr()
//  CtIDirectMusicSynth::QueryInterface()
//  CtIDirectMusicSynth::Open()
//  CtIDirectMusicSynth::Close()
//  CtIDirectMusicSynth::SetNumChannelGroups()
//  CtIDirectMusicSynth::Download()
//  CtIDirectMusicSynth::Unload()
//  CtIDirectMusicSynth::PlayBuffer()
//  CtIDirectMusicSynth::GetRunningStats()
//  CtIDirectMusicSynth::GetPortCaps()
//  CtIDirectMusicSynth::SetMasterClock()
//  CtIDirectMusicSynth::GetLatencyClock()
//  CtIDirectMusicSynth::Activate()
//  CtIDirectMusicSynth::Render() 
//  CtIDirectMusicSynth::SetSynthSink()
//
// History:
//  01/05/1998 - a-llucar - created
//	02/25/1998 - a-llucar - Changed Open(), Close(), SetNumChannelGroups(), 
//							Download(), Unload(), QueueBuffer(),
//							GetRunningStats(), GetLatencyClock().
//  03/05/1998 - davidkl - updated to latest dm arch
//  03/26/1998 - davidkl - implemented SetSynthSink.
//                         Brave new world... now a derived class
//  04/01/1998 - davidkl - minor cleanup
//===========================================================================
 
#include "dmth.h"
#include "dmthp.h"

//---------------------------------------------------------------------------


//===========================================================================
// CtIDirectMusicSynth::CtIDirectMusicSynth()
//
// Default constructor
//
// History:
//  01/05/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicSynth::CtIDirectMusicSynth(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicSynth";

} //*** end CtIDirectMusicSynth::CtIDirectMusicSynth()


//===========================================================================
// CtIDirectMusicSynth::~CtIDirectMusicSynth()
//
// Default constructor
//
// Parameters: none
//
// Returns: 
//
// History:
//  01/05/1998 - a-llucar - created
//===========================================================================
CtIDirectMusicSynth::~CtIDirectMusicSynth(void)
{
    // nothing to do

} //*** end CtIDirectMusicSynth::~CtIDirectMusicSynth()


//===========================================================================
// CtIDirectMusicSynth::InitTestClass
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
//  01/05/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicSynth::InitTestClass(IDirectMusicSynth *pdmSynth)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmSynth, sizeof(IDirectMusicSynth), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmSynth));

} //*** end CtIDirectMusicSynth::InitTestClass()


//===========================================================================
// CtIDirectMusicSynth::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicSynth **ppdmSynth - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  01/05/1998 - a-llucar - created
//  1/15/98 - bthomas - made it more COM-like
//===========================================================================
HRESULT CtIDirectMusicSynth::GetRealObjPtr(IDirectMusicSynth **ppdmSynth)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmSynth, sizeof(IDirectMusicSynth*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmSynth));

} //*** end CtIDirectMusicSynth::GetRealObjPtr()


//===========================================================================
// CtIDirectMusicSynth::QueryInterface
//
// Encapsulates calls to QueryInterface
//
// History:
//  01/05/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicSynth::QueryInterface(REFIID riid, 
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

} //*** end CtIDirectMusicSynth::QueryInterface


//===========================================================================
// CtIDirectMusicSynth::Open()
//
// Encapsulates calls to Open()
//
// History:
//  01/05/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicSynth::Open(LPDMUS_PORTPARAMS pPortParams)
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
    fnsLog(PARAMLOGLEVEL, "pPortParams == %p   %s",
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->Open(pPortParams);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Open()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSynth::Open()
 

//===========================================================================
// CtIDirectMusicSynth::Close()
//
// Encapsulates calls to Close()
//
// History:
//  01/05/1998 - a-llucar - created
//  03/05/1998 - davidkl - minor tweaks
//===========================================================================
HRESULT CtIDirectMusicSynth::Close(void)
{                                           
    HRESULT hRes     = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Close()",
            m_szInterfaceName);
            
    // call the real function
    hRes = ((IDirectMusicSynth*)m_pUnk)->Close();

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Close()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSynth::Close()
 

//===========================================================================
// CtIDirectMusicSynth::SetNumChannelGroups()
//
// Encapsulates calls to SetNumChannelGroups()
//
// History:
//  01/05/1998 - a-llucar - created
//  03/05/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicSynth::SetNumChannelGroups(DWORD dwGroups)
{                                           
    HRESULT hRes    = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::SetNumChannelGroups()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwGroups == %08Xh",
            dwGroups);
            
    // call the real function
    hRes = ((IDirectMusicSynth*)m_pUnk)->SetNumChannelGroups(dwGroups);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::SetNumChannelGroups()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSynth::SetNumChannelGroups()
 

//===========================================================================
// CtIDirectMusicSynth::Download()
//
// Encapsulates calls to Download()
//
// History:
//  01/05/1998 - a-llucar - created
//  04/01/1998 - davidkl - enhanced logging
//===========================================================================
HRESULT CtIDirectMusicSynth::Download(LPHANDLE phDownload, 
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->Download(phDownload, pvData, pbFree);

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

} //*** end CtIDirectMusicSynth::Download()


//===========================================================================
// CtIDirectMusicSynth::Unload()
//
// Encapsulates calls to Unload()
//
// History:
//  01/05/1998 - a-llucar - created
//  03/05/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicSynth::Unload(HANDLE hDownload, 
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->Unload(hDownload, lpFreeHandle, hUserData);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Unload()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSynth::Unload()


//===========================================================================
// CtIDirectMusicSynth::PlayBuffer()
//
// Encapsulates calls to PlayBuffer()
//
// History:
//  01/05/1998 - a-llucar - created
//  03/05/1998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicSynth::PlayBuffer(REFERENCE_TIME rt, 
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->PlayBuffer(rt, pbBuffer, cbBuffer);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::PlayBuffer()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSynth::PlayBuffer()


//===========================================================================
// CtIDirectMusicSynth::GetRunningStats()
//
// Encapsulates calls to GetRunningStats()
//
// History:
//  01/05/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicSynth::GetRunningStats(LPDMUS_SYNTHSTATS pStats)
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->GetRunningStats(pStats);

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

} //*** end CtIDirectMusicSynth::GetRunningStats()


//===========================================================================
// CtIDirectMusicSynth::GetPortCaps()
//
// Encapsulates calls to GetPortCaps()
//
// History:
//  01/05/1998 - a-llucar - created
//  04/01/1998 - davidkl - enhanced logging
//===========================================================================
HRESULT CtIDirectMusicSynth::GetPortCaps(LPDMUS_PORTCAPS pCaps)
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->GetPortCaps(pCaps);

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

} //*** end CtIDirectMusicSynth::GetPortCaps()


//===========================================================================
// CtIDirectMusicSynth::SetMasterClock()
//
// Encapsulates calls to SetMasterClock()
//
// History:
//  01/05/1998 - a-llucar - created
//  03/06/1998 - davidkl - now uses wrapper classes
//===========================================================================
HRESULT CtIDirectMusicSynth::SetMasterClock(CtIReferenceClock *ptClock)
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->SetMasterClock(pRefClock);

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

} //*** end CtIDirectMusicSynth::SetMasterClock()


//===========================================================================
// CtIDirectMusicSynth::GetLatencyClock()
//
// Encapsulates calls to GetLatencyClock()
//
// History:
//  01/05/1998 - a-llucar - created
//  03/06/1998 - davidkl - now uses wrapper classes, enhanced logging
//===========================================================================
HRESULT CtIDirectMusicSynth::GetLatencyClock(CtIReferenceClock **pptClock)
{                                           
    HRESULT         hRes            = E_NOTIMPL;
	BOOL			fValid_pptClock	= TRUE;
    IReferenceClock *pRefClock      = NULL;
    IReferenceClock **ppRefClock    = NULL;  

    // validate pptClock
    // NOTE:
    //	If we are passed an invalid CtIReferenceClock pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicSynth:GetLatencyClock().  
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->GetLatencyClock(ppRefClock);

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

} //*** end CtIDirectMusicSynth::GetLatencyClock()


//===========================================================================
// CtIDirectMusicSynth::Activate()
//
// Encapsulates calls to Activate()
//
// History:
//  01/05/1998 - a-llucar - created
//  03/06/1998 - davidkl - fixed logging
//  08/21/1998 - dvaidkl - removed hWnd
//===========================================================================
HRESULT CtIDirectMusicSynth::Activate(BOOL fEnable)
{                                           
    HRESULT hRes    = E_NOTIMPL;
	
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "--- Calling %s::Activate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "fEnable == %d",
            fEnable);
            
    // call the real function
    hRes = ((IDirectMusicSynth*)m_pUnk)->Activate(fEnable);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Activate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSynth::Activate()


//===========================================================================
// CtIDirectMusicSynth::Render()
//
// Encapsulates calls to Render()
//
// History:
//  03/06/1998 - davidkl - created
//  03/18/1998 - davidkl - renamed due to changes in dmusicc.h
//===========================================================================
HRESULT CtIDirectMusicSynth::Render(short *pBuffer, DWORD dwLength, 
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->Render(pBuffer, dwLength, dwPosition);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::Render()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicSynth::Render()


//===========================================================================
// CtIDirectMusicSynth::SetSynthSink
//
// Encapsulates calls to SetSynthSink()
//
// History:
//  03/26/1998 - davidkl - created
//===========================================================================
HRESULT CtIDirectMusicSynth::SetSynthSink(CtIDirectMusicSynthSink *ptdmSynSnk)
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->SetSynthSink(pdmSink);

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

} //*** end CtIDirectMusicSynth::SetSynthSink()


//===========================================================================
// CtIDirectMusicSynth::SetChannelPriority()
//
// Encapsulates calls to SetChannelPriority()
//
// History:
//  08/05/1998 - a-kellyc - created
//===========================================================================
HRESULT CtIDirectMusicSynth::SetChannelPriority(
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->SetChannelPriority(
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

} //*** end CtIDirectMusicSynth::SetChannelPriority()


//===========================================================================
// CtIDirectMusicSynth::GetChannelPriority()
//
// Encapsulates calls to GetChannelPriority()
//
// History:
//  08/05/1998 - a-kellyc - created
//===========================================================================
HRESULT CtIDirectMusicSynth::GetChannelPriority(
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->GetChannelPriority(
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

} //*** end CtIDirectMusicSynth::SetChannelPriority()
 

//===========================================================================
// CtIDirectMusicSynth::GetFormat()
//
// Encapsulates calls to GetFormat()
//
// History:
//  09/30/1998 - a-kellyc - created
//===========================================================================
HRESULT CtIDirectMusicSynth::GetFormat(
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->GetFormat(
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

} //*** end CtIDirectMusicSynth::GetFormat()
 

//===========================================================================
// CtIDirectMusicSynth::GetAppend()
//
// Encapsulates calls to GetAppend()
//
// History:
//  09/30/1998 - a-kellyc - created
//===========================================================================
HRESULT CtIDirectMusicSynth::GetAppend(
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
    hRes = ((IDirectMusicSynth*)m_pUnk)->GetAppend(
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

} //*** end CtIDirectMusicSynth::GetAppend()
 

//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================







