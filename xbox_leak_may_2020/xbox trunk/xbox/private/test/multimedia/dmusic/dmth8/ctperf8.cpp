 //+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctperf8.cpp
//
//--------------------------------------------------------------------------


#include "globals.h"
//---------------------------------------------------------------------------

//===========================================================================
// CtIDirectMusicPerformance8::CtIDirectMusicPerformance8()
//
// Default constructor
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
CtIDirectMusicPerformance8::CtIDirectMusicPerformance8(void)
{
    // initialize our member variables
	m_pUnk = NULL;
	m_dwRefCount = 1;
	m_szInterfaceName = "IDirectMusicPerformance8";
} 




//===========================================================================
// CtIDirectMusicPerformance8::~CtIDirectMusicPerformance8()
//
// Default constructor
//
// Parameters: none
//
// Returns:
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
CtIDirectMusicPerformance8::~CtIDirectMusicPerformance8(void)
{
} // *** end CtIDirectMusicPerformance8::~CtIDirectMusicPerformance8()




//===========================================================================
// CtIDirectMusicPerformance8::InitTestClass
//
// Internal harness method to initialize the test class.  Stores actual
//  IDirectMusicObject pointer for future use.
//
// Parameters:
//  IDirectMusicPortDownload *pdmPerformance - pointer to real
//                                      IDirectMusic object
//
// Returns:
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::InitTestClass(IDirectMusicPerformance8 *pdmPerformance8)
{
	// validate pointer... if bad, return error
	if(!helpIsValidPtr(pdmPerformance8, sizeof(IDirectMusicPerformance8), FALSE))
	{
        m_pUnk = NULL;
		return E_POINTER;
	}

    // call the base classes InitTestClass()
    return (CtIUnknown::InitTestClass((IUnknown*)pdmPerformance8));

} // *** end CtIDirectMusicPerformance8::InitTestClass()




//===========================================================================
// CtIDirectMusicPerformance8::GetRealObjPtr
//
// Allows us to tell who is hiding in our test class.
//
// Parameters:
//	IDirectMusicPerformance8 **ppdmPerformance8 - ptr used to return real object ptr
//
// Returns: HRESULT
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetRealObjPtr(IDirectMusicPerformance8 **ppdmPerformance8)
{

	// validate ppdm
	if(!helpIsValidPtr(ppdmPerformance8, sizeof(IDirectMusicPerformance8*), FALSE))
	{
		return E_POINTER;
	}

    // call the base classes GetRealObjPtr()
    return (CtIUnknown::GetRealObjPtr((IUnknown**)ppdmPerformance8));

} // *** end CtIDirectMusicPerformance8::GetRealObjPtr()


//===========================================================================
// CtIUnknown::AddRef
//
// Increments the reference count for our object
//
// Parameters: none
//
// Returns:
//  DWORD - new ref count
//
// History:
//	03/16/1998 - davidkl - created
//  03/26/1998 - davidkl - fixed potential faulting bug
//===========================================================================
DWORD CtIDirectMusicPerformance8::AddRef(void)
{
    DWORD dwRealObjRefCount    = 0;

    // increment our object's refcount
    InterlockedIncrement((long*)&m_dwRefCount);

    fnsIncrementIndent();

    if(m_pUnk)
    {
        // log inputs to AddRef()
        fnsLog(CALLLOGLEVEL, "--- Calling %s::AddRef()", 
                m_szInterfaceName);

        // call the real AddRef()
        dwRealObjRefCount = ((IDirectMusicPerformance*)m_pUnk)->AddRef();

        // log results from AddRef()
        fnsLog(CALLLOGLEVEL, "--- Returned from %s::AddRef()", 
                m_szInterfaceName);
        fnsLog(PARAMLOGLEVEL, "dwRefCount == %08Xh", 
                dwRealObjRefCount);
    }

    fnsDecrementIndent();

    // done
//    return m_dwRefCount;

	//LOULOU - MODIFIED FOR TEST PURPOSES
	return dwRealObjRefCount;
    
} //*** end CtIUnknown::AddRef()


//===========================================================================
// CtIUnknown::Release
//
// Decrements the reference count for our object
//
// Parameters: none
//
// Returns:
//  DWORD - new ref count
//
// History:
//	03/16/1998 - davidkl - created
//  03/26/1998 - davidkl - fixed potential faulting bug
//===========================================================================
DWORD CtIDirectMusicPerformance8::Release(void)
{
//    DWORD   dwOurRefCount       = m_dwRefCount;
    DWORD   dwRealObjRefCount   = 0;

    // decrement our object's refcount
    InterlockedDecrement((long*)&m_dwRefCount);
//    InterlockedDecrement((long*)&dwOurRefCount);

    fnsIncrementIndent();

    if(m_pUnk)
    {
        // log inputs to Release()
        fnsLog(CALLLOGLEVEL, "--- Calling %s::Release()", 
                m_szInterfaceName);

        // call the real Release()
        dwRealObjRefCount = ((IDirectMusicPerformance*)m_pUnk)->Release();

        // log results from Release()
        fnsLog(CALLLOGLEVEL, "--- Returned from %s::Release()", 
                m_szInterfaceName);
        fnsLog(PARAMLOGLEVEL, "dwRefCount == %08Xh", 
                dwRealObjRefCount);
    }

    fnsDecrementIndent();
  
    // is this the last release? if so, clean ourselves up
    if(0 == m_dwRefCount)
//    if(0 == dwOurRefCount)
    {
        delete this;
    }
    // done
	return dwRealObjRefCount;
} //*** end CtIUnknown::Release()



//===========================================================================
// CtIDirectMusicPerformance8::GetSegmentState()
//
// Encapsulates calls to GetSegmentState
//
// History:
//  12/31/1997 - a-llucar - created
//  03/04/1998 - davidkl - modified to use wrapped classes, fixed logging
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetSegmentState(
                                    CtIDirectMusicSegmentState** pptdmSegmentState,
									MUSIC_TIME mtTime)
{
    HRESULT                     hRes						= E_NOTIMPL;
    BOOL		                fValid_pptdmSegmentState    = TRUE;
    IDirectMusicSegmentState    *pdmSegSt                   = NULL;
    IDirectMusicSegmentState    **ppdmSegSt                 = NULL;

    // validate pptdmSegmentState
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegmentState pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicPerformance:PlaySegment().  Otherwise, we are to create and
    //  return a CtIDirectMusicSegmentState object
    if(!helpIsValidPtr((void*)pptdmSegmentState, 
                        sizeof(CtIDirectMusicSegmentState*), FALSE))
    {
        // bogus pointer, use as such
        fValid_pptdmSegmentState = FALSE;
        ppdmSegSt = (IDirectMusicSegmentState**)pptdmSegmentState;

    }
    else
    {
        // valid pointer, create a real object
        ppdmSegSt = &pdmSegSt;

        // just in case we fail, init test object ptr to NULL
        *pptdmSegmentState = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetSegmentState()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppSegmentState == %p   %s",
            ppdmSegSt,
            fValid_pptdmSegmentState ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "mtTime         == %08Xh",
            mtTime);
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->GetSegmentState(ppdmSegSt, mtTime);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetSegmentState()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes            == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pptdmSegmentState)
    {
        fnsLog(PARAMLOGLEVEL, "*ppSegmentState == %p",
                pdmSegSt);
    }

    // create the test object
    if(SUCCEEDED(hRes) && pptdmSegmentState)
    {
        hRes = dmthCreateTestWrappedObject(pdmSegSt, pptdmSegmentState);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
    if(fValid_pptdmSegmentState && pdmSegSt)
    {
        pdmSegSt->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPerformance8::GetSegmentState()



//===========================================================================
// CtIDirectMusicPerformance8::SendPMsg()
//
// Encapsulates calls to SendPMsg
//
// History:
//  12/31/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicPerformance8::SendPMsg(DMUS_PMSG* pPMSG)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL	fValid_pPMSG    = TRUE;

    // validate pPMSG
    if(!helpIsValidPtr((void*)pPMSG, sizeof(DMUS_PMSG), FALSE))
    {
        fValid_pPMSG = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SendPMSG()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPMSG == %p   %s",
            pPMSG,
            fValid_pPMSG ? "" : "BAD");
    if(fValid_pPMSG)
    {
        // BUGBUG log contents of pPMSG @ STRUCTLOGLEVEL
    }
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->SendPMsg(pPMSG);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SendPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::SendPMsg()

//===========================================================================
// CtIDirectMusicPerformance8::MusicToReferenceTime()
//
// Encapsulates calls to MusicToReferenceTime
//
// History:
//  12/31/1997 - a-llucar - created
//  03/04/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicPerformance8::MusicToReferenceTime(MUSIC_TIME mtTime, 
														REFERENCE_TIME* prtTime)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL		fValid_prtTime				= TRUE;

    // validate prtTime
    if(!helpIsValidPtr((void*)prtTime, sizeof(REFERENCE_TIME), FALSE))
    {
        fValid_prtTime		= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::MusicToReferenceTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "mtTime == %08Xh",
            mtTime);
    fnsLog(PARAMLOGLEVEL, "prtTime == %p   %s",
            prtTime,
            fValid_prtTime ? "" : "BAD");
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->MusicToReferenceTime(mtTime, prtTime);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::MusicToReferenceTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prtTime)
    {
        // BUGBUG log the contents of prtTime
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::MusicToReferenceTime()


//===========================================================================
// CtIDirectMusicPerformance8::ReferenceToMusicTime()
//
// Encapsulates calls to ReferenceToMusicTime
//
// History:
//  12/31/1997 - a-llucar - created
//  1/15/1998 - a-kellyc - 
//  03/04/1998 - davidkl - fixed logging
//===========================================================================
HRESULT CtIDirectMusicPerformance8::ReferenceToMusicTime(REFERENCE_TIME rtTime,
														MUSIC_TIME* pmtTime)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL		fValid_pmtTime				= TRUE;

    // validate pmtTime
    if(!helpIsValidPtr((void*)pmtTime, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtTime		= FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::ReferenceToMusicTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rtTime == %016Xh",
            rtTime);
    fnsLog(PARAMLOGLEVEL, "pmtTime == %p   %s",
            pmtTime,
            fValid_pmtTime ? "" : "BAD");
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->ReferenceToMusicTime(rtTime, pmtTime);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::ReferenceToMusicTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pmtTime)
    {
        // BUGBUG log the contents of pmtTime
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::ReferenceToMusicTime()

//===========================================================================
// CtIDirectMusicPerformance8::IsPlaying()
//
// Encapsulates calls to IsPlaying
//
// History:
//  12/31/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::IsPlaying(CtIDirectMusicSegment8* ptdmSegment,
                                        CtIDirectMusicSegmentState* ptdmSegState)
{
    HRESULT                     hRes				= E_NOTIMPL;
    BOOL		                fValid_ptdmSegState	= TRUE;
	BOOL		                fValid_ptdmSegment	= TRUE;
    IDirectMusicSegment         *pdmSeg             = NULL;
    IDirectMusicSegmentState    *pdmSegSt           = NULL;

    // validate pSegment
    if(!helpIsValidPtr((void*)ptdmSegment, sizeof(CtIDirectMusicSegment8), 
                    TRUE))
    {
        // bogus pointer, use as such
        fValid_ptdmSegment = FALSE;
        pdmSeg = (IDirectMusicSegment*)ptdmSegment;

    }
    else
    {
        // good pointer, get the real object
        if(ptdmSegment)
        {
            hRes = ptdmSegment->GetRealObjPtr(&pdmSeg);
            if(FAILED(hRes))
            {
                fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                        "object pointer (%s == %08Xh)",
                        tdmXlatHRESULT(hRes), hRes);
                goto END;
            }
        }
        else
        {
            // since NULL is ok
            pdmSeg = (IDirectMusicSegment*)ptdmSegment;
        }
    }

    // validate ptdmSegState
    if(!helpIsValidPtr((void*)ptdmSegState, 
                    sizeof(CtIDirectMusicSegmentState), TRUE))
    {
        // bogus pointer, use as such
        fValid_ptdmSegState = FALSE;
        pdmSegSt = (IDirectMusicSegmentState*)ptdmSegState;

    }
    else
    {
        // good pointer, get the real object
        if(ptdmSegState)
        {
            hRes = ptdmSegState->GetRealObjPtr(&pdmSegSt);
            if(FAILED(hRes))
            {
                fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                        "object pointer (%s == %08Xh)",
                        tdmXlatHRESULT(hRes), hRes);
                goto END;
            }
        }
        else
        {
            // since NULL is ok
            pdmSegSt = (IDirectMusicSegmentState*)ptdmSegState;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::IsPlaying()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSegment  == %p   %s",
            pdmSeg,
            fValid_ptdmSegment ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pSegState == %p   %s",
            pdmSegSt,
            fValid_ptdmSegState ? TEXT("") : TEXT("BAD"));
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->IsPlaying(pdmSeg, pdmSegSt);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::IsPlaying()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_ptdmSegment && pdmSeg)
    {
        pdmSeg->Release();
    }
    if(fValid_ptdmSegState && pdmSegSt)
    {
        pdmSegSt->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPerformance8::IsPlaying()



//===========================================================================
// CtIDirectMusicPerformance8::GetTime()
//
// Encapsulates calls to GetTime
//
// History:
//  12/31/1997 - a-llucar - created
//  1/15/1998 - a-kellyc - edited
//  03/04/1998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetTime(REFERENCE_TIME* prtNow, 
                                        MUSIC_TIME* pmtNow)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL	fValid_prtNow	= TRUE;
	BOOL	fValid_pmtNow	= TRUE;

    // validate prtNow
    if(!helpIsValidPtr((void*)prtNow, sizeof(REFERENCE_TIME), TRUE))
    {
        fValid_prtNow = FALSE;
    }

    // validate pmtNow
    if(!helpIsValidPtr((void*)pmtNow, sizeof(MUSIC_TIME), TRUE))
    {
        fValid_pmtNow = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "prtNow == %p   %s",
            prtNow,
            fValid_prtNow ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pmtNow == %p   %s",
            pmtNow,
            fValid_pmtNow ? "" : "BAD");
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->GetTime(prtNow, pmtNow);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetTime()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes    == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_prtNow && (NULL != prtNow))
    {
        fnsLog(PARAMLOGLEVEL, "*prtNow == %016Xh",
                *prtNow);
    }
    if(fValid_pmtNow && (NULL != pmtNow))
    {
        fnsLog(PARAMLOGLEVEL, "*pmtNow == %08Xh",
                *pmtNow);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::GetTime()

//===========================================================================
// CtIDirectMusicPerformance8::AllocPMsg()
//
// Encapsulates calls to AllocPMsg
//
// History:
//  12/31/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed, fixed logging
//  03/31/1998 - davidkl - fixed cb logging bug
//===========================================================================
HRESULT CtIDirectMusicPerformance8::AllocPMsg(ULONG cb, DMUS_PMSG** ppPMSG)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL		fValid_ppPMSG				= TRUE;

    // validate ppEvent
    if(!helpIsValidPtr((void*)ppPMSG, sizeof(DMUS_PMSG*), FALSE))
    {
        fValid_ppPMSG = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::AllocPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "cb     == %08Xh",
            cb);
    fnsLog(PARAMLOGLEVEL, "ppPMSG == %p   %s",
            ppPMSG,
            fValid_ppPMSG ? "" : "BAD");
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->AllocPMsg(cb, ppPMSG);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::AllocPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppPMSG)
    {
        // BUGBUG - log contents of ppPMSG
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::AllocPMsg()


//===========================================================================
// CtIDirectMusicPerformance8::FreePMsg()
//
// Encapsulates calls to FreePMsg
//
// History:
//  12/31/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicPerformance8::FreePMsg(DMUS_PMSG* pPMSG)
{
    HRESULT hRes			= E_NOTIMPL;
    BOOL	fValid_pPMSG    = TRUE;

    // validate pEvent
    if(!helpIsValidPtr((void*)pPMSG, sizeof(DMUS_PMSG), FALSE))
    {
        fValid_pPMSG = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::FreePMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pPMSG == %p   %s",
            pPMSG,
            fValid_pPMSG ? "" : "BAD");
    if(fValid_pPMSG)
    {
        // BUGBUG log contents of pPMSG
    }
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->FreePMsg(pPMSG);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::FreePMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::FreePMsg()

//===========================================================================
// CtIDirectMusicPerformance8::GetNotificationPMsg()
//
// Encapsulates calls to GetNotificationPMsg
//
// History:
//  12/31/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed, fixed logging
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetNotificationPMsg(
                                        DMUS_NOTIFICATION_PMSG** ppNotificationPMsg)
{
    HRESULT hRes				        = E_NOTIMPL;
    BOOL	fValid_ppNotificationPMsg	= TRUE;

    // validate ppNotifyEvent
    if(!helpIsValidPtr((void*)ppNotificationPMsg, sizeof(DMUS_PMSG**),
                        FALSE))
    {
        fValid_ppNotificationPMsg = FALSE;
    }
   
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetNotificationPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppNotificationPMsg == %p   %s",
            ppNotificationPMsg,
            fValid_ppNotificationPMsg ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->GetNotificationPMsg(ppNotificationPMsg);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetNotificationPMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppNotificationPMsg)
    {
        // BUGBUG log contents of ppNotificationPMsg
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::GetNotificationPMsg()


//===========================================================================
// CtIDirectMusicPerformance8::AddNotificationType()
//
// Encapsulates calls to AddNotificationType
//
// History:
//  12/31/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed, fixed logging
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicPerformance8::AddNotificationType(REFGUID rguidNotificationType)
{
    HRESULT hRes						    = E_NOTIMPL;
    char    szGuid[MAX_LOGSTRING];

    dmthGUIDtoString(rguidNotificationType, szGuid);
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::AddNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidNotificationType == %s (%s)",
            szGuid, dmthXlatGUID(rguidNotificationType));

    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->AddNotificationType(rguidNotificationType);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::AddNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::AddNotificationType()


//===========================================================================
// CtIDirectMusicPerformance8::RemoveNotificationType()
//
// Encapsulates calls to RemoveNotificationType
//
// History:
//  12/31/1997 - a-llucar - created
//  03/04/1998 - davidkl - renamed, fixed logging
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicPerformance8::RemoveNotificationType(
                                                REFGUID rguidNotificationType)
{
    HRESULT hRes						    = E_NOTIMPL;
    char    szGuid[MAX_LOGSTRING];

    dmthGUIDtoString(rguidNotificationType, szGuid);
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::RemoveNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidNotificationType == %s (%s)",
            szGuid, dmthXlatGUID(rguidNotificationType));

    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->RemoveNotificationType(rguidNotificationType);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::RemoveNotificationType()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::RemoveNotificationType()


//===========================================================================
// CtIDirectMusicPerformance8::GetGlobalParam()
//
// Encapsulates calls to GetGlobalParam
//
// History:
//  01/10/1998 - a-llucar - created
//	01/15/1998 - markburt - modified ppData to pData
//  03/05/1998 - davidkl - renamed, fixed logging
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetGlobalParam(REFGUID rguidType, 
                                                void* pData, DWORD dwSize)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL    fValid_pData        = TRUE;
    char    szGuid[MAX_LOGSTRING];


    // validate pData
    if(!helpIsValidPtr((void*)pData, sizeof(BYTE) * dwSize, FALSE))
    {
        fValid_pData = FALSE;
    }
    
    dmthGUIDtoString(rguidType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetGlobalParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType  == %s (%s)",
            szGuid, dmthXlatGUID(rguidType));
    fnsLog(PARAMLOGLEVEL, "pData      == %p   %s",
			pData,
            fValid_pData ? "" : "BAD");
	fnsLog(PARAMLOGLEVEL, "dwSize     == %08Xh",
			dwSize);
     
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->GetGlobalParam(rguidType, pData, dwSize);

    // log results
    fnsLog(CALLLOGLEVEL, "--- Returned from %s::GetGlobalParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pData)
    {
        // BUBUG log contents of pData @ MAXLOGLEVEL
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::GetGlobalParam()


//===========================================================================
// CtIDirectMusicPerformance8::SetGlobalParam()
//
// Encapsulates calls to SetGlobalParam
//
// History:
//  01/10/1998 - a-llucar - created
//  03/05/1998 - davidkl - renamed, fixed logging
//  04/13/1997 - davidkl - changed GUID* to REFGUID
//===========================================================================
HRESULT CtIDirectMusicPerformance8::SetGlobalParam(REFGUID rguidType, 
                                                void* pData, DWORD dwSize)
{
    HRESULT hRes				= E_NOTIMPL;
    BOOL	fValid_pGuidType	= TRUE;
    BOOL    fValid_pData        = TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pData
    if(!helpIsValidPtr((void*)pData, sizeof(BYTE) * dwSize, FALSE))
    {
        fValid_pData = FALSE;
    }
    
    dmthGUIDtoString(rguidType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SetGlobalParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType == %s (%s)",
            szGuid, dmthXlatGUID(rguidType));
    fnsLog(PARAMLOGLEVEL, "pData      == %p   %s",
			pData,
            fValid_pData ? "" : "BAD");
    if(fValid_pData)
    {
        // BUGBUG - log contents of pData @ MAXLOGLEVEL
    }
	fnsLog(PARAMLOGLEVEL, "dwSize     == %08Xh",
			dwSize);
     
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->SetGlobalParam(rguidType, pData, dwSize);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetGlobalParam()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::SetGlobal()


//===========================================================================
// CtIDirectMusicPerformance8::InitAudioX()
//
// Encapsulates calls to InitAudioX
//
// History:
//  01/16/2001 - danhaff - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::InitAudioX(
	DWORD dwDefaultPathType,                 // Requested default audio path type, also optional.
	DWORD dwPChannelCount,
    DWORD dwVoiceCount,               //Number of Voices (DSoundBuffers) allocated by synth.
    DWORD dwFlags)                   // Number of PChannels, if default audio path to be created.
{
    HRESULT			hRes					= E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::InitAudioX()",
            m_szInterfaceName);
    
    fnsLog(PARAMLOGLEVEL, "dwDefaultPathType == %08Xh (%s)",
            dwDefaultPathType, dmthXlatDMUS_APATH(dwDefaultPathType));
    fnsLog(PARAMLOGLEVEL, "dwPChannelCount   == %08Xh",
            dwPChannelCount);
    fnsLog(PARAMLOGLEVEL, "dwVoiceCount   == %08Xh",
            dwVoiceCount);
    fnsLog(PARAMLOGLEVEL, "dwFlags == %08Xh",
            dwFlags);

		// call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->InitAudioX(dwDefaultPathType,dwPChannelCount, dwVoiceCount, dwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::InitAudioX()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes           == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    return hRes;

} // *** end CtIDirectMusicPerformance8::InitAudioX()



//===========================================================================
// CtIDirectMusicPerformance8::PlaySegmentEx()
//
// Encapsulates calls to PlaySegmentEx
//
// History:
//  10/13/1999 - kcraven - created
//  01/17/2000 - kcraven - updated
//  02/02/2000 - kcraven - fixed logging string
//===========================================================================
HRESULT CtIDirectMusicPerformance8::PlaySegmentEx(CtIUnknown* pSource,                  // Segment to play. Alternately, could be an IDirectMusicSong.
                                           CHAR *pzSegmentName,                       // If song, which segment in the song.
                                           CtIUnknown* pTransition,                     // Optional template segment to compose transition with.
                                           DWORD dwFlags,                               // DMUS_SEGF_ flags.
                                           __int64 i64StartTime,                        // Time to start playback.
                                           CtIDirectMusicSegmentState8** ppSegmentState, // Returned Segment State.
                                           CtIUnknown *pFrom,                           // Optional segmentstate or audiopath to replace.
                                           CtIUnknown *pAudioPath)                      // Optional audioPath to play on.
//HRESULT CtIDirectMusicPerformance8::PlaySegmentEx
//(
//	CtIUnknown* pSource,
//	DWORD dwSegmentID,
//	CtIUnknown* pTransition,						// Optional
//	DWORD dwFlags, 
//	__int64 i64StartTime,
//	CtIDirectMusicSegmentState** ppSegmentState,	// Optional
//	CtIUnknown *pFrom,								// Optional
//	CtIUnknown *pAudioPath							// Optional
//)
{
    HRESULT                     hRes					= E_NOTIMPL;
    BOOL		                fValid_pSource			= TRUE;
    BOOL		                fValid_pTransition		= TRUE;
    BOOL		                fValid_ppSegmentState	= TRUE;
    BOOL		                fValid_pFrom			= TRUE;
    BOOL		                fValid_pAudioPath		= TRUE;
    IUnknown*					pdmSource				= NULL;
    IUnknown*					pdmTransition			= NULL;
    IDirectMusicSegmentState*	pdmSegSt                = NULL;
    IDirectMusicSegmentState**	ppdmSegSt				= NULL;
    IUnknown*					pdmFrom					= NULL;
	IUnknown*					pdmAudioPath			= NULL;

    // validate pSource
    if(!helpIsValidPtr((void*)pSource, sizeof(CtIUnknown),
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pSource = FALSE;
        pdmSource = (IUnknown*)pSource;

    }
    else
    {
        // good pointer, get the real object
        hRes = pSource->GetRealObjPtr(&pdmSource);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate pTransition
    if(!helpIsValidPtr((void*)pTransition, sizeof(CtIUnknown),
                    FALSE))
    {
        // bogus pointer, use as such
        if(pTransition != NULL)
		{
			fValid_pTransition = FALSE;
		}
        pdmTransition = (IUnknown*)pTransition;

    }
    else
    {
        // good pointer, get the real object
        hRes = pTransition->GetRealObjPtr(&pdmTransition);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ppSegmentState
    // NOTE:
    //	If we are passed an invalid CtIDirectMusicSegmentState pointer, we assume
    //  that we are to perform an invalid parameter test on
    //  IDirectMusicPerformance:PlaySegment().  Otherwise, we are to create and
    //  return a CtIDirectMusicSegmentState object
    // NOTE:
    //  NULL is valid here... be careful!
    if(!helpIsValidPtr((void*)ppSegmentState,
                        sizeof(CtIDirectMusicSegmentState*), TRUE))
    {
        // bogus pointer, use as such
        if(ppSegmentState != NULL)
		{
	        fValid_ppSegmentState = FALSE;
		}
        ppdmSegSt = (IDirectMusicSegmentState**)ppSegmentState;

    }
    else
    {
        if(ppSegmentState)
        {
            // valid pointer, create a real object
            ppdmSegSt = &pdmSegSt;

            // just in case we fail, init test object ptr to NULL
            *ppSegmentState = NULL;
        }

    }

     // validate pFrom
    if(!helpIsValidPtr((void*)pFrom, sizeof(CtIUnknown),
                    FALSE))
    {
        // bogus pointer, use as such
        if(pFrom != NULL)
		{
	        fValid_pFrom = FALSE;
		}
        pdmFrom = (IUnknown*)pFrom;

    }
    else
    {
        // good pointer, get the real object
        hRes = pFrom->GetRealObjPtr(&pdmFrom);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

   // validate pdmAudioPath
    if(!helpIsValidPtr((void*)pAudioPath, sizeof(CtIUnknown),
                    FALSE))
    {
        // bogus pointer, use as such
        if(pAudioPath != NULL)
		{
	        fValid_pAudioPath = FALSE;
		}
        pdmAudioPath = (IUnknown*)pAudioPath;

    }
    else
    {
        // good pointer, get the real object
        hRes = pAudioPath->GetRealObjPtr(&pdmAudioPath);
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
    fnsLog(CALLLOGLEVEL, "---- Calling %s::PlaySegmentEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSource        == %p   %s",
            pdmSource,
            fValid_pSource ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pzSegmentName == %p",
            pzSegmentName);
    fnsLog(PARAMLOGLEVEL, "pTransition    == %08Xh   %s",
            pdmTransition,
            fValid_pTransition ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "dwFlags        == %08Xh",
            dwFlags);
    fnsLog(PARAMLOGLEVEL, "i64StartTime   == %016Xh", //BUGBUG - can't print 64 bit numbers in fnshell
            i64StartTime);
    fnsLog(PARAMLOGLEVEL, "ppSegmentState == %p   %s",
            ppdmSegSt,
            fValid_ppSegmentState ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pFrom          == %p   %s",
            pdmFrom,
            fValid_pFrom ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pAudioPath     == %p   %s",
            pdmAudioPath,
            fValid_pAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->PlaySegmentEx(
				pdmSource,
				pzSegmentName,
				pdmTransition,
				dwFlags,
				i64StartTime,
				ppdmSegSt,
				pdmFrom,
				pdmAudioPath
				);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::PlaySegmentEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes            == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppSegmentState && ppSegmentState)
    {
        fnsLog(PARAMLOGLEVEL, "*ppSegmentState == %p",
                pdmSegSt);
    }

    // wrap the segment state so we don't return the real object
    if(SUCCEEDED(hRes) && ppSegmentState)
    {
        hRes = dmthCreateTestWrappedObject(pdmSegSt, ppSegmentState);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", hRes);
        }
    }

    fnsDecrementIndent();

    // done
END:
    if(fValid_pSource && pdmSource)
    {
        pdmSource->Release();
    }
    if(fValid_pTransition && pdmTransition)
    {
        pdmTransition->Release();
    }
    if(fValid_ppSegmentState && pdmSegSt)
    {
        pdmSegSt->Release();
    }
    if(fValid_pFrom && pdmFrom)
    {
        pdmFrom->Release();
    }
    if(fValid_pAudioPath && pdmAudioPath)
    {
        pdmAudioPath->Release();
    }
    return hRes;

} //*** end CtIDirectMusicPerformance8::PlaySegmentEx()




//===========================================================================
// CtIDirectMusicPerformance8::StopEx()
//
// Encapsulates calls to StopEx
//
// History:
//  01/17/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::StopEx
(
	CtIUnknown *pObjectToStop,
	__int64 i64StopTime,
	DWORD dwFlags
)
{
    HRESULT     hRes					= E_NOTIMPL;
    BOOL		fValid_pObjectToStop	= TRUE;
	IUnknown 	*pdmObjectToStop 		= NULL;

    // validate pObjectToStop
    if(!helpIsValidPtr((void*)pObjectToStop, sizeof(CtIUnknown), FALSE))
    {
        fValid_pObjectToStop = FALSE;
        pdmObjectToStop = (IUnknown*)pObjectToStop;
    }
    else
    {
        // good pointer, get the real object
        hRes = pObjectToStop->GetRealObjPtr(&pdmObjectToStop);
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
    fnsLog(CALLLOGLEVEL, "---- Calling %s::StopEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pObjectToStop == %p   %s",
            pdmObjectToStop,
            fValid_pObjectToStop ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "i64StopTime   == %016Xh", //BUGBUG - can't print 64 bit numbers in fnshell
            i64StopTime);
    fnsLog(PARAMLOGLEVEL, "dwFlags       == %08Xh",
            dwFlags);

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->StopEx(pdmObjectToStop,i64StopTime,dwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::StopEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
 END:
    if(fValid_pObjectToStop && pdmObjectToStop)
    {
        pdmObjectToStop->Release();
    }
   return hRes;

} // *** end CtIDirectMusicPerformance8::StopEx()




//===========================================================================
// CtIDirectMusicPerformance8::ClonePMsg()
//
// Encapsulates calls to ClonePMsg
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::ClonePMsg
(
	DMUS_PMSG* pSourcePMSG,
	DMUS_PMSG** ppCopyPMSG
)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL		fValid_pSourcePMSG				= TRUE;
    BOOL		fValid_ppCopyPMSG				= TRUE;

    // validate pSourcePMSG
    if(!helpIsValidPtr((void*)pSourcePMSG, sizeof(DMUS_PMSG*), FALSE))
    {
        fValid_pSourcePMSG = FALSE;
    }

    // validate ppCopyPMSG
    if(!helpIsValidPtr((void*)ppCopyPMSG, sizeof(DMUS_PMSG*), FALSE))
    {
        fValid_ppCopyPMSG = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::ClonePMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSourcePMSG == %p   %s",
            pSourcePMSG,
            fValid_pSourcePMSG ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppCopyPMSG  == %p   %s",
            ppCopyPMSG,
            fValid_ppCopyPMSG ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->ClonePMsg(pSourcePMSG,ppCopyPMSG);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::ClonePMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicPerformance8::ClonePMsg()


//===========================================================================
// CtIDirectMusicPerformance8::CreateAudioPath()
//
// Encapsulates calls to CreateAudioPath
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::CreateAudioPath(
	CtIUnknown *pSourceConfig, 
	BOOL fActivate,
	CtIDirectMusicAudioPath **ppNewPath)
{
    HRESULT					hRes					= E_NOTIMPL;
    BOOL					fValid_pSourceConfig	= TRUE;
    BOOL					fValid_ppNewPath		= TRUE;
	IUnknown*				pdmSourceConfig 		= NULL;
    IDirectMusicAudioPath*	pdmNewPath				= NULL;
    IDirectMusicAudioPath**	ppdmNewPath				= NULL;


    // validate pSourceConfig
    if(!helpIsValidPtr((void*)pSourceConfig, sizeof(CtIUnknown), FALSE))
    {
        fValid_pSourceConfig = FALSE;
        pdmSourceConfig = (IUnknown*)pSourceConfig;
    }
    else
    {
        // good pointer, get the real object
        hRes = pSourceConfig->GetRealObjPtr(&pdmSourceConfig);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ppNewPath
    if(!helpIsValidPtr((void*)ppNewPath, sizeof(CtIDirectMusicAudioPath*),
                        FALSE))
    {
        // bogus pointer, use as such
        fValid_ppNewPath = FALSE;
        ppdmNewPath = (IDirectMusicAudioPath**)ppNewPath;

    }
    else
    {
        // valid pointer, create a real object
        ppdmNewPath = &pdmNewPath;

        // just in case we fail, init test object ptr to NULL
        *ppNewPath = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::CreateAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSourceConfig == %p   %s",
            pdmSourceConfig,
            fValid_pSourceConfig ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "fActivate     == %d",
            fActivate);
    fnsLog(PARAMLOGLEVEL, "ppNewPath     == %p   %s",
            ppdmNewPath,
            fValid_ppNewPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->CreateAudioPath(pdmSourceConfig,fActivate,ppdmNewPath);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::CreateAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_ppNewPath)
    {
        fnsLog(PARAMLOGLEVEL, "*ppNewPath == %p",
                pdmNewPath);
		// create the test object
		if(SUCCEEDED(hRes) && pdmNewPath)
		{
			hRes = dmthCreateTestWrappedObject(pdmNewPath, ppNewPath);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", 
						hRes);
			}
		}
	}

    fnsDecrementIndent();

END:
    // done
    if(fValid_ppNewPath && pdmNewPath)
    {
        pdmNewPath->Release();
    }

    //Added 7/13/00 - Danhaff
    if (fValid_pSourceConfig && pdmSourceConfig)
    {
        pdmSourceConfig->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::CreateAudioPath()




//===========================================================================
// CtIDirectMusicPerformance8::CreateStandardAudioPath()
//
// Encapsulates calls to CreateStandardAudioPath
//
// History:
//  01/17/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::CreateStandardAudioPath
(
	DWORD dwType,
	DWORD dwPChannelCount,
	BOOL fActivate,
	CtIDirectMusicAudioPath **ppNewPath
)
{
    HRESULT			hRes				= E_NOTIMPL;
    BOOL			fValid_ppNewPath	= TRUE;
    IDirectMusicAudioPath	*pdmPath	= NULL;
    IDirectMusicAudioPath	**ppdmPath	= NULL;

    // validate pptdmPort
    if(!helpIsValidPtr((void*)ppNewPath, sizeof(CtIDirectMusicAudioPath*),
                        FALSE))
    {
        // bogus pointer, use as such
        fValid_ppNewPath = FALSE;
        ppdmPath = (IDirectMusicAudioPath**)ppNewPath;

    }
    else
    {
        // valid pointer, create a real object
        ppdmPath = &pdmPath;

        // just in case we fail, init test object ptr to NULL
        *ppNewPath = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::CreateStandardAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwType          == %08Xh (%s)",
            dwType,
            dmthXlatDMUS_APATH(dwType));
    fnsLog(PARAMLOGLEVEL, "dwPChannelCount == %08Xh",
            dwPChannelCount);
    fnsLog(PARAMLOGLEVEL, "fActivate == %d",
            fActivate);
    fnsLog(PARAMLOGLEVEL, "ppNewPath       == %p   %s",
            ppdmPath,
            fValid_ppNewPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->CreateStandardAudioPath(dwType,dwPChannelCount,fActivate,ppdmPath);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::CreateStandardAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_ppNewPath)
    {
        fnsLog(PARAMLOGLEVEL, "*ppNewPath == %p",
                pdmPath);
		// create the test object
		if(SUCCEEDED(hRes) && pdmPath)
		{
			hRes = dmthCreateTestWrappedObject(pdmPath, ppNewPath);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", 
						hRes);
			}
		}
	}

    fnsDecrementIndent();

    // done
    if(fValid_ppNewPath && pdmPath)
    {
        pdmPath->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::CreateStandardAudioPath()




//===========================================================================
// CtIDirectMusicPerformance8::SetDefaultAudioPath()
//
// Encapsulates calls to SetDefaultAudioPath
//
// History:
//  01/17/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::SetDefaultAudioPath
(
	CtIDirectMusicAudioPath *pAudioPath
)
{
    HRESULT		hRes				= E_NOTIMPL;
    BOOL		fValid_pAudioPath	= TRUE;
    IDirectMusicAudioPath*	pdmPath	= NULL;

    // validate pTemplate
    if(!helpIsValidPtr((void*)pAudioPath, sizeof(CtIDirectMusicAudioPath),
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pAudioPath = FALSE;
        pdmPath = (IDirectMusicAudioPath*)pAudioPath;

    }
    else
    {
        // good pointer, get the real object
        hRes = pAudioPath->GetRealObjPtr(&pdmPath);
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
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SetDefaultAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pAudioPath       == %p   %s",
            pdmPath,
            fValid_pAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->SetDefaultAudioPath(pdmPath);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetDefaultAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();
END:
    // done
    if(fValid_pAudioPath && pdmPath)
    {
        pdmPath->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::SetDefaultAudioPath()



//===========================================================================
// CtIDirectMusicPerformance8::GetDefaultAudioPath()
//
// Encapsulates calls to GetDefaultAudioPath
//
// History:
//  01/17/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetDefaultAudioPath
(
	CtIDirectMusicAudioPath **ppAudioPath
)
{
    HRESULT			hRes				= E_NOTIMPL;
    BOOL			fValid_ppAudioPath	= TRUE;
    IDirectMusicAudioPath	*pdmPath	= NULL;
    IDirectMusicAudioPath	**ppdmPath	= NULL;

    // validate pptdmPort
    if(!helpIsValidPtr((void*)ppAudioPath, sizeof(CtIDirectMusicAudioPath*),
                        FALSE))
    {
        // bogus pointer, use as such
        fValid_ppAudioPath= FALSE;
        ppdmPath = (IDirectMusicAudioPath**)ppAudioPath;

    }
    else
    {
        // valid pointer, create a real object
        ppdmPath = &pdmPath;

        // just in case we fail, init test object ptr to NULL
        *ppAudioPath = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetDefaultAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppAudioPath       == %p   %s",
            ppdmPath,
            fValid_ppAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->GetDefaultAudioPath(ppdmPath);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetDefaultAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_ppAudioPath)
    {
        fnsLog(PARAMLOGLEVEL, "*ppAudioPath == %08Xh",
                pdmPath);
		// create the test object
		if(SUCCEEDED(hRes) && pdmPath)
		{
			hRes = dmthCreateTestWrappedObject(pdmPath, ppAudioPath);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", 
						hRes);
			}
		}
	}

    fnsDecrementIndent();

    // done
    if(fValid_ppAudioPath && pdmPath)
    {
        pdmPath->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::GetDefaultAudioPath()




//===========================================================================
// CtIDirectMusicPerformance8::SetParamHook()
//
// Encapsulates calls to SetParamHook
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicPerformance8::SetParamHook(CtIDirectMusicParamHook* pIHook)
{
    HRESULT             hRes				= E_NOTIMPL;
    BOOL		        fValid_pIHook    = TRUE;
    IDirectMusicParamHook   *pdmIHook           = NULL;

    // validate pSegment
    if(!helpIsValidPtr((void*)pIHook, sizeof(CtIDirectMusicGraph),
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pIHook = FALSE;
        pdmIHook = (IDirectMusicParamHook*)pIHook;

    }
    else
    {
        // good pointer, get the real object
        hRes = pIHook->GetRealObjPtr(&pdmIHook);
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
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SetParamHook()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pGraph == %p   %s",
            pdmIHook,
            fValid_pIHook ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->SetParamHook(pdmIHook);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetParamHook()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pIHook && pdmIHook)
    {
        pdmIHook->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::SetParamHook()
*/
//===========================================================================
// CtIDirectMusicPerformance8::GetParamEx()
//
// Encapsulates calls to GetParamEx
//
// History:
//  04/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetParamEx(
	REFGUID rguidType, 
	DWORD dwTrackID,
	DWORD dwGroupBits, 
	DWORD dwIndex, 
	MUSIC_TIME mtTime, 
	MUSIC_TIME* pmtNext, 
	void* pData)
{
    HRESULT hRes				= E_NOTIMPL;
	BOOL	fValid_pmtNext		= TRUE;
	BOOL	fValid_pData		= TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pmtNext
    if(!helpIsValidPtr((void*)pmtNext, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtNext = FALSE;
    }
    
    // validate pData
    if(!helpIsValidPtr((void*)pData, 1, FALSE))
    {
        fValid_pData = FALSE;
    }

    dmthGUIDtoString(rguidType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetParamEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType    == %s (%s)",
			szGuid, dmthXlatGUID(rguidType));
    fnsLog(PARAMLOGLEVEL, "dwTrackID    == %08Xh",
            dwTrackID);
    fnsLog(PARAMLOGLEVEL, "dwGroupBits  == %08Xh",
            dwGroupBits);
    fnsLog(PARAMLOGLEVEL, "dwIndex      == %08Xh",
            dwIndex);
	fnsLog(PARAMLOGLEVEL, "mtTime       == %08Xh",
            mtTime);
	fnsLog(PARAMLOGLEVEL, "pmtNext      == %p   %s",
            pmtNext,
            fValid_pmtNext ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pData        == %p   %s",
            pData,
            fValid_pData ? "" : "BAD");
     
    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->GetParamEx(
		rguidType, 
		dwTrackID,
		dwGroupBits, 
		dwIndex, 
		mtTime, 
		pmtNext, 
		pData);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetParamEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pmtNext)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtNext   == %08Xh",
                *pmtNext);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::GetParamEx()




//===========================================================================
// CtIDirectMusicPerformance::CloseDown()
//
// Encapsulates calls to CloseDown
//
// History:
//  03/26/1998 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicPerformance::CloseDown(void)
{
    HRESULT hRes = E_NOTIMPL;

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::CloseDown()",
            m_szInterfaceName);
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->CloseDown();

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::CloseDown()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();


// done
    return hRes;

} //*** end CtIDirectMusicPerformance::CloseDown()


//===========================================================================
// CtIDirectMusicPerformance::Invalidate()
//
// Encapsulates calls to Invalidate
//
// History:
//  12/31/1997 - a-llucar - created
//===========================================================================
HRESULT CtIDirectMusicPerformance::Invalidate(MUSIC_TIME mtTime, DWORD dwFlags)
{
    HRESULT     hRes						= E_NOTIMPL;
    
    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::Invalidate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "mtTime == %08Xh",
            mtTime);
    fnsLog(PARAMLOGLEVEL, "dwFlags == %08Xh",
            dwFlags);
    
    // call the real function
    hRes = ((IDirectMusicPerformance*)m_pUnk)->Invalidate(mtTime, dwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::Invalidate()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance::Invalidate()
